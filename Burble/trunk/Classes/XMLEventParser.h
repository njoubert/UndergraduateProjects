//
//  XMLEventParser.h
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum parserState {
	kruft,
	person,
	group,
	person_group,
	person_position,
	messages,
	messages_message,
	done
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
