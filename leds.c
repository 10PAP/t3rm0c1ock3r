#include "leds.h"
#include "stm32f0xx.h"

uint8_t led_state[2][2] = {{0, 0}, {0, 0}};

void init_leds(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  
  // PC6 - PC9 - LEDs
  GPIOC->MODER &=
  ~(GPIO_MODER_MODER6 |
    GPIO_MODER_MODER7 |
    GPIO_MODER_MODER8 |
    GPIO_MODER_MODER9);
  GPIOC->MODER |=
    GPIO_MODER_MODER6_0 |
    GPIO_MODER_MODER7_0 |
    GPIO_MODER_MODER8_0 |
    GPIO_MODER_MODER9_0;
}


static void led(uint8_t state, uint8_t ledIdx) {
  switch (ledIdx) {
  case LED_RED:
    GPIOC->BSRR = state ? GPIO_BSRR_BS_6 : GPIO_BSRR_BR_6;
    break;
  case LED_BLUE:
    GPIOC->BSRR = state ? GPIO_BSRR_BS_7 : GPIO_BSRR_BR_7;
    break;
  case LED_ORANGE:
    GPIOC->BSRR = state ? GPIO_BSRR_BS_8 : GPIO_BSRR_BR_8;
    break;
  case LED_GREEN:
    GPIOC->BSRR = state ? GPIO_BSRR_BS_9 : GPIO_BSRR_BR_9;
    break;
  }
}

void leds_refresh() {
  led(led_state[0][0], LED_RIGHT);
  led(led_state[0][1], LED_LEFT);
  led(led_state[1][0], LED_UP);
  led(led_state[1][1], LED_DOWN);
}

static uint8_t next_ledidx(uint8_t ledIdx, uint8_t cw) {
  switch (ledIdx) {
  case LED_UP:
    return cw ? LED_RIGHT : LED_LEFT;
  case LED_DOWN:
    return cw ? LED_LEFT : LED_RIGHT;
  case LED_LEFT:
    return cw ? LED_UP : LED_DOWN;
  case LED_RIGHT:
    return cw ? LED_DOWN : LED_UP;
  }
}
