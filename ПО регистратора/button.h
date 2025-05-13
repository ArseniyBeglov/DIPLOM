#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f0xx.h"
#include <stdbool.h>

// Инициализация кнопок и таймера подавления дребезга
void Button_Init(void);

// Проверка: была ли нажата кнопка (сброс флага после вызова)
bool Button0_WasPressed(void); // PC0
bool Button1_WasPressed(void); // PC1


extern volatile uint8_t presses;   // 0,1,2
extern volatile bool doFlag;       // для кнопки 2

#endif // BUTTON_H
