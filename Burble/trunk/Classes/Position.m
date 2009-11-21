//
//  Position.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Position.h"


@implementation Position

@synthesize uid, lat, lon, vaccuracy, haccuracy, speed, elevation, heading, timestamp, person_id;

-(id)initWithCLLocation:(CLLocation*)loc {
	if (self = [super init]) {
		lat = loc.coordinate.latitude;
		lon = loc.coordinate.longitude;
		vaccuracy = loc.verticalAccuracy;
		haccuracy = loc.horizontalAccuracy;
		speed = loc.speed;
		heading = loc.course;
		elevation = loc.altitude;
		timestamp = loc.timestamp;
	}
	return self;
}

-(void)convertToData:(RPCPostData*)pData {
	NSString* latStr = [[NSString alloc] initWithFormat:@"%f", lat];	
	NSString* lonStr = [[NSString alloc] initWithFormat:@"%f", lon];	
	NSString* vaccuracyStr = [[NSString alloc] initWithFormat:@"%f", vaccuracy];	
	NSString* haccuracyStr = [[NSString alloc] initWithFormat:@"%f", haccuracy];	
	NSString* speedStr = [[NSString alloc] initWithFormat:@"%f", speed];	
	NSString* elevationStr = [[NSString alloc] initWithFormat:@"%f", elevation];	
	NSString* headingStr = [[NSString alloc] initWithFormat:@"%f", heading];	

	[pData appendValue:latStr forKey:kRPC_PositionLatKey];
	[pData appendValue:lonStr forKey:kRPC_PositionLonKey];
	[pData appendValue:vaccuracyStr forKey:kRPC_PositionVaccuracyKey];
	[pData appendValue:haccuracyStr forKey:kRPC_PositionHaccuracyKey];
	[pData appendValue:speedStr forKey:kRPC_PositionSpeedKey];
	[pData appendValue:elevationStr forKey:kRPC_PositionElevationKey];
	[pData appendValue:headingStr forKey:kRPC_PositionHeadingKey];
	
	[pData appendValue:[timestamp description] forKey:kRPC_PositionDateTimeKey];
	
	[latStr release];
	[lonStr release];
	[vaccuracyStr release];
	[haccuracyStr release];
	[speedStr release];
	[elevationStr release];
	[headingStr release];
}

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Position *newPos;
	newPos = [[[self class] allocWithZone:zone] init];
	newPos.lat = lat;
	newPos.lon = lon;
	newPos.vaccuracy = vaccuracy;
	newPos.haccuracy = haccuracy;
	newPos.speed = speed;
	newPos.elevation = elevation;
	newPos.heading = heading;
	newPos.timestamp = [timestamp copyWithZone:zone];
	newPos.person_id = person_id;
	newPos.uid = uid;
	return newPos;
}

@end
