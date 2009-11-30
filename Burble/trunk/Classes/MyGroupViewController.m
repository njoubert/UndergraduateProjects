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
#import "Util.h"

#define kNameValueTag	6
#define kDistValueTag	7
#define kOwnerValueTag	8
@implementation MyGroupViewController

@synthesize waypointsOrGroupControl, leaveButton, membersTableView, waypointsTableView, actionsToolbar, groupsTableView, totalView;

-(void)showCreateGroupView {
	CreateGroupModalViewController *cVC = [[[CreateGroupModalViewController alloc] initWithNibName:@"CreateGroupModalViewController" bundle:nil] autorelease];
	[self.navigationController pushViewController:cVC animated:YES];
}

-(void)refreshView {
	BurbleDataManager *dM = [BurbleDataManager sharedDataManager];
	if (members != nil)
		[members release];
	if (waypoints != nil)
		[waypoints release];
	waypoints = nil;
	members = nil;
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		members = [[dM sortByDistanceFromMe:[dM getGroupMembers]] retain];
		waypoints = [[dM sortByDistanceFromMe:[dM getWaypoints]] retain];
	}

	[membersTableView removeFromSuperview];
	[waypointsTableView removeFromSuperview];
	[groupsTableView removeFromSuperview];
	[membersTableView setFrame:[contentsView frame]];
	[waypointsTableView setFrame:[contentsView frame]];
	[groupsTableView setFrame:[totalView frame]];
	
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		
		leaveButton.enabled = YES;
		actionsToolbar.hidden = NO;
		self.navigationItem.rightBarButtonItem = inviteButton;		
		if (waypointsOrGroupControl.selectedSegmentIndex == 0) {			//members table
			self.navigationItem.title = [NSString stringWithFormat:@"%@ (%d)", 
										 [[BurbleDataManager sharedDataManager] getMyGroup].name,
										 [members count]];			
			[contentsView addSubview:membersTableView];
		} else {															//waypoints table
			self.navigationItem.title = [NSString stringWithFormat:@"%@ (%d)", 
										 [[BurbleDataManager sharedDataManager] getMyGroup].name,
										 [waypoints count]];
			[contentsView addSubview:waypointsTableView];
		}
		
	} else {
		
		actionsToolbar.hidden = YES;
		leaveButton.enabled = NO;
		self.navigationItem.rightBarButtonItem = createGroupButton;
		self.navigationItem.title = @"Previous Groups";
		
		[totalView addSubview:groupsTableView];	
		if ([[[BurbleDataManager sharedDataManager] groupsHistory] count] == 0)
			[self showCreateGroupView];
	}
	
	[membersTableView reloadData];
	[waypointsTableView reloadData];
	[groupsTableView reloadData];
}

-(void)downloadAndRefreshGroup {
	[self refreshView];
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		[[BurbleDataManager sharedDataManager] startDownloadGroupMembersAndCall:self withSelector:@selector(refreshView)];
		[[BurbleDataManager sharedDataManager] startDownloadWaypointsAndCall:self withSelector:@selector(refreshView)];
	}
}
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"My Group";
	refreshButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(downloadAndRefreshGroup)];
	self.navigationItem.leftBarButtonItem = refreshButton;
	
	inviteButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(inviteButtonPressed)];
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
	if (composeView != nil) {
		[composeView release];
		composeView = nil;
	} else {
		[self downloadAndRefreshGroup];
	}
}

-(IBAction)inviteButtonPressed {
	NSMutableArray* toSelect = [[NSMutableArray alloc] init];
	for (Person* p in [[BurbleDataManager sharedDataManager] getFriends]) {
		if (![members containsObject:p])
			[toSelect addObject:p];
	}
	inviteSelectionView = [[SelectRecipients alloc] initWithListOfPeople:toSelect selected:nil withCallbackTarget:self selector:@selector(inviteSelectionViewCallback)];
	inviteSelectionView.title = @"Invite People";
	[toSelect release];
	[self.navigationController pushViewController:inviteSelectionView animated:YES];

}
-(void)inviteSelectionViewCallback {
	NSMutableArray *selected = [inviteSelectionView getReferenceToSelectedPeople];
	if ([selected count] > 0) {
		
		Message* msg = [[Message alloc] initWithText:@"" group:[[BurbleDataManager sharedDataManager] getMyGroup]];
		[msg appendReceivers:selected];
		if ([[BurbleDataManager sharedDataManager] sendMessage:msg]) {
			NSString* message = [NSString stringWithFormat:@"We invited %d people to your group.", [selected count]];
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle: @"Invite Sent" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
		} else {
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle: @"Invite Failed" message:@"We could not create a connection to the server. This is wholly unexpected! Please do try again, and contact us." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
			
		}
		[msg release];
		
	}
	[inviteSelectionView release];
	inviteSelectionView = nil;
}

-(IBAction)composeButtonPressed {
	Person* me = [[BurbleDataManager sharedDataManager] copyOfMe];
	NSMutableArray* selected = [[NSMutableArray alloc] initWithArray:[[BurbleDataManager sharedDataManager] getGroupMembers]];
	NSMutableArray* toSelect = [[NSMutableArray alloc] initWithArray:[[BurbleDataManager sharedDataManager] getGroupMembers]];
	[selected removeObject:me];
	[toSelect removeObject:me];
	composeView = [[ComposeMessageViewController alloc] initWithListOfPeople:toSelect selected:selected withCallbackTarget:self selector:nil];
	[selected release];
	[toSelect release];
	[self.navigationController pushViewController:composeView animated:YES];
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
	[self refreshView];
}

-(void)leftGroupCallback:(id)response {
	[self downloadAndRefreshGroup];
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
	} else if (alertView == joinGroupAlertView && buttonIndex == 1) {
		[[BurbleDataManager sharedDataManager] startJoinGroup:_toJoin.group_id target:self selector:@selector(joinedGroup:)];
		[_toJoin release];
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

- (void)joinedGroup:(id)response {
	[joinGroupAlertView release];
	[self downloadAndRefreshGroup];
}
- (void)dealloc {
	[members release];
	[waypoints release];
	[super dealloc];
}



#pragma mark -
#pragma mark Table View Data Source Methods

// CELL COUNTS
- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	if (tableView == membersTableView) {
		return [members count];
	} else if (tableView == waypointsTableView) {
		return [waypoints count];
	} else if (tableView == groupsTableView) {
		return [[[BurbleDataManager sharedDataManager] groupsHistory] count];
	}
	return 0;
}


// CELL FOR MEMBERS
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPathForMembers:(NSIndexPath *)indexPath {
	static NSString *MemberCell = @"MemberCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MemberCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MemberCell] autorelease];
		
		CGRect nameValueRect = CGRectMake(10, 9, 200, 25);
		UILabel *nameValue = [[UILabel alloc] initWithFrame:nameValueRect];
		nameValue.tag = kNameValueTag;
		nameValue.font = [UIFont boldSystemFontOfSize:20];
		nameValue.textColor = [UIColor blackColor];
		[cell.contentView addSubview:nameValue];
		[nameValue release];
		
		CGRect distLabelRect = CGRectMake(210, 5, 200, 15);		
		UILabel *distLabel = [[UILabel alloc] initWithFrame:distLabelRect];
		distLabel.tag = kDistValueTag;
		distLabel.font = [UIFont systemFontOfSize:12];

		distLabel.textColor = [UIColor blueColor];
		[cell.contentView addSubview: distLabel];
		[distLabel release];		

		CGRect ageValueRect = CGRectMake(210, 23, 200, 15); 
		UILabel *ageValue = [[UILabel alloc] initWithFrame: ageValueRect];
		ageValue.tag = kAgeValueTag;
		ageValue.font = [UIFont systemFontOfSize:12];
		ageValue.textColor = [UIColor lightGrayColor];
		[cell.contentView addSubview:ageValue];
		[ageValue release];
		
	}
	NSUInteger row = indexPath.row;
	Person* m = [members objectAtIndex:row];
	
	UILabel* name = (UILabel*)[cell.contentView viewWithTag:kNameValueTag];
	name.text = m.name;
	
	UILabel* dist = (UILabel*)[cell.contentView viewWithTag:kDistValueTag];
	double distMeters = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:[m.position getLocation]];
	dist.text = [Util prettyDistanceInMeters:distMeters];
	
	UILabel* age = (UILabel*)[cell.contentView viewWithTag:kAgeValueTag];
	age.text = [Util prettyTimeAgo:m.position.timestamp];
	
	cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	return cell;
}

// CELL FOR WAYPOINTS
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPathForWaypoints:(NSIndexPath *)indexPath {
	static NSString *WaypointCell = @"WaypointCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:WaypointCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:WaypointCell] autorelease];
		
		CGRect nameValueRect = CGRectMake(10, 10, 260, 25);
		UILabel *nameValue = [[UILabel alloc] initWithFrame:nameValueRect];
		nameValue.tag = kNameValueTag;
		nameValue.font = [UIFont boldSystemFontOfSize:15];
		nameValue.textColor = [UIColor blackColor];
		[cell.contentView addSubview:nameValue];
		[nameValue release];
		
		CGRect distLabelRect = CGRectMake(190, 1, 80, 15);
		UILabel *distLabel = [[UILabel alloc] initWithFrame:distLabelRect];
		distLabel.tag = kDistValueTag;
		distLabel.font = [UIFont systemFontOfSize:12];
		distLabel.textAlignment = UITextAlignmentRight;
		distLabel.textColor = [UIColor blueColor];
		[cell.contentView addSubview: distLabel];
		[distLabel release];	
		
	}
	NSUInteger row = indexPath.row;
	Waypoint* w = [waypoints objectAtIndex:row];
	
	UILabel* name = (UILabel*)[cell.contentView viewWithTag:kNameValueTag];
	name.text = w.name;
	
	UILabel* dist = (UILabel*)[cell.contentView viewWithTag:kDistValueTag];
	double distMeters = [[BurbleDataManager sharedDataManager] calculateDistanceFromMe:[w getLocation]];
	dist.text = [Util prettyDistanceInMeters:distMeters];
	
	/*
	UIImage *image = [UIImage imageNamed:@"74-location.png"];
	UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
	CGRect frame = CGRectMake(0.0, 0.0, image.size.width, image.size.height);
	button.frame = frame;	// match the button's size with the image size
	[button setBackgroundImage:image forState:UIControlStateNormal];
	
	// set the button's target to this table view controller so we can interpret touch events and map that to a NSIndexSet
	//[button addTarget:self action:@selector(accessoryButtonTappedForRowWithIndexPath:event:) forControlEvents:UIControlEventTouchUpInside];
	button.backgroundColor = [UIColor clearColor];
	cell.accessoryView = button;
	*/
	
	cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	return cell;
}

// CELL FOR GROUPS
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPathForGroups:(NSIndexPath *)indexPath {
	static NSString *HistoricGroupsCell = @"HistoricGroupsCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:HistoricGroupsCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:HistoricGroupsCell] autorelease];
	}
	NSUInteger row = indexPath.row;
	Group* g = [[[BurbleDataManager sharedDataManager] groupsHistory] objectAtIndex:row];
	cell.textLabel.text = [NSString stringWithFormat:@"%d: %@", g.group_id, g.name];
	return cell;
}


// CELL SELECTOR
-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	if (tableView == membersTableView)
		return [self tableView:tableView cellForRowAtIndexPathForMembers:indexPath];
	else if (tableView == waypointsTableView)
		return [self tableView:tableView cellForRowAtIndexPathForWaypoints:indexPath];				
	else if (tableView == groupsTableView)
		return [self tableView:tableView cellForRowAtIndexPathForGroups:indexPath];
	return nil;
}

#pragma mark -
#pragma mark Table View Delegate Methods

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
	NSUInteger row = indexPath.row;
	Waypoint* w = [waypoints objectAtIndex:row];
	[[Test1AppDelegate sharedAppDelegate] locateWaypointOnMap:w];
}
// ACTION FOR GROUPS
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPathForGroups:(NSIndexPath *)indexPath {
	NSUInteger row = indexPath.row;
	Group* g = [[[BurbleDataManager sharedDataManager] groupsHistory] objectAtIndex:row];
	joinGroupAlertView = [[UIAlertView alloc]
						  initWithTitle: @"Joining Group" 
						  message:[NSString stringWithFormat:@"You are joining the group %@", g.name] 
						  delegate:self cancelButtonTitle:@"Wait, no!" 
						  otherButtonTitles:nil];
	[joinGroupAlertView addButtonWithTitle:@"Do it!"];
	_toJoin = [g retain];
	[joinGroupAlertView show];
}
// ACTION SELECTOR
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	if (tableView == membersTableView)
		[self tableView:tableView didSelectRowAtIndexPathForMembers:indexPath];
	else if (tableView == waypointsTableView)
		[self tableView:tableView didSelectRowAtIndexPathForWaypoints:indexPath];
	else if (tableView == groupsTableView)
		[self tableView:tableView didSelectRowAtIndexPathForGroups:indexPath];
	[tableView deselectRowAtIndexPath:indexPath animated:NO];
}

#pragma mark -

-(void)tableView:(UITableView *)tableView accessoryButtonTappedForMembers:(NSIndexPath *)indexPath {
	NSUInteger row = indexPath.row;
	Person* p = [members objectAtIndex:row];
	[[Test1AppDelegate sharedAppDelegate] locatePersonOnMap:p];
}
-(void)tableView:(UITableView *)tableView accessoryButtonTappedForWaypoints:(NSIndexPath *)indexPath {
	NSUInteger row = indexPath.row;
	Waypoint* w = [waypoints objectAtIndex:row];
	[[Test1AppDelegate sharedAppDelegate] locateWaypointOnMap:w];
}

// ACCESSORY SELECTOR
-(void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {
	if (tableView == membersTableView)
		[self tableView:tableView accessoryButtonTappedForMembers:indexPath];
	else if (tableView == waypointsTableView)
		[self tableView:tableView accessoryButtonTappedForWaypoints:indexPath];	
}

@end
