#ifndef ADC_H
#define ADC_H

#include "stm32f0xx.h"
#include "delay.h"

void ADC_Init(void);
uint16_t ADC_Read_Channel(uint8_t channel);

#endif
