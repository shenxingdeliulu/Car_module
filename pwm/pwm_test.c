#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <linux/ioctl.h>  

int main(int argc,char *argv[])  
{  
    int fd, i;  
    int buffer;
  	sscanf(argv[1],"%d",buffer);
    fd = open("/dev/pwm", 0);  
    if (fd < 0)  
    {  
        perror("open device failed\n");  
        exit(1);  
    }
   
   
         int b=write(fd,&buffer,1);
    if(b<0)
    {
        perror("Error");

    }
}  
