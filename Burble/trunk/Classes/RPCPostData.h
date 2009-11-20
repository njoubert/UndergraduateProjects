//
// RPCPostData.h
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


/*
 This wraps the functionality of sending parameters to the server.
 */
@interface RPCPostData : NSObject {
	NSMutableString* dataStr;	
}
- (NSString*) getString;
- (NSData*) getData;
- (void)appendValue:(NSString*)val forKey:(NSString*)key;
@end
