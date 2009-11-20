//
//  XMLEventParser.m
//  Test1
//
//  Created by Niels Joubert on 11/20/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XMLEventParser.h"


@implementation XMLEventParser

-(id)initWithData:(NSData*)data {
	if (self = [super init]) {
		
		
		//THIS SHOWS THE XML IF YOU WANNA SEE IT
		/*
		NSString *message= [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle:@"RESULT" message:message delegate:nil cancelButtonTitle:@"Great!" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];	
		*/
		
		_hasError = NO;
		_error = nil;
		_currentElementText = nil;
		_currentElementName = nil;
		_parser = [[NSXMLParser alloc] initWithData:data];
		[_parser setDelegate:self];

		//[_parser setShouldResolveExternalEntities:YES];
		[_parser parse];

		
	}
	return self;
}

-(void)parser:(NSXMLParser *)parser foundCharacters:(NSString *)string {
	[_currentElementText appendString:string];
}

-(BOOL)hasError {
	return _hasError;
}
-(NSDictionary*) getError {
	if ([self hasError])
		return _error;
	return nil;
}
@end
