#include "stm32f0xx.h"
#include "storage.h"

#define FLASH_PAGE_ADDRESS  0x0801FC00  
#define FLASH_KEY1          0x45670123
#define FLASH_KEY2          0xCDEF89AB

void Storage_SaveDAC(uint16_t value) {

    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }

    FLASH->CR |= FLASH_CR_PER; 
    FLASH->AR = FLASH_PAGE_ADDRESS;
    FLASH->CR |= FLASH_CR_STRT; 
    while (FLASH->SR & FLASH_SR_BSY); 

    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_PG; 


    *(volatile uint16_t*)FLASH_PAGE_ADDRESS = value;
    while (FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK; 
}

uint16_t Storage_LoadDAC(void) {
    uint16_t val = *(volatile uint16_t*)FLASH_PAGE_ADDRESS;
    if (val == 0xFFFF || val > 4095) {
        return 1;
    }
    return val;
}
