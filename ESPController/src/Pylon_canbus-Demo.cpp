#include <Arduino.h>
#include <EEPROM.h>
#include "HAL_ESP32.h"
#include "pylon_canbus.h"  // Header-Datei einbinden

void initEEPROM() {
    EEPROM.begin(EEPROM_SIZE);
    LOG_INFO("EEPROM initialisiert.");
}

void saveIDToEEPROM(int moduleIndex, uint16_t assignedID) {
    int addr = moduleIndex * 4;
    EEPROM.write(addr, (assignedID >> 8) & 0xFF);
    EEPROM.write(addr + 1, assignedID & 0xFF);
    EEPROM.commit();
}

uint16_t readIDFromEEPROM(int moduleIndex) {
    int addr = moduleIndex * 4;
    return (EEPROM.read(addr) << 8) | EEPROM.read(addr + 1);
}

bool isDuplicateID(uint16_t msgID) {
    for (int i = 0; i < moduleCount; i++) {
        if (bmsModules[i].assignedID == msgID) {
            return true;
        }
    }
    return false;
}

uint16_t getAvailableID(uint16_t originalID) {
    uint16_t newID = originalID;
    while (isDuplicateID(newID)) {  
        newID++;
    }
    return newID;
}

void setupCAN() {
    HAL_ESP32_Canbus_Initialize();
}

void receiveCAN() {
    CAN_FRAME rx_frame;
    if (HAL_ESP32_Canbus_Receive(&rx_frame)) {
        uint16_t msgID = rx_frame.id;
        uint16_t assignedID = msgID;

        if (isDuplicateID(msgID)) {
            assignedID = getAvailableID(msgID);
            LOG_WARN("Doppelte ID %X erkannt! Neue ID: %X (EEPROM gespeichert)", msgID, assignedID);
            saveIDToEEPROM(moduleCount, assignedID);
        } else {
            assignedID = readIDFromEEPROM(moduleCount);
            if (assignedID == 0xFFFF) {
                assignedID = msgID;
                saveIDToEEPROM(moduleCount, assignedID);
            }
        }

        uint8_t moduleIndex = (msgID == PYLON_MASTER_ID) ? 0 : (msgID - PYLON_SLAVE_BASE_ID + 1);
        if (moduleIndex >= MAX_MODULES) return;

        bmsModules[moduleIndex].originalID = msgID;
        bmsModules[moduleIndex].assignedID = assignedID;
        bmsModules[moduleIndex].voltage = ((rx_frame.data.byte[0] << 8) | rx_frame.data.byte[1]) / 10.0;
        bmsModules[moduleIndex].current = ((rx_frame.data.byte[2] << 8) | rx_frame.data.byte[3]) / 10.0;
        bmsModules[moduleIndex].soc = rx_frame.data.byte[4];

        if (msgID == PYLON_MASTER_ID) {
            masterIndex = moduleIndex;
            bmsModules[moduleIndex].isMaster = true;
        } else {
            bmsModules[moduleIndex].isMaster = false;
        }

        if (moduleIndex >= moduleCount) {
            moduleCount = moduleIndex + 1;
        }

        LOG_INFO("Modul %d [%s]: Ursprungs-ID %X, Zugewiesene ID %X, %.2fV, %.2fA, SoC: %.1f%%", 
                 moduleIndex, 
                 bmsModules[moduleIndex].isMaster ? "MASTER" : "SLAVE",
                 msgID, assignedID, 
                 bmsModules[moduleIndex].voltage, 
                 bmsModules[moduleIndex].current, 
                 bmsModules[moduleIndex].soc);
    }
}

void sendVictronCAN() {
    if (moduleCount == 0 || masterIndex == -1) return;

    CAN_FRAME tx_frame;
    tx_frame.id = VICTRON_CAN_ID;
    tx_frame.length = 8;

    float totalCurrent = 0;
    float avgSOC = 0;

    for (int i = 0; i < moduleCount; i++) {
        totalCurrent += bmsModules[i].current;
        avgSOC += bmsModules[i].soc;
    }
    avgSOC /= moduleCount;

    uint16_t v_mV = (uint16_t)(bmsModules[masterIndex].voltage * 10);
    uint16_t c_mA = (uint16_t)(totalCurrent * 10);

    tx_frame.data.byte[0] = v_mV >> 8;
    tx_frame.data.byte[1] = v_mV & 0xFF;
    tx_frame.data.byte[2] = c_mA >> 8;
    tx_frame.data.byte[3] = c_mA & 0xFF;
    tx_frame.data.byte[4] = (uint8_t)avgSOC;

    HAL_ESP32_Canbus_Send(&tx_frame);
    LOG_INFO("Gesendet an Victron: %.2fV, %.2fA, SoC: %.1f%%", bmsModules[masterIndex].voltage, totalCurrent, avgSOC);
}

void setup() {
    Serial.begin(115200);
    initEEPROM();
    setupCAN();
}

void loop() {
    receiveCAN();
    sendVictronCAN();
    delay(1000);
}
