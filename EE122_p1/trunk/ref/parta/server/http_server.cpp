// Server for Project 1, phase 1 of EE 122 Fall 2007.
//
// Written by Daniel Killebrew.
//
// The server (1) parses HTTP 1.0 request into method, URI and version,
// and (2) echoes back whatever clients send.  It uses select() to work
// with multiple concurrent clients.

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>

#ifdef WIN32
// Windows socket access.

#include <winsock2.h>

// Note, this code does *not* fully build under Windows (due to the
// lack of regex.h), but we provide a subset that does to serve
// as a starting point when working under Windows.

#else

#include <sys/socket.h>
#include <unistd.h>
#include <regex.h>
// The following allows us to use closesocket() in our code (as expected)
// by Windows, but not by Unix.
#define closesocket close
#include <netinet/in.h>

#endif

using namespace std;

bool verbose = false;

// We use the following class to manage a socket along with an
// associated buffer to hold data we've read from the socket.  We need
// to keep the buffer around so we can deal with receiving only part
// of a message on the socket followed by later receiving more of the
// message.
class sockAndBufs
{
public:
	int socket;

	// Standard Template Library string class:
	// the advantage is that it takes care of some of our memory
	// management. Plus it handles \0 in received buffers.
	string completeRequest;
	string incompleteBuffer;

	sockAndBufs(int s)
	{
		socket = s;
	}

	// A copy constructor to correctly create objects when they
	// are copied by the Standard Template Library. Not really
	// needed now that we use STL strings.
	sockAndBufs(const sockAndBufs &copyme)
	{
		socket = copyme.socket;
		completeRequest = copyme.completeRequest;
		incompleteBuffer = copyme.incompleteBuffer;
	}

	// We will store these socket buffers in a vector sorted on their
	// socket descriptors, so we can easily find the highest-numbered
	// descriptor (which select() needs).  The following defines
	// a comparison operator between two objects to determine which
	// is larger.  The sorting algorithm will automatically use this
	// operator to keep the different socket buffers ordered.
	bool operator<(const sockAndBufs &right) const
	{
		return socket < right.socket;
	}

	// Assignment operator:
	// This is required to work with the Standard Template Library,
	// which makes a copy of objects when they are passed into our vector.
	sockAndBufs& operator=(const sockAndBufs &right)
	{
		// Make sure we're not assigning to ourselves.
		if (this != &right) {
			socket = right.socket;
			completeRequest = right.completeRequest;
			incompleteBuffer = right.incompleteBuffer;
		}
		return *this;
	}
};

// Introduce a type name to make our code clearer/less verbose.
typedef vector<sockAndBufs>::iterator vecIterator;

// Attempts to send sendAmt of data from the given buffer.
// Returns 1 on success of sending everything, 0 if an error occurs
// (including the other side closing the socket).
int sendAll(int sock, char *buf, int sendAmt)
{
	int justSent, sentSoFar = 0;
	while (sendAmt > 0)
	{
		justSent = send(sock, buf, sendAmt, 0);
		if (justSent < 0) {
			return 0;
		}
		sentSoFar += justSent;
		sendAmt -= justSent;
	}
	return 1;
}

// Grab everything that's in the socket buffer. Place everything up to and
// including the first CRLF/CRLF in the completed request buffer.
// Leave data in incompleteBuffer until the blank line arrives.
// 
// Returns 0 for socket closed;
//  -1 for socket error;
//  1 for blank line hasn't arrived yet;
//  >1 for the full request has arrived.
int consumeRequestLine(sockAndBufs &client)
{
	const int bufSize = 128;
	char buf[bufSize];

	int amtRecvd = recv(client.socket, buf, bufSize - 1, 0);
	if (amtRecvd == 0) {
		if (verbose)
			printf("socket  %d closed by client\n", client.socket);
		return 0;

	} else if (amtRecvd < 0) {
		perror("ERROR -- Network Error: bad status from recv()");
		return -1;
	}

	// NUL-terminate the buffer.  We left room for doing so by
	// passing bufSize - 1 to recv() rather than bufsize.
	buf[amtRecvd] = '\0';

	// Append all bytes received.
	client.incompleteBuffer.append(buf, amtRecvd);

	// Search incomplete buffer for the blank line.
	string::size_type blankLineIndex =
		client.incompleteBuffer.find("\r\n\r\n");

	if (blankLineIndex == string::npos) {
		// Return 1 to signal blank line hasn't yet arrived.
		return 1;
	}

	// Grab everything up to the blank line, put into complete buffer.
	// The constant 4 here comes from the length of CRLF/CRLF.
	client.completeRequest.assign(client.incompleteBuffer, 0, blankLineIndex + 4);

	// Remove the stuff we just grabbed from incompleteBuffer.
	client.incompleteBuffer.erase(0, blankLineIndex + 4);

	return client.completeRequest.size();
}

// Search for 'GET', then remove 'GET' from the buffer.
int chompMethod(char *buf)
{
	char *pos = strstr(buf, "GET ");
	if (pos != buf) {
		printf("ERROR -- Invalid Method token.\n");
		return 0;
	}

	printf("Method = GET\n");

	// Erase the 'GET' from buffer.
	int newlen = strlen(buf)-3;
	memmove(buf, pos+3, newlen+1);

	return 1;
}

#ifndef WIN32
// Search for the URI, remove it from the buffer.
int chompRequestURI(char *buf)
{
	// +Space Request-URI
	// Space = SP | TAB
	// Request-URI = Absolute-Path
	// Absolute-Path = "/" *FileNameChar
	// FileNameChar = ALPHA | DIGIT | "." | "-" | "_" | "/"

	// Starting at the beginning of the string,
	// search for optional whitespace (space or horizontal tab);
	// followed by a forward slash;
	// followed by 0 or more alphabetic, digits, . - _ /
	const char *pattern = "^[ \t]+/[[:alpha:][:digit:]._/-]*";

	regex_t re;
	// Build in "re" a pattern matcher for the regular expression.
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		printf("regcomp failure\n");
		return 0;
	}

	// Now use the matcher against the buffer.
	regmatch_t matches[1];
	int ret = regexec(&re, buf, 1, matches, 0);
	regfree(&re);

	if (ret == REG_NOMATCH) {
		printf("ERROR -- Invalid Absolute-Path token.\n");
		return 0;
	} else if (ret != 0) {
		printf("regexec failed\n");
		return 0;
	}

	// Figure out which part of buffer matched the regular expression.
	int tlen = matches[0].rm_eo - matches[0].rm_so + 1;

	// Copy that part into a temporary buffer.
	char temp[tlen];
	memcpy(temp, buf+matches[0].rm_so, tlen-1);
	temp[tlen-1] = '\0'; // ensure it's NUL-terminated

	// Skip the leading whitespace.
	const char *tp = temp;
	while ( *tp == ' ' || *tp == '\t' )
		++tp;

	printf("Request-URI = %s\n", tp);

	// Delete the part that matched, as we've now consumed it.
	memmove(buf, buf+matches[0].rm_eo, strlen(buf)+1-matches[0].rm_eo);

	// Return indication of successful match.
	return 1;
}

// Search for HTTP version, remove it from the buffer passed in.
int chompHTTPVersion(char * buf)
{
	// +Space HTTP-Version
	// HTTP-Version = "HTTP" "/" +DIGIT "." +DIGIT

	// Search for start of string, 1 or more whitespace;
	// followed by 'HTTP/' ;
	// followed by 1 or more digits ;
	// followed by a . ;
	// followed by 1 or more digits 
	const char *pattern = "^[ \t]+HTTP/[0-9]+\\.[0-9]+";

	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		printf("regcomp failure\n");
		return 0;
	}

	regmatch_t matches[1];
	int ret = regexec(&re, buf, 1, matches, 0);
	regfree(&re);

	if (ret == REG_NOMATCH) {
		printf("ERROR -- Invalid HTTP-Version token.\n");
		return 0;
	} else if (ret != 0) {
		printf("regexec failed\n");
		return 0;
	}

	// Extract the matching part.
	int tlen = matches[0].rm_eo - matches[0].rm_so + 1;
	char temp[tlen];
	memcpy(temp, buf+matches[0].rm_so, tlen-1);
	temp[tlen-1] = '\0';

	// Skip the leading whitespace.
	const char *tp = temp;
	while ( *tp == ' ' || *tp == '\t' )
		++tp;

	printf("HTTP-Version = %s\n", tp);

	// Remove the match from buffer.
	memmove(buf, buf+matches[0].rm_eo, strlen(buf)+1-matches[0].rm_eo);
	return 1;
}

// Search for and remove the blank line.
int chompCRLFCRLF(char * buf)
{
	// *Space CRLF

	// Starting at beginning of string, search for 0 or more whitespace
	// followed by CRLF/CRLF.
	const char *pattern = "^[ \t]*\r\n\r\n";

	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		printf("regcomp failure\n");
		return 0;
	}

	regmatch_t matches[1];
	int ret = regexec(&re, buf, 1, matches, 0);
	regfree(&re);

	if (ret == REG_NOMATCH) {
		printf("ERROR -- Spurious token before CRLF.\n");
		return 0;
	} else if (ret != 0) {
		printf("regexec failed\n");
		return 0;
	}

	// Delete the matching part from the buffer.
	memmove(buf, buf+matches[0].rm_eo, strlen(buf)+1-matches[0].rm_eo);
	return 1;
}
#endif

// Utility function for printing the vector out.
void printHeap(vector<sockAndBufs> &vec)
{
	int count = 0;
	for (vecIterator i=vec.begin(); i!=vec.end(); i++)
	{
		printf("Item[%d]:\n", count);
		printf("\tsocket=\t%d\n", i->socket);
		printf("\tIncomplete buffer=\t\"%s\"\n",
			i->incompleteBuffer.c_str());
		printf("\tComplete Request=\t\"%s\"\n",
			i->completeRequest.c_str());
		count++;
	}
}

// Creates the server socket and sets it up for listening, returning its
// descriptor.  On error, generates a message and exits.
int createServerSocket(unsigned short port)
{
	if (port <= 0) {
		printf("server requires a port number > 0\n");
		exit(1);
	}

	int listenSock = socket(PF_INET, SOCK_STREAM, 0);
	if (listenSock<0) {
		perror("ERROR -- Network Error: couldn't create socket");
		exit(-1);
	}

	// Reuse port when binding.  This enables us to re-run the server
	// soon after it has previously exited and still use the same port
	// as before.  If we don't specify this, then TCP in the kernel
	// will prevent us from reusing the port for a while, as a way
	// to make sure that no data for old connections that is still
	// somewhere in the network is incorrectly accepted by any new
	// connecdtions.
	char yes = 1;
	if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("ERROR -- Network Error: couldn't set SO_REUSEADDR");
		exit(-1);
	}

	// Set up what port we are binding to.
	struct sockaddr_in listenAddr;
	memset(&listenAddr, 0, sizeof(listenAddr));
	listenAddr.sin_family = AF_INET;  // a single byte, so no ordering issues
	listenAddr.sin_port = htons(port); // network order
	listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenSock, (sockaddr*) &listenAddr, sizeof(listenAddr)) < 0) {
		perror("ERROR -- Network Error: couldn't bind()");
		exit(-1);
	}

	// Now that we're bound the socket to the port we care about,
	// tell the kernel that we're willing to accept new connections
	// on that port.  We give it a "backlog" parameter of 32 - that's
	// how many new connections can be pending on it (i.e., they arrive
	// but we haven't yet accept()'d them) before the kernel will tell
	// new remote hosts attempting to connect "sorry, service not
	// available".
	if (listen(listenSock, 32) < 0) {
		perror("ERROR -- Network Error: couldn't listen()");
		exit(-1);
	}

	return listenSock;
}

void acceptNewConnection(vector<sockAndBufs> &activeSockets, int listenSock)
{
	// The following socket address (which accept() will fill in) isn't
	// strictly necessary.  We can instead pass in a nil pointer and
	// empty size.  However, this isn't so clear from the accept()
	// manual page, so here we go ahead and set up a variable to receive
	// the socket address.
	struct sockaddr_in clientAddr;
	int size = sizeof(clientAddr);
	memset(&clientAddr, 0, size);

	int newSock;
#ifdef WIN32
	// The accept() system call under Windows is slightly different
	// than in Unix, in terms of the data type associated with
	// the size given in the third argument.
	newSock = accept(listenSock, (sockaddr*) &clientAddr, &size);
#else
	newSock = accept(listenSock, (sockaddr*) &clientAddr, (socklen_t*) &size);
#endif
	if (newSock < 0) {
		perror("ERROR -- Network Error: bad status from accept()");
		exit(-1);
	}

	if (verbose)
		printf("Connection created, socket %d\n", newSock);

	// Add the new socket to the vector of active sockets.
	activeSockets.push_back(sockAndBufs(newSock));

	// Ensure that the vector is ordered, so we can always find
	// the largest descriptor value quickly.
	sort(activeSockets.begin(), activeSockets.end());
}

// Process one instance of input from the client associated with the
// given socket.
//
// Returns 1 if the socket should be closed/removed, 0 otherwise.
int processClientInput(sockAndBufs &client)
{
	int bytes = consumeRequestLine(client);

	switch (bytes)
	{
	case -1:
	case 0:
		// Closed, or an error.
		return 1;

	case 1:
		// Blank line not arrived yet, need to just wait for more.
		return 0;

	default:
		// Copy the complete request into our own buffer
		// because we will alter it.
		char *buffer = (char*) malloc(bytes + 1);
		strcpy(buffer, client.completeRequest.c_str());
		buffer[bytes] = '\0';

		// Echo the line.
		printf("%s", client.completeRequest.c_str());
		if (!sendAll(client.socket, buffer, bytes)) {
			printf("ERROR -- unable to echo entire line to client.\n");
		}

		// Parse the request.
		// A failure at any point will print an error message
		// and we will stop working on it.
		int retval = chompMethod(buffer);

		// Windows doesn't have regex.h
#ifndef WIN32
		if (retval > 0) {
			retval = chompRequestURI(buffer);
			if (retval > 0) {
				retval = chompHTTPVersion(buffer);
				if (retval > 0) {
					retval = chompCRLFCRLF(buffer);
				}
			}
		}
#endif
		
		free(buffer);

		// Now that we've parsed their request, we're done with
		// this client's socket.
		return 1;
	}
}

// Process one set of new activity, which means arrivals of connections
// from new client, and/or activity on existing connections.
//
// Returns 0 if the activity indicates the server should exit, non-zero
// othewise.  (Currently this is always non-zero.)
int processNextActivity(vector<sockAndBufs> &activeSockets, int listenSock)
{
	// First we set up the set of descriptors we want to inspect
	// for new activity via select().
	fd_set readset;
	FD_ZERO(&readset);

	// Set readset for every active socket.
	for (unsigned int s = 0; s < activeSockets.size(); s++)
	{
		FD_SET(activeSockets[s].socket, &readset);
	}

	// Make a blocking call to select().  Here we take advantage of
	// the fact that activeSockets is ordered, so the highest-numbered
	// descriptor in it will always be the last.
	int highestFD = activeSockets.rbegin()->socket;
	int numReadySocks = select(highestFD+1, &readset, NULL, NULL, NULL);
	if (numReadySocks == -1) {
		perror("ERROR -- Network Error: bad status from select()");
		exit(-1);
	}

	// Check all active sockets for activity, as long as we know
	// we have more to process.  We do this using an "iterator"
	// that knows how to move through the vector; these are provided
	// by the STL.
	for (vecIterator i = activeSockets.begin();
	     i != activeSockets.end() && numReadySocks > 0; i++)
	{
		if (! FD_ISSET(i->socket, &readset))
			continue;	// this socket isn't active.

		// The socket is exhibiting activity.  Remove it
		// from readset and the count of pending activity,
		// since we're about to process it.
		FD_CLR(i->socket, &readset);
		numReadySocks--;  // since we're about to process it

		if (i->socket == listenSock) {
			acceptNewConnection(activeSockets, listenSock);
			// Note, we've now modified activeSockets,
			// so the vector has been rearranged.
			// Consequently, i is invalidated and we
			// need to be careful to start the iteration
			// over.
			i = activeSockets.begin();

		} else {
			if (processClientInput(*i)) {
				// Done with the socket.
				closesocket(i->socket);
				// Remove this socket from vector,
				// assign iterator to next element
				// after the one that got deleted.
				i = activeSockets.erase(i);
			}
		}
	}

	// Some sanity checking to make sure we consistently iterated
	// through all of the sockets with activity.
	assert(numReadySocks == 0);

	return 1;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("please supply a port to listen on! e.g. http_server 80\n");
		exit(0);
	}

	if (argc > 3) {
		printf("too many arguments - usage is http_server port [-v]\n");
		exit(0);
	}

	if (argc == 3) {
		if (strstr(argv[2], "-v")) {
			verbose = true;
		}
	}

#ifdef WIN32
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	WSAStartup( wVersionRequested, &wsaData );
#endif

	unsigned short listenPort = atoi(argv[1]);
	int listenSock = createServerSocket(listenPort);

	if (verbose)
		printf("Listening on port %d\n", listenPort);

	// We need to track all of our active sockets in order to
	// handle multiple clients concurrently.  We do so using a
	// vector sorted by the sockets' descriptors.  This lets us
	// easily find the highest-numbered descriptor, which we'll
	// need when calling select().
	vector<sockAndBufs> activeSockets;

	activeSockets.push_back(sockAndBufs(listenSock));

	// Enter main server loop: accept new connections and process
	// any activity seen on existing connections.
	while (true)
		if ( ! processNextActivity(activeSockets, listenSock) )
			break;

	closesocket(listenSock);
#ifdef WIN32
	WSACleanup();
#endif

	return 0;
}
