#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>  
#include <string.h> 
#include <linux/ioctl.h>  
#include <fcntl.h>
int main(int argc,char *argv[])  
{  
    int fd, i;  
    int buffer[1];
    sscanf(argv[1],"%d",buffer);
    //printf("%d\n",buffer[0] );
    fd = open("/dev/pwm",O_RDWR);  
    if (fd < 0)  
    {  
        perror("open device failed\n");  
        exit(1);  
    }
   switch(buffer[0])
   {
        case 1:
            ioctl(fd,1,10);
           
                break;
        case 0:
            ioctl(fd,3,10);break;
   }
   
         //int b=ioctl(fd,0,buffer[0]);
         
   
    return 0;
}  
