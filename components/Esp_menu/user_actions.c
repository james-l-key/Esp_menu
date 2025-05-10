/**
 * @file user_actions.c
 * @brief Implementation of user-defined menu action callbacks
 *
 * This file contains the implementations of callback functions that are triggered
 * when menu items are selected. Each function corresponds to an "action" field in
 * the menu.json configuration file.
 */

#include "Esp_menu.h"
#include "menu_data.h"
#include "esp_log.h"

/** @brief Tag for logging from user action callbacks */
static const char *TAG = "USER_ACTIONS";

/**
 * @brief Handler for "Option 1" menu selection
 *
 * This function is called when the user selects "Option 1" from the menu.
 * Implement your custom logic here to respond to this menu selection.
 */
void option1_action(void)
{
    ESP_LOGI(TAG, "Option 1 selected!");
    // Implement your logic here
}