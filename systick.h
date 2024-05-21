#pragma once
#include "stm32f0xx.h"

void init_systick();

extern uint8_t systick_passed;

extern volatile uint8_t ADC_accumulated;

extern volatile uint32_t globalSystickCounter;

void wait(uint32_t mls);