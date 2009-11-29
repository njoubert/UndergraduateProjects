//
//  XMLWaypointsParser.m
//  Test1
//
//  Created by Niels Joubert on 11/23/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLWaypointsParser.h"


@implementation XMLWaypointsParser

-(NSArray*)getWaypoints {
	return _waypoints;
}

-(void)parserDidStartDocument:(NSXMLParser *)parser {
	_waypoints = [[NSMutableArray alloc] init];
	_currentWaypoint = nil;
	_state = ePS_kruft;
}


-(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName 
							namespaceURI:(NSString *)namespaceURI 
							qualifiedName:(NSString *)qName 
							attributes:(NSDictionary *)attributeDict {
	if (_hasError)
		return;
	
	if ([elementName isEqualToString:@"error"]) {
		_error = [[NSMutableDictionary alloc] init];
	} else if ([elementName isEqualToString:@"description"] && _error != nil) {
		_currentElementText = [[NSMutableString alloc] init];
	} else if ([elementName isEqualToString:@"exception"] && _error != nil) {
		_currentElementText = [[NSMutableString alloc] init];
	}
	
	if ([elementName isEqualToString:@"waypoints"]) {
		_state = ePS_waypoints;
	}	
	if (_state == ePS_waypoints) {
		if ([elementName isEqualToString:@"waypoint"]) {
			_currentWaypoint = [[Waypoint alloc] init];
			_state = ePS_waypoints_waypoint;
		} 
	}
	if (_state == ePS_waypoints_waypoint) {
		if ([elementName isEqualToString:@"id"] || 
			[elementName isEqualToString:@"group-id"] || 
			[elementName isEqualToString:@"person-id"] || 
			[elementName isEqualToString:@"description"] ||
			[elementName isEqualToString:@"elevation"] || 
			[elementName isEqualToString:@"latitude"] || 
			[elementName isEqualToString:@"longitude"] || 
			[elementName isEqualToString:@"name"]) {
			_currentElementText = [[NSMutableString alloc] init];
		}
	}
}

-(void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName 
					namespaceURI:(NSString *)namespaceURI 
					qualifiedName:(NSString *)qName {
	if (_hasError)
		return;
	
	if ([elementName isEqualToString:@"error"]) {
		_hasError = YES;
		_error = [[NSMutableDictionary alloc] init];
	} else if ([elementName isEqualToString:@"description"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"description"];
	} else if ([elementName isEqualToString:@"exception"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"exception"];
	}
	
	if (_state == ePS_waypoints) {
		if ([elementName isEqualToString:@"waypoints"]) {
			_state = ePS_kruft;
		}		
	}
	if (_state == ePS_waypoints_waypoint) {	// we are currently parsing a waypoint
		if ([elementName isEqualToString:@"waypoint"]) { //snap we are done with this waypoint
			[_waypoints addObject:_currentWaypoint];
			[_currentWaypoint release];
			_currentWaypoint = nil;
			_state = ePS_waypoints;
		} else if ([elementName isEqualToString:@"id"]) {
			_currentWaypoint.uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"group-id"]) {
			_currentWaypoint.group_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"person-id"]) {
			_currentWaypoint.person_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"description"]) {
			_currentWaypoint.description = [[NSString alloc] initWithString:_currentElementText];
		} else if ([elementName isEqualToString:@"name"]) {
			_currentWaypoint.name = [[NSString alloc] initWithString:_currentElementText];
		} else if ([elementName isEqualToString:@"latitude"]) {
			[_currentWaypoint setLatitude:[_currentElementText doubleValue]];
		}  else if ([elementName isEqualToString:@"longitude"]) {
			[_currentWaypoint setLongitude:[_currentElementText doubleValue]];
		}  else if ([elementName isEqualToString:@"elevation"]) {
			_currentWaypoint.elevation = [_currentElementText doubleValue];
		}
	}
	
	if (_currentElementText != nil) {
		[_currentElementText release];
		_currentElementText = nil;
	}
}

-(void)dealloc {
	[_waypoints release];
	[super dealloc];
}

@end
