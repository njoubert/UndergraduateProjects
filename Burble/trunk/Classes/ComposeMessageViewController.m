//
//  ComposeMessageViewController.m
//  Test1
//
//  Created by Niels Joubert on 11/30/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "ComposeMessageViewController.h"
#import "Message.h"
#import "BurbleDataManager.h"

@implementation ComposeMessageViewController

@synthesize toLabel, messageTextView;

-(id)initWithListOfPeople:(NSArray*)toSelectFrom selected:(NSMutableArray*)selectedPeopleOrNil withCallbackTarget:(id)t selector:(SEL)sel {
	if (self = [super initWithNibName:@"ComposeMessageViewController" bundle:nil]) {
		if (selectedPeopleOrNil == nil)
			_recipients = [[NSMutableArray alloc] init];
		else
			_recipients = [selectedPeopleOrNil retain];
		_possibleRecipients = [toSelectFrom retain];
		
		_target = t;
		_selector = sel;
    }
    return self;	
}

- (IBAction) showSelectRecipients {
	selectRecipientsVC = [[SelectRecipients alloc] initWithListOfPeople:_possibleRecipients selected:_recipients withCallbackTarget:nil selector:nil];	
	[self.navigationController pushViewController:selectRecipientsVC animated:YES];
}

- (void)viewDidAppear:(BOOL)animated {
	if (selectRecipientsVC != nil) {
		[selectRecipientsVC release];	//if we are coming back from changing the recipients...
		selectRecipientsVC = nil;
	}
	int numR = [_recipients count];
	if (numR > 1 || numR == 0) 
		toLabel.text = [NSString stringWithFormat:@"%d recipients", numR];
	else
		toLabel.text = [NSString stringWithFormat:@"%d recipient", numR];
	if (numR == 0)
		sendButton.enabled = NO;
	else
		sendButton.enabled = YES;

	[messageTextView becomeFirstResponder];
}
// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	self.title = @"Compose";
	sendButton = [[UIBarButtonItem alloc] initWithTitle:@"Send" style:UIBarButtonItemStyleDone target:self action:@selector(sendButtonPressed)];
	self.navigationItem.rightBarButtonItem = sendButton;
	cancelButton = [[UIBarButtonItem alloc] initWithTitle:@"Cancel" style:UIBarButtonItemStylePlain target:self action:@selector(leaveWindow)];
	self.navigationItem.leftBarButtonItem = cancelButton;
    [super viewDidLoad];
}

-(void)leaveWindow {
	[_recipients release];
	[_possibleRecipients release];
	_recipients = nil;
	_possibleRecipients = nil;
	[self.navigationController popViewControllerAnimated:YES];
	if (_target != nil && [_target respondsToSelector:_selector])
		[_target performSelector:_selector];
	
}

- (void)sendButtonPressed {
	[messageTextView resignFirstResponder];
	
	Message* msg = [[Message alloc] initWithText:messageTextView.text];
	[msg appendReceivers:_recipients];
	
	BOOL success = [[BurbleDataManager sharedDataManager] sendMessage:msg];
	if (success) {
		
		NSString *message= [[NSString alloc] initWithFormat:@"Sent to %d recipients. It will be delivered momentarily.", [_recipients count]];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Message Sent" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
		[message release];
		
		[self leaveWindow];
		
	} else {
			
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Error!" message:@"We could not open a socket to the server" delegate:self cancelButtonTitle:@"Continue" otherButtonTitles:nil];
		[alert addButtonWithTitle:@"Retry"];
		[alert show];
		[alert release];
		
	}
}

-(void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	if (buttonIndex == 1) {
		[self sendButtonPressed];
	} else {
		[self leaveWindow];
	}
}


- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	/*
	if (textField == nameField) {
		
		NSString *name = [[NSString alloc] initWithString:nameField.text];
		
		NSString *message= [[NSString alloc] initWithFormat:@"You are registering the name %@", name];
		UIAlertView *alert = [[UIAlertView alloc]
							  initWithTitle: @"Confirm!" message:message delegate:self cancelButtonTitle:@"Wait, no!" otherButtonTitles:nil];
		[alert addButtonWithTitle:@"Do it!"];
		[alert show];
		[alert release];
		[message release];
	}
	 */
	return YES;
}

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
	[_recipients release];
	[_possibleRecipients release];
	[selectRecipientsVC release];
	[sendButton release];
	[messageTextView release];
	[toLabel release];
    [super dealloc];
}


@end
