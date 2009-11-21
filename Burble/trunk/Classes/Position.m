//
//  Position.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Position.h"


@implementation Position


-(NSData*) convertToData {
	return nil;
}

#pragma mark -
#pragma mark NSCopying
-(id)copyWithZone:(NSZone *)zone {
	Position *newPos;
	newPos = [[[self class] allocWithZone:zone] init];
	
	return newPos;
}

@end
