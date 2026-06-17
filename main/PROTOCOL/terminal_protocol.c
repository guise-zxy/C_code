#include "terminal_protocol.h"
#include <string.h>

bool terminal_parse_sensor_packet(
    const uint8_t *data,
    size_t len,
    terminal_sensor_data_t *out
)
{
    if (data == NULL || out == NULL) {
        return false;
    }

    if (len != 16) {
        return false;
    }

    if (data[0] != 1) {
        return false;
    }

    if (data[1] != 1) {
        return false;
    }

    memset(out, 0, sizeof(*out));

    out->version = data[0];
    out->packet_type = data[1];

    out->seq =
        ((uint16_t)data[2]) |
        ((uint16_t)data[3] << 8);

    out->lux =
        ((uint32_t)data[4]) |
        ((uint32_t)data[5] << 8) |
        ((uint32_t)data[6] << 16) |
        ((uint32_t)data[7] << 24);

    out->uv_x100 =
        ((uint16_t)data[8]) |
        ((uint16_t)data[9] << 8);

    out->battery = data[10];
    out->status = data[11];
    out->valid = true;

    return true;
}