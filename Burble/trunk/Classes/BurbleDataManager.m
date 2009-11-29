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
#import "XMLMessagesParser.h"
#import "XMLWaypointsParser.h"
#import "XMLFriendsParser.h"
#import "Util.h"

@interface TimerQueueAdapter : NSObject {
	SEL selToCall;
	id objToCallWith;
}
@property (assign, nonatomic) SEL selToCall;
@property (assign, nonatomic) id objToCallWith;
@end

@implementation TimerQueueAdapter
@synthesize selToCall, objToCallWith;
@end

@implementation BurbleDataManager
static BurbleDataManager *sharedDataManager;
@synthesize currentDirectoryPath, baseUrl, hasLastMapRegion, lastMapRegion, lastMapType, lastKnownLocation, groupsHistory;

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
		hasLastMapRegion = NO;
		
		CLLocationCoordinate2D locCor;
		locCor.longitude = 0;
		locCor.latitude = 0;
		NSDate *d = [NSDate date];
		lastKnownLocation = [[CLLocation alloc] initWithCoordinate:locCor altitude:0 horizontalAccuracy:-1 verticalAccuracy:-1 timestamp:d];
		
		myLocationManager = [[CLLocationManager alloc] init];
		myLocationManager.delegate = self;
		myLocationManager.desiredAccuracy = [[NSNumber numberWithDouble:kCLLocationAccuracyBest] doubleValue];
		myLocationManager.distanceFilter  = [[NSNumber numberWithDouble:kMinDistanceForUpdate] doubleValue];
		[myLocationManager startUpdatingLocation];
		
		groupWaypoints = [[NSMutableArray alloc] init];
		groupMembers = [[NSMutableArray alloc] init];
		
		callbackForLoginTarget = nil;
		callbackForLoginSelector = nil;
		joinGroupCallbackSel = nil;
		joinGroupCallbackObj = nil;
		createGroupCallbackObj = nil;
		createGroupCallbackSel = nil;
		leaveGroupCallbackObj = nil;
		leaveGroupCallbackSel = nil;
		myFBUID = 0;
		
		downloadedMessagesTarget = nil;
		downloadedMessagesSelector = nil;
		downloadedWaypointsTarget = nil;
		downloadedWaypointsSelector = nil;	
		downloadedFriendsTarget = nil;
		downloadedFriendsSelector = nil;
		downloadedGroupMembersTarget = nil;
		downloadedGroupMembersSelector = nil;
		
		//QUEUES:
		waypointQueue = [[NSMutableArray alloc] init];
		positionQueue = [[NSMutableArray alloc] init];
		outgoingMessagesQueue = [[NSMutableArray alloc] init];
		outgoingRequestsQueue = [[NSMutableArray alloc] init];
		groupsHistory = [[NSMutableArray alloc] init];
		
		//MESSAGES:
		allMessages = [[NSMutableArray alloc] init];	//This is a sorted-by-arrival-time messages
		
		//FRIENDS:
		allFriends = [[NSMutableArray alloc] init];
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

////////// WAYPOINTS CACHE

- (void)deleteWaypointsCache {
	NSString *waypointsFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kWaypointsCacheFilename];
	[[NSFileManager defaultManager] removeItemAtPath:waypointsFilePath error:NULL];
}

- (void)loadWaypointsCache {
	NSString *waypointsFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kWaypointsCacheFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:waypointsFilePath]) {
		NSMutableArray* loadedCache = [NSKeyedUnarchiver unarchiveObjectWithFile:waypointsFilePath];
		NSLog(@"Unarchived waypoints file.");
		[groupWaypoints release];
		groupWaypoints = [loadedCache retain];
	}
}

- (void)saveWaypointsCache {
	[self deleteWaypointsCache];
	NSString *waypointsFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kWaypointsCacheFilename];
	[NSKeyedArchiver archiveRootObject:groupWaypoints toFile:waypointsFilePath];
}

/////// GROUP HISTORY

- (void)deleteGroupHistoryCache {
	NSString *groupHistoryFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kGroupHistoryCacheFilename];
	[[NSFileManager defaultManager] removeItemAtPath:groupHistoryFilePath error:NULL];
}

- (void)loadGroupHistoryCache {
	NSString *groupHistoryFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kGroupHistoryCacheFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:groupHistoryFilePath]) {
		NSMutableArray* loadedCache = [NSKeyedUnarchiver unarchiveObjectWithFile:groupHistoryFilePath];
		[groupsHistory release];
		groupsHistory = [loadedCache retain];
	}
}

- (void)saveGroupHistoryCache {
	[self deleteGroupHistoryCache];
	NSString *groupHistoryFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kGroupHistoryCacheFilename];
	[NSKeyedArchiver archiveRootObject:groupsHistory toFile:groupHistoryFilePath];
}

//This checks that the presistent dictionary contains all the appropriate keys, and saves it.
//Assumes that presistent exists, and sets presistent to the value.
- (void)checkAndSavePresistentFile {
	if (nil == [presistent objectForKey:kPresistKeyGUID]) {
		NSString* guid = [[UIDevice currentDevice] uniqueIdentifier];
		[presistent setValue:guid forKey:kPresistKeyGUID];
	}
	if ([presistent objectForKey:@"myGroup.group_id"] != nil && [[presistent objectForKey:@"myGroup.group_id"] isEqualToString:@"0"]) {
		[presistent removeObjectForKey:@"myGroup.group_id"];
	}
	[presistent writeToFile:[[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename] atomically:YES];
}

- (void)presistMapState:(MKCoordinateRegion)region withType:(MKMapType)type {
	hasLastMapRegion = YES;
	lastMapRegion = region;
	lastMapType = type;
	[presistent removeObjectForKey:kPresistMapLat];
	[presistent removeObjectForKey:kPresistMapLon];
	[presistent removeObjectForKey:kPresistMapLatDelta];
	[presistent removeObjectForKey:kPresistMapLonDelta];
	[presistent removeObjectForKey:kPresistMapType];
	NSString *a1 = [[NSString stringWithFormat:@"%lf", region.center.latitude] retain];
	NSString *a2 = [[NSString stringWithFormat:@"%lf", region.center.longitude] retain];
	NSString *a3 = [[NSString stringWithFormat:@"%lf", region.span.latitudeDelta] retain];
	NSString *a4 = [[NSString stringWithFormat:@"%lf", region.span.longitudeDelta] retain];
	NSString *a5 = [NSString stringWithFormat:@"%d", type];
	[presistent setObject:a1 forKey:kPresistMapLat];
	[presistent setObject:a2 forKey:kPresistMapLon];
	[presistent setObject:a3 forKey:kPresistMapLatDelta];
	[presistent setObject:a4 forKey:kPresistMapLonDelta];
	[presistent setObject:a5 forKey:kPresistMapType];
}

//Runs after init, before anything else
- (void)loadData {
	//attempt to load PresistentData
	NSString *presistentFilePath = [[self currentDirectoryPath] stringByAppendingPathComponent:kPresistentFilename];
	if ([[NSFileManager defaultManager] fileExistsAtPath:presistentFilePath]) {
		presistent = [[NSMutableDictionary alloc] initWithContentsOfFile:presistentFilePath];
		if (nil != [presistent objectForKey:@"myGroup.group_id"] && 
				nil != [presistent objectForKey:@"myGroup.name"]) {
			NSLog(@"loading group id from disk");
			myGroup = [[Group alloc] init];
			myGroup.group_id = [[presistent objectForKey:@"myGroup.group_id"] intValue];
			myGroup.name = [[NSString alloc] initWithString:[presistent objectForKey:@"myGroup.name"]];
			if (nil != [presistent objectForKey:@"myGroup.description"])
				myGroup.description = [[NSString alloc] initWithString:[presistent objectForKey:@"myGroup.description"]];
		} else {
			myGroup = nil;
		}
		if (nil != [presistent objectForKey:kPresistMapLat]) {
			hasLastMapRegion = YES;
			lastMapRegion.center.latitude = [[presistent objectForKey:kPresistMapLat] doubleValue];
			lastMapRegion.center.longitude = [[presistent objectForKey:kPresistMapLon] doubleValue];
			lastMapRegion.span.latitudeDelta = [[presistent objectForKey:kPresistMapLatDelta] doubleValue];
			lastMapRegion.span.longitudeDelta = [[presistent objectForKey:kPresistMapLonDelta] doubleValue];
			lastMapType = (MKMapType) [[presistent objectForKey:kPresistMapType] intValue];
		} else {
			hasLastMapRegion = NO;
		}
		if (nil != [presistent objectForKey:@"myFBUID"])
			myFBUID = [[presistent objectForKey:@"myFBUID"] longLongValue];
		bIsFirstLaunch = FALSE;
	} else {
		presistent = [[NSMutableDictionary alloc] init];
		[self checkAndSavePresistentFile];
		bIsFirstLaunch = TRUE;
	}
	[self loadWaypointsCache];
	[self loadGroupHistoryCache];
}

- (void)cacheFBUID:(FBUID)fbuid {
	myFBUID = fbuid;
	[presistent setObject:[[NSNumber numberWithLongLong:fbuid] retain] forKey:@"myFBUID"];
	[self checkAndSavePresistentFile];
}

//Run right before app terminates
- (void)saveData {
	[self updatePresistentWithMyGroup];
	[self checkAndSavePresistentFile];
	[self saveWaypointsCache];
	[self saveGroupHistoryCache];
	
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
- (Person*) getMe {
	Person* p = [[Person alloc] init];
	p.position = [[Position alloc] initWithCLLocation:[self getLocation]];
	p.name = [self getName];
	p.uid = [self getUid];
	p.guid = [self getGUID];
	return p;
	
}
- (NSString*) getGUID {
	return [presistent objectForKey:kPresistKeyGUID];
}
-(int) getUid {
	return [[presistent objectForKey:@"uid"] intValue];;
}
- (FBUID) getFBUID {
	return myFBUID;
}
- (NSString*) getName {
	return [presistent objectForKey:@"name"];
}
- (NSString*) getFirstName {
	NSArray *firstWords = [[[self getName] componentsSeparatedByString:@" "] subarrayWithRange:NSMakeRange(0,1)];
	NSString *result = [firstWords componentsJoinedByString:@" "];
	return result;
}
- (void)updatePresistRemovePerson {
	[presistent removeObjectForKey:@"uid"];
	[presistent removeObjectForKey:@"name"];
	[presistent removeObjectForKey:@"email"];
	[presistent removeObjectForKey:@"number"];
	[self saveData];
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
	return [lastKnownLocation copy];
}


/*
 ================================================================================
						QUEUE MANAGEMENT  for SERVER DATA
 ================================================================================ 
 */
#pragma mark -
#pragma mark Queue Management for Server Data --- general queues

-(void)fireQueueEvent:(NSTimer*)timer {
	TimerQueueAdapter* adapter = [timer userInfo];
	if ([self respondsToSelector:adapter.selToCall]) {
		[self performSelector:adapter.selToCall withObject:adapter.objToCallWith];
	}
	[adapter release];
}

// Flushes any queue using the given selector to send each item. Does nice things like
// slow requests down to do sets of concurrent requests.
// One of two scenarios:Will send an element in the queue one by one to the given selector, OR will send a NSTimer
- (void)flushQueue:(NSMutableArray*)queue usingSender:(SEL)s {
	@synchronized(queue) {
		if (![self isRegistered])
			return;
		if ([queue count] == 0)
			return;
		id element;
		int numRequests = 1;
		
		while ([queue count] > 0) {
			element = [queue lastObject];
			[queue removeLastObject];
			if (element == nil)
				continue;
			if (numRequests <= kNumOfConcurrentRequestsFromQueue) {
				[self performSelector:s withObject:element];
				numRequests++;
			} else {
				//we set up a timer to send a sync request.
				//we catch the requests coming back and check if they have succeeded. if not we add back to the queue.
				TimerQueueAdapter* adapter = [[TimerQueueAdapter alloc] init];
				adapter.selToCall = s;
				adapter.objToCallWith = element;
				[NSTimer scheduledTimerWithTimeInterval:kTimeBetweenRequests*(numRequests/kNumOfConcurrentRequestsFromQueue) 
												 target:self
											   selector:@selector(fireQueueEvent:)
											   userInfo:adapter 
												repeats:NO];
				numRequests++;
			}
		}
	}
}

/************** Positions ****************/
#pragma mark Queue Management for Server Data --- positions

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
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/position", [presistent objectForKey:kPresistKeyGUID]];
	RPCPostRequest* request = [p getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendPositionToServerCallback:withObject:) withUserObject:p]) {
		[positionQueue addObject:p];
	}
}

//Starts the process of sending all the positions in the queue to the server
- (void)flushPositionQueue { [self flushQueue:positionQueue usingSender:@selector(sendPositionToServer:)]; }

/************** Waypoints ****************/
#pragma mark Queue Management for Server Data --- waypoints

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
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/add_waypoint_for_group/%d", [presistent objectForKey:kPresistKeyGUID], wp.group_id];
	RPCPostRequest* request = [wp getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendWaypointToServerCallback:withObject:) withUserObject:wp]) {
		[waypointQueue addObject:wp];
	}
}

// Starts the actual process of sending waypoints to the server, using the three helpers above.
- (void)flushWaypointQueue { [self flushQueue:waypointQueue usingSender:@selector(sendWaypointToServer:)]; }

/************** Outgoing Messages ****************/
#pragma mark Queue Management for Server Data --- outgoing messages

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
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/messages/send_msg", [presistent objectForKey:kPresistKeyGUID]];
	RPCPostRequest* request = [m getPostRequestToMethod:urlString withBaseUrl:baseUrl];
	if (nil == [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(sendMessageToServerCallback:withObject:) withUserObject:m]) {
		[outgoingMessagesQueue addObject:m];
	}
}

// Starts the actual process of sending waypoints to the server, using the three helpers above.
- (void)flushOutgoingMessagesQueue { [self flushQueue:outgoingMessagesQueue usingSender:@selector(sendMessageToServer:)]; }

/************** Just Send the Request Queue ****************/
#pragma mark Queue Management for Server Data --- general RPCrequests

- (void) sendRequestToServerCallback:(RPCURLResponse*)rpcResponse withObject:(id)o { 
	if (rpcResponse == nil || rpcResponse.response.statusCode == 500) {
		[outgoingRequestsQueue addObject:o];
	}
	return; 

}
- (void)sendRequestToServer:(RPCPostRequest*) r {
	if (nil == [RPCURLConnection sendAsyncRequest:r target:self selector:@selector(sendRequestToServerCallback:withObject:) withUserObject:r]) {
		[outgoingRequestsQueue addObject:r];
	}
}
// Starts the actual process of sending shit to the server
- (void)flushOutgoingQueue { [self flushQueue:outgoingRequestsQueue usingSender:@selector(sendRequestToServer:)]; }

/*
 ================================================================================
					QUEUE MANAGEMENT for PUSHED-FROM-SERVER DATA
 ================================================================================ 
 */
#pragma mark -
#pragma mark Queue Management for Pushed-From-Server Data

-(void)downloadMessagesCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {
		XMLMessagesParser* mparser = [[XMLMessagesParser alloc] initWithData:rpcResponse.data];
		if (![mparser hasError]) {
			NSArray* rcvdMsgs = [[mparser getMessages] retain];
			NSEnumerator* enumerator = [rcvdMsgs objectEnumerator];
			Message* m;
			NSLog(@"got %d messages", [rcvdMsgs count]);
			while (m = [enumerator nextObject]) {
				if ([allMessages containsObject:m]) {
					Message* alreadyM = [allMessages objectAtIndex:[allMessages indexOfObject:m]];
					if (m.read && !alreadyM.read) {
						//fukkin server thinks we havent read this but fuck we already have, goddaaaam!
						//we should tell the server that this is fucking read already, dayamn!
						[self markMessageAsRead:m];
						[m release];
					}
				} else {
					//fukkin yay message received! Now we can decide what to do with this.
					[allMessages addObject:m];
					if ([m.type isEqualToString:kMessageTypeGroupInvite] && !m.read) {
						NSString *message= [[NSString alloc] initWithFormat:@"Invitation to group %@. Check your feed to respond.", m.group.name];
						UIAlertView *alert = [[UIAlertView alloc]
											  initWithTitle: @"Group Invite Received!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
						[alert show];
						[alert release];
						[message release];		
					} else if ([m.type isEqualToString:kMessageTypeRoutingRequest] && !m.read) {
						
					}
				}
			}
			[rcvdMsgs release];
			//now we sort the allMessages array
			
			NSSortDescriptor *sortDescriptor;
			sortDescriptor = [[[NSSortDescriptor alloc] initWithKey:@"sent_time"
														  ascending:NO] autorelease];
			NSArray *sortDescriptors = [NSArray arrayWithObject:sortDescriptor];
			NSArray *sortedArray;
			sortedArray = [allMessages sortedArrayUsingDescriptors:sortDescriptors];
			[allMessages release];
			allMessages = [[NSMutableArray alloc] initWithArray:sortedArray];
			
			[[Test1AppDelegate sharedAppDelegate] setUnreadMessageDisplay:[self getUnreadMessagesCount]];
		}
		[mparser release];
	}
	if (downloadedMessagesTarget != nil && [downloadedMessagesTarget respondsToSelector:downloadedMessagesSelector]) {
		[downloadedMessagesTarget performSelector:downloadedMessagesSelector];
		downloadedMessagesTarget = nil;
		downloadedMessagesSelector = nil;
	}
}

//This will attempt to pull new messages from the server. Should be called periodically
-(BOOL)startDownloadMessages {
	if (![self isRegistered])
		return NO;
	NSLog(@"start downloading messages");
	NSString *urlString = [NSString stringWithFormat:@"%@/messages/index", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [NSURL URLWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest *request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	BOOL success = (nil != [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(downloadMessagesCallback:withObject:)]);
	[request release];
	return success;
}
-(BOOL)startDownloadMessagesAndCall:(id)target withSelector:(SEL)s {
	downloadedMessagesTarget = target;
	downloadedMessagesSelector = s;
	return [self startDownloadMessages];
}

-(void)downloadWaypointsCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {
		
		XMLWaypointsParser* wparser = [[XMLWaypointsParser alloc] initWithData:rpcResponse.data];
		if (![wparser hasError]) {
			NSArray* wpts = [[wparser getWaypoints] retain];
			NSEnumerator* enumerator = [wpts objectEnumerator];
			Waypoint* w;
			NSLog(@"got %d waypoints", [wpts count]);
			while (w = [enumerator nextObject]) {
				if (w.person_id == [self getUid]) {
					//search to check whether there
					//if this is one of our waypoints (person-id matches) check if the name matches, then set the uid
					if (![groupWaypoints containsObject:w]) {
						[groupWaypoints addObject:w];
					} else {
						Waypoint* wM = [groupWaypoints objectAtIndex:[groupWaypoints indexOfObject:w]];
						wM.person_id = [self getUid];
					}
				} else {
					if (![groupWaypoints containsObject:w]) {
						[groupWaypoints addObject:w];
					}
				}
			}
			[wpts release];
		}
		[wparser release];						 
	}
	if (downloadedWaypointsTarget != nil && [downloadedWaypointsTarget respondsToSelector:downloadedWaypointsSelector]) {
		[downloadedWaypointsTarget performSelector:downloadedWaypointsSelector];
		downloadedWaypointsTarget = nil;
		downloadedWaypointsSelector = nil;
	}
}


-(BOOL)startDownloadWaypoints {
	if (![self isRegistered])
		return NO;
	if (![self isInGroup])
		return NO;
	NSString *urlString = [NSString stringWithFormat:@"%@/groups/waypoints", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [NSURL URLWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest *request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	BOOL success = (nil != [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(downloadWaypointsCallback:withObject:)]);
	[request release];
	return success;
}
-(BOOL)startDownloadWaypointsAndCall:(id)target withSelector:(SEL)s {
	downloadedWaypointsTarget = target;
	downloadedWaypointsSelector = s;
	return [self startDownloadWaypoints];
}

-(void)downloadFriendsCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {
		XMLFriendsParser* fparser = [[XMLFriendsParser alloc] initWithData:rpcResponse.data];
		if (![fparser hasError]) {
			NSMutableArray* friends = [[fparser getFriends] retain];
			[allFriends release];
			allFriends = friends;
			NSLog(@"got friends array with %d entries", [friends count]);
//			NSEnumerator* enumerator = [friends objectEnumerator];
//			Person* f;
//			while (f = [enumerator nextObject]) {
//				[allFriends addObject:f];
//				NSLog(@"friend name: %@", f.name);
//				if ([allFriends containsObject:f]) {
//				
//				} else {
//					[allFriends addObject:f];
//				}
//			}
//			[friends release];
		}
		[fparser release];						 
	}
	if (downloadedFriendsTarget != nil && [downloadedFriendsTarget respondsToSelector:downloadedFriendsSelector]) {
		[downloadedFriendsTarget performSelector:downloadedFriendsSelector];
		downloadedFriendsTarget = nil;
		downloadedFriendsSelector = nil;
	}
	
}


-(BOOL)startDownloadFriends {
	if (![self isRegistered])
		return NO;
	NSString *urlString = [NSString stringWithFormat:@"%@/friends/index", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [NSURL URLWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest *request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	BOOL success = (nil != [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(downloadFriendsCallback:withObject:)]);
	[request release];
	return success;
}
-(BOOL)startDownloadFriendsAndCall:(id)target withSelector:(SEL)s {
	downloadedFriendsTarget = target;
	downloadedFriendsSelector = s;
	return [self startDownloadFriends];
}

-(void)downloadGroupMembersCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {
		XMLFriendsParser* fparser = [[XMLFriendsParser alloc] initWithData:rpcResponse.data];
		if (![fparser hasError]) {
			NSMutableArray* newGroupMembers = [[fparser getFriends] retain];
			if (groupMembers == nil)
				groupMembers = [[NSMutableArray alloc] initWithCapacity:[newGroupMembers count]];
			NSLog(@"got a group members array with %d entries", [newGroupMembers count]);
			
			for (Person* p in newGroupMembers) {
				if ([groupMembers containsObject:p]) {
					Person* original = [groupMembers objectAtIndex:[groupMembers indexOfObject:p]];
					[original updateWith:p];
				} else {
					[groupMembers addObject:p];
				}
			}
			NSMutableArray *toDelete = [NSMutableArray array];
			for (Person* p in groupMembers) {			//delete waypoints that no longer exist
				if (![newGroupMembers containsObject:p]) {
					[toDelete addObject: p];
				}
			}
			[groupMembers removeObjectsInArray:toDelete];

			//insert new ones if we do not have it already. else update position.
		}
		[fparser release];						 
	}
	if (downloadedGroupMembersTarget != nil && [downloadedGroupMembersTarget respondsToSelector:downloadedGroupMembersSelector]) {
		[downloadedGroupMembersTarget performSelector:downloadedGroupMembersSelector];
		downloadedGroupMembersTarget = nil;
		downloadedGroupMembersSelector = nil;
	}
	
}


-(BOOL)startDownloadGroupMembers {
	if (![self isRegistered])
		return NO;
	if (![self isInGroup])
		return NO;
	NSString *urlString = [NSString stringWithFormat:@"%@/groups/members", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [NSURL URLWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest *request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	BOOL success = (nil != [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(downloadGroupMembersCallback:withObject:)]);
	return success;
}
-(BOOL)startDownloadGroupMembersAndCall:(id)target withSelector:(SEL)s {
	downloadedGroupMembersTarget = target;
	downloadedGroupMembersSelector = s;
	return [self startDownloadGroupMembers];
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
	
	Person* _me = [[Person alloc] init];
	[_me setName:name];
	[_me setGuid:[presistent objectForKey:kPresistKeyGUID]];	
	
	NSString *urlString = [NSString stringWithFormat:@"%@/person/create", [presistent objectForKey:kPresistKeyGUID]];
	RPCPostRequest* request = [_me getPostRequestToMethod:urlString withBaseUrl:baseUrl];

	tryToRegister_Name = [name retain];
	
	[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(receiveTryToRegisterCallback:withObject:)];
	[request release];
	if (connection)
		return YES;
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
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
	[[BurbleDataManager sharedDataManager] startDownloadMessages];
	[[BurbleDataManager sharedDataManager] startDownloadWaypoints];
	[rpcResponse release];
	tryToRegister_Caller = nil;
	[tryToRegister_Name release];
	tryToRegister_Name = nil;
}

- (void)login {	
	NSString *urlString = [NSString stringWithFormat:@"%@/person/index", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [NSURL URLWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	[RPCURLConnection sendAsyncRequest:request target:self selector:@selector(loginCallback:withObject:)];
	[request release];
	
}
-(void)loginWithCallback:(id)target selector:(SEL)s {
	callbackForLoginTarget = target;
	callbackForLoginSelector = s;
	[self login];
}
- (void)loginCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	BOOL success = YES; //so that if we could not get a response that is cool...
	if (rpcResponse.response != nil) {
		if (rpcResponse.response.statusCode == 200) {		
			XMLPersonParser* pparser = [[XMLPersonParser alloc] initWithData:rpcResponse.data];
			if (![pparser hasError]) {
				Group* g = [pparser getGroup];
				if (nil != g) {
					[myGroup release];
					myGroup = g;
				}
				NSLog(@"successfull login!");
				success = YES;
				[self updatePresistentWithPerson:[pparser getPerson]];
				[self updatePresistentWithMyGroup];
			}
			[rpcResponse release];
			[pparser release];
		} else if (rpcResponse.response.statusCode == 500) {
			//remove the presist value of us, and we need to fukkin register shoit!
			[self updatePresistRemovePerson];
			success = NO;
		}
	}
	
	 if (callbackForLoginTarget != nil && [callbackForLoginTarget respondsToSelector:callbackForLoginSelector]) {
		 [callbackForLoginTarget performSelector:callbackForLoginSelector withObject:[NSNumber numberWithBool:success]];
		 callbackForLoginTarget = nil;
		 callbackForLoginSelector = nil;
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
	[self clearWaypoints];
	Group* newG = [[Group alloc] init];
	NSString* gN = [[NSString alloc] initWithString:name];
	NSString* gD = [[NSString alloc] initWithString:desc];
	[newG setName:gN];
	[newG setDescription:gD];
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/create", [presistent objectForKey:kPresistKeyGUID]];
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
			if (![groupsHistory containsObject:myGroup]) //add it to our history queue
				[groupsHistory insertObject:myGroup atIndex:0];
			[self saveData];
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

- (BOOL) startLeaveGroupWithTarget:(id)obj selector:(SEL)s {
	leaveGroupCallbackObj = obj;
	leaveGroupCallbackSel = s;
	
	if (myGroup == nil)
		return YES;

	//kill all waypoints you saved so far. They're on the server anyways.
	[self clearWaypoints];
	
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/leave", [presistent objectForKey:kPresistKeyGUID]];
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
		myGroup = nil;
		[groupMembers release];
		groupMembers = nil;
		[self saveData];
	}
	[myGroup release];
	myGroup = nil;
	[leaveGroupCallbackObj performSelector:leaveGroupCallbackSel withObject:rpcResponse.response];	
}

- (BOOL) startJoinGroup:(int)group_id target:(id)obj selector:(SEL)s {
	joinGroupCallbackObj = obj;
	joinGroupCallbackSel = s;
	
	if (group_id < 0)
		return NO;
	[self clearWaypoints];
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/groups/join/%d", [presistent objectForKey:kPresistKeyGUID], group_id];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	if ([RPCURLConnection sendAsyncRequest:request target:self selector:@selector(joinGroupCallback:withObject:)]) {
		[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
		return YES;
	}
	return NO;
	
}
- (void) joinGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	if (rpcResponse.response != nil && rpcResponse.response.statusCode == 200) {		
		XMLGroupParser* gparser = [[XMLGroupParser alloc] initWithData:rpcResponse.data];
		if (![gparser hasError]) {
			Group *g = [gparser getGroup];
			myGroup = [g copy];
			if (![groupsHistory containsObject:myGroup]) //add it to our history queue
				[groupsHistory insertObject:myGroup atIndex:0];
			[self saveData];
			[self startDownloadWaypoints];
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
	return (nil != myGroup) && (myGroup.group_id > 0);
}
- (BOOL)isInMyGroup:(Person*)p {
	return (p.group != nil) && (p.group.group_id == myGroup.group_id);
}

- (Group*) getMyGroup {
 	return myGroup;
}
- (NSArray*) getGroupMembers {
	if ([self isInGroup]) {
		return groupMembers;
	} else {
		return nil;
	}
}
- (int) getGroupMembersCount {
	if (groupMembers != nil)
		return [groupMembers count];
	return 0;
}
- (Person*) getGroupMember:(int)uid {
	Person* member = nil;
	NSEnumerator *enumerator = [groupMembers objectEnumerator];
	while (member = [enumerator nextObject]) {
		if (member.uid == uid) 
			return member;
	}
	return nil;
}


/*********************** FRIEND STUFF *****************************/
#pragma mark -

- (NSArray*) getFriends {
	return allFriends;
}
- (Person*) getFriend:(int)uid {
	Person* friend = nil;
	NSEnumerator *enumerator = [allFriends objectEnumerator];
	while (friend = [enumerator nextObject]) {
		if (friend.uid == uid)
			return friend;
	}
	return nil;
}
- (int) getFriendsCount {
	if (allFriends != nil)
		return [allFriends count];
	return 0;
}

- (void)addWaypoint:(Waypoint *)wP {
	if (myGroup == nil)
		return;
	wP.group_id = myGroup.group_id;
	wP.person_id = [self getUid];
	[groupWaypoints addObject:wP];
	[waypointQueue addObject:wP];
	[self flushWaypointQueue];
}



- (void)clearWaypoints {
	[self flushWaypointQueue];
	[groupWaypoints release];								
	groupWaypoints = [[NSMutableArray alloc] init];
}

- (int)getWaypointCount {
	return [groupWaypoints count];
}

- (NSArray*) getWaypoints {
	return groupWaypoints;
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
	return allMessages;
}

- (int) getMessagesCount {
	return [allMessages count];
}
- (int) getUnreadMessagesCount {
	int unreadCount = 0;
	NSEnumerator* enumerator = [allMessages objectEnumerator];
	Message* m;
	while (m = [enumerator nextObject]) {
		if (!m.read)
			unreadCount += 1;
	}
	return unreadCount;
}

- (BOOL)sendMessage:(Message*)msg {
	if ([self getUid] <= 0)
		return NO;
	[msg setSender_uid:[self getUid]];
	[msg setSent_time:[[NSDate date] retain]];
	[msg setISentThis:YES];
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
- (void) markMessageAsRead:(Message*)m {
	if (m == nil)
		return;
	m.read = YES;
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/messages/mark/%d", [presistent objectForKey:kPresistKeyGUID], m.uid];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	[outgoingRequestsQueue addObject:request];
	[self flushOutgoingQueue];
	[[Test1AppDelegate sharedAppDelegate] setUnreadMessageDisplay:[self getUnreadMessagesCount]];

}

/*
 ================================================================================
			DELEGATE METHODS	
 ================================================================================ 
 */

/*
 ================================================================================
 LOCATION STUFF	
 ================================================================================ 
 */
#pragma mark -
#pragma mark Location Stuff


-(double) calculateDistanceFromMe:(const CLLocation*)other {
	if (lastKnownLocation == nil || other == nil)
		return -1;
	
	return [lastKnownLocation getDistanceFrom:other];
	
}

-(NSArray*) sortByDistanceFromMe:(NSArray*)arr {
	NSArray *sortedArray; 
	sortedArray = [arr sortedArrayUsingFunction:distanceSortByGetLocation context:NULL];
	return sortedArray;
}

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



/*
 ================================================================================
 DELEGATE METHODS	
 ================================================================================ 
 */
#pragma mark -
#pragma mark Facebook Connect


-(void)scannedFBFriendsCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	
	if (rpcResponse != nil && rpcResponse.response.statusCode == 200) {
		[self startDownloadFriends];
		
		NSString* uidStr = [[NSString alloc] initWithData:rpcResponse.data encoding:NSUTF8StringEncoding];
		int friendCount = [uidStr intValue];
		
		NSString *title= [[NSString alloc] initWithString:@"Scanned Facebook Friends!"];
		NSString *message= [[NSString alloc] initWithFormat:@"We scanned your %d friends and connected you to %d Burble users. Your facebook ID has also been associated, so that your facebook friends can find you.", _fbFriendsCount, friendCount];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		[title release];
	} else {
		NSString *title= [[NSString alloc] initWithString:@"Server Error!"];
		NSString *message= [[NSString alloc] initWithString:@"Something went wrong on the server side. Sorry, we messed up!"];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		[title release];
	}
	
}
- (void)getFBFriendsList {
	NSString* fql = [NSString stringWithFormat:@"SELECT uid, name from user WHERE uid IN (SELECT uid2 FROM friend WHERE uid1=%lld)", myFBUID];
	NSDictionary* params = [NSDictionary dictionaryWithObject:fql forKey:@"query"];
	[[FBRequest requestWithSession:[self fbGetSession] delegate:self] call:@"facebook.friends.get" params:params]; 
	NSLog(@"sent fb username request");
} 
- (void)request:(FBRequest*)request didLoad:(id)result { 
	NSArray* users = result; 
	NSMutableArray* alluids = [[NSMutableArray alloc] initWithCapacity:[users count]];
	_fbFriendsCount = [users count];
	NSLog(@"how many did we get: %d", _fbFriendsCount);
	for (NSInteger i=0; i<[users count];i++) {
		NSDictionary *user = [users objectAtIndex:i];
		NSString* uid = [user objectForKey:@"uid"];
		[alluids addObject:uid];
	}
	
	//JSON THIS SHIT!! JSON >>> XML! FUCK XML! I HATE XML! JSON RULES THE WORLD! XML IS JSON'S BITCH!
	//READ IT AND WEEP BITCHES!
	NSMutableString* friendsIdsString = [[NSMutableString alloc] init];
	[friendsIdsString appendString:@"["];
	for (int i = 0; i < [alluids count]; i++) {
		if (i+1 == [alluids count]) {
			[friendsIdsString appendFormat:@"%@", [alluids objectAtIndex:i]];
		} else {
			[friendsIdsString appendFormat:@"%@,",[alluids objectAtIndex:i]];
		}
	}
	[friendsIdsString appendString:@"]"];
	
	//fukkin post this shit to the server, we're doing it live!!
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/scan_fb_friends", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* brequest = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	RPCPostData* pData = [[RPCPostData alloc] init];
	[pData appendValue:friendsIdsString forKey:@"friendsfbids"];
	[brequest setHTTPBodyPostData:pData];
	
	[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:brequest target:self selector:@selector(scannedFBFriendsCallback:withObject:)];
	[urlString release];
	[pData release];
	if (!connection) {
		[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
		NSString *title= [[NSString alloc] initWithString:@"Error!"];
		NSString *message= [[NSString alloc] initWithString:@"We could not create a connection to the server, this is strange!"];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"OK!" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		[title release];
	}

}

-(void)fbShowLoginBox {
	FBSession* session = [self fbGetSession];
	FBLoginDialog* dialog = [[[FBLoginDialog alloc] initWithSession:session] autorelease]; 
	[dialog show];
}

-(FBSession*)fbGetSession {
	if (_fbsession == nil)
		_fbsession = [[FBSession sessionForApplication:kfbAPIKey secret:kfbSecretKey delegate:self] retain];
	return _fbsession;
}	


-(BOOL)startAssociateFBUID {
	NSString *urlString = [[NSString alloc] initWithFormat:@"%@/person/set_fbuid", [presistent objectForKey:kPresistKeyGUID]];
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];
	RPCPostData* pData = [[RPCPostData alloc] init];
	[pData appendValue:[[NSNumber numberWithLongLong:myFBUID] stringValue] forKey:@"fbuid"];
	[request setHTTPBodyPostData:pData];
	
	[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
	RPCURLConnection *connection = [RPCURLConnection sendAsyncRequest:request target:self selector:@selector(associateFBUIDCallback:withObject:)];
	[urlString release];
	[pData release];
	if (connection)
		return YES;
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	return NO;
	
}
-(void)associateFBUIDCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
	[self getFBFriendsList];
}
//delegate method for facebook login
-(void)session:(FBSession*)session didLogin:(FBUID)uid {
	[self cacheFBUID:uid];
	[self checkAndSavePresistentFile];
	[self startAssociateFBUID];
}

@end

NSInteger distanceSortByGetLocation(id num1, id num2, void *context) {
	
	double d1 = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:[num1 getLocation]];
	double d2 = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:[num2 getLocation]];
		
    if (d1 < d2)
        return NSOrderedAscending;
    else if (d1 > d2)
        return NSOrderedDescending;
	return NSOrderedSame;
	
}

