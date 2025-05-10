#include "menu_data.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "iot_knob.h"
#include "iot_button.h"
#include "string.h"

#include "user_graphic.h"

static void event_handler(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        const char *text = lv_list_get_btn_text(obj);
    }
}

void menu_init(void)
{
    // Display setup
    lvgl_port_cfg_t lvgl_cfg = {
        .disp_cfg = {
            .type = LVGL_PORT_DISP_TYPE_I2C,
            .width = 128,
            .height = 64,
            .i2c = {
                .sda = 21,
                .scl = 22,
                .addr = 0x3C}}};
    lvgl_port_init(&lvgl_cfg);
    lv_disp_t *disp = lvgl_port_add_disp(&lvgl_cfg.disp_cfg);

    // Encoder setup

    knob_handle_t encoder1 = knob_create(5, 6);
    button_handle_t encoder1_btn = button_create(7);
    lv_indev_t *indev_encoder1 = lvgl_port_add_indev(encoder1, encoder1_btn);
    lv_group_t *group_encoder1 = lv_group_create();

    // Menu setup

    lv_obj_t *scr_main = lv_obj_create(NULL);

    // Load initial screen
    lv_scr_load(scr_main);
}