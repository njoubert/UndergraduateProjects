//============================================================================
// Name        : cplusplus.cpp
// Author      : Niels Joubert
// Version     :
// Copyright   : 2007
// Description : Hello World in C, Ansi-style
//============================================================================



#include <iostream>
#include "HTTPRequest.h"
#include "HTTPResponse.h"

using namespace std;

int main(void) {
	HTTPRequest* request = new HTTPRequest();
	HTTPResponse* response = new HTTPResponse();
	cout << "Hello!\n\n";
	delete request;
	delete response;
	return 0;
}
