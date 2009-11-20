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

@synthesize target, selector, conn, receivedData, response;

+ (void)initialize {
	pendingRequestsQueue = nil;
    static BOOL initialized = NO;
    if (!initialized) {
		pendingRequestsQueue = [[NSMutableArray alloc] init];
        initialized = YES;
    }
}

+(RPCURLConnection*)sendAsyncRequest: (NSURLRequest*)request target:(id)t selector:(SEL)s; {
	RPCURLConnection* c = [[RPCURLConnection alloc] initWithTarget:t andSelector:s];
	c.conn = [[NSURLConnection alloc] initWithRequest:request delegate:c startImmediately:YES];
	if (c.conn) {
		c.receivedData = [[NSMutableData alloc] init];
		[pendingRequestsQueue addObject:c];
		return c;
	} else {
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
	[connection release];	
	[target performSelector:selector withObject:response withObject:receivedData];
}
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
	[pendingRequestsQueue removeObject:self];
	[receivedData release];
	[connection release];
	[target performSelector:selector withObject:nil withObject:error];
}




@end
