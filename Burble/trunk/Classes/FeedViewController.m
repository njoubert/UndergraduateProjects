//
//  FeedViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FeedViewController.h"


@implementation FeedViewController

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


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{
	self.title = @"Feed";
	
	// Load up messages
	dataManager = [BurbleDataManager sharedDataManager];
	//NSArray *reqmessages = [dataManager getMessages];
	
	// dummy messages
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
	
    [super viewDidLoad];
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
	NSDictionary *rowData = [self.messages objectAtIndex:row];
	UILabel *sender = (UILabel *)[cell.contentView viewWithTag:
									kSenderValueTag];
	sender.text = [rowData objectForKey:@"sender"];
	sender.font = [UIFont boldSystemFontOfSize:12];

	
	UILabel *type = (UILabel *)[cell.contentView viewWithTag:
									kTypeValueTag];
	type.text = [rowData objectForKey:@"type"];
	type.font = [UIFont boldSystemFontOfSize:12];

	return cell;
}

/* Transition to message contents */
- (NSIndexPath *)tableView:(UITableView *)tableView
	willSelectRowAtIndexPath: (NSIndexPath *)indexPath
{
	return indexPath;
}

- (void)tableView:(UITableView *)tableView
	didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSUInteger row = [indexPath row]; 
	NSDictionary *rowData = [self.messages objectAtIndex:row];

	if ([rowData objectForKey:@"type"] == @"message")
	{
		NSString *message = [[NSString alloc] initWithFormat:
							 @"%@", [rowData objectForKey:@"content"]];
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
	if ([rowData objectForKey:@"type"] == @"invite")
	{
		NSString *message = [[NSString alloc] initWithFormat:
							 @"Invitation from %@", [rowData objectForKey:@"sender"]];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:@"Group Invitation"
							  message:message
							  delegate:nil
							  cancelButtonTitle:@"Accept"
							  otherButtonTitles:@"Decline"];
		[alert show];
		[message release];
		[alert release];
	}
	if ([rowData objectForKey:@"type"] == @"routing")
	{
		NSString *message = [[NSString alloc] initWithFormat:
							 @"Routing request from %@", [rowData objectForKey:@"sender"]];
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
