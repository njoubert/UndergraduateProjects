//============================================================================
// Name        : EE122 CLIENT
// Author      : Niels Joubert
// Version     :
// Copyright   : 2007 
// Description : Hello World in C, C99-Style
//============================================================================

#include "client.h"

int options[1] = {0};
connstate conn; //This should always reflect the currently connected status.

void exit_server(int sig) {
	if (conn.host != NULL) {
		free(conn.host);
		conn.host = NULL;
	}
	(void) signal(sig, SIG_DFL);
	raise(sig);

}

int setoption(char* name, int value) {
	if (strcmp(name,"-p") == 0) {
		options[0] = value;
		return value;
	} else
		return 0;
}

int getoption(char* name) {
	if (strcmp(name,"-p") == 0) {
		return options[0];
	} else
		return 0;
}

//At startup of program, we extract the command line options
//or print a nice error message.
int extractoptions(int argc, char** argv) {
	int i;
	for (i = 1; i < argc; i++) {
		if (setoption(argv[i], 1) == 0) {
			printf("Usage: http_client [-p]\n");
			printf("\tWe did not understand \"%s\"\n", argv[i]);
			exit(1);
		}
	}
	return 0;
}

void set_headers(http_header_list* header_list) {
	if (getoption("-p"))
		set_http_header(header_list, "Connection", "Keep-Alive");
	set_http_header(header_list, "User-Agent", "Mozilla/5.0 Bezerker/1.b");
}

void set_presistency(http_response* res) {
	int i;
	if (getoption("-p") == 0)
		return;
	
	i = get_http_header(res->header_list, "Connection");
	if (i < 0) {
		conn.presistent = 0;
		return;
	} else if (strcasecmp(res->header_list->headers[i].value, "Keep-Alive") != 0)
		conn.presistent = 0;
	
}

/**
 * Reads a line from the standard input
 * */
char* geturlline() {
	char* line = NULL;
	char buff[INPUT_BUFFER_SIZE];
	int c, i;
	int total=0; //The total amount of chars read in so far
	int more=1; //Have we received a \n yet?
	
	while (more) {
		
		i = 0;		
		while (i < INPUT_BUFFER_SIZE && (c=getchar()) != EOF && c != '\n') //i will never be bigger than the limit
			buff[i++] = c;
		if (c == EOF)
			exit(0);
		if (c == '\n') {
			buff[i++] = '\0';
			more = 0;
		}
		
		if (line == NULL)
			line = (char*) malloc(i*sizeof(char));
		else 
			line = (char*) realloc(line, (total+i)*sizeof(char));

		if (line == NULL)
			exit(1);
				
		memcpy((line + total), buff, i);
		
		total += i;
	
	}
	
	return line;
	
}

/*
 * Saves the information about the currently connected host
 * to the connstate variable. it also manages memory for pointers
 * inside this structure
 * */
void save_state_info(connstate* connptr, http_request* request, int sockfd) {
	char* temphost;
	
	connptr->port = request->port;
	connptr->established = 1;
	connptr->presistent = getoption("-p");
	connptr->sockfd = sockfd;
	
	if (connptr->host != NULL)
		free(connptr->host);
	temphost = (char*) malloc((strlen(request->host)+1)*sizeof(char));
	strcpy(temphost, request->host);
	connptr->host = temphost;
}

FILE* create_local_file(http_request* req) {
	char* filename = NULL;
	char* last;
	char* uri = (char*) malloc((strlen(req->uri) + 1)*sizeof(char));
	FILE* file;
	strcpy(uri, req->uri);
	
	last = strrchr(uri, '/');
	if (last == NULL)
		return NULL;
	
	if ((strcmp(last, "/") == 0) && (last - uri) > 0) {
		*(last) = 0;
		last = strrchr(uri, '/');
		filename = last + 1;
			
	} else if (strcmp(last, "/.") == 0) {
		
		filename = "dot";
		
	} else if (strcmp(last, "/..") == 0) {
		
		filename = "dotdot";
		
	} else if (strcmp(last, "/") == 0) {
		
		filename = "dir";
		
	} else {
		
		filename = (last + 1);
	}
	//printf("SAVING %s TO FILE %s\n", uri, filename);
	file = fopen(filename, "w");
	free(uri);
	return file;
}

/*
 * Manages closing sockets gracefully, keeping them alive if a keep-alive
 * connection was established.
 * */
int Close_socket(int sockfd, connstate* connptr) {
	if (connptr->closeme || !connptr->presistent || !connptr->established) {
		if (sockfd > 0)
			close(sockfd);
		connptr->established = 0;
		connptr->presistent = 0;
		connptr->sockfd = 0;
		connptr->closeme = 0;
		return 1;
	}
	
	return 0;
}

int receive_request(http_response* response, int sockfd, FILE* outfile) {
	int nrecv;
	int total;
	int recvsize;
	int content_len = 0;
	int chunk_len = 0;
	char recvbuffer[BUFFSIZE+1];
	
	
	if (strcasecmp(get_http_header_val(response->header_list, "Transfer-encoding"), "chunked") == 0) {
			//CHUNKED TRANSFER
		
			do {
				
				chunk_len = Recv_chunk_size(sockfd);
				
				if (chunk_len < 0) {
					fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
					conn.closeme = 1;
				}
				if (chunk_len == 0) {
					if ((nrecv = Recv(sockfd, recvbuffer, 2, 0)) <= 0) {
						
						//RECIEVING PROBLEMS
						fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
						conn.closeme = 1;
						
					}
				}

				//printf("Dowloading chunk size %d bytes.\n", chunk_len);				
		
				nrecv = 0;
				total = 0;
				while (total < chunk_len) {
					
					if ((chunk_len - total) > BUFFSIZE)
						recvsize = BUFFSIZE;
					else
						recvsize = (chunk_len - total);
					
					if ((nrecv = Recv(sockfd, recvbuffer, recvsize, 0)) <= 0) {
						
						//RECIEVING PROBLEMS
						fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
						conn.closeme = 1;
						
					} else {
						
						total += nrecv;
						recvbuffer[nrecv] = 0;
						fwrite(recvbuffer, sizeof(char), nrecv, outfile);
						
					}
					
				}
				
			
			} while (chunk_len > 0);
			//printf("DONE!\n");
		
			
		
	} else if (conn.presistent) {
			//PRESISTENT CONNECTION, NO CHUNKING
		
		
			sscanf(get_http_header_val(response->header_list, "Content-Length"), "%d", &content_len);
			//printf("Presistent: Content-Length: %d", content_len);
			fflush(stdout);
			
			nrecv = 0;
			total = 0;
			while (total < content_len) {
				
				if ((content_len - total) > BUFFSIZE)
					recvsize = BUFFSIZE;
				else
					recvsize = (content_len - total);
			
				if ((nrecv = Recv(sockfd, recvbuffer, recvsize, 0)) < 0) {
					
					//RECIEVING PROBLEMS
					
					fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
					conn.closeme = 1;
					
				} else if (nrecv == 0) {
					
					//fprintf(ERROR_STREAM, "ERROR -- Network Error: Connection Closed.\n");
					total = content_len;
					conn.closeme = 1;
					conn.presistent = 0;
					
				} else {
					
					total += nrecv;
					fwrite(recvbuffer, sizeof(char), nrecv, outfile);
				
				}
			}
		
		
		
	} else {
			//NORMAL CONNECTION
		
		
			conn.presistent = 0;
			nrecv = 1;
			while (nrecv != 0) {
			
				if ((nrecv = Recv(sockfd, recvbuffer, BUFFSIZE, 0)) < 0) {
					
					//RECIEVING PROBLEMS
					fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
					conn.closeme = 1;
					
				} else {
					
					fwrite(recvbuffer, sizeof(char), nrecv, outfile);
				
				}
			}
	
			
	}
	
	//printf("FILE SAVED!\n");
	return 0;

}
/*
 * Tries to connect to the given hostname and host order port, 
 * sets the servaddr structure, 
 * and return an ESTABLISHED sockfd.
 * Returns -1 if we could not connect to the host
 * and -2 if the hostname did not resolve.
 */
int Connect_to_hostname(http_request* request, connstate* connptr) {
	struct in_addr **inethandle;
	struct hostent *hp;
	int sockfd,port;
	char* hostname;
		
	hostname = request->host;
	port = request->port;
	
	//Check state info
	if (getoption("-p") && (connptr->host != NULL) && (connptr->established != 0)) {
			if ((strcasecmp(connptr->host, request->host) == 0) &&
					(connptr->port == request->port) &&
					(connptr->presistent != 0)) {
				//We have an active presistent connection
				return connptr->sockfd;
			} else {
				//We have an open connection, but not the one we want
				close(connptr->sockfd);
				connptr->established = 0;
			}	
	}
	
	if ((hp = Gethostbyname(hostname)) == NULL)
		return -2;
	else
		inethandle = (struct in_addr **) hp->h_addr_list;
	
	while (*inethandle != NULL) {
		if ((sockfd = Socket(AF_INET, SOCK_STREAM, 0)) < 0)
			return sockfd;
		
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(port);
		memcpy(&servaddr.sin_addr, *inethandle, sizeof(struct in_addr));
		
		if (Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == 0)
			break;
		close(sockfd);
		inethandle += 1;
	}
	if (*inethandle == NULL)
		return -1;
	
	//Save State Info
	save_state_info(connptr, request, sockfd);
	
	return sockfd;
}

/*
 * The point of entry for the client. 
 */
int main(int argc, char* argv[]) {
	int sockfd;
	http_request *request;
	http_response *response = NULL;
	FILE* outfile;
	char* inputbuffer;
	char* sendbuffer;
	char* response_headers_buffer;

	int nsend;

	(void) signal(SIGINT, exit_server);
	
	extractoptions(argc, argv);
	
	conn.host = NULL;
	conn.port = 0;
	conn.presistent = 0;
	conn.established = 0;
	conn.closeme = 0;
	
	while (1) {
	
			/* FIRST WE HANDLE INPUT */
		
			inputbuffer = geturlline();
			
			if ((request = urlstring_to_request(inputbuffer)) == NULL) {
				fprintf(ERROR_STREAM, "ERROR -- Invalid URL: %s\n", inputbuffer);
				free(request);
				free(inputbuffer);
				continue;
			}
			free(inputbuffer);
			
			/* THEN WE HANDLE CONNECTIONS */
			
			sockfd = Connect_to_hostname(request, &conn);
			
			set_headers(request->header_list);
			
			/* SENDING AND RECIEVING */
			
			sendbuffer = reqtons(request);
			nsend = strlen(sendbuffer);
			//printf("SENDBUFFER: \n%s\n", sendbuffer);
			
			if (sockfd < 0) {
				
					//CONNECTION PROBLEMS
					if (sockfd == -2)
						fprintf(ERROR_STREAM, "ERROR -- Network Error: Unknown host\n");
					else
						fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not connect to host\n");
					
					//This should break out of the if statement
					
			} else if (Send(sockfd, sendbuffer, nsend, 0) < nsend) {
				
					//SENDING PROBLEMS
					fprintf(ERROR_STREAM, "ERROR -- Network Error: %s\n", errmes);
					
					//This should break out of the if statement
					
			} else {
				
				//SENDING SUCCESS
				
				//RECEIVE THE FULL HEADERS
				
				response_headers_buffer = Recv_headers(sockfd);
				//printf("----------------\n");
				//printf("RESPONSE: \n%s\n", response_headers_buffer);
				//printf("----------------\n");
				response = parse_http_response_head(response_headers_buffer);
				
				free(response_headers_buffer);
				
				if (response == NULL) {
					
					conn.closeme = 1;
					fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not parse HTTP Response headers!\n");
						
				} else {
					
					//printf("===========================\n");
					print_http_response(response);
					//printf("===========================\n");
					
					set_presistency(response);
					
					//DECIDE WHAT TO DO FROM HERE - NORMAL, CHUNKED OR PRESISTENT
					
					if (response->status_code == 200) {
						
						outfile = create_local_file(request);
						
						if (outfile == NULL) {
						
							fprintf(ERROR_STREAM, "ERROR -- File Error: Could not create local file.\n");
						
						} else {
							
							if (receive_request(response, sockfd, outfile) != 0) {
								fprintf(ERROR_STREAM, "ERROR -- Network Error: Could not recieve request.\n");;//ERROR ERROR ERROR!
								conn.closeme = 1;
							}
						}
								
						if (outfile != NULL)
							fclose(outfile);
						
						//printf("Current HTTP Resonse:\n");
						//print_http_response(response);
					}
	
				}

			}
			
			if (request != NULL)
				request = free_http_request(request);
			if (response != NULL)
				response = free_http_response(response);
			if (sendbuffer != NULL) {
				free(sendbuffer);
				sendbuffer = NULL;
			}
			fflush(stdout);
			
			Close_socket(sockfd, &conn);
			
	}
	

//	if (sendbuffer != NULL)
//		free(sendbuffer);
//	if (request != NULL)
//		free(request);
	
	return EXIT_SUCCESS;
}
