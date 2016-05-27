#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <softPwm.h>
int main (void){
        int i;
        wiringPiSetup();
        pinMode(1,PWM_OUTPUT);
        pinMode(4,PWM_OUTPUT);
        pinMode(5, PWM_OUTPUT);
        softPwmCreate(1,0,100);
        softPwmCreate(4,0,100);
        softPwmCreate(5,0,100);
        for(;;){
            for(i = 0; i < 100; i++){
                softPwmWrite(1,i);
                delay(50);
            }
            softPwmWrite(1,0);
            delay(500);

            for(i = 0; i < 100; i++){
                softPwmWrite(4,i);
                delay(50);
            }
            softPwmWrite(4,0);
            delay(500);
            for(i = 0; i < 100; i++){
                softPwmWrite(5,i);
                delay(50);
            }
            softPwmWrite(5,0);
            delay(500);
        }
        return 0;
}
