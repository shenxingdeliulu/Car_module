#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <linux/ioctl.h>  
#include  <sys/select.h>
#include  <sys/time.h>
#include <signal.h> 
//int  num;
  int fd_speed, i;  
unsigned int result[2];    // 从ds18b20读出的结果，result[0]存放低八位   
 int speed,speed2; 
 int  num,num2;
/*void timefunc(int sig)      
    { 
        read(fd_speed, &result, sizeof(result));
        num=result[0]-num;
        printf("%d\n",num ); 
        num=0;
         speed=num/20;
        printf("Current speed:%d\n", speed);
        signal(SIGPROF, timefunc);   
     }*/

void Speed_Read(int sig)
{
       
        read(fd_speed, result, sizeof(result));
        num=result[0]-num;
        num2=result[1]-num2;
      //  sleep(1);
       // read(fd_speed, result, sizeof(result));
       // num=result[0]-num;
       // num2=result[1]-num2;
        printf("%d\n",num ); 
        printf("%d\n",num2);
         speed=num/20;
         speed2=num2/20;
        printf("Left Current speed:%d\n", speed);
        printf("Right Current speed:%d\n", speed2); 
         read(fd_speed, result, sizeof(result));
        num=result[0];
        num2=result[1];
        signal(SIGPROF, Speed_Read);  
}

int main()  
{  
  
  
  struct itimerval value;
value.it_value.tv_sec=1;    /* 定时1秒 */
value.it_interval.tv_sec=1;    /* 定时1秒 */
 signal(SIGPROF, Speed_Read);     
setitimer(ITIMER_PROF, &value, NULL);  

    fd_speed = open("/dev/speed", 0);  
    if (fd_speed < 0)  
    {  
        perror("open device failed\n");  
        exit(1);  
    }  
     read(fd_speed, &result, sizeof(result));
        num=result[0];
        num2=result[1];
   
       //	sleep(1);
   // while(1)
   // {

 
  

       
        //Speed_Read();
      
    //}
        for(;;);
        
     
}  
  
