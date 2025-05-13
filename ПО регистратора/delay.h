#ifndef DELAY_H
#define DELAY_H
#include "timeout.h"
#include "stm32f0xx.h"

void Delay_ms(uint32_t ms);
void Delay_us(uint32_t us);
void SysTick_Handler(void);

#endif
