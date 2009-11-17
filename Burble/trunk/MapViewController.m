//
//  MapViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MapViewController.h"

@implementation MapViewController

@synthesize myMap;
@synthesize mapTypeSegmentedControl;

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
    [super viewDidLoad];
	myMap.showsUserLocation = true;
	
}

- (void)viewDidAppear:(BOOL)animated {
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	if ([dataManager isFirstLaunch]) {
		NSString *message= [[NSString alloc] initWithFormat:@"You're being registered with our backend now. Your GUID is: %@", [dataManager getGUID]];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Welcome to Burble!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];
	} else if ([dataManager isRegistered]) {
		NSString *message= [[NSString alloc] initWithFormat:@"Hi %@", [dataManager getName]];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Welcome Back!" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];
	}
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

-(IBAction)waypointButtonPressed {
	NSString *message= [[NSString alloc] initWithString: @"Waypoint added"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: @"Waypoint" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];
}

-(IBAction)selectorButtonPressed {
	if (mapTypeSegmentedControl.selectedSegmentIndex == 0) {
		myMap.mapType = MKMapTypeStandard;
	} else {
		myMap.mapType = MKMapTypeSatellite;
	}	
}

-(IBAction)locateButtonPressed {
	CLLocation *location = myMap.userLocation.location;
	if (location.horizontalAccuracy > 0) {
	
		MKCoordinateRegion myRegion;
		MKCoordinateSpan mySpan;
		myRegion.center = location.coordinate;
		mySpan.latitudeDelta = (CLLocationDegrees) 0.08;
		mySpan.longitudeDelta = (CLLocationDegrees) 0.08;
		myRegion.span = mySpan;
		[myMap setRegion: myRegion animated: true];
		
	} else {
		
		NSString *message= [[NSString alloc] initWithString: @"We couldn't determine your location! Wait a minute and try again."];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Location not available" message:message delegate:nil cancelButtonTitle:@"Done" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];	
		
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
