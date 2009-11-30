//
//  GroupiesViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GroupiesViewController.h"
#import "NSDictionary-MutableDeepCopy.h"
#import "GroupiesDetailViewController.h"
#import "Test1AppDelegate.h"
#import "BurbleDataManager.h"
#import "ImportFriendsViewController.h"

@implementation GroupiesViewController

@synthesize addFriendView;
@synthesize mainView;
@synthesize table;
@synthesize noFriendsNotificationView;
@synthesize people;

//For old search methods
#pragma mark -
#pragma mark Custom Methods

//REWRITE THESE to search the array
-(void)resetSearch {

}

-(void)handleSearchForTerm:(NSString *)searchTerm {

}


//For importing FB friends

-(IBAction)addFriends: (id)sender {
	if (import == nil) 
		import = [[ImportFriendsViewController alloc] initWithNibName:@"ImportFriendsViewController" bundle:nil];
	//childController.message = test; //this doesn't really do anything right now
	import.title = @"Import Groupies";
	[self.navigationController pushViewController:import animated:YES];
}


//Sort methods to be implemented
-(IBAction)alphaSort {
}

-(IBAction)distSort {
}

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/
- (void)refreshViewData {
	if (people != nil)
		[people release];
	people = [[[BurbleDataManager sharedDataManager] getFriends] retain]; //this is an array of people object
	[table reloadData];
}
- (void)downloadAndRefreshViewData {
	[self refreshViewData];
	[[BurbleDataManager sharedDataManager] startDownloadFriendsAndCall:self withSelector:@selector(refreshViewData)];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"Groupies";
	addFriendsButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(addFriends:)];
	self.navigationItem.rightBarButtonItem = addFriendsButton;
	refreshButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(downloadAndRefreshViewData)];
	self.navigationItem.leftBarButtonItem = refreshButton;

	[super viewDidLoad];
}
- (void)viewDidAppear:(BOOL)animated {
	if ([[BurbleDataManager sharedDataManager] getFBUID] == 0 && [[BurbleDataManager sharedDataManager] getFriendsCount] == 0)
		[self.view addSubview:noFriendsNotificationView];
	//[self downloadAndRefreshViewData];
	[self refreshViewData];
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	self.table = nil;
	
	self.people = nil;
	childController = nil;
	import = nil;
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[addFriendView release];
	[mainView release];
	
	[table release];
	
	[people release];
		
	[childController release];
	[import release];
	
    [super dealloc];
}

#pragma mark -
#pragma mark Table View Data Source Methods

//This draws our list table for all of our Groupies
- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [people count];
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *PersonCell = @"PersonCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:PersonCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:PersonCell] autorelease];
	}
	NSUInteger row = [indexPath row];
	Person *p = [people objectAtIndex:row];
	cell.textLabel.text = [p name];
	cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	return cell;
}


#pragma mark -
#pragma mark Table View Delegate Methods

//This handles what happens when Groupies cells are tapped
-(void)showDetailController:(NSIndexPath *) indexPath {
	UITableViewCell *cell = [table cellForRowAtIndexPath:indexPath];
	NSString *name = cell.textLabel.text;
	NSUInteger row = [indexPath row];
	if (childController == nil) 
		childController = [[GroupiesDetailViewController alloc] initWithNibName:@"GroupiesDetailViewController" bundle:nil];
	Person *p = [people objectAtIndex:row];
	childController.title = name;
	childController.person = p;
	[self.navigationController pushViewController:childController animated:YES];	
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	[self showDetailController:indexPath]; 
}

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath: (NSIndexPath *)indexPath {
	return indexPath;	
}

-(void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {
	[self showDetailController:indexPath]; 
}


#pragma mark -
#pragma mark Search Bar Delegate Methods

//REWRITE THESE METHODS, Search is buggy

-(void)searchBarSearchButtonClicked:(UISearchBar *)searchBar {
	NSString *searchTerm = [searchBar text];
	[self handleSearchForTerm:searchTerm];
}

-(void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchTerm {
	if([searchTerm length] == 0) {
		[self resetSearch];
		[table reloadData];
		return;
	}
	[self handleSearchForTerm:searchTerm];
}

-(void)searchBarCancelButtonClicked:(UISearchBar *)searchBar {
	[self resetSearch];
	[table reloadData];
	[searchBar resignFirstResponder];
}


//handles what to do when a row is selected
/*
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
	NSString *test = cell.textLabel.text;
	NSString *message = [[NSString alloc] initWithFormat: @"You've selected: %@", test];
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Friend selected" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[message release];
	[alert release];
	[tableView deselectRowAtIndexPath:indexPath animated:YES];
}
*/
	
@end
