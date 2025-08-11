// Generated menu.c from template
// Includes required LVGL and user graphics headers
#include "lvgl.h"
#include "menu_data.h"
#include "user_graphic.h"
#include <string.h>

// Helper macro to sanitize names into valid C identifiers
// Jinja macro: replaces spaces, slashes, hyphens and '&' then lowers


// Forward declarations for submenu screens so event handlers can switch screens







static lv_obj_t *scr_waveform;



static lv_obj_t *scr_level_fine;



static lv_obj_t *scr_pw_ampmod;



static lv_obj_t *scr_favorites;




// Declarations for embedded images provided via graphics_code


// Image graphic: pitch_icon
LV_IMG_DECLARE(pitch_icon_dsc);



// Image graphic: waveform_icon
LV_IMG_DECLARE(waveform_icon_dsc);



// Image graphic: next_icon
LV_IMG_DECLARE(next_icon_dsc);



// Image graphic: prev_icon
LV_IMG_DECLARE(prev_icon_dsc);



static void event_handler(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    const char *text = lv_list_get_btn_text(lv_obj_get_parent(obj), obj);
    if (!text) return;
    
    
    if (strcmp(text, "Pitch Up") == 0) {
        
        pitch_up();
        
        return;
    }
    
    if (strcmp(text, "Pitch Down") == 0) {
        
        pitch_down();
        
        return;
    }
    
    if (strcmp(text, "Waveform") == 0) {
        
        lv_scr_load(scr_waveform);
        
        return;
    }
    
    if (strcmp(text, "Level/Fine") == 0) {
        
        lv_scr_load(scr_level_fine);
        
        return;
    }
    
    if (strcmp(text, "PW/AmpMod") == 0) {
        
        lv_scr_load(scr_pw_ampmod);
        
        return;
    }
    
    if (strcmp(text, "Favorites") == 0) {
        
        lv_scr_load(scr_favorites);
        
        return;
    }
    
    
}

void menu_init(void) {
    // Create main screen and any submenu screens
    lv_obj_t *scr_main = lv_obj_create(NULL);
    user_graphic_init(scr_main);
    
    
    
    
    
    
    
    scr_waveform = lv_obj_create(NULL);
    user_graphic_init(scr_waveform);
    
    
    
    scr_level_fine = lv_obj_create(NULL);
    user_graphic_init(scr_level_fine);
    
    
    
    scr_pw_ampmod = lv_obj_create(NULL);
    user_graphic_init(scr_pw_ampmod);
    
    
    
    scr_favorites = lv_obj_create(NULL);
    user_graphic_init(scr_favorites);
    
    
    

    // Styles for better focus visibility and spacing on monochrome OLED
    static lv_style_t style_focus;
    static lv_style_t style_normal;
    lv_style_init(&style_focus);
    lv_style_set_bg_opa(&style_focus, LV_OPA_COVER);
    lv_style_set_bg_color(&style_focus, lv_color_white());
    lv_style_set_text_color(&style_focus, lv_color_black());
    lv_style_set_border_width(&style_focus, 2);
    lv_style_set_border_color(&style_focus, lv_color_black());
    lv_style_set_outline_width(&style_focus, 1);
    lv_style_set_outline_color(&style_focus, lv_color_black());
    lv_style_set_pad_all(&style_focus, 2);

    lv_style_init(&style_normal);
    lv_style_set_pad_all(&style_normal, 1);
    lv_style_set_min_height(&style_normal, 14);

    // Build main menu list
    lv_obj_t *list_main = lv_list_create(scr_main);
    lv_obj_center(list_main);
    lv_obj_add_style(list_main, &style_focus, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_add_style(list_main, &style_normal, LV_PART_ITEMS);
    
    
    
    lv_obj_t *btn_main_0 = lv_list_add_btn(list_main, &pitch_icon_dsc, "Pitch Up");
    
    lv_obj_add_event_cb(btn_main_0, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    lv_obj_t *btn_main_1 = lv_list_add_btn(list_main, &pitch_icon_dsc, "Pitch Down");
    
    lv_obj_add_event_cb(btn_main_1, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    lv_obj_t *btn_main_2 = lv_list_add_btn(list_main, &waveform_icon_dsc, "Waveform");
    
    lv_obj_add_event_cb(btn_main_2, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    lv_obj_t *btn_main_3 = lv_list_add_btn(list_main, NULL, "Level/Fine");
    
    lv_obj_add_event_cb(btn_main_3, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    lv_obj_t *btn_main_4 = lv_list_add_btn(list_main, NULL, "PW/AmpMod");
    
    lv_obj_add_event_cb(btn_main_4, event_handler, LV_EVENT_CLICKED, NULL);
    
    
    lv_obj_t *btn_main_5 = lv_list_add_btn(list_main, NULL, "Favorites");
    
    lv_obj_add_event_cb(btn_main_5, event_handler, LV_EVENT_CLICKED, NULL);
    
    

    // Build submenu lists
    
    
    
    
    
    
    
    lv_obj_t *list_waveform = lv_list_create(scr_waveform);
    lv_obj_center(list_waveform);
    lv_obj_add_style(list_waveform, &style_focus, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_add_style(list_waveform, &style_normal, LV_PART_ITEMS);
        
        
        lv_obj_t *btn_waveform_0 = lv_list_add_btn(list_waveform, &next_icon_dsc, "Next");
        
        lv_obj_add_event_cb(btn_waveform_0, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_waveform_1 = lv_list_add_btn(list_waveform, &prev_icon_dsc, "Previous");
        
        lv_obj_add_event_cb(btn_waveform_1, event_handler, LV_EVENT_CLICKED, NULL);
        
    
    
    
    lv_obj_t *list_level_fine = lv_list_create(scr_level_fine);
    lv_obj_center(list_level_fine);
    lv_obj_add_style(list_level_fine, &style_focus, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_add_style(list_level_fine, &style_normal, LV_PART_ITEMS);
        
        
        lv_obj_t *btn_level_fine_0 = lv_list_add_btn(list_level_fine, NULL, "Level Up");
        
        lv_obj_add_event_cb(btn_level_fine_0, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_level_fine_1 = lv_list_add_btn(list_level_fine, NULL, "Level Down");
        
        lv_obj_add_event_cb(btn_level_fine_1, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_level_fine_2 = lv_list_add_btn(list_level_fine, NULL, "Fine Tune Up");
        
        lv_obj_add_event_cb(btn_level_fine_2, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_level_fine_3 = lv_list_add_btn(list_level_fine, NULL, "Fine Tune Down");
        
        lv_obj_add_event_cb(btn_level_fine_3, event_handler, LV_EVENT_CLICKED, NULL);
        
    
    
    
    lv_obj_t *list_pw_ampmod = lv_list_create(scr_pw_ampmod);
    lv_obj_center(list_pw_ampmod);
    lv_obj_add_style(list_pw_ampmod, &style_focus, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_add_style(list_pw_ampmod, &style_normal, LV_PART_ITEMS);
        
        
        lv_obj_t *btn_pw_ampmod_0 = lv_list_add_btn(list_pw_ampmod, NULL, "Pulse Width Up");
        
        lv_obj_add_event_cb(btn_pw_ampmod_0, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_pw_ampmod_1 = lv_list_add_btn(list_pw_ampmod, NULL, "Pulse Width Down");
        
        lv_obj_add_event_cb(btn_pw_ampmod_1, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_pw_ampmod_2 = lv_list_add_btn(list_pw_ampmod, NULL, "Amp Mod Slot Next");
        
        lv_obj_add_event_cb(btn_pw_ampmod_2, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_pw_ampmod_3 = lv_list_add_btn(list_pw_ampmod, NULL, "Amp Mod Slot Prev");
        
        lv_obj_add_event_cb(btn_pw_ampmod_3, event_handler, LV_EVENT_CLICKED, NULL);
        
    
    
    
    lv_obj_t *list_favorites = lv_list_create(scr_favorites);
    lv_obj_center(list_favorites);
    lv_obj_add_style(list_favorites, &style_focus, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_add_style(list_favorites, &style_normal, LV_PART_ITEMS);
        
        
        lv_obj_t *btn_favorites_0 = lv_list_add_btn(list_favorites, NULL, "Select Next");
        
        lv_obj_add_event_cb(btn_favorites_0, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_favorites_1 = lv_list_add_btn(list_favorites, NULL, "Select Prev");
        
        lv_obj_add_event_cb(btn_favorites_1, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_favorites_2 = lv_list_add_btn(list_favorites, NULL, "Save");
        
        lv_obj_add_event_cb(btn_favorites_2, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_favorites_3 = lv_list_add_btn(list_favorites, NULL, "Load");
        
        lv_obj_add_event_cb(btn_favorites_3, event_handler, LV_EVENT_CLICKED, NULL);
        
        
        lv_obj_t *btn_favorites_4 = lv_list_add_btn(list_favorites, NULL, "Clear");
        
        lv_obj_add_event_cb(btn_favorites_4, event_handler, LV_EVENT_CLICKED, NULL);
        
    
    
    

    // Show the main screen
    lv_scr_load(scr_main);
}