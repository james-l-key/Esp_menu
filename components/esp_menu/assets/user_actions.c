/**
 * @file user_actions.c
 * @brief Implementation of user-defined menu actions
 *
 * This file contains the implementation of user-specific menu actions
 * and parameter handling for the ESP Menu system.
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "user_actions.h"

static const char *TAG = "USER_ACTIONS";

// Global parameters structure
menu_params_t menu_params;

// Current favorite slot index
uint8_t current_slot = 0;

// NVS namespace for storing menu parameters
#define STORAGE_NAMESPACE "esp_menu"

void init_menu_params(void) {
    // Set default values
    menu_params.setting1 = 50;
    menu_params.setting2 = 128;
    menu_params.setting3 = 500;
    menu_params.toggle1 = false;
    menu_params.toggle2 = true;
    menu_params.option = 0;

    // Try to load saved values from NVS
    ESP_LOGI(TAG, "Initializing menu parameters");
    esp_err_t err = user_load_params_from_nvs();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to load parameters from NVS, using defaults");
    }
}

esp_err_t user_save_params_to_nvs(void) {
    ESP_LOGI(TAG, "Saving parameters to NVS");

    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Open NVS
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS (%s)", esp_err_to_name(err));
        return err;
    }

    // Save parameters
    err = nvs_set_u8(nvs_handle, "setting1", menu_params.setting1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving setting1 (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u8(nvs_handle, "setting2", menu_params.setting2);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving setting2 (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u16(nvs_handle, "setting3", menu_params.setting3);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving setting3 (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u8(nvs_handle, "toggle1", menu_params.toggle1 ? 1 : 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving toggle1 (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u8(nvs_handle, "toggle2", menu_params.toggle2 ? 1 : 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving toggle2 (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    err = nvs_set_u8(nvs_handle, "option", menu_params.option);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error saving option (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    // Commit changes
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing NVS (%s)", esp_err_to_name(err));
        nvs_close(nvs_handle);
        return err;
    }

    // Close NVS
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Parameters saved to NVS");
    return ESP_OK;
}

esp_err_t user_load_params_from_nvs(void) {
    ESP_LOGI(TAG, "Loading parameters from NVS");

    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Open NVS
    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS (%s)", esp_err_to_name(err));
        return err;
    }

    // Load parameters
    uint8_t u8_val;
    uint16_t u16_val;

    err = nvs_get_u8(nvs_handle, "setting1", &u8_val);
    if (err == ESP_OK) {
        menu_params.setting1 = u8_val;
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading setting1 (%s)", esp_err_to_name(err));
    }

    err = nvs_get_u8(nvs_handle, "setting2", &u8_val);
    if (err == ESP_OK) {
        menu_params.setting2 = u8_val;
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading setting2 (%s)", esp_err_to_name(err));
    }

    err = nvs_get_u16(nvs_handle, "setting3", &u16_val);
    if (err == ESP_OK) {
        menu_params.setting3 = u16_val;
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading setting3 (%s)", esp_err_to_name(err));
    }

    err = nvs_get_u8(nvs_handle, "toggle1", &u8_val);
    if (err == ESP_OK) {
        menu_params.toggle1 = (u8_val != 0);
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading toggle1 (%s)", esp_err_to_name(err));
    }

    err = nvs_get_u8(nvs_handle, "toggle2", &u8_val);
    if (err == ESP_OK) {
        menu_params.toggle2 = (u8_val != 0);
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading toggle2 (%s)", esp_err_to_name(err));
    }

    err = nvs_get_u8(nvs_handle, "option", &u8_val);
    if (err == ESP_OK) {
        menu_params.option = u8_val;
    } else if (err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Error loading option (%s)", esp_err_to_name(err));
    }

    // Close NVS
    nvs_close(nvs_handle);
    ESP_LOGI(TAG, "Parameters loaded from NVS");
    return ESP_OK;
}

void set_setting1(uint8_t value) {
    ESP_LOGI(TAG, "Setting 1 changed to %u", value);
    menu_params.setting1 = value;
    // Implement any action needed when this setting changes
}

void set_setting2(uint8_t value) {
    ESP_LOGI(TAG, "Setting 2 changed to %u", value);
    menu_params.setting2 = value;
    // Implement any action needed when this setting changes
}

void set_setting3(uint16_t value) {
    ESP_LOGI(TAG, "Setting 3 changed to %u", value);
    menu_params.setting3 = value;
    // Implement any action needed when this setting changes
}

void set_toggle1(bool state) {
    ESP_LOGI(TAG, "Toggle 1 changed to %s", state ? "ON" : "OFF");
    menu_params.toggle1 = state;
    // Implement any action needed when this toggle changes
}

void set_toggle2(bool state) {
    ESP_LOGI(TAG, "Toggle 2 changed to %s", state ? "ON" : "OFF");
    menu_params.toggle2 = state;
    // Implement any action needed when this toggle changes
}

void set_option(uint8_t option_index) {
    ESP_LOGI(TAG, "Option changed to %u", option_index);
    menu_params.option = option_index;
    // Implement any action needed when this option changes
}
