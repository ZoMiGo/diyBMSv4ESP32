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

#include "bms_id_manager.h"
#include <esp_log.h>

#define EEPROM_SIZE 1024
static const char *TAG = BMS_ID_MANAGER_TAG;

BMS_Data bmsModules[MAX_BMS_MODULES];
uint16_t lastID = 1;

// **Neue ID zuweisen**
uint16_t assignNewID() {
    return lastID++;
}

// **BMS-IDs initialisieren**
uint8_t totalSlaves = 0;  // Initialize totalSlaves globally

void initializeBMSIDs() {
    totalSlaves = 0;  // Reset counter
    for (int i = 0; i < 10; i++) { // Check up to 10 modules
        uint8_t id = EEPROM.read(i);
        if (id > 0 && id < 100) { // Valid ID range
            totalSlaves++;
        }
    }
    ESP_LOGI(TAG, "Detected %d Slaves", totalSlaves);
}

// **Master bestimmen**
void assignMaster() {
    uint16_t masterID = 255;

    for (int i = 0; i < MAX_BMS_MODULES; i++) {
        if (bmsModules[i].assignedID < masterID) {
            masterID = bmsModules[i].assignedID;
            bmsModules[i].isMaster = true;
        } else {
            bmsModules[i].isMaster = false;
        }
    }

    ESP_LOGI(TAG, "Master BMS ID: %d", masterID);
}

// **Doppelte IDs prüfen & korrigieren**
void checkDuplicateIDs() {
    uint8_t idList[101] = {0};

    for (int i = 0; i < MAX_BMS_MODULES; i++) {
        uint16_t id = bmsModules[i].assignedID;

        if (idList[id] > 0) {
            id = assignNewID();
            saveIDToEEPROM(i, id);
            ESP_LOGW(TAG, "Duplicate ID resolved for Module %d. New ID: %d", i, id);
        } else {
            idList[id]++;
        }
    }
}

// **ID aus EEPROM lesen**
uint16_t readIDFromEEPROM(int moduleIndex) {
    uint16_t id;
    EEPROM.get(moduleIndex * sizeof(uint16_t), id);
    return id;
}

// **ID im EEPROM speichern**
void saveIDToEEPROM(int moduleIndex, uint16_t assignedID) {
    EEPROM.put(moduleIndex * sizeof(uint16_t), assignedID);
    EEPROM.commit();
}
