#include <stm32f0xx.h>
#include <stdint.h>
#include <math.h>

#define AFR1 0b0001

#define RESET_BAUD_SPEED   9600
#define MESSAGE_BAUD_SPEED 115200

#define ONE_WIRE_0 0x00
#define ONE_WIRE_1 0xFF

#define T_CONV 750
#define RESOLUTION 12
#define ANSW_SIZE 9

static void SysTick_init() {
	SysTick->VAL  = 0;
	SystemCoreClockUpdate();
	SysTick->LOAD = SystemCoreClock / T_CONV - 1;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}

void block_until_tc() {
	while ((USART1->ISR & USART_ISR_TC) == 0) {}
}

uint8_t TERMO_transmit_receive(uint8_t byte) {
	volatile uint8_t data = 0;
	
	USART1->TDR = byte;
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

uint8_t TERMO_get_byte() {
	uint8_t byte = 0;
	for (int i = 0; i < 8; i++) {
		uint8_t bit = TERMO_receive();
		if (bit == 0xFF) {
				bit = 1;
		} else {
				bit = 0;
		}
		byte = byte | (bit << i);
	}
	return byte;
}

void TERMO_skip_rom() {
	TERMO_send_byte(0xCC);
}

float TERMO_toTemp(uint32_t answer) {
	float res = 0;
	for (int i = 0; i < RESOLUTION; i++) {
		uint8_t bit = answer & 0x1;
		res += bit * pow(2, i-4);
		answer >>= 1;
	}
	return res;
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
	
	/* Oversampling by 16 */
	/* Less significant bit first in transmit/receive */
	/* 8 data bit, 1 start bit, 1 stop bit, no parity */
	SystemCoreClockUpdate();
	USART1->BRR = SystemCoreClock / RESET_BAUD_SPEED;
	
	USART1->CR1 = USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RE; /* (3) */
	
	UART_enable();
}

void init() {
	SysTick_init();
	UART_init();
}

#define tick long
volatile tick current_tick;

void SysTick_Handler() {
		current_tick++;
}

typedef enum State {
	PREPARING,
	WAITING,
	READY
} State;

int main() {
	init();
	
	State s = PREPARING;
	tick start_tick;
	for (;;) {
		switch (s) {
			case PREPARING:
				UART_change_baud_rate(RESET_BAUD_SPEED);
				TERMO_reset();
				
				UART_change_baud_rate(MESSAGE_BAUD_SPEED);
				TERMO_skip_rom();
				
				TERMO_send_byte(0x44); // ConvertT
				
				s = WAITING;
				start_tick = current_tick;
				break;
			case WAITING:
				if (current_tick - start_tick >= 2) {
					s = READY;
				}
				break;
			case READY:
				UART_change_baud_rate(RESET_BAUD_SPEED);
				TERMO_reset();
			
				UART_change_baud_rate(MESSAGE_BAUD_SPEED);
				TERMO_skip_rom();
			
				TERMO_send_byte(0xBE); // Read Scratchpad
				
				uint8_t buf[ANSW_SIZE];
				for (int i = 0; i < ANSW_SIZE; i++) {
					buf[i] = TERMO_get_byte();
				}
				
				volatile float temp = TERMO_toTemp(buf[0] | (buf[1] << 8));
				
				UART_change_baud_rate(RESET_BAUD_SPEED);
				TERMO_reset();
				s = PREPARING;
				break;
		}
	}
}
