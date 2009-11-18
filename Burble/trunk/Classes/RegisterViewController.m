//
//  RegisterViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RegisterViewController.h"


@implementation RegisterViewController

@synthesize nameField;

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

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	if (textField == nameField) {
		[textField resignFirstResponder];
		
		NSString *name = [[NSString alloc] initWithString:nameField.text];
		
		NSString *message= [[NSString alloc] initWithFormat:@"You are registering the name %@", name];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Confirm!" message:message delegate:self cancelButtonTitle:@"Wait, no!" otherButtonTitles:nil];
		[alert addButtonWithTitle:@"Do it!"];
		[alert show];
		[alert release];
		[message release];
	}
	return YES;
}

-(void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (buttonIndex == 0) {
		//don't do anything, he decided not to go
	} else if (buttonIndex == 1) {
		
		[[Test1AppDelegate sharedAppDelegate] showActivityViewer];
		
		BOOL success = [[BurbleDataManager sharedDataManager] tryToRegister:nameField.text];
		
		[[Test1AppDelegate sharedAppDelegate] hideActivityViewer];
		
		if (success) {
			
			[self dismissModalViewControllerAnimated:YES];
			
		} else {
			NSString *message= [[NSString alloc] initWithString:@"We could not connect to our server. Check your internet connectivity!"];
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle: @"Confirm!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
			[message release];
		}
	}
}

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
    [super dealloc];
}


@end
