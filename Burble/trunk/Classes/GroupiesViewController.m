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
	NSString *path = [[NSBundle mainBundle] pathForResource:@"groupies" ofType:@"plist"];
	NSDictionary *dict = [[NSDictionary alloc] initWithContentsOfFile:path];
	self.allNames = dict;
	self.title = @"Groupies";
	[dict release];
	[self resetSearch];
	[table reloadData];
	[table setContentOffset:CGPointMake(0.0, 44.0) animated:NO];
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
	self.allNames = nil;
	self.names = nil;
	self.keys = nil;
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[addFriendView release];
	[mainView release];
	
	[table release];
	[search release];
	[allNames release];
	[names release];
	[keys release];
    [super dealloc];
}

#pragma mark -
#pragma mark Table View Data Source Methods

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
	return ([keys count] > 0) ? [keys count] : 1;
}

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if ([keys count] == 0)
		return 0;
	NSString *key= [keys objectAtIndex:section];
	NSArray *nameSection = [names objectForKey: key];
	return [nameSection count];
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	NSUInteger section = [indexPath section];
	NSUInteger row = [indexPath row];
	
	NSString *key = [keys objectAtIndex:section];
	NSArray *nameSection = [names objectForKey:key];
	
	static NSString *SectionsTableIdentifier = @"SectionsTableIdentifier";
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:SectionsTableIdentifier];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc]
				 initWithFrame:CGRectZero reuseIdentifier:SectionsTableIdentifier] autorelease];
	}
	cell.text = [nameSection objectAtIndex:row];
	return cell;
}

-(NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
	if([keys count] == 0) 
		return nil;
	NSString *key= [keys objectAtIndex:section];
	return key;
}

-(NSArray *)sectionIndexTitlesForTableView:(UITableView *)tableView {
	return keys;
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
