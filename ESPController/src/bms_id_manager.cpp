/*
(c)2025 Trajilovic Goran
www.globcast.eu gorance@live.de
*/
// bms_id_manager.cpp
#include "bms_id_manager.h"
#include <EEPROM.h>
#include <esp_log.h>

static const char* TAG = "BMS_ID_MANAGER";
uint8_t lastID = 1;

// Initialize BMS IDs
void initializeBMSIDs() {
    ESP_LOGI(TAG, "Initializing BMS IDs");
    for (int i = 0; i < 10; i++) { // Example for 10 modules
        uint8_t id = EEPROM.read(i);
        if (id == 0 || id > 100) {
            id = lastID++;
            EEPROM.write(i, id);
            EEPROM.commit();
        }
        ESP_LOGI(TAG, "Module %d assigned ID: %d", i, id);
    }
}

// Assign Master BMS
void assignMaster() {
    uint8_t masterID = 255;
    for (int i = 0; i < 10; i++) {
        uint8_t id = EEPROM.read(i);
        if (id < masterID) {
            masterID = id;
        }
    }
    ESP_LOGI(TAG, "Master BMS ID: %d", masterID);
}

// Check and resolve duplicate IDs
void checkDuplicateIDs() {
    uint8_t idList[10] = {0};
    for (int i = 0; i < 10; i++) {
        uint8_t id = EEPROM.read(i);
        if (idList[id] > 0) {
            id = lastID++;
            EEPROM.write(i, id);
            EEPROM.commit();
            ESP_LOGW(TAG, "Duplicate ID resolved for Module %d. New ID: %d", i, id);
        } else {
            idList[id]++;
        }
    }
}
