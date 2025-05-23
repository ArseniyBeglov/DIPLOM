#include "graphic.h"
#include <stdbool.h>
#include "adc.h"
#include <math.h>

extern void GLCD_SetPixel(unsigned char x, unsigned char y, unsigned char color);

const unsigned char color = 1;
volatile bool useRussian = true;
extern const unsigned short Untitled13x8[6][16];



#include "KS0108.h"

void DrawBatteryIcon(uint8_t level) {
    if (level > 5) level = 5;

    const uint8_t x0 = 114;  // правый край экрана
    const uint8_t y0 = 0;    // верхняя строка

    // Контур батареи (13x8)
    for (uint8_t x = 0; x < 12; x++) {
        GLCD_SetPixel(x0 + x, y0, 1);       // верхняя рамка
        GLCD_SetPixel(x0 + x, y0 + 7, 1);   // нижняя рамка
    }
    for (uint8_t y = 0; y < 8; y++) {
        GLCD_SetPixel(x0,     y0 + y, 1);   // левая рамка
        GLCD_SetPixel(x0 + 11, y0 + y, 1);  // правая рамка
    }

    // Контакт (наружу справа)
    GLCD_SetPixel(x0 + 12, y0 + 2, 1);
    GLCD_SetPixel(x0 + 12, y0 + 3, 1);
    GLCD_SetPixel(x0 + 12, y0 + 4, 1);
    GLCD_SetPixel(x0 + 12, y0 + 5, 1);

    // ? Очищаем внутреннюю область (всё внутри рамки)
    for (uint8_t x = 1; x < 11; x++) {
        for (uint8_t y = 1; y < 7; y++) {
            GLCD_SetPixel(x0 + x, y0 + y, 0);
        }
    }

    // ? Заполнение уровня заряда (до 5 блоков)
    for (uint8_t i = 0; i < level; i++) {
        uint8_t fillX = x0 + 1 + i * 2;
        for (uint8_t y = 1; y <= 6; y++) {
            GLCD_SetPixel(fillX,     y0 + y, 1);
            GLCD_SetPixel(fillX + 1, y0 + y, 1);
        }
    }
}



uint8_t GetBatteryLevelFromVoltage(float voltage) {
    if (voltage >= 2.59f) return 5;
    else if (voltage >= 2.534f) return 4;
    else if (voltage >= 2.492f) return 3;
    else if (voltage >= 2.45f) return 2;
    else if (voltage >= 2.17f) return 1;
    else return 0;
}


void GLCD_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a)
{
  unsigned char j;
  for (j = 0; j < a; j++) {
		GLCD_SetPixel(x, y + j, color);
		GLCD_SetPixel(x + b - 1, y + j, color);
	}
  for (j = 0; j < b; j++)	{
		GLCD_SetPixel(x + j, y, color);
		GLCD_SetPixel(x + j, y + a - 1, color);
	}
}

void GLCD_Circle(unsigned char cx, unsigned char cy ,unsigned char radius)
{
int x, y, xchange, ychange, radiusError;
x = radius;
y = 0;
xchange = 1 - 2 * radius;
ychange = 1;
radiusError = 0;
while(x >= y)
  {
  GLCD_SetPixel(cx+x, cy+y, color); 
  GLCD_SetPixel(cx-x, cy+y, color); 
  GLCD_SetPixel(cx-x, cy-y, color);
  GLCD_SetPixel(cx+x, cy-y, color); 
  GLCD_SetPixel(cx+y, cy+x, color); 
  GLCD_SetPixel(cx-y, cy+x, color); 
  GLCD_SetPixel(cx-y, cy-x, color); 
  GLCD_SetPixel(cx+y, cy-x, color); 
  y++;
  radiusError += ychange;
  ychange += 2;
  if ( 2*radiusError + xchange > 0 )
    {
    x--;
	radiusError += xchange;
	xchange += 2;
	}
  }
}

void GLCD_Line(int X1, int Y1,int X2,int Y2)
{
int CurrentX, CurrentY, Xinc, Yinc, 
    Dx, Dy, TwoDx, TwoDy, 
	TwoDxAccumulatedError, TwoDyAccumulatedError;

Dx = (X2-X1); 
Dy = (Y2-Y1); 

TwoDx = Dx + Dx; 
TwoDy = Dy + Dy; 

CurrentX = X1; 
CurrentY = Y1; 

Xinc = 1; 
Yinc = 1; 

if(Dx < 0) 
  {
  Xinc = -1; 
  Dx = -Dx;  
  TwoDx = -TwoDx; 
  }

if (Dy < 0) 
  {
  Yinc = -1; 
  Dy = -Dy; 
  TwoDy = -TwoDy; 
  }

GLCD_SetPixel(X1,Y1, color); 

if ((Dx != 0) || (Dy != 0)) 
  {
  if (Dy <= Dx) 
    { 
    TwoDxAccumulatedError = 0; 
    do 
	  {
      CurrentX += Xinc; 
      TwoDxAccumulatedError += TwoDy; 
      if(TwoDxAccumulatedError > Dx)  
        {
        CurrentY += Yinc; 
        TwoDxAccumulatedError -= TwoDx; 
        }
       GLCD_SetPixel(CurrentX,CurrentY, color);
       }while (CurrentX != X2); 
     }
   else 
      {
      TwoDyAccumulatedError = 0; 
      do 
	    {
        CurrentY += Yinc; 
        TwoDyAccumulatedError += TwoDx;
        if(TwoDyAccumulatedError>Dy) 
          {
          CurrentX += Xinc;
          TwoDyAccumulatedError -= TwoDy;
          }
         GLCD_SetPixel(CurrentX,CurrentY, color); 
         }while (CurrentY != Y2);
    }
  }
}

void DrawModeScreen(uint8_t mode) {
    GLCD_ClearScreen();

    const char* modeNamesRus[] = {
        "режим: постоянный",
        "режим: одиночный",
        "режим: калибровка"
    };

    const char* modeNamesEng[] = {
        "mode: continuous",
        "mode: single",
        "mode: calibration"
    };

    const char** modeNames = useRussian ? modeNamesRus : modeNamesEng;

    const uint8_t y_coords[] = {1, 2, 3};

    for (uint8_t i = 0; i < 3; i++) {
        uint8_t y = y_coords[i];

        if (i == mode) {
            uint8_t base_x = 5;
            uint8_t base_y = y * 8 + 3;

            GLCD_Line(base_x, base_y, base_x + 2, base_y + 3);
            GLCD_Line(base_x + 2, base_y + 3, base_x + 6, base_y - 3);
        }

        GLCD_GoTo(16, y);
        GLCD_WriteString(modeNames[i]);
    }
}


void GLCD_ShutDown_Device(void) {
    GLCD_ClearScreen();
    GLCD_GoTo(10, 3);
    GLCD_WriteString("Device turned off");
}

void GLCD_WakeUp_Device(void){
		GLCD_ClearScreen();
    GLCD_GoTo(10, 3);
    GLCD_WriteString("Waking up...");
}
