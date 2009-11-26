//
//  GroupiesDetailViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "FeedDetailViewController.h"
#import "BurbleDataManager.h"

@implementation FeedDetailViewController

@synthesize table;
@synthesize functions;
@synthesize message;

-(void)viewWillAppear:(BOOL)animated {
	[super viewWillAppear:animated];
}

/*
 // The designated initializer.  Override if you create the controller programmatically and want to perform customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
- (void)viewDidAppear:(BOOL)animated {
	
 	
}
 */

// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
   [super viewDidLoad];
}


/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	self.table = nil;
	self.functions = nil;
	
	[super viewDidUnload];
}


- (void)dealloc {
    [table release];
	[functions release];
	
	[super dealloc];
}


#pragma mark -
#pragma mark Table View Data Source Methods

//This draws our list table for all of our Groupies

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return 5;
}


-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *FunctionCell = @"FunctionCell";
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:FunctionCell];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:FunctionCell] autorelease];
	}
	NSUInteger row = [indexPath row];
	cell.textLabel.text = [functions objectAtIndex:row];
	if (row > 1) cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
	return cell;
}

#pragma mark -
#pragma mark Table View Delegate Methods

//This handles what happens when Functions cells are tapped
/*
-(NSIndexPath *)tableView:(UITableView *)tableView willSelectRowAtIndexPath: (NSIndexPath *)indexPath {
	[search resignFirstResponder];
	return indexPath;
}
 */

-(void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [tableView cellForRowAtIndexPath:indexPath];
	
}

//HAVE TO DO THIS WITH GROUPIE NAMES, TOO!
-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	NSString *hey = [[NSString alloc] initWithFormat: @"You've selected: Cool"];
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Friend selected" message:hey delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[hey release];
	[alert release];
	//[tableView deselectRowAtIndexPath:indexPath animated:YES];
}
	


@end
