//
//  XMLEventParser.h
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum parserState {
	ePS_kruft,
	ePS_person,
	ePS_group,
	ePS_person_group,
	ePS_person_position,
	ePS_messages,
	ePS_messages_message,
	ePS_messages_message_group,
	ePS_waypoints,
	ePS_waypoints_waypoint,
	ePS_done
} parserState;

@interface XMLEventParser : NSObject {
	NSXMLParser* _parser;
	NSMutableString* _currentElementText;
	BOOL _hasError;
	NSMutableDictionary* _error;
	parserState _state;
}
-(id)initWithData:(NSData*)data;
-(BOOL)hasError;
-(NSDictionary*) getError;

// =================================
// ======= PARSER DELEGATE METHODS, OVERRIDE ONLY THESE IN SUBCLASSES!
// =================================

//Document start and end
-(void)parserDidStartDocument:(NSXMLParser *)parser;
-(void)parserDidEndDocument:(NSXMLParser *)parser;

//Elements
-(void)parser:(NSXMLParser *)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict;
-(void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string;
-(void)parser:(NSXMLParser *)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName;

//Parser Error
-(void)parser:(NSXMLParser *)parser parseErrorOccurred:(NSError *)parseError;

@end
