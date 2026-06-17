#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "audio_input.h"

static const char *TAG = "main";

// void app_main(void)
// {
//     ESP_ERROR_CHECK(audio_input_init());

//     while (1) {
//         int rms = 0;
//         esp_err_t ret = audio_input_read_rms(&rms);

//         if (ret == ESP_OK) {
//             ESP_LOGI(TAG, "mic rms = %d", rms);
//         } else {
//             ESP_LOGE(TAG, "read rms failed: %s", esp_err_to_name(ret));
//         }

//         vTaskDelay(pdMS_TO_TICKS(500));
//     }
// }