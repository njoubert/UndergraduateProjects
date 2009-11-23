//
//  Group.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RemoteModel.h"

#define kRPC_GroupIdKey				@"group[id]"
#define kRPC_GroupNameKey			@"group[name]"
#define kRPC_GroupDescriptionKey	@"group[description]"

@interface Group : RemoteModel <NSCopying, NSCoding> {
	int group_id;
	NSString* name;
	NSString* description;
}
@property (copy, nonatomic) NSString* name;
@property (copy, nonatomic) NSString* description;
@property int group_id;

@end
