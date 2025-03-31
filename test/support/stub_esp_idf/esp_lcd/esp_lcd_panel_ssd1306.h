// test/support/stub_esp_idf/esp_lcd/esp_lcd_panel_ssd1306.h
#ifndef ESP_LCD_PANEL_SSD1306_H
#define ESP_LCD_PANEL_SSD1306_H

#include "../esp_common/esp_err.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"

typedef struct
{
    int reset_gpio_num;
    bool reset_level;
    int bits_per_pixel;
    int pclk_hz;
    struct
    {
        bool fb_in_psram;
        bool mirror_y;
    } flags;
    struct
    {
        int rst_duration_ms;
        int pclk_hz;
    } timings;
} esp_lcd_panel_dev_config_t;

esp_err_t esp_lcd_new_panel_ssd1306(esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_config, esp_lcd_panel_handle_t *ret_panel);

#endif // ESP_LCD_PANEL_SSD1306_H