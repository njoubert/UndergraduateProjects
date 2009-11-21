//
//  RPCConnection.h
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RPCPostRequest.h"
#import "RPCURLResponse.h"

/*
 * Use this to send asyncronous requests but you dont want to deal with receiving partial data
 * and getting partial reponses and the like. It works much like a syncronous request, to make things easier for you.
 * Inspired by NSTimer class
 *
 */
@interface RPCURLConnection : NSObject {
	NSURLConnection* conn;
	NSMutableData* receivedData;
	NSURLResponse* response;
	id target;
	SEL selector;
	id _userObj;
}
@property (assign, nonatomic) NSURLConnection* conn;
@property (assign, nonatomic) id target;
@property (assign, nonatomic) SEL selector;
@property (assign, nonatomic) NSMutableData* receivedData;
@property (assign, nonatomic) NSURLResponse* response;
@property (retain, nonatomic) id _userObj;

// This is how you start a request. 
// You will get a callback on selector.
// SELECTOR: must be function:RPCURLResponse:userObj
+(RPCURLConnection*)sendAsyncRequest: (NSURLRequest*)request target:(id)t selector:(SEL)s;

+(RPCURLConnection*)sendAsyncRequest: (NSURLRequest*)request target:(id)t selector:(SEL)s withUserObject:(id)obj;

-(id)initWithTarget:(id)t andSelector:(SEL)s;

@end
