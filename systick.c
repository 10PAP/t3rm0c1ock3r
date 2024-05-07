#include "stm32f0xx.h"
#include "systick.h"
#include "buttons.h"
#include "spi.h"
#include "display.h"
#include "leds.h"
#include "adc.h"

void init_systick() {
    SystemCoreClockUpdate();
    SysTick->LOAD = SystemCoreClock / 1000 - 1; // reload value
    SysTick->VAL = 0; // current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

static void check_tick() {
  uint8_t systick_passed = SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk ? 1 : 0;
}

int cnt = 0;
volatile uint8_t ADC_accumulated = 0;
void SysTick_Handler(void){
  read_btns();
  ++cnt;
  if (cnt == 100) {
    cnt = 0;
    ADC_accumulated = 1;
  }
}

