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

@implementation GroupiesViewController

@synthesize addFriendView;
@synthesize mainView;

@synthesize people;
@synthesize names;
@synthesize keys;
@synthesize table;
@synthesize search;
@synthesize allNames;

#pragma mark -
#pragma mark Custom Methods
-(void)resetSearch {
	self.names = [self.allNames mutableDeepCopy];
	NSMutableArray *keyArray = [[NSMutableArray alloc] init];
	[keyArray addObjectsFromArray:[[self.allNames allKeys]
								   sortedArrayUsingSelector:@selector(compare:)]];
	self.keys = keyArray;
	[keyArray release];
}

-(void)handleSearchForTerm:(NSString *)searchTerm {
	NSMutableArray *sectionsToRemove = [[NSMutableArray alloc] init];
	[self resetSearch];
	
	for (NSString *key in self.keys) {
		NSMutableArray *array = [names valueForKey:key];
		NSMutableArray *toRemove = [[NSMutableArray alloc] init];
		for (NSString *name in array) {
			if ([name rangeOfString:searchTerm
							options:NSCaseInsensitiveSearch].location == NSNotFound)
							[toRemove addObject:name];
		}
		if([array count] == [toRemove count]) 
			[sectionsToRemove addObject:key];
		[array removeObjectsInArray:toRemove];
		[toRemove release];
	}
	[self.keys removeObjectsInArray:sectionsToRemove];
	[sectionsToRemove release];
	[table reloadData];
}

-(IBAction)addFriends:(id)sender {
	self.view = addFriendView;
}

-(IBAction)importFriends:(id)sender {
	NSString *message= [[NSString alloc] initWithString: @"You've just imported your Facebook friends!"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: @"Add Facebook friends" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];
}

-(IBAction)backToMain:(id)sender {
	self.view = mainView;
}



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


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	dataManager = [BurbleDataManager sharedDataManager];
	NSArray *ar = [dataManager getFriends];
	self.people = ar;
	[ar release];
	self.title = @"Groupies";
	[self resetSearch];
	[super viewDidLoad];
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/


- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	self.table = nil;
	self.search = nil;
	self.people = nil;
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[addFriendView release];
	[mainView release];
	[people release];
	[table release];
	[search release];
	[allNames release];
	[names release];
	[keys release];
	[dataManager release];
    [super dealloc];
}

#pragma mark -
#pragma mark Table View Data Source Methods

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
	return cell;
}


#pragma mark -
#pragma mark Table View Delegate Methods

-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath: (NSIndexPath *)indexPath {
	[search resignFirstResponder];
	return indexPath;
}

#pragma mark -
#pragma mark Search Bar Delegate Methods

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
	search.text = @"";
	[self resetSearch];
	[table reloadData];
	[searchBar resignFirstResponder];
}



//handles what to do when a row is selected

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
	NSString *test = cell.textLabel.text;
	NSString *message = [[NSString alloc] initWithFormat: @"You've selected: %@", test];
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Friend selected" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[message release];
	[alert release];
	[tableView deselectRowAtIndexPath:indexPath animated:YES];
	
	//pull up new subviews here
	//nav view controller when clicking on friend
	//so pull up a new view and have that view be the parent nav controller
	
}

@end
