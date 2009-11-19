//
//  FeedViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Test1AppDelegate.h"
#import "BurbleDataManager.h"


@interface FeedViewController : UIViewController {
	NSArray *messages;
	
	UITableView *table;
	
	BurbleDataManager *dataManager;
}

@property (nonatomic, retain) NSArray *messages;
@property (nonatomic, retain) IBOutlet UITableView *table;

-(IBAction)buttonPressed;

@end
