//
//  RemoteModel.m
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "RemoteModel.h"

@implementation RemoteModel

- (void)convertToData:(RPCPostData*)pData {
	
}

- (RPCPostRequest*) getPostRequestToMethod:(NSString*)urlString withBaseUrl:(NSURL*)baseUrl {
	NSURL *regUrl = [[NSURL alloc] initWithString:urlString relativeToURL:baseUrl];
	RPCPostRequest* request = [[RPCPostRequest alloc] initWithURL:regUrl cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData timeoutInterval:20];	
	RPCPostData* pData = [[RPCPostData alloc] init];
	[self convertToData:pData];
	[request setHTTPBodyPostData:pData];
	[regUrl release];
	return request;
}


@end
