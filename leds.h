#pragma once
#include "stm32f0xx.h"

const static uint8_t LED_RED    = 6;
const static uint8_t LED_BLUE   = 7;
const static uint8_t LED_ORANGE = 8;
const static uint8_t LED_GREEN  = 9;

const static uint8_t LED_UP     = 6;
const static uint8_t LED_DOWN   = 7;
const static uint8_t LED_LEFT   = 8;
const static uint8_t LED_RIGHT  = 9;


extern uint8_t led_state[2][2];

void init_leds(void);
void leds_refresh();