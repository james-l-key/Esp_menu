/**
 * @file main.c
 * @brief Main application entry point demonstrating ESP Menu usage
 *
 * This example shows how to initialize and use the ESP Menu component
 * in an ESP32 application. It initializes the menu system and then
 * enters a loop to handle other application logic.
 */

#include <unistd.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_menu.h"
#include "menu_data.h" // Contains prototypes for menu actions
#include "user_actions.h" // Contains prototypes for user actions
#include "sdkconfig.h"

/** @brief Logging tag for main application */
static const char *TAG = "ESP_MENU_EXAMPLE";

/**
 * @brief Application entry point
 *
 * Initializes the ESP Menu system and then enters a main application loop.
 * The menu operates independently through interrupt handlers and LVGL tasks.
 */
void app_main(void)
{
    // Initialize the ESP Menu system
    ESP_LOGI(TAG, "Initializing ESP Menu system");
    esp_err_t result = esp_menu_init();

    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize menu system: %s", esp_err_to_name(result));
        return;
    }

    ESP_LOGI(TAG, "Menu system initialized successfully");

    // Main application loop
    while (1)
    {
        // Your application logic goes here
        // The menu system operates independently through LVGL tasks

        // Example of regular application work
        ESP_LOGI(TAG, "Application heartbeat...");
        sleep(5); // Sleep for 5 seconds between heartbeats

#ifdef CONFIG_ESPMENU_ENABLE_NVS
        // Optional: Save menu state periodically
        user_save_params_to_nvs();
#endif
    }
}