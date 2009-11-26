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
#import "MyGroupViewController.h"
#import "Person.h"

//This is the subview for each individual Groupie
//Will contain more subviews for:
//current group membership, distance away, locate page on map, message page, invite to group page (grayed)

@interface GroupiesDetailViewController : UIViewController {
	UITableView *table;
	NSArray *functions;
	
	Person* person;
	
	//create a new map view
	MapViewController *map;
	MyGroupViewController *invite;
	
}

@property (nonatomic, retain) Person* person;
@property (nonatomic, retain) IBOutlet UITableView *table;
@property (nonatomic, retain) NSArray *functions;


@end
