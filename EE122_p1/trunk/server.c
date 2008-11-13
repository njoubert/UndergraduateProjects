//============================================================================
// Name        : EE122 SERVER
// Author      : Niels Joubert ee122-bw
// Version     :
// Copyright   : 2007 
// Description : Bezerker - a simple web server
//============================================================================

#include "server.h"

int options[1] = {0};

char* starttime = "";
long requestcounter = 0;

int setoption(char* name, int value) {
	if (strcmp(name,"-c") == 0) {
		options[0] = value;
		return value;
	} else
		return 0;
}

int getoption(char* name) {
	if (strcmp(name,"-c") == 0) {
		return options[0];
	} else
		return 0;
}

/*
 * The signal handler that exits the server cleanly.
 */
void exit_server(int sig) {
	free(starttime);
	close(listenfd);
	(void) signal(sig, SIG_DFL);
	raise(sig);
}

/*
 * Returns 0 or bigger if \r\n\r\n is at the end.
 * Returns -1 for no match.
 * */
int whereisCRLF(char* buffer, int len) {
	char* breakptr;
	breakptr = strstr(buffer, "\r\n\r\n");
	if (breakptr != NULL)
		return (breakptr - buffer);
	return -1;
}

void saveStartTime() {
	  char* temptime;
	  char* savetime;
	  time_t rawtime;
	  struct tm * timeinfo;
	  char* temp;

	  time ( &rawtime );
	  timeinfo = localtime ( &rawtime );
	  temptime = asctime (timeinfo);
	  savetime = malloc((strlen(temptime) + 1) * sizeof(char));
	  strcpy(savetime, temptime);
	  
	  
	  if ((temp = strchr(savetime, '\n')))
		  *temp = '\0';
	  
	  starttime = savetime;
	  
	
}

void setDefaultHeaders(http_response* response) {
	  char* requestcounterstring;
	  char* currtime;
	  time_t rawtime;
	  struct tm * timeinfo;

	  time ( &rawtime );
	  timeinfo = localtime ( &rawtime );
	  currtime = asctime (timeinfo);
	  if (strchr(currtime, '\n'))
		  *strchr(currtime, '\n') = '\0';

	  requestcounterstring = (char*) malloc(16*sizeof(char));
	  snprintf(requestcounterstring, 16, "%d", (int) requestcounter);
	  
	  set_http_header(response->header_list, "Date", currtime);
	  set_http_header(response->header_list, "Started At", starttime);
	  set_http_header(response->header_list, "Requests Served", requestcounterstring);
	  set_http_header(response->header_list, "Server", "Bezerker 1.0b");
	  
	  free(requestcounterstring);
	
}

int sendResponse(int sockfd, http_response* response, FILE* fileptr) {
	char* buffer;
	char* chunkbuffer;
	char* headerbuffer;
	int nsend, nread;
	int chunksize;
	
	if (getoption("-c")!=0)
		set_http_header(response->header_list, "Transfer-Encoding", "chunked");
	
	headerbuffer = restons(response);
	//printf("===========================\n");
	//printf(headerbuffer);
	//printf("===========================\n");
	if (Send(sockfd, headerbuffer, strlen(headerbuffer), 0) < strlen(headerbuffer))
		fprintf(ERROR_STREAM, "ERROR -- Incommunicable Error: %s\n", errmes);
	
	free(headerbuffer);
	
	if ((chunksize = getoption("-c")) != 0) {
		
		//CHUNK IT
		
		
		buffer = (char*) malloc((chunksize+1) * sizeof(char));
		chunkbuffer = (char*) malloc((16)*sizeof(char));
		nread = 1;
		
		while (nread > 0) {
			
			nread = fread(buffer, sizeof(char), chunksize, fileptr);
			buffer[nread] = 0;

			snprintf(chunkbuffer, 16, "%x\r\n", nread);
			
			nsend = Send(sockfd, chunkbuffer, strlen(chunkbuffer), 0);
			if (nread == 0) {
				nsend = Send(sockfd, "\r\n", 2, 0);
			} else {
				nsend = Send(sockfd, buffer, nread, 0);
			}
			if (nsend < 0)
			return -1; //ERROR!
			
		}
		fclose(fileptr);
		
		free(chunkbuffer);
		free(buffer);
		
		return 0;
		
		
		
	} else {
		
		
		//JUST SEND IT
		buffer = (char*) malloc((BUFFSIZE+1) * sizeof(char));
		
		nread = 1;
		while (nread > 0) {
			
			nread = fread(buffer, sizeof(char), BUFFSIZE, fileptr);
			buffer[BUFFSIZE] = 0;
			
			nsend = Send(sockfd, buffer, nread, 0);
			if (nsend < 0)
				return -1; //ERROR!
			
		}
		free(buffer);
		fclose(fileptr);
		return 0;
		
	}
	
	return 1; //ERROR! Kill this guy
}

FILE* handleFileTypes(http_response* response, char* filepath, int dir, int fileSize) {
	FILE* fileptr;
	char* ext;
	char* size;
	DIR *dp;
	struct dirent *ep;
	
//	printf("-----------\nFile: ");
//	printf(filepath);
//	printf("\n-----------\n");
	
	if (dir) {
		
		//LIST THE DIRECTORY
		fileptr = tmpfile();
		
		  dp = opendir (filepath);
		  if (dp != NULL) {
			  
			  while ((ep = readdir (dp))) {
		    	  fputs(ep->d_name, fileptr);
		    	  fputs("\r\n", fileptr);
		      }
		      (void) closedir (dp);
		      
		      fileSize = ftell(fileptr);
		      fseek(fileptr, 0, SEEK_SET);
		      set_http_header(response->header_list, "Content-Type", "text/plain");
		      
		  } else {
			  
				//Something went badly wrong!
				response->status_code = 400;
				set_response_message(response, "Bad Request: Could not open directory.");
				fclose(fileptr);
				return NULL;
		    
		  }
		
		
	} else {
		
			fileptr = fopen(filepath, "r");
			if (fileptr == NULL) {
				
				//Something went badly wrong!
				response->status_code = 400;
				set_response_message(response, "Bad Request: Could not open file.");
				fclose(fileptr);
				return NULL;
			}
			
			//FILE TYPE HANDLING
			ext = strrchr(filepath, '.');
			
			//printf("EXTENTION: %s\n", ext);
			
			if (strcasecmp(ext, ".TXT") == 0) {
				set_http_header(response->header_list, "Content-Type", "text/plain");
			} else if (strcasecmp(ext, ".HTML") == 0) {
				set_http_header(response->header_list, "Content-Type", "text/html");
			} else if (strcasecmp(ext, ".GIF") == 0) {
				set_http_header(response->header_list, "Content-Type", "image/gif");
			} else if (strcasecmp(ext, ".JPG") == 0) {
				set_http_header(response->header_list, "Content-Type", "image/jpg");
			} else {
				response->status_code = 501;
				set_response_message(response, "Not Implemented");
				fclose(fileptr);
				return NULL;
			}
		
		
	}
	
	
	size = (char*) malloc(36*sizeof(char));
	snprintf(size, 36, "%d", fileSize);
	response->status_code = 200;
	set_response_message(response, "OK");
	set_http_header(response->header_list, "Content-Length", size);
	free(size);
	
	return fileptr;
	
}

/*
 * Resolves the URI in the request to a file on disk if possible, 
 * and returns a handle to that file.
 * If unresolvable, returns a NULL pointer.
 * Sets the response codes appropriately.
 * */
FILE* fileFileFromUri(http_request* request, http_response* response) {
	FILE* fileptr = NULL;
	char* path;
	char* indexpath;
	char* finalpath;
	int accessible=0, len=0, directory=0, fileSize=0; //To handle permissions...
	struct stat normalbuf;
	struct stat indexbuf;
	
	path = reltoabspath(request->uri);

	//Get some info here!
	if (stat(path, &normalbuf) != 0) {
	
		//File doesnt exist
		free(path);
		response->status_code = 404;
		set_response_message(response, "Not Found");
		return NULL;
	
	}
	
	//We have something at that point...

	if ((normalbuf.st_mode & S_IFMT) == S_IFDIR) {
		
		//DIRECTORY
		
		len = strlen(path);
		if (path[len-1] == '/') {
			indexpath = (char *) malloc((len + 11)*sizeof(char));
			strcpy(indexpath, path);
			strcpy(indexpath+len, "index.html");
		} else {
			indexpath = (char*) malloc((len + 12)*sizeof(char));
			strcpy(indexpath, path);
			strcpy(indexpath+len, "/index.html");
		}
		
		if (stat(indexpath, &indexbuf) == 0) {
			
			//INDEX FILE FOUND
			free(path);
			finalpath = indexpath;
			if ((indexbuf.st_mode & S_IRUSR) == S_IRUSR)
				accessible = 1;
			fileSize = indexbuf.st_size;
			
		} else {
			
			//LIST THE DIRECTORY
			finalpath = path;
			directory = 1;
			if ((normalbuf.st_mode & S_IRUSR) == S_IRUSR)
				accessible = 1;
			fileSize = normalbuf.st_size;
		
		}
		
		
	} else {
		
		//FILE
		finalpath = path;
		if ((normalbuf.st_mode & S_IRUSR) == S_IRUSR)
			accessible = 1;
		fileSize = normalbuf.st_size;
		
	}
	
	if (accessible) {
		
		fileptr = handleFileTypes(response, finalpath, directory, fileSize);
		
	} else {
		
		response->status_code = 403;
		set_response_message(response, "Forbidden");
		
	}
		
	free(finalpath);
	return fileptr;
}

/*
 * Handles a client's request, valid or invalid.
 * Returns 0 if the client's connection should be closed.
 * */
int handleClientRequest(http_request* request, int sockfd) {
	http_response* response;
	char* sendbuffer;
	char* msgbuffer;
	FILE* filetosend;
	
	int presistthis=0;
	
	response = get_default_response();
	
	requestcounter++;
	
	setDefaultHeaders(response);
	
	//Set presistance correctly
	if (strcasecmp(get_http_header_val(request->header_list, "connection"), "keep-alive")) {
		presistthis = 0;
		set_http_header(response->header_list, "Connection", "close");
	} else {
		presistthis = 1; //Set to non-negative if we found a presistant header.
		set_http_header(response->header_list, "Connection", "Keep-Alive");
	}
	
	
	//request->req_errno = 1;
	
	
	if (request->req_errno != 0) {
		
		//We couldn't parse his request correctly.
		msgbuffer = (char*) malloc (14 + strlen(s_request_errno[request->req_errno]));
		response->status_code = 400;
		strcpy(msgbuffer, "Bad Request: ");
		strcpy(msgbuffer + 13, s_request_errno[request->req_errno]);
		set_response_message(response, msgbuffer);
		
		sendbuffer = restons(response);
		//printf(sendbuffer);
		if (Send(sockfd, sendbuffer, strlen(sendbuffer), 0) < strlen(sendbuffer))
			fprintf(ERROR_STREAM, "ERROR -- Incommunicable Error: %s\n", errmes);
		
		free(sendbuffer);
		
	} else {
		
		//Resolve the request to a file, or set the response
		//codes to the correct error:
		filetosend = fileFileFromUri(request, response);
		
		if (filetosend == NULL) {
			
			//The file could not be sent, so let the user know...
			sendbuffer = restons(response);
			//printf(sendbuffer);
			if (Send(sockfd, sendbuffer, strlen(sendbuffer), 0) < strlen(sendbuffer))
				fprintf(ERROR_STREAM, "ERROR -- Incommunicable Error: %s\n", errmes);
			free(sendbuffer);
			
		} else {
			
			
			if (sendResponse(sockfd, response, filetosend) != 0) {
				fprintf(ERROR_STREAM, "ERROR -- Incommunicable Error: %s\n", errmes);
				return 0;
			}
			free_http_response(response);
			return presistthis;
			
		}
		
	}
	
	free_http_response(response);
	return 0;
			
}

/*
 * Handles data coming in from the client.
 * Returns ZERO if we are DONE.
 * Returns NONZERO if the client should stay connected.
 * */
int handleClientData(int sockfd, int ind, char** buffers, int* indices, int* presistency) {
	char* recvbuffer = NULL;	
	char* fullbuffer = NULL;
	int nrecv;	
	int endoffset;
	int totallen;
	http_request* request;
	int dropthissock;
	
	recvbuffer = (char*) malloc((BUFFSIZE + 1) * sizeof(char));
	
	/* First we handle new */
	if ((nrecv = Recv(sockfd, recvbuffer, BUFFSIZE, 0)) < 0) {
		fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
		return 0;
	} else if (nrecv == 0) {
		return nrecv;
	}
	recvbuffer[nrecv] = 0;
	
	if ((endoffset = whereisCRLF(recvbuffer, nrecv)) >= 0) { //There is a CRLF! Respond!
		
		if (buffers[ind] == NULL) { //No buffer yet, just respond.
				
			fflush(stdout);
			totallen = nrecv;
			fullbuffer = recvbuffer;
			
		} else { //He has a buffer, concat and repond.
			
			totallen = nrecv + indices[ind];
			fullbuffer = (char*) malloc((totallen + 1)*sizeof(char));
			
			memcpy(fullbuffer, buffers[ind], indices[ind]);
			free(buffers[ind]);
			buffers[ind] = NULL;
			
			memcpy((fullbuffer + indices[ind]), recvbuffer, nrecv);
			free(recvbuffer);
			indices[ind] = -1;
			
			fullbuffer[totallen] = 0;
			
		}
		
	} else { //Oh, request is not complete! No CRLF, he'll have to stick around
		
		if (buffers[ind] == NULL) { //No buffer? make one
			
			buffers[ind] = (char*) malloc((BUFFSIZE + 1) * sizeof(char));
			memcpy(buffers[ind], recvbuffer, nrecv);
			indices[ind] = nrecv;
			free(recvbuffer);
			
		} else { //Buffer? resize and concat.
			
			buffers[ind] = (char*) realloc(buffers[ind], indices[ind] + BUFFSIZE + 1);
			memcpy(buffers[ind] + indices[ind], recvbuffer, nrecv);
			free(recvbuffer);
			indices[ind] += nrecv;
			
		}
		
		return 1; //DONT KILL THIS CLIENT YET!
	}
		
	//At this point we assume that fullbuffer contains all the headers
	//sent by the client.
	
	printf(fullbuffer);
	
	request = parse_formatted_request(fullbuffer);
	dropthissock = handleClientRequest(request, sockfd);
	request = free_http_request(request);
	free(fullbuffer);

	return dropthissock;
}

/*
 * Implements select() to create non-blocking IO.
 * Concepts from UNIX Network Programming by Stevens
 * */
int selectloop(int listenfd) {
	int maxcli, maxfd, ready, connfd, sockfd, i;
	int client[FD_SETSIZE]; //Array of socket numbers for clients
	char* buffers[FD_SETSIZE];
	int bufferpos[FD_SETSIZE];
	int presistency[FD_SETSIZE];
	fd_set read_set, master_set;
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	
	maxcli = -1;
	maxfd = listenfd;
	for (i = 0; i < FD_SETSIZE; i++) {
		buffers[i] = NULL;
		bufferpos[i] = -1;
		client[i] = -1;
		presistency[i] = 0;
	}
	FD_ZERO(&master_set);
	FD_SET(listenfd, &master_set);
	
	for (;;) {
		read_set = master_set;
		ready = select(maxfd + 1, &read_set, NULL, NULL, NULL);
		
		//New client connection
		if (FD_ISSET(listenfd, &read_set)) { 
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen);
			
			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			
			if (i == FD_SETSIZE) {
				fprintf(ERROR_STREAM, "ERROR -- Network Error: More sockets than OS supports!\n");
				exit_server(1);
			}
			if (i > maxcli)
				maxcli = i;
			if (connfd > maxfd)
				maxfd = connfd;
			FD_SET(connfd, &master_set);
			
			if (--ready <= 0)
				continue;
		}
		
		//Check all clients
		for (i = 0; i <= maxcli; i++) {
			if ((sockfd = client[i]) < 0)
				continue; 
			if (FD_ISSET(sockfd, &read_set)) {
				
				if (handleClientData(sockfd, i, buffers, bufferpos, presistency) <= 0) {
					close(sockfd);
					FD_CLR(sockfd, &master_set);
					client[i] = -1;
				}
				
				if (--ready <= 0)
					break;	
			}
		}
	}
}

int main(int argc, char* argv[]) {
	struct sockaddr_in servaddr;
	in_port_t port;
	int goodcommand=1;
	
	(void) signal(SIGINT, exit_server);
	
	port = DEFAULT_SERVER_PORT;
	
	if (argc == 3 || argc == 4)
		goodcommand = setoption(argv[1], atoi(argv[2]));
	
	if ((argc == 4 || argc == 2)) {
		if (argc == 2)
			port = (short) strtol(argv[1], (char**) NULL, 10);
		else if (argc == 4)
			port = (short) strtol(argv[3], (char**) NULL, 10);
		else
			port = DEFAULT_SERVER_PORT;
		
	} 
	if (goodcommand == 0) {
		printf("Usage: http_server [-c chunksize] [listen_port] \n\t\tPort defaults to %d\n", DEFAULT_SERVER_PORT);
		exit(1);
	}
	
	saveStartTime();
	
	retry:

	listenfd = Socket(AF_INET, SOCK_STREAM,0);
	if (listenfd < 0) {
		fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not get socket\n");
		exit_server(1);
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	
	
	if (Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not bind to port\n");
		exit_server(1);
	}
	if (Listen(listenfd, 5) < 0) {
		fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not put socket to listening state\n");
		exit_server(1);
	}
	
	printf("Server listening on port %d\n", port);
	selectloop(listenfd);

	close(listenfd);
	return EXIT_SUCCESS;
}
