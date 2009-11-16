//
//  Test1AppDelegate.h
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface Test1AppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
	UITabBarController *rootNavController;
}

@property (nonatomic, retain) IBOutlet UITabBarController *rootNavController;
@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

