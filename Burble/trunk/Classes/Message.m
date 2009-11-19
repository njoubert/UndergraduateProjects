//
//  Message.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Message.h"


@implementation Message

@synthesize sender, message;

-(void) dealloc {
	[sender release];
	[message release];
	[super dealloc];
}

- (NSString*) getSender {;
	return sender;
}


#pragma mark -
#pragma mark NSCoding
-(void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeObject:self.sender		forKey:kMessageSenderKey];
	[coder encodeObject:self.message	forKey:kMessageMessageKey];
}
-(id)initWithCoder:(NSCoder *)coder {
	if (self = [super init]) {
		self.sender = [coder decodeObjectForKey:kMessageSenderKey];
		self.message = [coder decodeObjectForKey:kMessageMessageKey];
	}
	return self;
}

@end
