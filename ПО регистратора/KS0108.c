
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

extern const unsigned short LCD5x8H5x8[]; // массив кириллицы

void GLCD_WriteChar(char ch)
{
    uint8_t i;

    if ((uint8_t)ch >= 0xC0 && (uint8_t)ch <= 0xFF) {
        uint8_t index = (uint8_t)ch - 0xC0; 
        const unsigned short* glyph = &LCD5x8H5x8[index * 6 + 1]; 

        for (i = 0; i < 5; i++) {
            GLCD_WriteData((uint8_t)glyph[i]);
        }
    }
    else if ((uint8_t)ch >= 32 && (uint8_t)ch < 128) {
        extern const char font5x8[];
        const char* glyph = &font5x8[((uint8_t)ch - 32) * 5];
        for (i = 0; i < 5; i++) {
            GLCD_WriteData(glyph[i]);
        }
    } else {
        for (i = 0; i < 5; i++) {
            GLCD_WriteData(0x00);
        }
    }

    GLCD_WriteData(0x00); // интервал
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






