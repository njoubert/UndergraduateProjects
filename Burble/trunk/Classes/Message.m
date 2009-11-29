//
//  Message.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Message.h"


@implementation Message

@synthesize read, sent, type, sender_uid, text, waypoint_id, sent_time, uid, iSentThis, group;

-(void) dealloc {
	[receiver_uids release];
	[text release];
	[type release];
	[group release];
	[sent_time release];
	[super dealloc];
}

-(id)init {
	if (self = [super init]) {
		iSentThis = NO;
		read = NO;
		sent = NO;
		waypoint_id = -1;
		group = nil;
	}
	return self;
	
}
-(id)initWithText:(NSString *)msg {
	if (self = [super init]) {
		iSentThis = YES;
		read = NO;
		sent = NO;
		type = [[NSString alloc] initWithString:kMessageTypeText];
		text = [[NSString alloc] initWithString:msg];
		receiver_uids = [[NSMutableArray alloc] init];
		waypoint_id = -1;
		group = nil;
	}
	return self;
}
-(id)initWithText:(NSString*)comment waypoint:(int)wp_id {
	if (self = [super init]) {
		iSentThis = YES;
		read = NO;
		sent = NO;
		type = [[NSString alloc] initWithString:kMessageTypeRoutingRequest];
		text = [[NSString alloc] initWithString:comment];
		receiver_uids = [[NSMutableArray alloc] init];
		waypoint_id = wp_id;
		group = nil;
	}	
	return self;
}
-(id)initWithText:(NSString*)comment group:(Group*)gp {
	if (self = [super init]) {
		iSentThis = YES;
		read = NO;
		sent = NO;
		type = [[NSString alloc] initWithString:kMessageTypeGroupInvite];
		text = [[NSString alloc] initWithString:comment];
		receiver_uids = [[NSMutableArray alloc] init];
		waypoint_id = -1;
		group = [gp retain];
		
	}
	return self;
}

-(void)appendReceiverUid:(int)receiver_uid {
	if (receiver_uid <= 0)
		return;
	[receiver_uids addObject:[NSNumber numberWithInt:receiver_uid]];
}
-(void)appendReceiver:(Person*)receiver {
	if (receiver == nil)
		return;
	[self appendReceiverUid:receiver.uid];
}

-(NSString*)shortType {
	if (self.type == kMessageTypeText)
		return @"Text";
	else if (self.type == kMessageTypeGroupInvite)
		return @"Invite";
	else if (self.type == kMessageTypeRoutingRequest)
		return @"Route";
	return @"";
}


-(void) convertToData:(RPCPostData*)pData {
	NSString* senderIdStr = [NSString stringWithFormat:@"%d", sender_uid];
	NSString* waypointIdStr = [NSString stringWithFormat:@"%d", waypoint_id];
	NSString* groupIdStr = [NSString stringWithFormat:@"%d", group.group_id];
	
	[pData appendValue:senderIdStr forKey:kRPC_MessageSenderIDKey];
	[pData appendValue:waypointIdStr forKey:kRPC_MessageWaypointIDKey];
	[pData appendValue:groupIdStr forKey:kRPC_MessageGroupIDKey];
	[pData appendValue:type forKey:kRPC_MessageTypeKey];
	[pData appendValue:text forKey:kRPC_MessageTextKey];
	[pData appendValue:[sent_time description] forKey:kRPC_MessageSentTimeKey];
	
	//JSON the receiver list to the server
	NSMutableString* receiversString = [[NSMutableString alloc] init];
	[receiversString appendString:@"["];
	for (int i = 0; i < [receiver_uids count]; i++) {
		if (i+1 == [receiver_uids count])
			[receiversString appendFormat:@"%d",[[receiver_uids objectAtIndex:i] intValue]];
		else
			[receiversString appendFormat:@"%d,",[[receiver_uids objectAtIndex:i] intValue]];
	}
	[receiversString appendString:@"]"];
	[pData appendValue:receiversString forKey:kRPC_MessageReceiversKey];
	[receiversString release];
}

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Message* newMsg;
	newMsg = [[[self class] allocWithZone:zone] init];
	newMsg->read = read;
	newMsg->sender_uid = sender_uid;
	newMsg->waypoint_id = waypoint_id;
	newMsg->group = [group copy];
	newMsg->type = [[NSString allocWithZone:zone] initWithString:type];
	newMsg->text = [[NSString allocWithZone:zone] initWithString:text];
	newMsg->receiver_uids = [[NSMutableArray allocWithZone:zone] initWithArray:receiver_uids];
	return newMsg;
}

#pragma mark -
#pragma mark NSCoding
-(void)encodeWithCoder:(NSCoder *)coder {
	
}
-(id)initWithCoder:(NSCoder *)coder {
	if (self = [super init]) {
		
	}
	return self;
}

#pragma mark -
#pragma mark isEqual

-(BOOL)isEqual:(Message*)otherMsg {
	return (uid == otherMsg.uid);
}

@end
