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
		
		if (![[BurbleDataManager sharedDataManager] startTryToRegister:nameField.text caller:self]) {
			
			NSString *message= [[NSString alloc] initWithFormat:@"We could not create a connection object! This is madness!"];
			UIAlertView *alert = [[UIAlertView alloc]
								  initWithTitle: @"CRITICAL ERROR!" message:message delegate:self cancelButtonTitle:@"Oh damn, stop!" otherButtonTitles:nil];
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
