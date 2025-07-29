#ifndef MENU_DATA_H
#define MENU_DATA_H

#include "lvgl.h"


void amp_mod_slot_next(void);

void amp_mod_slot_prev(void);

void clear_favorite_action(void);

void fine_tune_down(void);

void fine_tune_up(void);

void level_down(void);

void level_up(void);

void load_favorite_action(void);

void pitch_down(void);

void pitch_up(void);

void pulse_width_down(void);

void pulse_width_up(void);

void save_favorite_action(void);

void select_favorite_slot_next(void);

void select_favorite_slot_prev(void);

void waveform_next(void);

void waveform_prev(void);


void menu_init(void);

#endif