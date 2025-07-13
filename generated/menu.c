#include "menu_data.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "iot_knob.h"
#include "iot_button.h"
#include "button_types.h"
#include "button_gpio.h"
#include "string.h"

#include "user_graphic.h"

static void event_handler(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        // Get the button's label text in LVGL v9.x
        lv_obj_t *label = lv_obj_get_child(obj, 0);
        const char *text = lv_label_get_text(label);
    }
}

void menu_init(void)
{
    // Initialize LVGL port with default configuration
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    // Configure the display
    const lvgl_port_display_cfg_t disp_cfg = {
        .hres = 128,
        .vres = 64,
        .flags = {
            .buff_dma = true,     // Use DMA for rendering (if applicable)
            .buff_spiram = false, // Use SPI RAM buffer if available
            .sw_rotate = false,   // Use software rotation
            .swap_bytes = false,  // Swap RGB565 bytes
        },
    };

    // Add display
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);

    // Create and register the main screen
    lv_obj_t *scr_main = NULL;

    scr_main = lv_obj_create(NULL);

    if (!scr_main)
    {
        scr_main = lv_obj_create(NULL);
    }

    // Encoder setup

    // Create knob with updated API
    knob_config_t knob_cfg = {
        .default_direction = 0,
        .gpio_encoder_a = 5,
        .gpio_encoder_b = 6};
    knob_handle_t encoder1 = iot_knob_create(&knob_cfg);

    // Create button with updated API for button component v4.x
    button_config_t btn_cfg = {
        .long_press_time = 0,
        .short_press_time = 0,
    };

    // Configure GPIO button properties
    button_gpio_config_t gpio_btn_cfg = {
        .gpio_num = 7,
        .active_level = 0,
        .enable_power_save = false,
        .disable_pull = false,
    };

    button_handle_t encoder1_btn = NULL;
    if (iot_button_new_gpio_device(&btn_cfg, &gpio_btn_cfg, &encoder1_btn) != ESP_OK)
    {
        // Handle error (e.g., log or assert)
        // For example:
        printf("Failed to initialize encoder button\n");
        return;
    }

    // Create encoder input device using updated API
    lvgl_port_encoder_cfg_t encoder_cfg = {
        .disp = disp,
        .encoder_a_b = encoder1,
        .encoder_enter = encoder1_btn};
    lv_indev_t *indev_encoder1 = lvgl_port_add_encoder(&encoder_cfg);
    lv_group_t *group_encoder1 = lv_group_create();

    // Menu setup

