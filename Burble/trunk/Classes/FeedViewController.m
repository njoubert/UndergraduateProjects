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
	self.title = @"Feed";
	
	// Load up messages
	dataManager = [BurbleDataManager sharedDataManager];
	NSArray *reqmessages = [dataManager getMessages];
	self.messages = reqmessages;
	[reqmessages release];
	
    [super viewDidLoad];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

-(IBAction)buttonPressed {
	NSString *message= [[NSString alloc] initWithString: @"Why did we do this again?"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: @"Jon wonders:" message:message delegate:nil cancelButtonTitle:@"Very hot" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];
}

//This draws our list table for all of our Messages

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [messages count];
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *MessageCell = @"MessageCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MessageCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MessageCell] autorelease];
	}
	NSUInteger row = [indexPath row];
	Message *m = [messages objectAtIndex:row];
	cell.textLabel.text = [m sender];
	cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	return cell;
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
    [super dealloc];
}


@end
