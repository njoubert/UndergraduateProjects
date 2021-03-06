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
		_loc = nil;
		lat = loc.coordinate.latitude;
		lon = loc.coordinate.longitude;
		vaccuracy = loc.verticalAccuracy;
		haccuracy = loc.horizontalAccuracy;
		speed = loc.speed;
		heading = loc.course;
		elevation = loc.altitude;
		timestamp = [loc.timestamp retain];
	}
	return self;
}

-(CLLocation*)getLocation {
	if (_loc == nil) {
		CLLocationCoordinate2D coord;
		coord.latitude = lat;
		coord.longitude = lon;
		_loc = [[CLLocation alloc] initWithCoordinate:coord altitude:elevation horizontalAccuracy:haccuracy verticalAccuracy:vaccuracy timestamp:timestamp];
	}
	return _loc;
}

-(void)convertToData:(RPCPostData*)pData {
	NSString* latStr = [NSString stringWithFormat:@"%f", lat];	
	NSString* lonStr = [NSString stringWithFormat:@"%f", lon];	
	NSString* vaccuracyStr = [NSString stringWithFormat:@"%f", vaccuracy];	
	NSString* haccuracyStr = [NSString stringWithFormat:@"%f", haccuracy];	
	NSString* speedStr = [NSString stringWithFormat:@"%f", speed];	
	NSString* elevationStr = [NSString stringWithFormat:@"%f", elevation];	
	NSString* headingStr = [NSString stringWithFormat:@"%f", heading];	

	[pData appendValue:latStr forKey:kRPC_PositionLatKey];
	[pData appendValue:lonStr forKey:kRPC_PositionLonKey];
	[pData appendValue:vaccuracyStr forKey:kRPC_PositionVaccuracyKey];
	[pData appendValue:haccuracyStr forKey:kRPC_PositionHaccuracyKey];
	[pData appendValue:speedStr forKey:kRPC_PositionSpeedKey];
	[pData appendValue:elevationStr forKey:kRPC_PositionElevationKey];
	[pData appendValue:headingStr forKey:kRPC_PositionHeadingKey];
	[pData appendValue:[timestamp description] forKey:kRPC_PositionDateTimeKey];

}

-(CLLocationCoordinate2D) coordinate {
	CLLocationCoordinate2D coord;
	coord.latitude = lat;
	coord.longitude = lon;
	return coord;
}

-(void)dealloc {
	[_loc release];
	[timestamp release];
	[super dealloc];
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
