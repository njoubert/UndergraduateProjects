//
//  Waypoint.m
//  Test1
//
//  Created by Niels Joubert on 11/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Waypoint.h"


@implementation Waypoint
@synthesize coordinate, uid;

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

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	
}



@end
