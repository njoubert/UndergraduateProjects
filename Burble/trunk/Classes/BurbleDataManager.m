//
//  BurbleDataManager.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "BurbleDataManager.h"
#import "XMLPersonParser.h"
#import "XMLGroupParser.h"

@implementation BurbleDataManager

static BurbleDataManager *sharedDataManager;

@synthesize currentDirectoryPath, baseUrl;

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
		
		//viewsToRefreshOnLogins = [[NSMutableDictionary alloc] init];
		
		baseUrl = [[NSURL alloc] initWithString:kBaseUrlStr];
		
		myGroup = nil; //load this from presistent later
		
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
		
		joinGroupCallbackSel = nil;
		joinGroupCallbackObj = nil;
		createGroupCallbackObj = nil;
		createGroupCallbackSel = nil;
		leaveGroupCallbackObj = nil;
		leaveGroupCallbackSel = nil;
		
		
			NSLog(@"weefdffde");
		//QUEUES:
		waypointQueue = [[NSMutableArray alloc] init];
		positionQueue = [[NSMutableArray alloc] init];
		outgoingMessagesQueue = [[NSMutableArray alloc] init];
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
		if (nil != [presistent objectForKey:@"myGroup.group_id"]) {
			myGroup = [[Group alloc] init];
			myGroup.group_id = [[presistent objectForKey:@"myGroup.group_id"] intValue];
			myGroup.name = [[NSString alloc] initWithString:[presistent objectForKey:@"myGroup.name"]];
			if (nil != [presistent objectForKey:@"myGroup.description"])
				myGroup.description = [[NSString alloc] initWithString:[presistent objectForKey:@"myGroup.description"]];
		}
		bIsFirstLaunch = FALSE;
	} else {
		presistent = [[NSMutableDictionary alloc] init];
		[self checkAndSavePresistentFile];
		bIsFirstLaunch = TRUE;
	}
}
	 
//Run right before app terminates
- (void)saveData {
	[self updatePresistentWithMyGroup];
	[self checkAndSavePresistentFile];
}

- (BOOL)isRegistered {
	return (nil != [presistent objectForKey:@"name"] && nil != [presistent objectForKey:@"uid"]);
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
-(int) getUid {
	return [[presistent objectForKey:@"uid"] intValue];;
}

- (NSString*) getName {
	return [presistent objectForKey:@"name"];
}
- (NSString*) getFirstName {
	NSArray *firstWords = [[[self getName] componentsSeparatedByString:@" "] subarrayWithRange:NSMakeRange(0,1)];
	NSString *result = [firstWords componentsJoinedByString:@" "];
	return result;
}

- (void)updatePresistentWithPerson: (Person*)p {
	if (nil == p)
		return;
	[presistent setObject:[[NSString stringWithFormat:@"%d", p.uid] retain] forKey:@"uid"];
	if ([p.name length] != 0) { 
		[presistent setObject:[[NSString alloc] initWithString:p.name] forKey:@"name"];
	}
	if ([p.email length] != 0) { 
		[presistent setObject:[[NSString alloc] initWithString:p.email] forKey:@"email"];
	}
	if ([p.number length] != 0) { 
		[presistent setObject:[[NSString alloc] initWithString:p.number] forKey:@"number"];
	}
	[self saveData];
}
- (void)updatePresistentWithMyGroup {
	if (nil == myGroup) {
		[presistent removeObjectForKey:@"myGroup.group_id"];
		[presistent removeObjectForKey:@"myGroup.name"];
		[presistent removeObjectForKey:@"myGroup.description"];
	} else {
		[presistent setValue:[[[NSString alloc] initWithFormat:@"%d", myGroup.group_id] autorelease] forKey:@"myGroup.group_id"];
		[presistent setValue:myGroup.name forKey:@"myGroup.name"];
		[presistent setValue:myGroup.description forKey:@"myGroup.description"];	
	}
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
						QUEUE MANAGEMENT  for SERVER DATA
 ================================================================================ 
 */
#pragma mark -
#pragma mark Queue Management for Server Data

/************** Positions ****************/

- (void) sendPositionToServerCallback:(RPCURLResponse*)rpcResponse withObject:(Position*)p {
	if (rpcResponse.response == nil) {
		[positionQueue addObject:p];
	} else if (rpcResponse.response.statusCode == 200) {
		; //done!
	} else {
		[positionQueue addObject:p];
		[self messageForUnrecognizedStatusCode:rpcResponse.response];
	}
}

- (void)sendPositionToServer:(Position*)p {
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/position", [presistent objectForKey:@"guid"]];
	RPCPostRequest* request = [p getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendPositionToServerCallback:withObject:) withUserObject:p]) {
		[positionQueue addObject:p];
	}
}

- (void)firePositionToServer:(NSTimer*)timer {
	[self sendPositionToServer:[timer userInfo]];
}

//Starts the process of sending all the positions in the queue to the server
- (void)flushPositionQueue {
	@synchronized(positionQueue) {
		if (![self isRegistered])
			return;
		if ([positionQueue count] == 0)
			return;
		Position* p;
		int numRequests = 1;
		
		while ([positionQueue count] > 0) {
			p = [positionQueue lastObject];
			[positionQueue removeLastObject];
			if (numRequests <= kNumOfConcurrentRequestsFromQueue) {
				[self sendPositionToServer:p];
				numRequests++;
			} else {
				//we set up a timer to send a sync request.
				//we catch the requests coming back and check if they have succeeded. if not we add back to the queue.
				[NSTimer scheduledTimerWithTimeInterval:kTimeBetweenRequests*(numRequests/kNumOfConcurrentRequestsFromQueue) 
												 target:self
											   selector:@selector(firePositionToServer:) 
											   userInfo:p 
												repeats:NO];
				numRequests++;
			}
		}
	}
}

/************** Waypoints ****************/

//cathes the callback from the server for sending a waypoint
- (void) sendWaypointToServerCallback:(RPCURLResponse*)rpcResponse withObject:(Waypoint*)wp {
	if (rpcResponse.response == nil) {
		[waypointQueue addObject:wp];
	} else if (rpcResponse.response.statusCode == 201) {
		NSString* uidStr = [[NSString alloc] initWithData:rpcResponse.data encoding:NSUTF8StringEncoding];
		wp.uid = [uidStr intValue];
	} else {
		[waypointQueue addObject:wp];
		[self messageForUnrecognizedStatusCode:rpcResponse.response];
	}
}

//sends the given waypoint to the server
- (void)sendWaypointToServer:(Waypoint*)wp {
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/add_waypoint_for_group/%d", [presistent objectForKey:@"guid"], wp.group_id];
	RPCPostRequest* request = [wp getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendWaypointToServerCallback:withObject:) withUserObject:wp]) {
		[waypointQueue addObject:wp];
	}
}

//Helper for NSTimer to postpone sending waypoint to server if there are already a bunch of requests running
- (void)fireWaypointToServer:(NSTimer*)timer {
	[self sendWaypointToServer:[timer userInfo]];
}


// Starts the actual process of sending waypoints to the server, using the three helpers above.
- (void)flushWaypointQueue {
	@synchronized(waypointQueue) {
		if (![self isRegistered])
			return;
		if ([waypointQueue count] == 0)
			return;
		
		Waypoint* wp;
		int numRequests = 1;
		
		while ([waypointQueue count] > 0) {
			wp = [waypointQueue lastObject];
			[waypointQueue removeLastObject];
			if (numRequests <= kNumOfConcurrentRequestsFromQueue) {
				[self sendWaypointToServer:wp];
				numRequests++;
			} else {
				//we set up a timer to send a sync request.
				//we catch the requests coming back and check if they have succeeded. if not we add back to the queue.
				[NSTimer scheduledTimerWithTimeInterval:kTimeBetweenRequests*(numRequests/kNumOfConcurrentRequestsFromQueue) 
												 target:self
											   selector:@selector(fireWaypointToServer:) 
											   userInfo:wp 
												repeats:NO];
				numRequests++;
			}
		}
	}
}

/************** Outgoing Messages ****************/

//cathes the callback from the server for sending a waypoint
- (void) sendMessageToServerCallback:(RPCURLResponse*)rpcResponse withObject:(Message*)m {
	if (rpcResponse.response == nil) {
		[outgoingMessagesQueue addObject:m];
	} else if (rpcResponse.response.statusCode == 201) {
		m.sent = YES;
	} else {
		[outgoingMessagesQueue addObject:m];
		[self messageForUnrecognizedStatusCode:rpcResponse.response];
	}
}

//sends the given waypoint to the server
- (void)sendMessageToServer:(Message*)m {
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/messages/sendMsg", [presistent objectForKey:@"guid"]];
	RPCPostRequest* request = [m getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendMessageToServerCallback:withObject:) withUserObject:m]) {
		[outgoingMessagesQueue addObject:m];
	}
}

//Helper for NSTimer to postpone sending waypoint to server if there are already a bunch of requests running
- (void)fireMessageToServer:(NSTimer*)timer {
	[self sendMessageToServer:[timer userInfo]];
}


// Starts the actual process of sending waypoints to the server, using the three helpers above.
- (void)flushOutgoingMessagesQueue {
	@synchronized(outgoingMessagesQueue) {
		if (![self isRegistered])
			return;
		if ([outgoingMessagesQueue count] == 0)
			return;
		
		Message* m;
		int numRequests = 1;
		
		while ([outgoingMessagesQueue count] > 0) {
			m = [outgoingMessagesQueue lastObject];
			[outgoingMessagesQueue removeLastObject];
			if (numRequests <= kNumOfConcurrentRequestsFromQueue) {
				[self sendMessageToServer:m];
				numRequests++;
			} else {
				//we set up a timer to send a sync request.
				//we catch the requests coming back and check if they have succeeded. if not we add back to the queue.
				[NSTimer scheduledTimerWithTimeInterval:kTimeBetweenRequests*(numRequests/kNumOfConcurrentRequestsFromQueue) 
												 target:self
											   selector:@selector(fireMessageToServer:) 
											   userInfo:m
												repeats:NO];
				numRequests++;
			}
		}
	}
}

/*
 ================================================================================
					DATA CALLS for SERVER MANAGED DATA (Cached locally)
 ================================================================================ 
 */


#pragma mark -
#pragma mark Data Calls for Server Data

- (void)messageForCouldNotConnectToServer {
	NSString *message= [[NSString alloc] initWithString:@"We could not connect to our server. Check your internet connectivity!"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: @"Confirm!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];	
}

- (void)messageForUnrecognizedStatusCode:(NSHTTPURLResponse*)res {
	NSString *title = [[NSString alloc] initWithFormat:@"Error code %d", [res statusCode]];
	NSString *message = [[NSString alloc] initWithString:@"Unfortunately our server reported an error. Sorry, this sucks, but email us and yell at us!"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];	
	[title release];
}
- (void)messageForParserError:(XMLEventParser*)ps {
	NSString *title= [[NSString alloc] initWithString:@"Parsing server data failed!"];
	NSString *message= [[NSString alloc] initWithString:@"OK we didn't expect this to happen... Email us and yell at us."];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK!" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];		
	[title release];
}


/*********************** LOGIN STUFF *****************************/

//calls back [target selector] with boolean indicating success
//Caller must be a UIViewController displaying a modal view to be dismissed on success.
- (BOOL)startTryToRegister:(NSString*)name caller:(id)obj {
	if ([name length] == 0)
		return NO;
	
	if (tryToRegister_Caller != nil)
		return NO;
	
	tryToRegister_Caller = obj;
	
	Person* me = [[Person alloc] init];
	[me setName:name];
	[me setGuid:[presistent objectForKey:@"guid"]];	
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/create", [presistent objectForKey:@"guid"]];
	RPCPostRequest* request = [me getPostRequestToMethod:urlString withBaseUrl:baseUrl];

	[name retain];
	tryToRegister_Name = name;
	
	[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(receiveTryToRegisterCallback:withObject:)];
	[urlString release];
	if (connection)
		return YES;
	return NO;
}

- (void)receiveTryToRegisterCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	
	NSHTTPURLResponse* urlres = rpcResponse.response;
	if (urlres == nil) { //we have an error
		[self messageForCouldNotConnectToServer];	
	} else if (urlres.statusCode == 201) { //created new person

		[presistent setValue:tryToRegister_Name forKey:@"name"];
		XMLPersonParser* pparser = [[XMLPersonParser alloc] initWithData:rpcResponse.data];
		if (![pparser hasError]) {
			Person *p = [[pparser getPerson] retain];
			[self updatePresistentWithPerson:p];
			[p release];
		}
		[pparser release];
		[self saveData];
		[tryToRegister_Caller dismissModalViewControllerAnimated:YES];

	} else if (urlres.statusCode == 200) { //found you as a current person
		
		[tryToRegister_Caller dismissModalViewControllerAnimated:YES];
		
		XMLPersonParser* pparser = [[XMLPersonParser alloc] initWithData:rpcResponse.data];
		if (![pparser hasError]) {
			Person *p = [[pparser getPerson] retain];
			[self updatePresistentWithPerson:p];
			
			NSString *title= [[NSString alloc] initWithFormat:@"Welcome back %@", p.name];
			NSString *message= [[NSString alloc] initWithString:@"Oh, apparently you and your phone is already in our database! We associated this phone with your account. If this is not what you want, you can manage your account in more detail on our website."];
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"Great!" otherButtonTitles:nil];
			[alert show];
			[alert release];
			[message release];		
			[title release];
			
			[p release];
		} else {
			[self messageForParserError:pparser];
		}
		[pparser release];

	} else {
		[self messageForUnrecognizedStatusCode:urlres];
	}
	[rpcResponse release];
	tryToRegister_Caller = nil;
	[tryToRegister_Name release];
	tryToRegister_Name = nil;
}

- (void)login {	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/index", [presistent objectForKey:@"guid"]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	[RPCURLConnection sendAsyncRequest:request target:self selector:@selector(loginCallback:withObject:)];
	
}
- (void)loginCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {		
		XMLPersonParser* pparser = [[XMLPersonParser alloc] initWithData:rpcResponse.data];
		if (![pparser hasError]) {
			
			Group* g = [pparser getGroup];
			if (nil != g) {
				[myGroup release];
				myGroup = g;
			}
			[self updatePresistentWithPerson:[pparser getPerson]];
			[self updatePresistentWithMyGroup];

		}
		[rpcResponse release];
		[pparser release];
	}
}

/*********************** GROUP STUFF *****************************/
#pragma mark -


- (BOOL) startCreateGroup:(NSString*)name withDesc:(NSString*)desc target:(id)obj selector:(SEL)s {
	createGroupCallbackObj = obj;
	createGroupCallbackSel = s;
	
	if (myGroup != nil) {
		//what do we do when you're currently in a group? we have to leave it, but we'll let the server take care of that.
		[myGroup release];
		myGroup = nil;
	}

	Group* newG = [[Group alloc] init];
	NSString* gN = [[NSString alloc] initWithString:name];
	NSString* gD = [[NSString alloc] initWithString:desc];
	[newG setName:gN];
	[newG setDescription:gD];
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/create", [presistent objectForKey:@"guid"]];
	RPCPostRequest* request = [newG getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	[newG release];
	
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(callbackForCreateGroup:withObject:)];

	[urlString release];
	if (connection) {
		[[Test1AppDelegate sharedAppDelegate] showActivityViewer]; //Block the user
		return YES;
	}
	return NO;
}

- (void) callbackForCreateGroup:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	if (rpcResponse.response == nil) { //we have an error
	
		[self messageForCouldNotConnectToServer];	
		[createGroupCallbackObj performSelector:createGroupCallbackSel withObject:nil];
	
	} else if (rpcResponse.response.statusCode == 201) {
		
		XMLGroupParser* gparser = [[XMLGroupParser alloc] initWithData:rpcResponse.data];
		
		if (![gparser hasError]) {
			Group *g = [gparser getGroup];
			myGroup = [g copy];
			[self saveData];
			
			NSString *title= [[NSString alloc] initWithFormat:@"Created group %@!", g.name];
			NSString *message= [[NSString alloc] initWithString:@"You can now invite your friends to join, and save waypoints."];
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"Great!" otherButtonTitles:nil];
			[alert show];
			[alert release];
			[message release];		
			[title release];

			[createGroupCallbackObj performSelector:createGroupCallbackSel withObject:myGroup];
		} else {
			[self messageForParserError:gparser];
			[createGroupCallbackObj performSelector:createGroupCallbackSel withObject:nil];
		}
		 
		[gparser release];
		
	} else  {
		
		[self messageForUnrecognizedStatusCode:rpcResponse.response];
		[createGroupCallbackObj performSelector:createGroupCallbackSel withObject:nil];
	
	}
}

- (BOOL) startLeaveGroup:target:(id)obj selector:(SEL)s {
	leaveGroupCallbackObj = obj;
	leaveGroupCallbackSel = s;
	
	if (myGroup == nil)
		return YES;

	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/leave", [presistent objectForKey:@"guid"]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	if ([RPCURLConnection sendAsyncRequest:request target:self selector:@selector(leaveGroupCallback:withObject:)]) {
		[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
		return YES;
	}
	return NO;
}

- (void) leaveGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {		
		[myGroup release];
		myGroup = nil;
		[self saveData];
	}
	[leaveGroupCallbackObj performSelector:leaveGroupCallbackSel withObject:rpcResponse.response];	
}

- (BOOL) startJoinGroup:(int)group_id target:(id)obj selector:(SEL)s {
	joinGroupCallbackObj = obj;
	joinGroupCallbackSel = s;
	
	if (group_id < 0)
		return NO;
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/join/%d", [presistent objectForKey:@"guid"], group_id];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	if ([RPCURLConnection sendAsyncRequest:request target:self selector:@selector(joinGroupCallback:withObject:)]) {
		[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
		return YES;
	}
	return NO;
	
}
- (void) joinGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {		
		XMLGroupParser* gparser = [[XMLGroupParser alloc] initWithData:rpcResponse.data];
		if (![gparser hasError]) {
			Group *g = [gparser getGroup];
			myGroup = [g copy];
			[self saveData];
			[joinGroupCallbackObj performSelector:joinGroupCallbackSel withObject:rpcResponse.response];	
		} else {
			[self messageForParserError:gparser];
			[joinGroupCallbackObj performSelector:joinGroupCallbackSel withObject:nil];	
		}
	} else {
		[self messageForCouldNotConnectToServer];
		[joinGroupCallbackObj performSelector:joinGroupCallbackSel withObject:nil];	
	}
}

- (BOOL)isInGroup {
	return (nil != myGroup);
}

- (Group*) getMyGroup {
 	return [myGroup copy];
}

/*********************** FRIEND STUFF *****************************/
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

- (void)addWaypoint:(Waypoint *)wP {
	if (myGroup == nil)
		return;
	wP.group_id = myGroup.group_id;
	[locallyAddedWaypoints addObject:wP];
	[waypointQueue addObject:wP];
	[self flushWaypointQueue];
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
	return nil;	
}

- (int) getMessagesCount {
	return 3;
}
- (int) getUnreadMessagesCount {
	return 1;
}

- (BOOL)sendMessage:(Message*)msg {
	if ([self getUid] <= 0)
		return NO;
	[msg setSender_uid:[self getUid]];
	[outgoingMessagesQueue addObject:msg];
	[self flushOutgoingMessagesQueue];
	return YES;
}
- (int)unsentMessagesCount {
	return [outgoingMessagesQueue count];
}
- (NSArray*)getUnsentMessages {
	return [[NSArray alloc] initWithArray:outgoingMessagesQueue copyItems:NO];
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

	[positionQueue addObject:[[Position alloc] initWithCLLocation:newLocation]];
	
	[newLocation retain];
	[lastKnownLocation release];
	lastKnownLocation = newLocation;
	
	 //update views and shit
	[self flushPositionQueue];
}

- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
	// The location "unknown" error simply means the manager is currently unable to get the location.
	if ([error code] != kCLErrorLocationUnknown) {
		
	}
}
@end
