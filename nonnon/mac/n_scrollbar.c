// Nonnon Win
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : how to revent pressNrun
//
//	call when mouseUp in window
// 	mouseUp is never sent when dragging cursor is exited




#ifndef _H_NONNON_WIN32_MAC_SCROLLBAR
#define _H_NONNON_WIN32_MAC_SCROLLBAR




#include "../neutral/bmp/all.c"
#include "../neutral/bmp/fade.c"


#include "./draw.c"
#include "./image.c"
#include "./window.c"




@protocol NonnonScrollbar_delegate

- (void) NonnonScrollbarMessage:(int)n_id value:(int)v reason:(int)reason;

@end


@interface NonnonScrollbar : NSView

@property (nonatomic,assign) id delegate;

@end




#define  N_WIN_SCROLLBAR_RECT_NONE ( -1 )




typedef struct {

	n_type_real x,y,sx,sy;

} n_win_scrollbar_rect;




#define n_win_scrollbar_rect_reset( p ) n_win_scrollbar_rect_set( p, N_WIN_SCROLLBAR_RECT_NONE,N_WIN_SCROLLBAR_RECT_NONE,N_WIN_SCROLLBAR_RECT_NONE,N_WIN_SCROLLBAR_RECT_NONE )

void
n_win_scrollbar_rect_get( n_win_scrollbar_rect *p, n_type_real *x, n_type_real *y, n_type_real *sx, n_type_real *sy )
{

	if (  x != NULL ) { (* x) = p-> x; }
	if (  y != NULL ) { (* y) = p-> y; }
	if ( sx != NULL ) { (*sx) = p->sx; }
	if ( sy != NULL ) { (*sy) = p->sy; }


	return;
}

void
n_win_scrollbar_rect_set( n_win_scrollbar_rect *p, n_type_real x, n_type_real y, n_type_real sx, n_type_real sy )
{

	p->x  = x;
	p->y  = y;
	p->sx = sx;
	p->sy = sy;


	return;
}

BOOL
n_win_scrollbar_rect_is_hovered( n_win_scrollbar_rect *p, NSView *view )
{
	NSRect rect = { p->x, p->y, p->sx, p->sy };
	return n_mac_window_is_hovered_offset_by_rect( view, rect );
}




#define N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL      ( 0 )
#define N_WIN_SCROLLBAR_LAYOUT_VERTICAL        ( 1 )

#define N_WIN_SCROLLBAR_OPTION_NONE            ( 0 <<  0 )
#define N_WIN_SCROLLBAR_OPTION_NO_ARROWS       ( 1 <<  0 )
#define N_WIN_SCROLLBAR_OPTION_PAGER           ( 1 <<  1 )
#define N_WIN_SCROLLBAR_OPTION_RECALC_UNIT_MAX ( 1 <<  2 )
#define N_WIN_SCROLLBAR_OPTION_CLAMP_BIG_THUMB ( 1 <<  3 )
#define N_WIN_SCROLLBAR_OPTION_SHAFT_GRADIENT  ( 1 <<  4 )
//#define N_WIN_SCROLLBAR_OPTION_NO_MAX_CLAMP    ( 1 <<  5 )

#define N_WIN_SCROLLBAR_SCROLL_NONE            ( 0 )
#define N_WIN_SCROLLBAR_SCROLL_SEND            ( 1 )
#define N_WIN_SCROLLBAR_SCROLL_AUTO            ( 2 )

#define N_MAC_SCROLLBAR_STDSIZE                ( 18 )

#define N_WIN_SCROLLBAR_REASON_REDRAW          ( 0 )
#define N_WIN_SCROLLBAR_REASON_ARROW_MOUSEDOWN ( 1 )
#define N_WIN_SCROLLBAR_REASON_SHAFT_MOUSEDOWN ( 2 )
#define N_WIN_SCROLLBAR_REASON_DRAG_MOUSEUP    ( 3 )
#define N_WIN_SCROLLBAR_REASON_DRAG_DRAGGED    ( 4 )
#define N_WIN_SCROLLBAR_REASON_WHEEL           ( 5 )




typedef struct {

	NonnonScrollbar     *view;
	int                  id;

	NSWindow            *nswindow;

	int                  layout;
	int                  option;
	int                  stripe_type;
	n_type_gfx           round_param;

	u32                  color_trans;
	u32                  color__back;
	u32                  color_thumb;
	u32                  color_bordr;
	u32                  color_shaft;
	u32                  color_press;
	u32                  color_hvr_t;
	u32                  color_hvr_s;
	u32                  color_arrow;
	u32                  color_hover;
	u32                  color_cornr;
	u32                  color_grad1;
	u32                  color_grad2;

	n_bmp_fade           fade_th;
	n_bmp_fade           fade_ul;
	n_bmp_fade           fade_dr;

	n_type_real          unit_pos , pixel_pos;
	n_type_real          unit_max , pixel_max;
	n_type_real          unit_page, pixel_page;
	n_type_real          unit_step, pixel_step;
	n_type_real          unit_max_original;
	n_type_real          pixel_thumb_original;
	n_type_real          pixel_thumb;
	n_type_real          pixel_minim;
	n_type_real          pixel_arrow_sx, pixel_arrow_sy;
	n_type_real          pixel_shaft;
	n_type_real          pixel_offset;
	n_type_real          pixel_offset_prv;
	n_type_real          pixel_patch;
	n_type_real          prev_pos;

	n_win_scrollbar_rect rect_shaft;
	n_win_scrollbar_rect rect_thumb;
	n_win_scrollbar_rect rect_arrow_ul;
	n_win_scrollbar_rect rect_arrow_dr;

	BOOL                 enabled_global;
	BOOL                 enabled_ul;
	BOOL                 enabled_dr;

	n_type_gfx           sx,sy;

	n_type_gfx           prv_cursor_x;
	n_type_gfx           prv_cursor_y;
	n_type_gfx           cur_cursor_x;
	n_type_gfx           cur_cursor_y;

	BOOL                 pressed_thumb;
	BOOL                 pressed_shaft;
	BOOL                 pressed_arrow_ul;
	BOOL                 pressed_arrow_dr;

	BOOL                 hovered_thumb;
	BOOL                 hovered_shaft;
	BOOL                 hovered_arrow_ul;
	BOOL                 hovered_arrow_dr;

	n_bmp                bmp_sh;
	n_bmp                bmp_ul;
	n_bmp                bmp_dr;
	n_bmp                bmp_th;
	n_bmp                bmp_gr;

	BOOL                 drag_onoff;
	BOOL                 drag_clamp;

	BOOL                 gray_onoff;

} n_win_scrollbar;




BOOL
n_win_scrollbar_is_thumb_minimal( n_win_scrollbar *p )
{

	BOOL ret = FALSE;

	if ( p->pixel_thumb_original < p->pixel_thumb )
	{
		ret = TRUE;
	}

	return ret;
}




void
n_win_scrollbar_ui_arrowbutton( n_win_scrollbar *p )
{

	// Arrows

	if ( p->option & N_WIN_SCROLLBAR_OPTION_NO_ARROWS )
	{

		//

	} else {
//NSLog( @"pixel_arrow : %f %f", p->pixel_arrow_sx, p->pixel_arrow_sy );

		n_type_gfx ctl = N_MAC_SCROLLBAR_STDSIZE;
		n_type_gfx csy = [p->view frame].size.height;

		NSFont *nsfont = [NSFont userFixedPitchFontOfSize:11];

		n_posix_char *str;


		// Arrow : Up / Left

		//if ( p->style == N_WIN_SCROLLBAR_STYLE_FLUENT_UI )
		{
			str = n_posix_literal( "◀" );
		}

		//if ( NULL == N_BMP_PTR( &p->bmp_ul ) )
		{
//NSLog( @"pressed_arrow_ul : %d", p->pressed_arrow_ul );

			u32 color;
			if ( p->pressed_arrow_ul )
			{
				color = p->color_press;
			} else {
				color = p->color__back;

				n_type_real d = (n_type_real) p->fade_ul.percent * 0.01;
				if ( p->fade_ul.color_fg == n_bmp_white )
				{
					//
				} else {
					d = 1.0 - d;
				}
				color = n_bmp_blend_pixel( color, p->color_hover, d );
			}

			n_bmp_new_fast( &p->bmp_ul, ctl, csy );
			n_bmp_flush( &p->bmp_ul, color );

			u32 color_arrow = p->color_arrow;
			if (
				( p->enabled_global == FALSE )
				||
				( p->enabled_ul     == FALSE )
			)
			{
				color_arrow = n_bmp_blend_pixel( p->color_arrow, p->color_shaft, 0.9 );
			}

			if ( p->gray_onoff )
			{
				color_arrow = n_bmp_blend_pixel( color_arrow, p->color_shaft, 0.9 );
			}

			NSImage *img = n_mac_image_nbmp2nsimage( &p->bmp_ul );
			n_mac_image_text_native
			(
				img,
				n_mac_str2nsstring( str ),
				nsfont,
				n_mac_argb2nscolor( color_arrow ),
				0, 1
			);

			NSRect rect = NSMakeRect( 0,0,ctl,csy );
			[img drawInRect:rect];

		}


		// Arrow : Down / Right

		//if ( p->style == N_WIN_SCROLLBAR_STYLE_FLUENT_UI )
		{
			str = n_posix_literal( "▶" );
		}

		//if ( NULL == N_BMP_PTR( &p->bmp_dr ) )
		{
			u32 color;
			if ( p->pressed_arrow_dr )
			{
				color = p->color_press;
			} else {
				color = p->color__back;

				n_type_real d = (n_type_real) p->fade_dr.percent * 0.01;
				if ( p->fade_dr.color_fg == n_bmp_white )
				{
					//
				} else {
					d = 1.0 - d;
				}
				color = n_bmp_blend_pixel( color, p->color_hover, d );
			}

			n_bmp_new_fast( &p->bmp_dr, ctl, csy );
			n_bmp_flush( &p->bmp_dr, color );

			u32 color_arrow = p->color_arrow;
			if (
				( p->enabled_global == FALSE )
				||
				( p->enabled_dr     == FALSE )
			)
			{
				color_arrow = n_bmp_blend_pixel( p->color_arrow, p->color_shaft, 0.9 );
			}

			if ( p->gray_onoff )
			{
				color_arrow = n_bmp_blend_pixel( color_arrow, p->color_shaft, 0.9 );
			}

			NSImage *img = n_mac_image_nbmp2nsimage( &p->bmp_dr );
			n_mac_image_text_native
			(
				img,
				n_mac_str2nsstring( str ),
				nsfont,
				n_mac_argb2nscolor( color_arrow ),
				0, 1
			);

			NSRect rect = NSMakeRect( p->sx-ctl,0,ctl,csy );
			[img drawInRect:rect];

		}

	}

	return;
}

void
n_win_scrollbar_ui( n_win_scrollbar *p )
{
//NSLog( @"n_win_scrollbar_ui()" );


	// Shaft

	u32 color_shaft = p->color_trans;

	//if ( NULL == N_BMP_PTR( &p->bmp_sh ) )
	{
		n_type_gfx  x = 0;
		n_type_gfx  y = 0;
		n_type_gfx sx = p->sx;
		n_type_gfx sy = p->sy;

		n_bmp_new_fast( &p->bmp_sh, sx,sy );

		if ( p->option & N_WIN_SCROLLBAR_OPTION_SHAFT_GRADIENT )
		{
			// [x] : mach band is not beautiful without arrangement
			if ( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( p->color_grad1 ) )
			{
				color_shaft = 0;
				n_bmp_flush_gradient( &p->bmp_sh, p->color_grad1, p->color_grad2, N_BMP_GRADIENT_HORIZONTAL );
			} else {
				n_bmp_flush_gradient( &p->bmp_sh, p->color_grad2, p->color_grad1, N_BMP_GRADIENT_HORIZONTAL );
				n_bmp_flush_mirror( &p->bmp_sh, N_BMP_COPY_MIRROR_LEFTSIDE_RIGHT );
			}
		} else {
			n_bmp_box( &p->bmp_sh, x,y,sx,sy, p->color_shaft );
		}
	}


	// Thumb

	//if ( NULL == N_BMP_PTR( &p->bmp_th ) )
	{
		n_type_gfx gsx = (n_type_gfx) p->pixel_thumb;
		n_type_gfx gsy = (n_type_gfx) p->pixel_minim;

		n_bmp_new_fast( &p->bmp_th, gsx, gsy );
		n_bmp_flush( &p->bmp_th, color_shaft );

		u32 color;
		if ( p->pressed_thumb )
		{
			color = p->color_press;
		} else {
			color = p->color_thumb;

			n_type_real d = (n_type_real) p->fade_th.percent * 0.01;
			if ( p->fade_th.color_fg == n_bmp_white )
			{
				//
			} else {
				d = 1.0 - d;
			}
			color = n_bmp_blend_pixel( color, p->color_hover, d );
		}

		n_type_gfx border = 1;

		n_type_gfx  x = 0;
		n_type_gfx  y = 0;
		n_type_gfx sx = gsx;
		n_type_gfx sy = gsy;

		u32 color_border = n_bmp_argb_mac( 128, 128,128,128 );
		//if ( n_mac_is_darkmode() )

		n_bmp_roundrect_pixel( &p->bmp_th, x,y,sx,sy, color_border, p->round_param );

		 x += border;
		 y += border;
		sx -= border * 2;
		sy -= border * 2;

		n_bmp_roundrect_pixel( &p->bmp_th, x,y,sx,sy, color, p->round_param );
	}


	return;
}




#define n_win_scrollbar_zero( p ) n_memory_zero( p, sizeof( n_win_scrollbar ) )

void
n_win_scrollbar_refresh( n_win_scrollbar *p )
{

	[p->view display];


	return;
}

n_type_real
n_win_scrollbar_position_unit2pixel( n_win_scrollbar *p )
{
	return ( p->unit_pos / p->unit_max ) * p->pixel_max;
}

n_type_real
n_win_scrollbar_position_pixel2unit( n_win_scrollbar *p )
{
	return ( p->pixel_pos / p->pixel_max ) * p->unit_max;
}

n_type_real
n_win_scrollbar_position_last_unit( n_win_scrollbar *p )
{
	return p->unit_max - p->unit_page;
}

n_type_real
n_win_scrollbar_position_last_pixel( n_win_scrollbar *p )
{
	return p->pixel_max - p->pixel_page;
}

void
n_win_scrollbar_position_cursor( n_win_scrollbar *p, n_type_gfx *x, n_type_gfx *y )
{

	NSPoint pt = n_mac_cursor_position_get( p->view );

	if ( x != NULL ) { (*x) = pt.x; }
	if ( y != NULL ) { (*y) = pt.y; }

//NSLog( @"%0.2f %0.2f", pt.x, pt.y );
//NSLog( @"%d %d", *x, *y );


	return;
}

void
n_win_scrollbar_position_thumb( n_win_scrollbar *p, n_type_real *x, n_type_real *y )
{

	n_type_real dx = 0;
	n_type_real dy = 0;


	// [!] : thumb position

	if ( p->drag_onoff )
	{

		// [Needed] : prv only

		//n_win_scrollbar_position_cursor( p, &p->cur_cursor_x, &p->cur_cursor_y );

		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			dx = p->prv_cursor_x - p->pixel_arrow_sx - p->pixel_offset;
		} else {
			dy = p->prv_cursor_y - p->pixel_arrow_sy - p->pixel_offset;
		}

	} else {

		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			dx = p->pixel_pos;
		} else {
			dy = p->pixel_pos;
		}

	}


	// [!] : clamper

	n_type_real d = 0;
	if ( p->option & N_WIN_SCROLLBAR_OPTION_RECALC_UNIT_MAX ) { d = p->pixel_patch; }

	n_type_real pixel_last = n_win_scrollbar_position_last_pixel( p ) - d;

	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		dx = n_posix_minmax_n_type_real( 0, pixel_last, dx );
	} else {
		dy = n_posix_minmax_n_type_real( 0, pixel_last, dy );
	}


	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		p->pixel_pos = dx;
	} else {
		p->pixel_pos = dy;
	}


	// [!] : write back

	if ( x != NULL ) { (*x) = dx; }
	if ( y != NULL ) { (*y) = dy; }


	return;
}

void
n_win_scrollbar_arrowbutton_onoff( n_win_scrollbar *p, n_type_real f, n_type_real t, n_type_real pos )
{

	{

		//BOOL p_enabled_ul = p->enabled_ul;
		//BOOL p_enabled_dr = p->enabled_dr;

		n_type_real delta_f = fabs( pos - f );
		n_type_real delta_t = fabs( pos - t );

		// [!] : delta will be non-zero value

		if ( delta_f < 0.0000000001 ) { p->enabled_ul = FALSE; } else { p->enabled_ul = TRUE; }
		if ( delta_t < 0.0000000001 ) { p->enabled_dr = FALSE; } else { p->enabled_dr = TRUE; }

		if ( delta_f < p->pixel_step )
		{
			p->enabled_ul = FALSE;

			if ( p->option & N_WIN_SCROLLBAR_OPTION_CLAMP_BIG_THUMB )
			{
				p->unit_pos = 0;
			}
		}

		//if ( p_enabled_ul != p->enabled_ul ) { n_bmp_free( &p->bmp_ul ); }
		//if ( p_enabled_dr != p->enabled_dr ) { n_bmp_free( &p->bmp_dr ); }

	}


	return;
}

void
n_win_scrollbar_on_settingchange( n_win_scrollbar *p, BOOL auto_style, BOOL redraw )
{

	if ( n_mac_is_darkmode() )
	{
		p->color_trans = n_bmp_black_invisible;
		p->color__back = n_bmp_rgb_mac(  66, 66, 66 );

		p->color_shaft = n_bmp_blend_pixel( n_bmp_white, n_bmp_black, 0.75 );

		p->color_arrow = n_bmp_blend_pixel( n_bmp_white, n_bmp_black, 0.25 );
		p->color_thumb = n_bmp_blend_pixel( p->color_trans, n_bmp_white, 0.75 );
		p->color_press = n_bmp_blend_pixel( p->color_thumb, n_bmp_black, 0.50 );

		p->color_hover = n_bmp_rgb_mac( 111,111,111 );
		p->color_cornr = n_bmp_rgb_mac(  66, 66, 66 );
	} else {
		p->color_trans = n_bmp_white_invisible;
		p->color__back = n_bmp_color_mac( n_mac_nscolor2argb( [NSColor windowBackgroundColor] ) );

		p->color_shaft = n_bmp_blend_pixel( n_bmp_white, n_bmp_black, 0.25 );

		p->color_arrow = n_bmp_black + 1;
		p->color_thumb = p->color__back;
		p->color_press = n_bmp_blend_pixel( p->color_thumb, n_bmp_black, 0.50 );

		p->color_hover = n_bmp_blend_pixel( p->color_thumb, n_bmp_white, 0.50 );
		p->color_cornr = n_mac_nscolor2argb( [NSColor windowBackgroundColor] );
	}


	// [Needed] : for initial arrow button size

	extern void n_win_scrollbar_metrics( n_win_scrollbar* );
	n_win_scrollbar_metrics( p );


	if ( redraw ) { n_win_scrollbar_refresh( p ); }


	return;
}

void
n_win_scrollbar_init( n_win_scrollbar *p, NonnonScrollbar *view )
{

	n_win_scrollbar_zero( p );


	p->view   = view;
	p->layout = N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL;
	p->option = N_WIN_SCROLLBAR_OPTION_NONE;

	p->round_param = 5;


	// [x] : memory leak

	n_win_scrollbar_on_settingchange( p, 0, FALSE );


	p->drag_clamp = TRUE;


	n_bmp_fade_init( &p->fade_th, n_bmp_black );
	n_bmp_fade_init( &p->fade_ul, n_bmp_black );
	n_bmp_fade_init( &p->fade_dr, n_bmp_black );


	p->enabled_global = TRUE;


	return;
}

void
n_win_scrollbar_exit( n_win_scrollbar *p )
{

	n_bmp_free_fast( &p->bmp_sh );
	n_bmp_free_fast( &p->bmp_ul );
	n_bmp_free_fast( &p->bmp_dr );
	n_bmp_free_fast( &p->bmp_th );
	n_bmp_free_fast( &p->bmp_gr );


	n_win_scrollbar_zero( p );


	return;
}

void
n_win_scrollbar_metrics( n_win_scrollbar *p )
{
//return;

	p->sx = [p->view frame].size.width;
	p->sy = [p->view frame].size.height;


	p->sx = p->sx;
	p->sy = N_MAC_SCROLLBAR_STDSIZE;

	if ( p->option & N_WIN_SCROLLBAR_OPTION_NO_ARROWS )
	{

		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			p->pixel_arrow_sx = p->pixel_arrow_sy = 0;
			p->pixel_minim = p->sy;
		} else {
			p->pixel_arrow_sx = p->pixel_arrow_sy = 0;
			p->pixel_minim = p->sx;
		}

	} else {

		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			p->pixel_minim    = p->sy;
			p->pixel_arrow_sx = (n_type_real) p->sy;
			p->pixel_arrow_sy = p->sy;
		} else {
			p->pixel_minim    = p->sx;
			p->pixel_arrow_sx = p->sx;
			p->pixel_arrow_sy = (n_type_real) p->sx;
		}

	}


	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		p->pixel_shaft = p->sx - ( p->pixel_arrow_sx * 2 );
	} else {
		p->pixel_shaft = p->sy - ( p->pixel_arrow_sy * 2 );
	}

	p->pixel_shaft = n_posix_max_n_type_real( p->pixel_minim, p->pixel_shaft );


	// [!] : the second mode

	p->unit_max   = p->unit_max_original;

	p->pixel_max  = n_posix_max_n_type_real( 1.0, p->pixel_shaft );
	p->pixel_step = ( p->unit_step / p->unit_max ) * p->pixel_max;
	p->pixel_page = ( p->unit_page / p->unit_max ) * p->pixel_max;
	p->pixel_pos  = ( p->unit_pos  / p->unit_max ) * p->pixel_max;
//n_win_scrollbar_hwndprintf_literal( p, " %g %g %g ", p->pixel_max, p->pixel_step, p->pixel_page );

	// [!] : minimum thumb size

	p->pixel_thumb_original = p->pixel_page;
	p->pixel_thumb          = n_posix_max_n_type_real( p->pixel_page, p->pixel_minim );

	// [Needed] : rounding is needed

	p->pixel_thumb_original = ceil( p->pixel_thumb_original );
	p->pixel_thumb          = ceil( p->pixel_thumb          );

//if ( p->layout == 1 ) { n_win_scrollbar_hwndprintf_literal( p, " %g %g ", p->pixel_thumb_original, p->pixel_thumb ); }


	// [!] : the third mode

	if ( p->pixel_thumb_original < p->pixel_thumb )
	{
//n_win_scrollbar_debug_count( p );

//n_win_scrollbar_hwndprintf_literal( p, " %g %g ", (n_type_real) p->sy, (n_type_real) p->pixel_max );

		p->pixel_patch = ( p->pixel_max - p->pixel_page + p->pixel_thumb ) - p->pixel_max;

//n_win_scrollbar_hwndprintf_literal( p, " %g ", p->pixel_patch );

		if ( p->option & N_WIN_SCROLLBAR_OPTION_RECALC_UNIT_MAX )
		{
			n_type_real d = ( p->pixel_patch / ( p->pixel_max - p->pixel_patch ) ) * p->unit_max_original;
//n_win_scrollbar_hwndprintf_literal( p, " %g ", d );

//n_win_scrollbar_hwndprintf_literal( p, " %g %g %g ", p->unit_max_original, p->unit_step, p->unit_page );

			p->unit_max   = p->unit_max_original + d;
			p->pixel_step = ( p->unit_step / p->unit_max ) * p->pixel_max;
			p->pixel_page = ( p->unit_page / p->unit_max ) * p->pixel_max;
			p->pixel_pos  = ( p->unit_pos  / p->unit_max ) * p->pixel_max;

//n_win_scrollbar_hwndprintf_literal( p, " %g ", p->unit_max );
		} else {
			p->pixel_max  = n_posix_max_n_type_real( 1.0, p->pixel_max - p->pixel_patch );
			p->pixel_step = ( p->unit_step / p->unit_max ) * p->pixel_max;
			p->pixel_page = ( p->unit_page / p->unit_max ) * p->pixel_max;
			p->pixel_pos  = ( p->unit_pos  / p->unit_max ) * p->pixel_max;
		}

	} else {

		p->pixel_patch = 0;

	}


	return;
}

static BOOL n_win_scrollbar_parameter_add_page = TRUE;

void
n_win_scrollbar_parameter( n_win_scrollbar *p, n_type_real step, n_type_real page, n_type_real max, n_type_real pos, BOOL redraw )
{

	// [!] : the first mode

	if ( n_win_scrollbar_parameter_add_page ) { max += page; }

	p->unit_step = step;
	p->unit_page = page;
	p->unit_max  = n_posix_max_n_type_real( 1.0, max ) ;
	p->unit_pos  = pos;
//if ( p->layout == 1 ) { n_win_scrollbar_hwndprintf_literal( p, " parameter() :  %g %g %g ", p->unit_pos, p->unit_page, p->unit_max ); }

	if ( max >= page )
	{
		p->enabled_ul = TRUE;
		p->enabled_dr = TRUE;
	} else {
		p->enabled_ul = FALSE;
		p->enabled_dr = FALSE;
	}

	p->unit_max_original = p->unit_max;


	if ( redraw )
	{
		n_win_scrollbar_refresh( p );
	}


	return;
}

BOOL
n_win_scrollbar_scroll_pixel( n_win_scrollbar *p, n_type_real pixel_delta, int option, int reason )
{

	//if ( pixel_delta != 0 )
	{

		p->pixel_pos += pixel_delta;


		n_type_real pixel_min = 0;
		n_type_real pixel_max = n_win_scrollbar_position_last_pixel( p );

		n_type_real d = 0;
		if ( p->option & N_WIN_SCROLLBAR_OPTION_RECALC_UNIT_MAX ) { d = p->pixel_patch; }

		p->pixel_pos = n_posix_minmax_n_type_real( pixel_min, pixel_max - d, p->pixel_pos );


		p->prev_pos = p->unit_pos;


		n_type_real unit_min = 0;
		n_type_real unit_max = n_win_scrollbar_position_last_unit( p );

		p->unit_pos = n_win_scrollbar_position_pixel2unit( p );


		// [Needed] : unit_max+1

		p->unit_pos = n_posix_minmax_n_type_real( unit_min, unit_max+1, p->unit_pos );
//NSLog( @"%f %f %f", unit_min, unit_max, p->unit_pos );

	}


	BOOL ret = FALSE;
	if ( option == N_WIN_SCROLLBAR_SCROLL_NONE )
	{
		//
	} else
	if ( option == N_WIN_SCROLLBAR_SCROLL_SEND )
	{
		ret = TRUE;
		[p->view.delegate NonnonScrollbarMessage:p->id value:p->unit_pos reason:reason];
	} else
	if ( option == N_WIN_SCROLLBAR_SCROLL_AUTO )
	{
		[p->view.delegate NonnonScrollbarMessage:p->id value:p->unit_pos reason:reason];
	}


	return ret;
}

BOOL
n_win_scrollbar_scroll_unit( n_win_scrollbar *p, n_type_real unit_delta, int option, int reason )
{
//n_win_scrollbar_debug_count( p );


	if ( unit_delta != 0 )
	{

		p->prev_pos = p->unit_pos;


		p->unit_pos += unit_delta;


		n_type_real unit_min = 0;
		n_type_real unit_max = n_win_scrollbar_position_last_unit( p );

		p->unit_pos = n_posix_minmax_n_type_real( unit_min, unit_max, p->unit_pos );


		n_type_real pixel_min = 0;
		n_type_real pixel_max = n_win_scrollbar_position_last_pixel( p );

		n_type_real d = 0;
		if ( p->option & N_WIN_SCROLLBAR_OPTION_RECALC_UNIT_MAX ) { d = p->pixel_patch; }

		p->pixel_pos = n_win_scrollbar_position_unit2pixel( p );
		p->pixel_pos = n_posix_minmax_n_type_real( pixel_min, pixel_max - d, p->pixel_pos );

	}


	BOOL ret = FALSE;
	if ( option == N_WIN_SCROLLBAR_SCROLL_NONE )
	{
		//
	} else
	if ( option == N_WIN_SCROLLBAR_SCROLL_SEND )
	{
		ret = TRUE;
		[p->view.delegate NonnonScrollbarMessage:p->id value:p->unit_pos reason:reason];
	} else
	if ( option == N_WIN_SCROLLBAR_SCROLL_AUTO )
	{
		[p->view.delegate NonnonScrollbarMessage:p->id value:p->unit_pos reason:reason];
	}


	return ret;
}




void
n_win_scrollbar_draw( n_win_scrollbar *p )
{
//NSLog( @"n_win_scrollbar_draw() : %0.0f", p->unit_pos );


	// UI Maker

	n_type_gfx ctl = N_MAC_SCROLLBAR_STDSIZE;

	n_win_scrollbar_metrics( p );

	n_win_scrollbar_arrowbutton_onoff( p, 0, n_win_scrollbar_position_last_pixel( p ), p->pixel_pos );

	n_bmp canvas; n_bmp_zero( &canvas ); n_bmp_new_fast( &canvas, p->sx, p->sy );

	n_bmp_fade_engine( &p->fade_th, TRUE );
	n_bmp_fade_engine( &p->fade_ul, TRUE );
	n_bmp_fade_engine( &p->fade_dr, TRUE );

	n_win_scrollbar_ui( p );


	// Redraw Area : not used in Mac

	n_type_gfx rx  = 0;//p->redraw_x;
	n_type_gfx ry  = 0;//p->redraw_y;
	n_type_gfx rsx = p->sx;//p->redraw_sx;
	n_type_gfx rsy = p->sy;//p->redraw_sy;


	// Shaft

	{
		n_bmp_fastcopy( &p->bmp_sh, &canvas, rx,ry,rsx,rsy, rx,ry );

		// [!] : for collision/hittest
		n_win_scrollbar_rect_set( &p->rect_shaft, rx,ry,rsx,rsy );
	}


	// Arrows

	if ( p->option & N_WIN_SCROLLBAR_OPTION_NO_ARROWS )
	{

		n_type_real d = N_WIN_SCROLLBAR_RECT_NONE;

		n_win_scrollbar_rect_set( &p->rect_arrow_ul, d,d,d,d );
		n_win_scrollbar_rect_set( &p->rect_arrow_dr, d,d,d,d );

	} else {

		// [!] : Top / Left

		//if ( redraw_ul )
		{
			//u32 color = n_bmp_rgb_mac( 0,255,0 );

			//n_bmp_new_fast( &p->bmp_ul, ctl, ctl );
			//n_bmp_flush   ( &p->bmp_ul, color );
			//n_bmp_fastcopy( &p->bmp_ul, &canvas, 0,0,ctl,ctl, 0,0 );

			n_win_scrollbar_rect_set( &p->rect_arrow_ul, 0,0,ctl,ctl );
		}

		// [!] : Bottom / Right

		//if ( redraw_dr )
		{
			n_type_gfx ox = p->sx - ctl;

			//u32 color = n_bmp_rgb_mac( 0,200,255 );

			//n_bmp_new_fast( &p->bmp_dr, ctl, ctl );
			//n_bmp_flush   ( &p->bmp_dr, color );
			//n_bmp_fastcopy( &p->bmp_dr, &canvas, 0,0,ctl,ctl, ox,0 );

			n_win_scrollbar_rect_set( &p->rect_arrow_dr, ox,0,ctl,ctl );
		}

	}


	// Thumb

	if ( p->enabled_global )
	{
		u32 color;
		if ( p->pressed_thumb )
		{
			color = p->color_press;
		} else {
			color = p->color_thumb;
		}

		n_type_gfx  x = (n_type_gfx) p->pixel_pos + N_MAC_SCROLLBAR_STDSIZE;
		n_type_gfx  y = 0;
		n_type_gfx sx = N_BMP_SX( &p->bmp_th );
		n_type_gfx sy = N_BMP_SY( &p->bmp_th );
//NSLog( @"%d %d %d %d", x,y,sx,sy );

		//n_bmp_rasterizer( &p->bmp_th, &canvas, x,y, color, FALSE );
		n_bmp_transcopy( &p->bmp_th, &canvas, 0,0,sx,sy, x,y );

		n_win_scrollbar_rect_set( &p->rect_thumb, x,y,sx,sy );
	}



	// Composition

	if ( p->enabled_global == FALSE )
	{
		n_bmp_flush_grayscale( &canvas );
		n_bmp_flush_mixer( &canvas, p->color_shaft, 0.5 );
	}

	n_bmp_cornermask( &canvas, p->round_param, 0, p->color_cornr );

//n_bmp_flush( &canvas, n_bmp_rgb_mac( 0,200,255 ) );

	NSRect frame = [p->view frame];

	//n_type_gfx csx = NSWidth ( frame );
	n_type_gfx csy = NSHeight( frame );

	n_type_gfx oy = ( csy - ctl ) / 2;

	p->rect_arrow_ul.y = oy;
	p->rect_arrow_dr.y = oy;
	p->rect_thumb   .y = oy;
	p->rect_shaft   .y = oy;

	if ( p->gray_onoff )
	{
		n_mac_image_bmp_gray( &canvas );
	}

	NSRect rect = NSMakeRect( 0,oy,p->sx,p->sy );
	n_mac_image_nbmp_direct_draw( &canvas, &rect, FALSE );


	n_bmp_free_fast( &canvas );


	// [!] : Arrow Button : for HiDPI

	n_win_scrollbar_ui_arrowbutton( p );


	// [!] : Border

	{
		NSColor *color_border;
		if ( n_mac_is_darkmode() )
		{
			color_border = [NSColor grayColor];
		} else {
			color_border = [NSColor lightGrayColor];
		}

		[p->view setWantsLayer: YES];
		[p->view.layer setBorderWidth: 1];
		[p->view.layer setBorderColor:color_border.CGColor];
		[p->view.layer setCornerRadius: p->round_param];
	}


	return;
}




n_type_real
n_win_scrollbar_pager( n_win_scrollbar *p, n_type_gfx cursor_x, n_type_gfx cursor_y )
{

	// [!] : Nonnon Original Scrollpager

	n_type_real pos = 0;

	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		pos = cursor_x - p->pixel_arrow_sx - ( p->pixel_thumb / 2 );
	} else {
		pos = cursor_y - p->pixel_arrow_sy - ( p->pixel_thumb / 2 );
	}


	return pos;
}

void
n_win_scrollbar_event_shaft( n_win_scrollbar *p, int reason )
{

	n_type_gfx cursor_x, cursor_y; n_win_scrollbar_position_cursor( p, &cursor_x, &cursor_y );

	if ( p->option & N_WIN_SCROLLBAR_OPTION_PAGER )
	{

		n_type_real pos = n_win_scrollbar_pager( p, cursor_x, cursor_y );
		p->pixel_pos = 0;

		n_win_scrollbar_scroll_pixel( p, pos, N_WIN_SCROLLBAR_SCROLL_AUTO, reason );

		[p->view display];

	} else {

		n_type_real delta = 0;

		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			if ( cursor_x < p->rect_thumb.x )
			{
				delta = -p->unit_page;
			} else
			if ( cursor_x > p->rect_thumb.x )
			{
				delta =  p->unit_page;
			}
		} else {
			if ( cursor_y < p->rect_thumb.y )
			{
				delta = -p->unit_page;
			} else
			if ( cursor_y > p->rect_thumb.y )
			{
				delta =  p->unit_page;
			}
		}
//NSLog( @" %f ", delta );

		n_win_scrollbar_scroll_unit( p, delta, N_WIN_SCROLLBAR_SCROLL_AUTO, reason );
		[p->view display];

	}


	return;
}

void
n_win_scrollbar_event_arrow( n_win_scrollbar *p, int reason )
{

	n_type_real delta = 0;

	if ( p->pressed_arrow_ul )
	{
		delta = -p->unit_step;
	} else
	if ( p->pressed_arrow_dr )
	{
		delta =  p->unit_step;
	}

	if ( delta != 0 )
	{
		n_win_scrollbar_scroll_unit( p, delta, N_WIN_SCROLLBAR_SCROLL_AUTO, reason );
		[p->view display];
	}


	return;
}




void
n_win_scrollbar_on_input( n_win_scrollbar *p )
{

	p->hovered_thumb    = n_win_scrollbar_rect_is_hovered( &p->rect_thumb   , p->view );
	p->hovered_arrow_ul = n_win_scrollbar_rect_is_hovered( &p->rect_arrow_ul, p->view );
	p->hovered_arrow_dr = n_win_scrollbar_rect_is_hovered( &p->rect_arrow_dr, p->view );
	p->hovered_shaft    = n_win_scrollbar_rect_is_hovered( &p->rect_shaft   , p->view );

	p->hovered_shaft    = p->hovered_shaft && ( p->hovered_thumb    == FALSE );
	p->hovered_shaft    = p->hovered_shaft && ( p->hovered_arrow_ul == FALSE );
	p->hovered_shaft    = p->hovered_shaft && ( p->hovered_arrow_dr == FALSE );


//NSLog( @" Input : %d %d %d %d ", p->hovered_arrow_ul, p->hovered_arrow_dr, p->hovered_thumb, p->hovered_shaft );
	return;
}

void
n_win_scrollbar_offset( n_win_scrollbar *p )
{

	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		p->pixel_offset = p->prv_cursor_x - p->rect_thumb.x;
	} else {
		p->pixel_offset = p->prv_cursor_y - p->rect_thumb.y;
	}


	return;
}

void
n_win_scrollbar_on_drag( n_win_scrollbar *p, int reason )
{
//return;

	n_win_scrollbar_position_cursor( p, &p->cur_cursor_x, &p->cur_cursor_y );


	n_type_real pixel_delta;
//if(pixel_delta){}
	if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
	{
		pixel_delta = p->cur_cursor_x - p->prv_cursor_x;
	} else {
		pixel_delta = p->cur_cursor_y - p->prv_cursor_y;
	}
//n_win_scrollbar_hwndprintf_literal( p, " %f ", pixel_delta );

//n_win_scrollbar_hwndprintf_literal( p, " %g %g ", p->pixel_pos, p->pixel_max - p->pixel_page );


//n_win_scrollbar_hwndprintf_literal( p, " %d %d ", p->cur_cursor_y, (int) p->pixel_arrow_sy );

	BOOL draw_always_onoff = FALSE;

	if ( ( p->drag_clamp )&&( pixel_delta != 0.0 ) )
	{

		n_type_gfx arrow;
		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			arrow = (n_type_gfx) p->pixel_arrow_sx;
		} else {
			arrow = (n_type_gfx) p->pixel_arrow_sy;
		}

		n_type_gfx cursor;
		if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_HORIZONTAL )
		{
			cursor = p->cur_cursor_x - arrow;
		} else {
			cursor = p->cur_cursor_y - arrow;
		}

		n_type_gfx minim = 0;
		n_type_gfx maxim = (n_type_gfx) p->pixel_shaft;

		minim += (n_type_gfx) (                  p->pixel_offset_prv );
		maxim -= (n_type_gfx) ( p->pixel_thumb - p->pixel_offset_prv );

		if ( cursor <= minim )
		{
//n_win_scrollbar_hwndprintf_literal( p, " minim " );
//n_win_scrollbar_debug_count();

			// [Needed] : set -1 for "pixel_delta"
			//
			//	when released at outside of thumb

			p->pixel_offset = p->pixel_offset_prv;
			p->pixel_pos    = 0;
			   pixel_delta  = -1;

			draw_always_onoff = TRUE;

		} else
		if ( cursor >= maxim )
		{
//n_win_scrollbar_hwndprintf_literal( p, " maxim " );
//n_win_scrollbar_hwndprintf_literal( p, " %d %d ", p->cur_cursor_y, p->pixel_arrow_sy );

//if ( p->layout == N_WIN_SCROLLBAR_LAYOUT_VERTICAL )
//{
//	n_win_scrollbar_hwndprintf_literal( p, " %d %d ", (int) p->pixel_shaft, (int) p->sy );
//}

			p->pixel_offset = p->pixel_offset_prv;
			p->pixel_pos    = 0;
			   pixel_delta  = p->pixel_shaft;

			draw_always_onoff = TRUE;

		} else {
//n_win_scrollbar_hwndprintf_literal( p, " else " );
		}

//n_win_scrollbar_hwndprintf_literal( p, " %d %d : %d : %d ", minim, maxim, (n_type_gfx) p->pixel_offset, cursor );
	}


//n_win_scrollbar_hwndprintf_literal( p, " %f %f ", p->pixel_pos, p->pixel_step );

	if ( draw_always_onoff )
	{
		n_win_scrollbar_scroll_pixel( p, pixel_delta, N_WIN_SCROLLBAR_SCROLL_AUTO, reason );

		n_win_scrollbar_refresh( p );
	} else {
		BOOL ret = n_win_scrollbar_scroll_pixel( p, pixel_delta, N_WIN_SCROLLBAR_SCROLL_AUTO, reason );
		if ( ret ) { [p->view display]; }
	}


	p->prv_cursor_x = p->cur_cursor_x;
	p->prv_cursor_y = p->cur_cursor_y;


	return;
}




@interface NonnonScrollbar ()

@property n_win_scrollbar scr;

@end


@implementation NonnonScrollbar {

}


@synthesize scr;




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{
		n_win_scrollbar_init( &scr, self );

		n_mac_timer_init( self, @selector( n_fade_timer_method ), 33 );


		// [!] : for lack of mouse capture
		//n_mac_timer_init( self, @selector( n_timer_method ), 100 );

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




- (void) applicationWillBecomeActive:(NSNotification *)notification
{
//NSLog( @"applicationWillBecomeActive" );

	n_win_scrollbar *p = &scr;

	p->gray_onoff = FALSE;

	[self display];

}

- (void) applicationWillResignActive:(NSNotification *)notification
{
//NSLog( @"applicationWillResignActive" );

	n_win_scrollbar *p = &scr;

	p->gray_onoff = TRUE;

	[self display];

}




- (n_win_scrollbar*) n_scrollbar_struct_get
{
	return &scr;
}

- (void) n_scrollbar_parameter:(int) id step:(int) step page:(int) page max:(int) max pos:(int) pos redraw:(BOOL) redraw
{

	n_win_scrollbar *p = &scr;

	p->id = id;

	n_win_scrollbar_parameter( p, step, page, max, pos, redraw );

}

- (void) n_scrollbar_gradient:(void*) zero from:(u32)f to:(u32)t
{

	n_win_scrollbar *p = &scr;

	p->option |= N_WIN_SCROLLBAR_OPTION_SHAFT_GRADIENT;

	p->color_grad1 = f;
	p->color_grad2 = t;

}

- (void) n_scrollbar_nswindow_set:(NSWindow*)nswindow
{

	n_win_scrollbar *p = &scr;

	p->nswindow = nswindow;

}

- (void) n_scrollbar_enable:(BOOL) onoff redraw:(BOOL) redraw
{

	n_win_scrollbar *p = &scr;

	p->enabled_global = onoff;

	if ( redraw ) { [p->view display]; }

}

- (void) n_scrollbar_position_set:(int) v redraw:(BOOL) redraw
{

	n_win_scrollbar *p = &scr;

	p->unit_pos = 0;
	n_win_scrollbar_scroll_unit( p, v, N_WIN_SCROLLBAR_SCROLL_AUTO, N_WIN_SCROLLBAR_REASON_REDRAW );

	if ( redraw ) { [p->view display]; }

}

- (n_type_real) n_scrollbar_position_get
{

	n_win_scrollbar *p = &scr;

	return p->unit_pos;
}




- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
	return YES;
}




- (BOOL) isFlipped
{
	return YES;
}

- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );

	n_win_scrollbar *p = &scr;

	//p->sx = rect.size.width;
	//p->sy = rect.size.height;

//n_mac_draw_roundrect( [NSColor whiteColor], rect, 0 );

	n_win_scrollbar_draw( p );

}




- (void) n_fade_timer_method
{
//return;

	n_win_scrollbar *p = &scr;

	if ( p->fade_th.stop == FALSE )
	{
		[p->view display];
	} else
	if ( p->fade_ul.stop == FALSE )
	{
		[p->view display];
	} else
	if ( p->fade_dr.stop == FALSE )
	{
		[p->view display];
	}// else

}




- (void) updateTrackingAreas
{
//return;

	int options = ( NSTrackingMouseMoved | NSTrackingActiveAlways );
	NSTrackingArea *trackingArea = [
		[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:options
			       owner:self
			    userInfo:nil
	];

	[self addTrackingArea:trackingArea];

}

-(void)mouseEntered:(NSEvent *)theEvent
{
//NSLog(@"mouseEntered");

	// [Needed] : NSTrackingMouseEnteredAndExited 

}

-(void)mouseExited:(NSEvent *)theEvent
{
//NSLog(@"mouseExited");

	// [Needed] : NSTrackingMouseEnteredAndExited 

	// [x] : this works with window, not control area

}

- (void) mouseMoved:(NSEvent *)theEvent
{
//NSLog( @"mouseMoved" );

	// [Needed] : NSTrackingMouseMoved 

	n_win_scrollbar *p = &scr;

	if ( p->enabled_global == FALSE ) { return; }

	if (
		( p->nswindow != NULL )
		&&
		( FALSE == n_mac_window_is_keywindow( p->nswindow ) )
	)
	{
		return;
	}

	n_win_scrollbar_on_input( p );

	if ( p->hovered_thumb )
	{
		n_bmp_fade_go( &p->fade_th, n_bmp_white );
		n_bmp_fade_go( &p->fade_ul, n_bmp_black );
		n_bmp_fade_go( &p->fade_dr, n_bmp_black );

		[p->view display];
	} else
	if ( p->hovered_arrow_ul )
	{
//NSLog( @"mouseMoved : hovered_arrow_ul" );

		n_bmp_fade_go( &p->fade_th, n_bmp_black );

		if ( p->enabled_ul )
		{
			n_bmp_fade_go( &p->fade_ul, n_bmp_white );
		}

		[p->view display];
	} else
	if ( p->hovered_arrow_dr )
	{
		n_bmp_fade_go( &p->fade_th, n_bmp_black );

		if ( p->enabled_dr )
		{
			n_bmp_fade_go( &p->fade_dr, n_bmp_white );
		}

		[p->view display];
	} else {
		n_bmp_fade_go( &p->fade_th, n_bmp_black );
		n_bmp_fade_go( &p->fade_ul, n_bmp_black );
		n_bmp_fade_go( &p->fade_dr, n_bmp_black );

		if (
			( p->fade_th.stop != FALSE )
			||
			( p->fade_ul.stop != FALSE )
			||
			( p->fade_dr.stop != FALSE )
		)
		{
			[p->view display];
		}
	}

}



- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	n_win_scrollbar *p = &scr;


	if ( p->enabled_global == FALSE ) { return; }


	n_win_scrollbar_on_input( p );

	n_win_scrollbar_position_cursor( p, &p->prv_cursor_x, &p->prv_cursor_y );


	if ( p->hovered_thumb )
	{
//NSLog( @"p->hovered_thumb" );

		p->pressed_thumb = TRUE;

		if ( p->drag_onoff == FALSE )
		{

			p->drag_onoff = TRUE;

			n_win_scrollbar_offset( p );
			p->pixel_offset_prv = p->pixel_offset;

		}

		[p->view display];

	} else
	if ( p->hovered_arrow_ul )
	{

		if ( p->enabled_ul == FALSE ) { return; }

		p->pressed_arrow_ul = TRUE;
		n_win_scrollbar_event_arrow( p, N_WIN_SCROLLBAR_REASON_ARROW_MOUSEDOWN );

	} else
	if ( p->hovered_arrow_dr )
	{

		if ( p->enabled_dr == FALSE ) { return; }

		p->pressed_arrow_dr = TRUE;
		n_win_scrollbar_event_arrow( p, N_WIN_SCROLLBAR_REASON_ARROW_MOUSEDOWN );

	} else
	if ( p->hovered_shaft )
	{
//n_win_scrollbar_hwndprintf_literal( p, " p->hovered_shaft " );

		n_win_scrollbar_event_shaft( p, N_WIN_SCROLLBAR_REASON_SHAFT_MOUSEDOWN );

	}


	return;
}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"mouseUp");

	n_win_scrollbar *p = &scr;


	if ( p->enabled_global == FALSE ) { return; }


	n_win_scrollbar_on_input( p );


	if ( p->drag_onoff )
	{
		n_win_scrollbar_on_drag( p, N_WIN_SCROLLBAR_REASON_DRAG_MOUSEUP );
	}


	p->drag_onoff       = FALSE;
	p->pressed_thumb    = FALSE;
	p->pressed_arrow_ul = FALSE;
	p->pressed_arrow_dr = FALSE;
	p->pressed_shaft    = FALSE;

	[p->view display];


	return;
}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );

	n_win_scrollbar *p = &scr;

	if ( p->drag_onoff )
	{
		n_win_scrollbar_on_drag( p, N_WIN_SCROLLBAR_REASON_DRAG_DRAGGED );

		[p->view display];
	}

}

- (void)scrollWheel:(NSEvent *)theEvent
{
//NSLog( @"%lu : %0.2f %0.2f", [theEvent phase], [theEvent deltaY], [theEvent scrollingDeltaY] ); return;


	n_win_scrollbar *p = &scr;

	if ( p->enabled_global == FALSE ) { return; }


	//static u32 timer = 0;
	//if ( FALSE == n_game_timer( &timer, 500 ) ) { return; }


	CGFloat unit = 0;

	CGFloat delta = [theEvent deltaY];
	if ( 0 > delta )
	{
		unit = -p->unit_step;
	} else
	if ( 0 < delta )
	{
		unit =  p->unit_step;
	}
//NSLog( @"%f", unit );

	n_win_scrollbar_scroll_unit( p, unit, N_WIN_SCROLLBAR_SCROLL_AUTO, N_WIN_SCROLLBAR_REASON_WHEEL );


	[p->view display];

}




@end




#endif // _H_NONNON_WIN32_MAC_SCROLLBAR


