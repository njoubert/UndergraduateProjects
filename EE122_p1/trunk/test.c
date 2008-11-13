#include "httpsyntax.h"
#include "common.h"

int test_url_input() {
	http_request* requestptr;
	int i, errornr = 13;
	printf("#########################\n# TEST URL INPUT \n#########################\n");
	char input[13][255] = {
			("HtTP//"),
			{"http://3www.ww3.com/"},
			{"http://www::80/"},
			{"http://kewcogel/   yo.html"},
			{"http://thefacebook_sucks.com/"},
			{"http://thefacebook.3facebook"},
			{"http://www.  nooo.com/"},
			{"http://***.com/"},
			
			{"http://www.gmail.com"},
			{"hTTp://mail.google.com/mail/"},
			{"hTtP://berkeley.edu:1935/stream.flv"},
			{"HtTP://yo"},
			{"http://ilunix3.eecs.berkeley.edu:7788/index.html"}
	};
	
	char output[13][255] = {
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			{"ERROR -- Invalid URL"},
			
			{"VALID"},
			{"VALID"},
			{"VALID"},
			{"VALID"},
			{"VALID"},
	};
	
	for (i = 0; i < errornr; i++) {
		printf("==== TEST %d ====\n", i);
		printf("INPUT: %s\n", input[i]);
		requestptr = urlstring_to_request(input[i]);
		
		printf("EXPECTED: %s\nRECIEVED: ", output[i]);
		if (requestptr == NULL)
			printf("ERROR -- Invalid URL: %s\n", input[i]);
		else {
			print_http_request(requestptr, 1);
			free_http_request(requestptr);
		}
	}
	
	return 0;
}

int test_http_request() {
	http_request* requestptr;
	int i, errornr = 13;
	printf("#########################\n# TEST HTTP REQUEST \n#########################\n");
	char input[13][255] = {
			{"sfddssdf\r\n"},
			{"GE"},
			{"\r\n\r\n"},
			{"GET/RESEARCH"},
			{"GET Research"},
			{"GET yoyo /Research/Index.html HTTP/1.0\r\n"},
			{"GET /Research/Index.html \r\n"},
			{"GET /Research/Index.html"},
			{"GET /Research/Index.html HTT/1.0\r\n"},
			{"GET /Research/"},
			{"GET /Research/Index.html yoyo HTTP/1.0 \r\n"},
			{"GET / HTTP/1.0 \r\n\r\n"},
			{"GET /Research/Areas/index.html HTTP/1.0 \r\n\r\n"}
	};
	
	char output[13][255] = {
			{"ERROR -- Invalid Method token."},
			{"ERROR -- Invalid Method token."},
			{"ERROR -- Invalid Method token."},
			{"ERROR -- Invalid Absolute-Path token."},
			{"ERROR -- Invalid Absolute-Path token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"ERROR -- Invalid HTTP-Version token."},
			{"METHOD = GET\nRequest-URI = /\nHTTP-Version = HTTP/1.0"},
			{"METHOD = GET\nRequest-URI = /Research/Areas/index.html\nHTTP-Version = HTTP/1.0"}
	};
	
	for (i = 0; i < errornr; i++) {
		requestptr = parse_formatted_request(input[i]);
		printf("==== TEST %d ====\n", i);
		printf("Expected: %s\nRecieved: ", output[i]);
		print_http_request(requestptr, 1);
		free_http_request(requestptr);
	}
	
	return 0;
}

int test_gethostbyname() {
	struct hostent *hp;
	
	hp = Gethostbyname("http://www.google.com/");
	
	
	return 0;
}

int test_http_request_headers() {
	http_request* request;
	request = get_default_request();
	set_http_header(request->header_list, "Connection", "Keep-Alive");
	set_http_header(request->header_list, "Content-Type", "text/http");
	set_http_header(request->header_list, "Date", "Wed, 31 Oct 2007 03:16:57 GMT");
	
	printf("==== TEST ====\nSetting values\n\n");
	print_http_request(request, 1);
	
	set_http_header(request->header_list, "Date", "TODAY");
	printf("==== TEST ====\nResetting values\n\n");
	print_http_request(request, 1);
	
	
	free_http_request(request);
	return 0;
}

void test_inputting_requests() {
	http_request* request;
	char* head;
	head = (char*) malloc(4096*sizeof(char));
	strcpy(head, "GET / HTTP/1.1\r\nHost: 192.168.0.10:7788\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1.8) Gecko/20071008 Firefox/2.0.0.8\r\nAccept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Language: en-us,en;q=0.5\r\nAccept-Encoding: gzip,deflate\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nKeep-Alive: 300\r\nConnection: keep-alive\r\n");
	
	request = parse_formatted_request(head);
	
	print_http_request(request, 0);
	
	printf("GETTING KEEP ALIVE HEADER:\n");
	printf(get_http_header_val(request->header_list, "connection"));
	
	free_http_request(request);
	free(head);
}


int main(void) {
	//test_http_request();
	//test_url_input();
	//test_gethostbyname();
	//test_http_request_headers();
	test_inputting_requests();
	return 0;
}
