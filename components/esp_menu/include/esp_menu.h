/**
 * @file esp_menu.h
 * @brief Header file for the ESP Menu component, providing menu system initialization and NVS functions.
 */

#ifndef ESP_MENU_H
#define ESP_MENU_H

#include "esp_err.h"

/**
 * @brief Initializes the ESP Menu system, including OLED display and rotary encoders.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_init(void);

#endif