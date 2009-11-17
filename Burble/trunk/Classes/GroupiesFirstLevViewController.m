//
//  GroupiesFirstLevViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "GroupiesFirstLevViewController.h"
#import "SecondLevelViewController.h"

@implementation GroupiesFirstLevViewController

@synthesize controllers;

-(void)viewDidLoad {
	self.title = @"First Level";
	NSMutableArray *array = [[NSMutableArray alloc] init];
	self.controllers = array;
	[array release];
	[super viewDidLoad];
}

-(void)viewDidUnload {
	self.controllers = nil;
	[super viewDidUnload];
}

-(void)dealloc {
	[controllers release];
	[super dealloc];
}

#pragma mark -
#pragma mark Table Data Source Methods
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [self.controllers count];
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *FirstLevelCell= @"FirstLevelCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:FirstLevelCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:FirstLevelCell] autorelease];
	}
	
	NSUInteger row = [indexPath row];
	SecondLevelViewController *controller = [controllers objectAtIndex:row];
	cell.textLabel.text = controller.title;
	cell.imageView.image = controller.rowImage;
	cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
	return cell;
}

#pragma mark -
#pragma mark Table View Delegate Methods

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	NSUInteger row = [indexPath row];
	SecondLevelViewController *nextController = [self.controllers objectAtIndex:row];
	[self.navigationController pushViewController:nextController animated:YES];
}							  							  

@end
