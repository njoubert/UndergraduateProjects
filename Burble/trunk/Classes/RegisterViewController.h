//
//  RegisterViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BurbleDataManager.h"
#import "Test1AppDelegate.h"

@interface RegisterViewController : UIViewController <UITextFieldDelegate, UIAlertViewDelegate> {
	UITextField *nameField;
	UIView *activityView;
}

@property (nonatomic, retain) IBOutlet UITextField *nameField;

-(IBAction) namePressed;

@end
