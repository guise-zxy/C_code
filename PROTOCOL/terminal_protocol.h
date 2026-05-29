#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    uint8_t version;
    uint8_t packet_type;
    uint16_t seq;
    uint32_t lux;
    uint16_t uv_x100;
    uint8_t battery;
    uint8_t status;
    bool valid;
} terminal_sensor_data_t;

bool terminal_parse_sensor_packet(
    const uint8_t *data,
    size_t len,
    terminal_sensor_data_t *out
);