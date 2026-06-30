#pragma once
#include <stdint.h>

uint16_t getVoltage(int regs){
    uint16_t retval = vols(regs);

    return retval;
}