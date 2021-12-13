#ifndef DIYBMSWebServer_Json_Req_H_
#define DIYBMSWebServer_Json_Req_H_

#include <stdio.h>

#include <esp_http_server.h>

#include <WiFi.h>

#include "settings.h"
#include "FS.h"
#include <LITTLEFS.h>
#include "SD.h"
#include <SPIFFS.h>
#include "time.h"
#include "defines.h"
#include "Rules.h"
#include "settings.h"
#include "ArduinoJson.h"
#include "PacketRequestGenerator.h"
#include "PacketReceiveProcessor.h"

#include "EmbeddedFiles_AutoGenerated.h"
//#include "EmbeddedFiles_Integrity.h"
#include "HAL_ESP32.h"

esp_err_t content_handler_getvictron(httpd_req_t *req);
esp_err_t content_handler_avrstorage(httpd_req_t *req);
esp_err_t content_handler_currentmonitor(httpd_req_t *req);
esp_err_t content_handler_rs485settings(httpd_req_t *req);
int fileSystemListDirectory(char *buffer, size_t bufferLen, fs::FS &fs, const char *dirname, uint8_t levels);
esp_err_t content_handler_storage(httpd_req_t *req);
esp_err_t SendFileInChunks(httpd_req_t *req, FS &filesystem, const char *filename);
esp_err_t content_handler_downloadfile(httpd_req_t *req);
esp_err_t content_handler_identifymodule(httpd_req_t *req);
esp_err_t content_handler_modules(httpd_req_t *req);
esp_err_t content_handler_avrstatus(httpd_req_t *req);
esp_err_t content_handler_rules(httpd_req_t *req);
esp_err_t content_handler_settings(httpd_req_t *req);
esp_err_t content_handler_integration(httpd_req_t *req);
esp_err_t content_handler_monitor3(httpd_req_t *req);
esp_err_t content_handler_monitor2(httpd_req_t *req);

extern uint32_t canbus_messages_failed_sent;
extern uint32_t canbus_messages_sent;
extern uint32_t canbus_messages_received;

extern fs::SDFS *_sdcard;
extern void (*_sdcardaction_callback)(uint8_t action);
extern PacketRequestGenerator *_prg;
extern PacketReceiveProcessor *_receiveProc;
extern diybms_eeprom_settings *_mysettings;
extern Rules *_rules;
extern ControllerState *_controlState;
extern HAL_ESP32 *_hal;

extern void setCacheControl(httpd_req_t *req);
extern char CookieValue[20 + 1];
#endif
