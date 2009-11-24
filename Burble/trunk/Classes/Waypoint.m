//
//  Waypoint.m
//  Test1
//
//  Created by Niels Joubert on 11/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Waypoint.h"


@implementation Waypoint
@synthesize coordinate, uid, group_id, person_id, name, description, elevation;

-(id)initWithNameAndDescription:(NSString*)myName description:(NSString*)myDescription; {
	if (self = [super init]) {
		name = [[NSString alloc] initWithString:myName];
		[name retain];
		description = [[NSString alloc] initWithString:myDescription];
		[description retain];
		uid = -1;
	}
	return self;
}

-(NSString*) title {
	return name;
}
-(NSString*) subtitle {
	if (uid <= 0)
		return [NSString stringWithFormat:@"%@ (local only)", description];
	else 
		return [NSString stringWithFormat:@"%@ (synced)", description];
}
-(void)setLatitude:(double)lat {
	coordinate.latitude = lat;
}
-(void)setLongitude:(double)lon {
	coordinate.longitude = lon;
}


-(void)convertToData:(RPCPostData*)pData {
	[pData appendValue:name forKey:kRPC_WaypointNameKey];
	[pData appendValue:description forKey:kRPC_WaypointDescKey];
	NSString* latStr = [[NSString alloc] initWithFormat:@"%f", coordinate.latitude];
	[pData appendValue:latStr forKey:kRPC_WaypointLatKey];
	NSString* lonStr = [[NSString alloc] initWithFormat:@"%f", coordinate.longitude];
	[pData appendValue:lonStr forKey:kRPC_WaypointLonKey];
	[latStr release];
	[lonStr release];
}

-(BOOL)isEqual:(Waypoint*)otherW {
	//This is specifically set up so that if we do not have a uid for one of our waypoints
	//that we sent, and we receive it back, we then get a uid for it and associate it
	//now this should not happen cause you should associate a uid when the server adds the waypoints
	//but i already wrote this before i realized that so for now we will keep this in here.
	if (uid > 0)
		return (uid == otherW.uid);
	else
		return ([name isEqualToString:otherW.name]) && ([description isEqualToString:otherW.description]) && (person_id == otherW.person_id);

}

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Waypoint *newW;
	newW = [[[self class] allocWithZone:zone] init];
	newW.name =  [name copyWithZone:zone];
	newW.description = [description copyWithZone:zone];
	newW.uid = uid;
	newW.group_id = group_id;
	newW.person_id = person_id;
	newW->iAmHere = iAmHere;
	newW->createdAt = [createdAt copyWithZone:zone];
	[newW setLatitude:self.coordinate.latitude];
	[newW setLongitude:self.coordinate.longitude];
	newW.elevation = elevation;
	return newW;
}

#pragma mark -
#pragma mark NSCoding
-(void)encodeWithCoder:(NSCoder *)coder {
	
}
-(id)initWithCoder:(NSCoder *)coder {
	if (self = [super init]) {
		
	}
	return self;
}

@end
