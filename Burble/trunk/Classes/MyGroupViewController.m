//
//  MyGroupViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/4/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "MyGroupViewController.h"
#import "BurbleDataManager.h"

@implementation MyGroupViewController

@synthesize groupTextField;
@synthesize groupMsgText;
@synthesize groupString;
@synthesize groupMsgString;
@synthesize myGroupView;
@synthesize createGroupView;
@synthesize inviteToGroupView;
@synthesize messageGroupView;
@synthesize groupLabel;
@synthesize groupMsgLabel;
@synthesize myGroup;


- (BOOL)textFieldShouldReturn:(UITextField *)theTextField {
	if(theTextField == groupTextField) {
		[groupTextField resignFirstResponder];
	}
	return YES;
}

-(BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text{
	if([text isEqualToString:@"\n"]){
		[textView resignFirstResponder];
		return NO;
	}
	return YES;
}

- (void)textViewDidBeginEditing:(UITextView *)textView {
	if([@"Press to enter text." isEqualToString:textView.text])
		textView.text = @"";
}

//Creates new group with specified title.
-(IBAction)createGroup:(id)sender {
	self.groupString = groupTextField.text;
	
	NSString *nameString = groupString;
	if([nameString length] == 0) {
		
		UIAlertView *groupCreationFailedAlert = [[UIAlertView alloc]
									  initWithTitle: @"Group creation failed" message:@"You must enter a title for your group!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[groupCreationFailedAlert show];
		[groupCreationFailedAlert release];
		
	} else {
		
		NSString* nameString = [[NSString alloc] initWithString:groupTextField.text];
		
		if ([[BurbleDataManager sharedDataManager] startCreateGroup:nameString withDesc:@"" target:self selector:@selector(groupCreated:)]) {
			//we started connection, now we just wait for groupCreated to be called.
			
		} else {
			[[BurbleDataManager sharedDataManager] messageForCouldNotConnectToServer];
		}
		[nameString release];
		
	}
}

-(void)groupCreated:(Group*)returnValue {
	if (returnValue != nil) {
		groupTextField.text = @"";	
		self.view = myGroupView;
		groupLabel.text = returnValue.name;
	}
}

-(IBAction)backToGroup:(id)sender {
	self.view = myGroupView;
}

-(IBAction)sendMsgToGroup:(id)sender {
	groupMsgString = groupMsgText.text;
	
	if(![@"" isEqualToString:groupMsgString] && ![@"Press to enter text." isEqualToString:groupMsgString]){
	
		self.view = myGroupView;
	
		UIAlertView *msgSentAlert = [[UIAlertView alloc]
											 initWithTitle: @"Message sent!" message:nil delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[msgSentAlert show];
		[msgSentAlert release];
	}else{
		
		UIAlertView *msgFailedAlert = [[UIAlertView alloc]
									 initWithTitle: @"Message was not sent!" message:@"Please enter text" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[msgFailedAlert show];
		[msgFailedAlert release];
	}
}

-(IBAction)leaveGroup:(id)sender {
	self.view = createGroupView;
}

-(IBAction)inviteToGroup:(id)sender {
	self.view = inviteToGroupView;
}

-(IBAction)messageGroup:(id)sender {
	groupMsgText.text = @"Press to enter text.";
	
	groupMsgLabel.text = [@"Msg to " stringByAppendingString:@"groupString"]; // groupString];
	
	self.view = messageGroupView;
}

-(IBAction)goToMyGroupView {
	self.view = myGroupView;
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


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"My Group";

	dataManager = [BurbleDataManager sharedDataManager];
	
	if (YES){ //[dataManager isInGroup]){  
		
		myGroup = [dataManager getMyGroup];
		groupString = [myGroup name];
		groupLabel.text = groupString;
		self.view = myGroupView;
		
	} else {
		
		self.view = createGroupView;
	
	}
	
	//This is displayed in the navigation title bar
	[super viewDidLoad];
}

/*
- (void)viewDidLoad {
	//loading up this particular user's friends
	dataManager = [BurbleDataManager sharedDataManager];
	NSArray *ar = [dataManager getFriends];
	self.people = ar; //this is an array of people objects
	[ar release];
	self.title = @"Groupies";
	
	[super viewDidLoad];
}
*/


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
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
	[groupTextField release];
	[groupMsgText release];
	[groupString release];
	[groupMsgString release];
	[myGroupView release];
	[createGroupView release];
	[inviteToGroupView release];
	[messageGroupView release];
	[groupLabel release];
	[groupMsgLabel release];
	[myGroup release];
    [super dealloc];
}
-(IBAction)inviteIdOneFUCKYEA {
	BurbleDataManager *dataManager = [BurbleDataManager sharedDataManager];
	if (![dataManager isInGroup])
		return;
	
	int gID = [dataManager getMyGroup].group_id;	

	Message* inviteMsg = [[Message alloc] initWithText:@"Join the group buddy!" group:gID];
	[inviteMsg appendReceiverUid:1];
	[dataManager sendMessage:inviteMsg];
	
}

@end
