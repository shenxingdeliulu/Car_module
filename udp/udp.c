#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <linux/in.h>

#define PORT 45454

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in server_addr,client_addr;
	client_fd=socket(AF_INET,SOCK_DGRAM,0);//建立数据报套接字
	
	if(client_fd<0)
		{

			perror("socket error");
			return -1;
		}
	memset(&server_addr,0,sizeof(server_addr));
	memset(&client_addr,0,sizeof(client_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	//server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_addr.s_addr=inet_addr("192.168.3.107");
	
	char buffer[1024];
	int size;
	int len=sizeof(server_addr);
	bzero(buffer,1024);
	strcpy(buffer, "This is a test message");
	size=sizeof(buffer);
	char recv_buf[20];
	
		while(1)
		{
		struct sockaddr_in from;
		sendto(client_fd,buffer,size,0,(struct sockaddr*)&server_addr,len);
		int recvsize=recvfrom(server_fd,recv_buf,sizeof(recv_buf),0,NULL,NULL);
		recv_buf[recvsize]='\0';
		printf("%d",recvsize);
		if(recvsize>0)
		{
			printf("received:%s",recv_buf);
		}
		else
		{
			perror("error");
		}
	
		}
	return 0;
}