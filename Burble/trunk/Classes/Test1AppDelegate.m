//
//  Test1AppDelegate.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "Test1AppDelegate.h"

@implementation Test1AppDelegate

@synthesize window;
@synthesize rootTabBarController;
@synthesize groupiesNavigationController;
@synthesize myGroupNavigationController;

- (void)applicationDidFinishLaunching:(UIApplication *)application {    
	[window addSubview:rootTabBarController.view];	
    [window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application {
	//[self saveBlogData];
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	[dataManager saveData];
	
}

- (void)dealloc {
	[rootTabBarController release];
    [window release];	
    [super dealloc];
}


@end
