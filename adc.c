#include "adc.h"
#include "display.h"
#include "stm32f0xx.h"

volatile uint16_t ADC_array[ADC_DMA_BUFFSIZE];

void init_adc(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA->MODER |= GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1;

  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_OVRMOD;
  ADC1->CFGR2 = ADC_CFGR2_CKMODE_0;
  ADC1->CHSELR = ADC_CHSELR_CHSEL1;
  ADC1->SMPR = ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;

  // calib
  ADC1->CR |= ADC_CR_ADCAL;
  while ((ADC1->CR & ADC_CR_ADCAL) != 0)
    ;
  // calib

  // dma
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
  DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));
  DMA1_Channel1->CMAR = (uint32_t)(ADC_array);
  DMA1_Channel1->CNDTR = ADC_DMA_BUFFSIZE;
  DMA1_Channel1->CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 |
                        DMA_CCR_CIRC | DMA_CCR_TCIE | DMA_CCR_HTIE;
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
  DMA1_Channel1->CCR |= DMA_CCR_EN;
  // dma

  // start
  if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) {
    ADC1->ISR |= ADC_ISR_ADRDY;
  }
  ADC1->CR |= ADC_CR_ADEN;
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0)
    ;

  ADC1->CR |= ADC_CR_ADSTART;
}

void ADC1_COMP_IRQHandler(void) {
  if (ADC1->ISR & ADC_ISR_OVR)
    ADC1->ISR |= ADC_ISR_OVR;
}

volatile int8_t ADC_half = -1;
void DMA1_Channel1_IRQHandler(void) {
  if (DMA1->ISR & DMA_ISR_HTIF1) {
    DMA1->IFCR |= DMA_IFCR_CHTIF1;
    ADC_half = 0;
  } else if (DMA1->ISR & DMA_ISR_TCIF1) {
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
    ADC_half = 1;
  }
}