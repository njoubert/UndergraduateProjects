//
//  AddWaypointViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AddWaypointViewController.h"
#import "BurbleDataManager.h"
#import "Waypoint.h"

@implementation AddWaypointViewController

@synthesize nameField, descField, isHereSwitch;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		
    }
    return self;
}
*/


- (void)saveW {
	Waypoint* wP = [[Waypoint alloc] initWithNameAndDescription:self.nameField.text description:self.descField.text];	
	wP->coordinate = waypointLocation;
	wP->elevation = 0; //we dont know this at the moment.
	wP->iAmHere = self.isHereSwitch.state;
	[[BurbleDataManager sharedDataManager] addWaypoint:wP];
	
	//TODO: force a map reload
	
	[self dismissModalViewControllerAnimated:YES];
}

- (void)cancelW {
	[self dismissModalViewControllerAnimated:YES];
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	[textField resignFirstResponder];
	return YES;
}

- (IBAction)saveButtonPressed {
	[self saveW];
}

- (IBAction) cancelButtonPressed {
	[self cancelW];
}

- (void)setWaypointLocation:(CLLocationCoordinate2D)cStruct {
	waypointLocation = cStruct;
}
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	
}

- (void)viewDidAppear:(BOOL)animated {
	self.title = @"Save Waypoint";
	self.nameField.text = [[BurbleDataManager sharedDataManager] getNextWaypointName];
	self.descField.text = [[BurbleDataManager sharedDataManager] getNextWaypointDesc];
	
	CLLocation *current = [[BurbleDataManager sharedDataManager] getLocation];
	if (nil != current && 0 < current.horizontalAccuracy) {
		CLLocation *waypCLL = [[CLLocation alloc] initWithLatitude:waypointLocation.latitude longitude:waypointLocation.longitude];
		CLLocationDistance dist = [current getDistanceFrom:waypCLL];

		if (dist < [current horizontalAccuracy]) {
			[self.isHereSwitch setOn:YES animated:NO];
		} else {
			[self.isHereSwitch setOn:NO animated:NO];	
		}
	}
	 
	[super viewDidAppear:animated];
						   
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}


@end
