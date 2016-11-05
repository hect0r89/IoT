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
    for(;;)
    {
        genieWriteObj(GENIE_OBJ_ANGULAR_METER, 0x00, temperature);
        genieWriteObj(GENIE_OBJ_METER, 0x00, humidity);
        if(temperature>maxTemperature)
        {
            genieWriteObj(GENIE_OBJ_USER_LED, 0x00, 1);

        }
        else
        {
            genieWriteObj(GENIE_OBJ_USER_LED, 0x00, 0);

        }

        time_t t;
        struct tm * timeinfo;
        time(&t);
        timeinfo = localtime(&t);
        char dataSensor[256];
        snprintf(dataSensor,256, "Temperatura: %f\nHumedad: %f\n",temperature, humidity);
        genieWriteStr(0x00, dataSensor);
        double horas = timeinfo->tm_hour;
        double minutos = timeinfo->tm_min;
        double segundos = timeinfo->tm_sec;

        genieWriteObj(GENIE_OBJ_LED_DIGITS, 0x01, (horas*100)+minutos);
        genieWriteObj(GENIE_OBJ_LED_DIGITS, 0x02, segundos);

        usleep(80000);
    }
    return NULL;
}

void handleGenieEvent(struct genieReplyStruct * reply)
{
    if(reply->cmd == GENIE_REPORT_EVENT)
    {
        if(reply->object == GENIE_OBJ_KNOB)
        {
            if(reply->index == 0)
                maxTemperature = reply->data;
        }
    }
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


    while(1)
    {

        if(write(file,0, 0) != 0)
        {
            puts("Failed to write");
            exit(1);
        }
        sleep(1);

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


            int reading_hum = (buf[0] << 8)+ buf[1];
            humidity = reading_hum / 16382.0 * 100.0;
            printf("Humedad: %f\n", humidity);

            int reading_temp = (buf[2] << 6) + (buf[3] >> 2);
            temperature = reading_temp/16382.0 * 165.0 - 40;
            printf("Temperatura: %f\n",temperature);





            while(genieReplyAvail())
            {
                genieGetReply(&reply);
                handleGenieEvent(&reply);
            }



        }

    }
    return 0;


}








