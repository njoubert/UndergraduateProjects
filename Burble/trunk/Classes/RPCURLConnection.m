//
//  RPCConnection.m
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RPCURLConnection.h"


@implementation RPCURLConnection

static NSMutableArray *pendingRequestsQueue;

@synthesize target, selector, conn, receivedData, response, _userObj;

+ (void)initialize {
	pendingRequestsQueue = nil;
    static BOOL initialized = NO;
    if (!initialized) {
		pendingRequestsQueue = [[NSMutableArray alloc] init];
        initialized = YES;
    }
}

+(RPCURLConnection*)sendAsyncRequest: (NSURLRequest*)request target:(id)t selector:(SEL)s; {
	return [RPCURLConnection sendAsyncRequest:request target:t selector:s withUserObject:nil];
}

+(RPCURLConnection*)sendAsyncRequest: (NSURLRequest*)request target:(id)t selector:(SEL)s withUserObject:(id)obj {
	RPCURLConnection* c = [[RPCURLConnection alloc] initWithTarget:t andSelector:s];
	c.conn = [[NSURLConnection alloc] initWithRequest:request delegate:c startImmediately:YES];
	if (c.conn) {
		[obj retain];
		c._userObj = obj;
		c.receivedData = [[NSMutableData alloc] init];
		[pendingRequestsQueue addObject:c];
		if ([pendingRequestsQueue count] > 0) {
			UIApplication* app = [UIApplication sharedApplication]; 
			app.networkActivityIndicatorVisible = YES; // to stop it, set this to NO 
		}
		return c;
	} else {
		[c.conn release];
		return nil;
	}
}

-(id)initWithTarget:(id)t andSelector:(SEL)s {
	if (self = [super init]) {
		target = t;
		selector = s;
	}
	return self;
}

/*
 ================================================================================
 DELEGATE METHODS FOR NSURLCONNECTION
 ================================================================================
 
 callbackFnct:(RPCURLResponse*)rpcResponse withObject:(id)userObj
	it's your job to release the rpcResponse
 
 */
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)resp {
	[response release];
	[resp retain];
	[self setResponse:resp];
    [receivedData setLength:0];
}
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    [receivedData appendData:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
	[pendingRequestsQueue removeObject:self];
	if ([pendingRequestsQueue count] == 0) {
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO]; // to stop it, set this to NO 
	}
	[connection release];
	RPCURLResponse* rpcResponse = [[RPCURLResponse alloc] init];
	[rpcResponse setResponse:response];
	[rpcResponse setData:receivedData];
	[target performSelector:selector withObject:rpcResponse withObject:_userObj];
}
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
	[pendingRequestsQueue removeObject:self];
	if ([pendingRequestsQueue count] == 0) {
		[[UIApplication sharedApplication] setNetworkActivityIndicatorVisible:NO]; // to stop it, set this to NO 
	}	
	[receivedData release];
	[connection release];
	RPCURLResponse* rpcResponse = [[RPCURLResponse alloc] init];
	[rpcResponse setError:error];
	[target performSelector:selector withObject:rpcResponse withObject:_userObj];
}




@end
