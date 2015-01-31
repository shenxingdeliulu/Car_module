#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <linux/ioctl.h>

void hmc5983_delay(int i)
{
    int j, k;
    for (j = 0; j < i; j++)
        for (k = 0; k < 50000; k++) ;
}



int main()
{
	int fd,i;
	short int x,y,z;
	unsigned char BUF[8];
	double angle;
	
	fd=open("/dev/hmc5983",0);
	if(fd<0)
	{
		perror("open device failed\n");
		exit(1);
	}
	while(1)
		{
			read(fd,&BUF,sizeof(BUF));
		 x=BUF[0] << 8 | BUF[1]; //Combine MSB and LSB of X Data output register
		printf("x=%d\n",x);	        
z=BUF[2] << 8 | BUF[3]; //Combine MSB and LSB of Z Data output register
		printf("z=%d\n",z);
    		y=BUF[4] << 8 | BUF[5]; //Combine MSB and LSB of Y Data output register	
		printf("y=%d\n",y);
		angle= atan2((double)y,(double)x) * (180 / 3.14159265) + 180;

		printf("Current angle:%6.2f\n",angle);
		 hmc5983_delay(1000);   //延时	
		}
		return 0;
}
