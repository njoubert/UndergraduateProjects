//
//  AddWaypointOverlayView.m
//  Test1
//
//  Created by Niels Joubert on 11/17/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "AddWaypointOverlayView.h"


@implementation AddWaypointOverlayView

-(id)initWithFrame:(CGRect)frame {
	if (self = [super initWithFrame:frame]) {
		UIImageView *imgView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"59-flag.png"]];
		imgView.frame = CGRectMake(frame.size.width/2-2, frame.size.height/2-26, 18, 26);
		[self addSubview:imgView];
		[self setOpaque:NO];
	}
	return self;
}

-(void) drawRect:(CGRect)rect {
	CGContextRef context = UIGraphicsGetCurrentContext();
	
	CGContextSetRGBFillColor(context, 1.0, 0.0, 1.0, 1.0);
	UIColor *lineColor = [[UIColor alloc] initWithRed:1.0 green:0.0 blue:0.0 alpha:1.0];
	CGContextSetStrokeColorWithColor(context, lineColor.CGColor);
	CGContextSetLineWidth(context, 3.0);
	//left
	CGContextMoveToPoint(context, 0, self.frame.size.height/2-1);
	CGContextAddLineToPoint(context, self.frame.size.width/2 - 10, self.frame.size.height/2-1);
	//right
	CGContextMoveToPoint(context, self.frame.size.width/2 + 10, self.frame.size.height/2-1);
	CGContextAddLineToPoint(context, self.frame.size.width , self.frame.size.height/2-1);
	//top
	//CGContextMoveToPoint(context, self.frame.size.width/2-1, 0);
	//CGContextAddLineToPoint(context, self.frame.size.width/2-1, self.frame.size.height/2-36);
	//bottom
	CGContextMoveToPoint(context, self.frame.size.width/2-1, self.frame.size.height/2+10);
	CGContextAddLineToPoint(context, self.frame.size.width/2-1, self.frame.size.height);
	
	CGContextStrokePath(context);
	[lineColor release];
}


@end
