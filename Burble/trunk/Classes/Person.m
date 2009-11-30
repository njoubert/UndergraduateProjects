//
//  Person.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Person.h"
#import "Util.h"

@implementation Person

@synthesize uid, name, number, email, guid, position, group;

-(id)init {
	if (self = [super init]) {
		position = nil;
		group = nil;
		guid = [[NSString alloc] init];
		name = [[NSString alloc] init];
		number = [[NSString alloc] init];
		email = [[NSString alloc] init];
	}
	return self;
}

-(void) dealloc {
	[group release];
	[position release];
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
- (void)updateWith:(Person*)p {
	uid = p.uid;
	[guid release];
	[name release];
	[number release];
	[email release];
	guid = [[NSString alloc] initWithString:p.guid];
	name = [[NSString alloc] initWithString:p.name];
	number = [[NSString alloc] initWithString:p.number];
	email = [[NSString alloc] initWithString:p.email];
	[group release];
	[position release];
	group = [p.group retain];
	position = [p.position retain];
}

-(CLLocation*)getLocation {
	if (position != nil)
		return [position getLocation];
	return nil;
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
#pragma mark MKAnnotation

- (CLLocationCoordinate2D) coordinate {
	if (position != nil)
		return position.coordinate;
	CLLocationCoordinate2D blank;
	return blank;
}

- (NSString*) title {
	return name;
}
- (NSString*) subtitle {
	if (position != nil)
		return [NSString stringWithFormat:@"%@ old", [Util prettyTimeAgo:position.timestamp]];
	return [NSString stringWithString:@"Unknown position"];
}
-(BOOL)isEqual:(Person*)other {
	if (uid > 0) {
		return (uid == other.uid);
	} else {
		return ([name isEqualToString:other.name] && [guid isEqualToString:other.guid]);
	}
	
}

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Person* newPerson;
	newPerson = [[[self class] allocWithZone:zone] init];
	newPerson.uid = self.uid;
	newPerson.guid = newPerson.name = newPerson.number = newPerson.email = nil;
	if (nil != guid)
		newPerson.guid = [[NSString alloc] initWithString:guid];
	if (nil != name)
		newPerson.name = [[NSString alloc] initWithString:name];
	if (nil != number)
		newPerson.number = [[NSString alloc] initWithString:number];
	if (nil != email)
		newPerson.email = [[NSString alloc] initWithString:email];
	return newPerson;
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
