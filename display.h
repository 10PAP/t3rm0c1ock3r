#pragma once
#include "stm32f0xx.h"

#define DISPLAY_HIGH (GPIOA->BSRR = GPIO_BSRR_BS_8)
#define DISPLAY_LOW (GPIOA->BSRR = GPIO_BSRR_BR_8)

typedef struct Position {
  int8_t x, y;
} Position;

extern Position player;
extern uint8_t display[8];
void display_clear(void);
void display_draw_player(Position* player);

void init_display(void);
void draw_level(uint16_t level);