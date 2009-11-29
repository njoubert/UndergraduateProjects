//
//  Group.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Group.h"


@implementation Group

@synthesize name, description, group_id;

-(void) convertToData:(RPCPostData*)pData {
	if (group_id > 0) {
		NSString* idStr = [[NSString alloc] initWithFormat:@"%d", group_id];	
		[pData appendValue:idStr forKey:kRPC_GroupIdKey];
		[idStr release];
	}
	[pData appendValue:name forKey:kRPC_GroupNameKey];
	[pData appendValue:description forKey:kRPC_GroupDescriptionKey];
}

-(BOOL)isEqual:(Group*)other {
	if (group_id > 0)
		return (group_id == other.group_id);
	else
		return ([name isEqualToString:other.name] && [description isEqualToString:other.description]);
}
#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Group* newGroup;
	newGroup = [[[self class] allocWithZone:zone] init];
	newGroup.group_id = self.group_id;
	newGroup.name = newGroup.description = nil;
	if (nil != name)
		newGroup.name = [[NSString alloc] initWithString:name];
	if (nil != description)
		newGroup.description = [[NSString alloc] initWithString:description];
	return newGroup;
}


#pragma mark -
#pragma mark NSCoding
-(void)encodeWithCoder:(NSCoder *)coder {
	[coder encodeInt:self.group_id					forKey:kRPC_GroupIdKey];
	[coder encodeObject:self.name					forKey:kRPC_GroupNameKey];
	[coder encodeObject:self.description			forKey:kRPC_GroupDescriptionKey];
}
-(id)initWithCoder:(NSCoder *)coder {
	if (self = [super init]) {
		self.group_id = [coder decodeIntForKey:kRPC_GroupIdKey];
		self.name = [coder decodeObjectForKey:kRPC_GroupNameKey];
		self.description = [coder decodeObjectForKey:kRPC_GroupDescriptionKey];
	}
	return self;
}
@end
