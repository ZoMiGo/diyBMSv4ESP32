/*------------------------------------------------------------------------
 *
 *   Project: PYLON TECH BATTERY Emulator for DiyBMS
 *            Using Canbus @ 500kbps and 11 bit addresses.
 *
 *   Author:  Trajilovic Goran (ZoMiGo)
 *
 * -----------------------------------------------------------------------
 */


#include "pylon_canbus.h"
#include "bms_id_manager.h"  // Include the ID Manager

extern uint8_t totalSlaves;  // Deklarierte globale Variable

#define USE_ESP_IDF_LOG 1
static constexpr const char *const TAG = "diybms-pylon";

ConsolidatedData consolidatedData;  // Hier wird die Struktur instanziiert

// CAN-Bus Nachricht senden
void send_canbus_message(uint32_t identifier, const uint8_t *buffer, const uint8_t length) {
    const char* messageType = "Unknown Message";

    switch (identifier) {
        case 0x351: messageType = "Charge/Discharge Limits"; break;
        case 0x355: messageType = "State of Charge/Health"; break;
        case 0x356: messageType = "Voltage/Current/Temperature"; break;
        case 0x359: messageType = "Status Flags"; break;
        case 0x35C: messageType = "Charge/Discharge Enable"; break;
        case 0x35E: messageType = "Pylontech Identifier"; break;
        case 0x301: case 0x302: case 0x303: case 0x304: case 0x305:
            messageType = "Slave Data Request"; break;
        default: break;
    }

    ESP_LOGI(TAG, "[%s] CAN ID: 0x%03X Sent", messageType, identifier);

    twai_message_t message;
    message.identifier = identifier;
    message.flags = TWAI_MSG_FLAG_NONE;
    message.data_length_code = length;
    memcpy(message.data, buffer, length);

    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        ESP_LOGI(TAG, "[%s] Sent Successfully", messageType);
    } else {
        ESP_LOGE(TAG, "[%s] Failed to Send", messageType);
    }
}

// CAN-Bus Initialisierung
void setupPylonCanBus() {
    ESP_LOGI(TAG, "Initializing Pylontech CAN Bus...");

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_16, GPIO_NUM_17, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(TAG, "Pylontech CAN Bus initialized.");
    } else {
        ESP_LOGE(TAG, "Failed to initialize Pylontech CAN Bus.");
    }
}

// Daten von Slaves abrufen
void requestDataFromSlaves() {
    ESP_LOGI(TAG, "[diybms-pylon] Requesting data from %d Slaves...", totalSlaves);

    for (uint8_t slaveID = 1; slaveID <= totalSlaves; slaveID++) {
        ESP_LOGI(TAG, "[CANBUS] Requesting from Slave ID: %d", slaveID);
        send_canbus_message(0x300 + slaveID, (uint8_t *)&slaveID, sizeof(slaveID));
    }
}

// Daten aus Slaves konsolidieren
void consolidateSlaveData(uint8_t slaveID, uint16_t voltage, uint16_t current, uint16_t soc) {
    if (isMaster) {  // Nur der Master konsolidiert die Daten
        // Akkumuliert die Spannung
        consolidatedData.totalVoltage += voltage;  // Gesamtspannung der Slaves

        // Akkumuliert den Strom
        consolidatedData.totalCurrent += current;

        // Berechne den höchsten SOC
        consolidatedData.soc = std::max(consolidatedData.soc, soc);

        // Beispielwert für SOH (State of Health)
        consolidatedData.soh = 100;
    }
}



// Daten an Victron weiterleiten (nur vom Master)
// Nur der Master sendet die konsolidierten Daten an Victron
void forwardDataToVictron() {
    if (isMaster) {  // Nur der Master sendet an Victron
        twai_message_t message;
        message.identifier = 0x355;  // CAN-ID für SOC und SOH
        message.data_length_code = 4;

        // Die Gesamtspannung wird als SOC gesendet
        message.data[0] = (consolidatedData.totalVoltage & 0xFF00) >> 8;  // High Byte der Gesamtspannung
        message.data[1] = (consolidatedData.totalVoltage & 0x00FF);         // Low Byte der Gesamtspannung
        message.data[2] = (consolidatedData.soh & 0xFF00) >> 8;            // High Byte des SOH
        message.data[3] = (consolidatedData.soh & 0x00FF);                 // Low Byte des SOH

        if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
            ESP_LOGI(TAG, "Total voltage and SOH forwarded to Victron");
        } else {
            ESP_LOGW(TAG, "Failed to forward total voltage and SOH to Victron");
        }
    }
}
// CAN-Nachrichten empfangen und an Victron weiterleiten
void receiveCANMessages() {
    twai_message_t message;
    esp_err_t result = twai_receive(&message, pdMS_TO_TICKS(100));

    if (result == ESP_OK) {
        ESP_LOGI(TAG, "[CANBUS] Received CAN ID: 0x%03X DLC: %d", message.identifier, message.data_length_code);

        for (int i = 0; i < message.data_length_code; i++) {
            ESP_LOGI(TAG, "[CANBUS] Data[%d]: 0x%02X", i, message.data[i]);
        }

        // Wenn der Master Daten empfangen hat, konsolidiere und sende sie an Victron
        if (isMaster) {
            // Hier wird die Konsolidierung und Weiterleitung an Victron ausgelöst.
            // Beispiel: Konsolidiere Daten, wenn nötig
             forwardDataToVictron();  // Die Daten werden nur vom Master weitergeleitet
        }
    } else if (result == ESP_ERR_TIMEOUT) {
        ESP_LOGW(TAG, "[CANBUS] No messages received (timeout)");
    } else {
        ESP_LOGE(TAG, "[CANBUS] Error receiving message: %s", esp_err_to_name(result));
    }
}




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
    }

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
