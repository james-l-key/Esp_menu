/**
 * @file user_graphic.h
 * @brief Header file for user-defined LVGL graphics for the oscillator module.
 */
#ifndef USER_GRAPHIC_H
#define USER_GRAPHIC_H

#include "lvgl.h"

/**
 * @brief Initializes custom LVGL graphics for the project.
 * @param parent The parent LVGL object for the graphics.
 */
void user_graphic_init(lv_obj_t *parent);

// Image declarations
LV_IMG_DECLARE(pitch_icon_dsc);
LV_IMG_DECLARE(waveform_icon_dsc);
LV_IMG_DECLARE(next_icon_dsc);
LV_IMG_DECLARE(prev_icon_dsc);

#endif
