//
//  FeedViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FeedViewController.h"
#import "SelectRecipients.h"
#import "BurbleDataManager.h"
#import "Util.h"

@implementation FeedViewController;
@synthesize table;
@synthesize messages;

-(void)refreshView  {
	if (messages != nil)
		[messages release];
	messages = [[[BurbleDataManager sharedDataManager] getMessages] retain];
	NSLog(@"refreshing messages, count %d", [messages count]);
	[table reloadData];
	//[table setNeedsDisplay];
	//[self.view setNeedsDisplay];
}

- (void)downloadAndRefreshMessages {
	[self refreshView];
	[[BurbleDataManager sharedDataManager] startDownloadMessagesAndCall:self withSelector:@selector(refreshView)];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"Feed";
	_currentMessage = nil;
	
	composeButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCompose target:self action:@selector(composeButtonPressed:)];
	self.navigationItem.rightBarButtonItem = composeButton;
	
	refreshButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemRefresh target:self action:@selector(downloadAndRefreshMessages)];
	self.navigationItem.leftBarButtonItem = refreshButton;
    [super viewDidLoad];
}

-(void)viewDidAppear:(BOOL)animated {
	if (composeView != nil) {
		[composeView release];
		composeView = nil;
	} else {
		[self downloadAndRefreshMessages];
	}
}


-(void)composeButtonPressed:(id)sender {
	NSMutableArray* toSelect = [[BurbleDataManager sharedDataManager] copyOfAllPeople];
	composeView = [[ComposeMessageViewController alloc] initWithListOfPeople:toSelect selected:nil withCallbackTarget:self selector:nil];
	[toSelect release];
	[self.navigationController pushViewController:composeView animated:YES];
}

#pragma mark -
#pragma mark Table View Data Source Methods

- (NSInteger) tableView:(UITableView *)tableView
  numberOfRowsInSection:(NSInteger)section {
	return [self.messages count];
}


- (UITableViewCell *)tableView:(UITableView *)tableView 
		cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	static NSString *CellTableIdentifier = @"CellTableIdentifier ";
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:
							 CellTableIdentifier];
	
	if (cell == nil)
	{
		cell = [[[UITableViewCell alloc]
				 initWithStyle:UITableViewCellStyleDefault
				 reuseIdentifier:CellTableIdentifier] autorelease];
		
		CGRect senderValueRect = CGRectMake(10, 5, 200, 15);
		UILabel *senderValue = [[UILabel alloc] initWithFrame:senderValueRect];
		senderValue.tag = kSenderValueTag;
		[cell.contentView addSubview:senderValue];
		[senderValue release];

		CGRect ageValueRect = CGRectMake(220, 5, 200, 15);
		UILabel *ageValue = [[UILabel alloc] initWithFrame:
							  ageValueRect];
		ageValue.tag = kAgeValueTag;
		[cell.contentView addSubview:ageValue];
		[ageValue release];

		CGRect typeLabelRect = CGRectMake(10, 24, 200, 15); 
		UILabel *typeLabel = [[UILabel alloc] initWithFrame:typeLabelRect];
		typeLabel.text = @"contents";
		typeLabel.font = [UIFont systemFontOfSize:12];
		typeLabel.textColor = [UIColor lightGrayColor];
		typeLabel.tag = kContentsValueTag;
		[cell.contentView addSubview: typeLabel];
		[typeLabel release];
		
	
	}
	 
	NSUInteger row = [indexPath row];
	NSLog(@"geting message from index %d, with %d msgs.", row, [self.messages count]);
	Message* msg = [self.messages objectAtIndex:row];
	UILabel *sender = (UILabel *)[cell.contentView viewWithTag:
									kSenderValueTag];
	Person* friend = [[BurbleDataManager sharedDataManager] getPerson:msg.sender_uid];
	if (friend == nil) {
		sender.text = [NSString stringWithFormat:@"Friend ID %d sent", msg.sender_uid];
	} else {
		if (msg.type == kMessageTypeGroupInvite)
			sender.text = [NSString stringWithFormat:@"Invite from %@:", friend.name];
		else if (msg.type == kMessageTypeRoutingRequest)
			sender.text = [NSString stringWithFormat:@"Route from %@:", friend.name];
		else
			sender.text = [NSString stringWithFormat:@"%@ said:", friend.name];	
		
	}
	sender.font = [UIFont boldSystemFontOfSize:12];

	if (msg.read)
		cell.accessoryType = UITableViewCellAccessoryCheckmark;
	else {
		cell.accessoryType = UITableViewCellAccessoryNone;
	}

	UILabel *contents = (UILabel *)[cell.contentView viewWithTag: kContentsValueTag];
	if (msg.type == kMessageTypeRoutingRequest) {
		contents.text = [NSString stringWithString:msg.text];
	} else if (msg.type == kMessageTypeGroupInvite) {
		contents.text = [NSString stringWithFormat:@"Invitation to %@", msg.group.name];
	} else if (msg.type == kMessageTypeText) {
		contents.text = msg.text;
	}
	

	UILabel *age = (UILabel*)[cell.contentView viewWithTag:kAgeValueTag];
	age.text = [NSString stringWithFormat:@"%@ old", [Util prettyTimeAgo:msg.sent_time]];
	age.font = [UIFont systemFontOfSize:12];
	age.textColor = [UIColor lightGrayColor];
	return cell;
}

/* Transition to message contents */
- (NSIndexPath *)tableView:(UITableView *)tableView
	willSelectRowAtIndexPath: (NSIndexPath *)indexPath
{
	return indexPath;
}

-(void)joinedGroup:(NSURLRequest*) req {
	[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (_currentMessage) {
		NSLog(@"clicked index %d", buttonIndex);
		if (buttonIndex == 0 && _currentMessage.group != nil) {
			if (![[BurbleDataManager sharedDataManager] startJoinGroup:_currentMessage.group.group_id target:self selector:@selector(joinedGroup:)]) {
				[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
				NSString *message = [[NSString alloc] initWithString:@"Please try again"];
				UIAlertView *alert = [[UIAlertView alloc]
									  initWithTitle:(@"Could not start connection!")
									  message:message
									  delegate:nil
									  cancelButtonTitle:@"Done"
									  otherButtonTitles:nil];
				[alert show];
				[message release];
				[alert release];
			}
		}
		_currentMessage = nil;
	}
}

//This handles what happens when Groupies cells are tapped
-(void)showDetailController:(NSIndexPath *) indexPath {
	NSUInteger row = [indexPath row]; 
	Message *msg = [self.messages objectAtIndex:row];
	[[BurbleDataManager sharedDataManager] markMessageAsRead:msg];
	
	//TODO: need to update the look of the cell to show that this has now been read.
	
	if (childView == nil) 
		childView = [[FeedDetailViewController alloc] initWithNibName:@"FeedDetailViewController" bundle:nil];
	
	childView.title = @"Message Details";
	childView.message = msg;
	[self.navigationController pushViewController:childView animated:YES];	
}

- (void)tableView:(UITableView *)tableView
	didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSUInteger row = [indexPath row]; 
	Message *msg = [self.messages objectAtIndex:row];
	[[BurbleDataManager sharedDataManager] markMessageAsRead:msg];
	[tableView cellForRowAtIndexPath:indexPath].accessoryType = UITableViewCellAccessoryCheckmark;
	[tableView setNeedsDisplay];
	Person* sender = [[BurbleDataManager sharedDataManager] getFriend:msg.sender_uid];
	if (sender == nil)
		sender = [[BurbleDataManager sharedDataManager] getGroupMember:msg.sender_uid];

	if (sender == nil) {
		sender = [[Person alloc] init];
		sender.name = [NSString stringWithFormat:@"Sender ID %d", msg.sender_uid];
	}
	if ([msg.type isEqualToString:kMessageTypeText]) {
		
		[self showDetailController:indexPath];
		
	} else if ([msg.type isEqualToString:kMessageTypeGroupInvite]) {
		Group *g = msg.group;
		_currentMessage = msg;
		if (g == nil)
			NSLog(@"nil message group");
		else
			NSLog(@"%@", g.name);
		NSString *message = [[NSString alloc] initWithFormat:
							 @"Invitation to group %@", g.name];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:@"Group Invitation"
							  message:message
							  delegate:self
							  cancelButtonTitle:@"Accept"
							  otherButtonTitles:nil];
		[alert addButtonWithTitle:@"Ignore"];
		[alert show];
		[message release];
		[alert release];
	} else if ([msg.type isEqualToString:kMessageTypeRoutingRequest]) {
		NSString *message = [[NSString alloc] initWithFormat:
							 @"Routing request from %@", sender.name];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:@"Routing Request"
							  message:message
							  delegate:nil
							  cancelButtonTitle:@"View"
							  otherButtonTitles:@"Dismiss"];
		[alert show];
		[message release];
		[alert release];
	}
	
	[tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	self.messages = nil;
	self.table = nil;
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc {
	[messages release];
	[table release];
    [super dealloc];
}

@end
