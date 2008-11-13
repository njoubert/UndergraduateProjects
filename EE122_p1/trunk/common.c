#include "common.h"

int Socket(int family, int type, int protocol) {
	int n;
	n = socket(family, type, protocol);
	if (n < 0)
		errmes = "socket error";
	return n;
}

int Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen) {
	int n;
	n = connect(sockfd, servaddr, addrlen);
	if (n < 0) {
		errmes = "connect error";
	}
	return n;
}

int Bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen) {
	int retval;
	if ((retval = bind(sockfd, myaddr, addrlen)) < 0)
		errmes = "could not bind address to socket";
	return retval;
}

int Listen(int sockfd, int backlog) {
	int retval;
	if ((retval = listen(sockfd, backlog)) < 0)
		errmes = "could not convert socket to listening state";
	return retval;
}

ssize_t Send(int sockfd, const void *buffer, size_t nbytes, int flags) {
	size_t nleft;
	ssize_t nsent;
	const void *ptr;
	
	ptr = buffer;
	nleft = nbytes;
	while (nleft > 0) {
		if ((nsent = send(sockfd, buffer, nleft, flags)) <= 0) {
			if (nsent < 0 && errno == EINTR)
				nsent = 0; //restart the send
			else {
				errmes = "could not send any data";
				return -1;
			}
		}
		nleft -= nsent;
		ptr += nsent;
	}
	return nbytes;
}

ssize_t Recv(int sockfd, void *buffer, size_t buffersize, int flags) {
	int nrecv;
	size_t nleft;
	char keepreading = 1;
	nleft = buffersize;
	
	while(keepreading > 0) {
		if ((nrecv = recv(sockfd, buffer, nleft, flags)) < 0) {
			if (errno == EINTR)
				nrecv = 0; //keep trying
			else {
				errmes = "could not recv data";
				keepreading = 0; //done
			}
		}
		keepreading = 0;
	}
	return nrecv;

}

/*
 * Reades only the headers of the HTTP Message from the given socket.
 * Returns these headers. 
 * The socket is now positioned right after the headers.
 * */
char* Recv_headers(int sockfd) {
	//Keep on peeking in larger amounts of data until we see the blank line
	//delimiting the end of the headers. Once we have that, read it into a buffer.
	char* tempbuffer;
	char* breakptr = NULL;
	int keepreading = 1;
	int len = 128;
	int breakpos = -1;
	int nrecv;
	int waittimeout = 15;
	tempbuffer = (char*) malloc((len+1) *sizeof(char));
	
	while (breakptr == NULL) {
		nrecv = recv(sockfd, tempbuffer, len, MSG_PEEK);
		tempbuffer[nrecv] = '\0';
		breakptr = strstr(tempbuffer, "\r\n\r\n");
		if (breakptr == NULL) {
			if (nrecv == 0) {
				errmes = "Could not find end of headers!";
				return NULL;
			}
			if (nrecv < len) {
				if (waittimeout < 0) {
					errmes = "Timeout in waiting for header end!";
					free(tempbuffer);
					return NULL;
				} else {
					waittimeout -= 1;
					sleep(1);
					continue; //No termination received yet... Wait!
				}
			}
			else {
				len *= 2; //Need more space!
				free(tempbuffer);
				tempbuffer = (char*) malloc((len+1) *sizeof(char));
			}
		}
	}
	
	breakpos = (breakptr - tempbuffer) + 4; //+4 to account for last \r\n\r\n
	free(tempbuffer);
	
	tempbuffer = (char*) malloc((breakpos+1)*sizeof(char));
	while (keepreading) {
		nrecv = recv(sockfd, tempbuffer, breakpos, 0);
		if (nrecv < 0 && errno != EINTR) {
			free(tempbuffer);
			return NULL;
		}
		keepreading = 0;
	}
	
	tempbuffer[nrecv] = '\0';
	return tempbuffer;
	
}

char* reltoabspath(char* relpath) {
	char* cwd;
	char* path;
	int cwdlen;
	cwd = getcwd(NULL, 0);
	cwdlen = strlen(cwd);
	if (relpath[0] != '/') {
		path = (char*) malloc((strlen(relpath) + cwdlen + 2));
		strcpy(path, cwd);
		strcpy(path + cwdlen, "/");
		strcpy(path + cwdlen + 1, relpath);
	} else {
		path = (char*) malloc((strlen(relpath) + cwdlen + 1));
		strcpy(path, cwd);
		strcpy(path + cwdlen, relpath);
		
	}
	free(cwd);
	return path;
	
}

/*
 * Reads the chunk size in format "%d\r\n" from sockfd.
 * */
int Recv_chunk_size(int sockfd) {
	//Keep on peeking in larger amounts of data until we see the blank line
	//delimiting the end of the headers. Once we have that, read it into a buffer.
	char* tempbuffer;
	char* breakptr = NULL;
	int keepreading = 1;
	int len = 8;
	int breakpos = -1;
	int nrecv;
	int number;
	int waittimeout = 15;
	tempbuffer = (char*) malloc((len+1) *sizeof(char));
	
	while (breakptr == NULL) {
		nrecv = recv(sockfd, tempbuffer, len, MSG_PEEK);
		tempbuffer[nrecv] = '\0';
		breakptr = strstr(tempbuffer, "\r\n");
		if (breakptr == NULL) {
			if (nrecv == 0) {
				errmes = "Could not find end of chunk size header!";
				return -1;
			}
			if (nrecv < len) {
				if (waittimeout < 0) {
					errmes = "Timeout in waiting for chunk size end!";
					free(tempbuffer);
					return -1;
				} else {
					waittimeout -= 1;
					sleep(1);
					continue; //No termination received yet... Wait!
				}
			}
			else {
				len *= 2; //Need more space!
				free(tempbuffer);
				tempbuffer = (char*) malloc((len+1) *sizeof(char));
			}
		} else if (breakptr == tempbuffer) {
			breakptr = NULL;
			recv(sockfd, tempbuffer, 2, 0); //Get rid of \r\n at start.
		}
	}
	
	breakpos = (breakptr - tempbuffer) + 2; //+2 to account for last \r\n
	free(tempbuffer);
	
	tempbuffer = (char*) malloc((breakpos+1)*sizeof(char));
	while (keepreading) {
		nrecv = recv(sockfd, tempbuffer, breakpos, 0);
		if (nrecv < 0 && errno != EINTR) {
			free(tempbuffer);
			errmes = "Could not read from socket!";
			return -2;
		}
		keepreading = 0;
	}
	
	tempbuffer[nrecv] = '\0';

	if (strlen(tempbuffer) > 8)
		return -4; //We only support up to integer addressable chunks!
	
	//convert hex to integer
	if (sscanf(tempbuffer, "%x", &number) < 1)
		if (sscanf(tempbuffer, "%X", &number) < 1) {
			free(tempbuffer);
			errmes = "Could not parse hex chunk size!";
			return -3;
		}
	
	free(tempbuffer);
	
	return number;
	
}



struct hostent* Gethostbyname(char* name) {
	struct hostent* host;
	if ((host = gethostbyname(name)) == NULL) {
		errmes = "could not resolve hostname";
		return NULL;
	}
	return host;
		
}
int Inet_pton(int family, const char *strptr, void *addrptr) {
	int n;
	n = inet_pton(family, strptr, addrptr);
	if (n <= 0)
		errmes = "invalid hostname";
	return n;
}

void Printsockaddrin_concise(const struct sockaddr_in *addr) {
	char address[16];
	printf("%s:%d", inet_ntop(addr->sin_family, 
			&(addr->sin_addr),
			address,
			16), ntohs(addr->sin_port));
}

void Printsockaddrin(const struct sockaddr_in *addr) {
	char address[16];
	printf("Address: %s\n", inet_ntop(addr->sin_family, 
			&(addr->sin_addr),
			address,
			16));
	printf("Port: %d\n", ntohs(addr->sin_port));
	printf("Family: %d\n", addr->sin_family);

}
