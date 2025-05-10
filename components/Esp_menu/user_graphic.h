#ifndef USER_GRAPHICS_H
#define USER_GRAPHICS_H
#include "lvgl.h"
static const uint8_t user_icon_data_array[] = {0xFF, 0x00, 0xFF, 0x00};
const lv_image_dsc_t user_icon_data = {
    .header = {.magic = LV_IMAGE_HEADER_MAGIC, .w = 8, .h = 8, .cf = LV_COLOR_FORMAT_MONO1},
    .data = user_icon_data_array,
    .data_size = sizeof(user_icon_data_array)
};
#endif