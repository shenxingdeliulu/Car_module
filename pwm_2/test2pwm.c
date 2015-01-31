

#include <stdio.h>   /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "pthread.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "sys/un.h"

/*---------------------Macro definitions---------------------------------*/
#define UART_DEVICE "/dev/s3c2410_serial2"
#define USB_DEVICE "/dev/ttyUSB2"

/*---------------------Global variables---------------------------------*/
int fd_pwm;
int fd_pwm2;
//int server_sockfd, client_sockfd;       
//char *server_ip;
char global_buf[50];
//unsigned int global_buf_len;
//unsigned int flag_buf=0;
//unsigned int flag_new_data=0;
//unsigned int flag_socket=0;
//int flag=0;
//pthread_mutex_t pwm1_mutex;
//pthread_mutex_t pwm2_mutex;




void *thread_pwm1()   //thread_socket线程接收远程终端的数据转发到WSN
{
/*	int server_len, client_len;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(6000);
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

	listen(server_sockfd, 5);

	char ch;

	//printf("server waiting\n");
	fprintf(stdout, "\nserver waiting\n");
	client_len = sizeof(client_address);
	


	client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

	//printf("Socket connect success\n");
	fprintf(stdout, "\nSocket connect success\n");
	flag_socket=1;    */
	int arg=5;
	while(1)
	{
/*	int i, len;
	char socket_buf[1000];
	
		len = read( client_sockfd, socket_buf, 1000);
		if(len <= 0)
		{
			//printf("recive data from socket fail !\n");
			perror("recive data from socket fail !");
			exit(EXIT_FAILURE);
		}
		else
		{
			//printf("\nrecieve data from socket sueccess, data_len= %d\n",len);
			fprintf(stdout, "\nrecieve data from socket sueccess, data_len= %d\n",len);
			printf("收到的数据是：%s\n",socket_buf );
		flag=(int)socket_buf[0];   */
//			pthread_mutex_lock(&pwm1_mutex);
			ioctl(fd_pwm,1,arg);
		
//			pthread_mutex_unlock(&pwm1_mutex);
/*			for(i=0;i<len;i++)
			{
				//printf("%c",socket_buf[i]);
				fprintf(stdout, "%c",socket_buf[i]);
				//fprintf(stdout,"%c",socket_buf[i]);
			}*/
			// len = write(fd_uart, socket_buf, len);    //send the data recived from socket to uart1
			// else
			// {
			// 	printf("\nwrite data to uart success,data_len=%d\n",len);
			// }
		}
	
}

void *thread_pwm2()
{
	int i=5;
	while(1)
	{	
		// pthread_mutex_lock(&pwm2_mutex);
		ioctl(fd_pwm2,1,i);
	//	close(fd_pwm);
	//	 pthread_mutex_unlock(&pwm2_mutex);
	}
	
} 




int main(void)
{
	int res;
	pthread_t pwm1_thread, pwm2_thread;
	void *thread_result;
//	pthread_mutex_init(&pwm2_mutex, NULL);
//	pthread_mutex_init(&pwm1_mutex, NULL);
	fd_pwm=open("/dev/iopwm",O_WRONLY | O_CREAT | O_APPEND);
	fd_pwm2=open("/dev/iopwm2",O_WRONLY | O_CREAT | O_APPEND);
//	fd_setpin=open("/dev/SetPin",O_WRONLY | O_CREAT | O_APPEND);
	if(fd_pwm<0)
	{
		printf("PWM failed");
		return 1;
	}
	if(fd_pwm2<0)
	{
		printf("PWM2 failed");
		return 1;
	
	}
	// ioctl(fd_setpin,1,1);
	// uart_open();
	// usb_open();
	
	// res=pthread_create( &uart_thread, NULL, thread_uart, 0 );
	// if(res != 0){
	// 	perror("Thread_uart creation failed");
	// 	exit(EXIT_FAILURE);
	// }
	// printf("Thread_uart creation successed!!!\n");
	res=pthread_create( &pwm1_thread, NULL, thread_pwm1, 0 );
	if(res != 0){
		perror("Thread_pwm1 creation failed");
		exit(EXIT_FAILURE);
	}
	printf("Thread_pwm1 creation successed!!!\n");
//	fprintf(stdout, "\nThread_pwm1 creation successed!!!\n");
	// res=pthread_create( &usb_thread, NULL, thread_usb, 0 );
	// if(res != 0){
	// 	perror("Thread_usbcreation failed");
	// 	exit(EXIT_FAILURE);
	// }
		res=pthread_create( &pwm2_thread,NULL, thread_pwm2,0);
        if(res!=0)
        {
                perror("Thread_pwm2 creation failed");
                exit(EXIT_FAILURE);
        }

	 printf("Thread_pwm2 creation successed!!!\n");

	
	 res=pthread_join( pwm1_thread, &thread_result );
	 if(res != 0){
	 	perror("Thread_usb join failed");
	 	exit(EXIT_FAILURE);
 	} 

	res=pthread_join( pwm2_thread, &thread_result );
	if(res != 0){
		perror("Thread_socket join failed");
		exit(EXIT_FAILURE);
	}
	// res=pthread_join( uart_thread, &thread_result );
	// if(res != 0){
	// 	perror("Thread_uart join failed");
	// 	exit(EXIT_FAILURE);
	// }	
	exit(EXIT_SUCCESS);
}


/*---------------------------------------------------------------------------------*/
