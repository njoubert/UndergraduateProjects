//
//  Message.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RemoteModel.h"
#import "Person.h"

#define kMessageTypeText			@"TextMessage"
#define kMessageTypeGroupInvite		@"GroupInviteMessage"
#define kMessageTypeRoutingRequest	@"RoutingRequestMessage"

#define kRPC_MessageSenderIDKey		@"message[sender_id]"
#define kRPC_MessageWaypointIDKey	@"message[waypoint_id]"
#define kRPC_MessageGroupIDKey		@"message[group_id]"
#define kRPC_MessageTextKey			@"message[text]"
#define kRPC_MessageTypeKey			@"type"
#define kRPC_MessageReceiversKey	@"receivers"

@interface Message : RemoteModel <NSCopying, NSCoding> {
	@public
	/* Internal Management, don't touch these from outside */
	BOOL read;
	BOOL sent;
	int sender_uid;
	NSMutableArray* receiver_uids;	//an array of NSNumbers
	NSString* type;				//see kMessageTypeXxx defines
	
	/* Things you can set and get */
	NSString *text;	//text message to send
	int waypoint_id;	//waypoint to route to. Ignores this if <= 0
	int group_id;
	
}

@property BOOL read;
@property BOOL sent;
@property (nonatomic, retain) NSString *text;
@property (nonatomic, assign) int sender_uid;
	
// To send a text message:
-(id)initWithText:(NSString*)msg;
// To send a routing request message:
-(id)initWithText:(NSString*)comment waypoint:(int)wp_id;
// To send a group invite message:
-(id)initWithText:(NSString*)comment group:(int)gp_id;

-(void)appendReceiverUid:(int)receiver_uid;
-(void)appendReceiver:(Person*)receiver;

-(id)copyWithZone:(NSZone *)zone;

@end
