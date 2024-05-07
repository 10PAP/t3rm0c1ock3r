#include "spi.h"
#include "stm32f0xx.h"
#include "display.h"

void init_spi(void) {
  // PB15 - SPI2_MOSI
  // PB13 - SPI2_SCK
  // PA8  - GPO_LE
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
  GPIOA->MODER &= ~GPIO_MODER_MODER8;
  GPIOA->MODER |= GPIO_MODER_MODER8_0;
  
  //GPIOB->AFR[0] = FnNumber << 4 * Pin;
  //GPIOB->AFR[1] = FnNumber << 4 * (Pin - 8);
  GPIOB->AFR[1] |= (0 << 4 * (13 - 8)) | (0 << 4 * (15 - 8)) | (0 << 4 * (14 - 8));
  GPIOB->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER15);
  GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;
  
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  
  SPI2->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_LSBFIRST;
  SPI2->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
  SPI2->CR1 |= SPI_CR1_MSTR;
  SPI2->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA;
  SPI2->CR2 = SPI_CR2_DS | SPI_CR2_RXNEIE;
  
  SPI2->CR1 |= SPI_CR1_SPE;
  
  NVIC_SetPriority(SPI2_IRQn, 0);
  NVIC_EnableIRQ(SPI2_IRQn);
  
  // start communication
  spi_refresh_display();
}

void spi_refresh_display() {
  DISPLAY_HIGH;
  DISPLAY_LOW;
  static uint8_t frameNumber = 0;
  uint16_t message = (display[frameNumber] << 8) | (1 << frameNumber);
  frameNumber = (frameNumber + 1) & 7;
  SPI2->DR = message;
}

void SPI2_IRQHandler(void) {
  if (SPI2->SR & SPI_SR_RXNE) {
    volatile uint16_t tmp = SPI2->DR;
    spi_refresh_display();
  }
}

