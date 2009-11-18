//
//  GroupiesDetailViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#import "MapViewController.h" 
#import "FeedViewController.h"
#import "MyGroupViewController.h"

//This is the subview for each individual Groupie
//Will contain more subviews for:
//current group membership, distance away, locate page on map, message page, invite to group page (grayed)

@interface GroupiesDetailViewController : UIViewController {
	UILabel *label;
	NSString *message;
	
	UITableView *table;
	NSArray *functions;
	
	MapViewController *map;
	FeedViewController *msg;
	MyGroupViewController *invite;
	
}

@property (nonatomic, retain) IBOutlet UILabel *label;
@property (nonatomic, retain) NSString *message;

@property (nonatomic, retain) IBOutlet UITableView *table;
@property (nonatomic, retain) NSArray *functions;


@end
