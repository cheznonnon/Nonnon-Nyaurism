// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : this is implemented for study
//
//	you can use "setEditable" YES to enable auto DnD


// [!] : Usage #1
//
//	1 : replace your NSImageView to NonnonImageView
//	2 : @interface AppDelegate () <NonnonDragAndDrop_delegate>
//	3 : implement NonnonDragAndDrop_dropped in AppDelegate
//	4 : call n_mac_nonnon_imageview() at applicationDidFinishLaunching


// [!] : Usage #2
//
//	1 : replace your NSImageView to NonnonImageView
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonImageView"
//	3 : modify behavior




#ifndef _H_NONNON_MAC_NONNON_IMAGEVIEW
#define _H_NONNON_MAC_NONNON_IMAGEVIEW




#import <Cocoa/Cocoa.h>


#include "_mac.c"




@interface NonnonImageView : NSImageView

@property (nonatomic,assign) id delegate;

@end


@implementation NonnonImageView


@synthesize delegate;


- init {
//NSLog(@"init");

	self = [super init];

	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
	}

	return self;
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

	// [!] : call when dropped

	//NSArray *array = [[sender draggingPasteboard] propertyListForType:NSPasteboardTypeFileURL];

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

//NSLog( @"%@", nsstr );

	[self.delegate NonnonDragAndDrop_dropped:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	return NSDragOperationCopy;
}

@end




NonnonImageView*
n_mac_nonnon_imageview
(
	id        self,
	NSWindow *window,
	CGFloat   x,
	CGFloat   y,
	CGFloat   sx,
	CGFloat   sy
)
{

	// [!] : usage
	//
	//    @property (strong) NonnonImageView *imageview;
	//    (strong) is important


	NonnonImageView *imageview;

	imageview = [[NonnonImageView alloc] init];


	// [!] : auto DnD is available
	//[imageview setEditable:YES];

	// [!] : without this, setFrame behaves badly
	imageview.translatesAutoresizingMaskIntoConstraints = NO;

	[imageview setFrame:NSMakeRect( x, y, sx, sy )];
	
	// [!] : after above code
	[[window contentView] addSubview:imageview];


	imageview.delegate = self;


	[imageview setTarget:self];


	return imageview;
}




#endif // _H_NONNON_MAC_NONNON_IMAGEVIEW


