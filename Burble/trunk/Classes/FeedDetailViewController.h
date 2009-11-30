//
//  GroupiesDetailViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import "ComposeMessageViewController.h"
#import "Message.h"

@interface FeedDetailViewController : UIViewController {
	
	UILabel *senderLabel;
	UITextView *messageText;
	Message* message;
	Person* sender;
	UIBarButtonItem *replyButton;
	ComposeMessageViewController *composeView;

	
}

@property (nonatomic, retain) Message* message;
@property (nonatomic, retain) Person* sender;
@property (nonatomic, retain) IBOutlet UITextView *messageText;
@property (nonatomic, retain) IBOutlet UILabel *senderLabel;

-(IBAction)replyTo:(id)s;

@end
