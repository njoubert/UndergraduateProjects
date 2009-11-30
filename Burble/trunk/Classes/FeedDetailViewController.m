//
//  GroupiesDetailViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FeedDetailViewController.h"
#import "BurbleDataManager.h"

@implementation FeedDetailViewController

@synthesize message;
@synthesize messageText;
@synthesize senderLabel;

-(IBAction)replyTo:(id)s {
	NSMutableArray* toSelect = [[BurbleDataManager sharedDataManager] copyOfAllPeople];
	NSMutableArray* selected = [[NSMutableArray alloc] initWithCapacity:1];
	[selected addObject: sender];
	composeView = [[ComposeMessageViewController alloc] initWithListOfPeople:toSelect selected:selected withCallbackTarget:self selector:@selector(composeViewCallback)];
	[toSelect release];
	[selected release];
	[self.navigationController pushViewController:composeView animated:YES];
}

-(void)composeViewCallback {
	[composeView release];
	composeView = nil;
}

- (void)viewDidAppear:(BOOL)animated {
	if (composeView != nil) {
		[composeView release];
		composeView = nil;
	}
	sender = [[BurbleDataManager sharedDataManager] getPerson:message.sender_uid];
	if (sender == nil) {
		sender = [[Person alloc] init];
		sender.uid = message.sender_uid;
		sender.name = [NSString stringWithFormat:@"Person ID %d", message.sender_uid];
	}
	senderLabel.text = sender.name;
	messageText.text = message.text;
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	replyButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemReply target:self action:@selector(replyTo:)];
	composeView = nil;
	self.navigationItem.rightBarButtonItem = replyButton;
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
	[super viewDidUnload];
}


- (void)dealloc {
	[super dealloc];
	[message release];
	[composeView release];
	[senderLabel release];
	[messageText release];
}

@end
