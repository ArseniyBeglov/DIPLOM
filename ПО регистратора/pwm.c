#include "pwm.h"

void PWM_PA5_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER &= ~(3 << (5 * 2));
    GPIOA->MODER |= (2 << (5 * 2));
    GPIOA->AFR[0] &= ~(0xF << (5 * 4));
    GPIOA->AFR[0] |= (2 << (5 * 4));

    TIM2->PSC = 0;
    TIM2->ARR = 1000 - 1;
    TIM2->CCR1 = 240;

    TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M);
    TIM2->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;

    TIM2->CCER |= TIM_CCER_CC1E;
    TIM2->CR1 |= TIM_CR1_CEN;
}
