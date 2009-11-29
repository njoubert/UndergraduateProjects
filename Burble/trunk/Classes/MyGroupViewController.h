//
//  MyGroupViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Group.h"
//#import "GroupiesDetailViewController.h"
@class GroupiesDetailViewController;

@interface MyGroupViewController : UIViewController <UITextFieldDelegate, UITableViewDataSource, UITableViewDelegate, UIAlertViewDelegate> {
	NSArray* members;
	NSArray* waypoints;
	
	
	UIBarButtonItem *refreshButton; //on the left
	UIBarButtonItem *inviteButton;
	UIBarButtonItem *createGroupButton;
	UIBarButtonItem *composeButton;
	UIBarButtonItem *leaveButton;
	UISegmentedControl *waypointsOrGroupControl;
	
	UIAlertView *leaveGroupAlertView;
	UIAlertView *createGroupAlertView;
	
	UIView *contentsView;
	UITableView *waypointsTableView;
	UITableView *membersTableView;
	GroupiesDetailViewController *personDetailController;
}
@property (nonatomic, retain) IBOutlet UIBarButtonItem *leaveButton;
@property (nonatomic, retain) IBOutlet UISegmentedControl *waypointsOrGroupControl;
@property (nonatomic, retain) IBOutlet UITableView *waypointsTableView;
@property (nonatomic, retain) IBOutlet UITableView *membersTableView;
@property (nonatomic, retain) IBOutlet UIView *contentsView;

-(IBAction)leaveButtonPressed;
-(IBAction)displaySelectorButtonPressed;
-(IBAction)composeButtonPressed;

@end
