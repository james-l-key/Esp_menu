#include "menu_data.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "iot_knob.h"
#include "iot_button.h"
#include "button_types.h"
#include "button_gpio.h"
#include "string.h"
#include "user_graphic.h"



// Image graphic: pitch_icon
LV_IMG_DECLARE(pitch_icon_dsc);



// Image graphic: waveform_icon
LV_IMG_DECLARE(waveform_icon_dsc);



// Image graphic: next_icon
LV_IMG_DECLARE(next_icon_dsc);



// Image graphic: prev_icon
LV_IMG_DECLARE(prev_icon_dsc);



// Global screen declarations
static lv_obj_t *scr_main;







static lv_obj_t *scr_waveform;



static lv_obj_t *scr_level_fine;



static lv_obj_t *scr_pw_ampmod;



static lv_obj_t *scr_favorites;




static void event_handler(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        const char *text = lv_list_get_btn_text(lv_obj_get_parent(obj), obj);
        
        
        if (strcmp(text, "Pitch Up") == 0) {
            
            pitch_up();
            
        }
        
        if (strcmp(text, "Pitch Down") == 0) {
            
            pitch_down();
            
        }
        
        if (strcmp(text, "Waveform") == 0) {
            
            lv_scr_load(scr_waveform);
            
        }
        
        if (strcmp(text, "Level/Fine") == 0) {
            
            lv_scr_load(scr_level_fine);
            
        }
        
        if (strcmp(text, "PW/AmpMod") == 0) {
            
            lv_scr_load(scr_pw_ampmod);
            
        }
        
        if (strcmp(text, "Favorites") == 0) {
            
            lv_scr_load(scr_favorites);
            
        }
        
        
    }
}

void menu_init(void) {
    // Note: LVGL and encoders are initialized by esp_menu_init()
    // This function only sets up the menu structure
    
    // Create and register screens
    scr_main = lv_obj_create(NULL);
    user_graphic_init(scr_main);
    
    
    
    
    
    
    
    scr_waveform = lv_obj_create(NULL);
    user_graphic_init(scr_waveform);
    
    
    
    scr_level_fine = lv_obj_create(NULL);
    user_graphic_init(scr_level_fine);
    
    
    
    scr_pw_ampmod = lv_obj_create(NULL);
    user_graphic_init(scr_pw_ampmod);
    
    
    
    scr_favorites = lv_obj_create(NULL);
    user_graphic_init(scr_favorites);
    
    
    

    // Menu setup
    
    
    
    lv_obj_t *list_main = lv_list_create(scr_main);
    
    
    
    lv_obj_t *img_1 = lv_img_create(list_main);
    lv_img_set_src(img_1, &pitch_icon_dsc);
    lv_obj_align(img_1, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *btn_1 = lv_list_add_btn(list_main, NULL, "Pitch Up");
    lv_obj_align_to(btn_1, img_1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(btn_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *img_2 = lv_img_create(list_main);
    lv_img_set_src(img_2, &pitch_icon_dsc);
    lv_obj_align(img_2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *btn_2 = lv_list_add_btn(list_main, NULL, "Pitch Down");
    lv_obj_align_to(btn_2, img_2, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(btn_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *img_3 = lv_img_create(list_main);
    lv_img_set_src(img_3, &waveform_icon_dsc);
    lv_obj_align(img_3, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *btn_3 = lv_list_add_btn(list_main, NULL, "Waveform");
    lv_obj_align_to(btn_3, img_3, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(btn_3, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *btn_4 = lv_list_add_btn(list_main, NULL, "Level/Fine");
    lv_obj_add_event_cb(btn_4, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *btn_5 = lv_list_add_btn(list_main, NULL, "PW/AmpMod");
    lv_obj_add_event_cb(btn_5, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *btn_6 = lv_list_add_btn(list_main, NULL, "Favorites");
    lv_obj_add_event_cb(btn_6, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    
    
    
    
    
    lv_obj_t *list_waveform = lv_list_create(scr_waveform);
    
    
    
    lv_obj_t *subimg_main_waveform_1 = lv_img_create(list_waveform);
    lv_img_set_src(subimg_main_waveform_1, &next_icon_dsc);
    lv_obj_align(subimg_main_waveform_1, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *subbtn_main_waveform_1 = lv_list_add_btn(list_waveform, NULL, "Next");
    lv_obj_align_to(subbtn_main_waveform_1, subimg_main_waveform_1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(subbtn_main_waveform_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subimg_main_waveform_2 = lv_img_create(list_waveform);
    lv_img_set_src(subimg_main_waveform_2, &prev_icon_dsc);
    lv_obj_align(subimg_main_waveform_2, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_t *subbtn_main_waveform_2 = lv_list_add_btn(list_waveform, NULL, "Previous");
    lv_obj_align_to(subbtn_main_waveform_2, subimg_main_waveform_2, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_add_event_cb(subbtn_main_waveform_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    
    lv_obj_t *list_level_fine = lv_list_create(scr_level_fine);
    
    
    
    lv_obj_t *subbtn_main_level_fine_1 = lv_list_add_btn(list_level_fine, NULL, "Level Up");
    lv_obj_add_event_cb(subbtn_main_level_fine_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_level_fine_2 = lv_list_add_btn(list_level_fine, NULL, "Level Down");
    lv_obj_add_event_cb(subbtn_main_level_fine_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_level_fine_3 = lv_list_add_btn(list_level_fine, NULL, "Fine Tune Up");
    lv_obj_add_event_cb(subbtn_main_level_fine_3, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_level_fine_4 = lv_list_add_btn(list_level_fine, NULL, "Fine Tune Down");
    lv_obj_add_event_cb(subbtn_main_level_fine_4, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    
    lv_obj_t *list_pw_ampmod = lv_list_create(scr_pw_ampmod);
    
    
    
    lv_obj_t *subbtn_main_pw_ampmod_1 = lv_list_add_btn(list_pw_ampmod, NULL, "Pulse Width Up");
    lv_obj_add_event_cb(subbtn_main_pw_ampmod_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_pw_ampmod_2 = lv_list_add_btn(list_pw_ampmod, NULL, "Pulse Width Down");
    lv_obj_add_event_cb(subbtn_main_pw_ampmod_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_pw_ampmod_3 = lv_list_add_btn(list_pw_ampmod, NULL, "Amp Mod Slot Next");
    lv_obj_add_event_cb(subbtn_main_pw_ampmod_3, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_pw_ampmod_4 = lv_list_add_btn(list_pw_ampmod, NULL, "Amp Mod Slot Prev");
    lv_obj_add_event_cb(subbtn_main_pw_ampmod_4, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    
    lv_obj_t *list_favorites = lv_list_create(scr_favorites);
    
    
    
    lv_obj_t *subbtn_main_favorites_1 = lv_list_add_btn(list_favorites, NULL, "Select Next");
    lv_obj_add_event_cb(subbtn_main_favorites_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_favorites_2 = lv_list_add_btn(list_favorites, NULL, "Select Prev");
    lv_obj_add_event_cb(subbtn_main_favorites_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_favorites_3 = lv_list_add_btn(list_favorites, NULL, "Save");
    lv_obj_add_event_cb(subbtn_main_favorites_3, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_favorites_4 = lv_list_add_btn(list_favorites, NULL, "Load");
    lv_obj_add_event_cb(subbtn_main_favorites_4, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    lv_obj_t *subbtn_main_favorites_5 = lv_list_add_btn(list_favorites, NULL, "Clear");
    lv_obj_add_event_cb(subbtn_main_favorites_5, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    
    
    

    // Load initial screen
    lv_scr_load(scr_main);
}