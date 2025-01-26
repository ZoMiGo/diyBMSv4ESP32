/*
(c)2025 Trajilovic Goran
gorance@live.de
www.globcast.eu
*/


#include "bms_can_communication.h"
#include <driver/twai.h>
#include <esp_log.h>

static const char* TAG = "BMS_CAN_COMM";

void setupCANCommunication() {
    ESP_LOGI(TAG, "Initializing CAN communication");
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    ESP_ERROR_CHECK(twai_driver_install(&g_config, &t_config, &f_config));
    ESP_ERROR_CHECK(twai_start());
}

void processIncomingCANMessages() {
    twai_message_t message;
    while (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
        ESP_LOGI(TAG, "Received CAN message: ID=%X, Length=%d", message.identifier, message.data_length_code);
    }
}
