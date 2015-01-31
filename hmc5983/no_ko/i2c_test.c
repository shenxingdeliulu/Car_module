#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "pthread.h"
#include <sys/wait.h>
#define I2C_RETRIES 0x0701
#define I2C_TIMEOUT 0x0702
#define I2C_RDWR 0x0707 
#define MAXBUFFER 1000
/******全局变量*****/
unsigned char BUF[6];
int fd,ret;
int fd_pwm,fd_setpin;
int fd_speed;
int speed=600;
int flag_angle;
unsigned int result[1];   
int speed;
float u;//这次输出的值
float u1;//上次输出的值 
float Kp=1.5;//比例系数 
float Ki=0;//积分系数 
float Kd=0.5;//微分系数 
float ek;//当次误差 
float ek1;//上一次误差 
float ek2;//上两次误差


struct i2c_rdwr_ioctl_data hmc5983_data;
pthread_mutex_t lock_angle = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_angle = PTHREAD_COND_INITIALIZER;
/********************/




/*****延时函数******/
void delay(int i)
{
    int j, k;
    for (j = 0; j < i; j++)
        for (k = 0; k < 50000; k++) ;
}

/***********hmc5983初始化函数****************/

void hmc5983_init()
{
	
	hmc5983_data.nmsgs=2;
	 /*
        * 因为操作时序中，最多是用到2个开始信号（字节读操作中），所以此将e2prom_data.nmsgs配置为2
        */
	hmc5983_data.msgs=(struct i2c_msg*)malloc(hmc5983_data.nmsgs*sizeof(struct i2c_msg));
	if(!hmc5983_data.msgs)
		{
			perror("malloc error");	
			exit(1);
		}
	ioctl(fd,I2C_TIMEOUT,1);/*超时时间*/
	ioctl(fd,I2C_RETRIES,2);/*重复次数*/
	/*write data to hmc5983，初始化hmc5983*/

	hmc5983_data.nmsgs=1;
	(hmc5983_data.msgs[0]).len=2;//1个hmc5983写入目标的地址和一个数
	(hmc5983_data.msgs[0]).addr=0x1E;//设备地址
	(hmc5983_data.msgs[0]).flags=0;//write
	(hmc5983_data.msgs[0]).buf=(unsigned char*)malloc(2);
	(hmc5983_data.msgs[0]).buf[0]=0x02;//hmc5983写入目标地址，向0x02地址写入0x00，设置5983为连续测量模式
	(hmc5983_data.msgs[0]).buf[1]=0x00;//the data to write
	ret=ioctl(fd,I2C_RDWR,&hmc5983_data);
	if(ret<0)
		{
			perror("ioctl error1");
		}
}

double hmc5983_read()
{
	double angle_read;
	short int x,y,z,i;
	     hmc5983_data.nmsgs=2;
		(hmc5983_data.msgs[0]).len=1;
		(hmc5983_data.msgs[0]).addr=0x1E;//设备地址
		(hmc5983_data.msgs[0]).flags=0;//write
		(hmc5983_data.msgs[0]).buf[0]=0x03;//数据地址
		(hmc5983_data.msgs[1]).len=6;
		(hmc5983_data.msgs[1]).addr=0x1E;
		(hmc5983_data.msgs[1]).flags=I2C_M_RD;//read
		(hmc5983_data.msgs[1]).buf=(unsigned char*)malloc(1);//存放返回值的地址
		memset((hmc5983_data.msgs[1]).buf,0,sizeof((hmc5983_data.msgs[1]).buf));
	ret=ioctl(fd,I2C_RDWR,&hmc5983_data);
		if(ret<0)
		{
			perror("ioctl error");
		}
		for(i=0;i<6;i++)
		{
		BUF[i]=hmc5983_data.msgs[1].buf[i];
		printf("buff%d[%d]=%x\n",i,i,BUF[i]);	
		}
	
		
	x=BUF[0] << 8 | BUF[1]; //Combine MSB and LSB of X Data output register
	printf("x=%d\n",x);
    z=BUF[2] << 8 | BUF[3]; //Combine MSB and LSB of Z Data output register
	printf("z=%d\n",z);
    y=BUF[4] << 8 | BUF[5]; //Combine MSB and LSB of Y Data output register
	printf("y=%d\n",y);

//	fprintf(pf,"%d %d\n",x,y);
	x=x+42;
	y=y+144;
	y=y*(248/228);
	
//	pthread_mutex_lock(&lock_angle);
//	angle= atan2((double)((y+27)*1.25),(double)(x-39)) * (180 / 3.14159265) + 180; // angle in degrees	
	angle_read= atan2((double)y,(double)x) * (180 / 3.14159265) + 180;
//    angle= atan2((double)y,(double)x) * (180 / 3.14159265) + 180; // angle in degrees
	
	printf("current angle is %6.2f\n",angle_read);
	return angle_read;

	
	delay(500);
}


//PID控制，参数1：sv(设定值)，参数2：fv(反馈值) 
void PID_Control(float sv,float fy)
{
	float pError=0,iError=0,dError=0;
	ek=sv-fy;
	pError=ek-ek1;//比例误差（等于当前误差减去前一次的误差）
	iError=ek;//积分误差（等于当前误差值）
	dError=ek-ek1*2+ek2;//微分误差（等于当前误差减去前一次2倍误差加上前两次的误差）
	ek2=ek1;//储存前两次的误差值
	ek1=ek;//储存前一次的误差值
	u=Kp*pError+Ki*iError+Kd*dError+u1;//获取PID调节的误差值
	if(u>=300)
		u=250;
	else
		if(u<=-300)
			u=-250;

	u1=u;//储存前一次的输出值

	printf("%5.2f\n",u);
	

}
void PWM_Control()
{
	ioctl(fd_setpin,0,1);
	ioctl(fd_pwm,1,(int)u+speed);
	ioctl(fd_pwm,0,speed-(int)u);
}
void Speed_Read()
{
		int  num;
		read(fd_speed, &result, sizeof(result));
		num=result[0];
		sleep(1);
		read(fd_speed, &result, sizeof(result));
		num=result[0]-num;

        printf("%d\n",num ); 
         speed=num/20;
        printf("Current speed:%d\n", speed);
}
void stop()
{
	ioctl(fd_setpin,4,1);
	close(fd);
	close(fd_setpin);
	close(fd_pwm);
	close(fd_speed);
	//close(fd_d);
	_exit(0);

}
void *thread_speed()
{
	  while(1)
    {
        read(fd_speed, &result, sizeof(result)); 
        printf("%d\n",result[0] ); 
         speed=result[0]/20;
        printf("Current speed:%d\n", speed);  
    }
}

/******主程序******/
int main()
{
	


//	FILE* pf;
	pthread_t speed_thread;
	void* thread_result;
/*	float lastTime=0.0;
	float ITerm,lasterror;//积分项，前次输入
	float Output=0.0;*/   
	float angle;
	int res;
	float Setpoint=285.0;//设置目标值
	fd=open("/dev/i2c/0",O_RDWR);
	fd_pwm=open("/dev/iopwm",O_RDWR);
	fd_setpin=open("/dev/SetPin",O_RDWR);
	 fd_speed= open("/dev/speed", 0);  
//	pf=fopen("/opt/hmc5983/no_ko/xy.txt","a+");
	/*/dev /i2c-0是在注册i2c-dev.c后产生的，代表一个可操作的适配器。如果不使用i2c-dev.c的方式，就没有，也不需要这个节点。*/
	if(fd<0)
                {
                        perror("open error");
                }
	

	hmc5983_init();
	sleep(1);
	
/*	res=pthread_create( &speed_thread, NULL, thread_speed, 0 );
	if(res != 0){
		perror("Thread_speed creation failed");
		exit(EXIT_FAILURE);
	}
	printf("Thread_speed creation successed!!!\n");
	*/

		
		while(1)
	{	
		// pf=fopen("/opt/hmc5983/no_ko/xy.txt","a+");

		
	
//	fclose(pf);
/*	time_t now;
	time(&now);
	float TimeCh=(float)now-lastTime;
	//printf("now=%d\n",now);//单位s
	float Kp=1.5,Ki=0.0,Kd=0.0;
	float SampleTime=0.1;//采样时间0.1s
	float Setpoint=180.0;//设置目标值
	
		angle=hmc5983_read(); 
		float Input=angle;//输入赋值
		float error=Setpoint-Input;//偏差值
		ITerm+=(Ki*error*TimeCh);//积分项
		float DTerm=Kd*(error-lasterror)/TimeCh;//微分项
		Output=Kp*error+ITerm-DTerm;//计算输出值
		printf("%5.2f",Output);
		ioctl(fd_setpin,0,1);
		ioctl(fd_pwm,0,(int)Output+speed);
		ioctl(fd_pwm,1,speed-(int)Output);

		lasterror=error;
		lastTime=now;
		sleep(1);  */

		angle=hmc5983_read();
		delay(1000);
 		 Speed_Read();
		PID_Control(angle,Setpoint);
        PWM_Control(); 	
		signal(SIGINT,stop);
	
	}
	
	return 0;
	
	

}
	
