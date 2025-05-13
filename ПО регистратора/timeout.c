#include "timeout.h"
#include "timeout.h"
#include "delay.h"
#include "graphic.h"
#include "button.h"
static volatile uint32_t timeout_counter = 0;
extern volatile uint8_t presses;

void Timeout_Init(void) {
    timeout_counter = 0;
}

void Timeout_Reset(void) {
    timeout_counter = 0;
}

void Timeout_SysTick_Handler(void) {
    if (timeout_counter < TIMEOUT_LIMIT_MS) {
        timeout_counter++;
    }
}

bool Timeout_IsExpired(void) {
    return timeout_counter >= TIMEOUT_LIMIT_MS;
}

void ShutDown_Device(void){
		GLCD_ShutDown_Device();
    GPIOA->MODER = 0xFFFFFFFF;
    GPIOB->MODER = 0xFFFFFFFF;

    for (int i = 2; i <= 15; i++) {
        GPIOC->MODER |= (3U << (i * 2)); 
    }


    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}