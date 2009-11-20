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
	[baseUrl release];
	[presistent release];
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
	sharedDataManager = nil;
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
		
		baseUrl = [[NSURL alloc] initWithString:kBaseUrlStr];
		
		CLLocationCoordinate2D locCor;
		locCor.longitude = 0;
		locCor.latitude = 0;
		NSDate *d = [[NSDate alloc] init];
		lastKnownLocation = [[CLLocation alloc] initWithCoordinate:locCor altitude:0 horizontalAccuracy:-1 verticalAccuracy:-1 timestamp:d];
		
		myLocationManager = [[CLLocationManager alloc] init];
		myLocationManager.delegate = self;
		myLocationManager.desiredAccuracy = [[NSNumber numberWithDouble:kCLLocationAccuracyBest] doubleValue];
		myLocationManager.distanceFilter  = [[NSNumber numberWithDouble:40] doubleValue];
		[myLocationManager startUpdatingLocation];
		
		locallyAddedWaypoints = [[NSMutableArray alloc] init];
		
		myG = nil;
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
	if (nil == [presistent objectForKey:@"guid"]) {
		NSString* guid = [[UIDevice currentDevice] uniqueIdentifier];
		[presistent setValue:guid forKey:@"guid"];
	}
	[presistent writeToFile:[[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename] atomically:YES];
}

//Runs after init, before anything else
- (void)loadData {
	//attempt to load PresistentData
	NSString *presistentFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:presistentFilePath]) {
		presistent = [[NSMutableDictionary alloc] initWithContentsOfFile:presistentFilePath];
		bIsFirstLaunch = FALSE;
	} else {
		presistent = [[NSMutableDictionary alloc] init];
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

/*
 ================================================================================
								DATA CALLS for INTERNAL DATA
 ================================================================================ 
 */

#pragma mark -
#pragma mark Data Calls for Internal

- (NSString*) getGUID {
	return [presistent objectForKey:@"guid"];
}

- (NSString*) getName {
	return [presistent objectForKey:@"name"];
}
- (NSString*) getFirstName {
	NSArray *firstWords = [[[self getName] componentsSeparatedByString:@" "] subarrayWithRange:NSMakeRange(0,1)];
	NSString *result = [firstWords componentsJoinedByString:@" "];
	return result;
}

/*
 ================================================================================
					DATA CALLS for DEVICE DATA
 ================================================================================ 
 */
#pragma mark -
#pragma mark Data Calls for Device Data

- (CLLocation*) getLocation {
	CLLocation* retVal = [[CLLocation alloc] initWithCoordinate:lastKnownLocation.coordinate altitude:lastKnownLocation.altitude horizontalAccuracy:lastKnownLocation.horizontalAccuracy verticalAccuracy:lastKnownLocation.verticalAccuracy timestamp:lastKnownLocation.timestamp];	
	return retVal;
}

/*
 ================================================================================
					DATA CALLS for SERVER MANAGED DATA (Cached locally)
 ================================================================================ 
 */


#pragma mark -
#pragma mark Data Calls for Server Data

//calls back [target selector] with boolean indicating success
- (BOOL)startTryToRegister:(NSString*) name caller:(id)obj {
	if ([name length] == 0)
		return NO;
	
	if (tryToRegister_Caller != nil)
		return NO;
	
	tryToRegister_Caller = obj;
	
	Person* me = [[Person alloc] init];
	[me setName:name];
	[me setGuid:[presistent objectForKey:@"guid"]];	
	RPCPostData* pData = [[RPCPostData alloc] init];
	[me convertToData:pData];
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/create", [presistent objectForKey:@"guid"]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	[request setHTTPBodyPostData:pData];
	
	[name retain];
	tryToRegister_Name = name;
	
	[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(receiveTryToRegisterCallback:withValue:)];
	[urlString release];
	[regUrl release];
	if (connection)
		return YES;
	return NO;
}

- (void)receiveTryToRegisterCallback:(NSHTTPURLResponse *)urlres withValue:(id)a2 {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	
	if (urlres == nil) { //we have an error
		
		NSString *message= [[NSString alloc] initWithString:@"We could not connect to our server. Check your internet connectivity!"];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Confirm!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
	
	} else if ([urlres statusCode] == 201) { //created new person

		[presistent setValue:tryToRegister_Name forKey:@"name"];
		[self saveData];
		[tryToRegister_Caller dismissModalViewControllerAnimated:YES];

	} else if ([urlres statusCode] == 200) { //found you as a current person
		
		//TODO: Read XML here, save as actual name.
		
		//[presistent setValue:@"Niels Joubert" forKey:@"name"];
		//[self saveData];
		[tryToRegister_Caller dismissModalViewControllerAnimated:YES];

		NSString *message= [[NSString alloc] initWithString:@"Oh, apparently you and your phone is already in our database! We associated this phone with your account. If this is not what you want, you can manage your account in more detail on our website."];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Snap! We know you!" message:message delegate:nil cancelButtonTitle:@"Great!" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		
	} else {	//something fucked up

		NSString *message= [[NSString alloc] initWithString:@"Unfortunately our server reported an error. Sorry, this sucks, but email us and yell at us!"];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"500 Server Error!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		
	}
	tryToRegister_Caller = nil;
	[tryToRegister_Name release];
	tryToRegister_Name = nil;
}


- (void)login {	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/index", [presistent objectForKey:@"guid"]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	[RPCURLConnection sendAsyncRequest:request target:self selector:@selector(loginCallback:withValue:)];
	
}
- (void)loginCallback:(NSHTTPURLResponse*)response withValue:(id)a2 {
	if (response != nil && [response statusCode] == 200) {
		
		NSString *message = [[NSString alloc] initWithData:(NSData*)a2 encoding:NSUTF8StringEncoding];
		
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Response:" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];
	}
}


- (BOOL) createGroup:(Group*)g {
	
}

- (BOOL)isInGroup {
	return (nil != myG);
}

- (Group*) getMyGroup {
 	
}

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

- (void)addWaypoint:(Waypoint *)wP {
	[locallyAddedWaypoints addObject:wP];
}

- (int)getWaypointCount {
	return [locallyAddedWaypoints count];
}

- (NSArray*) getWaypoints {
	NSArray* retVal = [[NSArray alloc] initWithArray:locallyAddedWaypoints];
	return retVal;
}

- (NSString*) getNextWaypointName {
	NSString *retVal = [[NSString alloc] initWithFormat:@"Waypoint %d", [self getWaypointCount] + 1];
	return retVal;
}
- (NSString*) getNextWaypointDesc {
	NSString *retVal = [[NSString alloc] initWithFormat:@"Placed by %@", [self getName]];
	return retVal;
}

- (NSArray*) getMessages {
// TODO: For now, this data is faked!
	
	Message* m1 = [[Message alloc] init];
	[m1 setSender:@"Niels"];
	
	Message* m2 = [[Message alloc] init];
	[m2 setSender:@"Janelle"];
	
	Message* m3 = [[Message alloc] init];
	[m3 setSender:@"Chris"];
	
	NSArray* msgs = [NSArray arrayWithObjects:m1, m2, m3, nil];
	return msgs;
	
}
/*
 ================================================================================
			DELEGATE METHODS	
 ================================================================================ 
 */


- (void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation {

	if (newLocation.horizontalAccuracy < 0) return;
	// test the age of the location measurement to determine if the measurement is cached
	// in most cases you will not want to rely on cached measurements
	NSTimeInterval locationAge = -[newLocation.timestamp timeIntervalSinceNow];
	if (locationAge > 5.0) return;
	// store all of the measurements, just so we can see what kind of data we might receive
	[newLocation retain];
	[lastKnownLocation release];
	lastKnownLocation = newLocation;
	//update views and shit
}

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
	// The location "unknown" error simply means the manager is currently unable to get the location.
	if ([error code] != kCLErrorLocationUnknown) {
		
	}
}
@end
