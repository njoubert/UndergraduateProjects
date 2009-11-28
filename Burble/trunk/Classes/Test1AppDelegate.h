//
//  Test1AppDelegate.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MapNavigationController.h"
#import "GroupiesNavigationController.h"
#import "MyGroupNavigationController.h"
#import "FeedNagivationController.h"
#import "MapViewController.h"
#import "FeedViewController.h"
#import "MyGroupViewController.h"
#import "GroupiesViewController.h"
#import "Person.h"


@interface Test1AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	UITabBarController *rootTabBarController;
	
	MapNavigationController *mapNavigationController;
	FeedNagivationController *feedNavigationController;
	MyGroupNavigationController *myGroupNavigationController;
	GroupiesNavigationController *groupiesNavigationController;
	
	MapViewController *mapViewController;
	//FeedViewController *feedViewController;
	MyGroupViewController *myGroupViewController;
	//GroupiesViewController *groupiesViewController;
	UIActivityIndicatorView *activityView;
}

+(Test1AppDelegate *) sharedAppDelegate;

-(void)hideActivityViewer;
-(void)showActivityViewer;
-(void)setUnreadMessageDisplay:(int)nr;

-(void)locatePositionOnMap:(Position*)pos;

@property (nonatomic, retain) IBOutlet UITabBarController *rootTabBarController;
@property (nonatomic, retain) IBOutlet MapNavigationController *mapNavigationController;
@property (nonatomic, retain) IBOutlet FeedNagivationController *feedNavigationController;
@property (nonatomic, retain) IBOutlet MyGroupNavigationController *myGroupNavigationController;
@property (nonatomic, retain) IBOutlet GroupiesNavigationController *groupiesNavigationController;
@property (nonatomic, retain) IBOutlet MapViewController *mapViewController;
//@property (nonatomic, retain) IBOutlet GroupiesNavigationController *feedViewController;
@property (nonatomic, retain) IBOutlet MyGroupViewController *myGroupViewController;
//@property (nonatomic, retain) IBOutlet GroupiesNavigationController *groupiesViewController;
@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

