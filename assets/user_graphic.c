/**
 * @file user_graphic.c
 * @brief Implementation of user-defined LVGL graphics for the oscillator module.
 */
#include "user_graphic.h"

// Provide tiny 12x12 1-bit placeholder icons so the demo links and runs.
// LVGL v9 image descriptor uses lv_image_dsc_t and lv_color_format_t from lv_color.h

#define ICON_W 12
#define ICON_H 12

// Simple bitmaps: each row is 12 pixels packed into 2 bytes (16 bits used, upper 4 bits ignored)
static const uint8_t s_icon_bits_pitch[] = {
    0xFF,0x0F, 0x81,0x08, 0xBD,0x0B, 0xA5,0x0A, 0xA5,0x0A, 0xBD,0x0B,
    0x81,0x08, 0xFF,0x0F, 0x00,0x00, 0x70,0x07, 0x50,0x05, 0x70,0x07,
};
static const uint8_t s_icon_bits_waveform[] = {
    0x01,0x00, 0x03,0x00, 0x07,0x00, 0x0F,0x00, 0x1F,0x00, 0x3F,0x00,
    0x1F,0x00, 0x0F,0x00, 0x07,0x00, 0x03,0x00, 0x01,0x00, 0x00,0x00,
};
static const uint8_t s_icon_bits_next[] = {
    0x10,0x00, 0x30,0x00, 0x70,0x00, 0xF0,0x00, 0x70,0x00, 0x30,0x00,
    0x10,0x00, 0x00,0x00, 0x10,0x00, 0x30,0x00, 0x10,0x00, 0x00,0x00,
};
static const uint8_t s_icon_bits_prev[] = {
    0x01,0x00, 0x03,0x00, 0x07,0x00, 0x0F,0x00, 0x07,0x00, 0x03,0x00,
    0x01,0x00, 0x00,0x00, 0x01,0x00, 0x03,0x00, 0x01,0x00, 0x00,0x00,
};

// Each row stride in bytes for LV_COLOR_FORMAT_I1 is ceil(width/8). For 12px -> 2 bytes per row.
#define ROW_STRIDE 2

const lv_image_dsc_t pitch_icon_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_I1,
        .flags = 0,
        .w = ICON_W,
        .h = ICON_H,
        .stride = ROW_STRIDE,
        .reserved_2 = 0,
    },
    .data_size = ICON_H * ROW_STRIDE,
    .data = s_icon_bits_pitch,
    .reserved = NULL,
    .reserved_2 = NULL,
};

const lv_image_dsc_t waveform_icon_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_I1,
        .flags = 0,
        .w = ICON_W,
        .h = ICON_H,
        .stride = ROW_STRIDE,
        .reserved_2 = 0,
    },
    .data_size = ICON_H * ROW_STRIDE,
    .data = s_icon_bits_waveform,
    .reserved = NULL,
    .reserved_2 = NULL,
};

const lv_image_dsc_t next_icon_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_I1,
        .flags = 0,
        .w = ICON_W,
        .h = ICON_H,
        .stride = ROW_STRIDE,
        .reserved_2 = 0,
    },
    .data_size = ICON_H * ROW_STRIDE,
    .data = s_icon_bits_next,
    .reserved = NULL,
    .reserved_2 = NULL,
};

const lv_image_dsc_t prev_icon_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_I1,
        .flags = 0,
        .w = ICON_W,
        .h = ICON_H,
        .stride = ROW_STRIDE,
        .reserved_2 = 0,
    },
    .data_size = ICON_H * ROW_STRIDE,
    .data = s_icon_bits_prev,
    .reserved = NULL,
    .reserved_2 = NULL,
};

void user_graphic_init(lv_obj_t *parent)
{
    LV_UNUSED(parent);
    // Optional: Add screen-level graphics (e.g., background image)
}