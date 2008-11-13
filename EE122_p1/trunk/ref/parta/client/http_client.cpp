// Client for Project 1, phase 1 of EE 122 Fall 2007.
//
// Written by Daniel Killebrew.
//
// The client (1) reads a URL from standard in or stops at EOF
// (2) connects to the server and sends a GET request
// (3) prints to standard out the server's response
// (4) waits for server to close connection
// (5) goes back to step (1)

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <regex.h>
#include <netinet/in.h>


using namespace std;


const char * defaultPort = "80";
const char * defaultPath = "/";
bool verbose = false;


// Attempts to send sendAmt of data from the given buffer.
// Returns how many bytes were actually sent, which could be fewer if
// an error occurs (including the other side closing the socket).
int sendAll(int sock, const char *buf, int sendAmt)
{
	int justSent, sentSoFar = 0;
	while (sendAmt > 0)
	{
		justSent = send(sock, buf, sendAmt, 0);
		if (justSent < 0) {
			return sentSoFar;
		}
		sentSoFar += justSent;
		sendAmt -= justSent;
	}
	return sentSoFar;
}

// Read from standard in until we get a newline;
// that will mean we've gotten an 'http://' from the user.
int grabFromStdin(string &partialLine, string &fullURL)
{
	const int size = 128;
	char buf[size];
	string::size_type newlineIndex;

	// Grab data from standard in until we encounter \n.
	do
	{
		char *retval = fgets(buf, size, stdin);
		if (retval == NULL) {
			// An error or End of File.
			if (ferror(stdin))
				printf("Error reading from standard in. Closing.\n");
			// Indicate that we are done reading from stdin.
			return 0;
		}
		partialLine.append(retval);
		newlineIndex = partialLine.find("\n");
	}
	while (newlineIndex == string::npos);

	// Copy everything up to, not including, the newline.
	fullURL.assign(partialLine, 0, newlineIndex);

	// Delete what we copied, along with the newline.  But we
	// need to keep the remainder, since fgets() may have started
	// reading into the next line.
	partialLine.erase(0, newlineIndex+1);

	return 1;
}

// Split the URL that the user types in into its component parts.
// Return them through the references that are passed in.
int parseURL(string &fullURL, string &hostmatch, string &portmatch, string &pathmatch)
{
	// HTTP-URL = "http://" Host ?Port ?Path
	// Host = Hostname | Hostnumber
	// Hostname = ALPHA *AlphaNum ?("." Hostname)
	// Hostnumber = +DIGIT "." +DIGIT "." +DIGIT "." +DIGIT
	// Port = ":" +DIGIT
	// Path = "/" *FileNameChar
	// FileNameChar = ALPHA | DIGIT | "." | "-" | "_" | "/"
	// AlphaNum = ALPHA | DIGIT

	// Don't forget that '\' is special to C, so we must escape it
	// with another '\'.
	//
	// We match the host, port and path using \(\) so we can extract
	// them as RE subexpressions:
	//
	//  \(hostname|hostnumber\)\(port\)?\(path\)?$";
	//
	// This makes for a complicated set of regular expressions.
	// You can read up on them if you are curious:
	//   http://www.opengroup.org/pubs/online/7908799/xbd/re.html#tag_007_003
	//   http://www.opengroup.org/pubs/online/7908799/xsh/regexec.html

	string start = "^[hH][tT][tT][pP]://";
	string hostname = "[[:alpha:]][[:alpha:][:digit:]]*(\\.[[:alpha:]][[:alpha:][:digit:]]*)*";
	string hostnumber = "[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+";
	string port = ":([0-9]+)";

	// Note that '-' must come last in the character class here,
	// because otherwise it has a different meaning (indicating
	// a *range* of characters to match).
	string path = "/[[:alpha:][:digit:]._/-]*";

	// Match the hostname/hostnumber:
	string pattern;
	pattern.append(start).append("(");
	pattern.append(hostname).append("|");
	pattern.append(hostnumber).append(")");
	
	string remaining = fullURL;

	regex_t re;
	regmatch_t matches[4];
	int ret;

	if (regcomp(&re, pattern.c_str(), REG_EXTENDED) != 0) {
		printf("couldn't build regular expression\n");
		return(0);      /* report error */
	}

	ret = regexec(&re, remaining.c_str(), 4, matches, 0);

	// All done with the pattern-matcher, delete it.
	regfree(&re);

	if (ret == REG_NOMATCH) {
		printf("ERROR -- Invalid URL: %s\n", fullURL.c_str());
		return 0;
	}
	else if (ret != 0) {
		printf("ERROR -- regexec failed\n");
		return 0;
	}
	// Extract the matching parts of host.
	hostmatch = remaining.substr(matches[1].rm_so,
					matches[1].rm_eo - matches[1].rm_so);

	// Now make a string from the remaining URL.
	remaining = remaining.substr(matches[0].rm_eo, fullURL.length());

	if (verbose)
		printf("After matching host, remaining URL -->%s<--\n",
			remaining.c_str());
	
	// Match port, if it's there.
	pattern = "^";
	pattern.append(port);
	portmatch = defaultPort;
	
	if (regcomp(&re, pattern.c_str(), REG_EXTENDED) != 0) {
		printf("regcomp failure\n");
		return(0);      /* report error */
	}

	ret = regexec(&re, remaining.c_str(), 4, matches, 0);
	regfree(&re); // done with the pattern-matcher

	if (ret == 0) {
		// We matched.
		portmatch = remaining.substr(matches[1].rm_so,
				matches[1].rm_eo - matches[1].rm_so);

		// Now make a string from the remaining URL.
		remaining = remaining.substr(matches[0].rm_eo, fullURL.length());
	}
		
	// Match the path, if it's there, and match the end of the URL.
	if (verbose)
		printf("After matching port, remaining URL -->%s<--\n",
			remaining.c_str());

	pattern = "^(";
	pattern.append(path).append(")?$"); // path component is optional

	pathmatch = defaultPath;
	if (regcomp(&re, pattern.c_str(), REG_EXTENDED) != 0) {
		printf("regcomp failure\n");
		return(0);      /* report error */
	}
	ret = regexec(&re, remaining.c_str(), 4, matches, 0);
	regfree(&re); // done with the pattern-matcher

	if (ret == 0) {
		// We matched.
		if (matches[1].rm_so != -1) {
			// Check whether there actually was a path.
			pathmatch = remaining.substr(matches[1].rm_so,
					matches[1].rm_eo - matches[1].rm_so);
		}
	}
	else if (ret == REG_NOMATCH) {
		printf("ERROR -- Invalid URL: %s\n", fullURL.c_str());
		return 0;
	}
	
	return 1;
}

// Connect to the server with this hostname and port.
// The socket will be stored into the socket descriptor passed in ("sock").
bool connectToServer(string host, int port, int &sock)
{
	struct hostent *hent;
	struct sockaddr_in saddr;
	
	if ((hent = gethostbyname(host.c_str())) == NULL) {
		printf("ERROR -- Network Error: gethostbyname error\n");
		return false;
 	}
 	
 	sock = socket(PF_INET, SOCK_STREAM, 0);
 	
 	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr = *((struct in_addr *) hent->h_addr);
 	
 	int ret = connect(sock, (sockaddr*) &saddr, sizeof(saddr));
 	if(ret == 0) {
 		return true;
 	} else {
 		perror("ERROR -- Network Error"); 		
 		return false;
 	}
}

// Create the GET request and send it on "sock".
bool sendRequest(string path, int sock)
{
	string GETrequest = "GET ";

	GETrequest.append(path);
	GETrequest.append(" HTTP/1.0\r\n\r\n");
	
	if (verbose)
		printf("Sending GET request -->%s<--\n", GETrequest.c_str());

	unsigned int bytes =
		sendAll(sock, GETrequest.c_str(), GETrequest.length());

	return bytes == GETrequest.length();
}

// Print out what the server sends until it closes the connection.
void handleResponse(int sock)
{
	const int size = 128;
	char buf[size];
	int bytes;

	do
	{
		// Don't let recv() overwrite the last byte, which
		// we will use for a NUL terminator.
		bytes = recv(sock, buf, size - 1, 0);

		if (bytes < 0)
			break; // Error condition.

		buf[bytes] = '\0'; // NUL-terminate what we've read.

		// Note, by printing out the bytes as they come in,
		// we avoid having to buffer up a potentially huge
		// server response into a single string.  This lets
		// us keep reusing the same buffer each pass through
		// the loop.
		printf("%s", buf);
	} while (bytes > 0);

	// Zero means a cleanly closed socket, so just return.  A negative
	// value indicates an error.
	if (bytes < 0) {
		printf("ERROR -- Network Error:  recv failed\n");
	}
}

// Keep grabbing lines from standard in, parsing each line,
// connecting to the server, etc.
int main(int argc, char **argv)
{
	string partialLine;
	string fullURL, host, port, path;
	
	if (argc > 2) {
		printf("ERROR -- usage is a single optional -v argument\n");
		return -1;

	} else if (argc == 2) {
		if (strstr(argv[1], "-v")) {
			verbose = true;
		}
	}
	
	// Loop while stdin is not EOF or some error.
	while (grabFromStdin(partialLine, fullURL))
	{
		if (!parseURL(fullURL, host, port, path))
			// Bad URL.  Skip it and get another.
			continue;

		if (verbose)
			printf("Host -->%s<--\nPort -->%s<--\nPath -->%s<--\n",
				host.c_str(), port.c_str(), path.c_str());

		int sock;
		if (!connectToServer(host, atoi(port.c_str()), sock)) {
			close(sock);
			continue;
		}

		if (!sendRequest(path, sock)) {
			close(sock);
			printf("ERROR -- Network Error:  could not send GET request\n");
			continue;
		}

		handleResponse(sock);
		close(sock);
	}

	return 0;
}
