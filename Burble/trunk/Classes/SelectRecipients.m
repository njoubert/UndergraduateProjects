//
//  SelectRecipients.m
//  Test1
//
//  Created by Niels Joubert on 11/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "SelectRecipients.h"
#import "Person.h"

#define kDictPersonKey @"dictPerson"

@implementation SelectRecipients


-(id)initWithListOfPeople:(NSArray*)toSelectFrom selected:(NSMutableArray*)selectedPeopleOrNil withCallbackTarget:(id)t selector:(SEL)sel {
	if (self = [super initWithNibName:@"SelectRecipients" bundle:nil]) {
		if (selectedPeopleOrNil == nil)
			_selectedPeople = [[NSMutableArray alloc] init];
		else
			_selectedPeople = [selectedPeopleOrNil retain];
		_peopleToSelectFrom = [toSelectFrom retain];
		
		_target = t;
		_selector = sel;
		
		NSLog(@"Select View with %d selected out of %d", [_selectedPeople count], [_peopleToSelectFrom count]);
		
		//calculate the people to show
		_showThesePeople = [[NSMutableArray alloc] init];
		for (Person* p in _peopleToSelectFrom) {
			NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithCapacity:[_peopleToSelectFrom count]];
			[dict setObject:p forKey:kDictPersonKey];
			if ([_selectedPeople containsObject:p]) {
				NSLog(@"Already checked value");
				[dict setValue:[NSNumber numberWithBool:YES] forKey:@"checked"];
			} else {
				[dict setValue:[NSNumber numberWithBool:NO] forKey:@"checked"];	
			}
			[_showThesePeople addObject:dict];
		}
    }
    return self;	
}

//This is just a helper method, you are supposed to already have a pointer to this.
-(NSMutableArray*)getReferenceToSelectedPeople {
	return _selectedPeople;
}


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
 	doneButton = [[UIBarButtonItem alloc] initWithTitle:@"Done" style:UIBarButtonItemStyleBordered target:self action:@selector(doneButtonPressed)];
	self.navigationItem.rightBarButtonItem = doneButton;
	self.title = @"Recipients";
	[super viewDidLoad];
}

- (void)doneButtonPressed {
	if (_target != nil && [_target respondsToSelector:_selector])
		[_target performSelector:_selector];
	[self.navigationController popViewControllerAnimated:YES];
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}

- (void)dealloc {
	[_selectedPeople release];
	[_peopleToSelectFrom release];
	[_showThesePeople release];
    [super dealloc];
}

#pragma mark - UITableView delegate methods

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	return [_showThesePeople count];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	[self tableView: self.tableView accessoryButtonTappedForRowWithIndexPath: indexPath];
	[self.tableView deselectRowAtIndexPath:indexPath animated:YES];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
	static NSString *kCustomCellID = @"selectPeopleForMessageCell";
	
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:kCustomCellID];
	if (cell == nil) {
		cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:kCustomCellID] autorelease];
		cell.accessoryType = UITableViewCellAccessoryDisclosureIndicator;
		cell.selectionStyle = UITableViewCellSelectionStyleBlue;
	}
	
	NSMutableDictionary *item = [_showThesePeople objectAtIndex:indexPath.row];
	Person* p = [item objectForKey:kDictPersonKey];
	cell.textLabel.text = p.name;
	
	[item setObject:cell forKey:@"cell"];
	
	BOOL checked = [[item objectForKey:@"checked"] boolValue];
	UIImage *image = (checked) ? [UIImage imageNamed:@"checked.png"] : [UIImage imageNamed:@"unchecked.png"];
	
	UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
	CGRect frame = CGRectMake(0.0, 0.0, image.size.width, image.size.height);
	button.frame = frame;	// match the button's size with the image size
	
	[button setBackgroundImage:image forState:UIControlStateNormal];
	
	// set the button's target to this table view controller so we can interpret touch events and map that to a NSIndexSet
	[button addTarget:self action:@selector(checkButtonTapped:event:) forControlEvents:UIControlEventTouchUpInside];
	button.backgroundColor = [UIColor clearColor];
	cell.accessoryView = button;
	
	return cell;
}

//Helper method to figure out which checkmark was tapped
- (void)checkButtonTapped:(id)sender event:(id)event {
	NSSet *touches = [event allTouches];
	UITouch *touch = [touches anyObject];
	CGPoint currentTouchPosition = [touch locationInView:self.tableView];
	NSIndexPath *indexPath = [self.tableView indexPathForRowAtPoint: currentTouchPosition];
	if (indexPath != nil) {
		[self tableView: self.tableView accessoryButtonTappedForRowWithIndexPath: indexPath];
	}
}

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath {	
	NSMutableDictionary *item = [_showThesePeople objectAtIndex:indexPath.row];
	
	Person* p = [item objectForKey:kDictPersonKey];
	BOOL checked = [[item objectForKey:@"checked"] boolValue];
	BOOL nowChecked = !checked;
	[item setObject:[NSNumber numberWithBool:nowChecked] forKey:@"checked"];
	
	//Here we modify the list of selected people appropriately:
	if (nowChecked) {
		[_selectedPeople addObject: p];			//add it to the list
	} else {
		[_selectedPeople removeObject: p]; 		//remove it from the list
	}
	NSLog(@"We now have %d checked people.", [_selectedPeople count]);
	
	UITableViewCell *cell = [item objectForKey:@"cell"];
	UIButton *button = (UIButton *)cell.accessoryView;
	
	UIImage *newImage = (checked) ? [UIImage imageNamed:@"unchecked.png"] : [UIImage imageNamed:@"checked.png"];
	[button setBackgroundImage:newImage forState:UIControlStateNormal];
}



@end
