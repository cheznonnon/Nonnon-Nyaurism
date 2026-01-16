// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@interface NonnonStub : NSView

@property (nonatomic,assign) id delegate;

@end


@implementation NonnonStub


@synthesize delegate;




- init
{
	self = [super init];
	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
	}

	return self;
}




- (void) updateTrackingAreas
{
//return;

	int options = (
		NSTrackingMouseEnteredAndExited |
		NSTrackingMouseMoved            |
		NSTrackingActiveAlways          |
		NSTrackingActiveInActiveApp
	);

	NSTrackingArea *trackingArea = [
		[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:options
			       owner:self
			    userInfo:nil
	];

	[self addTrackingArea:trackingArea];

}

- (void) mouseEntered:(NSEvent *)theEvent
{
//NSLog(@"mouseEntered");

	// [Needed] : NSTrackingMouseEnteredAndExited

	if ( n_false == n_mac_window_is_keywindow( self.window ) )
	{
		[self.window makeKeyWindow];
	}

}

- (void) mouseExited:(NSEvent *)theEvent
{
//NSLog(@"mouseExited");

	// [Needed] : NSTrackingMouseEnteredAndExited

}




- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
//NSLog( @"acceptsFirstMouse" );

	if ( n_mac_window_is_hovered( n_nyaurism->plotter ) ) { return NO; }


	return YES;
}




-(NSDragOperation) draggingEntered:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingEntered" );

	// [!] : call when hovered

	return NSDragOperationCopy;
}

-(void) draggingExited:( id <NSDraggingInfo> )sender
{
//NSLog( @"draggingExited" );
}

-(BOOL) prepareForDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"prepareForDragOperation" );

	return YES;
}

-(BOOL) performDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"performDragOperation" );

	return YES;
}

-(void) concludeDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"concludeDragOperation" );

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

	[self.delegate NonnonDragAndDrop_dropped:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	return NSDragOperationCopy;
}
@end


