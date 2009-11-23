//
//  XMLGroupParser.m
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLGroupParser.h"

@implementation XMLGroupParser

-(Group*)getGroup{
	if (_hasError)
		return nil;
	return _group;
}

-(void)parserDidStartDocument:(NSXMLParser *)parser {
	_group = [[Group alloc] init];
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
	
	if ([elementName isEqualToString:@"group"]) {
		_state = group;
	}
	if (_state == group) {
		if ([elementName isEqualToString:@"id"]) {
			if (![[attributeDict valueForKey:@"nil"] isEqualToString:@"true"]) {
				_currentElementText = [[NSMutableString alloc] init];
			}
		} else if ([elementName isEqualToString:@"name"]) {
			_currentElementText = [[NSMutableString alloc] init];
		} else if ([elementName isEqualToString:@"description"]) {
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
	if ([elementName isEqualToString:@"group"]) {
		_state = kruft;
	}
	if (_state == group) {
		if ([elementName isEqualToString:@"id"]) {
			if (_currentElementText != nil) {
				_group.group_id = [_currentElementText intValue];
			} else {
				_group.group_id = -1;
			}
		} else if ([elementName isEqualToString:@"name"]) {
			_group.name = [[NSString alloc] initWithString:_currentElementText];
		} else if ([elementName isEqualToString:@"number"]) {
			_group.description = [[NSString alloc] initWithString:_currentElementText];
		} 
	}
	
	if (_currentElementText != nil) {
		[_currentElementText release];
		_currentElementText = nil;
		
	}
}

-(void)dealloc {
	[_group release];
	[_error release];
	[super dealloc];
}


@end
