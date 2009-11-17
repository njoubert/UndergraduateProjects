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

@synthesize currentDirectoryPath;

- (void)dealloc {
	[super dealloc];
}


// Initialize the singleton instance if needed and return
+(BurbleDataManager *)sharedDataManager {
	//	@synchronized(self)
	{
		if (!sharedDataManager) {
			sharedDataManager = [[BurbleDataManager alloc] init];
			[sharedDataManager retain];
		}
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
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		self.currentDirectoryPath = documentsDirectory;
	}
	return self;
}


 /*
 ================================================================================
							INTERNAL STATE MANAGEMENT
 ================================================================================
 */

#pragma mark -
#pragma mark Internal State Management

//This checks that the presistent dictionary contains all the appropriate keys, and saves it.
//Assumes that presistent exists, and sets presistent to the value.
- (void)checkAndSavePresistentFile {
	NSMutableDictionary *toSave = [[NSMutableDictionary alloc] initWithDictionary:presistent];
	if (nil == [toSave objectForKey:@"guid"]) {
		NSString* guid = [[UIDevice currentDevice] uniqueIdentifier];
		[toSave setValue:guid forKey:@"guid"];
	}
	[toSave writeToFile:[[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename] atomically:YES];
	[presistent release];
	presistent = toSave;
	[toSave release];
}

//Runs after init, before anything else
- (void)loadData {
	//attempt to load PresistentData
	NSString *presistentFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:presistentFilePath]) {
		presistent = [[NSDictionary alloc] initWithContentsOfFile:presistentFilePath];
		bIsFirstLaunch = FALSE;
	} else {
		presistent = [[NSDictionary alloc] init];
		[self checkAndSavePresistentFile];
		bIsFirstLaunch = TRUE;
	}
}
	 
//Run right before app terminates
- (void)saveData {
	[self checkAndSavePresistentFile];
}

- (BOOL)isRegistered {
	return (nil != [presistent objectForKey:@"name"]);
}
- (BOOL)isFirstLaunch {
	return bIsFirstLaunch;
}

- (NSString*) getGUID {
	return [presistent objectForKey:@"guid"];
}

/*
 ================================================================================
								CONNECTION MANAGEMENT
 ================================================================================ 
 */

#pragma mark -
#pragma mark Connection Management

- (BOOL)login {
	//create a request
	
	//send it to the server
	
	//check the response
		//if not 200, error!
	
		
}

/*
 ================================================================================
								DATA CALLS
 ================================================================================ 
 */

#pragma mark -
#pragma mark Data Calls

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
