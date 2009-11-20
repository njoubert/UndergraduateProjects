//
//  Group.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RemoteModelProtocol.h"

@interface Group : NSObject <RemoteModelProtocol> {
	int group_id;
	NSString* name;
	NSString* description;
}
@property (retain, nonatomic) NSString* name;
@property (retain, nonatomic) NSString* description;

@end
