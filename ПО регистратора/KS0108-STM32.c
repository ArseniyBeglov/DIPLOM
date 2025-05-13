#include "stm32f0xx.h"

#define GLCD_DATA_PORT     GPIOB
#define GLCD_CTRL_PORT     GPIOC

#define GLCD_RS_PIN        9
#define GLCD_RW_PIN        8
#define GLCD_EN_PIN        10
#define GLCD_CS1_PIN       6
#define GLCD_CS2_PIN       7
#define GLCD_RST_PIN       11


#define DISPLAY_STATUS_BUSY 0x80

#define DATA_TO_OUT GLCD_DATA_PORT->MODER = 0b0101010101010101;
#define CTRL_TO_OUT GLCD_CTRL_PORT->MODER = 0x00555000;

extern unsigned char screen_x;
extern unsigned char screen_y;
uint8_t status;


void GLCD_Delay(void) {
    for (volatile int i = 0; i < 50; ++i) __NOP();
}

// ----------------------------------------------------------------------------

void GLCD_EnableController(uint8_t controller) {
    if (controller == 0)
        GPIOC->BRR = (1 << 6); // E1
    else
        GPIOC->BRR = (1 << 7); // E2
}


void GLCD_DisableController(uint8_t controller) {
    if (controller == 0)
        GPIOC->BSRR = (1 << 6);
    else
        GPIOC->BSRR = (1 << 7);
}

// ----------------------------------------------------------------------------
// ?????? ???????
uint8_t GLCD_ReadStatus(uint8_t controller) {
		GPIOC->BRR = (1 << GLCD_EN_PIN);   // EN = 0
		GPIOC->BRR = (1 << GLCD_RW_PIN);  // RW = 0
		GPIOC->BRR = (1 << 9);  // RS=0
    GLCD_DATA_PORT->MODER = 0x00;
    GLCD_DATA_PORT->PUPDR = 0b1010101010101010;

    GPIOC->BSRR = (1 << GLCD_RW_PIN);  // RW = 1


    GLCD_EnableController(controller);
    __NOP();
    GPIOC->BSRR = (1 << GLCD_EN_PIN);  // EN = 1
    __NOP();

    

    GPIOC->BRR = (1 << GLCD_EN_PIN);   // EN = 0
		//__NOP();
		GLCD_Delay();
		status = (uint8_t)(GLCD_DATA_PORT->IDR & 0xFF);
	  GPIOC->BRR = (1 << GLCD_RW_PIN);  // RW = 0
    GLCD_DisableController(controller);
    return status;
}


// ----------------------------------------------------------------------------

void GLCD_WriteCommand(uint8_t cmd, uint8_t controller) {
    while (GLCD_ReadStatus(controller) & DISPLAY_STATUS_BUSY) {
		GLCD_Delay();
		}
		GPIOC->BRR = (1 << 10); // EN = 0
    DATA_TO_OUT

    GPIOC->BRR = (1 << 8);  // RW = 0
   	GPIOC->BRR = (1 << 9);  //RS=0

    GLCD_EnableController(controller);

    GLCD_DATA_PORT->ODR = (GLCD_DATA_PORT->ODR & ~0xFF) | cmd;

    GPIOC->BSRR = (1 << 10); // EN = 1
    __NOP();
    GPIOC->BRR = (1 << 10);  // EN = 0
    GLCD_DisableController(controller);
}

// ----------------------------------------------------------------------------


void GLCD_WriteData(uint8_t data) {
		uint8_t controller = screen_x / 64;
   while (GLCD_ReadStatus(controller) & DISPLAY_STATUS_BUSY) {
		GLCD_Delay();
		}
		GPIOC->BRR = (1 << 10); // EN = 0
    DATA_TO_OUT

    GPIOC->BRR = (1 << 8);  // RW = 0
   	GPIOC->BSRR = (1 << 9);  //RS=1

    GLCD_EnableController(controller);

    GLCD_DATA_PORT->ODR = (GLCD_DATA_PORT->ODR & ~0xFF) | data;

    GPIOC->BSRR = (1 << 10); // EN = 1
    __NOP();
    GPIOC->BRR = (1 << 10);  // EN = 0
    GLCD_DisableController(controller);
		screen_x++;
}

// ----------------------------------------------------------------------------

uint8_t GLCD_ReadData(void) {
    uint8_t controller = screen_x / 64;
    uint8_t data;

    while (GLCD_ReadStatus(controller) & DISPLAY_STATUS_BUSY);


    GLCD_DATA_PORT->MODER &= ~0x0000FFFF;

    GPIOC->BSRR = (1 << 8);  // RW = 1
    GPIOC->BSRR = (1 << 9);  // RS = 1

    GLCD_EnableController(controller);
    GPIOC->BSRR = (1 << 10); // EN = 1
    GLCD_Delay();

    data = (uint8_t)(GLCD_DATA_PORT->IDR & 0xFF);

    GPIOC->BRR = (1 << 10);  // EN = 0
    GLCD_DisableController(controller);
    screen_x++;

    return data;
}

void GLCD_Reset(void){
		GPIOC->BRR = (1 << 11); // RES = 0
    for (volatile int i = 0; i < 1000; i++) __NOP();
    GPIOC->BSRR = (1 << 11); // RES = 1
}

// ----------------------------------------------------------------------------

void GLCD_InitializePorts(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

    DATA_TO_OUT

		CTRL_TO_OUT

}



// ----------------------------------------------------------------------------
uint8_t GLCD_ReadByteFromROMMemory(char *ptr) {
    return *ptr;
}
