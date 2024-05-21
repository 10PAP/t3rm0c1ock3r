#include "systick.h"
#include "adc.h"
#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "spi.h"
#include "stm32f0xx.h"


void init_systick() {
  SystemCoreClockUpdate();
  SysTick->LOAD = SystemCoreClock / 1000 - 1; // reload value
  SysTick->VAL = 0;                           // current value
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk |
                  SysTick_CTRL_TICKINT_Msk;
}

static void check_tick() {
  uint8_t systick_passed = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk ? 1 : 0;
}

int cnt = 0;
volatile uint8_t ADC_accumulated = 0;
volatile uint32_t globalSystickCounter = 0;
void SysTick_Handler(void) {
  ++globalSystickCounter;
  read_btns();
  ++cnt;
  if (cnt == 100) {
    cnt = 0;
    ADC_accumulated = 1;
  }
}

void wait(uint32_t mls) {
  uint32_t timerState = globalSystickCounter;
  while (globalSystickCounter - timerState < mls);
}
