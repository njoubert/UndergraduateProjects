//
//  ImportFriendsViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ImportFriendsViewController.h"
#import "BurbleDataManager.h"

@implementation ImportFriendsViewController

-(IBAction)importFriendsButtonPressed {
	[[BurbleDataManager sharedDataManager] fbShowLoginBox];
}
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
}


- (void)dealloc {
    [super dealloc];
}


@end
