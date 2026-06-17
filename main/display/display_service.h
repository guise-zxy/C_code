#pragma once

#include <stdbool.h>

#include "display_page.h"
#include "esp_err.h"

typedef struct {
    display_light_level_t light_level;
    bool sleep_mode_enabled;
    char sync_time_hhmm[6]; /* HH:MM */
    char sync_date_mmdd[6]; /* MM-DD */
} display_daily_report_t;

/*
 * Starts the only task that is allowed to access the EPD SPI device.
 * Call once during application startup.
 */
esp_err_t display_service_start(void);

/* Non-blocking: queue latest SHT31 values. Safe for an ordinary task. */
esp_err_t display_service_post_environment(float temperature_c,
                                           float humidity_pct,
                                           bool request_refresh);

/*
 * Non-blocking: queue an APP daily_report. This is suitable for a BLE callback:
 * it does not perform the several-second EPD refresh inside the callback.
 */
esp_err_t display_service_post_daily_report(const display_daily_report_t *report);

/* Queue one refresh request without changing the current model. */
esp_err_t display_service_request_refresh(void);
