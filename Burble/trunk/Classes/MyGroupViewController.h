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
	UITextView *groupMsgText;
	NSString *groupString;
	NSString *groupMsgString;
	UIView *myGroupView;
	UIView *createGroupView;
	UIView *inviteToGroupView;
	UIView *messageGroupView;
	UILabel *groupLabel;
	UILabel *groupMsgLabel;
	BurbleDataManager *dataManager;
	Group *myGroup;
}

@property (nonatomic, retain) IBOutlet UITextField *groupTextField;
@property (nonatomic, retain) IBOutlet UITextView *groupMsgText;
@property (nonatomic, copy) NSString *groupString;
@property (nonatomic, copy) NSString *groupMsgString;
@property (nonatomic, retain) IBOutlet UIView *myGroupView;
@property (nonatomic, retain) IBOutlet UIView *createGroupView;
@property (nonatomic, retain) IBOutlet UIView *inviteToGroupView;
@property (nonatomic, retain) IBOutlet UIView *messageGroupView;
@property (nonatomic, retain) IBOutlet UILabel *groupLabel;
@property (nonatomic, retain) IBOutlet UILabel *groupMsgLabel;
@property (nonatomic, retain) Group *myGroup;


-(IBAction)leaveGroup:(id)sender;
-(IBAction)createGroup:(id)sender;			//calls out to server
-(void)groupCreated:(Group*)returnValue;	//receives callback from server
-(IBAction)backToGroup:(id)sender;
-(IBAction)sendMsgToGroup:(id)sender;
-(IBAction)inviteToGroup:(id)sender;
-(IBAction)messageGroup:(id)sender;
-(IBAction)goToMyGroupView;


-(IBAction)inviteIdOneFUCKYEA;



@end
