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

@interface Waypoint : RemoteModel <NSCopying, MKAnnotation> {
	NSString* name;
	NSString* description;
	BOOL syncedWithServer;
@public
	BOOL iAmHere;
	NSDate* createdAt;	
	CLLocationCoordinate2D coordinate;
	CLLocationDistance elevation;
}
@property (nonatomic, readonly) CLLocationCoordinate2D coordinate;

-(id)initWithNameAndDescription:(NSString*)myName description:(NSString*)myDescription;
-(NSString*) title;
-(NSString*) subtitle;
@end
