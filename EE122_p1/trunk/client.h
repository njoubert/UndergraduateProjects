#ifndef CLIENT_H_
#define CLIENT_H_

#include "common.h"
#include "httpsyntax.h"

#define INPUT_BUFFER_SIZE 2048
#define ERROR_STREAM stderr

/* Stores the address info of the currently connected server */
struct sockaddr_in servaddr;

typedef struct connstate {
	char* host;
	int port;
	int sockfd;
	int established;
	int presistent;
	int closeme; //Set this when shit happens!
} connstate;

char* geturlline();

#endif /*CLIENT_H_*/
