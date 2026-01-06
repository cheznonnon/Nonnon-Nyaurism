// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSView (Custom View) to NonnonListbox
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonListbox"
//	3 : call _n_list.delegate_doubleclick = self; at awakeFromNib
//	4 : modify behavior


// [!] : trouble shooter
//
//	[ drawRect is not called at redraw ]
//
//		a : call [_n_list display];
//		b : see your @property and connect(DnD) to UI on the Xib canvas
//		c : layer may cause




#ifndef _H_NONNON_MAC_NONNON_LISTBOX
#define _H_NONNON_MAC_NONNON_LISTBOX




#import <Cocoa/Cocoa.h>


#include "../neutral/bmp/ui/roundframe.c"
#include "../neutral/txt.c"


#include "_mac.c"
#include "image.c"
#include "window.c"




#define N_MAC_LISTBOX_DRAW_LINENUMBER_NONE            ( 0 << 0 )
#define N_MAC_LISTBOX_DRAW_LINENUMBER_ZEROBASED_INDEX ( 1 << 0 )

// internal
void
n_mac_listbox_draw_linenumber
(
	      NSFont *font,
	  n_type_int  index,
	  n_type_int  offset,
	  n_type_int  txt_sy,
	  n_type_int  focus_f,
	  n_type_int  focus_t,
	  n_type_gfx   x,
	  n_type_gfx   y,
	  n_type_gfx  sx,
	  n_type_gfx  sy,
	     NSColor *color_back,
	     NSColor *color_text,
	         int  option
)
{
//return;

	n_type_int cch_y = index + offset;

	if ( option & N_MAC_LISTBOX_DRAW_LINENUMBER_ZEROBASED_INDEX )
	{
		//
	} else {
		cch_y++;
	}

	n_posix_bool over_ten_thousand = n_posix_false;
	if ( cch_y >= 10000 ) { over_ten_thousand = n_posix_true; }

	if ( cch_y >= 10000 ) { cch_y = cch_y % 10000; }

	//n_posix_char str[ 6 + 1 ];
	NSString *nsstr;

	// [Patch] : not working accurately

	if ( cch_y < 1000 )
	{
		if ( over_ten_thousand )
		{
			//n_posix_sprintf_literal( str, " %04d ", (int) cch_y );
			nsstr = [[NSString alloc] initWithFormat:@" %04d ", (int) cch_y];
		} else {
			//n_posix_sprintf_literal( str, " % 4d ", (int) cch_y );
			nsstr = [[NSString alloc] initWithFormat:@" % 4d ", (int) cch_y];
		}
	} else {
		//n_posix_sprintf_literal( str, " %d "  , (int) cch_y );
		nsstr = [[NSString alloc] initWithFormat:@" %d ", (int) cch_y];
	}


	NSColor *color_stripe;
	if ( ( index + offset ) & 1 )
	{
		color_stripe = [color_text blendedColorWithFraction:0.90 ofColor:color_back];
	} else {
		color_stripe = [color_text blendedColorWithFraction:0.95 ofColor:color_back];
	}

	NSRect rect = NSMakeRect( x,y,sx,sy );

	[color_stripe set];
	[NSBezierPath fillRect:rect];

	if ( ( index + offset ) < txt_sy )
	{

		NSColor *color_txt;
		if ( ( ( index + offset ) >= focus_f )&&( ( index + offset ) <= focus_t ) )
		{
			// [!] : indicator

			n_type_gfx size = 2;

			NSRect r = NSMakeRect( x,y,size,sy );

			[[NSColor controlAccentColor] set];
			[NSBezierPath fillRect:r];

			color_txt = [color_text blendedColorWithFraction:0.33 ofColor:color_back];
		} else {
			color_txt = [color_text blendedColorWithFraction:0.66 ofColor:color_back];
		}


		NSMutableDictionary *attr = [NSMutableDictionary dictionary];
		[attr setObject:font      forKey:NSFontAttributeName           ];
		[attr setObject:color_txt forKey:NSForegroundColorAttributeName];

		[nsstr drawInRect:rect withAttributes:attr];

	}


	return;
}




@interface NonnonListbox : NSView

@property (nonatomic,assign) id delegate_doubleclick;

@end


@interface NonnonListbox ()

@property CGFloat  n_focus;
@property n_txt   *n_txt;

@end


@implementation NonnonListbox {

	NSFont     *font;
	CGSize      font_size;
	CGFloat     scroll;
	CGPoint     pt;
	CGPoint     pt_press;
	NSRect      scroller_rect_thumb;
	NSRect      scroller_rect_shaft;
	CGFloat     offset;
	CGFloat     margin;
	BOOL        thumb_is_captured;
	BOOL        thumb_is_hovered;
	BOOL        shaft_is_hovered;
	CGFloat     thumb_offset;
	n_bmp_fade  fade;
	CGFloat     client_sy;

	NSFont     *linenumber_font;
	CGSize      linenumber_size;
	u32         linenumber_color_back;
	u32         linenumber_color_text;

	n_bmp       cache_base;
	n_bmp       cache_padding;
	n_bmp       cache_frame;

}


@synthesize delegate_doubleclick;


@synthesize n_focus;
@synthesize n_txt;




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{
		// [x] : not visible
		//array = [[NSMutableArray alloc] init];

		font      = [NSFont fontWithName:@"Helvetica" size:14];
		font_size = n_mac_image_text_pixelsize( @"a", font );

		offset    =  6;
		margin    =  2;

		linenumber_font = [NSFont userFixedPitchFontOfSize:14];
		linenumber_size = n_mac_image_text_pixelsize( @"000000", linenumber_font );

		[self n_reset];

		n_bmp_fade_init( &fade, n_bmp_black );

		n_bmp_zero( &cache_base );
		n_bmp_zero( &cache_padding );
		n_bmp_zero( &cache_frame );

		[[NSNotificationCenter defaultCenter]
			addObserver: self
		           selector: @selector( windowDidResize: )
			       name: NSWindowDidResizeNotification
			     object: nil
		];

		n_mac_timer_init( self, @selector( n_timer_method ), 33 );

	}
	
	return self;
}


- (void) windowDidResize:(NSNotification *)notification
{
//NSLog( @"resized" );

	n_bmp_free( &cache_base );
	n_bmp_free( &cache_padding );
	n_bmp_free( &cache_frame );

}


- (void) n_timer_method
{

	static BOOL prv = FALSE;

	thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_thumb );

	if ( thumb_is_hovered != prv )
	{
//[self display];
		if ( thumb_is_hovered )
		{
			n_bmp_fade_go( &fade, n_bmp_white );
		} else {
			n_bmp_fade_go( &fade, n_bmp_black );
		}
	}

	n_bmp_fade_engine( &fade, n_posix_true );
//NSLog( @"%d", fade.percent );
	if ( fade.stop == n_posix_false )
	{
		[self display];
	}

	prv = thumb_is_hovered;

}


- (BOOL) isFlipped
{
	return YES;
}


- (void) n_reset
{
	scroll  =  0;
	n_focus = -1;

	scroller_rect_thumb = NSMakeRect( 0,0,0,0 );
	scroller_rect_shaft = NSMakeRect( 0,0,0,0 );

	thumb_is_hovered = FALSE;

}


- (void) n_scroll_clamp
{

	CGFloat o                = offset;
	CGFloat items_per_canvas = ( self.frame.size.height - ( o * 2 ) ) / font_size.height;
	CGFloat txt_sy           = n_txt->sy;
//NSLog( @"%f %f", page, txt_sy );

	if ( txt_sy <= 1 )
	{
		scroll = 0;
	} else
	if ( items_per_canvas <= txt_sy )
	{
		if ( fabs( items_per_canvas ) ) { items_per_canvas -= 1.0; }

		CGFloat max_sy = txt_sy - items_per_canvas;

		if ( scroll <       0 ) { scroll =      0; } else
		if ( scroll >= max_sy ) { scroll = max_sy; }
	} else {
		scroll = 0;
	}

}


- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );

//NSLog( @"%lld", n_test_txt.sy );


	// [!] : Metrics

	BOOL is_darkmode = n_mac_is_darkmode();

	[self n_scroll_clamp];

	n_type_gfx sx = (n_type_gfx) rect.size.width;
	n_type_gfx sy = (n_type_gfx) rect.size.height;

	CGFloat o = offset;
	CGFloat m = margin;

	CGFloat minim = font_size.height + ( o * 2 );
	if ( ( sx < minim )||( sy < minim ) ) { return; }


	// [!] : Metrics 2 : Colors

	NSColor *nscolor_text  = [NSColor textColor];
	NSColor *nscolor_back  = [NSColor textBackgroundColor];
	u32        color_bg    = n_mac_nscolor2argb( nscolor_back );
	u32        color_fg    = n_mac_nscolor2argb( nscolor_text );
	u32        color_frame = n_bmp_blend_pixel( color_bg, color_fg, 0.25 );
	NSColor *nscolor_text_normal = nscolor_text;

	NSColor *nscolor_text_highlight;
	if ( is_darkmode )
	{
		nscolor_text_highlight = nscolor_text;
	} else {
		nscolor_text_highlight = [NSColor controlHighlightColor];
	}

	u32        color_stripe;
	NSColor *nscolor_stripe;

	if ( is_darkmode )
	{
		  color_stripe = n_bmp_blend_pixel( color_bg, color_fg, 0.10 );
		nscolor_stripe = n_mac_argb2nscolor( color_stripe );
	} else {
		  color_stripe = n_bmp_blend_pixel( color_bg, color_fg, 0.05 );
		nscolor_stripe = n_mac_argb2nscolor( color_stripe );
	}


	// [!] : Contents

	n_bmp frame; n_bmp_zero( &frame ); n_bmp_new_fast( &frame, sx,sy );
	n_bmp_flush( &frame, n_bmp_black_invisible );
//NSLog( @"%d %d", sx, sy );

	{
		if ( NULL == N_BMP_PTR( &cache_base ) )
		{
			n_bmp_carboncopy( &frame, &cache_base );
			n_bmp_flush( &cache_base, n_bmp_black_invisible );

			n_bmp_flush_roundrect( &cache_base, color_bg, 8 );
		}

		n_mac_image_nbmp_direct_draw( &cache_base, &rect, FALSE );

		//NSImage *img = n_mac_image_nbmp2nsimage( &frame );
		//[img drawInRect:rect];
	}


	// [!] : n_txt rendering engine

	NSMutableDictionary *attr = [NSMutableDictionary dictionary];
	[attr setObject:font forKey:NSFontAttributeName ];

	NSRect r = NSMakeRect( o, o, rect.size.width - ( o * 2 ), 0 );

	r.origin.x += linenumber_size.width + ( o / 2 );

	r.size.height = font_size.height;

	CGFloat max_sy = rect.size.height - ( o * 2 );


	n_type_int i = scroll;
//NSLog( @"%f %lld", scroll, n_mac_listbox_txt.sy );
	n_posix_loop
	{//break;

		NSString *text = n_mac_str2nsstring( n_txt_get( n_txt, i ) );
		if ( ( text.length != 0 )&&( n_focus == i ) )
		{
			[attr setObject:nscolor_text_highlight forKey:NSForegroundColorAttributeName];
		} else {
			[attr setObject:nscolor_text_normal    forKey:NSForegroundColorAttributeName];
		}

		if ( ( text.length != 0 )&&( n_focus == i ) )
		{
			[[NSColor controlAccentColor] set];
			[NSBezierPath fillRect:r];
		} else
		if ( i & 1 )
		{
			[nscolor_stripe set];
			[NSBezierPath fillRect:r];
		}

		if ( i < n_txt->sy )
		{
			[text drawInRect:r withAttributes:attr];
//NSLog( @"%@", text );
		}


		r.origin.y += font_size.height;
		if ( r.origin.y > max_sy ) { break; }


		i++;
	}


	// [!] : Line Number

	{
		n_bmp_flip_onoff = n_posix_true;

		n_type_int     i = 0;
		n_type_gfx pxl_y = 0;
		n_posix_loop
		{

			n_mac_listbox_draw_linenumber
			(
				linenumber_font,
				i, scroll, n_txt->sy, n_focus, n_focus,
				o, o + pxl_y, linenumber_size.width, font_size.height,
				nscolor_back,
				nscolor_text,
				N_MAC_LISTBOX_DRAW_LINENUMBER_NONE
			);

			i++;

			pxl_y += font_size.height;
			if ( pxl_y > max_sy ) { break; }

		}

		n_bmp_flip_onoff = n_posix_false;
	}


	// [!] : Padding

	{

		if ( NULL == N_BMP_PTR( &cache_padding ) )
		{
			u32 outer = color_bg;
			u32 inner = n_bmp_alpha_replace_pixel( color_bg, 0 );

			n_bmp_carboncopy( &frame, &cache_padding );
			n_bmp_flush( &cache_padding, n_bmp_black_invisible );

			n_bmp_ui_roundframe( &cache_padding, 0,0,sx,sy, o + m + m, m + m, outer, inner );
		}

		n_mac_image_nbmp_direct_draw( &cache_padding, &rect, FALSE );

		//NSImage *img = n_mac_image_nbmp2nsimage( &frame );
		//[img drawInRect:rect];
	}


	// [!] : Frame

	{

		if ( NULL == N_BMP_PTR( &cache_frame ) )
		{
			u32 outer = color_frame;
			u32 inner = n_bmp_alpha_replace_pixel( color_bg, 0 );

			n_bmp_carboncopy( &frame, &cache_frame );
			n_bmp_flush( &cache_frame, n_bmp_black_invisible );

			n_bmp_ui_roundframe( &cache_frame, 0,0,sx,sy, o + m, m, outer, inner );
		}

		n_bmp_flush_fastcopy( &cache_frame, &frame );


		// [!] : Fake Scroller

		CGFloat csy              = sy - ( o * 2 );
		CGFloat items_per_canvas = csy / font_size.height;
		CGFloat max_count        = (CGFloat) n_txt->sy;

		client_sy = sy;

//NSLog( @"%f %f", items_per_canvas, max_count );
		if ( trunc( items_per_canvas ) < max_count )
		{

			CGFloat correction = font_size.height * ( client_sy / ( max_count * font_size.height ) );
//NSLog( @"%f", correction );

			CGFloat page = max_count / items_per_canvas;

			n_type_gfx scrsx = 12;
			n_type_gfx scrsy = MAX( 12, trunc( csy / page ) );
			n_type_gfx scr_x = sx - o - scrsx;
			n_type_gfx scr_y = ceil( scroll * correction );
//NSLog( @"%f %f", scroll, font_size.height ); 


			// [!] : this is a kind of cheat
			scr_y = n_posix_minmax_n_type_gfx( o, csy - scrsy + o, scr_y );


			// [!] : for hit test
//scroller_rect = NSMakeRect( 0, 0, 0, 0 );
			scroller_rect_shaft = NSMakeRect( scr_x,     o, scrsx,   csy );
			scroller_rect_thumb = NSMakeRect( scr_x, scr_y, scrsx, scrsy );


			u32 color = color_fg;

			n_bmp_flip_onoff = n_posix_true;

			u32 color_shaft = n_bmp_alpha_replace_pixel( color, 16 );
			n_bmp_roundrect( &frame, scr_x,o, scrsx,csy, color_shaft, 50 );

//int alpha = 64;
//if ( thumb_is_hovered ) { alpha = 96; }
//u32 color_thumb = n_bmp_alpha_replace_pixel( color, alpha );


			int alpha_fade_bg;
			int alpha_fade_fg;
			if ( fade.color_fg == n_bmp_white )
			{
				alpha_fade_bg = 64;
				alpha_fade_fg = 96;
			} else {
				alpha_fade_bg = 96;
				alpha_fade_fg = 64;
			}
			int alpha = n_bmp_blend_channel( alpha_fade_bg, alpha_fade_fg, (double) fade.percent * 0.01 );
//NSLog( @"%d", fade.percent );

			u32 color_thumb = n_bmp_alpha_replace_pixel( color, alpha );
			n_bmp_roundrect( &frame, scr_x,scr_y, scrsx,scrsy, color_thumb, 50 );

			n_bmp_flip_onoff = n_posix_false;
		}

		n_mac_image_nbmp_direct_draw( &frame, &rect, TRUE );

		//NSImage *img = n_mac_image_nbmp2nsimage( &frame );
		//[img drawInRect:rect];
	}


	n_bmp_free( &frame );

}




- (void) NonnonListboxSetFocus:(NSEvent*) theEvent
{
//NSLog( @"NonnonListboxSetFocus" );

	NSPoint event_location = [theEvent locationInWindow];
	NSPoint local_point    = [self convertPoint:event_location fromView:nil];

	n_focus = trunc( scroll + trunc( ( local_point.y - 4 ) / font_size.height ) );
//NSLog( @"%f", n_focus );

	thumb_is_hovered = n_posix_false;

	[self display];

}

- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog( @"rightMouseUp" );

	thumb_is_captured = FALSE;


	if ( thumb_is_hovered ) { return; }
	if ( shaft_is_hovered ) { return; }


	[self NonnonListboxSetFocus:theEvent];


	[delegate_doubleclick rightMouseUp:theEvent];

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp" );

	thumb_is_captured = FALSE;


	if ( thumb_is_hovered ) { return; }
	if ( shaft_is_hovered ) { return; }


	CGPoint pt_cur = [theEvent locationInWindow];

	CGFloat dx = fabs( pt_press.x - pt_cur.x );
	CGFloat dy = fabs( pt_press.y - pt_cur.y );
//NSLog( @"%f %f", pt_press.y, pt_cur.y );

	const CGFloat threshold = 4;

	if ( ( dx < threshold )&&( dy < threshold ) )
	{
		[self NonnonListboxSetFocus:theEvent];
	}


	[delegate_doubleclick mouseUp:theEvent];

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	pt               = [NSEvent mouseLocation];
	pt_press         = [theEvent locationInWindow];
	thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_thumb );
	shaft_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_shaft );
//if ( thumb_is_hovered ) { NSLog( @"!" ); }

	NSPoint pt_cur   = n_mac_cursor_position_get( self );
	thumb_offset     = scroller_rect_thumb.origin.y - pt_cur.y;
//NSLog( @"%f : %f %f", thumb_offset, scroller_rect_thumb.origin.y, pt_cur.y );

	if ( thumb_is_hovered )
	{
		thumb_is_captured = TRUE;
	} else
	if ( shaft_is_hovered )
	{

		CGFloat sy               = [self frame].size.height;
		CGFloat csy              = sy - ( offset * 2 );
		CGFloat items_per_canvas = csy / font_size.height;

		if ( pt_cur.y < scroller_rect_thumb.origin.y )
		{
//NSLog( @"upper" );
			scroll -= items_per_canvas;
		} else {
//NSLog( @"lower" );
			scroll += items_per_canvas;
		}

		[self display];
	}

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );

	CGPoint pt_cur = [NSEvent mouseLocation];

	CGFloat dy = pt.y - pt_cur.y;
//NSLog( @"%f %f", dy, [theEvent deltaY] );

	pt = pt_cur;

//scroll -= dy / font_size.height;

	if ( thumb_is_captured )
	{
//NSLog( @"1" ); 
		CGFloat max_count  = (CGFloat) n_txt->sy;
		CGFloat correction = client_sy / ( max_count * font_size.height );

		scroll += dy / ( font_size.height * correction );
	} else {
//NSLog( @"2" ); 
		scroll -= dy / font_size.height;
	}


	[self display];

}

- (void)scrollWheel:(NSEvent *)theEvent
{

	scroll -= [theEvent scrollingDeltaY] / font_size.height;

	[self display];
 
}

@end




#endif // _H_NONNON_MAC_NONNON_LISTBOX


