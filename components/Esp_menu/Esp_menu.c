/**
 * @file Esp_menu.c
 * @brief Implementation file for the ESP Menu system.
 *
 * This file contains the core logic for initializing and managing the menu system,
 * including hardware setup (LCD display, I2C, GPIO for encoder/buttons) and
 * integration with the LVGL library for the user interface.
 */

#include "Esp_menu.h"  // Public interface for the menu system
#include "menu_data.h" // Auto-generated menu structure data
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h" // Generated configuration header (Kconfig)
#include "esp_lcd_panel_io.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h" // Specific LCD vendor drivers (like SSD1306)
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "button_gpio.h" // GPIO button component
#include "iot_button.h"  // Abstracted button component
#include "knob_gpio.h"   // GPIO knob (encoder) component
#include "iot_knob.h"    // Abstracted knob (encoder) component

/** @brief Logging tag for the Esp_menu component. */
#define TAG "Esp_menu"

/**
 * @brief Error checking macro specific to this component.
 * Checks the return code of an ESP-IDF function. If the code is not ESP_OK,
 * it logs an error message including the error name and line number,
 * and returns NULL. Useful within functions that should return a handle or pointer.
 * @param x The function call to check. Must return an esp_err_t.
 */
#define BSP_ERROR_CHECK_RETURN_NULL(x)                                               \
    do                                                                               \
    {                                                                                \
        esp_err_t err_rc = (x);                                                      \
        if (err_rc != ESP_OK)                                                        \
        {                                                                            \
            ESP_LOGE(TAG, "Error %s at line %d", esp_err_to_name(err_rc), __LINE__); \
            return NULL;                                                             \
        }                                                                            \
    } while (0)

#ifdef UNIT_TEST
#include "test_config.h" // Include test configuration overrides if UNIT_TEST is defined
#endif

/** @brief Global handle for the initialized LCD panel. NULL if not initialized. */
esp_lcd_panel_handle_t lcd_handle = NULL;

/**
 * @brief Initializes the menu system hardware and LVGL integration.
 *
 * This function performs the following initializations:
 * 1. Initializes the I2C master bus specified in Kconfig.
 * 2. Creates an I2C panel IO handle for the SSD1306 display.
 * 3. Creates and initializes the SSD1306 LCD panel driver.
 * 4. Configures GPIO pins for the rotary encoder (A, B, Button) with pull-ups enabled.
 * 5. Calls `i2c_communication_init()` (defined elsewhere) for inter-module communication setup.
 * 6. Initializes GPIO buttons for navigation (Previous, Next, Enter) using iot_button component.
 * 7. Adds the navigation buttons to the LVGL port.
 * 8. Initializes the rotary encoder button using iot_button component.
 * 9. Adds the rotary encoder (including its button) to the LVGL port.
 *
 * @note This function relies heavily on settings defined in Kconfig (menuconfig)
 *       for I2C pins, display type/address/size, and encoder/button GPIOs.
 * @note Error checking is performed using ESP_ERROR_CHECK, which will abort on failure.
 */
void menu_init()
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_LOGI(TAG, "Initialize I2C bus for display");
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = CONFIG_ESPMENU_I2C_HOST,          // Use Kconfig value
        .sda_io_num = CONFIG_ESPMENU_SSD1306_I2C_SDA, // Use Kconfig value
        .scl_io_num = CONFIG_ESPMENU_SSD1306_I2C_SCL, // Use Kconfig value
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    ESP_LOGI(TAG, "Initialize LCD panel IO (SSD1306)");
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = CONFIG_ESPMENU_SSD1306_I2C_ADDRESS, // Use Kconfig value
        .control_phase_bytes = 1,                       // Specific to SSD1306
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6 // Specific to SSD1306 I2C protocol
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Initialize LCD panel driver (SSD1306)");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1, // No reset pin used
        .flags = {
            .reset_active_high = 0,
        },
        // Note: SSD1306 specific config like height might be needed depending on the driver version
        // esp_lcd_panel_ssd1306_config_t ssd1306_config = { ... }; // If required by esp_lcd_new_panel_ssd1306
    };
    // Assuming esp_lcd_new_panel_ssd1306 exists and takes these arguments.
    // The exact function name might vary slightly based on ESP-IDF version or specific SSD1306 driver.
    // If a dedicated ssd1306 config struct is needed, it should be passed here.
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    lcd_handle = panel_handle; // Store the handle globally

    ESP_LOGI(TAG, "Configure GPIO for Rotary Encoder");
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, // Interrupt on any edge change for encoder pins
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_A) |     // Use Kconfig value
        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_B) |     // Use Kconfig value
        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON), // Use Kconfig value
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE // Enable pull-ups for encoder/button
    };
    gpio_config(&io_conf);

    // Note: Actual ISR setup for the encoder pins (A/B) seems missing here.
    // The encoder_isr_handler is defined but not attached to GPIO interrupts.
    // This might be handled within iot_knob or needs explicit setup.

    ESP_LOGI(TAG, "Initialize Inter-Module I2C Communication");
    i2c_communication_init(); // Init the separate I2C bus/functions for module communication

    ESP_LOGI(TAG, "Initialize Navigation Buttons");
    button_handle_t prev_btn_handle = NULL;
    button_handle_t next_btn_handle = NULL;
    button_handle_t enter_btn_handle = NULL;

    // Default button configuration (can be customized if needed)
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        // .long_press_time = CONFIG_BUTTON_LONG_PRESS_TIME_MS, // Example customization
        // .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS, // Example customization
    };

    // Assuming GPIOs 37, 38, 39 are used for Prev, Next, Enter respectively.
    // These should ideally be defined via Kconfig as well.
    button_gpio_config_t bsp_button_config[] = {
        {.gpio_num = GPIO_NUM_37, .active_level = 0}, // Active low button
        {.gpio_num = GPIO_NUM_38, .active_level = 0}, // Active low button
        {.gpio_num = GPIO_NUM_39, .active_level = 0}, // Active low button
    };

    // Create button instances using the iot_button component
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[0], &prev_btn_handle));
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[1], &next_btn_handle));
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[2], &enter_btn_handle));

    ESP_LOGI(TAG, "Add Navigation Buttons to LVGL");
    lv_disp_t *disp_handle = lv_disp_get_default(); // Get the default display from LVGL port
    if (!disp_handle)
    {
        ESP_LOGE(TAG, "LVGL Display not initialized!");
        // Handle error appropriately, maybe return or abort
        return;
    }

    lvgl_port_nav_btns_cfg_t btns = {
        .disp = disp_handle,
        .button_prev = prev_btn_handle,
        .button_next = next_btn_handle,
        .button_enter = enter_btn_handle
    };
    lv_indev_t *buttons_indev = lvgl_port_add_navigation_buttons(&btns);
    if (!buttons_indev)
    {
        ESP_LOGE(TAG, "Failed to add navigation buttons to LVGL");
        // Handle error, maybe clean up created buttons
        return;
    }
    // (void)buttons_indev; // Suppress unused variable warning if handle isn't used later

    ESP_LOGI(TAG, "Initialize Rotary Encoder Input");
    button_handle_t encoder_btn_handle = NULL;
    knob_handle_t encoder_knob_handle = NULL; // Handle for the knob itself

    // Configuration for the encoder's integrated button
    button_gpio_config_t encoder_btn_config = {
        .gpio_num = CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON, // Use Kconfig value
        .active_level = 0,                                  // Active low
    };
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &encoder_btn_config, &encoder_btn_handle));

    // Configuration for the encoder's rotation pins
    knob_config_t encoder_knob_cfg = {
        .type = KNOB_TYPE_GPIO,                              // Specify GPIO type
        .num_positions = 0,                                  // 0 for continuous rotation
        .gpio_encoder_a = CONFIG_ESPMENU_ROTARY_ENCODER_1_A, // Use Kconfig value
        .gpio_encoder_b = CONFIG_ESPMENU_ROTARY_ENCODER_1_B, // Use Kconfig value
    };
    ESP_ERROR_CHECK(iot_knob_new_gpio(&encoder_knob_cfg, &encoder_knob_handle));

    ESP_LOGI(TAG, "Add Rotary Encoder to LVGL");
    lvgl_port_encoder_cfg_t encoder_lvgl_cfg = {
        .disp = disp_handle,
        .encoder_enter = encoder_btn_handle, // Assign the button handle
        .encoder_a = encoder_knob_handle,    // Assign the knob handle
        .encoder_b = NULL                    // Typically NULL when using iot_knob which handles A/B internally
    };

    lv_indev_t *encoder_indev = lvgl_port_add_encoder(&encoder_lvgl_cfg);
    if (!encoder_indev)
    {
        ESP_LOGE(TAG, "Failed to add encoder to LVGL");
        // Handle error, maybe clean up created encoder/button
        return;
    }
    // (void)encoder_indev; // Suppress unused variable warning

    ESP_LOGI(TAG, "Menu Initialization Complete");
}

/**
 * @brief Interrupt Service Routine (ISR) handler for the rotary encoder.
 *
 * This function is intended to be called when an interrupt occurs on the
 * encoder's GPIO pins (likely A or B). It should read the encoder state,
 * determine the direction of rotation, and update the menu state accordingly
 * (e.g., change selected item, modify parameter value).
 *
 * If changing a parameter requires communication with another module, it should
 * use the I2C communication functions (e.g., `i2c_write_bytes`).
 *
 * @param arg Generic argument pointer passed from the ISR dispatcher (often unused).
 *
 * @warning The actual logic for reading the encoder and updating the menu
 *          is currently missing in this placeholder implementation.
 * @warning This ISR needs to be registered with the GPIO interrupt system
 *          during initialization (which seems missing in `menu_init`).
 * @warning ISRs should be fast and avoid blocking operations or complex logic.
 *          Consider deferring complex updates to a task using queues or semaphores.
 */
void encoder_isr_handler(void *arg)
{
    // TODO: Implement encoder rotation detection logic here.
    // - Read GPIO states for A and B.
    // - Determine direction (CW/CCW).
    // - Update menu selection or parameter value.
    // - Potentially signal a task to handle complex updates or I2C communication.
    // Example placeholder:
    // int direction = read_encoder_direction();
    // if (direction != 0) {
    //    update_menu_state(direction);
    //    // If needed:
    //    // trigger_i2c_update(parameter_index, new_value);
    // }

    // Example I2C call (replace with actual parameters):
    // uint8_t data_to_send[] = { register_addr, value };
    // i2c_write_bytes(MODULE_ADDRESS, data_to_send, sizeof(data_to_send));
}

/**
 * @brief Enumeration defining the types of input devices that can be cleaned up.
 */
typedef enum
{
    CLEANUP_BUTTONS, /**< Represents the navigation button input device. */
    CLEANUP_ENCODER  /**< Represents the encoder input device. */
} menu_cleanup_type_t;

/**
 * @brief Cleans up (removes) an LVGL input device.
 *
 * This function removes either the navigation buttons or the encoder
 * from the LVGL input device handling, freeing associated resources
 * within the LVGL port layer.
 *
 * @param handle The LVGL input device handle (`lv_indev_t*`) to remove.
 *               This handle should have been returned by the corresponding
 *               `lvgl_port_add_...` function during initialization.
 * @param type Specifies whether to remove the buttons or the encoder.
 */
void menu_cleanup(lv_indev_t *handle, menu_cleanup_type_t type)
{
    if (handle)
    {
        switch (type)
        {
        case CLEANUP_BUTTONS:
            ESP_LOGI(TAG, "Cleaning up navigation buttons LVGL input device");
            lvgl_port_remove_navigation_buttons(handle);
            break;
        case CLEANUP_ENCODER:
            ESP_LOGI(TAG, "Cleaning up encoder LVGL input device");
            lvgl_port_remove_encoder(handle);
            break;
        default:
            ESP_LOGW(TAG, "Unknown cleanup type: %d", type);
            break;
        }
    }
    else
    {
        ESP_LOGW(TAG, "Attempted to cleanup NULL input device handle (type: %d)", type);
    }
}

// Note: The Esp_menu_draw() function declared in Esp_menu.h is not implemented here.
// It would typically contain the LVGL UI drawing logic (creating screens, widgets, etc.)
// based on the menu_data structure and current state.
