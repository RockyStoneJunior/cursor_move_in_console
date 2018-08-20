#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

#define MAX_RECV_LEN 1024

int main()
{
	char recv_buff[MAX_RECV_LEN];
	unsigned short port = 9996;

	struct sockaddr_in serveraddr;

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(serverfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(serverfd, 5);

	int recv_len = 0;
	int sockaddr_len = sizeof(serveraddr);
	while(1)
	{
		int clientfd = accept(serverfd, (struct sockaddr *)&serveraddr, &sockaddr_len);

		while((recv_len = read(clientfd, recv_buff, MAX_RECV_LEN)) >  0)
		{
			write(STDOUT_FILENO, recv_buff, recv_len);
		}
	}

	close(serverfd);

	return 0;
}
