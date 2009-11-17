//
//  BurbleDataManager.h
//  
// This is a singleton class that manages all the data for our application. It's the heart and soul of the app.
// 
//

#import <Foundation/Foundation.h>
#import "Person.h"

#define kPresistentFilename @"BurbleData.plist"

@interface BurbleDataManager : NSObject {
	NSString *currentDirectoryPath;					//The path where we store data. Set by init.
	BOOL bIsFirstLaunch;								//Indicates whether this is the app's first launch
	NSMutableDictionary *presistent;
}
+ (BurbleDataManager *) sharedDataManager;

@property (nonatomic, copy) NSString *currentDirectoryPath;

// ============= INTERNAL STATE MANAGEMENT

- (void)loadData;
- (void)saveData;

- (BOOL)isFirstLaunch;
- (BOOL)isRegistered;

- (BOOL)tryToRegister:(NSString*) name;

- (NSString*) getGUID;
- (NSString*) getName;

// ============= CONNECTION MANAGEMENT

// This checks that we're registered on the server (and create an account if it does not exist). 
// Doesn't actually change any internal state (unlike most login systems), rather it's a simple check that we're online.
- (BOOL)login;	

// ============= DATA CALLS

- (NSArray*) getFriends;

@end
