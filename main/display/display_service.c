#include "display_service.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "EPD.h"
#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "display_service";

/*
 * 正式运行时保持 180000 ms：Waveshare 建议全刷间隔至少 180 秒。
 * 仅在短时间台架验证时，可临时改为 30000 ms，验证完成后必须恢复。
 */
#define DISPLAY_MIN_FULL_REFRESH_INTERVAL_MS  180000
#define DISPLAY_QUEUE_LENGTH                  8
#define DISPLAY_TASK_STACK_SIZE               4096
#define DISPLAY_TASK_PRIORITY                 5

typedef enum {
    DISPLAY_EVENT_ENVIRONMENT = 0,
    DISPLAY_EVENT_DAILY_REPORT,
    DISPLAY_EVENT_REFRESH,
} display_event_type_t;

typedef struct {
    display_event_type_t type;
    bool request_refresh;
    union {
        struct {
            float temperature_c;
            float humidity_pct;
        } environment;
        display_daily_report_t report;
    } data;
} display_event_t;

static uint8_t s_frame_buffer[EPD_BUF_SIZE] __attribute__((aligned(4)));
static display_page_model_t s_model;
static QueueHandle_t s_queue = NULL;
static TaskHandle_t s_task = NULL;
static TickType_t s_last_refresh_tick = 0;
static bool s_refresh_pending = false;

static void copy_fixed_string(char destination[6], const char *source, const char *fallback)
{
    if (source == NULL || source[0] == '\0') {
        snprintf(destination, 6, "%.5s", fallback);
        return;
    }

    snprintf(destination, 6, "%.5s", source);
}

static bool refresh_allowed_now(void)
{
    if (s_last_refresh_tick == 0) {
        return true;
    }

    TickType_t elapsed = xTaskGetTickCount() - s_last_refresh_tick;
    return elapsed >= pdMS_TO_TICKS(DISPLAY_MIN_FULL_REFRESH_INTERVAL_MS);
}

static TickType_t ticks_until_refresh_allowed(void)
{
    if (s_last_refresh_tick == 0) {
        return 0;
    }

    TickType_t min_ticks = pdMS_TO_TICKS(DISPLAY_MIN_FULL_REFRESH_INTERVAL_MS);
    TickType_t elapsed = xTaskGetTickCount() - s_last_refresh_tick;

    if (elapsed >= min_ticks) {
        return 0;
    }

    return min_ticks - elapsed;
}


static void update_datetime_snapshot_from_system_clock(void)
{
    time_t now = time(NULL);
    struct tm local_time = {0};

    if (now < (time_t)1704067200) { /* 2024-01-01 UTC：系统时钟尚未有效校准 */
        ESP_LOGW(TAG, "system clock is not synchronized; keep fallback date/time");
        return;
    }

    localtime_r(&now, &local_time);
    strftime(s_model.sync_time_hhmm, sizeof(s_model.sync_time_hhmm), "%H:%M", &local_time);
    strftime(s_model.sync_date_mmdd, sizeof(s_model.sync_date_mmdd), "%m-%d", &local_time);
}

static esp_err_t refresh_screen_now(void)
{
    update_datetime_snapshot_from_system_clock();

    ESP_RETURN_ON_ERROR(epd_init(), TAG, "epd_init failed");
    display_page_render(s_frame_buffer, &s_model);
    ESP_RETURN_ON_ERROR(epd_display_frame(s_frame_buffer), TAG, "epd_display_frame failed");

    /*
     * 台架调试阶段暂时不进入 deep sleep。
     * 先验证屏幕能否多次稳定刷新。
     */
    // ESP_RETURN_ON_ERROR(epd_sleep(), TAG, "epd_sleep failed");

    s_last_refresh_tick = xTaskGetTickCount();
    ESP_LOGI(TAG, "EPD full refresh completed");
    return ESP_OK;
}
static void apply_event(const display_event_t *event)
{
    if (event == NULL) {
        return;
    }

    switch (event->type) {
        case DISPLAY_EVENT_ENVIRONMENT:
            s_model.temperature_c = event->data.environment.temperature_c;
            s_model.humidity_pct = event->data.environment.humidity_pct;
            s_model.environment_valid = true;
            break;

        case DISPLAY_EVENT_DAILY_REPORT:
            s_model.light_level = event->data.report.light_level;
            s_model.sleep_mode_enabled = event->data.report.sleep_mode_enabled;
            s_model.report_received = true;
            copy_fixed_string(s_model.sync_time_hhmm,
                              event->data.report.sync_time_hhmm,
                              "--:--");
            copy_fixed_string(s_model.sync_date_mmdd,
                              event->data.report.sync_date_mmdd,
                              "-- --");
            ESP_LOGI(TAG,
                     "daily_report queued: light=%d sleep_mode=%d time=%s date=%s",
                     (int)s_model.light_level,
                     (int)s_model.sleep_mode_enabled,
                     s_model.sync_time_hhmm,
                     s_model.sync_date_mmdd);
            break;

        case DISPLAY_EVENT_REFRESH:
        default:
            break;
    }

    if (event->request_refresh) {
        s_refresh_pending = true;
    }
}

static void display_task(void *arg)
{
    (void)arg;

    while (true) {
        display_event_t event = {0};
        TickType_t wait_ticks = s_refresh_pending ? ticks_until_refresh_allowed() : portMAX_DELAY;

        if (xQueueReceive(s_queue, &event, wait_ticks) == pdPASS) {
            apply_event(&event);
        }

        if (s_refresh_pending && refresh_allowed_now()) {
            esp_err_t ret = refresh_screen_now();
            if (ret == ESP_OK) {
                s_refresh_pending = false;
            } else {
                ESP_LOGE(TAG, "refresh failed: %s", esp_err_to_name(ret));
                /* Avoid a tight retry loop if wiring or BUSY is abnormal. */
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }
}

static esp_err_t post_event(const display_event_t *event)
{
    if (s_queue == NULL || event == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (xQueueSend(s_queue, event, 0) != pdPASS) {
        ESP_LOGE(TAG, "display queue is full");
        return ESP_ERR_TIMEOUT;
    }

    return ESP_OK;
}

esp_err_t display_service_start(void)
{
    if (s_task != NULL) {
        return ESP_OK;
    }

    display_page_set_defaults(&s_model);

    ESP_RETURN_ON_ERROR(epd_gpio_init(), TAG, "epd_gpio_init failed");
    ESP_RETURN_ON_ERROR(epd_spi_bus_init(), TAG, "epd_spi_bus_init failed");

    s_queue = xQueueCreate(DISPLAY_QUEUE_LENGTH, sizeof(display_event_t));
    if (s_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    BaseType_t ok = xTaskCreate(display_task,
                                "display_task",
                                DISPLAY_TASK_STACK_SIZE,
                                NULL,
                                DISPLAY_TASK_PRIORITY,
                                &s_task);
    if (ok != pdPASS) {
        vQueueDelete(s_queue);
        s_queue = NULL;
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "display service started");
    return ESP_OK;
}

esp_err_t display_service_post_environment(float temperature_c,
                                           float humidity_pct,
                                           bool request_refresh)
{
    display_event_t event = {
        .type = DISPLAY_EVENT_ENVIRONMENT,
        .request_refresh = request_refresh,
        .data.environment = {
            .temperature_c = temperature_c,
            .humidity_pct = humidity_pct,
        },
    };

    return post_event(&event);
}

esp_err_t display_service_post_daily_report(const display_daily_report_t *report)
{
    if (report == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    display_event_t event = {
        .type = DISPLAY_EVENT_DAILY_REPORT,
        .request_refresh = true,
    };
    event.data.report = *report;

    return post_event(&event);
}

esp_err_t display_service_request_refresh(void)
{
    display_event_t event = {
        .type = DISPLAY_EVENT_REFRESH,
        .request_refresh = true,
    };

    return post_event(&event);
}
