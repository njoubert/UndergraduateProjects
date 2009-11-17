//
//  GroupiesViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FBConnect/FBConnect.h"
#import "GroupiesDetailViewController.h"
#import "BurbleDataManager.h"

@interface GroupiesViewController : UIViewController 
	<UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate>
{
	UIView *mainView;
	UIView *addFriendView;
	
	UITableView *table;
	UISearchBar *search;
	NSArray *people;
	NSDictionary *allNames;
	NSMutableDictionary *names;
	NSMutableArray *keys;
	BurbleDataManager *dataManager;
	
	NSArray *list; //will contain list of groupies, represents all the names to be listed on the buttons
	GroupiesDetailViewController *childController;

}

@property (nonatomic, retain) IBOutlet UIView *addFriendView;
@property (nonatomic, retain) IBOutlet UIView *mainView;

@property (nonatomic, retain) IBOutlet UITableView *table;
@property (nonatomic, retain) IBOutlet UISearchBar *search;
@property (nonatomic, retain) NSDictionary *allNames;
@property (nonatomic, retain) NSMutableDictionary *names;
@property (nonatomic, retain) NSMutableArray *keys;
@property (nonatomic, retain) NSArray *people;


-(void)resetSearch;
-(void)handleSearchForTerm:(NSString *)searchTerm;

-(IBAction)addFriends:(id)sender;
-(IBAction)importFriends:(id)sender;
-(IBAction)backToMain:(id)sender;

-(IBAction)alphaSort;
-(IBAction)distSort;

@end
