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

#import "BurbleDataManager.h"
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

- (void)addAnnotations {
	BurbleDataManager *dM = [BurbleDataManager sharedDataManager];
	
	NSArray* wayPoints = [dM getWaypoints];
	for (int i = 0; i < [wayPoints count]; i++) {
		[myMap addAnnotation:[wayPoints objectAtIndex:i]];
	}

	if ([dM getLocation] != nil && [dM getLocation].horizontalAccuracy > 0)
		[myMap addAnnotation:[dM getMe]];
	
	if (![dM isInGroup])
		return;
	NSArray* groupMembers = [dM getGroupMembers];
	int myUid = [dM getUid];
	Person* p;
	for (int i = 0; i < [groupMembers count]; i++) {
		p = [groupMembers objectAtIndex:i];
		if (p.uid != myUid) 
			[myMap addAnnotation:p];
	}
}

- (void)viewDidAppear:(BOOL)animated {
	[self refreshView];
	BurbleDataManager* dm = [BurbleDataManager sharedDataManager];
	if (dm.hasLastMapRegion) {
		[myMap setRegion:dm.lastMapRegion animated: NO];
		myMap.mapType = dm.lastMapType;
		if (dm.lastMapType == MKMapTypeHybrid) {
			mapTypeSegmentedControl.selectedSegmentIndex = 1;
		}
	}
	[[BurbleDataManager sharedDataManager] startDownloadGroupMembersAndCall:self withSelector:@selector(refreshView)];
	[[BurbleDataManager sharedDataManager] startDownloadWaypointsAndCall:self withSelector:@selector(refreshView)];
}
- (void) viewDidDisappear:(BOOL)animated {
	[[BurbleDataManager sharedDataManager] presistMapState:myMap.region withType:myMap.mapType];
}

- (void) refreshView {
	if ([[BurbleDataManager sharedDataManager] isInGroup]) {
		waypointButton.enabled = YES;
		groupLabel.text = [[NSString alloc] initWithFormat:@"In %@!", [[BurbleDataManager sharedDataManager] getMyGroup].name];
	} else {
		waypointButton.enabled = NO;
		groupLabel.text = [[NSString alloc] initWithFormat:@"Join a Group %@!", [[BurbleDataManager sharedDataManager] getFirstName]];
	}
	[myMap removeAnnotations:[myMap annotations]];
	[self addAnnotations];
}

/*
 ================================================================================
					WAYPOINT ACTIONS
 ================================================================================ 
 */
#pragma mark -
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
#pragma mark -
-(IBAction)selectorButtonPressed {
	if (mapTypeSegmentedControl.selectedSegmentIndex == 0) {
		myMap.mapType = MKMapTypeStandard;
	} else {
		myMap.mapType = MKMapTypeHybrid;
	}	
}

-(void)panMapToLocation:(CLLocationCoordinate2D) coord{

	MKCoordinateRegion myRegion;
	MKCoordinateSpan mySpan;
	myRegion.center = coord;
	MKCoordinateSpan currentSpan = myMap.region.span;
	if (currentSpan.latitudeDelta > 1.6 || currentSpan.longitudeDelta > 1.6) {
		mySpan.latitudeDelta = (CLLocationDegrees) 0.05;
		mySpan.longitudeDelta = (CLLocationDegrees) 0.05;
		myRegion.span = mySpan;
	} else {
		myRegion.span = currentSpan;
	}
	
	[myMap setRegion: myRegion animated: true];
	
}

-(IBAction)locateButtonPressed {
	CLLocation *location = [BurbleDataManager sharedDataManager].lastKnownLocation;
	if (location.horizontalAccuracy > 0) {
	
		[self panMapToLocation:location.coordinate];
		
	} else {
		
		NSString *message= [[NSString alloc] initWithString: @"We couldn't determine your location! Wait a minute and try again."];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Location not available" message:message delegate:nil cancelButtonTitle:@"Done" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];	
		
	}
}

/*
 ================================================================================
 LOCATE ACTIONS
 ================================================================================ 
 */

#pragma mark -

-(void)locatePersonOnMap:(Person*)person {
	if (person == nil || person.position == nil)
		return;
	[self panMapToLocation:[person.position getLocation].coordinate];	
	[myMap selectAnnotation:person animated:YES];	
}
-(void)locateWaypointOnMap:(Waypoint*)waypt {
	if (waypt == nil)
		return;
	[self panMapToLocation:waypt.coordinate];
	[myMap selectAnnotation:waypt animated:YES];
}

/*
 ================================================================================
 DEFAULT STUFF
 ================================================================================ 
 */
#pragma mark -

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
	[[BurbleDataManager sharedDataManager] presistMapState:myMap.region withType:myMap.mapType];
}


- (void)dealloc {
    [super dealloc];
}

#pragma mark MKMapViewDelegate
#pragma mark -

-(MKAnnotationView *)mapView:(MKMapView *)mapView viewForAnnotation:(id <MKAnnotation>)annotation {
	MKPinAnnotationView *view = nil;
	if ([annotation isKindOfClass:[Waypoint class]]) {
		
		view = (MKPinAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:@"waypoints"];
		if (nil == view)
			view = [[[MKPinAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"waypoints"] autorelease];
		if ([((Waypoint*) annotation) isMine]) {
			[view setPinColor:MKPinAnnotationColorRed];
		} else {
			[view setPinColor:MKPinAnnotationColorPurple];
		}
		[view setCanShowCallout:YES];
		[view setAnimatesDrop:NO];
		
	} else if ([annotation isKindOfClass:[Person class]]) {
		
		view = (MKPinAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:@"people"];
		if (nil == view)
			view = [[[MKAnnotationView alloc] initWithAnnotation:annotation reuseIdentifier:@"people"] autorelease];
		view.image = [UIImage imageNamed:@"dot.png"];
		[view setCanShowCallout:YES];
		
	} else  {
		NSLog(@"user location annotiation");
		//CLLocation *location = [[CLLocation alloc] initWithLatitude:annotation.coordinate.latitude longitude:annotation.coordinate.longitude];
		//[self setCurrentLocation:location];
	}
	return view;
}

@end
