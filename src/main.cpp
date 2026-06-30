#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>

const int sda = 32; //pullup R
const int scl = 33; //assuming a0
int ina226 = 0x40;


void set_scl(int state){
    digitalWrite(scl, state);
}
void set_sda (int state){
    digitalWrite(sda, state);
}

void toggle_pin_i2c(int state){
    set_scl(0);
    delayMicroseconds(2);
    set_sda(state);
    delayMicroseconds(2);
    set_scl(1);
    delayMicroseconds(2);
    set_scl(0);
    set_sda(1);//release it

}

void start(){
    set_scl(1);
    delayMicroseconds(2);
    set_sda(1);    //pull sda from high-low with clock high
    delayMicroseconds(2);
    set_scl(1);
    delayMicroseconds(2);
    set_sda(0);
}

void stop(){
    set_sda(0);
    delayMicroseconds(2);
    set_scl(1);// indicated by pulling SDA from low to high while SCL is high.
    delayMicroseconds(2);
    set_sda(1);
}

void send_byte_i2c(int addr, int read){
        for (int i = 0; i < 7; i++){
            int val = (addr >> (6-i) ) & 1;   //prints it out from msb to lsb. sends msb first.
            toggle_pin_i2c(val);
        }
        toggle_pin_i2c(read);
}

void send_reg_i2c(int reg){
    for (int i = 0; i < 8; i++){
        int val = ( reg >> (7-i) ) & 1;
        toggle_pin_i2c(val);
    }
}


bool ack(){
    set_scl(0);
    delayMicroseconds(2);
    set_sda(1);
    delayMicroseconds(2);
    set_scl(1);    //9th pulse
    int sdaState = digitalRead(sda);  //rising edge - should detect AFTER clock goes high.
    delayMicroseconds(2);
    set_scl(0);
    if (sdaState == 0){
        return 1;
    } else {
        printf("No Ack!");
        stop();
        return 0;
    }
}

void send_nack(){
    set_scl(0);
    set_sda(1); //release sda
    delayMicroseconds(2);
    set_scl(1);
    delayMicroseconds(2);
    int recv_nack = digitalRead(sda);
    set_scl(0);

}


// 0000 0000
uint8_t recv_byte(){

    set_sda(1);//release it
    uint8_t retval = 0x00;
    for (int i = 0; i < 8; i++){
        set_scl(0);
        delayMicroseconds(2);
        set_scl(1);
        int val =digitalRead(sda);
        retval |= val << (7-i); 
        delayMicroseconds(2);
        set_scl(0);  
    }
    return retval;

}

void send_ack(){
    set_scl(0);
    delayMicroseconds(2);
    set_sda(0);
    delayMicroseconds(2);
    set_scl(1);
    delayMicroseconds(2);
    set_scl(0);

}

void setup(){

    pinMode(sda, OUTPUT_OPEN_DRAIN);    //default [master]
    pinMode(scl, OUTPUT_OPEN_DRAIN);    //default [master]
    set_scl(1);//release it
    set_sda(1);//release it
}

void loop(){
    int volReg = 0x01;
    int busReg = 0x02;  //LSB is 1.25mv. multiply the number u get to get voltage.
    // registers: shunt voltage, calibration, current (shunt voltage to do my own resistance calc to verify)
    // need to convert 50v down. will use 2 resistors with the same value. of course, i will do my best to make it as identical as possible, but some form of loss is inherit.
    // triggered mode: set 00h bits of configuration register to 001 or 010 or 011
    //conversion ready: alert pin?
    //int configReg = 0x00;
    //"establish the pin states before activity occurs"
    //get the shunt voltage and do stuff. if too slow seperate & thread via qt
    //0x01 = shunt voltage reg
    start();
    send_byte_i2c(ina226,0);
    //printf("ACK 1 - ");
    if (!ack()) return;
    send_reg_i2c(busReg);  
   // printf("ACK 2 - ");
    if (!ack()) return;
    start();
    send_byte_i2c(ina226,1);
  //  printf("ACK 3\n");
    if (!ack()) return;
    uint8_t byte1 = recv_byte();
    send_ack();
    uint8_t byte2 = recv_byte();
    send_nack();
    uint16_t shuntVol = (byte1 << 8) | byte2;
    printf("%d - %f\n",shuntVol,(shuntVol * 1.25E-3));
    delay(500);
    stop();
}