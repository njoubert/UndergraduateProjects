//
//  BurbleDataManager.h
//  
// This is a singleton class that manages all the data for our application. It's the heart and soul of the app.
// 
//

#import <Foundation/Foundation.h>
#import "Person.h"
#import "Waypoint.h"
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>

#define kPresistentFilename @"BurbleData.plist"

@interface BurbleDataManager : NSObject <CLLocationManagerDelegate> {
	NSString *currentDirectoryPath;					//The path where we store data. Set by init.
	BOOL bIsFirstLaunch;								//Indicates whether this is the app's first launch
	NSMutableDictionary *presistent;
	
	CLLocationManager *myLocationManager;
	CLLocation *lastKnownLocation;
	
	NSMutableArray* locallyAddedWaypoints;
}
+ (BurbleDataManager *) sharedDataManager;

@property (nonatomic, copy) NSString *currentDirectoryPath;

// ============= INTERNAL STATE MANAGEMENT

- (void)loadData;
- (void)saveData;

- (BOOL)isFirstLaunch;
- (BOOL)isRegistered;

- (BOOL)tryToRegister:(NSString*) name;

// ============= CONNECTION MANAGEMENT

// This checks that we're registered on the server (and create an account if it does not exist). 
// Doesn't actually change any internal state (unlike most login systems), rather it's a simple check that we're online.
- (BOOL)login;	

// ============= DATA CALLS for INTERNAL STATE DATA

- (NSString*) getGUID;
- (NSString*) getName;
- (NSString*) getFirstName;

// ============= DATA CALLS for DEVICE DATA (Managed internally)

- (CLLocation*) getLocation; //This returns what the device thinks is our location (not necessarily the latest Position as sent to the server)

// ============= DATA CALLS for SERVER MANAGED DATA (Cached locally)

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
