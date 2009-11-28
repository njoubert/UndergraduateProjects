//
//  MyGroupViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MyGroupViewController.h"
#import "BurbleDataManager.h"

@implementation MyGroupViewController

/*
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
		
		NSString* nameString = [[NSString alloc] initWithString:groupTextField.text];
		
		if ([[BurbleDataManager sharedDataManager] startCreateGroup:nameString withDesc:@"" target:self selector:@selector(groupCreated:)]) {
			//we started connection, now we just wait for groupCreated to be called.
			
		} else {
			[[BurbleDataManager sharedDataManager] messageForCouldNotConnectToServer];
		}
		[nameString release];
		
	}
}

-(void)groupCreated:(Group*)returnValue {
	if (returnValue != nil) {
		groupTextField.text = @"";	
		self.view = myGroupView;
		groupLabel.text = returnValue.name;
	}
}
*/
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	

}

- (void)viewDidAppear:(BOOL)animated {
	

	
}
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
}

@end
