#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "EPD.h"

typedef enum {
    DISPLAY_LIGHT_WAITING = 0,
    DISPLAY_LIGHT_LOW,
    DISPLAY_LIGHT_NORMAL,
    DISPLAY_LIGHT_GOOD,
    DISPLAY_LIGHT_HIGH,
} display_light_level_t;

typedef struct {
    float temperature_c;
    float humidity_pct;
    bool environment_valid;

    display_light_level_t light_level;
    bool sleep_mode_enabled;
    bool report_received;

    char sync_time_hhmm[6]; /* HH:MM */
    char sync_date_mmdd[6]; /* MM-DD */
} display_page_model_t;

void display_page_set_defaults(display_page_model_t *model);
void display_page_render(uint8_t *frame_buffer, const display_page_model_t *model);
