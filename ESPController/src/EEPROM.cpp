#include "EEPROM.h"

void saveModuleID(uint8_t id) {
    EEPROM.write(0, id);
    EEPROM.commit();
}

uint8_t loadModuleID() {
    return EEPROM.read(0);
}
