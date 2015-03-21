#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	int sfd; char buf[1024]; int n, i;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t len;
	char *ip=argv[1];
	sfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8000);
	//serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);
	while(1) {
		if(fgets(buf, 1024, stdin) == NULL)
			break;
		sendto(sfd, buf, strlen(buf), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		n = recvfrom(sfd, buf, 1024, 0, NULL, NULL);
		write(STDOUT_FILENO, buf, n);
	}
	close(sfd);

	return 0;
}
