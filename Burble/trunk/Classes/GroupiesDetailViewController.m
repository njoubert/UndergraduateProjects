//
//  GroupiesDetailViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GroupiesDetailViewController.h"
#import "BurbleDataManager.h"

@implementation GroupiesDetailViewController

@synthesize table;
@synthesize functions;
@synthesize person;

-(void)viewWillAppear:(BOOL)animated {
	[super viewWillAppear:animated];
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

/*
- (void)viewDidAppear:(BOOL)animated {
	
 	
}
 */

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	NSString *pGroup;
	NSString *pDist;
	if (person.group != nil) {
		pGroup = [NSString stringWithFormat:@"In Group %@", person.group.name];
	} else {
		pGroup = [NSString stringWithString:@"Not in a group."];
	}
	if (person.position != nil) {
		CLLocation *pL = [person.position getLocation];
		double age = 1; //TODO: calculate age
		double dist = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:pL];
		if (dist > 0) {
			pDist = [NSString stringWithFormat:@"Distance: %f m (%d mins old)", dist, age]; 
		} else {
			pDist = [NSString stringWithFormat:@"Distance: Unknown"];
		}
		[pL release];
	} else {
			pDist = [NSString stringWithFormat:@"Distance: Positon is Unavailable"];		
	}
	
	NSMutableArray *array = [[NSMutableArray alloc] initWithObjects:pGroup, pDist, @"Locate", @"Send Message", nil];
	
	if ([[BurbleDataManager sharedDataManager] isInGroup])
		[array addObject:@"Invite to Group"];
	
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
	self.table = nil;
	self.functions = nil;
	
	map = nil;
	[super viewDidUnload];
}


- (void)dealloc {
    [table release];
	[functions release];
	
	[map release];
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
	//CREATE A NEW MAP CONTROLLER
	if (row == 2) { 
		if (map == nil) map = [[MapViewController alloc] initWithNibName:@"MapViewController" bundle:nil];
		map.title = name;
		[self.navigationController pushViewController:map animated:YES];
		
	}
		
	//Should actually go to create new message screen
	if (row == 3) {

	}
	
	//Should show invitation page
	if (row == 4) {
		//try to send invite
		Group *myG = [[BurbleDataManager sharedDataManager] getMyGroup];
		if (myG != nil) {
			Message* invite = [[Message alloc] initWithText:@"" group:myG];
			[invite appendReceiver:person];
			if ([[BurbleDataManager sharedDataManager] sendMessage:invite]) {

				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Friend invited!" message:@"Your invite was added to your outgoing messages queue and is being sent." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
				[alert show];
				[alert release];
			} else {
				
				UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"You can't do that!" message:@"Your account is not recognized on the server (no user uid). You might not have an internet connection, try restarting the app." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
				[alert show];
				[alert release];
			}
			
		} else {
			NSLog(@"We tried to invite someone and we were not in a group. This should not be happening!");
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"You can't do that!" message:@"You are not in a group!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];			
		}

	}
	 
	
}

//HAVE TO DO THIS WITH GROUPIE NAMES, TOO!
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	NSString *hey = [[NSString alloc] initWithFormat: @"You've selected: Cool"];
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Friend selected" message:hey delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[hey release];
	[alert release];
	//[tableView deselectRowAtIndexPath:indexPath animated:YES];
}
	


@end
