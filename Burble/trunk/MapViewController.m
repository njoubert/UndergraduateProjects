//
//  MapViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MapViewController.h"
#import "RegisterViewController.h"
#import "AddWaypointViewController.h"

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
	numOfWaypoints = 0;
	myMap.showsUserLocation = true;
	self.title = @"Map";
	
	//Set the button that will appear as the "Back" button on any view that is pushed on top of this navigation controller.
	UIBarButtonItem *backButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:nil action:nil];
	self.navigationItem.backBarButtonItem = backButton;
	[backButton release];
	
	//Create buttons
	cancelWaypointButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(cancelWaypointButtonPressed)];
	approveWaypointButton = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(approveWaypointButtonPressed)];
	waypointButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"map-marker.png"] style:UIBarButtonItemStylePlain target:self action:@selector(waypointButtonPressed)];
	locateButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"74-location.png"] style:UIBarButtonItemStylePlain target:self action:@selector(locateButtonPressed)];
	
	self.navigationItem.leftBarButtonItem = locateButton;
	self.navigationItem.rightBarButtonItem = waypointButton;
	
	[self refreshView];
	
}

- (void)refreshWaypoints {
	BurbleDataManager *dM = [BurbleDataManager sharedDataManager];
	if ([dM getWaypointCount] > numOfWaypoints) {
		NSArray* wayPoints = [dM getWaypoints];
		int i;
		for (i = numOfWaypoints; i < [wayPoints count]; i++) {
			[myMap addAnnotation:[wayPoints objectAtIndex:i]];
		}
		numOfWaypoints = i;
	}
}

- (void)viewDidAppear:(BOOL)animated {
	[self refreshView];
}

- (void) refreshView {
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		waypointButton.enabled = YES;
		groupLabel.text = [[NSString alloc] initWithFormat:@"In %@!", [[BurbleDataManager sharedDataManager] getMyGroup].name];
	} else {
		waypointButton.enabled = NO;
		groupLabel.text = [[NSString alloc] initWithFormat:@"Join a Group %@!", [[BurbleDataManager sharedDataManager] getFirstName]];
	}
	[self refreshWaypoints];
}


/*
 ================================================================================
					WAYPOINT ACTIONS
 ================================================================================ 
 */

-(IBAction)waypointButtonPressed {
	
	self.title = kAddWaypointTitle;
	self.navigationItem.leftBarButtonItem = cancelWaypointButton;
	self.navigationItem.rightBarButtonItem = approveWaypointButton;
	waypointOverlay = [[AddWaypointOverlayView alloc] initWithFrame:myMap.bounds];
	[myMap addSubview:waypointOverlay];
	
}

-(void)removeWayPOverlay {
	[waypointOverlay removeFromSuperview];
	[waypointOverlay release];
	self.title = kMapTitle;
	self.navigationItem.rightBarButtonItem = waypointButton;
	self.navigationItem.leftBarButtonItem = locateButton;
	
}

-(IBAction)cancelWaypointButtonPressed {
	[self removeWayPOverlay];
}
-(IBAction)approveWaypointButtonPressed {
	[self removeWayPOverlay];
	
	
	AddWaypointViewController *approveController = [[[AddWaypointViewController alloc] initWithNibName:@"AddWaypointViewController" bundle:nil] autorelease];
	CLLocationCoordinate2D centerCoordinate = [myMap centerCoordinate];
	[approveController setWaypointLocation:centerCoordinate];
	[approveController setMapViewToRefresh:self];
	[self presentModalViewController:approveController animated:YES];
	
}


/*
 ================================================================================
							OTHER ACTIONS
 ================================================================================ 
 */

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
		mySpan.latitudeDelta = (CLLocationDegrees) 0.03;
		mySpan.longitudeDelta = (CLLocationDegrees) 0.03;
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
