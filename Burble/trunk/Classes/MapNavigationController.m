//
//  MapNavigationController.m
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MapNavigationController.h"

#import "BurbleDataManager.h"
#import "RegisterViewController.h"

@implementation MapNavigationController

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
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/

- (void)showRegisterDialog {
	RegisterViewController *rVC = [[[RegisterViewController alloc] initWithNibName:nil bundle:nil] autorelease];
	[self presentModalViewController:rVC animated:YES];
}

-(void)loginCallback:(NSNumber*)successObj {
	BOOL success = [successObj boolValue];
	if (!success)
		[self showRegisterDialog];
}

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	if (![[BurbleDataManager sharedDataManager] isRegistered]) {
		[self showRegisterDialog];
	} else {
		[[BurbleDataManager sharedDataManager] loginWithCallback:self selector:@selector(loginCallback:)];
	}
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
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
