#include "httpsyntax.h"



/*
 * Reverse lookup table for request-types
 * */
static char s_request_type[6][8] = {{"INVALID"},{"GET"},{"POST"},{"DELETE"},{"PUT"},{"HEAD"}};

/* 
 * The following static variables define the HTTP Request format.
 * See http://www.w3.org/Protocols/rfc1945/rfc1945
 * */

static char* request_method_regex = "GET"; //Later on this can be [[:alpha:]]+
static size_t request_method_regex_subs = 0;

static char* request_uri_regex = "^[[:blank:]]+(/([[:alnum:]]|[.]|[-]|[_]|[/])*)"; 
static size_t request_uri_regex_subs = 2;

static char* request_version_regex = "^[[:blank:]]+HTTP/([[:digit:]]+).([[:digit:]]+)";
static size_t request_version_regex_subs = 2;

static char* request_termination_regex = "^[[:blank:]]*\r\n";
static size_t request_termination_regex_subs = 0;

/*
 * Matches the given string against the given regex.
 * YOU ARE RESPONSIBLE FOR FREEING THE RETURNED POINTER!
 * @param string			- string input to match
 * @param regex 			- pattern string to match
 * @param subs				- number of subpatterns in regex 
 * @param compflags			- flags to pass to regcomp
 * @param execflags			- flags to pass to regexec
 * @return regmatch_t*		- pointer to regmatch array, NULL if no matches, -1 if error occured
 * */
regmatch_t* match_regex(char* string, char* regex, size_t subs, int compflags, int execflags) {
	int n;
	regex_t regex_c;
	regmatch_t *regex_pmatch;
	regex_pmatch = NULL;
	
	if ((n = regcomp(&regex_c, regex, compflags)) > 0) {
		regfree(&regex_c);
		return (void*) -1;
	}
	
	regex_pmatch = (regmatch_t*) malloc((subs + 1)  * sizeof(regmatch_t));
	if (regex_pmatch == NULL) {
		regfree(&regex_c);
		return (void*) -1;
	}
	n = regexec(&regex_c, string, subs + 1, regex_pmatch, execflags);
	regfree(&regex_c);
	
	if (n != 0) {
		free(regex_pmatch);
		if (n == REG_NOMATCH)
			return NULL;
		else
			return (void*) -1;
	}
	return regex_pmatch;
}

/* ##################################################
 * 
 * 		The following code deals with translating
 * 		a hopefully well-formed GET request line
 * 		into an internal structure.
 * 
 * ##################################################
 * */

/*
 * Greedily matches for the method in the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or 0 on error.
 * */
int match_request_method(http_request* request, char* request_string) {
	regmatch_t *sub_match;
	int nextoffset;
	
	sub_match = match_regex(request_string, 
			request_method_regex, 
			request_method_regex_subs, 
			REQUEST_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		request->req_errno = ERR_METHOD;
		free(sub_match);
		return 0;
	}
	
	request->type = GET;
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * Greedily matches for the uri in the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or 0 on error.
 * */
int match_request_uri(http_request* request, char* request_string) {
	regmatch_t *sub_match;
	int nextoffset, strlength;
	
	sub_match = match_regex(
			request_string, 
			request_uri_regex, 
			request_uri_regex_subs,
			REQUEST_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		free(sub_match);
		request->req_errno = ERR_PATH;
		return 0;
	}
	
	strlength = sub_match[1].rm_eo - sub_match[1].rm_so;
	
	if (request->uri != NULL)
		free(request->uri);
	
	request->uri = malloc((strlength+1) * sizeof(char));
	if (request->uri == NULL) {
		printf("MALLOC FAILED!\n");
		request->req_errno = ERR_PATH;
		free(sub_match);
		return 0;
	}
	memcpy(request->uri, (request_string + sub_match[1].rm_so), strlength);
	request->uri[strlength] = 0;
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * Greedily matches for the version in the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or 0 on error.
 * */
int match_request_version(http_request* request, char* request_string) {
	int nextoffset, major_n, minor_n;
	char* major_c;
	char* minor_c;
	regmatch_t *sub_match;
	
	sub_match = match_regex(
			request_string, 
			request_version_regex, 
			request_version_regex_subs,
			REQUEST_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		request->req_errno = ERR_VERSION;
		return 0;
	}
		
	major_n = sub_match[1].rm_eo - sub_match[1].rm_so;
	minor_n = sub_match[2].rm_eo - sub_match[2].rm_so;
	major_c = (char*) malloc((major_n+1) * sizeof(char));
	minor_c = (char*) malloc((minor_n+1) * sizeof(char));
	
	if (major_c == NULL || minor_c == NULL) {
		printf("MALLOC FAILED!\n");
		request->req_errno = ERR_VERSION;
		if (major_c != NULL)
			free(major_c);
		if (minor_c != NULL)
			free(minor_c);
		free(sub_match);
		return 0;
	}
	
	memcpy(major_c, (request_string + sub_match[1].rm_so), major_n);
	memcpy(minor_c, (request_string + sub_match[2].rm_so), minor_n);
	major_c[major_n] = 0;
	minor_c[minor_n] = 0;
	
	request->majorversion = atoi(major_c);
	request->minorversion = atoi(minor_c);
	
	free(major_c);
	free(minor_c);
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;

}

/*
 * Greedily matches for the request termination in the given string.
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match
 * */
int match_request_termination(http_request* request, char* request_string) {
	regmatch_t *sub_match;
	int nextoffset;
	
	sub_match = match_regex(
			request_string, 
			request_termination_regex, 
			request_termination_regex_subs,
			REQUEST_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		request->req_errno = ERR_SPURIOUS;
		free(sub_match);
		return 0;
	}

	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * See httpsyntax.h
 * */
http_request* parse_formatted_request(char* request_string) {
	http_request *request;
	request = get_default_request();
	int offset, pos;
	offset = 0;

	if ((pos = match_request_method(request, request_string)) <= 0)
		return request;
	offset += pos;
	
	if ((pos = match_request_uri(request, (request_string + offset))) <= 0)
		return request;
	offset += pos;
	
	if ((pos = match_request_version(request, (request_string + offset))) <= 0)
		return request;
	offset += pos;
	
	if ((pos = match_request_termination(request, (request_string + offset))) <= 0)
		return request;
	offset += pos;
	
	if (parse_http_headerlines(request->header_list, request_string, offset)) {
		free_http_request(request);
		return NULL;
	}
	
	return request;
}

/* ##################################################
 * 
 * 		END OF MANIPULATING WELL-FORMED REQUESTS
 * 
 * ##################################################
 * */

/* ##################################################
 * 
 * 		WORKING WITH HTTP RESPONSES
 * 
 * ##################################################
 * */

/*
 * head is a character array, with headers terminated by \r\n
 * including the final header.
 * */
http_response* parse_http_response_head(char* head) {
	http_response* res;
	char* token;
	char* name;

	int offset;
	int i, total=0;
	res = get_default_response();
	if (head == NULL)
		return NULL;

	//printf("===========\n");
	//printf(head);
	//printf("===========\n");
		
	if ((token = strtok(head, "\r\n")) == NULL) {
		
		free_http_response(res);
		res = NULL;
		return NULL;
		
	} else  {
		 //Extract HTTP/%d.%d <code> <reason>
		
		i = sscanf(token, "HTTP/%d.%d %d %n", 
				&res->majorversion, 
				&res->minorversion, 
				&res->status_code,
				&total);
		
		if (i < 3) {
			free_http_response(res);
			res = NULL;
			return NULL;
		}
		
		if (res->reason_phrase != NULL)
			free(res->reason_phrase);
		name = (char*) malloc((strlen(token) - total + 1)*sizeof(char));
		strcpy(name, token + total);
		res->reason_phrase = name;
		
		
	}
	
	//printf("FIRST TOKEN: %s\n", token);
	offset = strlen(token) + 1;
	
	if (parse_http_headerlines(res->header_list, head, offset)) {
		free_http_response(res);
		return NULL;
	}
	
	return res;
	
}

int parse_http_headerlines(http_header_list* list, char* head, int offset) {
	char* token;
	char* name;
	char* val;
	
	if (list == NULL)
		return 1;

	while (((token = strtok((head + offset), "\r\n")) != NULL)) {
		
		//printf("TOKEN: %s\n", token);
			if (strcmp(token, "") == 0) { //Skips \n if we found \r
				offset += 1;
				continue;
			}
			offset += strlen(token) + 2;
			
			name = strtok(token, ":");
			val = (token + strlen(name) + 1);
			if (name == NULL || val == NULL) {
				return 1; //ERROR!
			} else {
				set_http_header(list, name, val);
			}
		
	}
	
	return 0;
	
}

/* ##################################################
 * 
 * 		END OF HTTP RESPONSE CODE
 * 
 * ##################################################
 * */

/* ##################################################
 * 
 * 		The following code deals with translating
 * 		TEXTUAL URLs into HTTP Requests
 * 
 * ##################################################
 * */

static char* uri_protocol_regex = "^http://";
static size_t uri_protocol_regex_subs = 0;

static char* uri_host_regex = "^(([[:alpha:]][[:alnum:]]*([.][[:alpha:]][[:alnum:]]*)*)|([[:digit:]]+[.][[:digit:]]+[.][[:digit:]]+[.][[:digit:]]+))";
static size_t uri_host_regex_subs = 4;

static char* uri_port_regex = "^(:([[:digit:]]+))?";
static size_t uri_port_regex_subs = 2;

static char* uri_path_regex = "^(/([[:alnum:]]|[[:digit:]]|[.]|[-]|[_]|[/])*)?$"; 
static size_t uri_path_regex_subs = 2;

/*
 * Greedily matches for the port at the beginning of the given string.
 * It returns the offset into the string of the first char after the match,
 * or 0 on error.
 * */
int match_url_protocol(http_request* request, char* uri_string) {
	regmatch_t *sub_match;
	int nextoffset;
	
	sub_match = match_regex(uri_string, uri_protocol_regex, uri_protocol_regex_subs,URI_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		free(sub_match);
		return 0;
	}
	
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * Greedily matches for the port at the beginning of the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or 0 on error.
 * */
int match_url_host(http_request* request, char* uri_string) {
	regmatch_t *sub_match;
	int nextoffset, strlength;
	
	sub_match = match_regex(uri_string, 
			uri_host_regex, 
			uri_host_regex_subs,
			URI_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		free(sub_match);
		return 0;
	}
	
	strlength = (sub_match[1].rm_eo - sub_match[1].rm_so);
	
	request->host = malloc((strlength+1) * sizeof(char));
	if (request->uri == NULL) {
		printf("MALLOC FAILED!\n");
		request->req_errno = ERR_HOST;
		free(sub_match);
		exit(1);
	}
	memcpy(request->host, (uri_string + sub_match[1].rm_so), strlength);
	request->host[strlength] = 0;
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * Greedily matches for the port at the beginning of the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or -1 on error.
 * */
int match_url_port(http_request* request, char* uri_string) {
	regmatch_t *sub_match;
	int nextoffset, strlength, p;
	char* port_c;
	
	sub_match = match_regex(uri_string, 
			uri_port_regex, 
			uri_port_regex_subs,
			URI_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		free(sub_match);
		return -1;
	}
	
	if ((sub_match[1].rm_eo - sub_match[1].rm_so) == 0) {
		free(sub_match);
		return 0;
	}
	
	strlength = (sub_match[2].rm_eo - sub_match[2].rm_so);
	if (strlength > 5) {	//Ports cannot be larger than 2^16 = 65536 = 5 characters
		free(sub_match);
		return -1;
	}
		
	port_c = (char*) malloc((strlength+1) * sizeof(char));
	if (port_c == NULL) {
		printf("MALLOC FAILED!\n");
		request->req_errno = ERR_PORT;
		free(sub_match);
		exit(1);
	}
	memcpy(port_c, (uri_string + sub_match[2].rm_so), strlength);
	port_c[strlength] = 0;
	p = atoi(port_c);
	free(port_c);
	
	if (p > 65536) { //Ports cannot be larger than 2^16 = 65536
		free(sub_match);
		return -1;
	}
	request->port = p;
		
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * Greedily matches for the path at the beginning of the given string.
 * On a match, it fills in the given request
 * On a failure it sets the struct's errorcode
 * It returns the offset into the string of the first char after the match,
 * or -1 on error.
 * */
int match_url_path(http_request* request, char* uri_string) {
	regmatch_t *sub_match;
	int nextoffset, strlength;
	
	sub_match = match_regex(uri_string, 
			uri_path_regex, 
			uri_path_regex_subs,
			REQUEST_REGEX_FLAGS, 0);
	
	if (sub_match == NULL || sub_match < 0) {
		request->req_errno = ERR_PATH;
		return -1;
	}
	
	strlength = sub_match[1].rm_eo - sub_match[1].rm_so;
	
	if (strlength == 0) {
		free(sub_match);
		return 0;
	}
	
	if (request->uri != NULL)
		free(request->uri);
	request->uri = malloc((strlength+1) * sizeof(char));
	if (request->uri == NULL) {
		printf("MALLOC FAILED!\n");
		request->req_errno = ERR_PATH;
		free(sub_match);
		exit(1);
	}

	memcpy(request->uri, (uri_string + sub_match[1].rm_so), strlength);
	request->uri[strlength] = 0;
	nextoffset = (int) sub_match[0].rm_eo;
	free(sub_match);
	return nextoffset;
}

/*
 * See httpsyntax.h
 * */
http_request* urlstring_to_request(char* uri_string) {
	http_request *request;
	int offset, pos, maxlen;
	
	request = get_default_request();
	maxlen = strlen(uri_string);
	offset = 0;
	pos = 0;

	if ((pos = match_url_protocol(request, uri_string)) <= 0) {
		free_http_request(request);
		return NULL;
	}	
	offset += pos;
	
	if ((pos = match_url_host(request, (uri_string + offset))) <= 0) {
			free_http_request(request);
			return NULL;
	}
	offset += pos;

	if (offset >= maxlen)
		return request;
	
	//only less-that since this is not required
	if ((pos = match_url_port(request, (uri_string + offset))) < 0) { 
			free_http_request(request);
			return NULL;
	}
	offset += pos;

	if (offset >= maxlen)
		return request;
	
	//only less-that since this is not required
	if ((pos = match_url_path(request, (uri_string + offset))) < 0) {
			free_http_request(request);
			return NULL;
	}
	
	return request;
	
}




/* ##################################################
 * 
 * 		END OF MANIPULATING TEXTUAL REQUESTS
 * 
 * ##################################################
 * */




/*
 * See httpsyntax.h
 * */
char* reqtons(const http_request* req) {
	char* reqstring;
	int total;
	int i;
	if ((req->type == INVALID) | (req->req_errno > 0)) {
		return NULL;
	}
	reqstring = (char*) calloc(MAXLINE, sizeof(char));
	if (reqstring == NULL)
		return NULL;
	
	total = snprintf(reqstring, MAXLINE, 
			"%s %s HTTP/%d.%d\r\n", 
			s_request_type[req->type],
			req->uri,
			req->majorversion,
			req->minorversion);
	
	for (i = 0; i < req->header_list->header_count; i++) {
		total += snprintf((reqstring + total), MAXLINE - total, 
				"%s: %s\r\n",
				req->header_list->headers[i].name,
				req->header_list->headers[i].value);
	}
	
	snprintf((reqstring + total), MAXLINE - total, "\r\n");
	
	return reqstring;
}

/*
 * See httpsyntax.h
 * */
char* restons(const http_response* res) {
	char* resstring;
	int total;
	int i;
	if (res == NULL)
		return NULL;
	
	resstring = (char*) calloc(MAXLINE, sizeof(char));
	if (resstring == NULL)
		return NULL;
	
	total = snprintf(resstring, MAXLINE,
			"HTTP/%d.%d %d %s\r\n",
			res->majorversion,
			res->minorversion,
			res->status_code,
			res->reason_phrase);
	
	for (i = 0; i < res->header_list->header_count; i++) {
		total += snprintf((resstring + total), MAXLINE - total, 
				"%s: %s\r\n",
				res->header_list->headers[i].name,
				res->header_list->headers[i].value);
	}
	
	snprintf((resstring + total), MAXLINE - total, "\r\n");
	
	return resstring;
}

void print_header_list(const http_header_list* list) {
	int i;
	for (i = 0; i < list->header_count; i++)
		printf("%s: %s\n", 
				list->headers[i].name, 
				list->headers[i].value);
	
}

/*
 * See httpsyntax.h
 * */
void print_http_request(const http_request* req, int full) {
	if ((req->type == INVALID) | (req->req_errno > 0)) {
		printf("%s\n", s_request_errno[req->req_errno]);
	} else {	
		printf("%s %s HTTP/%d.%d\n", 
				s_request_type[req->type],
				req->uri,
				req->majorversion, 
				req->minorversion);
		if (full > 0) {
			if (req->host != NULL)
				printf("Host = %s\n", req->host);
			printf("Port = %d\n", req->port);
		}
		print_header_list(req->header_list);
	}
	
	fflush(stdout);
}

void print_http_response(const http_response* res) {
	printf("HTTP/%d.%d ", res->majorversion, res->minorversion);
	printf("%d %s\n", res->status_code, res->reason_phrase);
	if (res->status_code == 200)
		print_header_list(res->header_list);
	
}

/*
 * See httpsyntax.h
 * */
int set_request_uri(http_request* req, char* uri, int urilen) {
	char* ptr;
	ptr = (char *) malloc(urilen+1);
	if (ptr == NULL)
		return 0;
	memcpy(ptr, uri, (size_t) urilen);
	*(ptr + urilen) = 0;
	if (req->uri != NULL)
		free(req->uri);
	req->uri = ptr;
	return 0;
}

/*
 * See httpsyntax.h
 * */
int set_response_message(http_response* res, char* msg) {
	char* ptr;
	if (res == NULL)
		return 0;
	if (res->reason_phrase != NULL)
		free(res->reason_phrase);
	ptr = (char*) malloc((strlen(msg) + 1)*sizeof(char));
	strcpy(ptr, msg);
	res->reason_phrase = ptr;
	return 0;
}

/*
 * See httpsyntax.h
 * */
void set_http_header(http_header_list* list, char* name, char* value) {
	int i, header_count;
	char* tempval;
	char* tempname;
	http_header* tempheader;
	
	header_count = list->header_count;
	
	name = name + strspn(name, " ");
	value = value + strspn(value, " ");
	
	//First check whether the header already exists
	if ((i = get_http_header(list, name)) > -1) {
		if (list->headers[i].value != NULL)
			free(list->headers[i].value);
		tempval = (char*) malloc((strlen(value)+1)*sizeof(char));
		list->headers[i].value = strcpy(tempval, value);
		if (tempval == NULL)
			exit(1);
		return;
	}
	
	
	//If not, resize header array, and add new header
	tempheader = (http_header*) malloc((header_count + 1)*sizeof(http_header));
	tempval = (char*) malloc((strlen(value)+1)*sizeof(char));
	tempname = (char*) malloc((strlen(name)+1)*sizeof(char));
	strcpy(tempval, value);
	strcpy(tempname, name);
	
	if (header_count > 0) {
		memcpy(tempheader, list->headers, header_count*sizeof(http_header));
		free(list->headers);
	}
	
	tempheader[header_count].name = tempname;
	tempheader[header_count].value = tempval;
	list->headers = tempheader;
	list->header_count = (header_count + 1);
	
}

int get_http_header(http_header_list* list, char* name) {
	int i;
	for (i = 0; i < list->header_count && list->headers != NULL; i++) {
		if (strcasecmp(list->headers[i].name, name) == 0) {
			return i;
		}	
	}
	return -1;
}

char* get_http_header_val(http_header_list* list, char* name) {
	int i;
	i = get_http_header(list, name);
	if (i > -1)
		return list->headers[i].value;
	return "";
	
}


/*
 * See httpsyntax.h
 * */
http_request* calloc_http_request() {
	http_request *ptr;
	ptr = (http_request *) calloc(1, sizeof(http_request));
	if (ptr == NULL)
		return NULL;
	return ptr;
}
/*
 * See httpsyntax.h
 * */
http_response* calloc_http_response() {
	http_response *ptr;
	ptr = (http_response *) calloc(1, sizeof(http_response));
	if (ptr == NULL)
		return NULL;
	return ptr;
}

http_header_list* get_blank_header_list() {
	http_header_list* list;
	list = (http_header_list*) calloc(1, sizeof(http_header_list));
	list->header_count = 0;
	list->headers = NULL;
	if (list == NULL) {
		fprintf(ERROR_STREAM, "ERROR -- Internal Error: Could not Malloc\n");
		exit(1);
	}
	return list;
}

/*
 * See httpsyntax.h
 * */
http_request* get_default_request() {
	http_request* request;
	request = calloc_http_request();
	request->type = GET;
	request->majorversion = 1;
	request->minorversion = 0;
	request->port = 80;
	request->header_list = get_blank_header_list();
	set_request_uri(request, "/", 1);
	return request;
}

http_response* get_default_response() {
	http_response* response;
	char* reason;
	response = calloc_http_response();
	reason = (char*) malloc(3*sizeof(char));
	strcpy(reason, "OK");
	response->majorversion = 1;
	response->minorversion = 0;
	response->status_code = 200;
	response->reason_phrase = reason;
	response->header_list = get_blank_header_list();
	return response;
}

void free_http_header_list(http_header_list* header_list) {
	int i;
	if (header_list == NULL)
		return;
	for (i = 0; i < header_list->header_count; i++) {
		if (header_list->headers[i].name != NULL)
			free(header_list->headers[i].name);
		if (header_list->headers[i].value != NULL)
			free(header_list->headers[i].value);
	}
	free(header_list->headers);
	free(header_list);
}
/*
 * See httpsyntax.h
 * */
void* free_http_request(http_request *req) {
	if (req == NULL)
		return NULL;
	if (req->uri != NULL)
		free(req->uri);
	if (req->host != NULL)
		free(req->host);
	if (req->header_list != NULL)
		free_http_header_list(req->header_list);
	free(req);
	return NULL;
}

void* free_http_response(http_response *res) {
	if (res == NULL)
		return NULL;
	if (res->reason_phrase != NULL)
		free(res->reason_phrase);
	if (res->header_list != NULL)
		free_http_header_list(res->header_list);
	free(res);
	return NULL;
}
