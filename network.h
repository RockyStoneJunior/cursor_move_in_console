#ifndef __NETWORK_H__
#define __NETWORK_H_

int net_init();
int send_message(int sockfd, char *message, int len);

#endif
