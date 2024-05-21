#pragma once
#include "stm32f0xx.h"

void init_displayHD(void);
void displayHDSendCommand(uint8_t DB7_4);
void displayHDSendAscii(uint8_t v);
void displayHDSendString(uint8_t* s);
void displayHDToggleBlink(uint8_t val);