/**
 * @file esp_menu.c
 * @brief Implementation of the ESP Menu component for initializing the menu
 * system with OLED display and rotary encoders.
 */

#include "esp_menu.h"
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
  i2c_master_bus_handle_t i2c_bus = NULL;
  i2c_master_bus_config_t bus_config = {
      .clk_source = I2C_CLK_SRC_DEFAULT,
      .glitch_ignore_cnt = 7,
      .i2c_port = CONFIG_ESPMENU_I2C_HOST,
      .sda_io_num = CONFIG_ESPMENU_DISPLAY_I2C_SDA,
      .scl_io_num = CONFIG_ESPMENU_DISPLAY_I2C_SCL,
      .flags.enable_internal_pullup = true,
  };
  BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&bus_config, &i2c_bus));

  // Initialize LCD panel I/O
  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_i2c_config_t io_config = {
      .dev_addr = CONFIG_ESPMENU_DISPLAY_I2C_ADDRESS,
      .control_phase_bytes = 1,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
      .dc_bit_offset = 6};
  BSP_ERROR_CHECK_RETURN_ERR(esp_lcd_new_panel_io_i2c(
      (esp_lcd_i2c_bus_handle_t)i2c_bus, &io_config, &io_handle));

  // Initialize OLED panel
  esp_lcd_panel_handle_t panel_handle = NULL;
  esp_lcd_panel_dev_config_t panel_config = {.reset_gpio_num = -1,
                                             .bits_per_pixel = 1};

#ifdef CONFIG_ESPMENU_DISPLAY_SSD1306
  BSP_ERROR_CHECK_RETURN_ERR(
      esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
#elif defined(CONFIG_ESPMENU_DISPLAY_SH1107)
  // Note: SH1107 support would require external component or custom
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
  lcd_handle = panel_handle;

  // Configure GPIO for rotary encoders
  gpio_config_t io_conf = {.intr_type = GPIO_INTR_ANYEDGE,
                           .mode = GPIO_MODE_INPUT,
                           .pin_bit_mask =
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_A) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_B) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON)
#ifdef CONFIG_ESPMENU_ROTARY_ENCODER_CNT_2
                               | (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_A) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_B) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_2_BUTTON)
#endif
#ifdef CONFIG_ESPMENU_ROTARY_ENCODER_CNT_3
                               | (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_A) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_B) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_3_BUTTON)
#endif
#ifdef CONFIG_ESPMENU_ROTARY_ENCODER_CNT_4
                               | (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_A) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_B) |
                               (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_4_BUTTON)
#endif
                               ,
                           .pull_down_en = GPIO_PULLDOWN_DISABLE,
                           .pull_up_en = GPIO_PULLUP_ENABLE};
  BSP_ERROR_CHECK_RETURN_ERR(gpio_config(&io_conf));

  // Initialize rotary encoders and buttons
  button_handle_t encoder_btn_handles[4] = {NULL};
  knob_handle_t encoder_knob_handles[4] = {NULL};

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

  // Initialize each encoder
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

    // Create knob
    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = encoder_pins[i][0],
        .gpio_encoder_b = encoder_pins[i][1],
    };

    ESP_LOGI(TAG, "Creating encoder on GPIO A:%d B:%d", encoder_pins[i][0],
             encoder_pins[i][1]);
    encoder_knob_handles[i] = iot_knob_create(&knob_cfg);
    if (!encoder_knob_handles[i]) {
      ESP_LOGE(TAG, "Failed to create knob %d", i);
      return ESP_FAIL;
    }
  }

  // Initialize LVGL
  lvgl_port_cfg_t lvgl_cfg = {.task_priority = 5,
                              .task_stack = 4096,
                              .task_affinity = -1,
                              .task_max_sleep_ms = 500,
                              .timer_period_ms = 5};
  BSP_ERROR_CHECK_RETURN_ERR(lvgl_port_init(&lvgl_cfg));

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

  // Add encoders to LVGL
  const char *encoder_names[4] = {"encoder1", "encoder2", "encoder3",
                                  "encoder4"};
  for (int i = 0; i < encoder_count; i++) // Use encoder_count instead of 4
  {
    lvgl_port_encoder_cfg_t encoder_cfg = {
        .disp = disp,
        .encoder_a_b = encoder_knob_handles[i],
        .encoder_enter = encoder_btn_handles[i]};
    lv_indev_t *indev = lvgl_port_add_encoder(&encoder_cfg);
    if (!indev) {
      ESP_LOGE(TAG, "Failed to add encoder %d to LVGL", i + 1);
      return ESP_FAIL;
    }
    lv_group_t *group = lv_group_create();
    lv_indev_set_group(indev, group);

    // Set user data to an associated LVGL object if needed
    // Don't call lv_obj_set_user_data on the indev directly as it's not an
    // lv_obj_t Store the name in the driver data instead if needed
    lv_indev_set_user_data(indev, (void *)encoder_names[i]);
  }

  // Initialize menu widgets
  ESP_LOGI(TAG, "Initializing LVGL menu widgets");
  menu_init();

  ESP_LOGI(TAG, "Menu system fully initialized");
  return ESP_OK;
}