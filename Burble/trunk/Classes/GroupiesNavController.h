//
//  GroupiesNavController.h
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface GroupiesNavController : UIViewController {
	UINavigationController *rootController;
}
@property (nonatomic, retain) IBOutlet UINavigationController *rootController;
@end
