#include "bitbang.h"


void setup(){
    int volReg = 0x01;
    int busReg = 0x02;

    int busVoltage = getVoltage(busReg);
    int shuntVoltage = getVoltage(volReg);
}
/*
time for 1 = 0.252ms
of delays
assume 0.5 second gaps.
*/
void main(){
    
}