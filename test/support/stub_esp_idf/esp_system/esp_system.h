// test/support/stub_esp_idf/esp_system/esp_system.h
#ifndef ESP_SYSTEM_H
#define ESP_SYSTEM_H

#include "../esp_common/esp_err.h"

// ESP system functions
esp_err_t esp_restart(void);

typedef enum
{
    ESP_RST_UNKNOWN = 0,
    ESP_RST_POWERON = 1,
    ESP_RST_SW = 3,
    ESP_RST_PANIC = 4,
    ESP_RST_INT_WDT = 5,
    ESP_RST_TASK_WDT = 6,
    ESP_RST_WDT = 7,
    ESP_RST_DEEPSLEEP = 8,
    ESP_RST_BROWNOUT = 9,
    ESP_RST_SDIO = 10,
} esp_reset_reason_t;

esp_reset_reason_t esp_reset_reason(void);

#endif // ESP_SYSTEM_H