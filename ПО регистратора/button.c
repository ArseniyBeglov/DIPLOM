#include "button.h"

volatile uint8_t presses = 0;
volatile bool doFlag = false;
extern volatile bool useRussian;


static volatile uint8_t button_pending = 0; 

void Button_Init(void) {
    RCC->AHBENR   |= RCC_AHBENR_GPIOCEN;
    RCC->APB2ENR  |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR  |= RCC_APB1ENR_TIM14EN;

    GPIOC->MODER  &= ~((3U << (0*2)) | (3U << (1*2)));
    GPIOC->PUPDR  &= ~((3U << (0*2)) | (3U << (1*2)));
    GPIOC->PUPDR  |=  ((1U << (0*2)) | (1U << (1*2)));  

    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0 | SYSCFG_EXTICR1_EXTI1);
    SYSCFG->EXTICR[0] |=  (SYSCFG_EXTICR1_EXTI0_PC | SYSCFG_EXTICR1_EXTI1_PC);

    EXTI->IMR   |= EXTI_IMR_MR0 | EXTI_IMR_MR1;
    EXTI->FTSR  |= EXTI_FTSR_TR0 | EXTI_FTSR_TR1;
    EXTI->RTSR  &= ~(EXTI_RTSR_TR0 | EXTI_RTSR_TR1);
    EXTI->PR     =  EXTI_PR_PR0 | EXTI_PR_PR1;

    NVIC_SetPriority(EXTI0_1_IRQn, 2);
    NVIC_EnableIRQ(EXTI0_1_IRQn);

    TIM14->PSC   = SystemCoreClock / 1000 - 1;
    TIM14->ARR   = 50; 
    TIM14->CR1   = TIM_CR1_OPM;
    TIM14->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(TIM14_IRQn, 3);
    NVIC_EnableIRQ(TIM14_IRQn);
}

void EXTI0_1_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;
        EXTI->IMR &= ~EXTI_IMR_MR0;
        button_pending = 1;
        TIM14->CNT = 0;
        TIM14->CR1 |= TIM_CR1_CEN;
    }
    if (EXTI->PR & EXTI_PR_PR1) {
        EXTI->PR = EXTI_PR_PR1;
        EXTI->IMR &= ~EXTI_IMR_MR1;
        button_pending = 2;
        TIM14->CNT = 0;
        TIM14->CR1 |= TIM_CR1_CEN;
    }
}

extern volatile bool useRussian;

void TIM14_IRQHandler(void) {
    if (TIM14->SR & TIM_SR_UIF) {
        TIM14->SR &= ~TIM_SR_UIF;
        TIM14->CR1 &= ~TIM_CR1_CEN;

        uint8_t pc0 = (GPIOC->IDR & (1 << 0)) == 0;
        uint8_t pc1 = (GPIOC->IDR & (1 << 1)) == 0;

        if (pc0 && pc1) {
            useRussian = !useRussian;
						presses = 0;
        } else {
            if (pc0 && button_pending == 1) {
                presses++;
                if (presses > 2) presses = 0;
            }
            if (pc1 && button_pending == 2) {
                doFlag = true;
            }
        }

        EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1;
        button_pending = 0;
    }
}
