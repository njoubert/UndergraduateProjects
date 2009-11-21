//
//  MyGroupViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Group.h"
#import "BurbleDataManager.h"


@interface MyGroupViewController : UIViewController <UITextFieldDelegate>{
	UITextField *groupTextField;
	UITextField *groupMsgTextField;
	UITextField *groupMsgRecTextField;
	NSString *groupString;
	UIView *myGroupView;
	UIView *createGroupView;
	UIView *inviteToGroupView;
	UIView *messageGroupView;
	UILabel *groupLabel;
	BurbleDataManager *dataManager;
	Group *myGroup;
}

@property (nonatomic, retain) IBOutlet UITextField *groupTextField;
@property (nonatomic, retain) IBOutlet UITextField *groupMsgTextField;
@property (nonatomic, retain) IBOutlet UITextField *groupMsgRecTextField;
@property (nonatomic, copy) NSString *groupString;
@property (nonatomic, retain) IBOutlet UIView *myGroupView;
@property (nonatomic, retain) IBOutlet UIView *createGroupView;
@property (nonatomic, retain) IBOutlet UIView *inviteToGroupView;
@property (nonatomic, retain) IBOutlet UIView *messageGroupView;
@property (nonatomic, retain) IBOutlet UILabel *groupLabel;
@property (nonatomic, retain) Group *myGroup;


-(IBAction)leaveGroup:(id)sender;
-(IBAction)createGroup:(id)sender;			//calls out to server
-(void)groupCreated:(Group*)returnValue;	//receives callback from server
-(IBAction)backToGroup:(id)sender;
-(IBAction)inviteToGroup:(id)sender;
-(IBAction)messageGroup:(id)sender;
-(IBAction)goToMyGroupView;


-(IBAction)inviteIdOneFUCKYEA;



@end
