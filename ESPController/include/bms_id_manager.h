/*------------------------------------------------------------------------
 *
 *   Project: BMS ID Manager for DiyBMS
 *            
 *
 *   Author:  Trajilovic Goran (ZoMiGo)
 *   www.globcast.eu
 *
 * -----------------------------------------------------------------------
 */

#ifndef BMS_ID_MANAGER_H
#define BMS_ID_MANAGER_H

#include <Arduino.h>
#include <EEPROM.h>

// Maximale Anzahl von unterstützten BMS-Modulen
#define MAX_BMS_MODULES 10

// Logging
#define USE_ESP_IDF_LOG 1
#ifndef BMS_ID_MANAGER_TAG
#define BMS_ID_MANAGER_TAG "BMS_ID_MANAGER"
#endif

// Struktur für BMS-Modul-Daten
typedef struct {
    uint16_t assignedID;
    bool isMaster;
    float voltage;
    float current;
    float soc;
} BMS_Data;

// Globale BMS-Module
extern BMS_Data bmsModules[MAX_BMS_MODULES];
extern uint8_t totalSlaves;  // Declare globally

// Funktionsdeklarationen
void initializeBMSIDs();   // IDs initialisieren
void assignMaster();       // Bestimmt das Modul mit der niedrigsten ID als Master
void checkDuplicateIDs();  // Überprüfung & Korrektur doppelter IDs
uint16_t readIDFromEEPROM(int moduleIndex);
void saveIDToEEPROM(int moduleIndex, uint16_t assignedID);
uint16_t assignNewID();

#endif // BMS_ID_MANAGER_H

