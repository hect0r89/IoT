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
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <geniePi.h>

double humidity = 0.0;
double temperature = 0.0;
double maxTemperature = 0.0;

static void *handleCoolGauge(void *data)
{
    //increment or decrement value, initialized to 1.

    for(;;)             //infinite loop
    {
        genieWriteObj(GENIE_OBJ_ANGULAR_METER, 0x00, temperature);

        genieWriteObj(GENIE_OBJ_METER, 0x00, humidity);
    }
    return NULL;
}

void handleGenieEvent(struct genieReplyStruct * reply)
{
    if(reply->cmd == GENIE_REPORT_EVENT)    //check if the cmd byte is a report event
    {
        if(reply->object == GENIE_OBJ_KNOB) //check if the object byte is that of a slider
        {
            if(reply->index == 0)		  //check if the index byte is that of Slider0
                //write to the LED digits object
                maxTemperature = reply->data;

        }
    }

    //if the received message is not a report event, print a message on the terminal window
    else
        printf("Unhandled event: command: %2d, object: %2d, index: %d, data: %d \r\n", reply->cmd, reply->object, reply->index, reply->data);
}


int main()
{
    int file;
    pthread_t myThread;
    struct genieReplyStruct reply ;
    genieSetup("/dev/ttyAMA0", 115200);
    genieWriteObj(GENIE_OBJ_KNOB, 0x00, 0);
    genieWriteObj(GENIE_OBJ_LED_DIGITS, 0x00, 0);


    (void)pthread_create (&myThread,  NULL, handleCoolGauge, NULL);







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
    //holds the value of the cool gauge


    while(1)
    {

        if(write(file,0, 0) != 0)
        {
            puts("Failed to write");
            exit(1);
        }

        usleep(100000);

        unsigned char buf[4] = {0};




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
            humidity = reading_hum / 16382.0 * 100.0;
            printf("Humedad: %f\n", humidity);

            int reading_temp = (buf[2] << 6) + (buf[3] >> 2);
            temperature = reading_temp/16382.0 * 165.0 - 40;
            printf("Temperatura: %f\n",temperature);

            //increment or decrement value, initialized to 1.

            if(temperature>maxTemperature)
            {
                genieWriteObj(GENIE_OBJ_USER_LED, 0x00, 1);

            }
            else
            {
                genieWriteObj(GENIE_OBJ_USER_LED, 0x00, 0);

            }

            while(genieReplyAvail())      //check if a message is available
            {
                genieGetReply(&reply);      //take out a message from the events buffer
                handleGenieEvent(&reply);   //call the event handler to process the message
            }
                           //10-millisecond delay.Don't hog the



            //write to Coolgauge0

            ussleep(1000);    //10-millisecond delay
            //increment or decrement








        }


        //printf("Channel %02d Data:  %04f\n",channel,data);
    }
    return 0;


}








