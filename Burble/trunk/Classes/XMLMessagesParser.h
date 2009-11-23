//
//  XMLMessagesParser.h
//  Test1
//
//  Created by Niels Joubert on 11/22/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMLEventParser.h"
#import "Message.h"

@interface XMLMessagesParser : XMLEventParser {
	Message* _currentMessage;
	NSMutableArray* _messages;
}
-(NSArray*)getMessages;

@end
