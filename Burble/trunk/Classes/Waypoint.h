//
//  Waypoint.h
//  Test1
//
//  Created by Niels Joubert on 11/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>
#import "RemoteModel.h"

#define kRPC_WaypointNameKey		@"waypoint[name]"
#define kRPC_WaypointDescKey		@"waypoint[description]"
#define kRPC_WaypointLatKey			@"waypoint[latitude]"
#define kRPC_WaypointLonKey			@"waypoint[longitude]"
#define kRPC_WaypointEleKey			@"waypoint[elevation]"
#define kRPC_WaypointCreatedAtKey	@"waypoint[created_at]"

#define kWaypointNameK		@"WaypointName"
#define kWaypointDescK		@"WaypointDesc"
#define kWaypointUIDK		@"WaypointUID"
#define kWaypointGIDK		@"WaypoingGID"
#define kWaypointPIDK		@"WaypointKID"
#define kWaypointIAmHereK	@"WaypoingIAmHereK"
#define kWaypointcreatedAtK	@"WaypoingCreatedAtK"
#define kWaypointLatK		@"WaypoingLatK"
#define kWaypointLonK		@"WaypoingLonK"
#define kWaypointEleK		@"WaypointEleK"

@interface Waypoint : RemoteModel <NSCopying, MKAnnotation, NSCoding> {
	NSString* name;
	NSString* description;
	int uid;
	int group_id;
	int person_id;
@public
	BOOL iAmHere;
	NSDate* createdAt;	
	CLLocationCoordinate2D coordinate;
	CLLocationDistance elevation;
}
@property (nonatomic, copy) NSString* name;
@property (nonatomic, copy) NSString* description;
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;
@property CLLocationDistance elevation;
@property int uid;
@property int group_id;
@property int person_id;

-(BOOL)isSynced;
-(BOOL)isMine;
-(id)initWithNameAndDescription:(NSString*)myName description:(NSString*)myDescription;
-(NSString*) title;
-(NSString*) subtitle;
-(void)setLatitude:(double)lat;
-(void)setLongitude:(double)lon;

@end
