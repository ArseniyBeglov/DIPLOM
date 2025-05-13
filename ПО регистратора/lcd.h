#ifndef LCD_H
#define LCD_H

#include "stm32f0xx.h"

void lcd_init_all(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_goto(uint8_t row, uint8_t column);
void lcd_char(uint8_t row, uint8_t column, char c);
void LCD_GPIO_Init(void);   

#endif
