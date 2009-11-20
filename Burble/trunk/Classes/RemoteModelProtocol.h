#import <Foundation/Foundation.h>
#import "RPCPostData.h"

/*
 
 This is the protocol for any object that corresponds to an ActiveRecord model on the server.
 This means that you can convert it to an NSData object and send it to the server.
 */
@protocol RemoteModelProtocol
- (void)convertToData:(RPCPostData*)pData;
@end
