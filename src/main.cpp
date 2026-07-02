#include "bitbang.h" 
#include <cstdio>

const double batteryCapacity = 345600;
double charge_used = 0; //96Ah

void setup(){
    prep();
}
void loop(){

    double charge_left = get_soc();

    //printf("%u%% remaining\n",(charge_left / batteryCapacity));
}