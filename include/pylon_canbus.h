/*------------------------------------------------------------------------
 *
 *   Project: PYLON TECH BATTERY Emulator for DiyBMS
 *            Using Canbus @ 500kbps and 11 bit addresses.
 *
 *   Author:  Trajilovic Goran (ZoMiGo)
 *
 * -----------------------------------------------------------------------
 */

#ifndef DIYBMS_PYLON_CANBUS_H_
#define DIYBMS_PYLON_CANBUS_H_

#include "defines.h"
#include "Rules.h"
#include <driver/twai.h>  // Für CAN-Bus Kommunikation
#include <cstring>
#include <cmath>
#include <algorithm> // Für std::min und std::max

extern bool isMaster;  // Deklaration von isMaster, damit es in der .cpp-Datei verwendet werden kann

// Deklarationen der externen Variablen
extern uint32_t canbus_messages_sent;           // Anzahl der gesendeten CAN-Bus Nachrichten
extern uint32_t canbus_messages_failed_sent;    // Anzahl der fehlgeschlagenen CAN-Bus Nachrichten
extern ControllerState _controller_state;       // Status des Controllers
extern diybms_eeprom_settings mysettings;       // EEPROM-Einstellungen
extern Rules rules;                             // Regeln für das Batterie-Management
extern currentmonitoring_struct currentMonitor; // Struktur für die aktuelle Überwachung
extern uint8_t TotalNumberOfCells();            // Gesamtzahl der Zellen
extern std::string hostname;                    // Hostname des Geräts
extern uint32_t canbus_messages_received;       // Anzahl der empfangenen CAN-Bus Nachrichten

// Struktur zur Konsolidierung der empfangenen Daten
struct ConsolidatedData {
    uint16_t totalVoltage = 0;   // Gesamtspannung der Slaves
    int16_t totalCurrent = 0;    // Gesamtstrom
    uint16_t soc = 0;            // State of Charge
    uint16_t soh = 100;          // State of Health
};

extern ConsolidatedData consolidatedData;  // Konsolidierte Daten, die vom Master gesendet werden

// Funktionsdeklarationen
void send_canbus_message(uint32_t identifier, const uint8_t *buffer, const uint8_t length);  // Funktion zum Senden von CAN-Bus-Nachrichten
void setupPylonCanBus();                           // Initialisierung des PylonTech CAN-Bus
void requestDataFromSlaves();                      // Anforderung von Daten von den Slaves
void consolidateSlaveData(uint8_t slaveID, uint16_t voltage, uint16_t current, uint16_t soc);  // Konsolidierung der Slavedaten
void forwardDataToVictron();                       // Weiterleitung der konsolidierten Daten an Victron
void receiveCANMessages();  // Deklaration der Funktion zum Empfangen von CAN-Nachrichten
void setupModuleRole();  // Deklaration der Funktion

// Pylon-spezifische Nachrichten
void pylon_message_351();  // Lade- und Entladegrenzen
void pylon_message_355();  // SOC und SOH
void pylon_message_356();  // Spannung, Strom, Temperatur
void pylon_message_359();  // Status-Flags
void pylon_message_35c();  // Lade-/Entladeaktivierung
void pylon_message_35e();  // Pylontech Herstellerkennung

#endif // DIYBMS_PYLON_CANBUS_H_
