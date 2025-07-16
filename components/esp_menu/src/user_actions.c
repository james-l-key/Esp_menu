/**
 * @file user_actions.c
 * @brief Implementation of user-defined menu actions for the oscillator module.
 */

#include "user_actions.h"
#include "esp_menu.h"
#ifdef CONFIG_ESPMENU_ENABLE_NVS
#include "nvs_flash.h"
#endif
#include <string.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/** @brief Number of favorite slots for parameter storage. */
#define NUM_FAVORITE_SLOTS 4

/** @brief Global oscillator parameters. */
MenuParams_t menu_params = {69, 0, OSC_WAVE_SINE, 65535, 32768, 0xFF, 0xFF, 0xFF};

/** @brief Current favorite slot index (0–3). */
uint8_t current_slot = 0;

/** @brief LVGL label for displaying parameters. */
static lv_obj_t *param_label = NULL;

/**
 * @brief Initializes project-specific state, including loading from NVS if enabled.
 */
void user_init(void)
{
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    load_from_nvs();
#endif
}

/**
 * @brief Updates the LVGL display with current parameters (pitch and waveform).
 */
void user_update_display(void)
{
    if (!param_label)
    {
        param_label = lv_label_create(lv_scr_act());
        lv_obj_set_pos(param_label, 0, 0);
    }
    char buf[32];
    const char *wave_names[] = {"Sine", "Triangle", "Saw", "Square", "Pulse"};
    snprintf(buf, sizeof(buf), "P:%d W:%s", menu_params.frequency_pitch, wave_names[menu_params.waveform]);
    lv_label_set_text(param_label, buf);
}

#ifdef CONFIG_ESPMENU_ENABLE_NVS
/**
 * @brief Saves current parameters to Non-Volatile Storage (NVS).
 */
void save_to_nvs(void)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK)
        return;
    nvs_set_u8(nvs, "freq_pitch", menu_params.frequency_pitch);
    nvs_set_i16(nvs, "freq_fine", menu_params.frequency_fine);
    nvs_set_u8(nvs, "waveform", menu_params.waveform);
    nvs_set_u16(nvs, "level", menu_params.level);
    nvs_set_u16(nvs, "pulse_width", menu_params.pulse_width);
    nvs_set_u8(nvs, "amp_mod_slot", menu_params.amp_mod_slot);
    nvs_set_u8(nvs, "freq_mod_slot", menu_params.freq_mod_slot);
    nvs_set_u8(nvs, "sync_slot", menu_params.sync_source_slot);
    nvs_commit(nvs);
    nvs_close(nvs);
}

/**
 * @brief Loads parameters from Non-Volatile Storage (NVS) and updates the display.
 */
void load_from_nvs(void)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READONLY, &nvs);
    if (err != ESP_OK)
        return;
    nvs_get_u8(nvs, "freq_pitch", &menu_params.frequency_pitch);
    nvs_get_i16(nvs, "freq_fine", &menu_params.frequency_fine);
    nvs_get_u8(nvs, "waveform", (uint8_t *)&menu_params.waveform);
    nvs_get_u16(nvs, "level", &menu_params.level);
    nvs_get_u16(nvs, "pulse_width", &menu_params.pulse_width);
    nvs_get_u8(nvs, "amp_mod_slot", &menu_params.amp_mod_slot);
    nvs_get_u8(nvs, "freq_mod_slot", &menu_params.freq_mod_slot);
    nvs_get_u8(nvs, "sync_slot", &menu_params.sync_source_slot);
    nvs_close(nvs);
    user_update_display();
}

/**
 * @brief Saves parameters to a favorite slot in NVS.
 * @param slot The favorite slot index (0–3).
 */
void save_favorite(uint8_t slot)
{
    if (slot >= NUM_FAVORITE_SLOTS)
        return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK)
        return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    nvs_set_blob(nvs, key, &menu_params, sizeof(MenuParams_t));
    nvs_commit(nvs);
    nvs_close(nvs);
}

/**
 * @brief Loads parameters from a favorite slot in NVS and updates the display.
 * @param slot The favorite slot index (0–3).
 */
void load_favorite(uint8_t slot)
{
    if (slot >= NUM_FAVORITE_SLOTS)
        return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READONLY, &nvs);
    if (err != ESP_OK)
        return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    size_t size = sizeof(MenuParams_t);
    nvs_get_blob(nvs, key, &menu_params, &size);
    nvs_close(nvs);
    save_to_nvs();
    user_update_display();
}

/**
 * @brief Clears a favorite slot in NVS.
 * @param slot The favorite slot index (0–3).
 */
void clear_favorite(uint8_t slot)
{
    if (slot >= NUM_FAVORITE_SLOTS)
        return;
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("oscillator", NVS_READWRITE, &nvs);
    if (err != ESP_OK)
        return;
    char key[16];
    snprintf(key, sizeof(key), "fav_slot_%d", slot);
    nvs_erase_key(nvs, key);
    nvs_commit(nvs);
    nvs_close(nvs);
}
#else
/**
 * @brief Stub for saving to NVS when NVS is disabled.
 */
void save_to_nvs(void) {}

/**
 * @brief Stub for loading from NVS when NVS is disabled.
 */
void load_from_nvs(void) {}

/**
 * @brief Stub for saving to a favorite slot when NVS is disabled.
 * @param slot The favorite slot index (0–3).
 */
void save_favorite(uint8_t slot) {}

/**
 * @brief Stub for loading from a favorite slot when NVS is disabled.
 * @param slot The favorite slot index (0–3).
 */
void load_favorite(uint8_t slot) {}

/**
 * @brief Stub for clearing a favorite slot when NVS is disabled.
 * @param slot The favorite slot index (0–3).
 */
void clear_favorite(uint8_t slot) {}
#endif

/**
 * @brief Increments the frequency pitch and updates the display.
 */
void pitch_up(void)
{
    menu_params.frequency_pitch = menu_params.frequency_pitch < 127 ? menu_params.frequency_pitch + 1 : 127;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
    user_update_display();
}

/**
 * @brief Decrements the frequency pitch and updates the display.
 */
void pitch_down(void)
{
    menu_params.frequency_pitch = menu_params.frequency_pitch > 0 ? menu_params.frequency_pitch - 1 : 0;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
    user_update_display();
}

/**
 * @brief Selects the next waveform type and updates the display.
 */
void waveform_next(void)
{
    menu_params.waveform = (menu_params.waveform + 1) % 5;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
    user_update_display();
}

/**
 * @brief Selects the previous waveform type and updates the display.
 */
void waveform_prev(void)
{
    menu_params.waveform = (menu_params.waveform + 4) % 5;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
    user_update_display();
}

/**
 * @brief Increases the output level.
 */
void level_up(void)
{
    menu_params.level = menu_params.level < 65535 - 655 ? menu_params.level + 655 : 65535;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Decreases the output level.
 */
void level_down(void)
{
    menu_params.level = menu_params.level > 655 ? menu_params.level - 655 : 0;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Increments the fine frequency adjustment.
 */
void fine_tune_up(void)
{
    menu_params.frequency_fine = menu_params.frequency_fine < 100 ? menu_params.frequency_fine + 1 : 100;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Decrements the fine frequency adjustment.
 */
void fine_tune_down(void)
{
    menu_params.frequency_fine = menu_params.frequency_fine > -100 ? menu_params.frequency_fine - 1 : -100;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Increases the pulse width for pulse wave.
 */
void pulse_width_up(void)
{
    menu_params.pulse_width = menu_params.pulse_width < 65535 - 655 ? menu_params.pulse_width + 655 : 65535;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Decreases the pulse width for pulse wave.
 */
void pulse_width_down(void)
{
    menu_params.pulse_width = menu_params.pulse_width > 655 ? menu_params.pulse_width - 655 : 0;
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Selects the next amplitude modulation slot.
 */
void amp_mod_slot_next(void)
{
    menu_params.amp_mod_slot = menu_params.amp_mod_slot == 0xFF ? 0 : (menu_params.amp_mod_slot < 15 ? menu_params.amp_mod_slot + 1 : 0xFF);
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Selects the previous amplitude modulation slot.
 */
void amp_mod_slot_prev(void)
{
    menu_params.amp_mod_slot = menu_params.amp_mod_slot == 0xFF ? 15 : (menu_params.amp_mod_slot > 0 ? menu_params.amp_mod_slot - 1 : 0xFF);
#ifdef CONFIG_ESPMENU_ENABLE_NVS
    extern bool param_changed;
    extern TickType_t last_param_change;
    param_changed = true;
    last_param_change = xTaskGetTickCount();
#endif
}

/**
 * @brief Selects the next favorite slot.
 */
void select_favorite_slot_next(void)
{
    current_slot = (current_slot + 1) % NUM_FAVORITE_SLOTS;
}

/**
 * @brief Selects the previous favorite slot.
 */
void select_favorite_slot_prev(void)
{
    current_slot = (current_slot + NUM_FAVORITE_SLOTS - 1) % NUM_FAVORITE_SLOTS;
}

/**
 * @brief Saves parameters to the current favorite slot.
 */
void save_favorite_action(void)
{
    save_favorite(current_slot);
}

/**
 * @brief Loads parameters from the current favorite slot.
 */
void load_favorite_action(void)
{
    load_favorite(current_slot);
}

/**
 * @brief Clears the current favorite slot.
 */
void clear_favorite_action(void)
{
    clear_favorite(current_slot);
}