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

@synthesize msgText;
@synthesize functions;
@synthesize message;

-(IBAction)replyTo:(id)sender {
	NSLog(@"wee mofo");
}

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
	msgText.text = self.message.text;
	replyButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemReply target:self action:@selector(replyTo:)];
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
	self.functions = nil;
	self.msgText = nil;

	[super viewDidUnload];
}


- (void)dealloc {
	[functions release];
	[msgText release];
	
	[super dealloc];
}

@end
