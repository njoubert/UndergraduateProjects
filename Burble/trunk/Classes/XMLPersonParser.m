//
//  XMLPersonParser.m
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLPersonParser.h"


@implementation XMLPersonParser

-(Person*)getPerson {
	if (_hasError)
		return nil;
	return _person;
}
-(Group*)getGroup {
	if (_hasError || _group == nil)
		return nil;
	return _group;
}
-(Position*)getPosition {
	if (_hasError || _position == nil)
		return nil;
	return _position;
}


-(void)parserDidStartDocument:(NSXMLParser *)parser {
	_person = [[Person alloc] init];
	_group = [[Group alloc] init];
	_position = [[Position alloc] init];
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
		_currentElementName = elementName;
	} else if ([elementName isEqualToString:@"exception"] && _error != nil) {
		_currentElementText = [[NSMutableString alloc] init];
		_currentElementName = elementName;
	}

	if ([elementName isEqualToString:@"person"]) {
		_state = person;
	}	
	if (_state == person) {
		if ([elementName isEqualToString:@"group"]) {
			_state = person_group;
		} else if ([elementName isEqualToString:@"position"]) {
			_state = person_position;
		} else if ([elementName isEqualToString:@"id"] ||
					[elementName isEqualToString:@"email"] || 
					[elementName isEqualToString:@"name"] || 
					[elementName isEqualToString:@"number"]) {
				_currentElementText = [[NSMutableString alloc] init];
		} 
	}
	if (_state == person_group) {
		if ([elementName isEqualToString:@"id"] || 
				   [elementName isEqualToString:@"name"] || 
				   [elementName isEqualToString:@"description"]) {
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
	if ([elementName isEqualToString:@"person"]) {
		_state = kruft;
	}
	if (_state == person_group) {
		if ([elementName isEqualToString:@"group"]) {
			_state = person;
		}
		if ([elementName isEqualToString:@"id"]) {
			if (_currentElementText != nil) {
				_group.group_id = [_currentElementText intValue];
			} else {
				_group.group_id = -1;
			}
		} else if ([elementName isEqualToString:@"name"]) {
			_group.name = [[NSString alloc] initWithString:_currentElementText];			
		} else if ([elementName isEqualToString:@"decription"]) {
			_group.description = [[NSString alloc] initWithString:_currentElementText];
		}
	}
	if (_state == person_position) {
		if ([elementName isEqualToString:@"position"]) {
			_state = person;
		}
	}
	if (_state == person) {
		if ([elementName isEqualToString:@"id"]) {
			if (_currentElementText != nil) {
				_person.uid = [_currentElementText intValue];
			} else {
				_person.uid = -1;
			}
		} else if ([elementName isEqualToString:@"email"]) {
			_person.email = [[NSString alloc] initWithString:_currentElementText];			
		} else if ([elementName isEqualToString:@"name"]) {
			_person.name = [[NSString alloc] initWithString:_currentElementText];
		} else if ([elementName isEqualToString:@"number"]) {
			_person.number = [[NSString alloc] initWithString:_currentElementText];
		} 
	}
	
	if (_currentElementName != nil) {
		[_currentElementName release];
		_currentElementName = nil;
	}
	if (_currentElementText != nil) {
		[_currentElementText release];
		_currentElementText = nil;
		
	}
}


-(void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError {
	if (_hasError)
		return;
}

-(void)dealloc {
	[_person release];
	[_error release];
	[super dealloc];
}
				
				
@end
