//
//  Message.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#define kMessageSenderKey	@"MessageSender"
#define kMessageMessageKey	@"MessageMessage"

@interface Message : NSObject <NSCoding> {
	NSString *sender;
	NSString *message;
}

@property (nonatomic, retain) NSString *sender;
@property (nonatomic, retain) NSString *message;

- (NSString*) getSender;

@end
