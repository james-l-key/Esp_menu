#ifndef ESP_LCD_TYPES_H
#define ESP_LCD_TYPES_H

// Common LCD color bit depths
typedef enum
{
    ESP_LCD_COLOR_SPACE_RGB,
    ESP_LCD_COLOR_SPACE_BGR
} esp_lcd_color_space_t;

// Common LCD color bit depths
typedef enum
{
    ESP_LCD_COLOR_BITS_8,
    ESP_LCD_COLOR_BITS_16,
    ESP_LCD_COLOR_BITS_18,
    ESP_LCD_COLOR_BITS_24
} esp_lcd_color_bits_t;

#endif // ESP_LCD_TYPES_H