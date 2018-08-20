#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

#define MAX_RECV_LEN 1024

int client_list[100];
int client_count = 0;

void *client_process_thread(void *count)
{
	int this_client_count = *(int *)count;
	int clientfd = client_list[this_client_count];
	char recv_buff[512];

	int recv_len;
	while((recv_len = read(clientfd, recv_buff, 512)) > 0)
	{
		for(int i = 0; i < client_count; i++)
		{
			if(i != this_client_count)
			{
				write(client_list[i], recv_buff, recv_len);
			}
		}	
	}

	close(clientfd);
}

int main()
{
	char recv_buff[MAX_RECV_LEN];
	unsigned short port = 9996;

	pthread_t client_process_thread_nr;

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

		client_list[client_count] = clientfd;
		int count = client_count;
		pthread_create(&client_process_thread_nr, NULL, client_process_thread, &count);
		client_count++;
	}

	return 0;
}
