#include "rtc.h"
#include "display.h"

void init_rtc(void) {
  if (RTC->ISR & RTC_ISR_INITS)
    return;
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  PWR->CR |= PWR_CR_DBP;
  RCC->BDCR |= RCC_BDCR_BDRST;
  RCC->BDCR &= ~RCC_BDCR_BDRST;

  RCC->BDCR |= RCC_BDCR_RTCSEL_1;
  RCC->BDCR |= RCC_BDCR_RTCEN;
  RCC->CSR |= RCC_CSR_LSION;
  while (!(RCC->CSR & RCC_CSR_LSIRDY))
    ;

  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  RTC->ISR |= RTC_ISR_INIT;
  while ((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF)
    ;
  RTC->PRER = 0x007F0137;
  RTC->ISR &= ~RTC_ISR_INIT;

  RTC->WPR = 0xFE;
  RTC->WPR = 0x64;
  PWR->CR &= ~PWR_CR_DBP;
}

uint32_t rtcEncode(const sTime *t) {
  return ((t->hours / 10) << RTC_TR_HT_Pos) |
         ((t->hours % 10) << RTC_TR_HU_Pos) |
         ((t->minutes / 10) << RTC_TR_MNT_Pos) |
         ((t->minutes % 10) << RTC_TR_MNU_Pos) |
         ((t->seconds / 10) << RTC_TR_ST_Pos) |
         ((t->seconds % 10) << RTC_TR_SU_Pos);
}

uint32_t rtcDecode(uint32_t iTime, sTime *oTime) {
  oTime->hours = 10 * ((iTime & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos) +
                 ((iTime & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
  oTime->minutes = 10 * ((iTime & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos) +
                   ((iTime & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
  oTime->seconds = 10 * ((iTime & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos) +
                   ((iTime & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
}
// https://microtechnics.ru/profilegrid_blogs/chast-16-tajmer-realnogo-vremeni-rtc-na-c/
