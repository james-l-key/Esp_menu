// Copyright 2025 james-l-key
#include "esp_menu.h"
#include "button_gpio.h"
#include "button_types.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_lvgl_port_knob.h"
#include "esp_timer.h"
#include "iot_button.h"
#include "iot_knob.h"
#include "lvgl.h"
#include "menu_data.h" // Generated menu system
#include "nvs.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <stdint.h>
#include <string.h>
#ifdef CONFIG_ESPMENU_ENABLE_NVS
#include "user_actions.h"
#endif

/** @brief Logging tag for ESP Menu component. */
#define TAG "Esp_menu"

// Forward declarations for menu support

// Note: Encoder callbacks removed - using esp_lvgl_port_knob for proper LVGL
// integration

/** @brief Macro to check ESP error codes and return on failure with logging. */
#define BSP_ERROR_CHECK_RETURN_ERR(x)                                          \
  do {                                                                         \
    esp_err_t err_rc = (x);                                                    \
    if (err_rc != ESP_OK) {                                                    \
      ESP_LOGE(TAG, "Error %s at line %d", esp_err_to_name(err_rc), __LINE__); \
      return err_rc;                                                           \
    }                                                                          \
  } while (0)

/** @brief Handle for the LCD panel. */
static esp_lcd_panel_handle_t lcd_handle = NULL;

/**
 * @brief Initializes the ESP Menu system, including I2C, OLED display, rotary
 * encoders, and LVGL.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_menu_init(void) {
    ESP_LOGI(TAG, "Starting menu system initialization");

#ifdef CONFIG_ESPMENU_ENABLE_NVS
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
            ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_LOGI(TAG, "Erasing NVS partition...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        // Retry initialization
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized successfully");

    // Initialize menu parameters
    init_menu_params();
#endif

    // Initialize I2C master bus for OLED display
    ESP_LOGI(TAG, "Initializing I2C: SDA=%d, SCL=%d, Host=%d, Address=0x%02X",
             CONFIG_ESPMENU_DISPLAY_I2C_SDA, CONFIG_ESPMENU_DISPLAY_I2C_SCL,
             CONFIG_ESPMENU_I2C_HOST, CONFIG_ESPMENU_DISPLAY_I2C_ADDRESS);

    // Validate I2C host number for ESP32-S3
    int i2c_port = CONFIG_ESPMENU_I2C_HOST;
    if (i2c_port < 0 || i2c_port >= I2C_NUM_MAX) {
        ESP_LOGW(TAG, "Invalid I2C host %d from config, using I2C_NUM_0", i2c_port);
        i2c_port = I2C_NUM_0;
    }

    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = i2c_port,
        .sda_io_num = CONFIG_ESPMENU_DISPLAY_I2C_SDA,
        .scl_io_num = CONFIG_ESPMENU_DISPLAY_I2C_SCL,
        .flags.enable_internal_pullup = true,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&bus_config, &i2c_bus));
    ESP_LOGI(TAG, "I2C master bus created on port %d", i2c_port);

    // Initialize LCD panel I/O
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = CONFIG_ESPMENU_DISPLAY_I2C_ADDRESS,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
        .scl_speed_hz =
        400000, // Add I2C clock frequency (400kHz standard for OLED)
    };
    BSP_ERROR_CHECK_RETURN_ERR(
        esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    // Initialize OLED panel
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {.reset_gpio_num = -1,
                                               .bits_per_pixel = 1
                                              };

#ifdef CONFIG_ESPMENU_DISPLAY_SSD1306
    BSP_ERROR_CHECK_RETURN_ERR(
        esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
#elif defined(CONFIG_ESPMENU_DISPLAY_SH1107)
    // Note: SH1107 support would require external component or custom
    //
    // implementation
    ESP_LOGW(TAG, "SH1107 display not yet supported in ESP-IDF. Using SSD1306 "
                  "compatibility mode.");
    BSP_ERROR_CHECK_RETURN_ERR(
        esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
#else
#error "No display type selected in menuconfig"
#endif
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_panel_reset(panel_handle));
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_panel_init(panel_handle));

    // Turn on the display - SSD1306 needs explicit activation
    BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_LOGI(TAG, "Display turned on");

    // Set display contrast to maximum (for SSD1306)
#if defined(CONFIG_ESPMENU_DISPLAY_SSD1306)
    // Set contrast to max (0xFF) using the public panel I/O API
    uint8_t contrast_value = 0xFF;
    esp_err_t err =
        esp_lcd_panel_io_tx_param(io_handle, 0x81, &contrast_value, 1);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set contrast: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "Display contrast set to maximum");
    }
#endif

    lcd_handle = panel_handle;

    ESP_LOGI(TAG, "Display size: %dx%d", 128, CONFIG_ESPMENU_DISPLAY_HEIGHT);

// Define number of encoders based on config
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
    const int encoder_count = 4;
#elif defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3)
    const int encoder_count = 3;
#elif defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_2)
    const int encoder_count = 2;
#else
    const int encoder_count = 1;
#endif

    ESP_LOGI(TAG, "Configured encoder count: %d", encoder_count);

#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_1)
    ESP_LOGI(TAG, "CONFIG_ESPMENU_ROTARY_ENCODER_CNT_1 is defined");
#endif

    // Define available encoder pins
    const int encoder_pins[][3] = {
        {   CONFIG_ESPMENU_ROTARY_ENCODER_1_A, CONFIG_ESPMENU_ROTARY_ENCODER_1_B,
            CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON
        },
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_2) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
        {   CONFIG_ESPMENU_ROTARY_ENCODER_2_A, CONFIG_ESPMENU_ROTARY_ENCODER_2_B,
            CONFIG_ESPMENU_ROTARY_ENCODER_2_BUTTON
        },
#endif
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
        {   CONFIG_ESPMENU_ROTARY_ENCODER_3_A, CONFIG_ESPMENU_ROTARY_ENCODER_3_B,
            CONFIG_ESPMENU_ROTARY_ENCODER_3_BUTTON
        },
#endif
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
        {   CONFIG_ESPMENU_ROTARY_ENCODER_4_A, CONFIG_ESPMENU_ROTARY_ENCODER_4_B,
            CONFIG_ESPMENU_ROTARY_ENCODER_4_BUTTON
        },
#endif
    };

    // Initialize rotary encoders and buttons
    button_handle_t encoder_btn_handles[4] = {NULL};

    // Initialize each encoder button
    for (int i = 0; i < encoder_count; i++) {
        // Create button
        button_config_t btn_cfg = {
            .long_press_time = 0,
            .short_press_time = 0,
        };

        // Configure GPIO button properties
        button_gpio_config_t gpio_btn_cfg = {
            .gpio_num = encoder_pins[i][2],
            .active_level = 0, // Assuming active low buttons
            .enable_power_save = false,
            .disable_pull = false,
        };

        ESP_LOGI(TAG, "Creating button on GPIO %d", encoder_pins[i][2]);
        BSP_ERROR_CHECK_RETURN_ERR(iot_button_new_gpio_device(
                                       &btn_cfg, &gpio_btn_cfg, &encoder_btn_handles[i]));
    }

    ESP_LOGI(TAG, "Button 0 created, knob will be created by LVGL port");

    // Initialize LVGL
    lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 5,
        .task_stack = 8192, // Increased from 4096 to prevent stack overflow
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    BSP_ERROR_CHECK_RETURN_ERR(lvgl_port_init(&lvgl_cfg));

    ESP_LOGI(TAG, "Starting LVGL task");

    // Add display to LVGL
    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = 128 * CONFIG_ESPMENU_DISPLAY_HEIGHT,
        .double_buffer = true,
        .hres = 128,
        .vres = CONFIG_ESPMENU_DISPLAY_HEIGHT,
        .monochrome = true,
        .rotation = {.swap_xy = false, .mirror_x = false, .mirror_y = false}
    };
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
    if (!disp) {
        ESP_LOGE(TAG, "Failed to add display to LVGL");
        return ESP_FAIL;
    }

    // Focus the screen object once
    lv_group_focus_obj(lv_scr_act());

    // Register encoder using LVGL port for proper integration
    for (int i = 0; i < encoder_count; i++) {
        ESP_LOGI(TAG, "Setting up encoder %d (A:%d B:%d Button:%d)", i,
                 encoder_pins[i][0], encoder_pins[i][1], encoder_pins[i][2]);

        // Configure knob for LVGL port
        knob_config_t knob_cfg = {
            .default_direction = 0,
            .gpio_encoder_a = encoder_pins[i][0],
            .gpio_encoder_b = encoder_pins[i][1],
            .enable_power_save = false,
        };

        // Configure LVGL port encoder
        lvgl_port_encoder_cfg_t encoder_cfg = {
            .disp = disp,
            .encoder_a_b = &knob_cfg,
            .encoder_enter =
            encoder_btn_handles[i], // Use the button handle we created earlier
        };

        lv_indev_t *encoder_indev = lvgl_port_add_encoder(&encoder_cfg);
        if (encoder_indev) {
            ESP_LOGI(TAG, "Encoder %d registered with LVGL successfully", i);
        } else {
            ESP_LOGE(TAG, "Failed to register encoder %d with LVGL", i);
            return ESP_FAIL;
        }

        // Only register the first encoder for now (most common case)
        // Multiple encoders would need additional handling
        if (i == 0) {
            break;
        }
    }

    // Initialize menu widgets
    ESP_LOGI(TAG, "Initializing generated LVGL menu system");
    menu_init(); // Use generated menu initialization

    // Fix group focus and activation for encoder navigation
    lvgl_port_lock(0);

    // The generated menu creates a group but doesn't set it as default
    // Find the encoder input device to get its group
    lv_indev_t *encoder_indev = lv_indev_get_next(NULL);
    lv_group_t *encoder_group = NULL;

    while (encoder_indev) {
        if (lv_indev_get_type(encoder_indev) == LV_INDEV_TYPE_ENCODER) {
            encoder_group = lv_indev_get_group(encoder_indev);
            break;
        }
        encoder_indev = lv_indev_get_next(encoder_indev);
    }

    if (encoder_group) {
        ESP_LOGI(TAG, "Found encoder group, setting as default");
        lv_group_set_default(encoder_group);

        // Find the main list and add all its buttons to the group for proper
        // navigation
        lv_obj_t *main_screen = lv_scr_act();
        lv_obj_t *list_main = lv_obj_get_child(main_screen, 0);

        if (list_main) {
            ESP_LOGI(TAG, "Adding list buttons to encoder group");
            uint32_t child_count = lv_obj_get_child_cnt(list_main);
            ESP_LOGI(TAG, "Found %d children in main list", (int)child_count);

            // Add each child to the group (they should be clickable list items)
            for (uint32_t i = 0; i < child_count; i++) {
                lv_obj_t *child = lv_obj_get_child(list_main, i);
                if (child) {
                    // Check if the object can receive input events (is clickable)
                    if (lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE)) {
                        lv_group_add_obj(encoder_group, child);
                        ESP_LOGI(TAG, "Added clickable item %d to encoder group", (int)i);
                    }
                }
            }

            // Focus the first clickable item if available
            if (child_count > 0) {
                lv_obj_t *first_item = lv_obj_get_child(list_main, 0);
                if (first_item && lv_obj_has_flag(first_item, LV_OBJ_FLAG_CLICKABLE)) {
                    lv_group_focus_obj(first_item);
                    ESP_LOGI(TAG, "Set focus to first clickable item");
                }
            }
        }

        // Force layout update and redraw
        lv_obj_update_layout(lv_scr_act());
        lv_obj_invalidate(lv_scr_act());

    } else {
        ESP_LOGW(TAG, "No encoder group found - creating fallback group");
        // Create a fallback group if none exists
        lv_group_t *fallback_group = lv_group_create();
        lv_group_set_default(fallback_group);

        // Find the main screen list and add it to the group
        lv_obj_t *list = lv_obj_get_child(lv_scr_act(), 0);
        if (list) {
            lv_group_add_obj(fallback_group, list);
            lv_group_focus_obj(list);
        }
    }

    lvgl_port_unlock();

        // Improve focus visibility and spacing at runtime (monochrome-friendly)
        lvgl_port_lock(0);
        lv_obj_t *act = lv_scr_act();
        if (act) {
            // Try to find the main list as first child
            lv_obj_t *list = lv_obj_get_child(act, 0);
            if (list) {
                static lv_style_t style_focus_rt;
                static bool style_inited = false;
                if (!style_inited) {
                    style_inited = true;
                    lv_style_init(&style_focus_rt);
                    lv_style_set_bg_opa(&style_focus_rt, LV_OPA_COVER);
                    lv_style_set_bg_color(&style_focus_rt, lv_color_white());
                    lv_style_set_text_color(&style_focus_rt, lv_color_black());
                    lv_style_set_border_width(&style_focus_rt, 2);
                    lv_style_set_border_color(&style_focus_rt, lv_color_black());
                    lv_style_set_outline_width(&style_focus_rt, 1);
                    lv_style_set_outline_color(&style_focus_rt, lv_color_black());
                    lv_style_set_pad_top(&style_focus_rt, 2);
                    lv_style_set_pad_bottom(&style_focus_rt, 2);
                    lv_style_set_pad_left(&style_focus_rt, 4);
                    lv_style_set_pad_right(&style_focus_rt, 4);
                }
                lv_obj_add_style(list, &style_focus_rt, LV_PART_ITEMS | LV_STATE_FOCUSED);

                // Normalize spacing for all items
                uint32_t n = lv_obj_get_child_cnt(list);
                for (uint32_t i = 0; i < n; i++) {
                    lv_obj_t *child = lv_obj_get_child(list, i);
                    if (child && lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE)) {
                        lv_obj_set_style_pad_top(child, 1, 0);
                        lv_obj_set_style_pad_bottom(child, 1, 0);
                        lv_obj_set_style_pad_left(child, 4, 0);
                        lv_obj_set_style_pad_right(child, 4, 0);
                    }
                }
                lv_obj_update_layout(list);
                lv_obj_invalidate(list);
            }
        }
        lvgl_port_unlock();

    ESP_LOGI(TAG, "Menu system fully initialized");
    return ESP_OK;
}