//
//  BurbleDataManager.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BurbleDataManager.h"


@implementation BurbleDataManager

static BurbleDataManager *sharedDataManager;

- (void)dealloc {
	[super dealloc];
}


// Initialize the singleton instance if needed and return
+(BurbleDataManager *)sharedDataManager {
	//	@synchronized(self)
	{
		if (!sharedDataManager)
			sharedDataManager = [[BurbleDataManager alloc] init];
		
		return sharedDataManager;
	}
}

// Stop second allocation
+(id)alloc {
	//	@synchronized(self)
	{
		NSAssert(sharedDataManager == nil, @"Attempted to allocate a second instance of a singleton.");
		sharedDataManager = [super alloc];
		return sharedDataManager;
	}
}

//Stop copying singleton
+(id)copy {
	//	@synchronized(self)
	{
		NSAssert(sharedDataManager == nil, @"Attempted to copy the singleton.");
		return sharedDataManager;
	}
}

//automatically called on program launch
+ (void)initialize
{
    static BOOL initialized = NO;
    if (!initialized) {
		// Load any previously archived blog data
		[[BurbleDataManager sharedDataManager] loadData];
        initialized = YES;
    }
}

- (id)init {
	if (self = [super init]) {
		
	}
	return self;
}

#pragma mark -

//Runs after init, before anything else
- (void)loadData {
	
}

//Run right before app terminates
- (void)saveData {

}

#pragma mark -

- (NSArray*) getFriends {
	Person* p1 = [[Person alloc] init];
	
	[p1 setUid:1];
	[p1 setName:@"Niels"];
	
	Person* p2 = [[Person alloc] init];
	[p2 setUid:2];
	[p2 setName:@"Janelle"];
	
	Person* p3 = [[Person alloc] init];
	[p3 setUid:3];
	[p3 setName:@"Chris"];

	Person* p4 = [[Person alloc] init];
	[p4 setUid:4];
	[p4 setName:@"Jon"];
	
	Person* p5 = [[Person alloc] init];
	[p5 setUid:5];
	[p5 setName:@"Gleb"];

	NSArray* a = [NSArray arrayWithObjects:p1, p2, p3, p4, p5, nil];
	return a;
	
}
@end
