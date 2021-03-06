//
//  Test1AppDelegate.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "Test1AppDelegate.h"

#import "BurbleDataManager.h"

@implementation Test1AppDelegate

static Test1AppDelegate *burbleApp = NULL;

@synthesize window;
@synthesize rootTabBarController;
@synthesize groupiesNavigationController;
@synthesize myGroupNavigationController;
@synthesize feedNavigationController;
@synthesize mapNavigationController;
@synthesize mapViewController, myGroupViewController;

- (id) init {
	if (!burbleApp) {
		burbleApp = [super init];
		regularUpdatesTimer = [NSTimer scheduledTimerWithTimeInterval:kFrequencyOfRegularUpdates target:self selector:@selector(regularUpdateTimerCall) userInfo:nil repeats:YES];
	}
	return burbleApp;
}

+ (Test1AppDelegate *)sharedAppDelegate {
	if (!burbleApp) {
		burbleApp = [[Test1AppDelegate alloc] init];
	}
	return burbleApp;
}

// This gets regularly called to poll the server.
-(void)regularUpdateTimerCall {
	
	[[BurbleDataManager sharedDataManager] startDownloadMessages];
	[[BurbleDataManager sharedDataManager] startDownloadWaypoints];	
	[[BurbleDataManager sharedDataManager] startDownloadGroupMembers];
	
	[mapViewController refreshView];
	//update the feed?
	//[feedViewController refreshView];
	[myGroupViewController refreshView];
}

-(void)hideActivityViewer {
	[[[activityView subviews] objectAtIndex:0] stopAnimating];
	[activityView removeFromSuperview];
	activityView = nil;
}

//blanks out the whole screen and displays a spinning gear.
-(void)showActivityViewer {
	[activityView release];
	activityView = [[UIView alloc] initWithFrame: CGRectMake(0, 0, window.bounds.size.width, window.bounds.size.height)];
	activityView.backgroundColor = [UIColor blackColor];
	activityView.alpha = 0.8;
	
	UIActivityIndicatorView *activityWheel = [[UIActivityIndicatorView alloc] initWithFrame: CGRectMake(window.bounds.size.width / 2 - 12, window.bounds.size.height / 2 - 12, 24, 24)];
	activityWheel.activityIndicatorViewStyle = UIActivityIndicatorViewStyleWhite;
	activityWheel.autoresizingMask = (UIViewAutoresizingFlexibleLeftMargin |
									  UIViewAutoresizingFlexibleRightMargin |
									  UIViewAutoresizingFlexibleTopMargin |
									  UIViewAutoresizingFlexibleBottomMargin);
	[activityView addSubview:activityWheel];
	[activityWheel release];
	[window addSubview: activityView];
	[activityView release];
	
	[[[activityView subviews] objectAtIndex:0] startAnimating];
}

-(void)setUnreadMessageDisplay:(int)nr {
	if (nr == 0) {
		feedNavigationController.tabBarItem.badgeValue = nil;
	} else {
		feedNavigationController.tabBarItem.badgeValue = [NSString stringWithFormat:@"%d", nr];
	}
}
-(void)locateMeOnMap {
	rootTabBarController.selectedViewController = mapNavigationController;
	[mapViewController locateMeOnMap];	
}
-(void)locatePersonOnMap:(Person*)person {
	if (person.uid == [[BurbleDataManager sharedDataManager] getUid]) {
		[self locateMeOnMap];
	} else {
		rootTabBarController.selectedViewController = mapNavigationController;
		[mapViewController locatePersonOnMap:person];
	}
}

-(void)locateWaypointOnMap:(Waypoint*)waypt {
	rootTabBarController.selectedViewController = mapNavigationController;
	[mapViewController locateWaypointOnMap:waypt];
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	[window addSubview:rootTabBarController.view];	
	[self setUnreadMessageDisplay:0];

	[[BurbleDataManager sharedDataManager] startDownloadFriends];
	[[BurbleDataManager sharedDataManager] startDownloadMessages];
	[[BurbleDataManager sharedDataManager] startDownloadWaypoints];

	[window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application {
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	[dataManager presistMapState:mapViewController.myMap.region withType:mapViewController.myMap.mapType];
	[dataManager saveData];
	
}

- (void)dealloc {
	[rootTabBarController release];
    [window release];	
    [super dealloc];
}


@end
