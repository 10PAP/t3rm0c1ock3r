#pragma once
#include "stm32f0xx.h"
void init_rtc(void);

typedef struct sTime {
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
} sTime;

uint32_t rtcEncode(const sTime* iTime);
uint32_t rtcDecode(uint32_t iTime, sTime* oTime);