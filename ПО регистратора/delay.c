#include "delay.h"


volatile uint32_t msTicks = 0;

void SysTick_Handler(void) {
    msTicks++;
		Timeout_SysTick_Handler();
}

void Delay_ms(uint32_t ms) {
    uint32_t currentTicks = msTicks;
    while ((msTicks - currentTicks) < ms);
}

void Delay_us(uint32_t us) {
    volatile uint32_t count;
    while(us--) {
        for(count = 0; count < 8; count++);
    }
}
