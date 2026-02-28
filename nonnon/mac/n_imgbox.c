// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC_N_IMGBOX
#define _H_NONNON_MAC_N_IMGBOX




#import <Cocoa/Cocoa.h>


#include "draw.c"
#include "image.c"




@interface NonnonImgbox : NSView

@property (nonatomic,assign) id delegate;

@property n_bmp *n_bmp_data;

@end


@implementation NonnonImgbox {

	CGPoint pt;
	CGFloat n_x;
	CGFloat n_y;

}


@synthesize delegate;

@synthesize n_bmp_data;




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{
		n_bmp_data = NULL;

		n_x = n_y = 0;

		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];
	}
	
	return self;
}




- (BOOL) isOpaque {
//NSLog( @" isOpaque " );
	return NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
//NSLog( @"drawRect" );

	if ( n_bmp_data == NULL ) { return; }

	NSRect rect = [super bounds];
	if ( FALSE == NSEqualRects( rect, dirtyRect ) ) { return; }
//NSLog( @"drawRect" );

	n_mac_draw_box( n_mac_nscolor_argb( 255, 128,128,128 ), rect );

	n_mac_image_nbmp_direct_draw( n_bmp_data, &rect, NO );

}




- (BOOL)acceptsFirstResponder
{
//NSLog(@"acceptsFirstResponder");
	return YES;
}

- (BOOL)becomeFirstResponder
{
//NSLog(@"becomeFirstResponder");
        return YES;
}

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d : Chars %@", event.keyCode, event.characters );

	[delegate keyDown:event];

}




- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"mouseUp");

	[delegate mouseUp:theEvent];

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	[delegate mouseDown:theEvent];

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog(@"mouseDragged");

	[delegate mouseDragged:theEvent];

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




#endif // _H_NONNON_MAC_N_IMGBOX


