//
//  MyGroupViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MyGroupViewController.h"
#import "BurbleDataManager.h"
#import "CreateGroupModalViewController.h"

@implementation MyGroupViewController

@synthesize waypointsOrGroupControl, leaveButton, membersTableView, waypointsTableView;
/*
//Creates new group with specified title.
-(IBAction)createGroup:(id)sender {
	self.groupString = groupTextField.text;
	
	NSString *nameString = groupString;
	if([nameString length] == 0) {
		
		UIAlertView *groupCreationFailedAlert = [[UIAlertView alloc]
									  initWithTitle: @"Group creation failed" message:@"You must enter a title for your group!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[groupCreationFailedAlert show];
		[groupCreationFailedAlert release];
		
	} else {
		
		NSString* nameString = [[NSString alloc] initWithString:groupTextField.text];
		
		if ([[BurbleDataManager sharedDataManager] startCreateGroup:nameString withDesc:@"" target:self selector:@selector(groupCreated:)]) {
			//we started connection, now we just wait for groupCreated to be called.
			
		} else {
			[[BurbleDataManager sharedDataManager] messageForCouldNotConnectToServer];
		}
		[nameString release];
		
	}
}

-(void)groupCreated:(Group*)returnValue {
	if (returnValue != nil) {
		groupTextField.text = @"";	
		self.view = myGroupView;
		groupLabel.text = returnValue.name;
	}
}
*/
-(void)refreshGroup {
	if (members != nil)
		[members release];
	if (waypoints != nil)
		[waypoints release];
	waypoints = nil;
	members = nil;
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		
		members = [[[BurbleDataManager sharedDataManager] getGroupMembers] retain];
		waypoints = [[[BurbleDataManager sharedDataManager] getWaypoints] retain];
		leaveButton.enabled = YES;
		self.navigationItem.rightBarButtonItem = composeButton;
		self.navigationItem.title = [[BurbleDataManager sharedDataManager] getMyGroup].name;
		
	} else {
		
		leaveButton.enabled = NO;
		self.navigationItem.rightBarButtonItem = createGroupButton;
		self.navigationItem.title = @"My Group";
	}
	[membersTableView reloadData];
	[waypointsTableView reloadData];
}
-(void)downloadAndRefreshGroup {
	[self refreshGroup];
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		[[BurbleDataManager sharedDataManager] startDownloadGroupMembersAndCall:self withSelector:@selector(refreshGroup)];
		[[BurbleDataManager sharedDataManager] startDownloadWaypointsAndCall:self withSelector:@selector(refreshGroup)];
	}
}
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"My Group";
	refreshButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(downloadAndRefreshGroup)];
	self.navigationItem.leftBarButtonItem = refreshButton;
	
	composeButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCompose target:self action:@selector(composeButtonPressed)];
	createGroupButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(createGroupButtonPressed)];
	
	leaveGroupAlertView = [[UIAlertView alloc]
							initWithTitle: @"Leaving Group?" 
						   message:@"You are leaving your group! You will no longer see this group's members or waypoints until you join again." 
						   delegate:self 
						   cancelButtonTitle:@"Wait, no!" 
						   otherButtonTitles:nil];
	[leaveGroupAlertView addButtonWithTitle:@"Do it!"];
	
	createGroupAlertView = [[UIAlertView alloc]
							initWithTitle: @"Create a Group" 
							message:@"You will be leaving your current group if you create a new group." 
							delegate:self cancelButtonTitle:@"Wait, no!" 
							otherButtonTitles:nil];
	[createGroupAlertView addButtonWithTitle:@"Do it!"];
	
	members = nil;
	waypoints = nil;
	personDetailController = nil;
}

- (void)viewDidAppear:(BOOL)animated {
	[self downloadAndRefreshGroup];
	[self displaySelectorButtonPressed];
	if (![[BurbleDataManager sharedDataManager] isInGroup]) 
		[self showCreateGroupView];
}

-(IBAction)composeButtonPressed {
	
}

-(void)showCreateGroupView {
	CreateGroupModalViewController *cVC = [[[CreateGroupModalViewController alloc] initWithNibName:@"CreateGroupModalViewController" bundle:nil] autorelease];
	[self.navigationController pushViewController:cVC animated:NO];
}

//he pressed the create button
-(IBAction)createGroupButtonPressed {
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		[createGroupAlertView show];
	} else {
		[self showCreateGroupView];
	}
}	

//he pressed the display selector button
-(IBAction)displaySelectorButtonPressed {
	[membersTableView removeFromSuperview];
	[waypointsTableView removeFromSuperview];
	if (waypointsOrGroupControl.selectedSegmentIndex == 0) {			//members table
		[contentsView addSubview:membersTableView];
	} else {															//waypoints table
		[contentsView addSubview:waypointsTableView];
	}
	
}

-(void)leftGroupCallback:(id)response {
	//Here we clear our local view as necessary and show the "Create group" dialog.
	[self downloadAndRefreshGroup];
	[self showCreateGroupView];
}

-(void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (alertView == leaveGroupAlertView && buttonIndex == 1) {
		if (![[BurbleDataManager sharedDataManager] startLeaveGroupWithTarget:self selector:@selector(leftGroupCallback:)]) {
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle: @"CRITICAL ERROR!" message:@"We could not create a connection object! This is madness!" delegate:self cancelButtonTitle:@"Oh damn, stop!" otherButtonTitles:nil];
			[alert show];
			[alert release];
		}		
	} else if (alertView == createGroupAlertView && buttonIndex == 1) {
		[self showCreateGroupView];
	}
}

-(IBAction)leaveButtonPressed {
	[leaveGroupAlertView show];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
	[super viewDidUnload]; 
}


- (void)dealloc {
	[members release];
	[waypoints release];
	[super dealloc];
}



#pragma mark -
#pragma mark Table View Data Source Methods

// CELL COUNTS FOR BOTH
- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if (tableView == membersTableView) {
		return [members count];
	} else if (tableView == waypointsTableView) {
		return [waypoints count];
	}
	return 0;
}


// CELL FOR MEMBERS
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPathForMembers:(NSIndexPath *)indexPath {
	static NSString *MemberCell = @"MemberCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MemberCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MemberCell] autorelease];
	}
	NSUInteger row = indexPath.row;
	Person* m = [members objectAtIndex:row];
	cell.textLabel.text = m.name;
	cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;

	
	return cell;
}

// CELL FOR WAYPOINTS
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPathForWaypoints:(NSIndexPath *)indexPath {
	static NSString *WaypointCell = @"WaypointCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:WaypointCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:WaypointCell] autorelease];
	}
	NSUInteger row = indexPath.row;
	Waypoint* w = [waypoints objectAtIndex:row];
	cell.textLabel.text = w.name;
	cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;

	
	return cell;
}

// CELL SELECTOR
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	if (tableView == membersTableView)
		return [self tableView:tableView cellForRowAtIndexPathForMembers:indexPath];
	else if (tableView == waypointsTableView)
		return [self tableView:tableView cellForRowAtIndexPathForWaypoints:indexPath];				
	return nil;
}

#pragma mark -
#pragma mark Table View Delegate Methods

//This handles what happens when Groupies cells are tapped
-(void)showDetailController:(NSIndexPath *) indexPath {

//	UITableViewCell *cell = [table cellForRowAtIndexPath:indexPath];
//	NSString *name = cell.textLabel.text;
//	NSUInteger row = [indexPath row];
//	if (childController == nil) 
//		childController = [[GroupiesDetailViewController alloc] initWithNibName:@"GroupiesDetailViewController" bundle:nil];
//	Person *p = [people objectAtIndex:row];
//	childController.title = name;
//	childController.person = p;
//	[self.navigationController pushViewController:childController animated:YES];	

}

// ACTION FOR MEMBERS
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPathForMembers:(NSIndexPath *)indexPath {
	NSUInteger row = indexPath.row;
	if (personDetailController == nil)
		personDetailController = [[GroupiesDetailViewController alloc] initWithNibName:@"GroupiesDetailViewController" bundle:nil];
	Person *p = [members objectAtIndex:row];
	personDetailController.person = p;
	personDetailController.title = p.name;
	[self.navigationController pushViewController:personDetailController animated:YES];	
}
// ACTION FOR WAYPOINTS
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPathForWaypoints:(NSIndexPath *)indexPath {
	
}
// ACTION SELECTOR
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	if (tableView == membersTableView)
		[self tableView:tableView didSelectRowAtIndexPathForMembers:indexPath];
	else if (tableView == waypointsTableView)
		[self tableView:tableView didSelectRowAtIndexPathForWaypoints:indexPath];				
}

//-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath: (NSIndexPath *)indexPath {
//	return indexPath;
//}

//-(void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {
//
//}


@end
