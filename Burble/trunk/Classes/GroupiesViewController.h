//
//  GroupiesViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FBConnect/FBConnect.h"
#import "GroupiesDetailViewController.h"
#import "Test1AppDelegate.h"
#import "ImportFriendsViewController.h"

//This is the root view for the Groupies navigation controller

@interface GroupiesViewController : UIViewController 
	<UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate>
{
	//These switch off between main Groupies page to importing FB friends page
	UIView *mainView;
	UIView *addFriendView;
	
	UITableView *table;

	UIBarButtonItem *addFriendsButton;
	
	//Contains all People objects
	NSArray *people;
	
	UIView *noFriendsNotificationView;
	
	GroupiesDetailViewController *childController;
	ImportFriendsViewController *import;

}

@property (nonatomic, retain) IBOutlet UIView *addFriendView;
@property (nonatomic, retain) IBOutlet UIView *mainView;
@property (nonatomic, retain) IBOutlet UITableView *table;
@property (nonatomic, retain) IBOutlet UIView *noFriendsNotificationView;

@property (nonatomic, retain) NSArray *people;


//Methods for adding FB friends
//Only simulates the add
-(IBAction)addFriends:(id)sender;
-(IBAction)importFriends:(id)sender;

//Sort methods 
//MUST BE IMPLEMENTED
-(IBAction)alphaSort;
-(IBAction)distSort;

//Search methods
//MUST BE IMPLEMENTED

@end
