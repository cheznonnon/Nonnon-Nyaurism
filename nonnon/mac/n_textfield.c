// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSView to NonnonTextField
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonTextField"
//	3 : modify behavior




#ifndef _H_NONNON_MAC_NONNON_TEXTFIELD
#define _H_NONNON_MAC_NONNON_TEXTFIELD




#import <Cocoa/Cocoa.h>


#include "../neutral/bmp/all.c"
#include "../neutral/bmp/ui/roundframe.c"

#include "./_mac.c"
#include "./image.c"




NSString*
n_textfield_path( NSString *nsstr, BOOL folder_only )
{

	if ( folder_only )
	{
		n_posix_char *path = n_mac_nsstring2str( nsstr );
		if (
			( n_posix_stat_is_file( path ) )
			||
			(
				( n_string_path_ext_is_same_literal( ".app"      , path ) )
				||
				( n_string_path_ext_is_same_literal( ".xcodeproj", path ) )
			)
		)
		{
			n_posix_char *upper = n_string_path_upperfolder_new( path );
			n_string_path_free( path ); path = upper;
		}
		nsstr = n_mac_str2nsstring( path );
		n_string_path_free( path );
	}


	return nsstr;
}




@interface NonnonTextField : NSTextField
@end


@interface NonnonTextField ()

@property BOOL n_folder_only;

@end


@implementation NonnonTextField
{
	BOOL        is_hovered;
	NSTimer    *timer;
	BOOL        queue;
	n_bmp_fade  fade;
}


@synthesize n_folder_only;




- (id) initWithCoder : (NSCoder*) decoder
{

	self = [super initWithCoder:decoder];

	if ( self )
	{
		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];

		queue = FALSE;
		timer = n_mac_timer_init( self, @selector( n_timer_method ), 1000.0 / 30.0 );

		n_bmp_fade_init( &fade, n_bmp_black );

		[self setStringValue:@"Drop Here"];
	}

	return self;
}




- (void) n_timer_method
{

	if ( queue )
	{
		n_bmp_fade_engine( &fade, n_posix_true );
		[self display];
		if ( fade.stop ) { queue = FALSE; }
	}

}




- (void) drawRect : (NSRect) dirtyRect
{
//NSLog(@"drawRect");

	//[super drawRect:dirtyRect];


	// [x] : macOS Sonoma : dirtyRect has parent window size at the first time

	NSRect rect = [super bounds];//dirtyRect;


	// [!] : Metrics

	NSFont *font      = n_mac_stdfont();
	CGSize  font_size = n_mac_image_text_pixelsize( @"a", font );

	n_type_gfx sx = (n_type_gfx) rect.size.width;
	n_type_gfx sy = (n_type_gfx) rect.size.height;

	CGFloat o = 2;//offset;
	CGFloat m = 2;//margin;

	CGFloat minim = font_size.height + ( o * 2 );
	if ( ( sx < minim )||( sy < minim ) ) { return; }


	// [!] : Metrics 2 : Colors

	NSColor *nscolor_text  = [NSColor textColor];
	u32        color_bg    = n_mac_nscolor2argb( [NSColor textBackgroundColor] );
	u32        color_fg    = n_mac_nscolor2argb( nscolor_text );
	u32        color_frame = n_bmp_blend_pixel( color_bg, color_fg, 0.25 );

	if ( is_hovered )
	{
		u32 c = n_mac_nscolor2argb( [NSColor controlAccentColor] );
		    c = n_bmp_color_mac( c );
		    c = n_bmp_blend_pixel( c, color_bg, 0.5 );

		color_bg = n_bmp_blend_pixel( color_bg, c, (double) fade.percent * 0.01 );
		//color_bg = c;
	} else 
	if ( queue )
	{
		u32 c = n_mac_nscolor2argb( [NSColor controlAccentColor] );
		    c = n_bmp_color_mac( c );
		    c = n_bmp_blend_pixel( c, color_bg, 0.5 );

		color_bg = n_bmp_blend_pixel( c, color_bg, (double) fade.percent * 0.01 );
	}

	n_bmp frame; n_bmp_zero( &frame ); n_bmp_new_fast( &frame, sx,sy );
	n_bmp_flush( &frame, n_bmp_black_invisible );


	// [!] : Frame

	{
		u32 outer = color_frame;
		u32 inner = color_bg;//n_bmp_alpha_replace_pixel( color_bg, 0 );

		n_bmp_ui_roundframe( &frame, 0,0,sx,sy, o + m, m, outer, inner );

		NSImage *img = n_mac_image_nbmp2nsimage( &frame );
		[img drawInRect:rect];
	}


	// [!] : Text

	NSMutableDictionary *attr = [NSMutableDictionary dictionary];
	[attr setObject:font         forKey:NSFontAttributeName ];
	[attr setObject:nscolor_text forKey:NSForegroundColorAttributeName];

	// [!] : ellipsis like "long text..."
	NSMutableParagraphStyle *ps = [[NSMutableParagraphStyle alloc] init];
	[ps setLineBreakMode:NSLineBreakByTruncatingTail];
	[attr setObject:ps           forKey:NSParagraphStyleAttributeName];

	NSRect r = NSMakeRect( o+m+m, o+m, rect.size.width - ( o * 2 ) - ( o + m + m ), rect.size.height - ( o * 2 ) );

	[self.stringValue drawInRect:r withAttributes:attr];


	n_bmp_free( &frame );

}


-(NSDragOperation) draggingEntered:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingEntered" );

	// [!] : call when hovered

	if ( is_hovered == FALSE )
	{
//[self display];
		is_hovered = TRUE;
		queue      = TRUE;

		n_bmp_fade_go( &fade, n_bmp_white );
	}


	return NSDragOperationCopy;
}

-(void) draggingExited:( id <NSDraggingInfo> )sender
{
//NSLog( @"draggingExited" );

	if ( is_hovered != FALSE )
	{
//[self display];
		is_hovered = FALSE;
		queue      = TRUE;

		n_bmp_fade_go( &fade, n_bmp_black );
	}

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

	//[self setStringValue:@""];

	if ( is_hovered != FALSE )
	{
//[self display];
		is_hovered = FALSE;
		queue      = TRUE;

		n_bmp_fade_go( &fade, n_bmp_black );
	}


	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSString     *nsstr      = [[NSURL URLFromPasteboard:pasteboard] path];

	nsstr = n_textfield_path( nsstr, self.n_folder_only );

	[self setStringValue:nsstr];

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	return NSDragOperationCopy;
}

@end




#endif // _H_NONNON_MAC_NONNON_TEXTFIELD


