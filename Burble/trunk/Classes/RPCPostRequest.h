//
//  RPCPostRequest.h
//  Test1
//
//  Created by Niels Joubert on 11/19/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RPCPostData.h"
//This is our modified NSMutableURLRequest that automatically initializes it to a POST request to our server.
@interface RPCPostRequest : NSMutableURLRequest {

}
-(id)initWithURL:(NSURL *)url cachePolicy:(NSURLRequestCachePolicy)cachePolicy timeoutInterval:(NSTimeInterval)timeoutInterval;
-(void)setHTTPBodyPostData:(RPCPostData*)pD;
@end
