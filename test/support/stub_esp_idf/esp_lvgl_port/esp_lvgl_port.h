// test/support/stub_esp_idf/esp_lvgl_port/esp_lvgl_port.h
#ifndef ESP_LVGL_PORT_H
#define ESP_LVGL_PORT_H

#include "../esp_common/esp_err.h"

// Forward declarations for LVGL types
typedef struct _lv_obj_t lv_obj_t;
typedef struct _lv_disp_t lv_disp_t;
typedef struct _lv_indev_t lv_indev_t;

// Basic LVGL port structures and functions
typedef struct
{
    int dummy; // Placeholder
} esp_lvgl_port_cfg_t;

typedef void *esp_lvgl_port_handle_t;

// Basic API stubs
esp_err_t esp_lvgl_port_init(const esp_lvgl_port_cfg_t *cfg);
esp_lvgl_port_handle_t esp_lvgl_port_create(const esp_lvgl_port_cfg_t *cfg);
esp_err_t esp_lvgl_port_deinit(void);
esp_err_t esp_lvgl_port_destroy(esp_lvgl_port_handle_t handle);
lv_disp_t *esp_lvgl_port_add_disp(esp_lvgl_port_handle_t handle, uint32_t width, uint32_t height, void *buf);
lv_indev_t *esp_lvgl_port_add_touch(esp_lvgl_port_handle_t handle);

#endif // ESP_LVGL_PORT_H