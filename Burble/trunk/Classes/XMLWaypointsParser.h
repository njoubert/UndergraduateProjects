//
//  XMLWaypointsParser.h
//  Test1
//
//  Created by Niels Joubert on 11/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMLEventParser.h"
#import "Waypoint.h"

@interface XMLWaypointsParser : XMLEventParser {
	Waypoint* _currentWaypoint;
	NSMutableArray* _waypoints;
}
-(NSArray*)getWaypoints;

@end
