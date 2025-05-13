#include "dac.h"

void DAC_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    GPIOA->MODER |= GPIO_MODER_MODER4;
}
