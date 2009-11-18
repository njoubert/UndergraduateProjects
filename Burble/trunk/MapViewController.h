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
#import "BurbleDataManager.h"
#import "RegisterViewController.h"
#import "AddWaypointViewController.h"

@interface MapViewController : UIViewController {
	MKMapView *myMap;
	UISegmentedControl *mapTypeSegmentedControl;
	UILabel *groupLabel;
}

@property (nonatomic, retain) IBOutlet UINavigationBar *topBar;
@property (nonatomic, retain) IBOutlet MKMapView *myMap;
@property (nonatomic, retain) IBOutlet UISegmentedControl *mapTypeSegmentedControl;
@property (nonatomic, retain) IBOutlet UILabel *groupLabel;


-(IBAction)waypointButtonPressed;
-(IBAction)selectorButtonPressed;
-(IBAction)locateButtonPressed;

@end
