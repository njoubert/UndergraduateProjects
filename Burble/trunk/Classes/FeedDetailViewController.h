//
//  GroupiesDetailViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#import "Message.h"

@interface FeedDetailViewController : UIViewController {
	NSArray *functions;
	
	UILabel *msgText;
	Message* message;
}

@property (nonatomic, retain) Message* message;
@property (nonatomic, retain) IBOutlet UILabel *msgText;
@property (nonatomic, retain) NSArray *functions;

-(IBAction)replyTo:(id)sender;

@end
