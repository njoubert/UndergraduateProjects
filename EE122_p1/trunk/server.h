#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stddef.h>
#include <time.h>
#include "common.h"
#include "httpsyntax.h"



#define DEFAULT_SERVER_PORT 7788
#define ERROR_STREAM stderr

/* Stores the address info of this server to which clients can connect. */
struct sockaddr_in servaddr;

int listenfd;	/* The socket the server is listening on. */

#endif /*SERVER_H_*/
