#pragma once
#include "stm32f0xx.h"
#define ADC_DMA_BUFFSIZE (4096)
void init_adc(void);
extern volatile int8_t ADC_half;
extern volatile uint16_t ADC_array[ADC_DMA_BUFFSIZE];