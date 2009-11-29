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
	
	if ([elementName isEqualToString:@"messages"]) {
		_state = ePS_messages;
	}	
	if (_state == ePS_messages) {
		if ([elementName isEqualToString:kMessageTypeText]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeText;
			_state = ePS_messages_message;
		} else if ([elementName isEqualToString:kMessageTypeGroupInvite]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeGroupInvite;
			_state = ePS_messages_message;
		} else if([elementName isEqualToString:kMessageTypeRoutingRequest]) {
			_currentMessage = [[Message alloc] init];
			_currentMessage.type = kMessageTypeRoutingRequest;
			_state = ePS_messages_message;
		} 
	}
	if (_state == ePS_messages_message) {
		if ([elementName isEqualToString:@"id"] || 
			[elementName isEqualToString:@"group-id"] || 
			[elementName isEqualToString:@"waypoint-id"] ||
			[elementName isEqualToString:@"sender-id"] || 
			[elementName isEqualToString:@"sent-time"] || 
			[elementName isEqualToString:@"text"] || 
			[elementName isEqualToString:@"read"]) {
			_currentElementText = [[NSMutableString alloc] init];
		} else if ([elementName isEqualToString:@"group"]) {
			Group* g = [[Group alloc] init];
			_currentMessage.group = g;
			[g release];
			_state = ePS_messages_message_group;
		}
	}
	if (_state == ePS_messages_message_group) {
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

	if (_state == ePS_messages) {	// we have received all the messages. we are basically donezo.
		if ([elementName isEqualToString:@"messages"]) {
			_state = ePS_kruft;
		}		
	}
	if (_state == ePS_messages_message) {	// we are currently parsing a message
		if ([elementName isEqualToString:@"messages"]) {
			_state = ePS_kruft;
		} else if ([elementName isEqualToString:kMessageTypeText] || 
			[elementName isEqualToString:kMessageTypeGroupInvite] || 
			[elementName isEqualToString:kMessageTypeRoutingRequest]) { //snap we are done with this message
			[_messages addObject:_currentMessage];
			[_currentMessage release];
			_currentMessage = nil;
			_state = ePS_messages;
		} else if ([elementName isEqualToString:@"id"]) {
			_currentMessage.uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"sender-id"]) {
			_currentMessage.sender_uid = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"waypoint-id"]) {
			_currentMessage.waypoint_id = [_currentElementText intValue];
		} else if ([elementName isEqualToString:@"sent-time"]) {
			NSDateFormatter *dF = [[NSDateFormatter alloc] init];
			[dF setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss'Z'"];
			[dF setTimeZone:[NSTimeZone timeZoneWithAbbreviation:@"GMT"]];
			_currentMessage.sent_time = [dF dateFromString:_currentElementText];
			[dF release];
			if (_currentMessage.sent_time == nil)
				NSLog(@"WTF nil date noooooooo");			
		} else if ([elementName isEqualToString:@"text"]) {
			_currentMessage.text = _currentElementText;
		} else if ([elementName isEqualToString:@"read"]) {
			_currentMessage.read = [_currentElementText boolValue];
		}
	}
	
	if (_state == ePS_messages_message_group) {
		if ([elementName isEqualToString:@"group"]) {
			_state = ePS_messages_message;	
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
	[_messages release];
	[super dealloc];
}

@end
