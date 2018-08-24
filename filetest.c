
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

char file_buff[1024];

int main()
{
	char username[16];
	char password[16];
	
	char *line = NULL;
	size_t len= 0;

	FILE *fp = fopen("account.txt", "r+");

	if(fp == NULL)
	{
		printf("file does not exit!\n");
	}
	
	int ret;
	while((ret = getline(&line, &len, fp)) != EOF)
	{
		//sscanf(line,"%[^:]s%[^\n]s", username, password);
		char *ptr_colon = strchr(line, ':');
		char *ptr_enter = strchr(line, '\n');
		int username_len = ptr_colon - line;
		strncpy(username, line, username_len);

		ptr_colon++;
		int password_len = ptr_enter - ptr_colon;
		strncpy(password, ptr_colon, password_len);
		username[username_len] = '\0';
		password[password_len] = '\0';
		printf("%s:%s\n", username, password);
	}

	free(line);
	return 0;
}
