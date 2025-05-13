#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>

void Storage_SaveDAC(uint16_t value);
uint16_t Storage_LoadDAC(void);

#endif
