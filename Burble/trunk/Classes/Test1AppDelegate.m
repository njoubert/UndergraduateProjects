//
//  Test1AppDelegate.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "Test1AppDelegate.h"

@implementation Test1AppDelegate

static Test1AppDelegate *burbleApp = NULL;

@synthesize window;
@synthesize rootTabBarController;
@synthesize groupiesNavigationController;
@synthesize myGroupNavigationController;

- (id) init {
	if (!burbleApp) {
		burbleApp = [super init];
	}
	return burbleApp;
}

+ (Test1AppDelegate *)sharedAppDelegate {
	if (!burbleApp) {
		burbleApp = [[Test1AppDelegate alloc] init];
	}
	return burbleApp;
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
	activityView.alpha = 0.4;
	
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

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	//TODO: Do not do this here, you should do an actual spinner view and show the user without interrupting him.
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	[dataManager login];
	
	//If this is the first launch, we need to popup a "type your name bitch!" tab
	[window addSubview:rootTabBarController.view];	
    [window makeKeyAndVisible];
}

- (void)applicationWillTerminate:(UIApplication *)application {
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	[dataManager saveData];
	
}

- (void)dealloc {
	[rootTabBarController release];
    [window release];	
    [super dealloc];
}


@end
