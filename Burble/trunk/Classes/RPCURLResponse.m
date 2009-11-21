//
//  RPCURLResponse.m
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RPCURLResponse.h"


@implementation RPCURLResponse

@synthesize response, data, error;

-(void)dealloc {
	[response release];
	[data release];
	[error release];
}
@end
