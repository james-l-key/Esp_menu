/**
 * @file user_actions.h
 * @brief Header file for user-defined menu actions and parameters
 * @ingroup ESP_menu
 *
 * This file contains the definitions for user-specific menu actions and
 * parameter structures related to the oscillator module. It defines the
 * data structures that are displayed and manipulated through the menu
 * interface.
 *
 * @note This is an example file. You should modify it to match your
 * project's specific requirements.
 */

#ifndef USER_ACTIONS_H
#define USER_ACTIONS_H

#include "lvgl.h"
#include "waveform_gen.h"

/**
 * @brief Structure for oscillator module parameters
 * @ingroup ESP_menu
 *
 * This structure holds all modifiable parameters for the oscillator module,
 * including frequency, waveform type, and modulation settings.
 */
typedef struct
{
    uint8_t frequency_pitch;  ///< MIDI note number (0–127)
    int16_t frequency_fine;   ///< Fine frequency adjustment in cents (-100 to 100)
    OscWaveform_t waveform;   ///< Waveform type (sine, triangle, saw, square, pulse)
    uint16_t level;           ///< Output level (0–65535)
    uint16_t pulse_width;     ///< Pulse width for pulse wave (0–65535)
    uint8_t amp_mod_slot;     ///< Amplitude modulation slot (0–15 or 0xFF)
    uint8_t freq_mod_slot;    ///< Frequency modulation slot (0–15 or 0xFF)
    uint8_t sync_source_slot; ///< Sync source slot (0–15 or 0xFF)
} MenuParams_t;

/** @brief Global oscillator parameters. */
extern MenuParams_t menu_params;

/**
 * @brief Initializes project-specific state, including loading from NVS if enabled.
 */
void user_init(void);

/**
 * @brief Updates the LVGL display with current parameters.
 */
void user_update_display(void);

/**
 * @brief Increments the frequency pitch.
 */
void pitch_up(void);

/**
 * @brief Decrements the frequency pitch.
 */
void pitch_down(void);

/**
 * @brief Selects the next waveform type.
 */
void waveform_next(void);

/**
 * @brief Selects the previous waveform type.
 */
void waveform_prev(void);

/**
 * @brief Increases the output level.
 */
void level_up(void);

/**
 * @brief Decreases the output level.
 */
void level_down(void);

/**
 * @brief Increments the fine frequency adjustment.
 */
void fine_tune_up(void);

/**
 * @brief Decrements the fine frequency adjustment.
 */
void fine_tune_down(void);

/**
 * @brief Increases the pulse width for pulse wave.
 */
void pulse_width_up(void);

/**
 * @brief Decreases the pulse width for pulse wave.
 */
void pulse_width_down(void);

/**
 * @brief Selects the next amplitude modulation slot.
 */
void amp_mod_slot_next(void);

/**
 * @brief Selects the previous amplitude modulation slot.
 */
void amp_mod_slot_prev(void);

/**
 * @brief Selects the next favorite slot.
 */
void select_favorite_slot_next(void);

/**
 * @brief Selects the previous favorite slot.
 */
void select_favorite_slot_prev(void);

/**
 * @brief Saves parameters to the current favorite slot.
 */
void save_favorite_action(void);

/**
 * @brief Loads parameters from the current favorite slot.
 */
void load_favorite_action(void);

/**
 * @brief Clears the current favorite slot.
 */
void clear_favorite_action(void);

/**
 * @brief Current favorite slot index (0–3).
 * This is exposed for NVS save/load functionality.
 */
extern uint8_t current_slot;

#endif