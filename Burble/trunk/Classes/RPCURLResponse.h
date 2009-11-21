//
//  RPCURLResponse.h
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RPCURLResponse : NSObject {
	NSHTTPURLResponse* response;
	NSData* data;
	NSError* error;
}
@property (nonatomic, retain) NSHTTPURLResponse* response;
@property (nonatomic, retain) NSData* data;
@property (nonatomic, retain) NSError* error;
@end
