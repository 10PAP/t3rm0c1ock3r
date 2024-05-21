#include "displayHD.h"
#include "systick.h"

void init_displayHD(void) {
  /*
  EN - PC4
  RS - PB12
  RW - PB3
  D4 - PB4
  D5 - PB5
  D6 - PB6
  D7 - PB7
  подсветка - PC6
  */
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

  GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5 |
                    GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER12);
  GPIOB->MODER |= GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 |
                  GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 |
                  GPIO_MODER_MODER7_0 | GPIO_MODER_MODER12_0;

  GPIOC->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER6);
  GPIOC->MODER |= GPIO_MODER_MODER4_0 | GPIO_MODER_MODER6_0;

  GPIOC->BSRR = GPIO_BSRR_BS_6;  // подсветка = 1
  GPIOB->BSRR = GPIO_BSRR_BR_3;  // RW = 0
  GPIOB->BSRR = GPIO_BSRR_BR_12; // RS = 0
  wait(30);
  displayHDSendCommand(0x3);
  wait(10);
  displayHDSendCommand(0x3);
  wait(1);
  displayHDSendCommand(0x3);

  // rows, columns, font
  displayHDSendCommand(0x2); // Function set (Set interface to be 4 bits long.)

  displayHDSendCommand(0x2);
  displayHDSendCommand(0x8); // NF**

  // display off
  displayHDSendCommand(0x0);  // 0000
  displayHDSendCommand(0b1100); // 1000

  // display clear
  displayHDSendCommand(0x0); // 0000
  displayHDSendCommand(0x1); // 0001

  // entry mode set
  displayHDSendCommand(0x0); // 0 0 0 0
  displayHDSendCommand(0x6); // 0 1 I/D S

  GPIOB->BSRR = GPIO_BSRR_BS_12; // RS = 1
  displayHDSendString("HELLO"); 
}

/*
EN - PC4
RS - PB12
RW - PB3
D4 - PB4
D5 - PB5
D6 - PB6
D7 - PB7
подсветка - PC6
*/

void displayHDSendCommand(uint8_t msg) {
  GPIOC->BSRR = GPIO_BSRR_BS_4;
  wait(1);

  GPIOB->ODR &= ~(GPIO_ODR_4 | GPIO_ODR_5 | GPIO_ODR_6 | GPIO_ODR_7);
  GPIOB->ODR |= (msg << 4);

  wait(1);
  GPIOC->BSRR = GPIO_BSRR_BR_4;
  wait(1);
}

void displayHDSendAscii(uint8_t v) {
  displayHDSendCommand(v >> 4);
  displayHDSendCommand(v & 0xF);
}

void displayHDSendString(uint8_t* s) {
  GPIOB->BSRR = GPIO_BSRR_BR_12; // RS = 0
  displayHDSendCommand(0x0); // 0000
  displayHDSendCommand(0x2); // 0001
  GPIOB->BSRR = GPIO_BSRR_BS_12; // RS = 0

  for (;*s;++s)
    displayHDSendAscii(*s); 
}

void displayHDToggleBlink(uint8_t val) {
  GPIOB->BSRR = GPIO_BSRR_BR_12; // RS = 0
  displayHDSendCommand(0x0); // 0000

  if (val) displayHDSendCommand(0xF); // 0001
  else displayHDSendCommand(0x1000); // 0001
  GPIOB->BSRR = GPIO_BSRR_BS_12; // RS = 0
  
}