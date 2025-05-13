#include "lcd.h"
#include "delay.h"

#define LCD_RS_PIN (1 << 3)
#define LCD_E_PIN  (1 << 4)
#define LCD_D4_PIN (1 << 5)
#define LCD_D5_PIN (1 << 6)
#define LCD_D6_PIN (1 << 7)
#define LCD_D7_PIN (1 << 8)

void pulse_e(void) {
    GPIOB->BSRR = LCD_E_PIN;
    Delay_us(1);
    GPIOB->BRR = LCD_E_PIN;
    Delay_us(40);
}

void lcd_cmd_4(uint8_t data) {
    if(data & 0x01) GPIOB->BSRR = LCD_D4_PIN; else GPIOB->BRR = LCD_D4_PIN;
    if(data & 0x02) GPIOB->BSRR = LCD_D5_PIN; else GPIOB->BRR = LCD_D5_PIN;
    if(data & 0x04) GPIOB->BSRR = LCD_D6_PIN; else GPIOB->BRR = LCD_D6_PIN;
    if(data & 0x08) GPIOB->BSRR = LCD_D7_PIN; else GPIOB->BRR = LCD_D7_PIN;
}

void lcd_cmd(uint8_t cmd) {
    GPIOB->BRR = LCD_RS_PIN;
    lcd_cmd_4(cmd >> 4);
    pulse_e();
    lcd_cmd_4(cmd & 0x0F);
    pulse_e();
}

void lcd_data(uint8_t data) {
    GPIOB->BSRR = LCD_RS_PIN;
    lcd_cmd_4(data >> 4);
    pulse_e();
    lcd_cmd_4(data & 0x0F);
    pulse_e();
}

void lcd_goto(uint8_t row, uint8_t column) {
    uint8_t address = (row == 1) ? (0x80 + (column - 1)) : (0xC0 + (column - 1));
    lcd_cmd(address);
}

void lcd_char(uint8_t row, uint8_t column, char c) {
    lcd_goto(row, column);
    lcd_data(c);
}

void lcd_init_all(void) {
    Delay_ms(20);
    lcd_cmd_4(0x03); pulse_e(); Delay_ms(5);
    lcd_cmd_4(0x03); pulse_e(); Delay_ms(5);
    lcd_cmd_4(0x03); pulse_e(); Delay_ms(5);
    lcd_cmd_4(0x02); pulse_e(); Delay_ms(5);

    lcd_cmd(0x28); Delay_ms(5);
    lcd_cmd(0x0C); Delay_ms(5);
    lcd_cmd(0x01); Delay_ms(5);
    lcd_cmd(0x06); Delay_ms(5);
}

void LCD_GPIO_Init(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    GPIOB->MODER &= ~(
        (3UL << (3 * 2)) |
        (3UL << (4 * 2)) |
        (3UL << (5 * 2)) |
        (3UL << (6 * 2)) |
        (3UL << (7 * 2)) |
        (3UL << (8 * 2))
    );
    GPIOB->MODER |= (
        (1UL << (3 * 2)) |
        (1UL << (4 * 2)) |
        (1UL << (5 * 2)) |
        (1UL << (6 * 2)) |
        (1UL << (7 * 2)) |
        (1UL << (8 * 2))
    );
}
