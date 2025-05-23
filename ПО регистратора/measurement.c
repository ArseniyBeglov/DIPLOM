#include "measurement.h"
#include "adc.h"
#include "dac.h"
#include "uart.h"
#include "graphic.h"
#include "button.h""
#include "delay.h"
#include <stdio.h>
#include "storage.h"
#include <stdbool.h>
extern volatile bool useRussian;

#define V_REF 3.0f
extern uint16_t dac_calibrated_value;

static float compute_angle(float p0, float p1) {
    float x = (float)p0;
    float y = (float)p1;

    return
        -2.742483f
        + -2.205985e-13f * x
        + 2.615924e-13f * y
        + -2.096010e-10f * x*x
        + -7.393949e-11f * x*y
        + 2.920277e-10f * y*y
        + -3.699174e-08f * x*x*x
        + 5.999678e-09f * x*x*y
        + -6.727848e-08f * x*y*y
        + 7.824669e-08f * y*y*y
        + 7.754824e-11f * x*x*x*x
        + -5.054770e-11f * x*x*x*y
        + -1.682028e-10f * x*x*y*y
        + 2.972625e-10f * x*y*y*y
        + -1.418920e-10f * y*y*y*y
        + -7.362228e-14f * x*x*x*x*x
        + 3.017857e-13f * x*x*x*x*y
        + -5.631253e-13f * x*x*x*y*y
        + 5.767277e-13f * x*x*y*y*y
        + -3.245988e-13f * x*y*y*y*y
        + 8.034189e-14f * y*y*y*y*y;
}




static void Measurement_PerformCycle(void) {
    char buffer[50];

    DAC->CR |= DAC_CR_EN1;
		DAC->DHR12R1 = dac_calibrated_value;

    Delay_ms(5);

    uint16_t adc_value_pa0 = ADC_Read_Channel(0);
    uint16_t adc_value_pa1 = ADC_Read_Channel(1);

		float voltage_pa0 = (adc_value_pa0 / 4095.0f) * V_REF * 1000.0f;
		float voltage_pa1 = (adc_value_pa1 / 4095.0f) * V_REF * 1000.0f;

    DAC->CR &= ~DAC_CR_EN1;
    DAC->DHR12R1 = 4095;

    float angle = compute_angle(voltage_pa0, voltage_pa1);


    if (useRussian) {
		sprintf(buffer, "Угол: %.2f +- 0.04", angle);
	} else {
		sprintf(buffer, "Angle: %.2f +- 0.04" , angle);
	}




    UART1_SendString(buffer);
    UART1_SendString("\r\n");

    GLCD_GoTo(0, 6);
    GLCD_WriteString("                              "); 
    GLCD_GoTo(0, 6);
    GLCD_WriteString(buffer);

		DAC->CR |= DAC_CR_EN1;
		DAC->DHR12R1 = 3403;
		uint16_t adc_battery = ADC_Read_Channel(2);
		float voltage = (adc_battery / 4095.0f) * 3.0f;
		uint8_t level = GetBatteryLevelFromVoltage(voltage);
		DrawBatteryIcon(level);
		DAC->CR &= ~DAC_CR_EN1;
}


void Measurement_Continuous(void) {
    Measurement_PerformCycle(); 
}

void Measurement_Single(void) {
    Measurement_PerformCycle(); 
}

static bool calibration_aborted(uint8_t expectedPresses) {
    if (presses != expectedPresses) {
       
        DAC->CR &= ~DAC_CR_EN1;
        DAC->DHR12R1 = 4095;
        return true;
    }
    return false;
}

void Measurement_Calibration(void) {
    static bool calibrationStarted = false;
    static uint16_t maxValue0 = 0;
    static uint16_t maxValue1 = 0;
    static uint16_t dacValue = 1861;

    calibrationStarted = true;
    maxValue0 = 0;
    maxValue1 = 0;
    DAC->CR |= DAC_CR_EN1;
    DAC->DHR12R1 = dacValue;

    GLCD_GoTo(0, 6);
		GLCD_GoTo(0, 6);
		GLCD_WriteString(useRussian ? "Калибровка запущена" : "Calibration started");
		UART1_SendString(useRussian ? "Калибровка запущена\r\n" : "Calibration started\r\n");



    Delay_ms(1000);

    // Р¤Р°Р·Р° 1: Р–РґС‘Рј, РїРѕРєР° СЃРѕРѕС‚РЅРѕС€РµРЅРёРµ РђР¦Рџ РїРѕРїР°РґС‘С‚ РІ 0.95..1.05
    while (1) {
				if (calibration_aborted(2)) return;
        uint16_t val0 = ADC_Read_Channel(0);
        uint16_t val1 = ADC_Read_Channel(1);

        if (val1 == 0) continue; 

        float ratio = (float)val0 / val1;

        if (ratio >= 0.95f && ratio <= 1.05f) break;

        GLCD_GoTo(0, 6);
		if (ratio > 1.05f) {
			GLCD_WriteString(useRussian ? "Наклон вправо     " : "Tilt: Clockwise     ");
		} else {
			GLCD_WriteString(useRussian ? "Наклон влево      " : "Tilt: Anti-Clock    ");
		}


        Delay_ms(200);
    }

    // Р¤Р°Р·Р° 2: РџРѕРёСЃРє РјР°РєСЃРёРјСѓРјР°
    bool passedPeak = false;

		while (1) {
			if (calibration_aborted(2)) return;

			uint16_t val0 = ADC_Read_Channel(0);
			uint16_t val1 = ADC_Read_Channel(1);

			if (val0 > maxValue0) maxValue0 = val0;
			if (val1 > maxValue1) maxValue1 = val1;

			uint16_t diff0 = maxValue0 - val0;
			uint16_t diff1 = maxValue1 - val1;

			uint16_t threshold0 = maxValue0 * 5 / 100;
			uint16_t threshold1 = maxValue1 * 5 / 100;

			if (!passedPeak) {
			GLCD_GoTo(0, 6);
			GLCD_WriteString(useRussian ? "Опустите волокна   " : "Go lower           ");

			}

			if (!passedPeak && (diff0 > threshold0 || diff1 > threshold1)) {
					passedPeak = true;
					GLCD_GoTo(0, 6);
					GLCD_WriteString(useRussian ? "Поднимите волокна  " : "Go higher          ");

			}

			if (passedPeak) {
					bool inRange0 = val0 > (maxValue0 * 95 / 100) && val0 < (maxValue0 * 105 / 100);
					bool inRange1 = val1 > (maxValue1 * 95 / 100) && val1 < (maxValue1 * 105 / 100);

					if (inRange0 && inRange1) {
							GLCD_GoTo(0, 6);
							GLCD_WriteString(useRussian ? "Максимум найден    " : "Max found          ");
							UART1_SendString(useRussian ? "Максимум найден\r\n" : "Max found\r\n");
							break;
					}
			}

			Delay_ms(200);
		}		

    
		GLCD_GoTo(0, 6);
		GLCD_WriteString("                    ");
		GLCD_WriteString(useRussian ? "Настройка DAC..." : "In progress...");

		bool success = false;
    while (1) {
				if (calibration_aborted(2)) return;
				DAC->DHR12R1 = dacValue;
				Delay_ms(5);

				uint16_t val0 = ADC_Read_Channel(0);
				float voltage = (val0 / 4095.0f) * V_REF;
				DAC->DHR12R1 = 4095;
				if (voltage >= 2.4f && voltage <= 2.6f) {
						GLCD_GoTo(0, 6);
						GLCD_WriteString(useRussian ? "Калибровка завершена" : "Calibration done");
						UART1_SendString(useRussian ? "Калибровка завершена\r\n" : "Calibration ended\r\n");

						success = true;
						break;
				}

				if (voltage > 2.6f) {
						if (dacValue == 0) break; 
						dacValue++;
				} else { // voltage < 2.4f
						if (dacValue >= 4095) break; 
						dacValue--;
				}
				
				Delay_ms(10);
		}

		if (success) {
				Storage_SaveDAC(dacValue);
				dac_calibrated_value = dacValue; 
		}

		if (!success) {
				GLCD_GoTo(0, 6);
				GLCD_WriteString(useRussian ? "Ошибка калибровки" : "Calibration fail");
				UART1_SendString(useRussian ? "Ошибка калибровки\r\n" : "Calibration failed\r\n");

		}

    DAC->CR &= ~DAC_CR_EN1;
    DAC->DHR12R1 = 0;
}
