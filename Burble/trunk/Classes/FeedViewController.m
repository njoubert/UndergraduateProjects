//
//  FeedViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FeedViewController.h"


@implementation FeedViewController;
@synthesize table;
@synthesize messages;
@synthesize childView;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

-(void)refreshMessages  {
	if (messages != nil)
		[messages release];
	messages = [[[BurbleDataManager sharedDataManager] getMessages] retain];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{
	self.title = @"Feed";
	
	// Load up messages
	dataManager = [BurbleDataManager sharedDataManager];
	_currentMessage = nil;
	//NSArray *reqmessages = [dataManager getMessages];
	
	// dummy messages
	/*
	NSDictionary *row1 = [[NSDictionary alloc] initWithObjectsAndKeys:
						   @"Jon", @"sender", @"message", @"type", @"Hi! How's your day going?", @"content", nil];
	NSDictionary *row2 = [[NSDictionary alloc] initWithObjectsAndKeys:
						   @"Niels", @"sender", @"routing", @"type", nil, @"content", nil];
	NSDictionary *row3 = [[NSDictionary alloc] initWithObjectsAndKeys:
						   @"Janelle", @"sender", @"invite", @"type", nil, @"content", nil];
	
	NSArray *array = [[NSArray alloc] initWithObjects:
					  row1, row2, row3, nil];

	self.messages = array;
	
	[row1 release];
	[row2 release];
	[row3 release];
	[array release];
	*/
	[self refreshMessages];
    [super viewDidLoad];
}

-(void)viewDidAppear:(BOOL)animated {
	
	[[BurbleDataManager sharedDataManager] startDownloadMessagesAndCall:self withSelector:@selector(refreshMessages)];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

#pragma mark -
#pragma mark Table View Data Source Methods

- (NSInteger) tableView:(UITableView *)tableView
  numberOfRowsInSection:(NSInteger)section
{
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
		
		CGRect senderLabelRect = CGRectMake(0, 5, 70, 15);
		UILabel *senderLabel = [[UILabel alloc] initWithFrame:senderLabelRect];
		senderLabel.textAlignment = UITextAlignmentRight;
		senderLabel.text = @"sender";
		senderLabel.font = [UIFont boldSystemFontOfSize:12];
		senderLabel.textColor = [UIColor lightGrayColor];
		[cell.contentView addSubview: senderLabel];
		[senderLabel release];
		
		CGRect typeLabelRect = CGRectMake(0, 26, 70, 15); 
		UILabel *typeLabel = [[UILabel alloc] initWithFrame:typeLabelRect];
		typeLabel.textAlignment = UITextAlignmentRight;
		typeLabel.text = @"type";
		typeLabel.font = [UIFont boldSystemFontOfSize:12];
		typeLabel.textColor = [UIColor lightGrayColor];
		[cell.contentView addSubview: typeLabel];
		[typeLabel release];

		CGRect senderValueRect = CGRectMake(80, 5, 200, 15);
		UILabel *senderValue = [[UILabel alloc] initWithFrame:
			senderValueRect];
		senderValue.tag = kSenderValueTag;
		[cell.contentView addSubview:senderValue];
		[senderValue release];
		
		CGRect typeValueRect = CGRectMake(80, 25, 200, 15);
		UILabel *typeValue = [[UILabel alloc] initWithFrame:
			typeValueRect];
		typeValue.tag = kTypeValueTag;
		[cell.contentView addSubview:typeValue];
		[typeValue release];
	}

	NSUInteger row = [indexPath row];
	//NSDictionary *rowData = [self.messages objectAtIndex:row];
	Message* msg = [self.messages objectAtIndex:row];
	UILabel *sender = (UILabel *)[cell.contentView viewWithTag:
									kSenderValueTag];
	Person* friend = [[BurbleDataManager sharedDataManager] getFriend:msg.sender_uid];
	if (friend == nil) {
		sender.text = [NSString stringWithFormat:@"Friend ID %d", msg.sender_uid];
	} else {
		sender.text = [NSString stringWithString:friend.name];		
	}
	sender.font = [UIFont boldSystemFontOfSize:12];

	if (msg.read)
		cell.accessoryType = UITableViewCellAccessoryCheckmark;
	//cell.accessoryType = UITableViewCellAcc
	UILabel *type = (UILabel *)[cell.contentView viewWithTag:
									kTypeValueTag];
	type.text = msg.type;
	type.font = [UIFont boldSystemFontOfSize:12];

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

- (void)tableView:(UITableView *)tableView
	didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSUInteger row = [indexPath row]; 
	Message *msg = [self.messages objectAtIndex:row];
	[[BurbleDataManager sharedDataManager] markMessageAsRead:msg];
	
	//TODO: need to update the look of the cell to show that this has now been read.
	
	Person* sender = [[BurbleDataManager sharedDataManager] getFriend:msg.sender_uid];
	if (sender == nil) {
		sender = [[Person alloc] init];
		sender.name = [NSString stringWithFormat:@"Sender ID %d", msg.sender_uid];
	}
	if ([msg.type isEqualToString:kMessageTypeText])
	{
		NSString *message = [[NSString alloc] initWithString:msg.text];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:(@"Message Details")
							  message:message
							  delegate:nil
							  cancelButtonTitle:@"Done"
							  otherButtonTitles:nil];
		[alert show];
		[message release];
		[alert release];
	}
	if ([msg.type isEqualToString:kMessageTypeGroupInvite])
	{
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
	}
	if ([msg.type isEqualToString:kMessageTypeRoutingRequest])
	{
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
