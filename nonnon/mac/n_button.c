// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File




#ifndef _H_NONNON_MAC_NONNON_BUTTON
#define _H_NONNON_MAC_NONNON_BUTTON




#import <Cocoa/Cocoa.h>


#include "../neutral/posix.c"

#include "../neutral/bmp/ui/roundframe.c"

#include "../neutral/bmp/all.c"
#include "../neutral/bmp/fade.c"

#include "../bridge/gdi.c"

#include "image.c"




void
n_mac_image_bmp_halo( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_gdi gdi; n_gdi_zero( &gdi );

	n_type_gfx size = N_BMP_SX( bmp );
//NSLog( @"%d", size );

	gdi.base_color_bg = n_bmp_black_invisible;

	gdi.icon_in       = bmp;
	gdi.icon_sx       = size;
	gdi.icon_sy       = size;
	gdi.icon_style    = N_GDI_ICON_SMOOTH;

	u32 color_outline;
	if ( n_mac_is_darkmode() )
	{
		color_outline = n_bmp_rgb( 111,111,111 );
	} else {
		color_outline = n_bmp_rgb( 192,192,192 );
	}

	gdi.icon_effect_style[ 0 ] = N_GDI_EFFECT_OUTLINE_FOG;
	gdi.icon_effect_color[ 0 ] = color_outline;
	gdi.icon_effect_param[ 0 ] = 3;

	n_gdi_bmp( &gdi, bmp );


	return;
}




#define N_MAC_BUTTON_FADE_TYPE_HOT    ( 0 )
#define N_MAC_BUTTON_FADE_TYPE_PRESS  ( 1 )
#define N_MAC_BUTTON_FADE_TYPE_NORMAL ( 2 )




typedef struct {

	// [!] : internal

	double       scale;

	n_bmp_fade   fade;

	u32          color_bg_normal;
	u32          color_bg_hot;
	u32          color_bg_pressed;
	u32          color_bg_disabled;

	// [!] : hidden : use "button.hidden" property

	// [!] : enable_onoff : you need to handle manually : NSView hasn't ".enable" property

	BOOL       enable_onoff;
	BOOL         sink_onoff;
	BOOL          hot_onoff;
	BOOL        press_onoff;
	BOOL         prev_onoff;
	BOOL         fake_onoff;
	BOOL        right_onoff;
	BOOL       no_hot_onoff;

	n_bmp         icon_main;
	n_bmp         icon_gray;
	n_bmp         icon_halo;


	// [!] : option : you can set these directly

	NSWindow     *nswindow;

	BOOL          show_border;
	BOOL          direct_click_onoff;
	BOOL          menu_click_as_normal_click;
	BOOL          auto_halo_onoff;

} n_mac_button;


#define n_mac_button_zero( p ) n_memory_zero( p, sizeof( n_mac_button ) )




void
n_mac_button_system_themed( n_bmp *bmp )
{

	NSColor *nscolor_accent = [NSColor controlAccentColor];

	u32 base = n_mac_nscolor2argb( nscolor_accent );
	u32 face = n_bmp_black;
	u32 lite = n_bmp_white;
	u32 pink = n_bmp_hue_wheel_tweak_pixel( base, 64 );

	n_bmp_flush_replacer( bmp, n_bmp_rgb( 200,240,255 ), n_bmp_blend_pixel( base, lite, 0.75 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb( 100,220,255 ), n_bmp_blend_pixel( base, lite, 0.50 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb(   0,200,255 ), n_bmp_blend_pixel( base, face, 0.00 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb(   0,150,200 ), n_bmp_blend_pixel( base, face, 0.25 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb(   0,100,150 ), n_bmp_blend_pixel( base, face, 0.50 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb(   0, 50,100 ), n_bmp_blend_pixel( base, face, 0.75 ) );

	n_bmp_flush_replacer( bmp, n_bmp_rgb( 255,  0,150 ), n_bmp_blend_pixel( pink, face, 0.00 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb( 200,  0,100 ), n_bmp_blend_pixel( pink, face, 0.25 ) );
	n_bmp_flush_replacer( bmp, n_bmp_rgb( 150,  0, 50 ), n_bmp_blend_pixel( pink, face, 0.50 ) );


	return;
}

void
n_mac_button_box( n_bmp *bmp, NSRect rect, NSColor *color )
{

	n_type_gfx x,y,sx,sy; n_mac_rect_expand_size( rect, &x, &y, &sx, &sy );

	u32 argb = n_mac_nscolor2argb( color );

	n_bmp_box( bmp, x,y,sx,sy, argb );


	return;
}




void
n_mac_button_color_scheme( n_mac_button *p )
{

	if ( n_mac_is_darkmode() )
	{
		p->color_bg_normal   = n_mac_nscolor2argb( [NSColor clearColor] );
		p->color_bg_hot      = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_white, 0.25 );
		p->color_bg_pressed  = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_white, 0.50 );
		p->color_bg_disabled = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_white, 0.10 );
	} else {
		p->color_bg_normal   = n_mac_nscolor2argb( [NSColor clearColor] );
		p->color_bg_hot      = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_white, 0.80 );
		p->color_bg_pressed  = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_black, 0.25 );
		p->color_bg_disabled = n_bmp_blend_pixel( p->color_bg_normal, n_bmp_black, 0.10 );
	}


	return;
}

BOOL
n_mac_button_is_enabled( n_mac_button *p )
{
	return p->enable_onoff;
}

void
n_mac_button_fade_go( n_mac_button *p, u32 color )
{

	if ( p->fade.color_to == color ) { return; }

	p->fade.stop = TRUE;

	n_bmp_fade_go( &p->fade, color );


	return;
}

void
n_mac_button_draw( n_mac_button *p, NSRect rect )
{

	n_type_gfx sx,sy; n_mac_rect_expand_size( rect, NULL, NULL, &sx, &sy );
//NSLog( @"Canvas Size %d", sx );


	n_mac_button_color_scheme( p );


	n_bmp bmp_canvas; n_bmp_zero( &bmp_canvas ); n_bmp_new_fast( &bmp_canvas, sx,sy );
	n_bmp_flush( &bmp_canvas, p->color_bg_normal );


	u32 color_border;
	if ( p->show_border )
	{
		if ( n_mac_is_darkmode() )
		{
			color_border = n_mac_nscolor2argb( [NSColor      grayColor] );
		} else {
			color_border = n_mac_nscolor2argb( [NSColor lightGrayColor] );
		}
	} else {
		color_border = n_mac_nscolor2argb( [NSColor clearColor] );
	}


	u32 color_bg = p->fade.color_md;

	n_type_gfx  ox = 2;
	n_type_gfx  oy = 2;
	n_type_gfx osx = ox * 2;
	n_type_gfx osy = oy * 2;

	if ( p->show_border )
	{
		n_bmp_squircle( &bmp_canvas, ox,oy,sx-osx,sy-osy, color_border, 6.66 );

		ox  += 1;
		oy  += 1;
		osx += 2;
		osy += 2;

		n_bmp_squircle( &bmp_canvas, ox,oy,sx-osx,sy-osy, color_bg    , 6.66 );
	} else {
		n_bmp_squircle( &bmp_canvas, ox,oy,sx-osx,sy-osy, color_bg    , 6.66 );
	}


	n_type_gfx scale       = 1;
	n_type_gfx offset_sink = scale;


	{
		n_type_gfx ico_sx = N_BMP_SX( &p->icon_main );
		n_type_gfx ico_sy = N_BMP_SY( &p->icon_main );


		n_type_gfx tx = ( sx - ico_sx ) / 2;
		n_type_gfx ty = ( sy - ico_sy ) / 2;

		if ( p->sink_onoff )
		{
			n_type_gfx o = offset_sink;

			tx += o;
			ty += o;
		}

		if (
			( p->fade.stop == FALSE )
			&&
			(
				( p->fade.color_fr == p->color_bg_disabled )
				||
				( p->fade.color_to == p->color_bg_disabled )
			)
		)
		{
			n_type_real pc = p->fade.percent * 0.01;

			if ( p->auto_halo_onoff )
			{
				if ( p->fade.color_to == p->color_bg_disabled )
				{
					n_bmp_blendcopy( &p->icon_halo, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty,       pc );
				} else {
					n_bmp_blendcopy( &p->icon_halo, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.2 * pc );
				}
			}

			if ( p->fade.color_to == p->color_bg_disabled )
			{
				n_bmp_blendcopy( &p->icon_gray, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.2 * (1.0-pc) );
				n_bmp_blendcopy( &p->icon_main, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty,            pc  );
			} else {
				n_bmp_blendcopy( &p->icon_main, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty,       (1.0-pc) );
				n_bmp_blendcopy( &p->icon_gray, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.2 *      pc  );
			}
		} else
		if ( n_mac_button_is_enabled( p ) )
		{
			if ( p->auto_halo_onoff )
			{
				//n_bmp_rasterizer( &p->icon_halo, &bmp_canvas, tx,ty, n_bmp_rgb_mac( 255,255,255 ), FALSE );
				n_bmp_transcopy( &p->icon_halo, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty );
			}

			n_bmp_transcopy( &p->icon_main, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty );
		} else {
			if ( p->auto_halo_onoff )
			{
				//n_bmp_rasterizer( &p->icon_halo, &bmp_canvas, tx,ty, p->color_bg_disabled );
				n_bmp_blendcopy( &p->icon_halo, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.2 );
			}
			n_bmp_blendcopy( &p->icon_gray, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.0 );
		}

	}


	n_bmp_mac_color( &bmp_canvas );
	n_mac_image_nbmp_direct_draw( &bmp_canvas, &rect, NO );


	n_bmp_free_fast( &bmp_canvas );


	return;
}




@interface NonnonButton : NSView

@property (nonatomic,assign) id delegate;
@property (nonatomic,assign) id delegate_right;

@end


@implementation NonnonButton {

	n_mac_button button;
	BOOL         n_fade_queue;

}


@synthesize delegate;
@synthesize delegate_right;




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{

		n_mac_button_zero( &button );


		n_mac_button_color_scheme( &button );

		button.auto_halo_onoff = TRUE;

		n_bmp_fade_init( &button.fade, button.color_bg_normal );

		n_mac_timer_init( self, @selector( n_timer_method_fade ),  33 );


		NSTrackingArea* trackingArea = [[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways
			       owner:self
			    userInfo:nil
		];
		[self addTrackingArea:trackingArea];


		// [!] : for lack of mouse capture
		//n_mac_timer_init( self, @selector( n_timer_method_press_n_run ), 500 );

		[NSEvent addLocalMonitorForEventsMatchingMask:
			NSEventMaskLeftMouseDragged |
			NSEventMaskLeftMouseUp
			handler:^NSEvent* _Nullable( NSEvent * _Nonnull event )
			{
				switch( event.type )
				{
					case NSEventTypeLeftMouseDragged:

						[self mouseDragged:event];

					break;

					case NSEventTypeLeftMouseUp:

						[self mouseUp:event];

					break;

					default:

						// [Needed]

					break;
				}

				return event;
			}
		];

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
	}

	return self;
}




- (BOOL) isFlipped
{
	return YES;
}



- (void) n_fade_go
{
//return;

	// [x] : don't do n_mac_timer_exit() in a timer method

	n_fade_queue = TRUE;

}

- (void) n_timer_method_fade
{
//return;

	if ( n_fade_queue == FALSE ) { return; }


	n_mac_button *p = &button;

//p->fade.msec = 1000;
	n_bmp_fade_engine( &p->fade, TRUE );

	[self display];

//NSLog( @"%d", p->fade.percent );
	if ( p->fade.percent >= 100 )
	{
		n_fade_queue = FALSE;
	}

}




- (void) n_nswindow_set:(NSWindow*)nswindow
{

	n_mac_button *p = &button;

	p->nswindow = nswindow;

}

- (void) n_icon_free
{

	n_mac_button *p = &button;

	n_bmp_free( &p->icon_main );
	n_bmp_free( &p->icon_gray );
	n_bmp_free( &p->icon_halo );

}

- (void) n_icon_make:(n_bmp*) icon mode:(int)mode;
{

	n_type_gfx sx = self.bounds.size.width;
	n_type_gfx sy = self.bounds.size.height;

	n_bmp_resizer( icon, sx,sy, n_bmp_black_invisible, N_BMP_RESIZER_CENTER );

	if ( mode == 0 )
	{
		//
	} else
	if ( mode == 1 )
	{
		n_mac_image_bmp_gray( icon );
	} else
	if ( mode == 2 )
	{
		n_mac_image_bmp_halo( icon );
	}

}

- (void) n_icon_set:(n_bmp*) icon;
{

	n_mac_button *p = &button;


	[self n_icon_free];

	n_bmp_carboncopy( icon, &p->icon_main );
	n_bmp_carboncopy( icon, &p->icon_gray );
	n_bmp_carboncopy( icon, &p->icon_halo );


	[self n_icon_make:&p->icon_main mode:0];
	[self n_icon_make:&p->icon_gray mode:1];
	[self n_icon_make:&p->icon_halo mode:2];

}

- (void) n_icon_set_fast:(n_bmp*)icon_main gray:(n_bmp*)icon_gray halo:(n_bmp*)icon_halo;
{

	n_mac_button *p = &button;

	n_bmp_alias( icon_main, &p->icon_main );
	n_bmp_alias( icon_gray, &p->icon_gray );
	n_bmp_alias( icon_halo, &p->icon_halo );


}

- (void) n_reset
{

	n_mac_button *p = &button;

	p->hot_onoff   = FALSE;
	p->press_onoff = FALSE;

	n_mac_button_fade_go( p, p->color_bg_normal );
	[self n_fade_go];

	[self display];

}

- (void) n_no_hot
{

	n_mac_button *p = &button;

	p->no_hot_onoff = TRUE;;

	[self n_reset];

}

- (void) n_auto_halo:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->auto_halo_onoff = onoff;

}

- (BOOL) n_is_enabled
{

	n_mac_button *p = &button;

	return p->enable_onoff;
}

- (void) n_enable:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->enable_onoff = onoff;

	if ( onoff )
	{
		n_mac_button_fade_go( p, p->color_bg_normal );
	} else {
		static BOOL is_init = TRUE;
		if ( is_init )
		{
			is_init = FALSE;
			n_bmp_fade_init( &p->fade, button.color_bg_disabled );
		} else {
			n_mac_button_fade_go( p, p->color_bg_disabled );
		}
	}

	[self n_fade_go];

	[self display];

}

- (BOOL) n_is_pressed
{

	n_mac_button *p = &button;

	return p->press_onoff;
}

- (BOOL) n_is_pressed_right
{

	n_mac_button *p = &button;

	return p->right_onoff;
}

- (void) n_press:(BOOL)onoff
{

	// [!] : important : on/off switch : use below "n_fake" to make pressed effect

	n_mac_button *p = &button;

	p->prev_onoff = p->press_onoff = onoff;

}

- (void) n_fake:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->fake_onoff = onoff;

	if ( onoff )
	{
		n_mac_button_fade_go( p, p->color_bg_pressed );
	} else {
		if ( n_mac_window_is_hovered( self ) )
		{
			p->hot_onoff = TRUE;
			n_mac_button_fade_go( p, p->color_bg_hot );
		} else {
			p->hot_onoff = FALSE;
			n_mac_button_fade_go( p, p->color_bg_normal );
		}
	}

	[self n_fade_go];

	[self display];

}

- (void) n_border:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->show_border = onoff;

}

- (void) n_direct_click:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->direct_click_onoff = onoff;

}

- (void) menu_click_as_normal_click:(BOOL)onoff
{

	n_mac_button *p = &button;

	p->menu_click_as_normal_click = onoff;

}




- (BOOL) acceptsFirstMouse:(NSEvent *)event
{

	n_mac_button *p = &button;

	return p->direct_click_onoff;
}




- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );
//NSLog( @"%f %f", rect.size.width, rect.size.height );
//NSLog( @"%f %f", [super bounds].size.width, [super bounds].size.height );

	n_mac_button *p = &button;


	// [x] : Sonoma Xcode 15 : rect has window size at the first run

	n_mac_button_draw( p, [super bounds] );

}




- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");
//return;

	n_mac_button *p = &button;


	if ( FALSE == n_mac_button_is_enabled( p ) ) { return; }


	p-> prev_onoff = p->press_onoff;
	p->  hot_onoff = FALSE;
	p-> sink_onoff = TRUE;
	p->press_onoff = TRUE;

	n_mac_button_fade_go( p, p->color_bg_pressed );

	[self n_fade_go];

	[self display];


	if ( n_mac_window_is_hovered( self ) )
	{
		if ( delegate ) { [delegate mouseDown:theEvent]; }
	}

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"mouseUp");
//return;

	n_mac_button *p = &button;


	if ( FALSE == n_mac_button_is_enabled( p ) ) { return; }


	if ( p->press_onoff )
	{
//NSLog(@"mouseUp : press_onoff" );

		p->sink_onoff = FALSE;

		if ( n_mac_window_is_hovered( self ) )
		{
//NSLog(@"mouseUp : press_onoff : n_mac_window_is_hovered() : %x", self );
			if ( delegate ) { [delegate mouseUp:theEvent]; }
		}

		if ( p->no_hot_onoff )
		{
			p->no_hot_onoff = FALSE;
		} else
		if ( p->fake_onoff )
		{
			[self display];
		} else
		if ( p->enable_onoff )
		{
			p->hot_onoff = TRUE;
			n_mac_button_fade_go( p, p->color_bg_hot );
			[self n_fade_go];
			[self display];
		} else {
			p->hot_onoff = FALSE;
			n_mac_button_fade_go( p, p->color_bg_normal );
			[self n_fade_go];
			[self display];
		}

		p->press_onoff = p->prev_onoff;
	}

}




- (void) rightMouseDown:(NSEvent*) theEvent
{

	n_mac_button *p = &button;

	p->right_onoff = TRUE;

	if ( p->menu_click_as_normal_click )
	{
		[self mouseDown:theEvent];
	}

}

- (void) rightMouseUp:(NSEvent*) theEvent
{

	n_mac_button *p = &button;

	if ( p->menu_click_as_normal_click )
	{
		[self mouseUp:theEvent];
	}

	if ( p->right_onoff )
	{
		if ( n_mac_window_is_hovered( self ) )
		{
			[delegate_right rightMouseUp:theEvent];
		}

		p->right_onoff = FALSE;
	}

}




- (void) n_timer_method_press_n_run
{
//return;

	// [!] : prevent pressNrun

	if ( n_mac_window_is_hovered( self ) ) { return; }


	n_mac_button *p = &button;


	BOOL redraw = FALSE;


	if ( p->fake_onoff ) { return; }

	if ( FALSE == n_mac_button_is_enabled( p ) )
	{
		redraw = TRUE;

		p->hot_onoff = FALSE;
	} else
	if ( p->hot_onoff )
	{
		redraw = TRUE;

		p->hot_onoff = FALSE;
		n_mac_button_fade_go( p, p->color_bg_normal );
		[self n_fade_go];
	} else
	if ( p->press_onoff )
	{
		redraw = TRUE;

		p->press_onoff = p->prev_onoff;
		n_mac_button_fade_go( p, p->color_bg_normal );
		[self n_fade_go];
	}

	if ( p->sink_onoff )
	{
		redraw = TRUE;

		p->sink_onoff = FALSE;
	}


	if ( redraw )
	{
		[self display];
	}

}

-(void)mouseEntered:(NSEvent *)theEvent {
//NSLog(@"mouseEntered");

	n_mac_button *p = &button;

	if ( p->fake_onoff )
	{
		//
	} else
	if ( p->hot_onoff )
	{
		//
	} else
	if ( FALSE == n_mac_button_is_enabled( p ) )
	{
		//p->hot_onoff = FALSE;
		//n_mac_button_fade_go( p, p->color_bg_normal );
		//[self n_fade_go];
		//[self display];
	} else
	if (
		( p->nswindow == NULL )
		||
		( n_mac_window_is_keywindow( p->nswindow ) )
	)
	{
		p->hot_onoff = TRUE;
		n_mac_button_fade_go( p, p->color_bg_hot );
		[self n_fade_go];
		[self display];
	}

}

-(void)mouseDragged:(NSEvent *)event {
//NSLog(@"mouseDragged");

	[self n_timer_method_press_n_run];

}

-(void)mouseExited:(NSEvent *)theEvent {
//NSLog(@"mouseExited");

	[self n_timer_method_press_n_run];

}



- (void) applicationWillBecomeActive:(NSNotification *)notification
{
//NSLog( @"applicationWillBecomeActive" );
}

- (void) applicationWillResignActive:(NSNotification *)notification
{
//NSLog( @"applicationWillResignActive" );
}




@end




#endif // _H_NONNON_MAC_NONNON_BUTTON


