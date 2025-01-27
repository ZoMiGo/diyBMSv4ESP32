/*------------------------------------------------------------------------
 *
 *   Project: PYLON TECH BATTERY Emulator for DiyBMS by Stuart Pittaway
 *            Using Canbus @ 500kbps and 11 bit addresses.
 *
 *   Author:  Trajilovic Goran (ZoMiGo)
 *   Original Quelcode: Stuart Pittaway
 *
 * -----------------------------------------------------------------------
 */


#define USE_ESP_IDF_LOG 1
static constexpr const char *const TAG = "diybms-pylon";

#include "pylon_canbus.h"
#include <EEPROM.h>
#include "HAL_ESP32.h"

bool isMaster = false; // Global variable to determine if this module is the master

void setupPylonCanBus() {
    ESP_LOGI(TAG, "Initializing Pylon CAN Bus");
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(RS485_RX, RS485_TX, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "CAN Driver installed successfully");
    } else {
        ESP_LOGE(TAG, "Failed to install CAN Driver");
    }
}

void setupModuleRole() {
    uint8_t moduleID = EEPROM.read(0); // Read ID from EEPROM

    if (moduleID == 1) { // Master Module
        isMaster = true;
        ESP_LOGI(TAG, "This module is the Master (ID: %d)", moduleID);
    } else { // Slave Module
        isMaster = false;
        ESP_LOGI(TAG, "This module is a Slave (ID: %d)", moduleID);
    }
}

// Master-specific functionality
void requestDataFromSlaves() {
    ESP_LOGI(TAG, "Requesting data from all slaves...");

    for (int i = 0; i < 10; i++) {
        uint8_t slaveID = EEPROM.read(i);
        if (slaveID != 0) {
            ESP_LOGI(TAG, "Requesting data from Slave ID: %d", slaveID);
            twai_message_t message;
            message.identifier = 0x100 + slaveID;
            message.data_length_code = 8;
            memset(message.data, 0, 8);
            message.data[0] = slaveID;

            if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
                ESP_LOGI(TAG, "Request sent to Slave ID %d", slaveID);
            } else {
                ESP_LOGW(TAG, "Failed to send request to Slave ID %d", slaveID);
            }
        }
    }
}

void receiveDataFromSlaves() {
    twai_message_t message;
    while (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        uint8_t slaveID = message.identifier - 0x200; // Extract Slave ID
        uint16_t voltage = (message.data[0] << 8) + message.data[1];
        int8_t temperature = message.data[2];

        ESP_LOGI(TAG, "Received data from Slave %d: Voltage=%d, Temp=%d", slaveID, voltage, temperature);

        // Process or store received data
    }
}

// Slave-specific functionality
void sendDataToMaster() {
    twai_message_t message;
    message.identifier = 0x200 + EEPROM.read(0); // Slave ID-based Identifier
    message.data_length_code = 8;
    memset(message.data, 0, 8);

    // Example: Sending Voltage and Temperature
    message.data[0] = rules.highestBankVoltage / 256;  // High Byte of Voltage
    message.data[1] = rules.highestBankVoltage % 256;  // Low Byte of Voltage
    message.data[2] = rules.highestExternalTemp;       // Temperature

    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        ESP_LOGI(TAG, "Data sent to Master");
    } else {
        ESP_LOGW(TAG, "Failed to send data to Master");
    }
}

// Pylon-specific messages
void pylon_message_351() {
    struct data351 {
        uint16_t battery_charge_voltage;
        int16_t battery_charge_current_limit;
        int16_t battery_discharge_current_limit;
        uint16_t battery_discharge_voltage;
    };

    data351 data;
    data.battery_discharge_voltage = mysettings.dischargevolt;

    uint16_t default_charge_voltage = 1;
    int16_t default_charge_current_limit = 1;
    int16_t default_discharge_current_limit = 1;

    if (mysettings.canbusinverter == CanBusInverter::INVERTER_DEYE) {
        default_charge_voltage = rules.lowestBankVoltage / 100;
        default_charge_current_limit = 0;
        default_discharge_current_limit = 0;
    }

    data.battery_charge_voltage = default_charge_voltage;
    data.battery_charge_current_limit = default_charge_current_limit;
    data.battery_discharge_current_limit = default_discharge_current_limit;

    if (rules.IsChargeAllowed(&mysettings)) {
        if (rules.numberOfBalancingModules > 0 && mysettings.stopchargebalance) {
        } else {
            data.battery_charge_voltage = rules.DynamicChargeVoltage();
            data.battery_charge_current_limit = rules.DynamicChargeCurrent();
        }
    }

    if (rules.IsDischargeAllowed(&mysettings)) {
        data.battery_discharge_current_limit = mysettings.dischargecurrent;
    }

    send_canbus_message(0x351, (uint8_t *)&data, sizeof(data351));
}

void pylon_message_355() {
    if (_controller_state != ControllerState::Running) return;

    struct data355 {
        uint16_t stateofchargevalue;
        uint16_t stateofhealthvalue;
    };

    if (mysettings.currentMonitoringEnabled && currentMonitor.validReadings) {
        data355 data;
        data.stateofchargevalue = rules.StateOfChargeWithRulesApplied(&mysettings, currentMonitor.stateofcharge);
        data.stateofhealthvalue = (uint16_t)(trunc(mysettings.soh_percent));

        send_canbus_message(0x355, (uint8_t *)&data, sizeof(data355));
    }
}

void pylon_message_359() {
    struct data359 {
        uint8_t byte0;
        uint8_t byte1;
        uint8_t byte2;
        uint8_t byte3;
        uint8_t byte4;
        uint8_t byte5;
        uint8_t byte6;
        uint8_t byte7;
    };

    data359 data;
    memset(&data, 0, sizeof(data359));

    if (_controller_state == ControllerState::Running) {
        data.byte0 |= ((rules.ruleOutcome(Rule::BankOverVoltage) || rules.ruleOutcome(Rule::CurrentMonitorOverVoltage)) ? B00000010 : 0);
        data.byte0 |= ((rules.ruleOutcome(Rule::BankUnderVoltage) || rules.ruleOutcome(Rule::CurrentMonitorUnderVoltage)) ? B00000100 : 0);

        if (rules.moduleHasExternalTempSensor) {
            data.byte0 |= (rules.ruleOutcome(Rule::ModuleOverTemperatureExternal) ? B00001000 : 0);
            data.byte0 |= (rules.ruleOutcome(Rule::ModuleUnderTemperatureExternal) ? B00010000 : 0);
        }

        data.byte2 = 0;
        if (rules.highestBankVoltage / 100 > mysettings.chargevolt) {
            data.byte2 |= B00000010;
        }
        if (rules.lowestBankVoltage / 100 < mysettings.dischargevolt) {
            data.byte2 |= B00000100;
        }
        if (rules.moduleHasExternalTempSensor && rules.highestExternalTemp > mysettings.chargetemphigh) {
            data.byte2 |= B00001000;
        }
        if (rules.moduleHasExternalTempSensor && rules.lowestExternalTemp < mysettings.chargetemplow) {
            data.byte2 |= B00010000;
        }
    }

    data.byte3 |= ((_controller_state != ControllerState::Running) ? B00001000 : 0);

    if (mysettings.currentMonitoringEnabled && currentMonitor.validReadings) {
        data.byte4 = max((uint8_t)1, (uint8_t)round(mysettings.nominalbatcap / 74.0));
    } else {
        data.byte4 = 1;
    }

    data.byte5 = 0x50;
    data.byte6 = 0x4e;

    send_canbus_message(0x359, (uint8_t *)&data, sizeof(data359));
}

void pylon_message_35c() {
    struct data35c {
        uint8_t byte0;
    };

    data35c data;
    data.byte0 = 0;

    if (rules.IsChargeAllowed(&mysettings)) {
        data.byte0 |= B10000000;
    }

    if (rules.IsDischargeAllowed(&mysettings)) {
        data.byte0 |= B01000000;
    }

    send_canbus_message(0x35c, (uint8_t *)&data, sizeof(data35c));
}

void pylon_message_35e() {
    uint8_t pylon[] = {0x50, 0x59, 0x4c, 0x4f, 0x4e, 0x20, 0x20, 0x20};
    send_canbus_message(0x35e, (uint8_t *)&pylon, sizeof(pylon));
}

void pylon_message_356() {
    struct data356 {
        int16_t voltage;
        int16_t current;
        int16_t temperature;
    };

    data356 data;

    if (mysettings.currentMonitoringEnabled && currentMonitor.validReadings) {
        data.voltage = currentMonitor.modbus.voltage * 100.0;
        data.current = currentMonitor.modbus.current * 10;
    } else {
        data.voltage = rules.highestBankVoltage / 10;
        data.current = 0;
    }

    if (rules.moduleHasExternalTempSensor) {
        data.temperature = (int16_t)rules.highestExternalTemp * (int16_t)10;
    } else {
        data.temperature = 0;
    }

    send_canbus_message(0x356, (uint8_t *)&data, sizeof(data356));
}

