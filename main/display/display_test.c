#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"

#include "display_service.h"
#include "SHT31.h"

static const char *TAG = "display_test";

/* 当前 SHT31 接线：SDA -> GPIO8, SCL -> GPIO9 */
#define I2C_MASTER_SDA_IO           8
#define I2C_MASTER_SCL_IO           9
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000
#define I2C_MASTER_TIMEOUT_MS       1000
#define SENSOR_POLL_INTERVAL_MS     30000

/*
 * 1：模块测试阶段，模拟 APP 已经同步一份 daily_report。
 * 0：后续 BLE 联调阶段，不再模拟；由 BLE 回调调用
 *    display_service_post_daily_report()。
 */
#define ENABLE_MOCK_DAILY_REPORT    1

/*
 * 模块测试阶段暂时没有 APP 下发真实时间，也没有联网 SNTP。
 * 因此在启动时，用本次编译时间初始化 ESP32 系统时钟。
 * 后续 BLE 联调时，应删除该临时代码，改为 APP 下发 Unix 时间戳后
 * 调用 settimeofday() 校时。
 */
static int month_from_abbreviation(const char *month)
{
    static const char *const months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };

    for (int i = 0; i < 12; ++i) {
        if (strcmp(month, months[i]) == 0) {
            return i;
        }
    }

    return -1;
}

static esp_err_t set_test_clock_from_build_time(void)
{
    char month_text[4] = {0};
    int day = 0;
    int year = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;

    if (sscanf(__DATE__, "%3s %d %d", month_text, &day, &year) != 3 ||
        sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second) != 3) {
        ESP_LOGE(TAG, "cannot parse build timestamp: %s %s", __DATE__, __TIME__);
        return ESP_FAIL;
    }

    int month = month_from_abbreviation(month_text);
    if (month < 0) {
        ESP_LOGE(TAG, "unknown build month: %s", month_text);
        return ESP_FAIL;
    }

    /* POSIX TZ 语法中 CST-8 表示 UTC+8。 */
    setenv("TZ", "CST-8", 1);
    tzset();

    struct tm time_info = {
        .tm_year = year - 1900,
        .tm_mon = month,
        .tm_mday = day,
        .tm_hour = hour,
        .tm_min = minute,
        .tm_sec = second,
        .tm_isdst = -1,
    };

    time_t unix_seconds = mktime(&time_info);
    if (unix_seconds == (time_t)-1) {
        ESP_LOGE(TAG, "mktime failed for build timestamp");
        return ESP_FAIL;
    }

    struct timeval now = {
        .tv_sec = unix_seconds,
        .tv_usec = 0,
    };

    if (settimeofday(&now, NULL) != 0) {
        ESP_LOGE(TAG, "settimeofday failed");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "temporary test clock initialized from build time: %s %s", __DATE__, __TIME__);
    return ESP_OK;
}

static esp_err_t app_i2c_master_init(i2c_master_bus_handle_t *bus_handle)
{
    if (bus_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&bus_config, bus_handle);
}

static esp_err_t find_and_init_sht31(i2c_master_bus_handle_t bus_handle,
                                     sht31_t *sht31)
{
    if (bus_handle == NULL || sht31 == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t address = 0;

    if (sht31_probe(bus_handle, SHT31_ADDR_LOW, I2C_MASTER_TIMEOUT_MS) == ESP_OK) {
        address = SHT31_ADDR_LOW;
    } else if (sht31_probe(bus_handle, SHT31_ADDR_HIGH, I2C_MASTER_TIMEOUT_MS) == ESP_OK) {
        address = SHT31_ADDR_HIGH;
    } else {
        ESP_LOGE(TAG, "SHT31 not found at 0x44 or 0x45");
        return ESP_ERR_NOT_FOUND;
    }

    ESP_LOGI(TAG, "SHT31 found at 0x%02X", address);
    return sht31_init(sht31, bus_handle, address, I2C_MASTER_FREQ_HZ);
}

static void post_mock_daily_report_once(void)
{
#if ENABLE_MOCK_DAILY_REPORT
    const display_daily_report_t report = {
        .light_level = DISPLAY_LIGHT_LOW,
        .sleep_mode_enabled = true,
        /* 当前仅作为系统时钟无效时的兜底值。 */
        .sync_time_hhmm = "--:--",
        .sync_date_mmdd = "-- --",
    };

    ESP_LOGI(TAG, "post mock daily_report: LOW, sleep mode enabled");
    ESP_ERROR_CHECK(display_service_post_daily_report(&report));
#else
    ESP_LOGI(TAG, "mock daily_report disabled; wait for BLE report");
    ESP_ERROR_CHECK(display_service_request_refresh());
#endif
}

static void sensor_task(void *arg)
{
    sht31_t *sht31 = (sht31_t *)arg;
    bool first_valid_sample = true;

    while (true) {
        float temperature_c = 0.0f;
        float humidity_pct = 0.0f;
        esp_err_t ret = sht31_read_temp_humi(sht31, &temperature_c, &humidity_pct);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SHT31 read failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(SENSOR_POLL_INTERVAL_MS));
            continue;
        }

        ESP_LOGI(TAG, "SHT31: %.1f C, %.1f %%", temperature_c, humidity_pct);

        /*
         * 第一份温湿度先进入模型，再由 mock daily_report 触发首次刷屏。
         * 后续样本主动请求刷新；display_service 会限制全刷频率。
         */
        ESP_ERROR_CHECK(display_service_post_environment(temperature_c,
                                                         humidity_pct,
                                                         !first_valid_sample));

        if (first_valid_sample) {
            post_mock_daily_report_once();
            first_valid_sample = false;
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_POLL_INTERVAL_MS));
    }
}

// void app_main(void)
// {
//     static sht31_t sht31 = {0};
//     i2c_master_bus_handle_t bus_handle = NULL;

//     ESP_LOGI(TAG, "start EPD Chinese template + live SHT31 module test");

//     ESP_ERROR_CHECK(set_test_clock_from_build_time());
//     ESP_ERROR_CHECK(display_service_start());
//     ESP_ERROR_CHECK(app_i2c_master_init(&bus_handle));
//     ESP_ERROR_CHECK(find_and_init_sht31(bus_handle, &sht31));

//     BaseType_t ok = xTaskCreate(sensor_task,
//                                 "sht31_task",
//                                 4096,
//                                 &sht31,
//                                 4,
//                                 NULL);
//     ESP_ERROR_CHECK(ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM);
// }
