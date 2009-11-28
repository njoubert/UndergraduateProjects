//
//  MapViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>
#import "AddWaypointOverlayView.h"
#import "Group.h"
#import "Person.h"
#import "Waypoint.h"

#define kMapTitle			@"Map"
#define kAddWaypointTitle	@"Add Waypoint"

@interface MapViewController : UIViewController <MKMapViewDelegate> {
	MKMapView *myMap;
	UISegmentedControl *mapTypeSegmentedControl;
	UILabel *groupLabel;
	AddWaypointOverlayView *waypointOverlay;
	UIBarButtonItem *locateButton;
	UIBarButtonItem *waypointButton;
	UIBarButtonItem *cancelWaypointButton;
	UIBarButtonItem *approveWaypointButton;
}

@property (nonatomic, retain) IBOutlet UINavigationBar *topBar;
@property (nonatomic, retain) IBOutlet MKMapView *myMap;
@property (nonatomic, retain) IBOutlet UISegmentedControl *mapTypeSegmentedControl;
@property (nonatomic, retain) IBOutlet UILabel *groupLabel;

-(void)locatePersonOnMap:(Person*)person;
-(void)locateWaypointOnMap:(Waypoint*)waypt;

-(IBAction)waypointButtonPressed;
-(IBAction)cancelWaypointButtonPressed;
-(IBAction)approveWaypointButtonPressed;
-(IBAction)selectorButtonPressed;
-(IBAction)locateButtonPressed;

- (void) refreshView;

@end
