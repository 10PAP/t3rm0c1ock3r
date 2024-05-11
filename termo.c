#include <stm32f0xx.h>
#include <stdint.h>

#define AFR1 0b0001

#define RESET_BAUD_SPEED   9600
#define MESSAGE_BAUD_SPEED 115200

#define ONE_WIRE_0 0x00
#define ONE_WIRE_1 0xFF

static void SysTick_init() {
	SysTick->VAL  = 0;
	SystemCoreClockUpdate();
	SysTick->LOAD = SystemCoreClock / 10 - 1;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void block_until_tc() {
	while ((USART1->ISR & USART_ISR_TC) == 0) {}
}

uint8_t TERMO_transmit_receive(uint8_t byte) {
	volatile uint8_t data = 0;
	
	USART1->TDR = bit;
	block_until_tc();
	
	volatile uint8_t answer = USART1->RDR;
	return answer;
}

uint8_t TERMO_receive() {
	return TERMO_transmit_receive(ONE_WIRE_1);
}

uint8_t TERMO_reset() {
	return TERMO_transmit_receive(0xF0);
}

void TERMO_send_byte(uint8_t byte) {
	for (int i = 0; i < 8; i++) {
		uint8_t bit = (byte & (1 << i)) >> i;
		switch (bit) {
			case 0:
				TERMO_transmit_receive(ONE_WIRE_0);
				break;
			case 1:
				TERMO_transmit_receive(ONE_WIRE_1);
				break;
		}
	}
}

void TERMO_skip_rom() {
	TERMO_send_byte(0xCC);
}

void UART_enable() {
	USART1->CR1 |= USART_CR1_UE;
	block_until_tc();
}

void UART_disable() {
	block_until_tc();
	USART1->CR1 &= ~USART_CR1_UE;
}

void UART_change_baud_rate(uint32_t rate) {
	UART_disable();
	
	SystemCoreClockUpdate();
	USART1->BRR = SystemCoreClock / rate;
	
	UART_enable();
}

void UART_init() {
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
	
	// Init pins
	GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT_9;
	GPIOA->PUPDR  |= GPIO_PUPDR_PUPDR10_0;
	
	// GPIOB->AFR[0] |= Fn << 4 * Pin;
	// GPIOB->AFR[1] |= Fn << 4 * (Pin - 8);
	GPIOA->AFR[1] |= AFR1 << 4 * (9 - 8);
	GPIOA->AFR[1] |= AFR1 << 4 * (10 - 8);
	
	/* Oversampling by 16, 9600 baud */
	/* Less significant bit first in transmit/receive */
	/* 8 data bit, 1 start bit, 1 stop bit, no parity
	 Transmission enabled, reception enabled */
	SystemCoreClockUpdate();
	USART1->BRR = SystemCoreClock / RESET_BAUD_SPEED;
	
	USART1->CR1 = USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RE; /* (3) */
	
	UART_enable();
}

void init() {
	UART_init();
}

int main() {
	init();
	
	for (;;) {
		UART_change_baud_rate(RESET_BAUD_SPEED);
		TERMO_reset();
		
		UART_change_baud_rate(MESSAGE_BAUD_SPEED);
		TERMO_skip_rom();
		TERMO_send_byte(0x44);
		
		
	}
}