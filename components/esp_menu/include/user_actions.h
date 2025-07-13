/**
 * @file user_actions.h
 * @brief Header file for user-defined menu actions and parameters
 * @ingroup ESP_menu
 *
 * This file contains the definitions for user-specific menu actions and
 * parameter structures. It defines the data structures that are displayed 
 * and manipulated through the menu interface.
 *
 * @note This is an example file. You should modify it to match your
 * project's specific requirements.
 */

#ifndef USER_ACTIONS_H
#define USER_ACTIONS_H

#include "lvgl.h"
#include "esp_err.h"

/**
 * @brief Structure for example menu parameters
 * @ingroup ESP_menu
 *
 * This structure holds all modifiable parameters for the example,
 * including various settings that can be adjusted through the menu.
 */
typedef struct
{
    uint8_t setting1;      ///< Example setting 1 (0-100)
    uint8_t setting2;      ///< Example setting 2 (0-255)
    uint16_t setting3;     ///< Example setting 3 (0-1000)
    bool toggle1;          ///< Example toggle setting
    bool toggle2;          ///< Another example toggle
    uint8_t option;        ///< Selected option (0-5)
} menu_params_t;

/**
 * @brief Global parameters structure
 */
extern menu_params_t menu_params;

/**
 * @brief Current favorite slot index
 */
extern uint8_t current_slot;

/**
 * @brief Initialize menu parameters with default values
 */
void init_menu_params(void);

/**
 * @brief Save menu parameters to NVS
 * 
 * @return ESP_OK if successful, or an error code
 */
esp_err_t user_save_params_to_nvs(void);

/**
 * @brief Load menu parameters from NVS
 * 
 * @return ESP_OK if successful, or an error code
 */
esp_err_t user_load_params_from_nvs(void);

/**
 * @brief Example action function for setting 1
 * 
 * @param value New value for setting 1
 */
void set_setting1(uint8_t value);

/**
 * @brief Example action function for setting 2
 * 
 * @param value New value for setting 2
 */
void set_setting2(uint8_t value);

/**
 * @brief Example action function for setting 3
 * 
 * @param value New value for setting 3
 */
void set_setting3(uint16_t value);

/**
 * @brief Example action function for toggle 1
 * 
 * @param state New state for toggle 1
 */
void set_toggle1(bool state);

/**
 * @brief Example action function for toggle 2
 * 
 * @param state New state for toggle 2
 */
void set_toggle2(bool state);

/**
 * @brief Example action function for option selection
 * 
 * @param option_index Selected option index
 */
void set_option(uint8_t option_index);

#endif /* USER_ACTIONS_H */