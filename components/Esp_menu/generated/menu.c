#include "menu_data.h"
#include "lvgl.h"
#include "espressif__lvgl_port.h"
#include "espressif__knob.h"
#include "espressif__button.h"
#include "string.h"



static void event_handler(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        const char *text = lv_list_get_btn_text(obj);
        
        
        /* DEBUG: Widget type=list, items=[{'text': 'Option 1', 'action': 'option1_action'}, {'text': 'Settings', 'screen': 'settings'}] */
        
        /* DEBUG: Processing list widget with 2 items */
        
        /* DEBUG: Adding item Option 1 */
        if (strcmp(text, "Option 1") == 0) {
            
            option1_action();
            
        }
        
        /* DEBUG: Adding item Settings */
        if (strcmp(text, "Settings") == 0) {
            
            lv_scr_load(scr_settings);
            
        }
        
        
        
        
        
        
    }
}

void menu_init(void) {
    // Display setup
    lvgl_port_cfg_t lvgl_cfg = {
        .disp_cfg = {
            .type = LVGL_PORT_DISP_TYPE_I2C,
            .width = 128,
            .height = 64,
            .i2c = {
                .sda = 21,
                .scl = 22,
                .addr = 0x3C
            }
        }
    };
    lvgl_port_init(&lvgl_cfg);
    lv_disp_t *disp = lvgl_port_add_disp(&lvgl_cfg.disp_cfg);

    // Encoder setup
    
    knob_handle_t encoder1 = knob_create(12, 13);
    button_handle_t encoder1_btn = button_create(14);
    lv_indev_t *indev_encoder1 = lvgl_port_add_indev(encoder1, encoder1_btn);
    lv_group_t *group_encoder1 = lv_group_create();
    

    // Menu setup
    
    lv_obj_t *scr_main = lv_obj_create(NULL);
    
    /* DEBUG: Widget type=list, items=[{'text': 'Option 1', 'action': 'option1_action'}, {'text': 'Settings', 'screen': 'settings'}] */
    
    /* DEBUG: Processing list widget with 2 items */
    lv_obj_t *list_main_1 = lv_list_create(scr_main);
    
    /* DEBUG: Adding item Option 1 */
    lv_list_add_btn(list_main_1, NULL, "Option 1");
    
    /* DEBUG: Adding item Settings */
    lv_list_add_btn(list_main_2, NULL, "Settings");
    
    lv_obj_add_event_cb(list_main_1, event_handler, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(group_encoder1, list_main_1);
    lv_indev_set_group(indev_encoder1, group_encoder1);
    
    
    
    lv_obj_t *scr_settings = lv_obj_create(NULL);
    
    

    // Load initial screen
    lv_scr_load(scr_main);
}