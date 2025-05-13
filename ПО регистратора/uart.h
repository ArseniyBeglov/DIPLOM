#ifndef UART_H
#define UART_H

#include "stm32f0xx.h"

void UART1_Init(void);
void UART1_SendChar(char ch);
void UART1_SendString(const char *str);
void USART1_IRQHandler(void);
#endif
