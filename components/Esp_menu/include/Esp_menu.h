/**
 * @file Esp_menu.h
 * @brief Header file for the ESP Menu component, providing menu system initialization and NVS functions.
 */

#ifndef ESP_MENU_H
#define ESP_MENU_H

#include "esp_err.h"

/**
 * @brief Initializes the ESP Menu system, including SSD1306 display and rotary encoders.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_init(void);

#ifdef CONFIG_ESPMENU_ENABLE_NVS
/**
 * @brief Saves menu parameters to Non-Volatile Storage (NVS).
 */
void save_to_nvs(void);

/**
 * @brief Loads menu parameters from Non-Volatile Storage (NVS).
 */
void load_from_nvs(void);
#endif

#endif