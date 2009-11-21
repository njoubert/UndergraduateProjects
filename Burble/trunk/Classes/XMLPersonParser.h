//
//  XMLPersonParser.h
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMLEventParser.h"
#import "Person.h"
#import "Group.h"
#import "Position.h"

@interface XMLPersonParser : XMLEventParser {
	Person* _person;
	Group* _group;
	Position* _position;
}
-(Person*)getPerson;
-(Group*)getGroup;
-(Position*)getPosition;

@end
