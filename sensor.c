#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wiringPi.h>

main()
{
    int file;
    wiringPiSetup();
    pinMode(1,OUTPUT);
    pinMode(0,PWM_OUTPUT);
    pinMode(4,PWM_OUTPUT);
    pinMode(5, PWM_OUTPUT);

    softPwmCreate(0,0,100);
    softPwmCreate(4,0,100);
    softPwmCreate(5,0,100);


    file = open("/dev/i2c-1", O_RDWR);
    if(file<0)
    {
        puts("fallo al abrir puerto");
    }

    int addr = 0x27;
    if(ioctl(file,I2C_SLAVE, addr)<0)
    {
        puts("Fallo al asignar bus");
    }

    while(1)
    {

        if(write(file,0, 0) != 0)
        {
            puts("Failed to write");
            exit(1);
        }

        usleep(100000);

        unsigned char buf[4] = {0};
        float data;
        char channel;


        // Using I2C Read
        if (read(file,buf,4) < 0)
        {

            /* ERROR HANDLING: i2c transaction failed */
            puts("Failed to read from the i2c bus.\n");
            puts("\n\n");
        }
        else
        {

            time_t t;
            struct tm * timeinfo;
            time(&t);
            timeinfo = localtime(&t);
            printf("%s", asctime(timeinfo));
            //printf("%d-%d-%d %d:%d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

            int reading_hum = (buf[0] << 8)+ buf[1];
            double humidity = reading_hum / 16382.0 * 100.0;
            printf("Humedad: %f\n", humidity);

            int reading_temp = (buf[2] << 6) + (buf[3] >> 2);
            double temperature = reading_temp/16382.0 * 165.0 - 40;
            printf("Temperatura: %f\n",temperature);


            if(temperature > 27.0)
            {
                digitalWrite(1, HIGH);
            }
            else
            {
                digitalWrite(1, LOW);
            }

            int cont = 0;
            if(humidity < 40.0)
            {
                while(cont<7)
                {
                    softPwmWrite(0,25);
                    delay(250);
                    softPwmWrite(0,0);
                    delay(250);
                    cont++;

                }

            }
            if(humidity >= 40.0 && humidity <= 70.0)
            {
                while(cont<7)
                {
                    softPwmWrite(4,25);
                    delay(250);
                    softPwmWrite(4,0);
                    delay(250);
                    cont++;

                }

            }
            if(humidity > 70.0)
            {
               while(cont<7)
                {
                    softPwmWrite(5,25);
                    delay(250);
                    softPwmWrite(5,0);
                    delay(250);
                    cont++;

                }

            }

        }


        //printf("Channel %02d Data:  %04f\n",channel,data);
    }


}








