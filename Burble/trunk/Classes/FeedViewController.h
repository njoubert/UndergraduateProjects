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
#define kTypeValueTag		2

@interface FeedViewController : UIViewController
	<UITableViewDataSource, UITableViewDelegate>
{
	Message* _currentMessage;
	NSArray *messages;
	UITableView *table;
	FeedDetailViewController *childView;
}

@property (nonatomic, retain) NSArray *messages;
@property (nonatomic, retain) IBOutlet UITableView *table;

@end
