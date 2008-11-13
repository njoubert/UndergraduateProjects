Niels Joubert
ee122-bw
18275309

			README FOR WEB SERVER AND CLIENT
			
CLIENT
------

usage:
	http_client [-p]
		-p attempts presistent connections.

The client I built is a simple HTTP browser
that issues GET requests for the URLs you hand
to it line by line on STDIN. Upon loading the client, you will 
be prompted to enter an URL. This will be parsed 
into are HTTP Request if possible, and issued.

input:
	http://<host>[:<port>][<path>]\n

To end the program's operation you can either send
it an interrupt signal using Ctrl-C or send it
the Ctrl-D signal.

There are two categories of error messages
you will receive if the program cannot 
succeed in its function If the program fails 
due to a network error, you will see the following
message on STDERR:

	ERROR -- Network Error: <description>

If the program fails due to an input error, you will
see the following message on STDERR:
	
	ERROR -- Invalid URL: <url>
	
The client will terminate if an unrecoverable error
occured, such as memory allocation failing.

The client supports chunked data returned from the server, and 
also supports presistent connections.
Any URL entered will be downloaded to a file in the local directory, 
with either the name of the file requested, or one of the following
predefined names for these situations:

  REQUEST       FILENAME
  /				dir
  foo/			foo
  foo/.			dot
  foo/..		dotdot
 
SERVER
------

usage:
	http_server [-c chunksize]
		The default port if none is supplied is 7788
		Port should never be bigger than 65536
	
The server I built is a simple extention to the HTTP/1.0 format
It will receive GET requests, and print the request body to the
screen. The request will then be parsed and honored if possible,
returning a HTTP Response with response code and reason phrase.

It supports presistant connections and chunking of data.
Errors that cannot be communicated to the client will
be printed out to STDERR or STDOUT.

