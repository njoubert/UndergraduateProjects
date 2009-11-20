//
//  Person.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Person.h"


@implementation Person

@synthesize uid, name, number, email, guid;

-(void) dealloc {
	[name release];
	[number release];
	[email release];
	[super dealloc];
}

- (NSString*) getFirstName {
	NSArray *firstWords = [[name componentsSeparatedByString:@" "] subarrayWithRange:NSMakeRange(0,1)];
	NSString *result = [firstWords componentsJoinedByString:@" "];
	return result;
}

-(void)convertToData:(RPCPostData*)pData {
	NSString* uidStr = [[NSString alloc] initWithFormat:@"%d", uid];	
	[pData appendValue:uidStr forKey:kRPC_PersonIdKey];
	[pData appendValue:guid forKey:kRPC_PersonGuidKey];
	[pData appendValue:name forKey:kRPC_PersonNameKey];
	[pData appendValue:number forKey:kRPC_PersonNumberKey];
	[pData appendValue:email forKey:kRPC_PersonEmailKey];
	[uidStr release];
}

#pragma mark -
#pragma mark NSCoding
-(void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeInt:self.uid					forKey:kPersonIdKey];
	[coder encodeObject:self.name		forKey:kPersonNameKey];
	[coder encodeObject:self.number		forKey:kPersonNumberKey];
	[coder encodeObject:self.email		forKey:kPersonEmailKey];
}
-(id)initWithCoder:(NSCoder *)coder {
	if (self = [super init]) {
		self.uid = [coder decodeIntForKey:kPersonIdKey];
		self.name = [coder decodeObjectForKey:kPersonNameKey];
		self.number = [coder decodeObjectForKey:kPersonNumberKey];
		self.email = [coder decodeObjectForKey:kPersonEmailKey];
	}
	return self;
}

@end
