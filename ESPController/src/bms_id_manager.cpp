/*
(c)2025 Trajilovic Goran
www.globcast.eu gorance@live.de
*/
#include "bms_id_manager.h"
#include <EEPROM.h>
#include <esp_log.h>

static const char* TAG = "BMS_ID_MANAGER";
uint8_t lastID = 1;

void initializeBMSIDs() {
    for (int i = 0; i < 10; i++) { // Example for 10 modules
        uint8_t id = EEPROM.read(i);
        if (id == 0 || id > 100) {
            id = lastID++;
            EEPROM.write(i, id);
            EEPROM.commit();
            ESP_LOGI(TAG, "Assigned ID %d to module %d", id, i);
        }
    }
}

void assignMaster() {
    uint8_t masterID = 255;
    for (int i = 0; i < 10; i++) {
        uint8_t id = EEPROM.read(i);
        if (id < masterID) {
            masterID = id;
        }
    }
    ESP_LOGI(TAG, "Master module ID is %d", masterID);
}

void checkDuplicateIDs() {
    for (int i = 0; i < 10; i++) {
        for (int j = i + 1; j < 10; j++) {
            if (EEPROM.read(i) == EEPROM.read(j)) {
                uint8_t newID = lastID++;
                EEPROM.write(j, newID);
                EEPROM.commit();
                ESP_LOGW(TAG, "Resolved duplicate ID for module %d, new ID %d", j, newID);
            }
        }
    }
}
