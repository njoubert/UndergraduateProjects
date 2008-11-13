#ifndef COMMON_H_
#define COMMON_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <regex.h>

/*
 * DISCLAIMER:
 * This file follows the concepts and some of the sample code
 * as presented in UNIX Network Programming by Stevens, Fenner and Rudoff
 */

#define MAXLINE 4096
#define BUFFSIZE 2048

char* errmes;

/* 
 * Creates a socket and returns the file descriptor. 
 * */
int Socket(int domain, int type, int protocol);

/* 
 * Connects to a server using the given socket and address 
 * */
int Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

/*
 * Binds a socket to a known port.
 * */
int Bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);

/*
 * Converts a socket into a passive listening socket.
 * */
int Listen(int sockfd, int backlog);

/* 
 * Sends the given buffer info, looping and handling errors as necessary. 
 * */
ssize_t Send(int sockfd, const void *buffer, size_t nbytes, int flags);

/* 
 * Recieved data, setting error messages as necessary. 
 * */
ssize_t Recv(int sockfd, void *buffer, size_t buffersize, int flags);

/*
 * Finds only the headers, and returns a null terminated string
 * containing them. Leaves the socket pointing to the char after
 * the blank line termination.
 * */
char* Recv_headers(int sockfd);

char* reltoabspath(char* relpath);

int Recv_chunk_size(int sockfd);

/* 
 * Resolves hostname from ascii name, or sets errmes and returns null 
 * */
struct hostent* Gethostbyname(char* name);

/* 
 * Converts a dotted-quad string to a network-order address 
 * */
int Inet_pton(int family, const char *strptr, void *addrptr);

/*
 * Prints a short ip:port representation of an address
 * */
void Printsockaddrin_concise(const struct sockaddr_in *addr);

/* 
 * Outputs the information of a sockaddr_in structure to stdout 
 * */
void Printsockaddrin(const struct sockaddr_in *addr);

#endif /*COMMON_H_*/
