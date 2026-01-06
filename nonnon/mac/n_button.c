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

#include "image.c"




void
n_bmp_specialcopy( n_bmp *bmp_f, n_bmp *bmp_t, n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy, n_type_gfx tx, n_type_gfx ty, u32 fg, u32 bg )
{

	if ( n_bmp_error_clipping( bmp_f,bmp_t, &fx,&fy,&fsx,&fsy, &tx,&ty ) ) { return; }


	n_type_gfx xx = 0;
	n_type_gfx yy = 0;
	n_posix_loop
	{

		u32 color_f; n_bmp_ptr_get_fast( bmp_f, fx + xx, fy + yy, &color_f );
		u32 color_t; n_bmp_ptr_get_fast( bmp_t, tx + xx, ty + yy, &color_t );

		int a = n_bmp_a( color_f );

		u32 color;
		if ( 0 == a )
		{
			color = color_t;
		} else {
			if ( 255 == a )
			{
				color = color_f;
			} else {
				color = n_bmp_blend_pixel( fg, bg, n_bmp_blend_alpha2ratio( a ) );
			}
		}

		n_bmp_ptr_set_fast( bmp_t, tx + xx, ty + yy, color );


		xx++;
		if ( xx >= fsx )
		{

			xx = 0;

			yy++;
			if ( yy >= fsy ) { break; }
		}
	}


	return;
}



#define N_MAC_BUTTON_FADE_TYPE_HOT    ( 0 )
#define N_MAC_BUTTON_FADE_TYPE_PRESS  ( 1 )
#define N_MAC_BUTTON_FADE_TYPE_NORMAL ( 2 )




typedef struct {

	// [!] : internal

	double       scale;

	n_bmp_fade   fade;
	int          fade_type;

	n_posix_bool active_onoff;
	n_posix_bool   sink_onoff;
	n_posix_bool    hot_onoff;
	n_posix_bool   hide_onoff;
	n_posix_bool  press_onoff;
	n_posix_bool   prev_onoff;
	n_posix_bool   gray_onoff;
	n_posix_bool   fake_onoff;
	n_posix_bool  right_onoff;


	// [!] : option : you can set these directly

	NSWindow     *nswindow;

	n_bmp         icon;

	n_posix_bool  show_border;
	BOOL          direct_click_onoff;

	BOOL          menu_click_as_normal_click;

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

n_posix_bool
n_mac_button_is_enabled( n_mac_button *p )
{
	return p->active_onoff;
}

void
n_mac_button_fade_go( n_mac_button *p )
{

	u32 color;
	if ( p->fade.color_fg == n_bmp_white )
	{
		color = n_bmp_black;
	} else {
		color = n_bmp_white;
	}

	p->fade.stop = n_posix_true;

	n_bmp_fade_go( &p->fade, color );


	return;
}

void
n_mac_button_draw( n_mac_button *p, NSRect rect )
{

	n_type_gfx sx,sy; n_mac_rect_expand_size( rect, NULL, NULL, &sx, &sy );
//NSLog( @"Canvas Size %d", sx );


	n_bmp bmp_canvas; n_bmp_zero( &bmp_canvas ); n_bmp_new_fast( &bmp_canvas, sx,sy );
	n_bmp_flush( &bmp_canvas, n_mac_nscolor2argb( [NSColor clearColor] ) );


	u32 color_f;
	u32 color_t;
	u32 color_c;

	// [x] : clearColor : not beautiful in text edge

	if ( n_mac_is_darkmode() )
	{
		color_f = n_bmp_black;
		color_t = n_bmp_white;
		color_c = n_mac_nscolor2argb( [NSColor clearColor] );
	} else {
		color_f = n_bmp_white;
		color_t = n_bmp_black;
		color_c = n_mac_nscolor2argb( [NSColor clearColor] );

		// [x] : many colors are possible?
		//
		//	client area color will change slightly by window positon
		//	but windowBackgroundColor has a fixed value

//NSLog( @"%d %d %d", n_bmp_r( color_c ), n_bmp_g( color_c ), n_bmp_b( color_c ) );
	}

	n_type_real r_norml;
	n_type_real r_press;
	n_type_real r_disbl;

	if ( n_mac_is_darkmode() )
	{
		r_norml = 0.33;
		r_press = 0.25;
		r_disbl = 0.20;
	} else {
		r_norml = 0.00;
		r_press = 0.25;
		r_disbl = 0.10;
	}

	u32 color_bg = 0;

	if ( n_posix_false == n_mac_button_is_enabled( p ) )
	{
		color_bg = n_bmp_blend_pixel( color_f, color_t, r_disbl );
	} else
	if ( p->press_onoff )
	{
		color_bg = n_bmp_blend_pixel( color_f, color_t, r_press );
	} else
	if ( p->fake_onoff )
	{
		color_bg = n_bmp_blend_pixel( color_f, color_t, r_press );
	} else
	if ( p->fade.stop == n_posix_false )
	{
		u32 f = 0;
		u32 t = 0;

		if ( p->fade_type == N_MAC_BUTTON_FADE_TYPE_HOT )
		{
			if ( p->hot_onoff )
			{
//NSLog( @"normal to hot" );
				f = color_c;
				t = n_bmp_blend_pixel( color_f, color_t, r_norml );
			} else {
//NSLog( @"hot to normal" );
				f = n_bmp_blend_pixel( color_f, color_t, r_norml );
				t = color_c;
			}
		} else
		if ( p->fade_type == N_MAC_BUTTON_FADE_TYPE_PRESS )
		{
			if ( p->press_onoff )
			{
				f = n_bmp_blend_pixel( color_f, color_t, r_press );
				t = n_bmp_blend_pixel( color_f, color_t, r_norml );
			} else {
				f = n_bmp_blend_pixel( color_f, color_t, r_press );
				t = n_bmp_blend_pixel( color_f, color_t, r_norml );
			}
		} else
		//if ( p->fade_type == N_MAC_BUTTON_FADE_TYPE_NORMAL )
		{
			f = color_c;
			t = color_c;
		}

		color_bg = n_bmp_blend_pixel( f, t, (double) p->fade.percent * 0.01 );
	} else {
		if ( p->hot_onoff )
		{
			color_bg = n_bmp_blend_pixel( color_f, color_t, r_norml );
		} else {
			color_bg = color_c;
		}
	}

//NSLog( @"%d %d %d", n_bmp_r( color_bg ), n_bmp_g( color_bg ), n_bmp_b( color_bg ) );


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
		n_type_gfx ico_sx = N_BMP_SX( &p->icon );
		n_type_gfx ico_sy = N_BMP_SY( &p->icon );


		n_type_gfx tx = ( sx - ico_sx ) / 2;
		n_type_gfx ty = ( sy - ico_sy ) / 2;

		//n_bmp bmp_icon; n_bmp_zero( &bmp_icon );
		//n_mac_image_rc_load_bmp( @"neko", &bmp_icon );

		if (
			( n_mac_button_is_enabled( p ) )
			&&
			( p->gray_onoff == FALSE )
		)
		{
//NSLog( @"1" );
			if ( p->sink_onoff )
			{
				n_type_gfx o = offset_sink;

				tx += o;
				ty += o;
			}

			n_bmp_specialcopy( &p->icon, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, color_t, color_bg );
		} else
		if ( p->fade.percent != 100 )
		{
//NSLog( @"2" );
			if ( p->sink_onoff )
			{
				n_type_gfx o = offset_sink;

				tx += o;
				ty += o;
			}

			n_bmp_specialcopy( &p->icon, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, color_t, color_bg );
		} else {
			n_bmp grayed; n_bmp_carboncopy( &p->icon, &grayed );

			n_mac_image_bmp_gray( &grayed );

			n_bmp_blendcopy( &grayed, &bmp_canvas, 0,0,ico_sx,ico_sy, tx,ty, 0.2 );

			n_bmp_free_fast( &grayed );
		}

		//n_bmp_free_fast( &bmp_icon );

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

		n_bmp_fade_init( &button.fade, n_bmp_black );

		n_mac_timer_init( self, @selector( n_timer_method_fade        ),  33 );

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


	n_bmp_fade_engine( &p->fade, n_posix_true );

	[self display];

//NSLog( @"%d", p->fade.percent );
	if ( p->fade.percent >= 100 )
	{
		n_fade_queue = FALSE;
	}

}

- (void) n_timer_method_press_n_run
{
//return;

	// [!] : prevent pressNrun

	if ( n_mac_window_is_hovered( self ) ) { return; }


	n_mac_button *p = &button;


	n_posix_bool redraw = n_posix_false;


	if ( p->hide_onoff ) { return; }

	if ( p->gray_onoff ) { return; }

	if ( n_posix_false == n_mac_button_is_enabled( p ) )
	{
		redraw = n_posix_true;

		p->hot_onoff = FALSE;
	} else
	if ( p->hot_onoff )
	{
		redraw = n_posix_true;

		p->hot_onoff = FALSE;

		p->fade_type = N_MAC_BUTTON_FADE_TYPE_HOT;
		n_mac_button_fade_go( p );

		[self n_fade_go];
	} else
	if ( p->press_onoff )
	{
		redraw = n_posix_true;

		p->press_onoff = p->prev_onoff;

		p->fade_type = N_MAC_BUTTON_FADE_TYPE_NORMAL;
		n_mac_button_fade_go( p );

		[self n_fade_go];
	}

	if ( p->sink_onoff )
	{
		redraw = n_posix_true;

		p->sink_onoff = FALSE;
	}


	if ( redraw )
	{
		[self display];
	}

}




- (void) n_nswindow_set:(NSWindow*)nswindow
{

	n_mac_button *p = &button;

	p->nswindow = nswindow;

}

- (void) n_icon_set:(n_bmp*) icon;
{

	n_mac_button *p = &button;

	p->icon = (*icon);

}

- (void) n_icon_free
{

	n_mac_button *p = &button;

	n_bmp_free( &p->icon );

}

- (n_posix_bool) n_is_enabled
{

	n_mac_button *p = &button;

	return p->active_onoff;

}

- (void) n_enable:(n_posix_bool)onoff
{

	n_mac_button *p = &button;

	p->active_onoff = onoff;

}

- (n_posix_bool) n_is_grayed
{

	n_mac_button *p = &button;

	return p->gray_onoff;
}

- (void) n_gray:(n_posix_bool)onoff
{

	n_mac_button *p = &button;

	p->gray_onoff = onoff;

}

- (n_posix_bool) n_is_pressed
{

	n_mac_button *p = &button;

	return p->press_onoff;
}

- (n_posix_bool) n_is_pressed_right
{

	n_mac_button *p = &button;

	return p->right_onoff;
}

- (void) n_press:(n_posix_bool)onoff
{

	// [!] : important : on/off switch : use below "n_fake" to make pressed effect

	n_mac_button *p = &button;

	p->prev_onoff = p->press_onoff = onoff;

}

- (void) n_fake:(n_posix_bool)onoff
{

	n_mac_button *p = &button;

	p->fake_onoff = onoff;

}

- (void) n_border:(n_posix_bool)onoff
{

	n_mac_button *p = &button;

	p->show_border = onoff;

}

- (void) n_direct_click:(n_posix_bool)onoff
{

	n_mac_button *p = &button;

	p->direct_click_onoff = onoff;

}

- (void) menu_click_as_normal_click:(n_posix_bool)onoff
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


	if ( p->hide_onoff ) { return; }


	// [x] : Sonoma Xcode 15 : rect has window size at the first run

	n_mac_button_draw( p, [super bounds] );

}




- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");
//return;

	n_mac_button *p = &button;


	if ( p->hide_onoff ) { return; }

	if ( n_posix_false == n_mac_button_is_enabled( p ) ) { return; }


	p-> prev_onoff = p->press_onoff;
	p->  hot_onoff = n_posix_false;
	p-> sink_onoff = n_posix_true;
	p->press_onoff = n_posix_true;

	p->fade_type = 	N_MAC_BUTTON_FADE_TYPE_PRESS;
	n_mac_button_fade_go( p );

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


	if ( p->hide_onoff ) { return; }

	if ( n_posix_false == n_mac_button_is_enabled( p ) ) { return; }


	if ( p->press_onoff )
	{
//NSLog(@"mouseUp : press_onoff" );

		p-> hot_onoff = n_posix_true;
		p->sink_onoff = n_posix_false;

		p->fade_type = 	N_MAC_BUTTON_FADE_TYPE_PRESS;
		n_mac_button_fade_go( p );

		[self n_fade_go];

		[self display];

		if ( n_mac_window_is_hovered( self ) )
		{
//NSLog(@"mouseUp : press_onoff : n_mac_window_is_hovered() : %x", self );
			if ( delegate ) { [delegate mouseUp:theEvent]; }
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




-(void)mouseEntered:(NSEvent *)theEvent {
//NSLog(@"mouseEntered");

	n_mac_button *p = &button;

	if ( p->hide_onoff ) { return; }

	if ( p->gray_onoff ) { return; }

	if ( n_posix_false == n_mac_button_is_enabled( p ) )
	{
		p->hot_onoff = FALSE;
		[self display];
	} else
	if ( p->hot_onoff )
	{
		//
	} else
	if (
		( p->nswindow == NULL )
		||
		( n_mac_window_is_keywindow( p->nswindow ) )
	)
	{
		p->hot_onoff = TRUE;

		p->fade_type = 	N_MAC_BUTTON_FADE_TYPE_HOT;
		n_mac_button_fade_go( p );

		[self n_fade_go];
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

	[self n_gray:NO];
	[self display];

}

- (void) applicationWillResignActive:(NSNotification *)notification
{
//NSLog( @"applicationWillResignActive" );

	n_mac_button *p = &button;


	[self n_gray:YES];

	if ( p->hot_onoff )
	{
		p->hot_onoff = FALSE;

		p->fade_type = 	N_MAC_BUTTON_FADE_TYPE_HOT;
		n_mac_button_fade_go( p );

		[self n_fade_go];
	}

	[self display];

}




@end




#endif // _H_NONNON_MAC_NONNON_BUTTON


