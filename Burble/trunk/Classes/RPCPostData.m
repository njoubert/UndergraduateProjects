//
// RPCPostData.m
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RPCPostData.h"

@implementation RPCPostData

- (id)init {
	if (self = [super init]) {
		dataStr = [[NSMutableString alloc] init];
	}
	return self;
}
- (NSString*) getString {
	return [[NSString alloc] initWithString:dataStr];
}

- (NSData*) getData {
	return [NSData dataWithBytes: [dataStr UTF8String] length: [dataStr length]];
}

- (void)appendValue:(NSString*)val forKey:(NSString*)key {
	if (val == nil || key == nil || [val length] == 0 || [key length] == 0) {
		return; //if the string is nil dont do shit
	}
	
	//escape the values
	NSString* valEsc = [val stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	NSString* keyEsc = [key stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
	
	//now decide how to add it to the mutable datastr
	if ([dataStr length] == 0) {
		[dataStr appendFormat:@"%@=%@", keyEsc, valEsc];			//just add it
	} else {
		[dataStr appendFormat:@"&%@=%@", keyEsc, valEsc]; 		//first stick an & in front
	}	 
}

@end
