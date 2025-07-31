// Copyright 2025 james-l-key
#include "esp_menu.h"
#include "button_gpio.h"
#include "button_types.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "iot_button.h"
#include "iot_knob.h"
#include "menu_data.h"
#include "sdkconfig.h"
#include <stdint.h>
#ifdef CONFIG_ESPMENU_ENABLE_NVS
#include "nvs.h"
#include "nvs_flash.h"
#include "user_actions.h"
#endif

/** @brief Logging tag for ESP Menu component. */
#define TAG "Esp_menu"

// Event logging for encoder button presses (C-style callbacks)
static void encoder_button_press_down_cb(void *btn, void *usr_data) {
  ESP_LOGI(TAG, "Encoder button %d pressed", (int)(intptr_t)usr_data);
}

static void encoder_button_press_up_cb(void *btn, void *usr_data) {
  ESP_LOGI(TAG, "Encoder button %d released", (int)(intptr_t)usr_data);
}

// Debug callbacks for rotary encoder rotation
static void encoder_clockwise_cb(void *knob, void *usr_data) {
  int idx = (int)(intptr_t)usr_data;
  ESP_LOGI(TAG, "Encoder %d rotated CLOCKWISE", idx);
}

static void encoder_anticlockwise_cb(void *knob, void *usr_data) {
  int idx = (int)(intptr_t)usr_data;
  ESP_LOGI(TAG, "Encoder %d rotated ANTICLOCKWISE", idx);
}
/**
 * @file esp_menu.c
 * @brief Implementation of the ESP Menu component for initializing the menu
 * system with OLED display and rotary encoders.
 */

#include "button_gpio.h"
#include "button_types.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "esp_menu.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iot_button.h"
#include "iot_knob.h"
#include "menu_data.h"
#include "sdkconfig.h"
#ifdef CONFIG_ESPMENU_ENABLE_NVS
#include "nvs.h"
#include "nvs_flash.h"
#include "user_actions.h"
#endif

/** @brief Logging tag for ESP Menu component. */
#define TAG "Esp_menu"

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
                                             .bits_per_pixel = 1};

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
      {CONFIG_ESPMENU_ROTARY_ENCODER_1_A, CONFIG_ESPMENU_ROTARY_ENCODER_1_B,
       CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON},
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_2) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
      {CONFIG_ESPMENU_ROTARY_ENCODER_2_A, CONFIG_ESPMENU_ROTARY_ENCODER_2_B,
       CONFIG_ESPMENU_ROTARY_ENCODER_2_BUTTON},
#endif
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3) ||                            \
    defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
      {CONFIG_ESPMENU_ROTARY_ENCODER_3_A, CONFIG_ESPMENU_ROTARY_ENCODER_3_B,
       CONFIG_ESPMENU_ROTARY_ENCODER_3_BUTTON},
#endif
#if defined(CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4)
      {CONFIG_ESPMENU_ROTARY_ENCODER_4_A, CONFIG_ESPMENU_ROTARY_ENCODER_4_B,
       CONFIG_ESPMENU_ROTARY_ENCODER_4_BUTTON},
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
      .timer_period_ms = 5};
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
      .rotation = {.swap_xy = false, .mirror_x = false, .mirror_y = false}};
  lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
  if (!disp) {
    ESP_LOGE(TAG, "Failed to add display to LVGL");
    return ESP_FAIL;
  }

  // Focus the screen object once
  lv_group_focus_obj(lv_scr_act());

  // Array to store knob handles for each encoder
  knob_handle_t knob_handles[4] = {NULL};

  for (int i = 0; i < encoder_count; i++) {
    ESP_LOGI(TAG, "Creating knob %d (A:%d B:%d)", i, encoder_pins[i][0],
             encoder_pins[i][1]);

    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = encoder_pins[i][0],
        .gpio_encoder_b = encoder_pins[i][1],
        .enable_power_save = false,
    };

    knob_handle_t knob = iot_knob_create(&knob_cfg);
    knob_handles[i] = knob;

    if (!knob) {
      ESP_LOGE(TAG, "Failed to create knob %d", i);
      return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Knob %d handle: %p", i, (void *)knob);
  }

  // Resume knob polling (required for event detection)
  iot_knob_resume();

  // Register rotation callbacks for each encoder knob
  for (int i = 0; i < encoder_count; i++) {
    knob_handle_t knob = knob_handles[i];
    if (knob) {
      iot_knob_register_cb(knob, KNOB_RIGHT, encoder_clockwise_cb,
                           (void *)(intptr_t)i);
      iot_knob_register_cb(knob, KNOB_LEFT, encoder_anticlockwise_cb,
                           (void *)(intptr_t)i);
    }
  }

  for (int i = 0; i < encoder_count; i++) {
    button_handle_t btn = encoder_btn_handles[i];
    if (btn) {
      iot_button_register_cb(btn, BUTTON_PRESS_DOWN, NULL,
                             encoder_button_press_down_cb, (void *)(intptr_t)i);
      iot_button_register_cb(btn, BUTTON_PRESS_UP, NULL,
                             encoder_button_press_up_cb, (void *)(intptr_t)i);
    }
  }

  // Register rotation callbacks for each encoder knob (debug)
  // If you have knob handles, register the callbacks:
  // TODO: Replace with actual knob handle retrieval if available
  // Example:
  // for (int i = 0; i < encoder_count; i++) {
  //   knob_handle_t knob = ...; // get knob handle for encoder i
  //   if (knob) {
  //     iot_knob_register_cb(knob, KNOB_EVENT_CLOCKWISE, encoder_clockwise_cb,
  //     (void *)(intptr_t)i); iot_knob_register_cb(knob,
  //     KNOB_EVENT_ANTICLOCKWISE, encoder_anticlockwise_cb, (void
  //     *)(intptr_t)i);
  //   }
  // }

  // Initialize menu widgets
  ESP_LOGI(TAG, "Initializing LVGL menu widgets");
  menu_init();

  ESP_LOGI(TAG, "Menu system fully initialized");
  return ESP_OK;
}