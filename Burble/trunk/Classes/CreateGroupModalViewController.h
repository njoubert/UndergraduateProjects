//
//  CreateGroupModalViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface CreateGroupModalViewController : UIViewController <UITextFieldDelegate, UIAlertViewDelegate> {
	UITextField *nameField;
}

@property (nonatomic, retain) IBOutlet UITextField *nameField;
@end
