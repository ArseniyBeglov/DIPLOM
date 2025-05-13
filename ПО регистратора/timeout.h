#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx.h"

#define TIMEOUT_LIMIT_MS 120000 // 120 секунд

void Timeout_Init(void);
void Timeout_SysTick_Handler(void);
void Timeout_Reset(void);
bool Timeout_IsExpired(void);
void ShutDown_Device(void);
#endif // TIMEOUT_H