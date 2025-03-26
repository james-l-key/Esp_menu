// menu/menu.c

#include "Esp_menu.h"
#include "driver/gpio.h"
#include "sdkconfig.h" // Generated configuration header
#include "esp_lcd_panel_io_i2c.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

esp_lcd_panel_handle_t lcd_handle = NULL;

void menu_init()
{

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = CONFIG_SSD1306_ADDRESS,
        .sda_io_num = CONFIG_SSD1306_SDA_PIN,
        .scl_io_num = CONFIG_SSD1306_SCL_PIN,
        .pclk_hz = 100000,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(&io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = -1, // No reset pin for SSD1306
        .bits_per_pixel = 1,
        .pclk_hz = 100000,
        .reset_level = 0,
        .flags.fb_in_psram = false, // Frame buffer in internal RAM
        .flags.mirror_y = true,
        .timings.rst_duration_ms = 10,
        .timings.pclk_hz = 100000,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    lcd_handle = panel_handle;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << CONFIG_ENCODER_A_PIN) | (1ULL << CONFIG_ENCODER_B_PIN) | (1ULL << CONFIG_ENCODER_BUTTON_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // Encoder init code...
    // Set up interrupt for encoder
    i2c_communication_init(); // Init the inter module I2C.
}

void menu_draw()
{
    // Draw current menu and selected item on the LCD
    // Use data from menu_data variable, and current menu state.
    // Use esp_lcd_panel_ops functions to draw to the screen.
}

void encoder_isr_handler(void *arg)
{
    // Encoder rotation handling
    // Update selected item or parameter value
    // If a parameter needs to change a module, use i2c_communication function.
    // i2c_write_bytes(Module_Address, data, length);
}