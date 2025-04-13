// Esp_menu/Esp_menu.h

#ifndef ESP_MENU_H
#define ESP_MENU_H

#include <stdint.h>

void i2c_communication_init(void);

typedef enum
{
    WAVE_SINE,
    WAVE_SQUARE,
    WAVE_SAW,
    WAVE_TRIANGLE
} WaveformType;

typedef struct
{
    const char *label;
    void *variable;
    int min;
    int max;
    float step;
    const char **values;
} ParameterItem;

typedef struct
{
    const char *label;
    int item_count;
    void *items; // Union of ParameterItem and MenuItem
} MenuItem;

typedef struct
{
    int menu_count;
    MenuItem *menus;
} MenuData;

extern const MenuData menu_data;

void Esp_menu_init();
void Esp_menu_draw();
void encoder_isr_handler(void *arg);

#endif // ESP_MENU_H