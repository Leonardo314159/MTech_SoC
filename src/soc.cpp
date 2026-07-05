#include "bitbang.h"
#include <cstdio>
#include <esp32-hal.h>

const int volReg = 0x01;
const int busReg = 0x02;
/*
time for 1 = 0.252ms
of delays
assume 0.5 second gaps.
*/

/*
4608 wH = 96aH = 
= 345,600C

*/
const uint32_t batteryCapacityC = 345600;
// (75mV / 100A) means resistance is 0.00075 ohms
const float shuntR = 0.00075;

double soc = batteryCapacityC;

double get_soc(){
    uint16_t busRaw = vols(busReg);

    uint16_t shuntRaw = vols(volReg);
    double shuntVoltage = shuntRaw * 2.5e-6;//every step is 1.25uV
    double busVoltage = busRaw * 1.25e-3;
    double current = (shuntVoltage / shuntR);
    double chargeConsumed = current * 0.5; //0.24s delay here, 260ms of delays in bitbang = 0.26 = 0.76 of total time
    soc = soc - chargeConsumed;

    printf("Bus:%f - ShuntL %f - Current:%f A - SoC:%f%%\n",
    busVoltage, shuntVoltage, current, (soc/batteryCapacityC));
    delay(240);
    
    return soc;
}