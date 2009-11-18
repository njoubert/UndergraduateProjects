//
//  GroupiesDetailViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GroupiesDetailViewController.h"


@implementation GroupiesDetailViewController
@synthesize label;
@synthesize message;

@synthesize table;
@synthesize functions;

-(void)viewWillAppear:(BOOL)animated {
	label.text = message;
	[super viewWillAppear:animated];
	//this view will have more:
	//current group info, map page for current location, message screen, distance away, invite to group screen
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
	NSArray *array = [[NSArray alloc] initWithObjects:@"Current Group", @"Distance Away", @"Locate", @"Send Message", @"Invite to Group", nil];
	self.functions = array;
	[array release];
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
	self.label = nil;
	self.message = nil;
	
	self.table = nil;
	self.functions = nil;
	
	map = nil;
	msg = nil;
	invite = nil;
	[super viewDidUnload];
}


- (void)dealloc {
	[label release];
	[message release];

    [table release];
	[functions release];
	
	[map release];
	[msg release];
	[invite release];
	[super dealloc];
}


#pragma mark -
#pragma mark Table View Data Source Methods

//This draws our list table for all of our Groupies

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return 5;
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *FunctionCell = @"FunctionCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:FunctionCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:FunctionCell] autorelease];
	}
	NSUInteger row = [indexPath row];
	cell.textLabel.text = [functions objectAtIndex:row];
	if (row > 1) cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	return cell;
}

#pragma mark -
#pragma mark Table View Delegate Methods

//This handles what happens when Functions cells are tapped
/*
-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath: (NSIndexPath *)indexPath {
	[search resignFirstResponder];
	return indexPath;
}
 */

-(void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
	NSString *name = cell.textLabel.text;
	NSUInteger row = [indexPath row];
	
	//Assumes we have the location of the friend to show on the map
	if (row == 2) { 
		if (map == nil) map = [[MapViewController alloc] initWithNibName:@"MapViewController" bundle:nil];
		map.title = name;
		[self.navigationController pushViewController:map animated:YES];
	}
	
	//Should actually go to create new message screen
	if (row == 3) {
		if (msg == nil) msg = [[FeedViewController alloc] initWithNibName:@"FeedViewController" bundle:nil];
		msg.title = name;
		[self.navigationController pushViewController:msg animated:YES];
	}
	
	//Should show invitation page
	if (row == 4) {
		if (invite == nil) message = [[MyGroupViewController alloc] initWithNibName:@"MyGroupViewController" bundle:nil];
		invite.title = name;
		[self.navigationController pushViewController:invite animated:YES];
	}
	
}


@end
