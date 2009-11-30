//
//  MyGroupViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Group.h"
#import "SelectRecipients.h"
#import "ComposeMessageViewController.h"

//#import "GroupiesDetailViewController.h"
@class GroupiesDetailViewController;

@interface MyGroupViewController : UIViewController <UITextFieldDelegate, UITableViewDataSource, UITableViewDelegate, UIAlertViewDelegate> {
	NSArray* members;
	NSArray* waypoints;
	
	UIToolbar *actionsToolbar;
	UIBarButtonItem *refreshButton; //on the left
	UIBarButtonItem *inviteButton;
	UIBarButtonItem *createGroupButton;
	UIBarButtonItem *composeButton;
	UIBarButtonItem *leaveButton;
	UISegmentedControl *waypointsOrGroupControl;
	
	UIAlertView *leaveGroupAlertView;
	UIAlertView *createGroupAlertView;
	UIAlertView *joinGroupAlertView;
	
	UIView *contentsView;
	UIView *totalView;
	UITableView *waypointsTableView;
	UITableView *membersTableView;
	UITableView *groupsTableView;
	SelectRecipients *inviteSelectionView;
	ComposeMessageViewController *composeView;
	
	Group* _toJoin;
	
	
	GroupiesDetailViewController *personDetailController;
}
@property (nonatomic, retain) IBOutlet UIBarButtonItem *leaveButton;
@property (nonatomic, retain) IBOutlet UISegmentedControl *waypointsOrGroupControl;
@property (nonatomic, retain) IBOutlet UITableView *waypointsTableView;
@property (nonatomic, retain) IBOutlet UITableView *membersTableView;
@property (nonatomic, retain) IBOutlet UITableView *groupsTableView;
@property (nonatomic, retain) IBOutlet UIView *contentsView;
@property (nonatomic, retain) IBOutlet UIView *totalView;
@property (nonatomic, retain) IBOutlet UIToolbar *actionsToolbar;

-(IBAction)leaveButtonPressed;
-(IBAction)displaySelectorButtonPressed;
-(IBAction)composeButtonPressed;

@end
