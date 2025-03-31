// test/support/stub_esp_idf/freertos/task.h
#ifndef FREERTOS_TASK_H
#define FREERTOS_TASK_H

#include "FreeRTOS.h" // Include this instead of redefining types
#include <stdbool.h>

// Task creation function
BaseType_t xTaskCreate(
    void (*pxTaskCode)(void *),
    const char *const pcName,
    const uint32_t usStackDepth,
    void *const pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t *const pxCreatedTask);

// Delay functions
void vTaskDelay(const TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t *pxPreviousWakeTime, const TickType_t xTimeIncrement);

// Task control
void vTaskSuspend(TaskHandle_t xTaskToSuspend);
void vTaskResume(TaskHandle_t xTaskToResume);
void vTaskDelete(TaskHandle_t xTaskToDelete);

#endif // FREERTOS_TASK_H