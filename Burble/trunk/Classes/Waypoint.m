//
//  Waypoint.m
//  Test1
//
//  Created by Niels Joubert on 11/18/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Waypoint.h"


@implementation Waypoint
@synthesize coordinate;

-(id)initWithNameAndDescription:(NSString*)myName description:(NSString*)myDescription; {
	if (self = [super init]) {
		name = [[NSString alloc] initWithString:myName];
		[name retain];
		description = [[NSString alloc] initWithString:myDescription];
		[description retain];
		
		syncedWithServer = NO;
	}
	return self;
}

-(NSString*) title {
	return name;
}
-(NSString*) subtitle {
	return description;
}

-(NSData*) convertToData {
	
}

@end
