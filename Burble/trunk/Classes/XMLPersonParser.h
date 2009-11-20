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

@interface XMLPersonParser : XMLEventParser {
	Person* _person;
}
-(Person*)getPerson;


@end
