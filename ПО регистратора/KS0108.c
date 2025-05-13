
#include "KS0108.h"
#include "font5x8.h"
#include "stm32f0xx.h"

extern void GLCD_Reset(void);
extern void GLCD_InitializePorts(void);
extern void GLCD_WriteData(unsigned char);
extern void GLCD_WriteCommand(unsigned char, unsigned char);
extern unsigned char GLCD_ReadData(void);
extern unsigned char GLCD_ReadStatus(uint8_t);

unsigned char screen_x = 0, screen_y = 0; // screen coordinates
uint8_t  stat;

void GLCD_Initialize(void) {
    GLCD_InitializePorts();
		GLCD_Reset();
    for (uint8_t i = 0; i < 2; i++) {
        GLCD_WriteCommand(0x3E, i); // Display OFF
        GLCD_WriteCommand(0xC0, i); // Set start line = 0
        GLCD_WriteCommand(0x40, i); // Set Y address = 0
        GLCD_WriteCommand(0xB8, i); // Set X address = 0
        GLCD_WriteCommand(0x3F, i); // Display ON
				stat = GLCD_ReadStatus(i);
    }

    screen_x = 0;
    screen_y = 0;
}


void GLCD_GoTo(unsigned char x, unsigned char y)
{
unsigned char i;
screen_x = x;
screen_y = y;

for(i = 0; i < KS0108_SCREEN_WIDTH/64; i++)
  {
  GLCD_WriteCommand(DISPLAY_SET_Y | 0,i);
  GLCD_WriteCommand(DISPLAY_SET_X | y,i);
  GLCD_WriteCommand(DISPLAY_START_LINE | 0,i);
  }
GLCD_WriteCommand((DISPLAY_SET_Y | (x % 64)), (x / 64));
GLCD_WriteCommand((DISPLAY_SET_X | y), (x / 64));
}

void GLCD_TextGoTo(unsigned char x, unsigned char y)
{
GLCD_GoTo(x*6,y);
}

void GLCD_ClearScreen(void)
{
unsigned char i, j;
for(j = 0; j < KS0108_SCREEN_HEIGHT/8; j++)
	{
	GLCD_GoTo(0,j);
	for(i = 0; i < KS0108_SCREEN_WIDTH; i++)
 		GLCD_WriteData(0x00);
	}
}

void GLCD_WriteChar(char charToWrite)
{
    uint8_t i;
    uint16_t index;

    if ((uint8_t)charToWrite >= 32 && (uint8_t)charToWrite < 128) {
        index = ((uint8_t)charToWrite - 32) * 5;
    } else if ((uint8_t)charToWrite >= 0x80 && (uint8_t)charToWrite <= 0xBF) {
        index = (96 + ((uint8_t)charToWrite - 0x80)) * 5;
    } else {
        for (i = 0; i < 5; i++) GLCD_WriteData(0x00);
        GLCD_WriteData(0x00);
        return;
    }

    for(i = 0; i < 5; i++) {
        GLCD_WriteData(font5x8[index + i]);
    }
    GLCD_WriteData(0x00); // пробел между символами
}


void GLCD_WriteString(char * stringToWrite)
{
while(*stringToWrite)
  GLCD_WriteChar(*stringToWrite++);
}

void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
unsigned char tmp;
GLCD_GoTo(x, y/8);
tmp = GLCD_ReadData();
GLCD_GoTo(x, y/8);
tmp = GLCD_ReadData();
GLCD_GoTo(x, y/8);
if(color)
	tmp |= (1 << (y%8));
else
	tmp &= ~(1 << (y%8));
GLCD_WriteData(tmp);
}

void GLCD_Bitmap(char * bmp, unsigned char x, unsigned char y, unsigned char dx, unsigned char dy)
{
unsigned char i, j;
for(j = 0; j < dy / 8; j++)
  {
  GLCD_GoTo(x,y + j);
  for(i = 0; i < dx; i++) 
    GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp++));
  }
}






