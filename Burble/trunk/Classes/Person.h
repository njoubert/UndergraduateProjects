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

#import <Foundation/Foundation.h>

@interface Person : NSObject <NSCoding> {
	int uid;
	NSString	*name;
	NSString	*number;
	NSString	*email;
}
@property int uid;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *number;
@property (nonatomic, retain) NSString *email;



@end
