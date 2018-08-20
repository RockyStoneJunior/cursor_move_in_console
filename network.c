#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

int net_init()
{
	char *host = "localhost";
	unsigned short port = 9996;
	struct sockaddr_in servaddr;
	in_addr_t in_addr;

	struct hostent *hp = gethostbyname(host);
	in_addr = inet_addr(inet_ntoa(*(struct in_addr *)(hp->h_addr)));

	servaddr.sin_addr.s_addr = in_addr;
	servaddr.sin_port = htons(port);
	servaddr.sin_family = AF_INET;

	int serverfd = socket(AF_INET, SOCK_STREAM, 0);
	connect(serverfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	return serverfd;
}

int send_message(int sockfd, char *msg, int len)
{
	write(sockfd, msg, len);
}

















