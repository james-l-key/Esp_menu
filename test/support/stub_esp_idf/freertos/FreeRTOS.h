#ifndef FREERTOS_H
#define FREERTOS_H

#include <stdint.h>

// Basic FreeRTOS types
typedef void *TaskHandle_t;
typedef unsigned long TickType_t;
typedef unsigned long UBaseType_t;
typedef long BaseType_t;

// Common FreeRTOS constants
#define pdTRUE (1)
#define pdFALSE (0)
#define pdPASS (1)
#define pdFAIL (0)
#define portMAX_DELAY (0xFFFFFFFFUL)

#endif // FREERTOS_H