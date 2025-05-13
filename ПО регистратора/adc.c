#include "adc.h"

void ADC_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOA->MODER |= GPIO_MODER_MODER0 | GPIO_MODER_MODER1; 

    ADC1->CR &= ~ADC_CR_ADEN;
    ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    ADC1->SMPR |= ADC_SMPR_SMP_0;

    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);

    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint16_t ADC_Read_Channel(uint8_t channel) {
    ADC1->CHSELR = (1 << channel); 
    Delay_ms(1);                   
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));
    (void)ADC1->DR;                 

    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));
    return ADC1->DR;
}

