#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"

#include "ble_terminal.h"

static const char *TAG = "app";

// void app_main(void)
// {
//     ESP_LOGI(TAG, "Sleep-Agent BLE terminal test start");

//     ESP_ERROR_CHECK(ble_terminal_init());

//     while (1) {
//         ESP_LOGI(TAG, "main alive");
//         vTaskDelay(pdMS_TO_TICKS(5000));
//     }
// }