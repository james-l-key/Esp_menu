// menu/menu.c

#include "Esp_menu.h"
#include "menu_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h" // Generated configuration header
#include "esp_lcd_panel_io.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "button_gpio.h"
#include "iot_button.h"
#include "knob_gpio.h"
#include "iot_knob.h"

#define TAG "Esp_menu"

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

esp_lcd_panel_handle_t lcd_handle = NULL;

void menu_init()
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_LOGI(TAG, "Initialize I2C bus");
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

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = CONFIG_ESPMENU_SSD1306_I2C_ADDRESS, // SSD1306 address, typically 0x3C or 0x3D
        .control_phase_bytes = 1,                       // According to SSD1306 datasheet
        .lcd_cmd_bits = 8,                              // Command bit-width
        .lcd_param_bits = 8,                            // Parameter bit-width
        .dc_bit_offset = 6                              // D/C bit position in the first byte
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_bus, &io_config, &io_handle));
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = CONFIG_ESPMENU_SSD1306_HEIGHT, // Height of the display
    };
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1, // No reset pin
        .flags = {
            .reset_active_high = 0,
        },
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    lcd_handle = panel_handle;

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_A) |
        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_B) |
        (1ULL << CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);

    // Encoder init code...
    // Set up interrupt for encoder

    i2c_communication_init(); // Init the inter module I2C.

    button_handle_t prev_btn_handle = NULL;
    button_handle_t next_btn_handle = NULL;
    button_handle_t enter_btn_handle = NULL;

    button_config_t btn_cfg = {0};

    button_gpio_config_t bsp_button_config[] = {
        {
            .gpio_num = GPIO_NUM_37,
            .active_level = 0,
        },
        {
            .gpio_num = GPIO_NUM_38,
            .active_level = 0,
        },
        {
            .gpio_num = GPIO_NUM_39,
            .active_level = 0,
        },
    };

    iot_button_new_gpio_device(&btn_cfg, &bsp_button_config[0], &prev_btn_handle);
    iot_button_new_gpio_device(&btn_cfg, &bsp_button_config[1], &next_btn_handle);
    iot_button_new_gpio_device(&btn_cfg, &bsp_button_config[2], &enter_btn_handle);

    lv_disp_t *disp_handle = lv_disp_get_default(); // Get the default display

    lvgl_port_nav_btns_cfg_t btns = {
        .disp = disp_handle,
        .button_prev = prev_btn_handle,
        .button_next = next_btn_handle,
        .button_enter = enter_btn_handle
    };

    lv_indev_t *buttons_handle = lvgl_port_add_navigation_buttons(&btns);
    (void)buttons_handle; // Suppress unused variable warning

    // Add encoder code
    button_handle_t encoder_btn_handle = NULL;

    button_gpio_config_t encoder_btn_config = {
        .gpio_num = CONFIG_ESPMENU_ROTARY_ENCODER_1_BUTTON,
        .active_level = 0,
    };

    knob_config_t encoder_config = {
        .gpio_encoder_a = CONFIG_ESPMENU_ROTARY_ENCODER_1_A,
        .gpio_encoder_b = CONFIG_ESPMENU_ROTARY_ENCODER_1_B,
    };

    ESP_ERROR_CHECK(iot_button_new_gpio_device(&btn_cfg, &encoder_btn_config, &encoder_btn_handle));

    lvgl_port_encoder_cfg_t encoder = {
        .disp = disp_handle,
        .encoder_enter = encoder_btn_handle
    };
}

void encoder_isr_handler(void *arg)
{
    // Encoder rotation handling
    // Update selected item or parameter value
    // If a parameter needs to change a module, use i2c_communication function.
    // i2c_write_bytes(Module_Address, data, length);
}

typedef enum
{
    CLEANUP_BUTTONS,
    CLEANUP_ENCODER
} menu_cleanup_type_t;

void menu_cleanup(lv_indev_t *handle, menu_cleanup_type_t type)
{
    if (handle)
    {
        switch (type)
        {
        case CLEANUP_BUTTONS:
            lvgl_port_remove_navigation_buttons(handle);
            break;
        case CLEANUP_ENCODER:
            lvgl_port_remove_encoder(handle);
            break;
        }
    }
}