//
//  CreateGroupModalViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/27/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "CreateGroupModalViewController.h"
#import "Group.h"
#import "BurbleDataManager.h"

@implementation CreateGroupModalViewController

- (void)viewDidLoad {
	self.title = @"Create Group";
}
- (void)viewDidDisappear:(BOOL)animated {
	[self.navigationController popToRootViewControllerAnimated:NO];
}

- (void)createdGroup:(Group*) g {
	if (g == nil) {
		
		NSString *message= [[NSString alloc] initWithFormat:@"We could not create this group! Unfortunately we experienced an error after sending the request to the server."];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Server Error!" message:message delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];
		
	} else {
		
		[self dismissModalViewControllerAnimated:YES];
		
		NSString *title= [[NSString alloc] initWithFormat:@"Created group %@!", g.name];
		NSString *message= [[NSString alloc] initWithString:@"You can now invite your friends to join, and share waypoints and positions."];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:title message:message delegate:nil cancelButtonTitle:@"Great!" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];		
		[title release];
		
		
	}
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	if (textField == nameField) {
		[textField resignFirstResponder];
		
		NSString *name = [[NSString alloc] initWithString:nameField.text];
		
		NSString *message= [[NSString alloc] initWithFormat:@"You are creating the group %@", name];
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
		
		if (![[BurbleDataManager sharedDataManager] startCreateGroup:nameField.text withDesc:@"" target:self selector:@selector(createdGroup:)]) {
			
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
