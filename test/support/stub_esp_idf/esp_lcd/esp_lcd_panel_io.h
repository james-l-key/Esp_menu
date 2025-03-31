// test/support/stub_esp_idf/esp_lcd/esp_lcd_panel_io.h
#ifndef ESP_LCD_PANEL_IO_H
#define ESP_LCD_PANEL_IO_H

#include <stdint.h>
#include <stddef.h>
#include "../esp_common/esp_err.h"

typedef struct esp_lcd_panel_io_t *esp_lcd_panel_io_handle_t;

// Basic SPI configuration for LCD panel
typedef struct
{
    int cs_gpio_num;
    int dc_gpio_num;
    int spi_mode;
    int pclk_hz;
    int trans_queue_depth;
    void *user_ctx;
    void (*on_color_trans_done)(void *user_ctx);
} esp_lcd_panel_io_spi_config_t;

// I2C configuration for LCD panel
typedef struct
{
    uint32_t dev_addr;
    uint32_t control_phase_bytes;
    uint32_t lcd_cmd_bits;
    uint32_t lcd_param_bits;
    struct
    {
        uint32_t dc_bit_offset;
        bool disable_dc_line;
    } dc_line;
    bool disable_control_phase;
} esp_lcd_panel_io_i2c_config_t;

// SPI bus handle type
typedef void *esp_lcd_spi_bus_handle_t;

#endif // ESP_LCD_PANEL_IO_Hesp_err_t esp_lcd_panel_io_del(esp_lcd_panel_io_handle_t io);esp_err_t esp_lcd_panel_io_tx_color(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *color, size_t color_size);esp_err_t esp_lcd_panel_io_tx_param(esp_lcd_panel_io_handle_t io, int lcd_cmd, const void *param, size_t param_size);esp_err_t esp_lcd_new_panel_io_i2c(const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);esp_err_t esp_lcd_new_panel_io_spi(esp_lcd_spi_bus_handle_t bus, const esp_lcd_panel_io_spi_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);
       // Function prototypes