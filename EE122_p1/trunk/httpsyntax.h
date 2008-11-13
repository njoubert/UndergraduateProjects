#ifndef HTTPSYNTAX_H_
#define HTTPSYNTAX_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

#define REQUEST_REGEX_FLAGS	REG_EXTENDED
#define URI_REGEX_FLAGS REG_ICASE|REG_EXTENDED
#define ERROR_STREAM stderr

#define MAXLINE 4096

/*
 * Represents all the possible HTTP Request types.
 * */
typedef enum e_request_type { 
	INVALID=0, GET=1, POST=2, DELETE=3, PUT=4, HEAD=5
} e_request_type;

/*
 * Represents errors in parsing an HTTP Request.
 * */
typedef enum e_request_errno {
	ERR_NONE=0, ERR_METHOD=1, ERR_PATH=2, ERR_VERSION=3, ERR_SPURIOUS=4, ERR_PORT=5, ERR_HOST=6
} e_request_errno;

/*
 * Reverse lookup table for errors
 * */
static char s_request_errno[7][50] = {
		{"ERROR -- Invalid Request object: "},
		{"ERROR -- Invalid Method token: "},
		{"ERROR -- Invalid Absolute-Path token: "},
		{"ERROR -- Invalid HTTP-Version token: "},
		{"ERROR -- Spurious token before CRLF: "},
		{"ERROR -- Invalid Port."},
		{"ERROR -- Invalid Host."}
};

/*
 * Represents the headers of a http request.
 * */
typedef struct http_header {
	char* name;
	char* value;
} http_header;

typedef struct http_header_list {
	int header_count;
	http_header* headers;
} http_header_list;

/*
 * Represents a single HTTP request.
 * */
typedef struct http_request {
	e_request_type type;
	char* host;
	short port;
	char* uri;
	short majorversion;
	short minorversion;
	http_header_list* header_list;
	e_request_errno req_errno;
} http_request;

typedef struct http_response {
	int majorversion;
	int minorversion;
	int status_code;
	char* reason_phrase;
	http_header_list* header_list;
} http_response;

/*
 * Attempts to parse a HTTP Request from the given string.
 * Always returns a pointer to a valid http_request.
 * IT IS UP TO YOU TO FREE THIS!
 * */
http_request* parse_formatted_request(char* request_string);

/*
 * Parses a human-entered URL into an http_request object.
 * Copies the uri_string contents into its own buffers.
 * @param uri_string 		- string to parse
 * @return http_request on success, NULL on failure
 * */
http_request* urlstring_to_request(char* uri_string);

http_response* parse_http_reponse_head(char* head);

/*
 * Converts the given request to a network string.
 * YOU ARE RESPONSIBLE FOR FREEING THE RETURNED POINTER!
 * */
char* reqtons(const http_request* req);


/*
 * Converts the given response to a network string.
 * YOU ARE RESPONSIBLE FOR FREEING THE RETURNED POINTER!
 * */
char* restons(const http_response* res);

/*
 * Prints the details of an http_request to stdout.
 * */
void print_http_request(const http_request* req, int full);

void print_http_response(const http_response* res);

/*
 * Allocates space on the heap and copies the given uri to it.
 * Sets http_request's uri to point to this.
 * @param urilen excludes the NULL at end of char*, this funct adds it
 * */
int set_request_uri(http_request* req, char* uri, int urilen);

int set_response_message(http_response* res, char* msg);

/*
 * 
 * Sets a name, value pair in the http header.
 * Will overwrite the value if the name already exists
 * Copies the given name, value strings and expands the 
 * request object as necessary.
 * */
void set_http_header(http_header_list* list, char* name, char* value);

/*
 * Returns the offset into the http_header array inside the
 * request where the header with the specific name can be found.
 * Case-sensitive - looks for exact match!
 * */
int get_http_header(http_header_list* list, char* name);

char* get_http_header_val(http_header_list* list, char* name);

http_response* parse_http_response_head(char* head);

int parse_http_headerlines(http_header_list* list, char* head, int offset);

/*
 * Allocates space for a http_request on the heap.
 * Returns NULL on failure
 * */
http_request* calloc_http_request();

/*
 * Allocates space for a http_response on the heap.
 * Returns NULL on failure
 * */
http_response* calloc_http_response();

/*
 * Creates a request with the default options set,
 * leaving only host to be filled in
 * */
http_request* get_default_request();

http_response* get_default_response();

/*
 * Frees the header list;
 * */
void free_http_header_list(http_header_list* header_list);

/*
 * Frees the heap space taken by an http_request including URI
 * */
void* free_http_request(http_request *req);

void* free_http_response(http_response *res);

#endif /*HTTPSYNTAX_H_*/
