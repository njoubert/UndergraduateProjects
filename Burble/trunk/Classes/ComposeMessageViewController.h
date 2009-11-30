//
//  ComposeMessageViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SelectRecipients.h"

@interface ComposeMessageViewController : UIViewController <UITextFieldDelegate, UIAlertViewDelegate> {
	SelectRecipients *selectRecipientsVC;

	NSMutableArray *_possibleRecipients;		//all the people you can possibly select
	NSMutableArray *_recipients;				//the selected recipients
	
	UILabel *toLabel;
	UITextView *messageTextView;
	UIBarButtonItem *sendButton;
	UIBarButtonItem *cancelButton;
	
	id _target;
	SEL _selector;
}
@property (nonatomic, retain) IBOutlet UILabel *toLabel;
@property (nonatomic, retain) IBOutlet UITextView *messageTextView;


-(id)initWithListOfPeople:(NSArray*)toSelectFrom selected:(NSMutableArray*)selectedPeople withCallbackTarget:(id)t selector:(SEL)sel;

- (IBAction)showSelectRecipients;

@end
