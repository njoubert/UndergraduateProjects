//
//  XMLMessagesParser.m
//  Test1
//
//  Created by Niels Joubert on 11/22/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLMessagesParser.h"


@implementation XMLMessagesParser

-(NSArray*)getMessages {
	return _messages;
}

-(void)parserDidStartDocument:(NSXMLParser *)parser {
	_messages = [[NSMutableArray alloc] init];
	_currentMessage = nil;
	_state = kruft;
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
	
	if ([elementName isEqualToString:@"messages"]) {
		_state = messages;
	}	
	if (_state == messages) {
		if ([elementName isEqualToString:@"text-message"]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeText;
			_state = messages_message;
		} else if ([elementName isEqualToString:@"group-invite-message"]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeGroupInvite;
			_state = messages_message;
		} else if([elementName isEqualToString:@"routing-request-message"]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeRoutingRequest;
			_state = messages_message;
		} 
	}
	if (_state == messages_message) {
		if ([elementName isEqualToString:@"id"] || 
			[elementName isEqualToString:@"group-id"] || 
			[elementName isEqualToString:@"waypoint-id"] ||
			[elementName isEqualToString:@"sender-id"] || 
			[elementName isEqualToString:@"sent-time"] || 
			[elementName isEqualToString:@"text"] || 
			[elementName isEqualToString:@"read"]) {
			_currentElementText = [[NSMutableString alloc] init];
		} else if ([elementName isEqualToString:@"group"]) {
			_currentMessage.group = [[Group alloc] init];
			_state = messages_message_group;
		}
	}
	if (_state == messages_message_group) {
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
		_error = [[NSMutableDictionary alloc] init];
	} else if ([elementName isEqualToString:@"description"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"description"];
	} else if ([elementName isEqualToString:@"exception"] && _error != nil) {
		[_error setObject:_currentElementText forKey:@"exception"];
	}

	if (_state == messages) {	// we have received all the messages. we are basically donezo.
		if ([elementName isEqualToString:@"messages"]) {
			_state = kruft;
		}		
	}
	if (_state == messages_message) {	// we are currently parsing a message
		if ([elementName isEqualToString:@"messages"]) {
			_state = kruft;
		} else if ([elementName isEqualToString:@"text-message"] || 
			[elementName isEqualToString:@"group-invite-message"] || 
			[elementName isEqualToString:@"routing-request-message"]) { //snap we are done with this message
			[_messages addObject:_currentMessage];
			_currentMessage = nil;
			_state = messages;
		} else if ([elementName isEqualToString:@"position"]) {
			_state = person;
		} else if ([elementName isEqualToString:@"id"]) {
			_currentMessage.uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"sender-id"]) {
			_currentMessage.sender_uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"waypoint-id"]) {
			_currentMessage.waypoint_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"sent-time"]) {
			NSDateFormatter *dF = [[NSDateFormatter alloc] init];
			[dF setDateFormat:@"yyyy-MM-dd'T'hh:mm:ss'Z'"];
			[dF setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"GMT"]];
			_currentMessage.sent_time = [dF dateFromString:_currentElementText];
		} else if ([elementName isEqualToString:@"text"]) {
			_currentMessage.text = _currentElementText;
		} else if ([elementName isEqualToString:@"read"]) {
			_currentMessage.read = [_currentElementText boolValue];
		}
	}
	
	if (_state == messages_message_group) {
		if ([elementName isEqualToString:@"group"]) {
			_state = messages_message;	
		} else if ([elementName isEqualToString:@"id"]) {
			_currentMessage.group.group_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"name"]) {
			_currentMessage.group.name = _currentElementText;
		} else if ([elementName isEqualToString:@"description"]) {
			_currentMessage.group.description = _currentElementText;
		}
	}
	
	if (_currentElementText != nil) {
		[_currentElementText release];
		_currentElementText = nil;
		
	}
}

-(void)dealloc {
	[super dealloc];
	[_messages dealloc];
}

@end
