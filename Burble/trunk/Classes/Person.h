//
//  Person.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#define kPersonIdKey		@"PersonID"
#define kPersonNameKey		@"PersonName"
#define kPersonNumberKey	@"PersonNumber"
#define kPersonEmailKey		@"PersonEmail"

#define kRPC_PersonIdKey		@"person[id]"
#define kRPC_PersonGuidKey		@"person[guid]"
#define kRPC_PersonNameKey		@"person[name]"
#define kRPC_PersonNumberKey	@"person[number]"
#define kRPC_PersonEmailKey		@"person[email]"

#import <Foundation/Foundation.h>
#import "RemoteModel.h"

@interface Person : RemoteModel <NSCopying, NSCoding> {
	int uid;
	NSString	*guid;
	NSString	*name;
	NSString	*number;
	NSString	*email;
}
@property int uid;
@property (nonatomic, retain) NSString *guid;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *number;
@property (nonatomic, retain) NSString *email;

- (NSString*) getFirstName;

@end
