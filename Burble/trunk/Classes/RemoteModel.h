#import <Foundation/Foundation.h>
#import "RPCPostData.h"
#import "RPCPostRequest.h"
//#import "BurbleDataManager.h"

/* 
 This is the protocol for any object that corresponds to an ActiveRecord model on the server.
 This means that you can convert it to an NSData object and send it to the server.
 */
@interface RemoteModel : NSObject {
	
}
- (void)convertToData:(RPCPostData*)pData;

// A convenience method that replies on convertToData
// It converts the object to data, and sets it as the body 
// in a POST request to the given http://baseURL/urlString
- (RPCPostRequest*) getPostRequestToMethod:(NSString*)urlString withBaseUrl:(NSURL*)baseUrl;

@end

