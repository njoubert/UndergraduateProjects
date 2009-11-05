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
@synthesize rootNavController;

- (void)applicationDidFinishLaunching:(UIApplication *)application {    

	
	/*
	 Here we make the rootNavController the visible controller when we launch.
	 */
	[window addSubview:rootNavController.view];
    [window makeKeyAndVisible];
}


- (void)dealloc {
	[rootNavController release];
    [window release];
    [super dealloc];
}


@end
