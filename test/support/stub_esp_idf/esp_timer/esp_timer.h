// test/support/stub_esp_idf/esp_timer/esp_timer.h
#ifndef ESP_TIMER_H
#define ESP_TIMER_H

#include "../esp_common/esp_err.h"
#include <stdint.h>

typedef struct esp_timer *esp_timer_handle_t;

typedef void (*esp_timer_cb_t)(void *arg);

typedef struct
{
    esp_timer_cb_t callback;
    void *arg;
    const char *name;
    bool skip_unhandled_events;
} esp_timer_create_args_t;

esp_err_t esp_timer_create(const esp_timer_create_args_t *create_args, esp_timer_handle_t *out_handle);
esp_err_t esp_timer_start_once(esp_timer_handle_t timer, uint64_t timeout_us);
esp_err_t esp_timer_start_periodic(esp_timer_handle_t timer, uint64_t period_us);
esp_err_t esp_timer_stop(esp_timer_handle_t timer);
esp_err_t esp_timer_delete(esp_timer_handle_t timer);

#endif // ESP_TIMER_H