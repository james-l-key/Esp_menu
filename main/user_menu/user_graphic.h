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

#endif