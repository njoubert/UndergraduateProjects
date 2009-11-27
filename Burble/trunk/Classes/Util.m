//
//  Util.m
//  Test1
//
//  Created by Niels Joubert on 11/26/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "Util.h"


@implementation Util

+(NSString*)prettyTimeAgo:(NSDate*)past {
	NSString* retval;
	int seconds;
	int minutes;
	int hours;
	int days;
	
	NSTimeInterval interval = [past timeIntervalSinceNow];
	NSLog(@"interval: %f", interval);
	seconds = -1*interval;
	minutes = seconds / 60;
	hours = minutes / 60;
	minutes = minutes % 60;
	days = hours / 24;
	hours = hours % 24;

	NSString* dStr;
	NSString* hStr;
	NSString* mStr;
	
	if (days > 1) {
		dStr = @"days";
	} else {
		dStr = @"day";
	}
	if (hours > 1) {
		hStr = @"hours";
	} else {
		hStr = @"hour";
	}
	if (minutes > 1) {
		mStr = @"mins";
	} else {
		mStr = @"min";
	}
		
	if (days > 0) {
		retval = [NSString stringWithFormat:@"%d %@, %d %@", days, dStr, hours, hStr];
	} else if (hours > 0) {
		retval = [NSString stringWithFormat:@"%d %@, %d %@", hours, hStr, minutes, mStr];
	} else {
		retval = [NSString stringWithFormat:@"%d %@", minutes, mStr];		  
	}
	return retval;
}
+(NSString*)prettyDistanceInMeters:(double)dist {
	int meters = dist;
	meters = meters % 1000;
	int kilometers = dist / 1000;
	NSString* retval;
	if (kilometers > 0) {
		double kilo = kilometers + ((double) meters) / 1000;
		retval = [NSString stringWithFormat:@"%0.2f km", kilo];
	} else {
		retval = [NSString stringWithFormat:@"%d m", meters];
	}
	return retval;
}
@end
