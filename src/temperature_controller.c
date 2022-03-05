#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>

#include "temperature_controller.h"

#define RESISTOR 4
#define COOLER 5


void config_temperature(){
    wiringPiSetup();

    pinMode(RESISTOR, OUTPUT);
    pinMode(COOLER, OUTPUT);

    softPwmCreate(RESISTOR,1,100);
    softPwmCreate(COOLER,1,100);
}

void change_temperature(double power){
    if(power < 0){
        power *= -1;
        if(power < 40) 
            power = 40;
        softPwmWrite(COOLER,power);
        softPwmWrite(RESISTOR,0);
    } else{
        softPwmWrite(RESISTOR,power);
        softPwmWrite(COOLER,0);
    }
}

void close_temp(){
        softPwmWrite(RESISTOR,0);
        softPwmWrite(COOLER,0);
}
