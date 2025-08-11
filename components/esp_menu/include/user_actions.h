// Copyright 2025 james-l-key
/**
 * @file user_actions.h
 * @brief Project-level user parameter API and NVS hooks used by esp_menu.
 *
 * This header declares minimal stubs so esp_menu can build even when the
 * application hasn't provided custom implementations yet.
 */

#ifndef COMPONENTS_ESP_MENU_INCLUDE_USER_ACTIONS_H_
#define COMPONENTS_ESP_MENU_INCLUDE_USER_ACTIONS_H_

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Minimal user parameters structure (expand in your application as needed)
typedef struct menu_params_s {
	int placeholder;  // replace with real parameters
} menu_params_t;

// Global parameters instance (owned by user_actions.c)
extern menu_params_t g_menu_params;

// Initialize parameters to defaults
void init_menu_params(void);

// Optional NVS persistence hooks (no-op stubs by default)
esp_err_t user_save_params_to_nvs(void);
esp_err_t user_load_params_from_nvs(void);

#ifdef __cplusplus
}
#endif

#endif  // COMPONENTS_ESP_MENU_INCLUDE_USER_ACTIONS_H_

