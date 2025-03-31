// test/support/stub_esp_idf/esp_lcd/esp_lcd_panel_ops.h
#ifndef ESP_LCD_PANEL_OPS_H
#define ESP_LCD_PANEL_OPS_H

#include "../esp_common/esp_err.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct esp_lcd_panel_t *esp_lcd_panel_handle_t;

// LCD panel operation function pointer types
typedef esp_err_t (*esp_lcd_panel_del_t)(esp_lcd_panel_handle_t panel);
typedef esp_err_t (*esp_lcd_panel_reset_t)(esp_lcd_panel_handle_t panel);
typedef esp_err_t (*esp_lcd_panel_init_t)(esp_lcd_panel_handle_t panel);
typedef esp_err_t (*esp_lcd_panel_draw_bitmap_t)(esp_lcd_panel_handle_t panel, int x_start, int y_start, int x_end, int y_end, const void *color_data);
typedef esp_err_t (*esp_lcd_panel_invert_color_t)(esp_lcd_panel_handle_t panel, bool invert_color_data);
typedef esp_err_t (*esp_lcd_panel_mirror_t)(esp_lcd_panel_handle_t panel, bool mirror_x, bool mirror_y);
typedef esp_err_t (*esp_lcd_panel_swap_xy_t)(esp_lcd_panel_handle_t panel, bool swap_xy);
typedef esp_err_t (*esp_lcd_panel_set_gap_t)(esp_lcd_panel_handle_t panel, int x_gap, int y_gap);
typedef esp_err_t (*esp_lcd_panel_disp_on_off_t)(esp_lcd_panel_handle_t panel, bool on_off);

// LCD panel operation structure
typedef struct
{
    esp_lcd_panel_del_t del;
    esp_lcd_panel_reset_t reset;
    esp_lcd_panel_init_t init;
    esp_lcd_panel_draw_bitmap_t draw_bitmap;
    esp_lcd_panel_invert_color_t invert_color;
    esp_lcd_panel_mirror_t mirror;
    esp_lcd_panel_swap_xy_t swap_xy;
    esp_lcd_panel_set_gap_t set_gap;
    esp_lcd_panel_disp_on_off_t disp_on_off;
} esp_lcd_panel_ops_t;

#endif // ESP_LCD_PANEL_OPS_H