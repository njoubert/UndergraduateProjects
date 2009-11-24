//
//  XMLFriendsParser.h
//  Test1
//
//  Created by Niels Joubert on 11/24/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMLEventParser.h"
#import "Person.h"
#import "Position.h"
#import "Group.h"

@interface XMLFriendsParser : XMLEventParser {
	Person* _currentPerson;
	NSMutableArray* _friends;
}
-(NSMutableArray*)getFriends;

@end
