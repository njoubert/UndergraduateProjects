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
	
	currentPeople = [[NSMutableArray alloc] initWithCapacity:4];
	currentWaypoints = [[NSMutableArray alloc] initWithCapacity:4];
	currentMe = nil;
	
	//Create buttons
	cancelWaypointButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(cancelWaypointButtonPressed)];
	approveWaypointButton = [[UIBarButtonItem alloc] initWithTitle:@"Save" style:UIBarButtonItemStylePlain target:self action:@selector(approveWaypointButtonPressed)];
	waypointButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"map-marker.png"] style:UIBarButtonItemStylePlain target:self action:@selector(waypointButtonPressed)];
	locateButton = [[UIBarButtonItem alloc] initWithImage:[UIImage imageNamed:@"74-location.png"] style:UIBarButtonItemStylePlain target:self action:@selector(locateButtonPressed)];
	
	self.navigationItem.leftBarButtonItem = locateButton;
	self.navigationItem.rightBarButtonItem = waypointButton;
	
	[self refreshView];
}

- (void)reloadMyLocationAndAnnotation {
	BurbleDataManager *dM = [BurbleDataManager sharedDataManager];
	
	//First we update where I am
	if ([dM getLocation] != nil && [dM getLocation].horizontalAccuracy > 0) {
		if (currentMe == nil) {
			currentMe = [dM getMe];
			[myMap addAnnotation:currentMe];
		} else {
			[currentMe updateWith:[dM getMe]];
		}
		
	}
	
}
- (void)addAnnotations {
	BurbleDataManager *dM = [BurbleDataManager sharedDataManager];

	[self reloadMyLocationAndAnnotation];
	
	//Then we diff all the new waypoints
	NSArray* newWaypoints = [dM getWaypoints];
	for (Waypoint* cWP in newWaypoints) {		//only add new waypoints
		if (![currentWaypoints containsObject:cWP]) {
			[currentWaypoints addObject:cWP];
			[myMap addAnnotation:cWP];
		}
	}
	NSMutableArray *toDelete = [NSMutableArray array];
	for (Waypoint* cWP in currentWaypoints) {			//delete waypoints that no longer exist
		if (![newWaypoints containsObject:cWP]) {
			NSLog(@"removing waypoint from map");
			[myMap removeAnnotation:cWP];
			[toDelete addObject: cWP];
		}
	}
	[currentWaypoints removeObjectsInArray:toDelete];
	
	//Then we diff all the old waypoints
	if (![dM isInGroup]) {
		//ok remove all the current group members
		for (Person* p in currentPeople) {
			[myMap removeAnnotation:p];
			NSLog(@"removing all group members");
		}
		[currentPeople release];
		currentPeople = [[NSMutableArray alloc] initWithCapacity:4];
		
	} else {
		
		//Then we diff all the new waypoints
		NSArray* groupMembers = [dM getGroupMembers];
		int myUid = [dM getUid];
		for (Person* p in groupMembers) {		//only add new waypoints
			if (p.uid != myUid && ![currentPeople containsObject:p]) {
				[currentPeople addObject:p];
				[myMap addAnnotation:p];
			}
		}
		NSMutableArray *toDelete = [NSMutableArray array];
		for (Person* p in currentPeople) {			//delete waypoints that no longer exist
			if (![groupMembers containsObject:p]) {
				NSLog(@"removing person from map");
				[myMap removeAnnotation:p];
				[toDelete addObject: p];
			}
		}
		[currentPeople removeObjectsInArray:toDelete];
		
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

-(void)panMapToLocation:(CLLocationCoordinate2D) coord {
	
	MKCoordinateSpan currentSpan = myMap.region.span;	
	if (currentSpan.latitudeDelta > 1.6 || currentSpan.longitudeDelta > 1.6 ||
		currentSpan.latitudeDelta < 0.006 || currentSpan.longitudeDelta < 0.006) {
		MKCoordinateRegion myRegion;
		MKCoordinateSpan mySpan;
		myRegion.center = coord;
		mySpan.latitudeDelta = (CLLocationDegrees) 0.04;
		mySpan.longitudeDelta = (CLLocationDegrees) 0.04;
		myRegion.span = mySpan;
		[myMap setRegion: [myMap regionThatFits:myRegion] animated: true];

	} else {
		[myMap setCenterCoordinate:coord animated:YES];
	}
	

	
}

-(IBAction)locateButtonPressed {
	CLLocation* l = [BurbleDataManager sharedDataManager].lastKnownLocation;
	if (l.horizontalAccuracy > 0) {
	
		[self locateMeOnMap];
		
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

-(void)locateMeOnMap {
	[self reloadMyLocationAndAnnotation];
	[self panMapToLocation:currentMe.coordinate];
	[myMap selectAnnotation:currentMe animated:YES];
}
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
		//NSLog(@"user location annotiation");
		//CLLocation *location = [[CLLocation alloc] initWithLatitude:annotation.coordinate.latitude longitude:annotation.coordinate.longitude];
		//[self setCurrentLocation:location];
	}
	return view;
}

@end
