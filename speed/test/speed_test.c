#include <stdio.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <linux/ioctl.h>  
#include  <sys/select.h>
#include  <sys/time.h>
#include <signal.h> 
int  num;
  int fd_speed, i;  
    unsigned int result[1];    // 从ds18b20读出的结果，result[0]存放低八位   
    int speed;
void timefunc(int sig)      /* 定时事件代码 */
{
    
       
        read(fd_speed, &result, sizeof(result));
        num=result[0]-num;

        printf("%d\n",num ); 
        num=0;
         speed=num/20;
        printf("Current speed:%d\n", speed);
signal(SIGPROF, timefunc);    /* 捕获定时信号 */
}
int main()  
{  
  
  
  struct itimerval value;
value.it_value.tv_sec=1;    /* 定时1秒 */
value.it_interval.tv_sec=1;    /* 定时1秒 */

    fd_speed = open("/dev/speed", 0);  
    if (fd_speed < 0)  
    {  
        perror("open device failed\n");  
        exit(1);  
    }  

   
       //	sleep(1);
    while(1)
    {

 read(fd_speed, &result, sizeof(result));
        num=result[0];
        signal(SIGPROF, timefunc);     /* 捕获定时信号 */
setitimer(ITIMER_PROF, &value, NULL); /* 定时开始 */
       

      
    }
        
     
}  
  
