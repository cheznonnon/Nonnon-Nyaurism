// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSView (Custom View) to NonnonTxtbox
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonTxtbox"
//	3 : modify behavior


// [!] : trouble shooter
//
//	[ drawRect is not called at redraw ]
//
//		a : call [_n_txtbox NonnonTxtboxRedraw];
//		b : see your @property and connect(DnD) to UI on the Xib canvas
//		c : layer may cause




#ifndef _H_NONNON_MAC_NONNON_TXTBOX
#define _H_NONNON_MAC_NONNON_TXTBOX




#import <Cocoa/Cocoa.h>


#include "../neutral/bmp/ui/rectframe.c"
#include "../neutral/bmp/ui/roundframe.c"

#include "../neutral/txt.c"

#include "../bridge/helper.c"
#include "../bridge/gdi.c"


#include "_mac.c"
#include "draw.c"
#include "image.c"
#include "window.c"


#include "n_txtbox/00_codec.c"
#include "n_txtbox/01_character.c"
#include "n_txtbox/02_caret.c"
#include "n_txtbox/03_selection.c"
#include "n_txtbox/04_edit.c"
#include "n_txtbox/05_date.c"
#include "n_txtbox/06_undo.c"




#define N_TXTBOX_IME_ENABLE




#define N_MAC_TXTBOX_MODE_EDITBOX ( 0 )
#define N_MAC_TXTBOX_MODE_LISTBOX ( 1 )
#define N_MAC_TXTBOX_MODE_FINDBOX ( 2 )
#define N_MAC_TXTBOX_MODE_ONELINE ( 3 )


#define N_MAC_TXTBOX_CARET_SIZE   ( 2 )


#define N_MAC_TXTBOX_DRAW_LINENUMBER_NONE            ( 0 << 0 )
#define N_MAC_TXTBOX_DRAW_LINENUMBER_ONEBASED_INDEX  ( 1 << 0 )
#define N_MAC_TXTBOX_DRAW_LINENUMBER_ZEROBASED_INDEX ( 1 << 1 )


#define N_MAC_TXTBOX_DELEGATE_MOUSEDOWN_LEFT  ( 1 <<  0 )
#define N_MAC_TXTBOX_DELEGATE_MOUSEUP_LEFT    ( 1 <<  1 )
#define N_MAC_TXTBOX_DELEGATE_MOUSEDOWN_RIGHT ( 1 <<  2 )
#define N_MAC_TXTBOX_DELEGATE_MOUSEUP_RIGHT   ( 1 <<  3 )
#define N_MAC_TXTBOX_DELEGATE_LISTBOX_EDITED  ( 1 <<  4 )
#define N_MAC_TXTBOX_DELEGATE_LISTBOX_MOVED   ( 1 <<  5 )
#define N_MAC_TXTBOX_DELEGATE_EDITED          ( 1 <<  6 )
#define N_MAC_TXTBOX_DELEGATE_FIND            ( 1 <<  7 )
#define N_MAC_TXTBOX_DELEGATE_SWAP            ( 1 <<  8 )
#define N_MAC_TXTBOX_DELEGATE_F3              ( 1 <<  9 )
#define N_MAC_TXTBOX_DELEGATE_DELETE          ( 1 << 10 )
#define N_MAC_TXTBOX_DELEGATE_SHIFT           ( 1 << 11 )
#define N_MAC_TXTBOX_DELEGATE_UNDO            ( 1 << 12 )
#define N_MAC_TXTBOX_DELEGATE_ALL             0xffff




@protocol NonnonTxtbox_delegate

- (void) NonnonTxtbox_delegate_dropped;

@optional

- (void) NonnonTxtbox_delegate_listbox_edited:(n_type_int)index;
- (void) NonnonTxtbox_delegate_listbox_rename:(n_type_int)index;

- (void) NonnonTxtbox_delegate_edited:(id)txtbox onoff:(BOOL)onoff;
- (void) NonnonTxtbox_delegate_find  :(id)txtbox;
- (void) NonnonTxtbox_delegate_swap  :(id)txtbox is_up:(BOOL)is_up;
- (void) NonnonTxtbox_delegate_F3    :(id)txtbox is_left:(BOOL)is_left;
- (void) NonnonTxtbox_delegate_delete:(id)txtbox;
- (void) NonnonTxtbox_delegate_shift :(NSEvent*) event;
- (void) NonnonTxtbox_delegate_undo  :(id)txtbox;

@end




#ifdef N_TXTBOX_IME_ENABLE
@interface NonnonTxtbox : NSView <NSTextInputClient>
#else
@interface NonnonTxtbox : NSView
#endif

@property (nonatomic,assign) id delegate;
@property int                   delegate_option;

@end


@interface NonnonTxtbox ()


@property n_type_int  n_focus;
@property n_txt      *n_txt_data;
@property n_txt      *n_txt_deco;
@property BOOL        n_direct_click_onoff;
@property NSString   *n_path;
@property int         n_option_linenumber;

@property BOOL        n_edited;
@property BOOL        n_is_enter_pressed;

@property int         n_mode;

@property BOOL        n_listbox_no_selection_onoff;
@property BOOL        n_listbox_edit_onoff;
@property BOOL        n_listbox_no_edit;

@property n_bmp      *n_findbox_bmp_icon;

@property CGFloat     n_corner_size;

@property BOOL        n_border_separator_only;

@property BOOL        n_path_ellipsis_onoff;
@property n_type_int  n_path_ellipsis_offset;

@property BOOL       *n_search_marker;
@property n_type_int  n_search_marker_count;

@end




static NonnonTxtbox *n_txtbox_first_responder = nil;




@implementation NonnonTxtbox {

	n_bmp       canvas;
	NSFont     *font;
	CGSize      font_size;
	CGFloat     scroll;
	CGFloat     scroll_step;
	CGFloat     scroll_page;
	CGPoint     pt;
	CGPoint     pt_grag_n_drag;
	NSRect      scroller_rect_thumb;
	NSRect      scroller_rect_shaft;
	CGFloat     offset, offset_x, offset_y;
	CGFloat     margin;
	CGFloat     padding;
	BOOL        thumb_is_captured;
	BOOL        thumb_is_hovered;
	BOOL        shaft_is_hovered;
	CGFloat     thumb_offset;
	n_type_int  redraw_fy;
	n_type_int  redraw_ty;
	BOOL        is_grayed;
	BOOL        is_darkmode;
	BOOL        is_key_input;
	BOOL        grab_n_drag_onoff;

	BOOL        drag_timer_queue;

	n_bmp_fade  scrollbar_fade;
	BOOL        scrollbar_fade_hovered_onoff;
	BOOL        scrollbar_fade_captured_onoff;

	NSFont     *linenumber_font;
	CGSize      linenumber_size;
	u32         linenumber_color_back;
	u32         linenumber_color_text;

	CGPoint     caret_pt;
	n_caret     caret_fr;
	n_caret     caret_to;
	BOOL        caret_blink_onoff;
	BOOL        caret_blink_force_onoff;
	n_bmp_fade  caret_blink_fade;
	u32         caret_blink_interval;
	CGFloat     caret_centered_offset;

	BOOL        shift_selection_is_tail;

	n_undo      undo;

	NSRect      find_icon_rect;
	BOOL        find_icon_is_pressed;
	BOOL        find_icon_focus_prev;
	BOOL        find_icon_press_prev;
	NSImage    *find_icon_cache_nsimage;
	n_bmp_fade  find_icon_fade;

	NSRect      delete_circle_rect;
	BOOL        delete_circle_is_pressed;
	BOOL        delete_circle_is_hovered;
	n_bmp_fade  delete_circle_fade_hovered;
	int         delete_circle_fade_pressed_phase;
	n_bmp_fade  delete_circle_fade_pressed;

	n_type_int  listbox_edit_index;

	BOOL        smooth_wheel_is_mos;
	BOOL        smooth_wheel_onoff;
	CGFloat     smooth_wheel_delta;
	BOOL        smooth_wheel_mutex;

	BOOL        smooth_wheel_animation_onoff;
	u32         smooth_wheel_animation_msec;
	int         smooth_wheel_animation_count;
	u32         smooth_wheel_animation_timer;

	NSColor    *nscolor_white;
	NSColor    *nscolor_black;
	NSColor    *nscolor_text;
	NSColor    *nscolor_back;
	NSColor    *nscolor_text_highlight;
	NSColor    *nscolor_crlf;
	NSColor    *nscolor_accent;
	NSColor    *nscolor_nofocus;
	NSColor    *nscolor_ime;
	NSColor    *nscolor_stripe;
	NSColor    *nscolor_frame;
	NSColor    *nscolor_text_normal;

	NSMutableDictionary *attr;
	NSMutableDictionary *attr_crlf;
	NSMutableDictionary *attr_ime;

#ifdef N_TXTBOX_IME_ENABLE

	NSTextInputContext *ime;
	n_caret             ime_caret_fr;
	n_caret             ime_caret_to;
	NSString           *ime_nsstr;
	BOOL                ime_onoff;
	BOOL                ime_freed;
	NSRange             ime_focus;
	BOOL                ime_delay;
	BOOL                ime_is_first;
	n_type_int          ime_caret_offset;
	NSRect              ime_caret_rect;

	NSRect              underline_rect;
	CGFloat             underline_fx;
	CGFloat             underline_tx;

#endif

}




@synthesize delegate_option;
@synthesize delegate;

@synthesize n_focus;
@synthesize n_txt_data;
@synthesize n_txt_deco;
@synthesize n_direct_click_onoff;
@synthesize n_path;
@synthesize n_option_linenumber;

@synthesize n_edited;
@synthesize n_is_enter_pressed;

@synthesize n_mode;

@synthesize n_listbox_no_selection_onoff;
@synthesize n_listbox_edit_onoff;
@synthesize n_listbox_no_edit;

@synthesize n_findbox_bmp_icon;

@synthesize n_corner_size;

@synthesize n_border_separator_only;

@synthesize n_path_ellipsis_onoff;
@synthesize n_path_ellipsis_offset;

@synthesize n_search_marker;
@synthesize n_search_marker_count;




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{

		n_bmp_safemode = FALSE;

		n_bmp_transparent_onoff_default = FALSE;


		{
			NSArray      *paths   = NSSearchPathForDirectoriesInDomains( NSDesktopDirectory, NSUserDomainMask, YES );
			NSString     *desktop = [paths objectAtIndex:0];
			n_posix_char  tmpname[ 100 ]; n_string_path_tmpname( tmpname );

	    		n_path = [NSString stringWithFormat:@"%@/%s.txt", desktop, tmpname];
//NSLog( @"%@", path );
		}


		[self registerForDraggedTypes:[NSArray arrayWithObject:NSPasteboardTypeFileURL]];

/*
		// [x] : gray-out at lost focus : un-implementable : system should do

		[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector( applicationWillBecomeActive: )
			       name:NSApplicationDidBecomeActiveNotification
			     object:nil
		];

		[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector( applicationWillResignActive: )
			       name:NSApplicationDidResignActiveNotification
			     object:nil
		];
*/
/*
		NSTrackingArea* trackingArea = [[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways
			       owner:self
			    userInfo:nil
		];
		[self addTrackingArea:trackingArea];
*/

		// [!] : accent color
		[[NSDistributedNotificationCenter defaultCenter]
			addObserver: self
			   selector: @selector( accentColorChanged: )
			       name: @"AppleColorPreferencesChangedNotification"
			     object: nil
		];

		// [!] : dark mode
		[[NSDistributedNotificationCenter defaultCenter]
			addObserver: self
			   selector: @selector( darkModeChanged: )
			       name: @"AppleInterfaceThemeChangedNotification"
			     object: nil
		];

		n_findbox_bmp_icon = NULL;


		// [x] : Catalina : monospacedSystemFontOfSize : CJK unified idiogram
		//
		//	non-japanese glyphs will be used
		//
		//	Monterey : it seems to be fixed

		//font      = [NSFont monospacedSystemFontOfSize:15 weight:NSFontWeightRegular];
		font      = [NSFont userFixedPitchFontOfSize:15];
		//font      = [NSFont systemFontOfSize:15 weight:NSFontWeightRegular];

		//n_mac_txtbox_character_monospace_onoff = TRUE;
		//font      = [NSFont fontWithName:@"Hiragino Maru Gothic Pro" size:14];

		font_size = n_mac_image_text_pixelsize( @" ", font );

		linenumber_font = [NSFont userFixedPitchFontOfSize:14];
		linenumber_size = n_mac_image_text_pixelsize( @"000000", font );

		[self NonnonTxtboxFontChangeAttr];


		offset = 6;
		margin = 3;

		offset_x = offset_y = offset;

		[self NonnonTxtboxReset];

		is_darkmode = n_mac_is_darkmode();
		[self NonnonTxtBoxColorScheme];

		n_bmp_fade_init( &scrollbar_fade, n_bmp_black );

		n_undo_zero( &undo ); n_txt_zero( &undo.txt ); n_txt_utf8_new( &undo.txt );

		n_edited = FALSE;

		smooth_wheel_is_mos = FALSE;
		smooth_wheel_onoff  = FALSE;

		n_mac_timer_init( self, @selector( _NonnonTxtboxScrollbarTimer    ),  33 );
		n_mac_timer_init( self, @selector( _NonnonTxtboxCaretBlinkTimer   ),  66 );
		n_mac_timer_init( self, @selector( _NonnonTxtboxDeleteCircleTimer ),  33 );
		n_mac_timer_init( self, @selector( _NonnonTxtboxDragTimer         ),  33 );
		n_mac_timer_init( self, @selector( _NonnonTxtboxSmoothWheelTimer  ),   1 );
//n_mac_timer_init( self, @selector( _NonnonTxtboxTestTimer         ),  33 );

	}
	
	return self;
}




- (void) accentColorChanged:(NSNotification *)notification
{
//NSLog( @"accentColorChanged" );

	// [x] : Sonoma : buggy : old value is set

	n_mac_timer_init_once( self, @selector( n_timer_method_color ), 200 );

}

- (void) darkModeChanged:(NSNotification *)notification
{
//NSLog( @"darkModeChanged" );

	// [x] : Sonoma : buggy : old value is set

	n_mac_timer_init_once( self, @selector( n_timer_method_color ), 200 );

}

- (void) n_timer_method_color
{
//NSLog( @"n_timer_method_color" );

	is_darkmode = n_mac_is_darkmode();

	[self NonnonTxtBoxColorScheme];

	[self NonnonTxtboxRedraw];

}

- (void) NonnonTxtBoxColorScheme
{

	if ( is_darkmode )
	{
		nscolor_text = n_mac_nscolor_argb( 255,222,222,222 );
	} else {
		nscolor_text = [NSColor textColor];
	}

	nscolor_back = [NSColor textBackgroundColor];
	if ( is_grayed )
	{
		nscolor_text = n_mac_nscolor_blend( nscolor_back, nscolor_text, 0.55 );
		nscolor_back = n_mac_nscolor_blend( nscolor_back, nscolor_text, 0.05 );
	}

	nscolor_text_normal = nscolor_text;

	if ( is_darkmode )
	{
		nscolor_text_highlight = nscolor_text;
	} else {
		nscolor_text_highlight = nscolor_back;
	}

	if ( is_darkmode )
	{
		nscolor_nofocus = n_mac_nscolor_argb( 255,100,100,100 );
	} else {
		nscolor_nofocus = n_mac_nscolor_argb( 255,200,200,200 );
	}

	nscolor_accent = [NSColor controlAccentColor];


#ifdef N_TXTBOX_IME_ENABLE

	nscolor_ime = nscolor_accent;

#endif

}




- (void) NonnonTxtboxRedraw
{

	// [x] : setNeedsDisplay is heavier by approx. 1%

	[self display];
	//[self setNeedsDisplay:YES];

}

- (void) NonnonTxtboxRedrawPartial
{
//return;

	// [!] : efficient mode : unimplementable : macOS is too buggy

	[self display];

/*
	redraw_fy = n_focus;
	redraw_ty = redraw_fy + 1;

	n_type_int i = scroll;

	NSRect _rect_main = NSMakeRect( padding - margin, offset_y - caret_centered_offset, self.frame.size.width - ( offset_x * 2 ), font_size.height );

	_rect_main.origin.y -= font_size.height * i;
	_rect_main.origin.y += font_size.height * redraw_fy;

	[self displayRect:_rect_main];
*/
}




- (void) NonnonTxtboxSearchMarkerReset:(BOOL)redraw
{
//NSLog( @"%x", (int) n_search_marker );

	if ( n_search_marker != NULL )
	{
		n_memory_free( n_search_marker );

		n_search_marker       = NULL;
		n_search_marker_count = 0;

		if ( redraw ) { [self NonnonTxtboxRedraw]; }
	}

}




- (void) NonnonTxtboxCaretReset
{

	n_focus = -1;

	caret_fr = NonnonMakeCaret( 0,0,0,0 );
	caret_to = NonnonMakeCaret( 0,0,0,0 );

#ifdef N_TXTBOX_IME_ENABLE

	// [!] : Sonoma : popup indicator is implemented : this is used in the first time

	ime_caret_fr = caret_fr;
	ime_caret_to = caret_to;

#endif

}

- (void) NonnonTxtboxCaretCalculate
{

	// [!] : caret on the current screen

	n_caret pt = caret_fr;

	if ( caret_fr.cch.y == caret_to.cch.y )
	{
		if ( caret_fr.cch.y == n_focus )
		{
			if ( caret_fr.pxl.x == caret_to.pxl.x )
			{
//NSLog( @"==" );
				//
			} else
			if ( caret_fr.pxl.x < caret_to.pxl.x )
			{
//NSLog( @"< : %0.2f %0.2f", caret_fr.pxl.x, caret_to.x );
				pt = caret_to;
			} else {
//NSLog( @"> : %0.2f %0.2f", caret_fr.pxl.x, caret_to.x );
				pt = caret_to;
			}
		}
	} else
	if ( caret_fr.cch.y < caret_to.cch.y )
	{
		if ( caret_to.cch.y == n_focus ) { pt = caret_to; }
	} else
	if ( caret_fr.cch.y > caret_to.cch.y )
	{
		if ( caret_to.cch.y == n_focus ) { pt = caret_to; }
	}

	caret_pt.x = padding + pt.pxl.x - 1;
	caret_pt.y = offset_y + ( ( pt.cch.y - trunc( scroll ) ) * font_size.height );


#ifdef N_TXTBOX_IME_ENABLE

	// [!] : Sonoma : popup indicator is implemented : this is used in the first time

	ime_caret_fr = caret_fr;
	ime_caret_to = caret_to;

#endif


	return;
}

- (BOOL) NonnonTxtboxCaretIsOnScreen
{
#ifdef N_TXTBOX_IME_ENABLE

	if ( ime_onoff )
	{
		return FALSE;
	}

#endif


	[self NonnonTxtboxCaretCalculate];


	if ( n_focus < ( scroll - 1 ) ) { return FALSE; }


	BOOL ret = FALSE;

	if ( caret_fr.cch.y == caret_to.cch.y )
	{
		if ( caret_fr.cch.y == n_focus ) { ret = TRUE; }
	} else
	if ( caret_fr.cch.y < caret_to.cch.y )
	{
		if ( caret_to.cch.y == n_focus ) { ret = TRUE; }
	} else
	if ( caret_fr.cch.y > caret_to.cch.y )
	{
		if ( caret_to.cch.y == n_focus ) { ret = TRUE; }
	}


//NSLog( @"%lld : %d", n_focus, ret );


	return ret;
}




- (void) NonnonTxtboxFontChangeAttr
{

	attr = [NSMutableDictionary dictionary];
	[attr setObject:font forKey:NSFontAttributeName];

#ifdef N_TXTBOX_IME_ENABLE

	attr_ime = [NSMutableDictionary dictionary];
	[attr_ime setObject:font forKey:NSFontAttributeName];

#endif
}

- (void) NonnonTxtboxFontChange:(NSFont*)font_new
{

	font = font_new;
	[self NonnonTxtboxFontChangeAttr];

}




- (void) NonnonTxtboxReset
{

	 if ( n_border_separator_only ) { offset_x = 4; }


	if ( self.n_option_linenumber == N_MAC_TXTBOX_DRAW_LINENUMBER_NONE )
	{
		padding = offset_x;
	} else {
		padding = offset_x + linenumber_size.width + margin;
	}

	if ( self.n_findbox_bmp_icon != NULL )
	{
		padding += self.frame.size.height;
	}


	scroll = 0;

	if ( n_listbox_no_selection_onoff )
	{
		n_focus = -1;
	} else {
		n_focus = 0;
	}

	scroller_rect_thumb = NSMakeRect( 0,0,0,0 );
	scroller_rect_shaft = NSMakeRect( 0,0,0,0 );


	n_bmp_fade_init( &scrollbar_fade, n_bmp_black );

	thumb_is_hovered  = scrollbar_fade_hovered_onoff  = FALSE;
	thumb_is_captured = scrollbar_fade_captured_onoff = FALSE;


	caret_pt = NSMakePoint( 0, 0 );

	caret_fr = NonnonMakeCaret( 0,0,0,0 );
	caret_to = NonnonMakeCaret( 0,0,0,0 );

	caret_blink_onoff       = TRUE;
	caret_blink_force_onoff = FALSE;

	n_bmp_fade_init( &caret_blink_fade, n_bmp_black );

	shift_selection_is_tail = TRUE;

#ifdef N_TXTBOX_IME_ENABLE

	ime_caret_fr = caret_fr;
	ime_caret_to = caret_to;

	ime_freed = FALSE;

#endif

	n_edited = FALSE;

	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
	{
		[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
	}


	n_bmp_fade_init( &delete_circle_fade_hovered, n_bmp_black );
	n_bmp_fade_init( &delete_circle_fade_pressed, n_bmp_black );


	redraw_fy = -1;
	redraw_ty = -1;

}

- (void) NonnonTxtboxFocus2Caret
{

	n_posix_char *line = n_txt_get( n_txt_data, n_focus );
	n_type_int    cch  = n_posix_strlen( line );

	caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		cch
	);

}




- (CGFloat) NonnonTxtboxFocusCalculate:(NSPoint) local_point
{
	return trunc( scroll ) + trunc( ( local_point.y - offset_y ) / font_size.height );
}

- (n_caret) NonnonTxtboxMouseCursorDetect
{

	NSPoint local_point = n_mac_cursor_position_get( self );

	n_focus = [self NonnonTxtboxFocusCalculate:local_point];
	if ( n_focus < 0 ) { n_focus = 0; }
	if ( n_focus >= n_txt_data->sy ) { n_focus = n_txt_data->sy - 1; }
//NSLog( @"%f", n_focus );

	NSRect r = NSMakeRect( padding, offset_y, [self frame].size.width - ( offset_x * 2 ), font_size.height );

	n_caret caret = n_txtbox_caret_detect_pixel2caret
	(
		n_txt_data,
		n_focus,
		r,
		font,
		font_size,
		local_point
	);

//NSLog( @"%lld %lld", caret.cch.x, caret.cch.y );


	return caret;
}




- (void) _NonnonTxtboxScrollbarTimer
{

	if ( smooth_wheel_mutex ) { return; }


	if ( scrollbar_fade_captured_onoff )
	{
		n_bmp_fade_engine( &scrollbar_fade, TRUE );
//NSLog( @"%d", scrollbar_fade.percent );
		if ( scrollbar_fade.stop == FALSE )
		{
			[self NonnonTxtboxRedraw];
		} else {
			scrollbar_fade_captured_onoff = FALSE;
		}

		return;
	}


//return;

	static BOOL prv = FALSE;

	BOOL cur = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_thumb );
//NSLog( @"fade %d %d %d", prv, cur, thumb_is_hovered );

	if ( ( prv == FALSE )&&( cur )&&( thumb_is_hovered == FALSE ) )
	{
//NSLog( @"fade in" );
		thumb_is_hovered = TRUE;

		scrollbar_fade_hovered_onoff = TRUE;
		n_bmp_fade_always_on( &scrollbar_fade, n_bmp_black, n_bmp_white );
	} else
	if ( ( cur == FALSE )&&( thumb_is_hovered ) )
	{
//NSLog( @"fade out" );
		thumb_is_hovered = FALSE;

		scrollbar_fade_hovered_onoff = TRUE;
		n_bmp_fade_always_on( &scrollbar_fade, n_bmp_black, n_bmp_white );
	} 

	n_bmp_fade_engine( &scrollbar_fade, TRUE );
//NSLog( @"%d", scrollbar_fade.percent );

	if ( scrollbar_fade.stop == FALSE )
	{
		[self NonnonTxtboxRedraw];
	} else {
		scrollbar_fade_hovered_onoff = FALSE;
	}

	prv = thumb_is_hovered;

}




- (void) NonnonTxtboxUndo:(int) action
{

	if ( action == N_TXTBOX_UNDO_RESTORE )
	{
		n_txt_copy( &undo.txt, n_txt_data );

		scroll   = undo.scroll;
		n_focus  = undo.focus;
		caret_fr = undo.caret_fr;
		caret_to = undo.caret_to;
	} else
	if ( action == N_TXTBOX_UNDO_REGISTER )
	{
		n_txt_copy( n_txt_data, &undo.txt );

		undo.scroll   = scroll;
		undo.focus    = n_focus;
		undo.caret_fr = caret_fr;
		undo.caret_to = caret_to;
	}

}

- (void) _NonnonTxtboxCaretBlinkTimer
{
//return;

	if ( FALSE == n_mac_window_is_keywindow( self.window ) ) { return; }


	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		if ( self.n_listbox_edit_onoff == FALSE ) { return; }
	}


	if ( n_game_timer( &caret_blink_interval, 500 ) )
	{
		n_bmp_fade_init( &caret_blink_fade, n_bmp_black );
		if ( caret_blink_onoff )
		{
			n_bmp_fade_go( &caret_blink_fade, n_bmp_white );
			caret_blink_onoff = caret_blink_force_onoff = FALSE;
		} else {
			n_bmp_fade_go( &caret_blink_fade, n_bmp_white );
			caret_blink_onoff =  TRUE;
		}
	}

	n_bmp_fade_engine( &caret_blink_fade, TRUE );


//return;

//NSLog( @"!" );
	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
//NSLog( @"N_MAC_TXTBOX_MODE_FINDBOX" );
		[self NonnonTxtboxRedraw];
	} else
	if ( grab_n_drag_onoff )
	{
//NSLog( @"grab_n_drag_onoff" );
		[self NonnonTxtboxRedraw];
	} else
	if ( drag_timer_queue )
	{
//NSLog( @"drag_timer_queue" );
		[self NonnonTxtboxRedraw];
	} else
	if ( is_key_input )
	{
//NSLog( @"is_key_input" );
		[self NonnonTxtboxRedraw];
	} else
	if ( [self NonnonTxtboxCaretIsOnScreen] )
	{
//NSLog( @"caret" );
		[self NonnonTxtboxRedrawPartial];
	} else
	if ( ime_onoff )
	{
//NSLog( @"IME" );
		[self NonnonTxtboxRedrawPartial];
	} else
	//
	{
//NSLog( @"N/A" );
	}

}

- (void) _NonnonTxtboxDeleteCircleTimer
{

	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		BOOL redraw = FALSE;

		n_bmp_fade_engine( &find_icon_fade, TRUE );
		if ( find_icon_fade.stop == FALSE ) { redraw = TRUE; }

		n_bmp_fade_engine( &delete_circle_fade_hovered, TRUE );
		if ( delete_circle_fade_hovered.stop == FALSE ) { redraw = TRUE; }

		if ( delete_circle_fade_pressed_phase == 1 )
		{
			delete_circle_fade_pressed_phase++;
			n_bmp_fade_always_on( &delete_circle_fade_pressed, n_bmp_black, n_bmp_white );
		}

		if ( delete_circle_fade_pressed_phase == 2 )
		{
			n_bmp_fade_engine( &delete_circle_fade_pressed, TRUE );
			if ( delete_circle_fade_pressed.stop )
			{
				delete_circle_fade_pressed_phase = 0;

				[self NonnonTxtboxSelectAll:FALSE];
				n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to );

				[ime discardMarkedText];
				ime_nsstr = @"";
				ime_onoff = FALSE;

				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_DELETE )
				{
					[self.delegate NonnonTxtbox_delegate_delete:self];
				}

				caret_blink_interval     = 0;
				caret_blink_onoff        = caret_blink_force_onoff = FALSE;
				caret_blink_fade.percent = 100;
			}

			redraw = TRUE;
		}

		if ( redraw ) { [self NonnonTxtboxRedraw]; }
	}

}

- (void) _NonnonTxtboxDragTimer
{

	if ( drag_timer_queue )
	{
		NSPoint pt = n_mac_cursor_position_get( self );
//NSLog( @"%f %f", pt.y, [self frame].size.height );

		if ( pt.y <= 0 )
		{
			CGFloat boost = 1 + ( fabs( pt.y ) / 100 );
			scroll -= boost;
		} else
		if ( pt.y >= [self frame].size.height )
		{
			CGFloat boost = 1 + ( fabs( pt.y - [self frame].size.height ) / 100 );
			scroll += boost;
		}

		caret_to = [self NonnonTxtboxMouseCursorDetect];

		[self NonnonTxtboxRedraw];
	}

}

- (void) _NonnonTxtboxSmoothWheelTimer
{

	if ( smooth_wheel_is_mos ) { return; }

	if ( smooth_wheel_onoff )
	{
		CGFloat delta = fabs( smooth_wheel_delta );
		if ( delta == 0 ) { return; }
//NSLog( @"%0.2f", delta );

		// [!] : small amount support : Mos hasn't this feature

		static BOOL slow = FALSE;

		if ( smooth_wheel_animation_onoff == FALSE )
		{
			smooth_wheel_animation_onoff = TRUE;
			smooth_wheel_animation_timer = n_posix_tickcount();
			smooth_wheel_animation_count = 0;
			smooth_wheel_animation_msec  = 0;

//NSLog( @"%0.2f", delta );
			if ( delta <= 3 )
			{
				slow = TRUE;
			}
		}

		if ( smooth_wheel_animation_onoff )
		{
			smooth_wheel_animation_msec = n_posix_min( pow( delta, 3 ), 300 );
		}
//NSLog( @"slow %d : %d msec : delta %0.2f", slow, smooth_wheel_animation_msec, delta );

		if ( smooth_wheel_animation_onoff )
		{
			if ( smooth_wheel_delta < 0 )
			{
				scroll += 1;
			} else {
				scroll -= 1;
			}

			[self NonnonTxtboxRedraw];

			BOOL go = FALSE;
			if ( slow )
			{
//NSLog( @"slow" );
				smooth_wheel_animation_count++;
				if ( smooth_wheel_animation_count >= 3 )
				{
					go = TRUE;
				}
			} else
			if ( n_game_timer( &smooth_wheel_animation_timer, smooth_wheel_animation_msec ) )
			{
//NSLog( @"fast" );
				go = TRUE;
			}

			if ( go )
			{
				smooth_wheel_animation_onoff = FALSE;

				smooth_wheel_onoff = FALSE;
				smooth_wheel_mutex = FALSE;

				slow = FALSE;
			}
		}

	}

}
/*
- (void) _NonnonTxtboxTestTimer
{

	NSRect rect = NSMakeRect( 0,0,200,200 );
	[self displayRect:rect];

}
*/



- (void) NonnonTxtboxKeyboardInputMethod:(NSString*)nsstring
{
//return;

	if ( n_txt_data->readonly ) { return; }


#ifdef N_TXTBOX_IME_ENABLE

	BOOL is_tab = FALSE;
	{
		n_posix_char *s = n_mac_nsstring2str( nsstring );
		if ( s[ 0 ] == N_STRING_CHAR_TAB )
		{
//NSLog( @"tab" );
			is_tab = TRUE;
		}

		n_string_free( s );
	}

	if ( is_tab == FALSE )
	{
		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];

		return;
	}

#endif


	n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to );
//return;


	n_posix_char *line_f = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );
	n_posix_char *line_m = n_mac_nsstring2str( nsstring );
	n_posix_char *line_t = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );

	line_f[ caret_fr.cch.x ] = N_STRING_CHAR_NUL;
	n_posix_sprintf_literal( line_t, "%s", &line_t[ caret_fr.cch.x ] );

	n_type_int    cch = n_posix_strlen( line_f ) + n_posix_strlen( line_m ) + n_posix_strlen( line_t );
	n_posix_char *s   = n_string_new( cch );
	n_posix_sprintf_literal( s, "%s%s%s", line_f, line_m, line_t );
//NSLog( @"%s : %s : %s", line_f, line_m, line_t );

	//NSString *tmp = [[NSString alloc] initWithUTF8String:s];
//NSLog( @"%@", tmp );

	n_txt_mod( n_txt_data, n_focus, s );


	caret_fr.cch.x = caret_to.cch.x = n_posix_strlen( line_f ) + n_posix_strlen( line_m );

	caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		caret_fr.cch.x
	);


	n_string_free( s );


	n_string_free( line_f );
	n_string_free( line_m );
	n_string_free( line_t );


	[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];


	n_edited = TRUE;
	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
	{
		[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
	}


	[self NonnonTxtboxCaretOutOfCanvasUpDown];
	[self NonnonTxtboxRedraw];

}




- (void) NonnonTxtboxCaretTailSet
{

	caret_fr = caret_to = n_mac_txtbox_caret_tail
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		&shift_selection_is_tail
	);

}

- (void) NonnonTxtboxTripleclickDetect
{
//NSLog( @"%f", n_focus );

	n_posix_char *line = n_txt_get( n_txt_data, n_focus );
	n_type_int    cch  = n_posix_strlen( line );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		cch
	);


	caret_fr = NonnonMakeCaret
	(
		0, n_focus * font_size.height,
		0, n_focus
	);

	caret_to = NonnonMakeCaret
	(
		caret.pxl.x, n_focus * font_size.height,
		cch, n_focus
	);

	shift_selection_is_tail = TRUE;


	return;
}

- (void) NonnonTxtboxDoubleclickDetect:(BOOL)smart_selection_onoff tab_patch:(BOOL)tab_patch
{
//return;

	// [x] : Xcode : bug : japanese characters are not output

	n_posix_char *line = n_txt_get( n_txt_data, n_focus );
//NSLog( @"%lld : %s", n_focus, line ); return;
//NSLog( @"%c", line[ caret_fr.cch.x ] );

	if (
		( smart_selection_onoff )
		&&
		( n_mac_txtbox_selection_is_stop_char_single_utf8_search( &line[ caret_fr.cch.x ], "　" ) )
	)
	{
//NSLog( @"zenkaku sapce" );

		n_type_int cch_space = n_posix_strlen( "　" );

		{

			n_type_int cch_fr = caret_fr.cch.x;
			n_posix_loop
			{
				if ( cch_fr <= 0 ) { break; }

				if ( FALSE == n_mac_txtbox_selection_is_stop_char_single_utf8_search( &line[ cch_fr ], "　" ) )
				{
					cch_fr += cch_space;
					break;
				}

				cch_fr -= cch_space;
			}


			caret_fr = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_fr
			);

		}

		{

			n_type_int cch_to = caret_to.cch.x;
			n_posix_loop
			{
				if ( line[ cch_to ] == '\0' ) { break; }

				if ( FALSE == n_mac_txtbox_selection_is_stop_char_single_utf8_search( &line[ cch_to ], "　" ) )
				{
					break;
				}

				cch_to += cch_space;
			}


			caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_to
			);

		}

	} else
	if (
		( line[ caret_fr.cch.x ] ==  ' ' )
		||
		( line[ caret_fr.cch.x ] == '\t' )
		||
		(
			( tab_patch )
			&&
			( ( caret_fr.cch.x > 0 )&&( line[ caret_fr.cch.x - 1 ] == '\t' ) )
		)
	)
	{
//NSLog( @"blank" );

		if (
			( tab_patch )
			&&
			( ( caret_fr.cch.x > 0 )&&( line[ caret_fr.cch.x - 1 ] == '\t' ) )
		)
		{
			caret_fr.cch.x--;
		}

		{

			n_type_int cch_fr = caret_fr.cch.x;
			n_posix_loop
			{
				if ( cch_fr <= 0 ) { break; }

//NSLog( @"%c", line[ cch_fr ] );

				if (
					( line[ cch_fr ] !=  ' ' )
					&&
					( line[ cch_fr ] != '\t' )
				)
				{
					cch_fr++;
					break;
				}

				cch_fr--;
			}


			caret_fr = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_fr
			);

		}

		{

			n_type_int cch_to = caret_to.cch.x;
			n_posix_loop
			{
				if ( line[ cch_to ] == '\0' ) { break; }

				if (
					( line[ cch_to ] !=  ' ' )
					&&
					( line[ cch_to ] != '\t' )
				)
				{
					break;
				}

				cch_to++;
			}


			caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_to
			);

		}

	} else {
//NSLog( @"non-blank : %s : %c", line, line[ caret_fr.cch.x ] );

		n_posix_char *starting_char     = &line[ caret_fr.cch.x ];
		n_type_int    starting_char_cch = n_mac_txtbox_utf8_cb( starting_char );
//NSLog( @"%lld", starting_char_cch );

		{

			n_type_int cch_fr = caret_fr.cch.x;
//NSLog( @"start %c", line[ cch_fr ] );

			n_posix_loop
			{
				if ( cch_fr < 0 ) { cch_fr = 0; break; }

				n_type_int step = n_mac_txtbox_utf8_cb( &line[ cch_fr ] );

				if (
					( line[ cch_fr ] ==  ' ' )
					||
					( line[ cch_fr ] == '\t' )
					||
					(
						( smart_selection_onoff )
						&&
						(
							( starting_char_cch != step )
							||
							( n_mac_txtbox_selection_is_stop_char_utf8( starting_char, &line[ cch_fr ] ) )
							||
							( n_mac_txtbox_digit_detect( starting_char, line, cch_fr ) )
						)
					)
				)
				{
					cch_fr += step;
					break;
				}

				BOOL break_needed = FALSE;

				u8 *str = (u8*) line;
				n_posix_loop
				{
					if ( cch_fr <= 0 ) { cch_fr = 0; break_needed = TRUE; break; }

					cch_fr--;

					if ( str[ cch_fr ] >= 0xf0 )
					{
						break;
					} else
					if ( str[ cch_fr ] >= 0xe0 )
					{
						break;
					} else
					if ( str[ cch_fr ] >= 0xc0 )
					{
						break;
					} else
					if ( str[ cch_fr ] <= 127 )
					{
						break;
					}

				}

				if ( break_needed ) { break; }
			}

			caret_fr = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_fr
			);

//NSLog( @"%c", line[ cch_fr ] );

		}

		{

			n_type_int cch_to = caret_to.cch.x;
			n_posix_loop
			{
				if ( line[ cch_to ] == '\0' ) { break; }

				n_type_int step = n_mac_txtbox_utf8_cb( &line[ cch_to ] );

				if (
					( line[ cch_to ] ==  ' ' )
					||
					( line[ cch_to ] == '\t' )
					||
					(
						( smart_selection_onoff )
						&&
						(
							( starting_char_cch != step )
							||
							( n_mac_txtbox_selection_is_stop_char_utf8( starting_char, &line[ cch_to ] ) )
							||
							( n_mac_txtbox_digit_detect( starting_char, line, cch_to ) )
						)
					)
				)
				{
//NSLog( @"To : found %@", n_mac_str2nsstring( &line[ cch_to ] ) );
					break;
				}

				cch_to += step;
			}


			caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch_to
			);

		}

	}


	shift_selection_is_tail = TRUE;


//n_caret_debug_cch( caret_fr, caret_to );
	return;	
}




-(void) NonnonTxtboxCaretOutOfCanvasUpDown
{
//NSLog( @"NonnonTxtboxCaretOutOfCanvasUpDown" );

	CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
	CGFloat items_per_canvas = trunc( csy / font_size.height );


	CGFloat minim = MIN( caret_fr.cch.y, caret_to.cch.y );
	CGFloat maxim = MAX( caret_fr.cch.y, caret_to.cch.y );
//NSLog( @"%f %f %f %f", minim, maxim, scroll, items_per_canvas );

	// [!] : out of bound : centering

	if ( minim < ( trunc( scroll ) - 1 ) )
	{
//NSLog( @"minim" );
		scroll = minim - ( items_per_canvas / 2 );
		return;
	} else
	if ( maxim >= ( trunc( scroll ) + items_per_canvas + 1 ) )
	{
//NSLog( @"maxim" );
		scroll = maxim - ( items_per_canvas / 2 );
		return;
	} else {
//NSLog( @"others" );
	}

	// [!] : scroll per one line

	if ( minim < trunc( scroll ) )
	{
		scroll--;
	} else
	if ( maxim >= ( trunc( scroll ) + items_per_canvas ) )
	{
		scroll++;
	}

}

-(void) NonnonTxtboxCaretOutOfCanvasUpDownListbox
{
//NSLog( @"NonnonTxtboxCaretOutOfCanvasUpDownListbox" );

	CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
	CGFloat items_per_canvas = trunc( csy / font_size.height );


	CGFloat minim = n_focus;//MIN( caret_fr.cch.y, caret_to.cch.y );
	CGFloat maxim = n_focus;//MAX( caret_fr.cch.y, caret_to.cch.y );
//NSLog( @"%f %f %f %f", minim, maxim, scroll, items_per_canvas );

	// [!] : out of bound : centering

	if ( minim < ( trunc( scroll ) - 1 ) )
	{
//NSLog( @"minim" );
		scroll = minim - ( items_per_canvas / 2 );
		return;
	} else
	if ( maxim >= ( trunc( scroll ) + items_per_canvas + 1 ) )
	{
//NSLog( @"maxim" );
		scroll = maxim - ( items_per_canvas / 2 );
		return;
	} else {
//NSLog( @"others" );
	}

	// [!] : scroll per one line

	if ( minim < trunc( scroll ) )
	{
		scroll--;
	} else
	if ( maxim >= ( trunc( scroll ) + items_per_canvas ) )
	{
		scroll++;
	}

}

-(void) NonnonTxtboxCaretOutOfCanvasUpDownSearch
{
//NSLog( @"NonnonTxtboxCaretOutOfCanvasUpDown" );

	CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
	CGFloat items_per_canvas = csy / font_size.height;


	CGFloat minim = MIN( caret_fr.cch.y, caret_to.cch.y );
	CGFloat maxim = MAX( caret_fr.cch.y, caret_to.cch.y );
//NSLog( @"%f %f %f", minim, maxim, scroll );

	if ( minim < scroll )
	{
//NSLog( @"up" );
		scroll = minim - ( items_per_canvas / 2 );
	} else
	if ( maxim >= ( scroll + items_per_canvas - 1 ) )
	{
//NSLog( @"down : %f %f", maxim, ( scroll + items_per_canvas - 1 ) );
		scroll = maxim - ( items_per_canvas / 2 );
	}

}




- (void) NonnonTxtboxInsert:(NSString*)nsstring
{
//return;
//NSLog( @"modified cch : %lld", ime_caret_fr.cch.x );

	if ( n_txt_data->readonly ) { return; }


	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		if ( self.n_listbox_edit_onoff == FALSE ) { return; }
	}


	n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to );


	ime_caret_fr = caret_fr;
	ime_caret_to = caret_to;


//n_caret_debug_cch( caret_fr, caret_to );

	n_posix_char *line = n_txt_get( n_txt_data, n_focus );

	n_posix_char *line_f = n_string_carboncopy( line );
	n_posix_char *line_m = n_mac_nsstring2str( nsstring );
	n_posix_char *line_t = n_string_carboncopy( line );

//NSLog( @"modified : %s : %@", line_f, n_mac_str2nsstring( line_t ) );


	n_type_int cch_f = n_posix_strlen( line_f );
	n_type_int cch_m = n_posix_strlen( line_m );
	n_type_int cch_t = n_posix_strlen( line_t );


	n_posix_char *mod = n_string_new( cch_f + cch_m + cch_t );

	if ( ime_caret_fr.cch.x < cch_f )
	{
		line_f[ ime_caret_fr.cch.x ] = N_STRING_CHAR_NUL;
	}
//NSLog( @"%s", line_f );

	n_type_int cch_cut = ime_caret_to.cch.x;
//NSLog( @"cch_cut : %lld", cch_cut );

	if ( ( cch_cut >= 0 )&&( cch_cut < cch_f ) )
	{
		n_posix_sprintf_literal( line_t, "%s", &line_t[ cch_cut ] );
	} else {
		n_string_truncate( line_t );
	}

	n_posix_sprintf_literal( mod, "%s%s%s", line_f, line_m, line_t );
//NSLog( @"%s : %s : %s", line_f, line_m, line_t );

	n_txt_mod_fast( n_txt_data, n_focus, mod );


	n_string_free( line_f );
	n_string_free( line_m );
	n_string_free( line_t );


	caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		ime_caret_fr.cch.x + cch_m
	);

	ime_caret_fr = caret_fr;
	ime_caret_to = caret_to;


	n_edited = TRUE;
	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
	{
		[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
	}


	return;
}

- (void) NonnonTxtboxSelectAll:(BOOL)redraw
{

	caret_fr = NonnonMakeCaret( 0, 0, 0, 0 );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_txt_data->sy - 1,
		font,
		font_size,
		n_posix_strlen( n_txt_get( n_txt_data, n_txt_data->sy - 1 ) )
	);

	caret_to = NonnonMakeCaret
	(
		caret.pxl.x,
		( (CGFloat) n_txt_data->sy - 1 ) * font_size.height,
		caret.cch.x,
		n_txt_data->sy - 1
	);

	if ( redraw )
	{
		[self NonnonTxtboxRedraw];
	}


	return;
}



/*
- (void) applicationWillBecomeActive:(NSNotification *)notification
{
//NSLog( @"applicationWillBecomeActive" );

	is_grayed = NO;
	[self NonnonTxtboxRedraw];

}

- (void) applicationWillResignActive:(NSNotification *)notification
{
//NSLog( @"applicationWillResignActive" );

	is_grayed = YES;
	[self NonnonTxtboxRedraw];

}
*/



#ifdef N_TXTBOX_IME_ENABLE

- (BOOL)hasMarkedText;
{
//NSLog( @"hasMarkedText" );

	return NO;
}

- (NSRange)markedRange
{
//NSLog( @"markedRange" );

	NSRange range = { NSNotFound, 0 };
	return range;
}

- (NSRange)selectedRange
{
//NSLog( @"selectedRange" );

	NSRange range = { NSNotFound, 0 };
	return range;
}

- (void)setMarkedText:(id)string 
        selectedRange:(NSRange)selectedRange 
     replacementRange:(NSRange)replacementRange
{
//NSLog( @"setMarkedText : %@", [string string] );

//NSLog( @"setMarkedText : Set     %lu %lu",    selectedRange.location,    selectedRange.length );
//NSLog( @"setMarkedText : Replace %lu %lu", replacementRange.location, replacementRange.length );


	// [x] : buggy : when IME is OFF : no insertText is sent
	ime_is_first = FALSE;


	BOOL prv_onoff = ime_onoff;

	ime_nsstr = [[string string] copy];
	ime_onoff = ( [ime_nsstr length] != 0 );
	ime_freed = FALSE;
	ime_delay = FALSE;
	ime_focus = selectedRange;

	ime_caret_offset = selectedRange.location + selectedRange.length;

//NSLog( @"NSString : %@", ime_nsstr );


	if ( ime_onoff )
	{
		if ( prv_onoff == FALSE ) { ime_caret_fr = caret_fr; }
		ime_caret_to = caret_fr;

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];
	}


	return;
}

- (void)unmarkText
{
//NSLog( @"unmarkText" );

	return;
}

- (NSArray<NSAttributedStringKey> *)validAttributesForMarkedText
{
//NSLog( @"validAttributesForMarkedText" );

	return [[NSArray alloc] init];
}

- (NSAttributedString *)attributedSubstringForProposedRange:(NSRange)range 
                                                actualRange:(NSRangePointer)actualRange
{
//NSLog( @"attributedSubstringForProposedRange" );

	return nil;
}

- (void)insertText:(id)string 
  replacementRange:(NSRange)replacementRange
{
//NSLog( @"insertText" );
//NSLog( @"insertText : %lu %lu", replacementRange.location, replacementRange.length );

//NSLog( @"insertText : %d : %@ : %@", ime_is_first, string, ime_nsstr );

	// [x] : buggy : when IME is OFF : no insertText is sent
	ime_is_first = TRUE;

	ime_delay = ime_onoff;

	[ime discardMarkedText];
	ime_nsstr = @"";
	ime_onoff = FALSE;

	[self NonnonTxtboxInsert:string];


	return;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point
{
//NSLog( @"characterIndexForPoint" );

	return 0;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range 
                         actualRange:(NSRangePointer)actualRange
{
//NSLog( @"firstRectForCharacterRange" );
//NSLog( @"firstRectForCharacterRange : %@", NSStringFromRange( range ) );


	n_posix_char *str = n_mac_nsstring2str( ime_nsstr );
	//n_type_int    cch = n_posix_strlen( str );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		ime_caret_fr.cch.x// + cch
	);

	n_string_free( str );


	// [Needed] : composition popup needs trunc()
//NSLog( @"%f", fmod( scroll, 1 ) );

	CGFloat ox = caret.pxl.x + padding;
	CGFloat oy = caret.pxl.y - ( trunc( scroll ) * font_size.height );
	//CGFloat yy = NSHeight( [ [self.window screen] visibleFrame ] );
//NSLog( @" %0.2f %0.2f : %0.2f : %0.2f ", ox, oy, yy, scroll );


	// [x] : oy : offset is not needed when popup moves to upper side

	ox += offset_x;
	oy += offset_y + font_size.height;

//NSLog( @" %0.2f %0.2f : %0.2f : %0.2f ", ox, oy, yy, scroll );


	NSPoint point         = NSMakePoint( ox, oy );
	NSPoint pointInWindow = [self convertPoint:point toView:nil];
	NSRect  rect          = NSMakeRect( pointInWindow.x, pointInWindow.y, 0, 0 );
	NSRect  pointOnScreen = [[self window] convertRectToScreen:rect];

//NSLog( @" %0.2f %0.2f ", pointOnScreen.origin.x, pointOnScreen.origin.y );


	return pointOnScreen;
}

- (void)doCommandBySelector:(SEL)selector
{
//NSLog( @"doCommandBySelector" );

	return;
}
#endif




@end




#include "n_txtbox/07_dnd.c"
#include "n_txtbox/08_draw.c"
#include "n_txtbox/09_search.c"
#include "n_txtbox/10_keyboard.c"
#include "n_txtbox/11_mouse.c"




#endif // _H_NONNON_MAC_NONNON_TXTBOX


