//
//  MyGroupViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MyGroupViewController.h"

@implementation MyGroupViewController

@synthesize groupTextField;
@synthesize groupString;
@synthesize myGroupView;
@synthesize createGroupView;
@synthesize inviteToGroupView;
@synthesize messageGroupView;
@synthesize groupLabel;

- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
	if(theTextField == groupTextField) {
		[groupTextField resignFirstResponder];
	}
	return YES;
}

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
		
		groupTextField.text = @"";
		UIAlertView *newGroupAlert = [[UIAlertView alloc]
									  initWithTitle: @"Your new group is called:" message:nameString delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[newGroupAlert show];
		[newGroupAlert release];
		
		self.view = myGroupView;
		groupLabel.text = nameString;
	}
}

-(IBAction)backToGroup:(id)sender {
	self.view = myGroupView;
}

-(IBAction)leaveGroup:(id)sender {
	self.view = createGroupView;
}

-(IBAction)inviteToGroup:(id)sender {
	self.view = inviteToGroupView;
}

-(IBAction)messageGroup:(id)sender {
	self.view = messageGroupView;
}

-(IBAction)goToMyGroupView {
	self.view = myGroupView;
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
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/

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
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[groupTextField release];
	[groupString release];
	[myGroupView release];
	[createGroupView release];
	[inviteToGroupView release];
	[messageGroupView release];
	[groupLabel release];
    [super dealloc];
}


@end
