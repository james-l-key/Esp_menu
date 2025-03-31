// test/support/stub_esp_idf/esp_lcd/esp_lcd_panel_vendor.h
#ifndef ESP_LCD_PANEL_VENDOR_H
#define ESP_LCD_PANEL_VENDOR_H

#include "../esp_common/esp_err.h"
#include <stdbool.h> // Add this for bool type

typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;

// Common LCD panel vendor IDs
#define ST7735_PANEL_ID 0x7735
#define ST7789_PANEL_ID 0x7789
#define ILI9341_PANEL_ID 0x9341

// Stub functions typically used with LCD panels
esp_err_t esp_lcd_panel_init(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_reset(esp_lcd_panel_handle_t panel);
esp_err_t esp_lcd_panel_invert_color(esp_lcd_panel_handle_t panel, bool invert_color_data);
esp_err_t esp_lcd_panel_set_gap(esp_lcd_panel_handle_t panel, int x_gap, int y_gap);
esp_err_t esp_lcd_panel_mirror(esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y);
esp_err_t esp_lcd_panel_disp_on_off(esp_lcd_panel_handle_t panel, bool on_off);
esp_err_t esp_lcd_panel_swap_xy(esp_lcd_panel_handle_t panel, bool swap_xy);

#endif // ESP_LCD_PANEL_VENDOR_H