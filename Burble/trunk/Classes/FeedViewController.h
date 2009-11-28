//
//  FeedViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Test1AppDelegate.h"
#import "FeedDetailViewController.h"
#import "Message.h"

#define kSenderValueTag		1
#define kContentsValueTag	2
#define kAgeValueTag		3
	

@interface FeedViewController : UIViewController
	<UITableViewDataSource, UITableViewDelegate>
{
	Message* _currentMessage;
	NSArray *messages;
	UITableView *table;
	FeedDetailViewController *childView;
	
	UIBarButtonItem *composeButton;
	UIBarButtonItem *refreshButton;
}

@property (nonatomic, retain) NSArray *messages;
@property (nonatomic, retain) IBOutlet UITableView *table;

@end
