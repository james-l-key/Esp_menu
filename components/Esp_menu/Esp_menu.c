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
#include "test_config.h"
#endif

/** @brief Global handle for the initialized LCD panel. */
esp_lcd_panel_handle_t lcd_handle = NULL;

/**
 * @brief Main application entry point.
 */
void app_main(void)
{
    ESP_LOGI(TAG, "Starting menu system initialization");

    // Initialize I2C bus
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = CONFIG_ESPMENU_I2C_HOST,
        .sda_io_num = CONFIG_ESPMENU_SSD1306_I2C_SDA,
        .scl_io_num = CONFIG_ESPMENU_SSD1306_I2C_SCL,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    // Initialize LCD panel IO (SSD1306)
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = CONFIG_ESPMENU_SSD1306_I2C_ADDRESS,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6};
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_bus, &io_config, &io_handle));

    // Initialize LCD panel driver (SSD1306)
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1,
        .flags = {
            .reset_active_high = 0,
        },
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    lcd_handle = panel_handle;

    // Configure GPIO for Rotary Encoder
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_A) |
                        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_B) |
                        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE};
    gpio_config(&io_conf);

    // Initialize Navigation Buttons
    button_handle_t prev_btn_handle = NULL;
    button_handle_t next_btn_handle = NULL;
    button_handle_t enter_btn_handle = NULL;
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
    };
    button_gpio_config_t bsp_button_config[] = {
        {.gpio_num = GPIO_NUM_37, .active_level = 0},
        {.gpio_num = GPIO_NUM_38, .active_level = 0},
        {.gpio_num = GPIO_NUM_39, .active_level = 0},
    };
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[0], &prev_btn_handle));
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[1], &next_btn_handle));
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &bsp_button_config[2], &enter_btn_handle));

    // Add Navigation Buttons to LVGL
    lv_disp_t *disp_handle = lvgl_port_get_default_display();
    if (!disp_handle)
    {
        ESP_LOGE(TAG, "LVGL Display not initialized!");
        return;
    }
    lvgl_port_nav_btns_cfg_t btns = {
        .disp = disp_handle,
        .button_prev = prev_btn_handle,
        .button_next = next_btn_handle,
        .button_enter = enter_btn_handle};
    lv_indev_t *buttons_indev = lvgl_port_add_navigation_buttons(&btns);
    if (!buttons_indev)
    {
        ESP_LOGE(TAG, "Failed to add navigation buttons to LVGL");
        return;
    }

    // Initialize Rotary Encoder
    button_handle_t encoder_btn_handle = NULL;
    knob_handle_t encoder_knob_handle = NULL;
    button_gpio_config_t encoder_btn_config = {
        .gpio_num = CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON,
        .active_level = 0,
    };
    ESP_ERROR_CHECK(iot_button_new_gpio(&btn_cfg, &encoder_btn_config, &encoder_btn_handle));
    knob_config_t encoder_knob_cfg = {
        .type = KNOB_TYPE_GPIO,
        .num_positions = 0,
        .gpio_encoder_a = CONFIG_ESPMENU_ROTARY_ENCODER_1_A,
        .gpio_encoder_b = CONFIG_ESPMENU_ROTARY_ENCODER_1_B,
    };
    ESP_ERROR_CHECK(iot_knob_new_gpio(&encoder_knob_cfg, &encoder_knob_handle));

    // Add Rotary Encoder to LVGL
    lvgl_port_encoder_cfg_t encoder_lvgl_cfg = {
        .disp = disp_handle,
        .encoder_enter = encoder_btn_handle,
        .encoder_a = encoder_knob_handle,
        .encoder_b = NULL};
    lv_indev_t *encoder_indev = lvgl_port_add_encoder(&encoder_lvgl_cfg);
    if (!encoder_indev)
    {
        ESP_LOGE(TAG, "Failed to add encoder to LVGL");
        return;
    }

    // Call the generated menu_init to set up LVGL widgets
    ESP_LOGI(TAG, "Initializing LVGL menu widgets");
    menu_init(); // From menu.c

    ESP_LOGI(TAG, "Menu system fully initialized");
}