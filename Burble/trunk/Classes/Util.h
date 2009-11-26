//
//  Util.h
//  Test1
//
//  Created by Niels Joubert on 11/26/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface Util : NSObject {

}
+(NSString*)prettyTimeAgo:(NSDate*)past;
+(NSString*)prettyDistanceInMeters:(double)dist;
@end
