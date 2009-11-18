//
//  AddWaypointViewController.h
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MapKit/MapKit.h>
#import <MapKit/MKTypes.h>
#import <MapKit/MKGeometry.h>
#import <MapKit/MKMapView.h>


@interface AddWaypointViewController : UIViewController <UITextFieldDelegate> {
	CLLocationCoordinate2D waypointLocation;
	UITextField *nameField;
	UITextField *descField;
	UISwitch *isHereSwitch;
	UILabel *latLabel;
	UILabel *longLabel;
}

@property (nonatomic, retain) IBOutlet UITextField *nameField;
@property (nonatomic, retain) IBOutlet UITextField *descField;
@property (nonatomic, retain) IBOutlet UISwitch *isHereSwitch;

- (void)setWaypointLocation:(CLLocationCoordinate2D)cStruct;
- (IBAction) saveButtonPressed;
- (IBAction) cancelButtonPressed;

- (void)saveW;
- (void)cancelW;

@end
