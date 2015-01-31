#include <sys/types.h>
#include <sys/socket.h>
#include <linux/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PORT 8888
int main(int agrc,char*argv[]){
 int s;//定义服务器端的套接字描述符
 struct sockaddr_in server_addr,client_addr;//服务器地址结构与收到的客户端地址结构
 int len;
 char buffer[1024];
 s=socket(AF_INET,SOCK_DGRAM,0);//建立一个数据报套接字
 if(s<0){
    perror("socket error");
    return;
}

memset(&server_addr,0,sizeof(server_addr));//将地址结构清0
server_addr.sin_family=AF_INET;
server_addr.sin_port=htons(PORT);
server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//任意地址，主机字节序转换为网络字节序
int ret=bind(s,(struct sockaddr*)&server_addr,sizeof(server_addr));//将IP地址与端口号绑定到套接字上,表示服务器用此端口号进行数据的接收与发送
if(ret<0){
    perror("bind error");
    return;
}

//利用for循环进行数据的发送与接收

for(;;){
 bzero(buffer,1024);
 len=sizeof(client_addr);
 //接收数据，并将客户端的地址信息保存到client_addr
 int rec=recvfrom(s,buffer,1024,0,(struct sockaddr*)&client_addr,&len);
 printf("%d\n",rec);
 printf("server:%s\n",buffer);
 if(ret==-1){
    perror("recv error");
    return;
}
sendto(s,buffer,rec,0,(struct sockaddr*)&client_addr,len);//将数据从服务器端发送给客户端client_addr表示目我地址信息，len指目我地址长度信息
//client_addr保存着客户
char *addr;
addr=(char*)inet_ntoa(client_addr.sin_addr);//返回值为一静态内存的指针，全局的，线程不安全，是不可重入的
// char addr[16];
// inet_ntop(AF_INET,(void*)&client_addr.sin_addr,addr,16);//协议族，in_addr指针，字符数组，数组长度
printf("client IP is:%s\n",addr);
int port=ntohs(client_addr.sin_port);//输出客户端的所使用的端口号
printf("Port is:%d\n",port);

}

close(s);
return 0;

}