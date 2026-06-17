#include "display_page.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "display_font.h"

static inline void set_pixel(uint8_t *buffer, int x, int y, bool black)
{
    if (buffer == NULL || x < 0 || x >= EPD_WIDTH || y < 0 || y >= EPD_HEIGHT) {
        return;
    }

    size_t index = (size_t)y * (EPD_WIDTH / 8) + (size_t)(x / 8);
    uint8_t mask = (uint8_t)(0x80U >> (x % 8));

    if (black) {
        buffer[index] &= (uint8_t)~mask;
    } else {
        buffer[index] |= mask;
    }
}

static const char *utf8_next(const char *text, uint32_t *codepoint)
{
    const unsigned char *p = (const unsigned char *)text;

    if (p == NULL || *p == 0 || codepoint == NULL) {
        return NULL;
    }

    if (p[0] < 0x80U) {
        *codepoint = p[0];
        return text + 1;
    }

    if ((p[0] & 0xE0U) == 0xC0U && (p[1] & 0xC0U) == 0x80U) {
        *codepoint = ((uint32_t)(p[0] & 0x1FU) << 6) |
                     (uint32_t)(p[1] & 0x3FU);
        return text + 2;
    }

    if ((p[0] & 0xF0U) == 0xE0U &&
        (p[1] & 0xC0U) == 0x80U &&
        (p[2] & 0xC0U) == 0x80U) {
        *codepoint = ((uint32_t)(p[0] & 0x0FU) << 12) |
                     ((uint32_t)(p[1] & 0x3FU) << 6) |
                     (uint32_t)(p[2] & 0x3FU);
        return text + 3;
    }

    /* Skip malformed byte so one bad character does not block the page. */
    *codepoint = (uint32_t)' ';
    return text + 1;
}

static void draw_glyph(uint8_t *buffer,
                       const display_font_glyph_t *glyph,
                       int x,
                       int y)
{
    if (glyph == NULL) {
        return;
    }

    for (int row = 0; row < DISPLAY_FONT_HEIGHT; ++row) {
        for (int byte_i = 0; byte_i < DISPLAY_FONT_ROW_BYTES; ++byte_i) {
            uint8_t bits = glyph->bitmap[row * DISPLAY_FONT_ROW_BYTES + byte_i];
            for (int bit = 0; bit < 8; ++bit) {
                if ((bits & (uint8_t)(0x80U >> bit)) != 0U) {
                    set_pixel(buffer, x + byte_i * 8 + bit, y + row, true);
                }
            }
        }
    }
}

static void draw_text_utf8(uint8_t *buffer, const char *text, int x, int y)
{
    int cursor_x = x;
    const char *cursor = text;

    while (cursor != NULL && *cursor != '\0') {
        uint32_t codepoint = 0;
        cursor = utf8_next(cursor, &codepoint);
        if (cursor == NULL) {
            break;
        }

        const display_font_glyph_t *glyph = display_font_find(codepoint);
        if (glyph == NULL) {
            cursor_x += 10;
            continue;
        }

        draw_glyph(buffer, glyph, cursor_x, y);
        cursor_x += glyph->advance;
    }
}

static const char *light_level_text(display_light_level_t level)
{
    switch (level) {
        case DISPLAY_LIGHT_LOW:    return "偏低";
        case DISPLAY_LIGHT_NORMAL: return "适宜";
        case DISPLAY_LIGHT_GOOD:   return "充足";
        case DISPLAY_LIGHT_HIGH:   return "偏高";
        case DISPLAY_LIGHT_WAITING:
        default:                   return "待同步";
    }
}

static const char *advice_text(display_light_level_t level)
{
    switch (level) {
        case DISPLAY_LIGHT_LOW:    return "明日上午增加户外活动";
        case DISPLAY_LIGHT_NORMAL: return "保持当前日间活动节奏";
        case DISPLAY_LIGHT_GOOD:   return "今日光照充足，保持当前节奏";
        case DISPLAY_LIGHT_HIGH:   return "注意防晒并减少暴晒";
        case DISPLAY_LIGHT_WAITING:
        default:                   return "请先同步今日光照数据";
    }
}

void display_page_set_defaults(display_page_model_t *model)
{
    if (model == NULL) {
        return;
    }

    memset(model, 0, sizeof(*model));
    model->light_level = DISPLAY_LIGHT_WAITING;
    snprintf(model->sync_time_hhmm, sizeof(model->sync_time_hhmm), "--:--");
    snprintf(model->sync_date_mmdd, sizeof(model->sync_date_mmdd), "-- --");
}

void display_page_render(uint8_t *frame_buffer, const display_page_model_t *model)
{
    char line[128];

    if (frame_buffer == NULL || model == NULL) {
        return;
    }

    memset(frame_buffer, EPD_COLOR_WHITE, EPD_BUF_SIZE);

    draw_text_utf8(frame_buffer, "卧室终端助手", 34, 14);

    snprintf(line, sizeof(line), "%s %s", model->sync_time_hhmm, model->sync_date_mmdd);
    draw_text_utf8(frame_buffer, line, 34, 48);

    snprintf(line, sizeof(line), "今日光照：%s", light_level_text(model->light_level));
    draw_text_utf8(frame_buffer, line, 34, 101);

    snprintf(line, sizeof(line), "建议：%s", advice_text(model->light_level));
    draw_text_utf8(frame_buffer, line, 34, 133);

    if (model->environment_valid) {
        snprintf(line, sizeof(line), "卧室温度：%.1f ℃", model->temperature_c);
    } else {
        snprintf(line, sizeof(line), "卧室温度：--.- ℃");
    }
    draw_text_utf8(frame_buffer, line, 34, 190);

    if (model->environment_valid) {
        snprintf(line, sizeof(line), "卧室湿度：%.0f %%", model->humidity_pct);
    } else {
        snprintf(line, sizeof(line), "卧室湿度：-- %%");
    }
    draw_text_utf8(frame_buffer, line, 34, 222);

    snprintf(line, sizeof(line), "睡眠模式：%s", model->sleep_mode_enabled ? "已开启" : "未开启");
    draw_text_utf8(frame_buffer, line, 34, 254);
}
