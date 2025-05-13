#include "stm32f0xx.h"
#include <stdio.h>
#include "delay.h"
#include "uart.h"
#include "adc.h"
#include "dac.h"
#include "pwm.h"
#define V_REF 3.0f
#include "graphic.h"
#include "button.h"
#include "measurement.h"  
#include "storage.h"

uint16_t dac_calibrated_value;
uint8_t lastPresses = 255;

int main(void) {
    SystemInit();
    Button_Init();
    GLCD_Initialize();
    GLCD_ClearScreen();
    SysTick_Config(SystemCoreClock / 1000);
    Timeout_Init();
		UART1_Init();
    ADC_Init();
    DAC_Init();
		

		dac_calibrated_value = Storage_LoadDAC();

    while (1) {
        if (presses != lastPresses) {
            lastPresses = presses;
            DrawModeScreen(presses);
            Timeout_Reset();
        }
        
        if (presses == 0) {
            Measurement_Continuous(); 
        }

        if (presses == 1 && doFlag) {
            doFlag = false;
            Timeout_Reset();
            Measurement_Single(); 
        }

				if (presses == 2 && doFlag) {
        doFlag = false;
        Timeout_Reset();
        Measurement_Calibration();
				}


        if (Timeout_IsExpired()) {
            ShutDown_Device();
        }
    }
}