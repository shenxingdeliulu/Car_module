
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>

	
	
int main(int argc,char **argv)
{
	int buzzer_fd;
	unsigned long freq;
	char *endstr,*str;
	printf("Usage: %s <on/off> <freq>\n","./beep_test");

	if( argc==3 )
	{     //例： ./beep_test on 100  或者  ./beep_test off 100
		buzzer_fd = open( "/dev/iopwm",O_RDWR );//打开设备
		if(buzzer_fd<0)
		{
			perror("open device:");
			exit(1);
		}
		
		str = argv[2];
		errno = 0;
		freq = strtol(str , &endstr,0);
		
		if((errno == ERANGE &&(freq == LONG_MAX || freq == LONG_MIN))||(errno != 0 && freq == 0))
		{
			perror("freq :");
			exit(EXIT_FAILURE);
		}
		if(endstr == str)
		{
			fprintf(stderr,"Please input a digits for freq\n");
			exit(EXIT_FAILURE);
		}
	while(1)
	ioctl(buzzer_fd,1,freq);
	}

	close(buzzer_fd);
	return 0;
} 
