//
//  XMLGroupParser.h
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "XMLEventParser.h"
#import "Group.h"

@interface XMLGroupParser : XMLEventParser {
	Group* _group;
}
-(Group*)getGroup;


@end
