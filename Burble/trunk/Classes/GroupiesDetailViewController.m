//
//  GroupiesDetailViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GroupiesDetailViewController.h"
#import "BurbleDataManager.h"
#import "Util.h"

#define kNumOfSections	2

@implementation GroupiesDetailViewController

@synthesize table;
@synthesize person;

-(void)setupViewData {
	NSString *pGroup;
	NSString *pDist;
	infoSectionData = [[NSMutableArray alloc] initWithCapacity:2];
	functionSectionData = [[NSMutableArray alloc] initWithCapacity:2];
	
	if (person.group != nil) {
		pGroup = [NSString stringWithFormat:@"Member Of: %@", person.group.name];
	} else {
		pGroup = [NSString stringWithString:@"Not in a group."];
	}
	[infoSectionData addObject:pGroup];
	
	if (person.position != nil) {
		CLLocation *pL = [person.position getLocation];
		double dist = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:pL];
		if (dist > 0) {
			pDist = [NSString stringWithFormat:@"Distance: %@", [Util prettyDistanceInMeters:dist]]; 
		} else {
			pDist = [NSString stringWithFormat:@"Distance: Unknown"];
		}
		[infoSectionData addObject:pDist];
		[infoSectionData addObject:[NSString stringWithFormat:@"Last Seen: %@ ago", [Util prettyTimeAgo:person.position.timestamp]]];
		[pL release];
	} else {
		pDist = [NSString stringWithFormat:@"Distance: Positon is Unavailable"];		
		[infoSectionData addObject:pDist];
	}
	
	[functionSectionData addObject:@"Locate"];
	[functionSectionData addObject:@"Send Message"];
	
	
	if ([[BurbleDataManager sharedDataManager] isInGroup])
		[functionSectionData addObject:@"Invite to Group"];
		
}

-(void)viewWillAppear:(BOOL)animated {
	[super viewWillAppear:animated];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	[self setupViewData];
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
	functionSectionData = nil;
	infoSectionData = nil;
	
	map = nil;
	[super viewDidUnload];
}


- (void)dealloc {
    [table release];
	[functionSectionData release];
	[infoSectionData release];
	
	[map release];
	[super dealloc];
}


#pragma mark -
#pragma mark Table View Data Source Methods

//This draws our list table for all of our Groupies

- (NSInteger) numberOfSectionsInTableView:(UITableView*)tableView {
	return kNumOfSections;
}

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if (section == 0) {
		return [infoSectionData count];
	} else if (section == 1) {
		return [functionSectionData count];
	}
	return 0;
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	NSUInteger section = indexPath.section;
	NSUInteger row = indexPath.row;
	
	
	static NSString *FunctionCell = @"FunctionCell";
	static NSString *InfoCell = @"InfoCell";
	if (section == 0) { //info cell
		
		UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:InfoCell];
		if (cell == nil) {
			cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:FunctionCell] autorelease];
		}
		cell.textLabel.text = [infoSectionData objectAtIndex:row];
		cell.textLabel.font = [UIFont systemFontOfSize:15];
		return cell;

	} else if (section == 1) { //functions cell
		
		UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:FunctionCell];
		if (cell == nil) {
			cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:FunctionCell] autorelease];
		}
		cell.textLabel.text = [functionSectionData objectAtIndex:row];
		cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
		cell.textLabel.font = [UIFont boldSystemFontOfSize:15];
		return cell;
		
	}
	return nil;
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
	if (row == 0) { 
		if (map == nil) map = [[MapViewController alloc] initWithNibName:@"MapViewController" bundle:nil];
		map.title = name;
		[self.navigationController pushViewController:map animated:YES];
		
	}
		
	//Should actually go to create new message screen
	if (row == 1) {

	}
	
	//Should show invitation page
	if (row == 2) {
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

-(NSIndexPath*)tableView:(UITableView *)tableView willSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	if (indexPath.section == 0) {
		return nil;
	} else {
		return indexPath;
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
