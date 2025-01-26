/*
(c)2025 Trajilovic Goran
www.globcast.eu
*/
#ifndef DIYBMS_PYLON_CANBUS_H_ 
#define DIYBMS_PYLON_CANBUS_H_

#include "defines.h"
#include "Rules.h"
#include <driver/twai.h>
#include <cstring>
#include <cmath>
#include <algorithm> // for std::min and std::max

// Externe Variablen
extern uint32_t canbus_messages_sent;
extern uint32_t canbus_messages_failed_sent;
extern ControllerState _controller_state;
extern diybms_eeprom_settings mysettings;
extern Rules rules;
extern currentmonitoring_struct currentMonitor;
extern uint8_t TotalNumberOfCells();
extern std::string hostname;
extern uint32_t canbus_messages_received;

// Struktur zur Konsolidierung der Daten
struct ConsolidatedData {
    uint16_t minVoltage = 0xFFFF;
    uint16_t maxVoltage = 0;
    int16_t totalCurrent = 0;
    uint16_t soc = 0;  // State of Charge
    uint16_t soh = 0;  // State of Health
};

extern ConsolidatedData consolidatedData;

// Funktionen
void send_canbus_message(uint32_t identifier, const uint8_t *buffer, const uint8_t length);
void setupPylonCanBus();
void requestDataFromSlaves();
void consolidateSlaveData(uint8_t slaveID, uint16_t voltage, uint16_t current, uint16_t soc);
void forwardDataToVictron();

void pylon_message_351();
void pylon_message_355();
void pylon_message_356();
void pylon_message_359();
void pylon_message_35c();
void pylon_message_35e();

// Konsolidierung der empfangenen Daten
inline void consolidateSlaveData(uint8_t slaveID, uint16_t voltage, uint16_t current, uint16_t soc) {
    consolidatedData.minVoltage = std::min(consolidatedData.minVoltage, voltage);
    consolidatedData.maxVoltage = std::max(consolidatedData.maxVoltage, voltage);
    consolidatedData.totalCurrent += current;
    consolidatedData.soc = std::max(consolidatedData.soc, soc);
    // SOH wird hier exemplarisch auf 100% gesetzt
    consolidatedData.soh = 100;
}

// Weiterleitung der konsolidierten Daten an Victron
inline void forwardDataToVictron() {
    // Beispiel: Weiterleiten der SOC und SOH
    twai_message_t message;
    message.identifier = 0x355;  // Beispiel-SOC/SOH-Identifier
    message.data_length_code = 4;
    message.data[0] = (consolidatedData.soc & 0xFF00) >> 8;
    message.data[1] = (consolidatedData.soc & 0x00FF);
    message.data[2] = (consolidatedData.soh & 0xFF00) >> 8;
    message.data[3] = (consolidatedData.soh & 0x00FF);

    if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        ESP_LOGI("PylonTech", "SOC and SOH forwarded to Victron");
    } else {
        ESP_LOGW("PylonTech", "Failed to forward SOC and SOH to Victron");
    }
}

#endif


