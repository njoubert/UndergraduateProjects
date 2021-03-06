//
//  BurbleDataManager.h
//  
// This is a singleton class that manages all the data for our application. It's the heart and soul of the app.
// 
//

#import <Foundation/Foundation.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>
#import "Person.h"
#import "Message.h"
#import "Waypoint.h"
#import "Group.h"
#import "Message.h"
#import "Position.h"
#import "RPCURLConnection.h"
#import "RPCURLResponse.h"
#import "Test1AppDelegate.h"
#import "XMLEventParser.h"
#import "FBConnect/FBConnect.h"

#define kPresistMapLat				@"map.region.lat"
#define kPresistMapLon				@"map.region.lon"
#define kPresistMapLatDelta			@"map.region.latDelta"
#define kPresistMapLonDelta			@"map.region.lonDelta"
#define kPresistMapType				@"map.type"

#define kPresistKeyGUID	@"guid"

#define kfbAPIKey		@"e26ae2fc668e7ed1bc8f0322d2ec122d"
#define kfbSecretKey	@"8aeda7f2bc85ebdb0d7f39689ee14962"

#define kNumOfConcurrentRequestsFromQueue	2
#define kTimeBetweenRequests				2.0
#define kMinDistanceForUpdate				100
#define pollMessageFrequency				5000

#define kBaseUrlStr @"http://burble.njoubert.com/iphone/"
//#define kBaseUrlStr @"http://localhost:3000/iphone/"

#define kPresistentFilename			@"BurbleData.plist"
#define kWaypointsCacheFilename		@"waypoints"
#define kGroupHistoryCacheFilename	@"GroupHistory"


@interface BurbleDataManager : NSObject <CLLocationManagerDelegate, FBSessionDelegate, FBRequestDelegate> {
	NSString *currentDirectoryPath;					//The path where we store data. Set by init.
	BOOL bIsFirstLaunch;								//Indicates whether this is the app's first launch
	NSMutableDictionary *presistent;
	NSURL *baseUrl;
	
	BOOL hasLastMapRegion;
	MKCoordinateRegion lastMapRegion;
	MKMapType lastMapType;
	
	Person* me;
	
	//LOGIN STUFF:
	id tryToRegister_Caller;
	NSString* tryToRegister_Name;
	//NSMutableDictionary* viewsToRefreshOnLogins;
	
	CLLocationManager *myLocationManager;
	CLLocation *lastKnownLocation;
	NSMutableArray* groupWaypoints;
	
	//QUEUES:
	NSMutableArray* waypointQueue;
	NSMutableArray* positionQueue;
	NSMutableArray* outgoingMessagesQueue;
	NSMutableArray* outgoingRequestsQueue; //for stuff you dont care about what comes back
	
	//MESSAGES:
	NSMutableArray* allMessages;	//This is a sorted-by-arrival-time messages
	
	//FRIENDS:
	NSMutableArray* allFriends;
	
	//GROUP STUFF:
	Group* myGroup;
	NSMutableArray* groupMembers;
	NSMutableArray* groupsHistory;

	//FACEBOOK:
	FBUID myFBUID;
	FBSession* _fbsession;
	int _fbFriendsCount;
	
	//CALLBACKS:
	id callbackForLoginTarget;
	SEL callbackForLoginSelector;
	id createGroupCallbackObj;
	SEL createGroupCallbackSel;
	id leaveGroupCallbackObj;
	SEL leaveGroupCallbackSel;
	id joinGroupCallbackObj;
	SEL joinGroupCallbackSel;
	id  downloadedMessagesTarget;
	SEL downloadedMessagesSelector;
	id  downloadedWaypointsTarget;
	SEL downloadedWaypointsSelector;	
	id  downloadedFriendsTarget;
	SEL downloadedFriendsSelector;	
	id  downloadedGroupMembersTarget;
	SEL downloadedGroupMembersSelector;	
}
+ (BurbleDataManager *) sharedDataManager;

@property (nonatomic, copy) NSString *currentDirectoryPath;
@property (nonatomic, retain) NSURL *baseUrl;
@property (readonly) BOOL hasLastMapRegion;
@property (readonly) MKCoordinateRegion lastMapRegion;
@property (readonly) MKMapType lastMapType;
@property (readonly) CLLocation *lastKnownLocation;
@property (readonly) NSMutableArray *groupsHistory;

// ============= INTERNAL STATE MANAGEMENT

- (void)presistMapState:(MKCoordinateRegion)region withType:(MKMapType)type;
- (void)loadData;
- (void)saveData;

- (BOOL)isFirstLaunch;
- (BOOL)isRegistered;

// ============= DATA CALLS for INTERNAL STATE DATA

- (Person*) copyOfMe;
- (NSString*) getGUID;
- (int) getUid;
- (NSString*) getName;
- (NSString*) getFirstName;
- (FBUID) getFBUID;

//Updates the internal presistent data with the given Person
- (void) updatePresistentWithPerson:(Person*)p;
- (void) updatePresistentWithMyGroup;

// ============= DATA CALLS for DEVICE DATA (Managed internally)

- (CLLocation*) getLocation; //This returns what the device thinks is our location (not necessarily the latest Position as sent to the server)

// ============= QUEUE MANAGEMENT  for SERVER DATA

- (void)flushPositionQueue;
- (void)flushWaypointQueue;
- (void)flushOutgoingMessagesQueue;

// =============  QUEUE MANAGEMENT for PUSHED-FROM-SERVER DATA

//This will attempt to pull new messages from the server. Should be called periodically
-(BOOL)startDownloadMessages;
-(BOOL)startDownloadMessagesAndCall:(id)target withSelector:(SEL)s;
-(BOOL)startDownloadWaypoints;
-(BOOL)startDownloadWaypointsAndCall:(id)target withSelector:(SEL)s;
-(BOOL)startDownloadFriends;
-(BOOL)startDownloadFriendsAndCall:(id)target withSelector:(SEL)s;
-(BOOL)startDownloadGroupMembers;						//We should call this frequently and update shit!
-(BOOL)startDownloadGroupMembersAndCall:(id)target withSelector:(SEL)s;

// ============= DATA CALLS for SERVER MANAGED DATA (Cached locally)

- (void)messageForCouldNotConnectToServer;
- (void)messageForUnrecognizedStatusCode:(NSHTTPURLResponse*)res;
- (void)messageForParserError:(XMLEventParser*)ps;

//puts up a spinner that blocks the user from doing anything until we get a receiveTryToRegisterCallback
- (BOOL)startTryToRegister:(NSString*)name caller:(id)obj;
- (void)receiveTryToRegisterCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj;

//runs in the background and tries to update your account info, make sure you're all good to go.
- (void)login;
-(void)loginWithCallback:(id)target selector:(SEL)s;
//- (void)loginCallback:(id)a1 withValue:(id)a2;

//will call selector with group or error indicating success.
- (BOOL) startCreateGroup:(NSString*)name withDesc:(NSString*)desc target:(id)obj selector:(SEL)s;		
- (void) createGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj;
- (BOOL) startLeaveGroupWithTarget:(id)obj selector:(SEL)s;
- (void) leaveGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj;
- (BOOL) startJoinGroup:(int)group_id target:(id)obj selector:(SEL)s;
- (void) joinGroupCallback:(RPCURLResponse*)rpcResponse withObject:(id)userObj;

- (int) getFriendsCount;
- (NSArray*) getFriends;
- (Person*) getFriend:(int)uid;

- (NSMutableArray*) copyOfAllPeople;
- (Person*) getPerson:(int)uid;

- (BOOL)isInGroup;
- (BOOL)isInMyGroup:(Person*)p;
- (int) getGroupMembersCount;
- (NSArray*) getGroupMembers;
- (Person*) getGroupMember:(int)uid;
- (Group*) getMyGroup;

- (NSString*) getNextWaypointName;
- (NSString*) getNextWaypointDesc;
- (void)addWaypoint:(Waypoint*) wP;
- (void)clearWaypoints;
- (int)getWaypointCount;
- (NSArray*)getWaypoints;

- (NSArray*) getMessages;		//returns a list of Message* objects
- (int) getMessagesCount;
- (int) getUnreadMessagesCount;
- (void) markMessageAsRead:(Message*)m;

- (BOOL)sendMessage:(Message*)msg; //given a message and a list of uids we add it to the queue. You only set message and type
- (int)unsentMessagesCount;
- (NSArray*)getUnsentMessages;



// =============  LOCATION DATA

-(void)sendMyPosition;
-(double) calculateDistanceFromMe:(const CLLocation*)other;
-(NSArray*) sortByDistanceFromMe:(NSArray*)arr;

// =============  FACEBOOK INTEGRATION

-(void)fbShowLoginBox;
-(FBSession*)fbGetSession;

@end

NSInteger distanceSortByGetLocation(id num1, id num2, void *context);
