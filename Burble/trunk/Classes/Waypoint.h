//
//  Waypoint.h
//  Test1
//
//  Created by Niels Joubert on 11/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>

@interface Waypoint : NSObject {
	NSString* name;
	NSString* description;
@public
	BOOL iAmHere;
	NSDate* createdAt;	
	CLLocationCoordinate2D coordinate;
	CLLocationDistance elevation;
}

-(id)initWithNameAndDescription:(NSString*)myName description:(NSString*)myDescription;

@end
