#pragma once
#include <stdint.h>

//bitbang
uint16_t vols(uint8_t regs);
void prep();

//soc
double get_soc();