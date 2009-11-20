//
//  RPCPostRequest.m
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RPCPostRequest.h"


@implementation RPCPostRequest

-(id)init {
	if (self = [super init]) {
		[self setHTTPMethod:@"POST"];
		[self setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
		
	}
	return self;
}
-(id)initWithURL:(NSURL *)url cachePolicy:(NSURLRequestCachePolicy)cachePolicy timeoutInterval:(NSTimeInterval)timeoutInterval {
	if (self = [super initWithURL:url cachePolicy:cachePolicy timeoutInterval:timeoutInterval]) {
		[self setHTTPMethod:@"POST"];
		[self setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"content-type"];
	}
	return self;
}
-(void)setHTTPBodyPostData:(RPCPostData*)pD {
	[super setHTTPBody:[pD getData]];
}


@end
