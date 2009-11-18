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
#import "Test1AppDelegate.h"
#import "BurbleDataManager.h"

#import "ImportFriendsViewController.h"

//This is the root view for the Groupies navigation controller

@interface GroupiesViewController : UIViewController 
	<UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate>
{
	//These switch off between main Groupies page to importing FB friends page
	UIView *mainView;
	UIView *addFriendView;
	
	UITableView *table;
	UISearchBar *search;
	
	//Contains all People objects
	NSArray *people;
	
	//These were used for search, but they need to be fixed??
	NSDictionary *allNames;
	NSMutableDictionary *names;
	NSMutableArray *keys;
	
	BurbleDataManager *dataManager;
	GroupiesDetailViewController *childController;
	ImportFriendsViewController *import;
	
	//For search??
	NSMutableArray *namesForSearch;
	NSMutableArray *nameCopies;
	BOOL searching;

}

@property (nonatomic, retain) IBOutlet UIView *addFriendView;
@property (nonatomic, retain) IBOutlet UIView *mainView;

@property (nonatomic, retain) IBOutlet UITableView *table;
@property (nonatomic, retain) IBOutlet UISearchBar *search;

@property (nonatomic, retain) NSArray *people;

@property (nonatomic, retain) NSDictionary *allNames;
@property (nonatomic, retain) NSMutableDictionary *names;
@property (nonatomic, retain) NSMutableArray *keys;

@property (nonatomic, retain) NSMutableArray *namesForSearch;
@property (nonatomic, retain) NSMutableArray *nameCopies;


-(void)resetSearch;
-(void)handleSearchForTerm:(NSString *)searchTerm;

//Methods for adding FB friends
-(IBAction)addFriends:(id)sender;
-(IBAction)importFriends:(id)sender;

//Sort methods to be implemented
-(IBAction)alphaSort;
-(IBAction)distSort;

@end
