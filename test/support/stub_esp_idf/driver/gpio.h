#ifndef GPIO_H
#define GPIO_H

#include "../esp_common/esp_err.h"
#include <stdint.h>

typedef enum
{
    GPIO_INTR_DISABLE = 0,
    GPIO_INTR_POSEDGE = 1,
    GPIO_INTR_NEGEDGE = 2,
    GPIO_INTR_ANYEDGE = 3,
    GPIO_INTR_LOW_LEVEL = 4,
    GPIO_INTR_HIGH_LEVEL = 5,
} gpio_int_type_t;

typedef enum
{
    GPIO_MODE_DISABLE = 0,
    GPIO_MODE_INPUT = 1,
    GPIO_MODE_OUTPUT = 2,
    GPIO_MODE_OUTPUT_OD = 3,
    GPIO_MODE_INPUT_OUTPUT_OD = 4,
    GPIO_MODE_INPUT_OUTPUT = 5,
} gpio_mode_t;

#define GPIO_PULLUP_ENABLE 1
#define GPIO_PULLUP_DISABLE 0
#define GPIO_PULLDOWN_ENABLE 1
#define GPIO_PULLDOWN_DISABLE 0

typedef struct
{
    gpio_int_type_t intr_type;
    gpio_mode_t mode;
    uint64_t pin_bit_mask;
    bool pull_up_en;
    bool pull_down_en;
} gpio_config_t;

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);

#endif // GPIO_H