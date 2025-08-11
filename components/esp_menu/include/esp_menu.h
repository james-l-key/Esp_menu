// Copyright 2025 james-l-key
/**
 * @file esp_menu.h
 * @brief Header file for the ESP Menu component, providing menu system initialization and NVS functions.
 */

#ifndef COMPONENTS_ESP_MENU_INCLUDE_ESP_MENU_H_
#define COMPONENTS_ESP_MENU_INCLUDE_ESP_MENU_H_

#include "esp_err.h"  // NOLINT(build/include_subdir)

/**
 * @brief Initializes the ESP Menu system, including OLED display and rotary encoders.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_init(void);

/**
 * @brief Deinitialize the ESP Menu system and free allocated resources.
 *        Best-effort cleanup to restore heap for unit tests and shutdown.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_deinit(void);

#endif  // COMPONENTS_ESP_MENU_INCLUDE_ESP_MENU_H_
