
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

main(){
int file;
int adapter_nr = 1;
const char *buffer;


file = open("/dev/i2c-1", O_RDWR);
if(file>0){
puts("Ok");
}

int addr = 0x27;
if(ioctl(file,I2C_SLAVE, addr)>=0){
puts("Ok");
}

char buf[10] = {0};
    float data;
    char channel;

int i ;
    for(i=0 ; i<4; i++) {
        // Using I2C Read
        if (read(file,buf,2) != 2) {
            /* ERROR HANDLING: i2c transaction failed */
            puts("Failed to read from the i2c bus.\n");

            puts("\n\n");
        } else {
            data = (float)((buf[0] & 0b00001111)<<8)+buf[1];
            data = data/4096*5;
            //channel = ((buf[0] & 0b00110000)>>4);
            printf("%f\n",data);
            //printf("Channel %02d Data:  %04f\n",channel,data);
        }
    }






}



