//
//  XMLFriendsParser.m
//  Test1
//
//  Created by Niels Joubert on 11/24/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLFriendsParser.h"


@implementation XMLFriendsParser

-(NSMutableArray*)getFriends {
	return _friends;
}

-(void)parserDidStartDocument:(NSXMLParser *)parser {
	_friends = [[NSMutableArray alloc] init];
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
	
	if ([elementName isEqualToString:@"friendslist"]) {
		_state = ePS_friendslist;
	} else if (_state == ePS_friendslist) {
		if ([elementName isEqualToString:@"person"]) {
			_currentPerson = [[Person alloc] init];
			_state = ePS_person;
		}
	} else if (_state == ePS_person) {
		if ([elementName isEqualToString:@"group"]) {
			_state = ePS_person_group;
			Group* g = [[Group alloc] init];
			_currentPerson.group = g;
			[g release];
		} else if ([elementName isEqualToString:@"position"]) {
			_state = ePS_person_position;
			Position* p = [[Position alloc] init];
			_currentPerson.position = p;
			[p release];
		} else if ([elementName isEqualToString:@"id"] ||
				   [elementName isEqualToString:@"email"] || 
				   [elementName isEqualToString:@"name"] || 
				   [elementName isEqualToString:@"number"]) {
			_currentElementText = [[NSMutableString alloc] init];
		} 
	} else if (_state == ePS_person_group) {
		if ([elementName isEqualToString:@"id"] || 
			[elementName isEqualToString:@"name"] || 
			[elementName isEqualToString:@"description"]) {
			_currentElementText = [[NSMutableString alloc] init];
		} 
	} else if (_state == ePS_person_position) {
		if ([elementName isEqualToString:@"id"] || 
			[elementName isEqualToString:@"person-id"] || 
			[elementName isEqualToString:@"latitude"] || 
			[elementName isEqualToString:@"longitude"] || 
			[elementName isEqualToString:@"vaccuracy"] || 
			[elementName isEqualToString:@"haccuracy"] || 
			[elementName isEqualToString:@"elevation"] || 
			[elementName isEqualToString:@"heading"] || 
			[elementName isEqualToString:@"speed"] ||
			[elementName isEqualToString:@"timestamp"]) {
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
	} else if ([elementName isEqualToString:@"description"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"description"];
	} else if ([elementName isEqualToString:@"exception"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"exception"];
	}
	if ([elementName isEqualToString:@"friendslist"]) {
		_state = ePS_kruft;
	}
	if (_state == ePS_person_group) {
		
		if ([elementName isEqualToString:@"group"]) {
			_state = ePS_person;
		}
		if ([elementName isEqualToString:@"id"]) {
			if (_currentElementText != nil) {
				_currentPerson.group.group_id = [_currentElementText intValue];
			} else {
				_currentPerson.group.group_id = -1;
			}
		} else if ([elementName isEqualToString:@"name"]) {
			_currentPerson.group.name = [[NSString alloc] initWithString:_currentElementText];			
		} else if ([elementName isEqualToString:@"decription"]) {
			_currentPerson.group.description = [[NSString alloc] initWithString:_currentElementText];
		}
		
	}
	if (_state == ePS_person_position) {
		
		if ([elementName isEqualToString:@"position"]) {
			_state = ePS_person;
		}
		if ([elementName isEqualToString:@"id"]) {
			_currentPerson.position.uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"person-id"]) {
			_currentPerson.position.person_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"latitude"]) {
			_currentPerson.position.lat = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"longitude"]) {
			_currentPerson.position.lon = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"vaccuracy"]) {
			_currentPerson.position.vaccuracy = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"haccuracy"]) {
			_currentPerson.position.haccuracy = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"elevation"]) {
			_currentPerson.position.elevation = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"heading"]) {
			_currentPerson.position.heading = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"speed"]) {
			_currentPerson.position.speed = [_currentElementText doubleValue];
		} else if ([elementName isEqualToString:@"timestamp"]) {
			NSDateFormatter *dF = [[NSDateFormatter alloc] init];
			[dF setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
			[dF setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"GMT"]];
			_currentPerson.position.timestamp = [dF dateFromString:_currentElementText];
			[dF release];
		}
	}
	if (_state == ePS_person) {
		if ([elementName isEqualToString:@"person"]) {
			[_friends addObject:_currentPerson];
			[_currentPerson release];
			_currentPerson = nil;
			_state = ePS_friendslist;
		} else if ([elementName isEqualToString:@"id"]) {
			if (_currentElementText != nil) {
				_currentPerson.uid = [_currentElementText intValue];
			} else {
				_currentPerson.uid = -1;
			}
		} else if ([elementName isEqualToString:@"email"]) {
			_currentPerson.email = [[NSString alloc] initWithString:_currentElementText];			
		} else if ([elementName isEqualToString:@"name"]) {
			_currentPerson.name = [[NSString alloc] initWithString:_currentElementText];
		} else if ([elementName isEqualToString:@"number"]) {
			_currentPerson.number = [[NSString alloc] initWithString:_currentElementText];
		} 
		
	}
	
	if (_currentElementText != nil) {
		[_currentElementText release];
		_currentElementText = nil;
		
	}
}

-(void)dealloc {
	[_friends release];
	[super dealloc];
}


@end
