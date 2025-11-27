#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IOCTL_1 1
#define IOCTL_2 2

#define DEVICE_FILE_NAME "/dev/chardev"

int main(){
    int file_desc, ret_val;
    char msg[100];
    char msgsend[100]="Tekst iz appa\n";

    printf("Open device file. %s\n", DEVICE_FILE_NAME);
    file_desc = open(DEVICE_FILE_NAME,2);
    if(file_desc<0){
        printf("Cant open: %s", DEVICE_FILE_NAME);
        exit(-1);
    }
    read(file_desc,msg,100);
    printf("Read frome kernel module: %s\n",msg);
    if(ret_val < 0){
        printf("Error reading from kernel: %s\n", msg);
        exit(-1);
    }
    write(file_desc,msgsend,strlen(msgsend));
    printf("Write to kernel moduel: %s\n", msgsend);
    if(ret_val < 0){
        printf("Write to kernel failed: %s\n", msgsend);
        exit(-1);
    }
    sleep(1);
    close(file_desc);
    printf("Device closed: %s", DEVICE_FILE_NAME);
}
