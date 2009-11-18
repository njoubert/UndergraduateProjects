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
@synthesize groupLabel;

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
	self.title = @"Map View";
	
	//Set the button that will appear as the "Back" button on any view that is pushed on top of this navigation controller.
	UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:nil action:nil];
	self.navigationItem.backBarButtonItem = backButton;
	[backButton release];
	
	UIBarButtonItem *waypointButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"map-marker.png"] style:UIBarButtonItemStylePlain target:self action:@selector(waypointButtonPressed)];
	UIBarButtonItem *locateButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"74-location.png"] style:UIBarButtonItemStylePlain target:self action:@selector(locateButtonPressed)];
	self.navigationItem.leftBarButtonItem = locateButton;
	self.navigationItem.rightBarButtonItem = waypointButton;
	
}

- (void)viewDidAppear:(BOOL)animated {
	groupLabel.text = [[NSString alloc] initWithFormat:@"%@, Join a Group!", [[BurbleDataManager sharedDataManager] getName]];
}

/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

-(IBAction)waypointButtonPressed {
	/*
	NSString *message= [[NSString alloc] initWithString: @"Waypoint added"];
	UIAlertView *alert = [[UIAlertView alloc]
						  initWithTitle: @"Waypoint" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
	[message release];
	*/
	
	
	//UIView* overlay = [[UIView alloc] initWithFrame:myMap.frame];
	//[overlay setOpaque:NO];
	//[self.view addSubview:overlay];
	
	AddWaypointViewController *wC = [[AddWaypointViewController alloc] initWithNibName:nil bundle:nil];
	[self.navigationController pushViewController:wC animated:YES];
	
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
