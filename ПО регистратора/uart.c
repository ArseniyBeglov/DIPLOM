#include "uart.h"
#include "button.h" 
#include <string.h>


#define RX_BUFFER_SIZE 32
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_index = 0;


void UART1_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    GPIOA->MODER &= ~((3U << (9 * 2)) | (3U << (10 * 2))); 
    GPIOA->MODER |=  ((2U << (9 * 2)) | (2U << (10 * 2))); 

    GPIOA->AFR[1] &= ~((0xF << ((9 - 8) * 4)) | (0xF << ((10 - 8) * 4)));
    GPIOA->AFR[1] |=  ((1 << ((9 - 8) * 4)) | (1 << ((10 - 8) * 4))); 

    USART1->BRR = 8000000 / 9600;
    USART1->CR1 |= USART_CR1_RE | USART_CR1_TE;    
    USART1->CR1 |= USART_CR1_RXNEIE;               
    USART1->CR1 |= USART_CR1_UE;                    

    NVIC_EnableIRQ(USART1_IRQn);                    
}


void UART1_SendChar(char ch) {
    while (!(USART1->ISR & USART_ISR_TXE));
    USART1->TDR = ch;
}

void UART1_SendString(const char *str) {
    while (*str) {
        UART1_SendChar(*str++);
    }
}

void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE) {
        char received = USART1->RDR;

        if (received == '\r' || received == '\n') {
            rx_buffer[rx_index] = '\0'; 
            if (strcmp(rx_buffer, "Calibration") == 0) {
                presses = 2;  
            }	

            rx_index = 0; 
        } else {
            if (rx_index < RX_BUFFER_SIZE - 1) {
                rx_buffer[rx_index++] = received;
            } else {
                rx_index = 0; 
            }
        }
    }
}