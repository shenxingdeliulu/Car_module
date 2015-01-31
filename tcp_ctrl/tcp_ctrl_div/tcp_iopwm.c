

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
#include "unistd.h"
/*---------------------Macro definitions---------------------------------*/
#define UART_DEVICE "/dev/s3c2410_serial2"
#define USB_DEVICE "/dev/ttyUSB2"

/*---------------------Global variables---------------------------------*/
int fd_pwm,fd_pwm2;
int fd_setpin;
int server_sockfd, client_sockfd;       
char *server_ip;
char global_buf[50];
unsigned int global_buf_len;
unsigned int flag_buf=0;
unsigned int flag_new_data=0;
unsigned int flag_socket=0;
int pwm=5;
pthread_mutex_t lock_pwm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready_pwm = PTHREAD_COND_INITIALIZER;
char flag_pwm;



void *thread_socket()   //thread_socket线程接收远程终端的数据转发到WSN
{
	int server_len, client_len;
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
	flag_socket=1;
	while(1)
	{
	int i, len;
	char socket_buf[1000];
		bzero(socket_buf,1000);
		len = read( client_sockfd, socket_buf, 1000);
		socket_buf[len]='\0';
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
			if(!strncasecmp(socket_buf, "startB4", 7)) 
			{
				pthread_mutex_lock(&lock_pwm);
				flag_pwm =1;
				ioctl(fd_setpin,0,1);
				pthread_mutex_unlock(&lock_pwm);
				pthread_cond_signal(&ready_pwm);
			fprintf(stdout, "\nflag_pwm = %d\n",flag_pwm);
			}
						
			if(!strncasecmp(socket_buf, "stopB4", 6))	
			{
				pthread_mutex_lock(&lock_flag);
			        flag_pwm =0;
				ioctl(fd_setpin,4,1);
				pthread_mutex_unlock(&lock_flag);
			fprintf(stdout, "\nflag_pwm = %d\n",flag_pwm);
			}
			if(!strncasecmp(socket_buf,"startB1",7))
			{
				ioctl(fd_setpin,1,1);

			}
			if(!strncasecmp(socket_buf,"stopB1",6))
			{
				ioctl(fd_setpin,0,1);
			}
			if(!strncasecmp(socket_buf,"startLeft",9))
			{
				ioctl(fd_setpin,5,1);
			}
			if(!strncasecmp(socket_buf,"stopLeft",8))
			{
				ioctl(fd_setpin,0,1);
			}
			if(!strncasecmp(socket_buf,"startRight",10))
			{	
				ioctl(fd_setpin,3,1);
			}
			if(!strncasecmp(socket_buf,"stopRight",9))
			{
				ioctl(fd_setpin,0,1);
			}
			if(!strncasecmp(socket_buf,"startB3",7))
			{
				pthread_mutex_lock(&lock_pwm);
				pwm++;
				if(pwm>20)
				pwm=20;
				pthread_mutex_unlock(&lock_pwm);
				fprintf(stdout, "\npwm = %d \n", pwm);
			}
			if(!strncasecmp(socket_buf,"startB2",7))
			{
				pthread_mutex_lock(&lock_pwm);
				pwm--;
				if(pwm<5)
				pwm=5;
				pthread_mutex_unlock(&lock_pwm);
				fprintf(stdout, "\npwm = %d \n", pwm);				
			}
			
						
		}
		
}
	close(client_sockfd);
}

void *thread_pwm()
{
	while(1)
        {
        	
        	pthread_mutex_lock(&lock_flag);
        	while(flag_pwm == 0)
        	{
        		//ioctl(fd_setpin,1,1);
        		pthread_cond_wait(&ready_pwm, &lock_flag);
        	}
	        
	        pthread_mutex_unlock(&lock_flag);
		 pthread_mutex_lock(&lock_pwm);
		ioctl(fd_pwm,3,1);
	        ioctl(fd_pwm,4,pwm);
		
	        if(pwm > 20) pwm=20;
		 pthread_mutex_unlock(&lock_pwm);
	        

	        fprintf(stdout, "\npwm = %d \n", pwm);

        }

        close(fd_pwm);
	close(fd_setpin);
        pthread_exit((void *)0);
}

void* thread_pwm2()
{	
	while(1)
	{
                 pthread_mutex_lock(&lock_pwm);
//                ioctl(fd_pwm,1,pwm);
              ioctl(fd_pwm2,1,pwm);

                if(pwm > 20) pwm=20;
                 pthread_mutex_unlock(&lock_pwm);


//                fprintf(stdout, "\npwm = %d \n", pwm);
	}
}


int main(void)
{
	int res;
	pthread_t socket_thread, pwm_thread,  pwm2_thread;
	void *thread_result;
	fd_pwm=open("/dev/iopwm",O_WRONLY | O_CREAT | O_APPEND);
//	fd_pwm2=open("/dev/iopwm2",O_WRONLY | O_CREAT | O_APPEND);
	fd_setpin=open("/dev/SetPin",O_WRONLY | O_CREAT | O_APPEND);
	if(fd_pwm<0)
	{
		printf("PWM failed");
		return 1;
	}
	if(fd_setpin<0)
	{
		printf("SetPin failed");
		return 1;
	
	}
//	 ioctl(fd_setpin,1,1);
	// uart_open();
	// usb_open();
	
	// res=pthread_create( &uart_thread, NULL, thread_uart, 0 );
	// if(res != 0){
	// 	perror("Thread_uart creation failed");
	// 	exit(EXIT_FAILURE);
	// }
	// printf("Thread_uart creation successed!!!\n");
	res=pthread_create( &socket_thread, NULL, thread_socket, 0 );
	if(res != 0){
		perror("Thread_socket creation failed");
		exit(EXIT_FAILURE);
	}
	printf("Thread_socket creation successed!!!\n");
//	fprintf(stdout, "\nThread_socket creation successed!!!\n");
	 res=pthread_create( &pwm_thread, NULL, thread_pwm, 0 );
	 if(res != 0){
	 	perror("Thread_usbcreation failed");
	 	exit(EXIT_FAILURE);
	 }
	 printf("Thread_pwm creation successed!!!\n");
//	res=pthread_create( &pwm2_thread, NULL, thread_pwm2, 0 );
//        if(res != 0){
//               perror("Thread_pwm2 creation failed");
//                exit(EXIT_FAILURE);
//        }
//	printf("Thread_pwm2 creation successed!!!\n");

	 res=pthread_join( socket_thread, &thread_result );
	 if(res != 0){
	 	perror("Thread_socket join failed");
	 	exit(EXIT_FAILURE);
	 }

	 res=pthread_join( pwm_thread, &thread_result );
	 if(res != 0){
	 	perror("Thread_pwm join failed");
	 	exit(EXIT_FAILURE);
	 }	
//	res=pthread_join( pwm2_thread, &thread_result );
//         if(res != 0){
//                perror("Thread_pwm2 join failed");
//                exit(EXIT_FAILURE);
//         }

	exit(EXIT_SUCCESS);
}


/*---------------------------------------------------------------------------------*/
