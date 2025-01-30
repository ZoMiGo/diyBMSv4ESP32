
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

#ifndef DIYBMS_PYLON_CANBUS_H_
#define DIYBMS_PYLON_CANBUS_H_

#include "defines.h"
#include "Rules.h"
#include <driver/twai.h>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "pylon_canbus.h"

// Logging & Debugging
//#define USE_ESP_IDF_LOG 1
//static constexpr const char *const TAG = "diybms-pylon";

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
extern void send_canbus_message(uint32_t identifier, const uint8_t *buffer, uint8_t length);

// Datenstruktur für CAN-Bus Nachrichten
struct ConsolidatedData {
    uint16_t minVoltage = 0xFFFF;
    uint16_t maxVoltage = 0;
    int16_t totalCurrent = 0;
    uint16_t soc = 0;
    uint16_t soh = 0;
};

extern ConsolidatedData consolidatedData;

// Funktionen für CAN-Kommunikation
void setupPylonCanBus();
void requestDataFromSlaves();
void consolidateSlaveData(uint8_t slaveID, uint16_t voltage, uint16_t current, uint16_t soc);
void forwardDataToVictron();
void receiveCANMessages();  // Add this function prototype

// Pylontech-spezifische CAN-Nachrichten
void pylon_message_351();
void pylon_message_355();
void pylon_message_356();
void pylon_message_359();
void pylon_message_35c();
void pylon_message_35e();

#endif // DIYBMS_PYLON_CANBUS_H_
