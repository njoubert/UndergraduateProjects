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
#import "Test1AppDelegate.h"
#import "RegisterViewController.h"

#define kPresistentFilename @"BurbleData.plist"

#define kBaseUrlStr @"http://burble.njoubert.com/iphone/"
//#define kBaseUrlStr @"http://localhost:3000/iphone/"


@interface BurbleDataManager : NSObject <CLLocationManagerDelegate> {
	NSString *currentDirectoryPath;					//The path where we store data. Set by init.
	BOOL bIsFirstLaunch;								//Indicates whether this is the app's first launch
	NSMutableDictionary *presistent;
	
	NSURL *baseUrl;
	
	id tryToRegister_Caller;
	NSString* tryToRegister_Name;
	
	CLLocationManager *myLocationManager;
	CLLocation *lastKnownLocation;
	
	NSMutableArray* locallyAddedWaypoints;
	
	Group* myG;
}
+ (BurbleDataManager *) sharedDataManager;

@property (nonatomic, copy) NSString *currentDirectoryPath;

// ============= INTERNAL STATE MANAGEMENT

- (void)loadData;
- (void)saveData;

- (BOOL)isFirstLaunch;
- (BOOL)isRegistered;


// ============= DATA CALLS for INTERNAL STATE DATA

- (NSString*) getGUID;
- (NSString*) getName;
- (NSString*) getFirstName;

// ============= DATA CALLS for DEVICE DATA (Managed internally)

- (CLLocation*) getLocation; //This returns what the device thinks is our location (not necessarily the latest Position as sent to the server)

// ============= DATA CALLS for SERVER MANAGED DATA (Cached locally)


//puts up a spinner that blocks the user from doing anything until we get a receiveTryToRegisterCallback
- (BOOL)startTryToRegister:(NSString*)name caller:(id)obj;
- (void)receiveTryToRegisterCallback:(id)a1 withValue:(id)a2;

//runs in the background and tries to update your account info, make sure you're all good to go.
- (void)login;
- (void)loginCallback:(id)a1 withValue:(id)a2;

- (BOOL)isInGroup;
- (Group*) getMyGroup;
- (BOOL) createGroup:(Group*)g;

- (int) getFriendsCount;
- (NSArray*) getFriends;

- (int) getMessagesCount;
- (int) getUnreadMessagesCount;
- (NSArray*) getMessages;
- (NSArray*) getUnreadMessages;

- (NSString*) getNextWaypointName;
- (NSString*) getNextWaypointDesc;
- (void)addWaypoint:(Waypoint*) wP;
- (int)getWaypointCount;
- (NSArray*)getWaypoints;



@end
