//
//  Position.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>
#import "RemoteModel.h"

#define kRPC_PositionLatKey					@"position[latitude]"
#define kRPC_PositionLonKey					@"position[longitude]"
#define kRPC_PositionVaccuracyKey			@"position[vaccuracy]"
#define kRPC_PositionHaccuracyKey			@"position[haccuracy]"
#define kRPC_PositionSpeedKey				@"position[speed]"
#define kRPC_PositionElevationKey			@"position[elevation]"
#define kRPC_PositionHeadingKey				@"position[heading]"
#define kRPC_PositionDateTimeKey			@"position[timestamp]"

@interface Position : RemoteModel <NSCopying, NSCoding> {
	int uid;
	int person_id;
	double lat;
	double lon;
	double vaccuracy;
	double haccuracy;
	double speed;
	double elevation;
	double heading;
	NSDate* timestamp;
}
@property int uid;
@property int person_id;
@property double lat;
@property double lon;
@property double vaccuracy;
@property double haccuracy;
@property double speed;
@property double elevation;
@property double heading;
@property (nonatomic, copy) NSDate* timestamp;

-(id)initWithCLLocation:(CLLocation*)loc;

-(CLLocation*)getLocation;

@end
