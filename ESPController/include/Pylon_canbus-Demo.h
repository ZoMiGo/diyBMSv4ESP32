#ifndef PYLON_CANBUS_H
#define PYLON_CANBUS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "HAL_ESP32.h"

#define EEPROM_SIZE 64  
#define USE_ESP_IDF_LOG 1  

#if USE_ESP_IDF_LOG
#include "esp_log.h"
static constexpr const char *const TAG = "diybms-pylon";
#define LOG_INFO(...) ESP_LOGI(TAG, __VA_ARGS__)
#define LOG_WARN(...) ESP_LOGW(TAG, __VA_ARGS__)
#define LOG_ERROR(...) ESP_LOGE(TAG, __VA_ARGS__)
#else
static constexpr const char *const TAG = "diybms-pylon";
#define LOG_INFO(...) Serial.printf("[%s] ", TAG); Serial.printf(__VA_ARGS__); Serial.println();
#define LOG_WARN(...) Serial.printf("[%s] WARNUNG: ", TAG); Serial.printf(__VA_ARGS__); Serial.println();
#define LOG_ERROR(...) Serial.printf("[%s] FEHLER: ", TAG); Serial.printf(__VA_ARGS__); Serial.println();
#endif

const uint16_t PYLON_MASTER_ID = 0x351;
const uint16_t PYLON_SLAVE_BASE_ID = 0x360;
const uint16_t VICTRON_CAN_ID = 0x355;

typedef struct {
    uint16_t originalID;
    uint16_t assignedID;
    float voltage;
    float current;
    float soc;
    bool isMaster;
} BMS_Data;

#define MAX_MODULES 16  

void initEEPROM();
void saveIDToEEPROM(int moduleIndex, uint16_t assignedID);
uint16_t readIDFromEEPROM(int moduleIndex);
bool isDuplicateID(uint16_t msgID);
uint16_t getAvailableID(uint16_t originalID);
void setupCAN();
void receiveCAN();
void sendVictronCAN();

#endif // PYLON_CANBUS_H
