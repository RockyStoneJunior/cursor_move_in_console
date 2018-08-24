#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define MAX_RECV_LEN 1024

#define ERR_USERNAME	"Username does not exist!\n\n"
#define ERR_PASSWORD	"Password error!\n\n"
#define MSG_LOGINOK	"Login OK!\n\n"

struct account{
	char username[16];
	char password[16];
	int  socketfd;
	bool login;
};

struct account account_list[100];
int account_nr = 0;

int client_list[100];
int client_nr = 0;

int client_count = 0;

void *client_process_thread(void *this_count)
{
	int this_client_count = *(int *)this_count;
	int clientfd = client_list[this_client_count];
	char recv_buff[512];
	char send_buff[512];
	char send_len_buff[2];
	bool login = false;
	int count = 0;
	int account_count = 0;

	int err_username_len = strlen(ERR_USERNAME);
	int err_password_len = strlen(ERR_PASSWORD);
	int msg_loginok_len = strlen(MSG_LOGINOK);

	int recv_len;
	while(1)
	{
		if((recv_len = read(clientfd, recv_buff, 512)) > 0)
		{
			char *ptr_colon = strchr(recv_buff, ':');
			char *ptr_enter = strchr(recv_buff, '\n');

			int username_len = ptr_colon - recv_buff;
			ptr_colon++;
			int password_len = ptr_enter - ptr_colon;

			while(count < account_nr)
			{
				if(!strncmp(account_list[count].username, recv_buff, username_len))
				{
					break;
				}

				count++;
			}

			if(count < account_nr)
			{
				if(!strncmp(account_list[count].password, ptr_colon, password_len))
				{
					strcpy(&send_buff[2], MSG_LOGINOK);
					strcpy(&send_buff[msg_loginok_len + 2], account_list[count].username);

					int msg_len = msg_loginok_len + strlen(account_list[count].username);

					send_buff[0] = (char)msg_len;
					send_buff[1] = (char)(msg_len >> 8);

					write(client_list[this_client_count], send_buff, msg_len + 2);
					account_list[count].socketfd = client_list[this_client_count];
					account_list[count].login = true;
					account_count = count;
					break;
				}else{
					send_buff[0] = (char)err_password_len;
					send_buff[1] = (char)(err_password_len >> 8);
					strcpy(&send_buff[2], ERR_PASSWORD);

					write(client_list[this_client_count], send_buff, err_password_len + 2);
					account_list[count].socketfd = client_list[this_client_count];
					count =  0;
				}
			}else{
				send_buff[0] = (char)err_username_len;
				send_buff[1] = (char)(err_username_len >> 8);
				strcpy(&send_buff[2], ERR_USERNAME);

				write(client_list[this_client_count], send_buff, err_username_len + 2);
				count =  0;
			}
		}
	}

	for(count = 0; count < account_nr; count++)
	{
		if((count != account_count)&&(account_list[count].login == true))
		{
			int name_len = strlen(account_list[count].username);
			printf("%s:%d\n", account_list[count].username, name_len);
			send_len_buff[0] = (char)name_len;
			send_len_buff[1] = (char)(name_len >> 8);
			//strcpy(&send_buff[2], account_list[count].username);

			write(client_list[this_client_count], send_len_buff, 2);
			write(client_list[this_client_count], account_list[count].username, name_len);
		}
	}

	while((recv_len = read(clientfd, recv_buff, 512)) > 0)
	{
		send_len_buff[0] = (char)recv_len;
		send_len_buff[1] += (char)(recv_len >> 8);

		for(int i = 0; i < client_nr; i++)
		{
			if(i != this_client_count)
			{
				write(client_list[i], send_len_buff, 2);
				write(client_list[i], recv_buff, recv_len);
			}
		}	
	}

	close(clientfd);
}

int account_list_init()
{
	char *line =NULL;
	size_t len;

	memset(account_list, 0, sizeof(account_list));

	FILE *fp = fopen("account.txt", "r+");

	int ret;
	int count = 0;
	while((ret = getline(&line, &len, fp)) != EOF)
	{
		char *ptr_colon = strchr(line, ':');
		char *ptr_enter = strchr(line, '\n');

		int username_len = ptr_colon - line;
		ptr_colon++;
		int password_len = ptr_enter - ptr_colon;

		strncpy(account_list[count].username, line, username_len);
		strncpy(account_list[count].password, ptr_colon, password_len);
		account_list[count].socketfd = 0;
		account_list[count].login = false;

		count++;
	}

	account_nr = count;

	free(line);
	fclose(fp);

	while(count--)
	{
		printf("%s:%s\n", account_list[count].username, account_list[count].password);
	}
}

int main()
{
	char recv_buff[MAX_RECV_LEN];
	unsigned short port = 9996;

	pthread_t client_process_thread_nr;

	struct sockaddr_in serveraddr;

	account_list_init();

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
		int flags = 1;
		setsockopt(clientfd, SOL_TCP, TCP_NODELAY, (void *)&flags, sizeof(flags));

		client_list[client_nr] = clientfd;
		int count = client_nr;
		pthread_create(&client_process_thread_nr, NULL, client_process_thread, &count);
		client_nr++;
	}

	return 0;
}
