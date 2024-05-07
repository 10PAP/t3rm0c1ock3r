#include "stm32f0xx.h"
#include "systick.h"
#include "leds.h"


void init_btns() {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
  
  // PC12 - bottom keyboard line
  GPIOC->MODER &= ~GPIO_MODER_MODER12;
  GPIOC->MODER |= GPIO_MODER_MODER12_0;
  
  // PA0 - button
  // PA4, PA5 - keyboard stocks (left/right)
  GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER15);
  
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5);
  GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR4_1 | GPIO_PUPDR_PUPDR5_1);
  
  // PA15 - top keyboard line
  GPIOA->MODER |= GPIO_MODER_MODER15_0;
}



uint8_t btn_mat_state[2][2] = {{0, 0}, {0, 0}};
void read_btns() {
  static uint8_t kline = 0;
  static uint8_t btn_mat_ticks[2][2] = {{0, 0}, {0, 0}};

  GPIOA->BSRR = !kline ? GPIO_BSRR_BS_15 : GPIO_BSRR_BR_15;
  GPIOC->BSRR = kline ? GPIO_BSRR_BS_12 : GPIO_BSRR_BR_12;  
  kline = kline == 0 ? 1 : 0;

  uint8_t b1 = (GPIOA->IDR & GPIO_IDR_4) ? 1 : 0;
  uint8_t b2 = (GPIOA->IDR & GPIO_IDR_5) ? 1 : 0;

  if (b1 != btn_mat_state[kline][0])
    ++btn_mat_ticks[kline][0];
  else btn_mat_ticks[kline][0] = 0;
  
  if (b2 != btn_mat_state[kline][1])
    ++btn_mat_ticks[kline][1];
  else btn_mat_ticks[kline][1] = 0;
  
  if (btn_mat_ticks[kline][0] == 10)
    btn_mat_state[kline][0] = b1;
  if (btn_mat_ticks[kline][1] == 10)
    btn_mat_state[kline][1] = b2;
}

static uint8_t user_button_read() {
  return (GPIOA->IDR & GPIO_IDR_0) ? 1 : 0;
}
