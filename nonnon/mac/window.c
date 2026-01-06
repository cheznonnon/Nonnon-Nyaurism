// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC_WINDOW
#define _H_NONNON_MAC_WINDOW




#include "./_mac.c"




void
n_mac_window_dialog_ok( n_posix_char *text )
{

	NSAlert *alert = [[NSAlert alloc] init];

	[alert addButtonWithTitle:@"OK"];

	alert.messageText = n_mac_str2nsstring( text );

	alert.icon = [NSImage imageNamed:NSImageNameInfo];

	[alert runModal];


	return;
}

BOOL
n_mac_window_dialog_yesno( n_posix_char *text )
{

	NSAlert *alert = [[NSAlert alloc] init];

	[alert addButtonWithTitle:@"Yes"];
	[alert addButtonWithTitle:@"No"];

	alert.messageText = n_mac_str2nsstring( text );

	alert.icon = [NSImage imageNamed:NSImageNameInfo];

	NSModalResponse ret = [alert runModal];
//NSLog( @"%d : %d : %d", NSAlertFirstButtonReturn, NSAlertSecondButtonReturn, ret );


	return ( ret == NSAlertFirstButtonReturn );
}

void
n_mac_window_centering( NSWindow *window )
{

	// [!] : call this at applicationWillFinishLaunching
	//
	//	it's not applicationDidFinishLaunching

	CGFloat sx = NSWidth ( [window frame] );
	CGFloat sy = NSHeight( [window frame] );

	CGFloat x = ( NSWidth ( [ [window screen] visibleFrame ] ) / 2 ) - ( sx / 2 );
	CGFloat y = ( NSHeight( [ [window screen] visibleFrame ] ) / 2 ) - ( sy / 2 );

	CGFloat dock_sy = [ [window screen] visibleFrame ].origin.y;

	[window setFrame:NSMakeRect( x,y+dock_sy,sx,sy ) display:YES ];


	return;
}

void
n_mac_window_image2window
(
	NSImageView  *imageview,
	NSImage      *image,
	NSWindow     *window,
	NSScrollView *scroll,
	CGFloat       min_size,
	CGFloat       max_ratio
)
{
//max_ratio = 1.0;

	CGFloat isx = image.size.width;
	CGFloat isy = image.size.height;

	CGFloat sx = MAX( min_size, isx );
	CGFloat sy = MAX( min_size, isy );

	CGFloat max_sx = NSWidth ( [ [window screen] frame ] ) * max_ratio;
	CGFloat max_sy = NSHeight( [ [window screen] frame ] ) * max_ratio;


	sx = MIN( sx, max_sx );
	sy = MIN( sy, max_sy );

	NSSize size = NSMakeSize( sx,sy );

	[window setContentMinSize:size];
	[window setContentMaxSize:size];
	[window setContentSize   :size];


	if ( isx > max_sx )
	{
		[scroll setHasHorizontalScroller:YES];
	} else {
		[scroll setHasHorizontalScroller:NO];
	}

	if ( isy > max_sy )
	{
		[scroll setHasVerticalScroller:YES];
		
		// [!] : default zero is at bottom

		NSPoint point = NSMakePoint( 0, isy );
		[scroll.contentView scrollToPoint:point];
	} else {
		[scroll setHasVerticalScroller:NO];
	}


	// [x] : not working
	//[imageview setImageScaling:NSImageScaleAxesIndependently];
	[imageview setImage:image];


	return;
}

NSPoint
n_mac_cursor_position_get( NSView *view )
{

	NSPoint globalLocation = [ NSEvent mouseLocation ];
	NSPoint windowLocation = [ [ view window ] convertPointFromScreen: globalLocation ];
	NSPoint   viewLocation = [ view convertPoint: windowLocation fromView: nil ];


	return viewLocation;
}

n_posix_bool
n_mac_window_is_hovered( NSView *view )
{

	n_posix_bool ret = n_posix_false;

	NSPoint pt = n_mac_cursor_position_get( view );
//NSLog( @"%f %f", pt.y, [ view bounds ].origin.y );

	if( NSPointInRect( pt, [ view bounds ] ) )
	{
		ret = true;
	}


	return ret;
}

n_posix_bool
n_mac_window_is_hovered_offset_by_rect( NSView *view, NSRect rect )
{

	n_posix_bool ret = n_posix_false;

	NSPoint pt = n_mac_cursor_position_get( view );

	if( NSPointInRect( pt, [ view bounds ] ) )
	{
//NSLog( @"1" );
		if ( NSPointInRect( pt, rect ) )
		{
//NSLog( @"2" );
			ret = true;
		}
	} else {
//NSLog( @"x" );
	}


	return ret;
}

void
n_mac_window_show( NSWindow *window )
{

	[window orderFront:nil];

	return;
}

void
n_mac_window_hide( NSWindow *window )
{

	[window orderOut:nil];

	return;
}

BOOL
n_mac_window_is_keywindow( NSWindow *window )
{
	return window.isKeyWindow;
}

BOOL
n_mac_window_is_visible( NSWindow *window )
{
	// [x] : macOS Sonoma : isVisible is deprecated : returns FALSE when child windows

	return ( ( window.isVisible )||( window.occlusionState & NSWindowOcclusionStateVisible ) );
}

void
n_mac_window_all_controls_onoff( NSView *view, BOOL onoff )
{

	// [!] : Thx : DeepSeek AI

	// [!] : use [NSWindow contentView] for NSView

	for( NSView *subview in view.subviews )
	{

		if ( [subview isKindOfClass:[NSControl class]] )
		{
			NSControl *control = (NSControl *)subview;

			if ( [control isKindOfClass:[NSTextField class]] )
			{
				if ( onoff )
				{
					[control setAlphaValue:1.0];
				} else {
					[control setAlphaValue:0.2];
				}
			} else {
				[control setEnabled:onoff];
			}
		}

		n_mac_window_all_controls_onoff( subview, onoff );
	}


	return;
}




// [x] : use Xib
//
//	you cannot make accurate controls without Interface Builder
//
//	[ Troubleshooter ]
//
//		Xcode : raidobutton-like circle is missing
//
//			check IBOutlet or (IBAction)
//			when not exist, DnD from Xib canvas and naming it
//			or else DnD from the circle into control on Xib
//
//			+ DnD uses menu button
//
//		App : not beautiful when resized
//
//			1 : see right pane
//			2 : set red arrow inner/outer (these arrows are clickable)
//			3 : arrange it

/*
NSButton*
n_mac_button
(
	id            self,
	NSWindow     *window,
	n_posix_char *label,
	CGFloat       x,
	CGFloat       y,
	CGFloat       sx,
	CGFloat       sy,
	SEL           selector
)
{

	// [!] : usage
	//
	//	@property (strong) NSButton *button;
	//	(strong) is important
	//
	//	selector : @selector( method_name: )
	//	colon is important


	NSButton *button;

	button = [[NSButton alloc] init];


	[button setTitle: n_mac_str2nsstring( label )];
	[button setButtonType:NSButtonTypeMomentaryLight];
	[button setBezelStyle:NSBezelStyleRounded];

	
	// [!] : without this, setFrame behaves badly
	// [x] : DnD in other controls will stop if NO
	//button.translatesAutoresizingMaskIntoConstraints = NO;
	
	[button setFrame:NSMakeRect( x, y, sx, sy )];
	
	// [!] : after above code
	[[window contentView] addSubview:button];


	[button setTarget:self];
	[button setAction:selector];


	return button;
}

NSImageView*
n_mac_imageview
(
	id        self,
	NSWindow *window,
	CGFloat   x,
	CGFloat   y,
	CGFloat   sx,
	CGFloat   sy,
	SEL       selector
)
{

	// [!] : usage
	//
	//	@property (strong) NSImageView *imageview;
	//	(strong) is important


	NSImageView *imageview;

	imageview = [[NSImageView alloc] initWithFrame:NSMakeRect( x, y, sx, sy )];
	[[window contentView] addSubview:imageview];

	[imageview setEditable:YES];

	// [!] : without this, setFrame behaves badly
	// [x] : DnD in other controls will stop if NO
	//imageview.translatesAutoresizingMaskIntoConstraints = NO;

	[imageview setTarget:self];
	[imageview setAction:selector];


	return imageview;
}
*/

#endif // _H_NONNON_MAC_WINDOW


