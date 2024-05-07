#include "rtc.h"
#include "display.h"

void init_rtc(void) {
    if(RTC->ISR & RTC_ISR_INITS)
      return;
    RCC->APB1ENR  |= RCC_APB1ENR_PWREN;
    PWR->CR       |= PWR_CR_DBP;
    RCC->BDCR     |= RCC_BDCR_BDRST;
    RCC->BDCR     &= ~RCC_BDCR_BDRST;
    
    RCC->BDCR |= RCC_BDCR_RTCSEL_1;
    RCC->BDCR |= RCC_BDCR_RTCEN;
    RCC->CSR |= RCC_CSR_LSION;
    while (!(RCC->CSR & RCC_CSR_LSIRDY));

    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR |= RTC_ISR_INIT;
    while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF);
    RTC->PRER = 0x007F0137;
    RTC->ISR &=~ RTC_ISR_INIT;

    RTC->WPR = 0xFE;
    RTC->WPR = 0x64;
    PWR->CR &= ~PWR_CR_DBP; 
}