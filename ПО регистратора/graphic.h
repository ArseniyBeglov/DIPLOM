#include "stm32f0xx.h"
#include "KS0108.h"
void GLCD_Rectangle(unsigned char x, unsigned char y, unsigned char b, unsigned char a);
void GLCD_Circle(unsigned char cx, unsigned char cy ,unsigned char radius);
void GLCD_Line(int X1, int Y1,int X2,int Y2);
void DrawModeScreen(uint8_t mode);
void GLCD_ShutDown_Device(void);
void GLCD_WakeUp_Device(void);
