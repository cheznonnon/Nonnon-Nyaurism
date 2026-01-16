// Nonnon Nyaurism for Mac
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




static n_posix_char n_nyaurism_tooltip_str[ 100 ] = "0 msec.";

void
n_nyaurism_tooltip_calc( n_type_real pixel_x )
{

	n_type_real norm = (n_type_real) pixel_x / 512;
//NSLog( @"%f %f", norm, norm * N_WAV_MSEC( &n_nyaurism->wav ) );

	n_posix_sprintf_literal( n_nyaurism_tooltip_str, "%0.0f msec", norm * N_WAV_MSEC( &n_nyaurism->wav ) );


	return;
}

void
n_nyaurism_tooltip_draw( n_bmp *bmp_canvas, n_posix_char *str )
{

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.sx                 = 0;
	gdi.sy                 = 0;
	gdi.style              = 0;//N_GDI_SMOOTH;

	gdi.base_color_bg      = n_bmp_rgb_mac( 222,222,222 );
	gdi.base_color_fg      = n_bmp_rgb_mac( 222,222,222 );
	gdi.base_style         = N_GDI_BASE_SOLID;

	gdi.frame_style        = N_GDI_FRAME_ROUND;//N_GDI_FRAME_SIMPLE;

	gdi.text               = str;
	gdi.text_font          = n_posix_literal( "Trebuchet MS" );
	gdi.text_size          = 14;
	gdi.text_color_main    = n_bmp_rgb_mac( 0,0,0 );
	gdi.text_style         = N_GDI_TEXT_MAC_NO_CROP;
	gdi.text_fxsize2       = 0;


	n_bmp bmp; n_bmp_zero( &bmp ); n_gdi_bmp( &gdi, &bmp );
//n_bmp_save( &bmp, "/Users/nonnon/Desktop/ret.bmp" );


	// [!] : Centering

	n_type_gfx bmpsx = N_BMP_SX( bmp_canvas );
	n_type_gfx bmpsy = N_BMP_SY( bmp_canvas );

	n_type_gfx cx = ( ( bmpsx - gdi.sx ) / 2 );
	n_type_gfx cy = ( ( bmpsy - gdi.sy ) / 2 );


	n_bmp_transcopy( &bmp, bmp_canvas, 0,0,gdi.sx,gdi.sy, cx,cy );


	n_bmp_free_fast( &bmp );


	return;
}




void
n_nyaurism_plotter_selection_off( void )
{
//return;

	n_nyaurism->selection_reverse         = n_false;
	n_nyaurism->plotter_selection_channel = 0;

	n_nyaurism->selection_from_pixel = 0;
	n_nyaurism->selection_loop_pixel = 0;
	n_nyaurism->selection_size_pixel = 0;

	n_nyaurism_slider_redraw( &n_nyaurism->wav );


	return;
}

n_type_gfx
n_nyaurism_plotter_selection_left_edge( void )
{

	n_type_gfx ret;

	if ( n_nyaurism->selection_reverse )
	{
		ret = n_nyaurism->selection_loop_pixel;
	} else {
		ret = n_nyaurism->selection_from_pixel;
	}


	return ret;
}

n_bool
n_nyaurism_plotter_selection_line_only( void )
{

	n_bool ret = n_false;


	if (
		( 0 != n_nyaurism_plotter_selection_left_edge() )
		&&
		( 0 == n_nyaurism->selection_size_pixel )
	)
	{
		ret = n_true;
	}


	return ret;
}

void
n_nyaurism_plotter_selection_pixel2sample( n_wav *wav, n_type_gfx *ret_x, n_type_gfx *ret_sx )
{
//return;

	n_type_gfx st = n_nyaurism->selection_step;
	n_type_gfx  x = st * n_nyaurism_plotter_selection_left_edge();
	n_type_gfx sx = st * n_nyaurism->selection_size_pixel;

	if ( x < 0 ) { sx += x; x = 0; }


	n_type_gfx c = (n_type_gfx) N_WAV_COUNT( wav );
	if ( ( x + sx ) >= c ) { sx = c - x; }


	if ( ret_x  != NULL ) { (*ret_x ) =  x; }
	if ( ret_sx != NULL ) { (*ret_sx) = sx; }


	return;
}




#include "./plotter_verb.c"




void
n_nyaurism_plotter_draw( n_bmp *bmp, n_wav *wav, n_type_gfx sx, n_type_gfx sy )
{
//return;

	if ( n_wav_error_format( wav ) ) { return; }


	const u32  color_l = n_bmp_rgb_mac(   0,200,255 );
	const u32  color_r = n_bmp_rgb_mac(   0,255,200 );
	const u32  c_grid1 = n_bmp_rgb_mac(  50, 50, 50 );
	const u32  c_grid2 = n_bmp_rgb_mac(   0,100,100 );
	const u32  cselect = n_bmp_rgb_mac( 255,255,255 );


	sx = n_posix_max_n_type_gfx( 1, sx );
	sy = n_posix_max_n_type_gfx( 1, sy );


	const n_type_gfx  count  = (n_type_gfx) N_WAV_COUNT( wav );
	const n_type_real per_ms = N_WAV_RATE ( wav ) / 1000;

	n_type_gfx  unit_x  = sx;
	n_type_gfx  unit_y  = sy / 4;
	n_type_gfx  line_l  = unit_y * 1;
	n_type_gfx  line_r  = unit_y * 3;
	n_type_real ratio_x = (n_type_real) unit_x /     count;
	n_type_real ratio_y = (n_type_real) unit_y / n_wav_sample_amp( wav );
	n_type_gfx  step_x  = n_posix_max_n_type_gfx( 1, count / unit_x );
//NSLog( @"%d %d %d %d", sy, unit_y, line_l, line_r );

	n_nyaurism->selection_step = step_x;
//NSLog( @"Step %d", step_x );


	// [!] : Main Canvas

	static n_bool init = n_false;
	if ( init == n_false )
	{
		init = n_true;
		n_bmp_zero( bmp );

		n_bmp_new( &n_nyaurism->seekbar_bmp, sx, sy );
	}

	n_bmp_new( bmp, sx,sy );


	// [!] : Grid

	n_type_gfx grid_1 = (n_type_gfx) ( (n_type_real) (   10 * per_ms ) * ratio_x );
	n_type_gfx grid_2 = (n_type_gfx) ( (n_type_real) ( 1000 * per_ms ) * ratio_x );


	n_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_true;


	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	u32 cores = n_posix_cpu_count(); if ( sx < cores ) { cores = 1; }


	n_type_gfx thread_sx = sx / cores;


	u32 i = 0;
	n_posix_loop
	{


	NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{


	n_type_gfx x = i * thread_sx;
	n_type_gfx t = 0;
	n_posix_loop
	{

		// [!] : Grid : threshold is 4px

		if ( ( grid_1 >= 4 )&&( 0 == ( x % grid_1 ) ) )
		{
			n_bmp_box( bmp, x,0, 1,sy, c_grid1 );
		} else
		if ( ( grid_2 >= 4 )&&( 0 == ( x % grid_2 ) ) )
		{
			n_bmp_box( bmp, x,0, 1,sy, c_grid2 );
		}


		u32 pos = x * step_x;
		if ( pos >= count ) { break; }


		// [x] : simple and fast but not-displayed data will be made

		n_type_real l,r;

		n_wav_sample_get( wav, pos, &l, &r );
/*
		// [x] : average : too much shrunk

		n_type_real l_avr = 0;
		n_type_real r_avr = 0;

		u32 z = 0;
		n_posix_loop
		{
			n_type_real l,r; n_wav_sample_get( wav, pos + z, &l, &r );

			l_avr += l;
			r_avr += r;

			z++;
			if ( z >= step_x ) { break; }
		}

		l = (n_type_real) l_avr / step_x;
		r = (n_type_real) r_avr / step_x;
*/

		// [!] : Main : ( n * -1 ) : up-side-down : WAV to BMP

		n_type_gfx ty_l = (n_type_gfx) ( line_l + trunc( l * ratio_y * -1 ) );
		n_type_gfx ty_r = (n_type_gfx) ( line_r + trunc( r * ratio_y * -1 ) );

		n_type_gfx fy_l = line_l;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_l, color_l );

			if ( fy_l == ty_l ) { break; }
			if ( fy_l > ty_l ) { fy_l--; } else { fy_l++; }
		}

		n_type_gfx fy_r = line_r;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_r, color_r );

			if ( fy_r == ty_r ) { break; }
			if ( fy_r > ty_r ) { fy_r--; } else { fy_r++; }
		}

/*
		// [x] : too much chunky

		n_type_real l_min = 0;
		n_type_real l_max = 0;
		n_type_real r_min = 0;
		n_type_real r_max = 0;

		u32 z = 0;
		n_posix_loop
		{
			n_type_real l,r; n_wav_sample_get( wav, pos + z, &l, &r );

			if ( l < l_min ) { l_min = l; }
			if ( l > l_max ) { l_max = l; }

			if ( r < r_min ) { r_min = r; }
			if ( r > r_max ) { r_max = r; }

			z++;
			if ( z >= step_x ) { break; }
		}


		// [!] : Main : ( n * -1 ) : up-side-down : WAV to BMP

		n_type_gfx ty_l_min = (n_type_gfx) ( line_l + trunc( l_min * ratio_y * -1 ) );
		n_type_gfx ty_l_max = (n_type_gfx) ( line_l + trunc( l_max * ratio_y * -1 ) );
		n_type_gfx ty_r_min = (n_type_gfx) ( line_r + trunc( r_min * ratio_y * -1 ) );
		n_type_gfx ty_r_max = (n_type_gfx) ( line_r + trunc( r_max * ratio_y * -1 ) );

		n_type_gfx fy_l = line_l;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_l, color_l );

			fy_l++;
			if ( fy_l > ty_l_min ) { break; }
		}

		fy_l = line_l;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_l, color_l );

			fy_l--;
			if ( fy_l < ty_l_max ) { break; }
		}

		n_type_gfx fy_r = line_r;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_r, color_r );

			fy_r++;
			if ( fy_r > ty_r_min ) { break; }
		}

		fy_r = line_r;
		n_posix_loop
		{//break;
			// [!] : don't use n_bmp_ptr_set_fast()
			n_bmp_ptr_set( bmp, x,fy_r, color_r );

			fy_r--;
			if ( fy_r < ty_r_max ) { break; }
		}
*/
/*
		// [!] : hard to recognize peak position

		u32 z = 0;
		n_posix_loop
		{
			n_type_real l,r; n_wav_sample_get( wav, pos + z, &l, &r );

			n_type_gfx ty_l = (n_type_gfx) ( line_l + trunc( l * ratio_y * -1 ) );
			n_type_gfx ty_r = (n_type_gfx) ( line_r + trunc( r * ratio_y * -1 ) );

			n_bmp_ptr_set( bmp, x, ty_l, color_l );
			n_bmp_ptr_set( bmp, x, ty_r, color_r );

			z++;
			if ( z >= step_x ) { break; }
		}
*/

		x++; t++;
		if ( t >= thread_sx ) { break; }
	}


	}];
	[n_wav_queue addOperation:o];


	[n_wav_queue waitUntilAllOperationsAreFinished];


		i++;
		if ( i >= cores ) { break; }
	}


	n_bmp_is_multithread = prv;


	// [!] : L/R separator

	n_bmp_line( bmp, 0, sy/2, sx, sy/2, n_bmp_rgb_mac( 128,128,128 ) );


	// [!] : Indicator / Selector

	{
		n_type_gfx from = n_nyaurism_plotter_selection_left_edge();
		n_type_gfx size = n_nyaurism->selection_size_pixel;
//NSLog( @"%d %d", from, size );

		if ( n_nyaurism->selection_shift_onoff )
		{
			if ( size == 0 ) { size = sx; }
		}

		n_type_gfx ty  = 0;
		n_type_gfx tsy = sy;
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			ty  = sy / 2;
			tsy = sy / 2;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			tsy = sy / 2;
		}

		n_bmp_mixer( bmp, from, ty, size, tsy, cselect, 0.2 );

		if ( size == 0 )
		{
			if ( from )
			{
//NSLog( @"indicator : %d", from );
				if ( n_nyaurism->plotter_selection_channel == 1 )
				{
					n_bmp_line( bmp, from, tsy, from, sy, cselect );
				} else
				if ( n_nyaurism->plotter_selection_channel == 2 )
				{
					n_bmp_line( bmp, from, 0, from, tsy, cselect );
				} else {
					n_bmp_line( bmp, from, 0, from, sy, cselect );
				}
			}
		} else
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			n_bmp_line( bmp, from -    1, tsy, from -    1,  sy, cselect );
			n_bmp_line( bmp, from + size, tsy, from + size,  sy, cselect );
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			n_bmp_line( bmp, from -    1,   0, from -    1, tsy, cselect );
			n_bmp_line( bmp, from + size,   0, from + size, tsy, cselect );
		} else {
			n_bmp_line( bmp, from -    1,   0, from -    1,  sy, cselect );
			n_bmp_line( bmp, from + size,   0, from + size,  sy, cselect );
		}
	}


	if ( ( n_nyaurism->selection_size_pixel )||( n_nyaurism->selection_shift_onoff ) )
	{
		n_nyaurism_tooltip_draw( bmp, n_nyaurism_tooltip_str );
	}


	return;
}

void
n_nyaurism_plotter_seekbar_draw( n_bmp *bmp, n_wav *wav, n_type_gfx sx, n_type_gfx sy, float seek_norm )
{
//return;

	n_bmp_flush( bmp, n_bmp_black_invisible );

	if ( seek_norm != -1 )
	{
		n_bmp_box( bmp, (float) sx * seek_norm, 0, 1, sy, n_bmp_rgb_mac( 255,0,150 ) );
	}


	return;
}




@interface NonnonPlotter : NSView

@property NSMenu *n_popup_menu;

@end




@implementation NonnonPlotter {

}




- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{
		n_bmp_safemode = n_false;

		n_bmp_transparent_onoff_default = n_false;

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

	}


	return self;
}




- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );

	NSRect r = NSMakeRect( 0,0,512,256 );

	if ( n_nyaurism_now_playing() )
	{
		n_type_gfx bmpsx = r.size.width;
		n_type_gfx bmpsy = r.size.height;

		n_nyaurism_plotter_seekbar_draw( &n_nyaurism->seekbar_bmp, &n_nyaurism->wav, bmpsx, bmpsy, n_nyaurism->seekbar_float_norm );

		n_bmp bmp_canvas; n_bmp_carboncopy( &n_nyaurism->bmp, &bmp_canvas );

		n_bmp_flush_transcopy( &n_nyaurism->seekbar_bmp, &bmp_canvas );

		n_mac_image_nbmp_direct_draw( &bmp_canvas, &r, YES );

		n_bmp_free( &bmp_canvas );
	} else {
		n_nyaurism_plotter_draw( &n_nyaurism->bmp, &n_nyaurism->wav, r.size.width, r.size.height );
		n_mac_image_nbmp_direct_draw( &n_nyaurism->bmp, &r, YES );
	}

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

- (void) keyDown:(NSEvent*) theEvent
{
//NSLog( @"Key Code = %d", theEvent.keyCode );

	if ( n_nyaurism_now_playing() ) { return; }


	switch( theEvent.keyCode ) {

#ifdef DEBUG
	case N_MAC_KEYCODE_F1:
	{

		//n_fft_histogram_test( &n_nyaurism->wav );

		n_wav_pinknoise( &n_nyaurism->wav, 0, 1.0, 1.0 );

		[self display];
	}
	break;
#endif

	case N_MAC_KEYCODE_F2:
	{
		n_nyaurism->fname = n_mac_fork_rename( n_nyaurism->fname );

		NSString *title = [NSString stringWithFormat:@"%@ - Nyaurism", n_nyaurism->fname];
		[n_mac_image_window setTitle:title];
	}
	break;

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_undo( &n_nyaurism->wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_SELECT_ALL: // [!] : 'A'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_select_all( &n_nyaurism->wav, YES );

			[self display];
		}

	break;

	case N_MAC_KEYCODE_CUT: // [!] : 'X'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_cut( &n_nyaurism->wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_COPY: // [!] : 'C'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_copy( &n_nyaurism->wav, &n_nyaurism->wav_clip );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_PASTE: // [!] : 'V'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_paste( &n_nyaurism->wav );
			[self display];
		}

	break;

	case 11: // [!] : 'B'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_nyaurism->selection_shift_onoff ) { break; }

			n_nyaurism_plotter_overwrite( &n_nyaurism->wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_BACKSPACE:

		if ( n_nyaurism->selection_shift_onoff ) { break; }

		n_nyaurism_plotter_delete( &n_nyaurism->wav );
		[self display];

	break;

	} // switch

}




- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog( @"mouseDown : %ld", [theEvent clickCount] );

	n_nyaurism->selection_drag_onoff = n_false;

	if ( n_nyaurism_now_playing() ) { return; }

	NSPoint pt = n_mac_cursor_position_get( self );

	if ( [theEvent clickCount] >= 2 )
	{
//NSLog( @"double-click" );

		n_nyaurism->selection_select_all = n_true;

		NSUInteger flags = [theEvent modifierFlags];
		if ( flags & NSEventModifierFlagShift ) { return; }

		n_nyaurism->selection_reverse = n_false;

		n_nyaurism->selection_from_pixel = 0;
		n_nyaurism->selection_loop_pixel = 0;
		n_nyaurism->selection_size_pixel = N_WAV_COUNT( &n_nyaurism->wav ) / n_nyaurism->selection_step;

		n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_undo );
		n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );

		n_nyaurism_tooltip_calc( n_nyaurism->selection_size_pixel );

	} else
	if ( n_nyaurism->selection_shift_onoff )
	{

		n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_undo );
		n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );

		n_nyaurism->selection_shift_pixel = pt.x;

		n_type_gfx f = n_nyaurism_plotter_selection_left_edge();
		n_nyaurism->selection_reselect_pixel = pt.x - f;

		if ( f )
		{
			n_nyaurism_plotter_copy( &n_nyaurism->wav, &n_nyaurism->wav_base );
		} else {
			n_nyaurism_plotter_select_all( &n_nyaurism->wav, NO );
		}
	} else
	//
	{

		if ( n_nyaurism->selection_command_onoff )
		{
			if ( ( pt.y >   0 )&&( pt.y < 128 ) )
			{
				if ( n_nyaurism->plotter_selection_channel == 2 )
				{
					n_nyaurism_plotter_selection_off();
				} else {
					n_nyaurism->plotter_selection_channel = 1;
				}
			} else
			if ( ( pt.y > 128 )&&( pt.y < 256 ) )
			{
				if ( n_nyaurism->plotter_selection_channel == 1 )
				{
					n_nyaurism_plotter_selection_off();
				} else {
					n_nyaurism->plotter_selection_channel = 2;
				}
			}
		} else {
			if ( ( pt.y >   0 )&&( pt.y < 128 ) )
			{
				if ( n_nyaurism->plotter_selection_channel == 2 )
				{
					n_nyaurism_plotter_selection_off();
				}
			} else
			if ( ( pt.y > 128 )&&( pt.y < 256 ) )
			{
				if ( n_nyaurism->plotter_selection_channel == 1 )
				{
					n_nyaurism_plotter_selection_off();
				}
			}
		}


		n_type_gfx f = n_nyaurism_plotter_selection_left_edge();
		n_type_gfx s = n_nyaurism->selection_size_pixel;

		if ( n_nyaurism->selection_select_all )
		{
			n_nyaurism->selection_select_all = n_false;
			n_nyaurism_plotter_selection_off();
		} else
		if ( ( f < pt.x )&&( ( f + s ) > pt.x ) )
		{
			n_nyaurism->selection_reselect_onoff = n_true;
			n_nyaurism->selection_reselect_pixel = pt.x - f;
		} else
		if ( s != 0 )
		{
			n_nyaurism_plotter_selection_off();
		} else
		//
		{
			n_nyaurism->selection_reverse = n_false;

			n_nyaurism->selection_from_pixel = pt.x;
			n_nyaurism->selection_loop_pixel = pt.x;
			n_nyaurism->selection_size_pixel = 0;

			if ( ( pt.x * n_nyaurism->selection_step ) < N_WAV_COUNT( &n_nyaurism->wav )  )
			{
				n_nyaurism->selection_drag_onoff = n_true;
			}
		}
	}

	[self display];

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp : %ld", [theEvent clickCount] );

	if ( n_nyaurism_now_playing() ) { return; }

	if ( n_nyaurism->selection_reselect_onoff )
	{
		n_nyaurism->selection_reselect_onoff = n_false;
	} else
	if ( n_nyaurism->selection_drag_onoff )
	{
		n_nyaurism->selection_drag_onoff = n_false;

		NSPoint pt = n_mac_cursor_position_get( self );
		if ( pt.x < n_nyaurism->selection_from_pixel )
		{
			n_nyaurism->selection_from_pixel = n_nyaurism->selection_loop_pixel;
		}
	}

	n_nyaurism_slider_redraw( &n_nyaurism->wav );

	[self display];

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );

	if ( n_nyaurism_now_playing() ) { return; }


	NSPoint pt = n_mac_cursor_position_get( self );

	if ( n_nyaurism->selection_reselect_onoff )
	{
		n_type_gfx f = pt.x - n_nyaurism->selection_reselect_pixel;
		n_type_gfx s = n_nyaurism->selection_size_pixel;

		// [!] : currently out-of-bound is not supported
		if ( f < 0 )
		{
			f = 0;
		} else
		if ( ( f + s ) > 512 )
		{
			f = 512 - s;
		}

		n_nyaurism->selection_from_pixel = f;
		n_nyaurism->selection_loop_pixel = n_nyaurism->selection_from_pixel;

		n_nyaurism_slider_redraw( &n_nyaurism->wav );

		[self display];
	} else
	if ( n_nyaurism->selection_shift_onoff )
	{
		// [!] : don't use u32 here
		n_type_int f = pt.x - n_nyaurism->selection_reselect_pixel;
		n_type_int s = n_nyaurism->selection_size_pixel;
/*
		// [!] : out-of-bound is acceptable
		if ( f < 0 )
		{
			f = 0;
		} else
		if ( ( f + s ) > 512 )
		{
			f = 512 - s;
		}
*/
		n_nyaurism_tooltip_calc( f );

		n_nyaurism->selection_from_pixel = (n_type_gfx) f;
		n_nyaurism->selection_loop_pixel = n_nyaurism->selection_from_pixel;

		n_type_int  x = f * n_nyaurism->selection_step;
		n_type_int sx = s * n_nyaurism->selection_step;

		n_type_real l = 1.0; if ( n_nyaurism->plotter_selection_channel == 1 ) { l = -1; }
		n_type_real r = 1.0; if ( n_nyaurism->plotter_selection_channel == 2 ) { r = -1; }

		n_nyaurism_backup( &n_nyaurism->wav_undo, &n_nyaurism->wav );

		if ( n_nyaurism->mix_onoff )
		{
			n_wav_copy( &n_nyaurism->wav_base, &n_nyaurism->wav, 0,sx, x, l,r, N_WAV_COPY_ADD );
		} else {
			n_wav_copy( &n_nyaurism->wav_base, &n_nyaurism->wav, 0,sx, x, l,r, N_WAV_COPY_SET );
		}

		n_nyaurism_slider_redraw( &n_nyaurism->wav );

		[self display];
	} else
	if ( n_nyaurism->selection_drag_onoff )
	{
		if ( pt.x < 0 ) { pt.x = 0; }

		n_type_gfx s = N_WAV_COUNT( &n_nyaurism->wav ) / n_nyaurism->selection_step;

		if ( ( n_nyaurism->selection_step == 1 )&&( pt.x > s ) )
		{
			n_nyaurism->selection_size_pixel = s - n_nyaurism_plotter_selection_left_edge();
		} else
		if ( pt.x < n_nyaurism->selection_from_pixel )
		{
			n_nyaurism->selection_reverse =  n_true;

			n_nyaurism->selection_size_pixel = n_nyaurism->selection_from_pixel - pt.x;
			n_nyaurism->selection_loop_pixel = pt.x;
		} else {
			n_nyaurism->selection_reverse = n_false;

			n_type_gfx size = 512 - n_nyaurism_plotter_selection_left_edge();

			n_nyaurism->selection_size_pixel = n_posix_min_n_type_gfx( size, pt.x - n_nyaurism_plotter_selection_left_edge() );
		}

		n_nyaurism_tooltip_calc( n_nyaurism->selection_size_pixel );
	}

	[self display];

}
/*
-(void)n_timer_method
{
	if ( n_false == n_mac_window_is_hovered( self ) )
	{
		NSEvent *e = [[NSEvent alloc] init];
		[self mouseUp:e];
	}

}
*/
- (void) updateTrackingAreas
{

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

- (void) mouseMoved:(NSEvent*) theEvent
{
//NSLog( @"mouseMoved" );

	if ( n_nyaurism_now_playing() ) { return; }

}

-(void)flagsChanged:(NSEvent *)event
{
//NSLog( @"flagsChanged" );

	if ( n_nyaurism_now_playing() ) { return; }


	NSUInteger flags = [event modifierFlags];

	if ( flags & NSEventModifierFlagShift )
	{
//NSLog( @"shift on" );

		n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( &n_nyaurism->wav, &x, &sx );

		if ( ( x != 0 )&&( sx == 0 ) )
		{
			// [!] : indicator mode
		} else
		//
		{
			n_nyaurism->selection_shift_onoff = n_true;

			n_nyaurism_tooltip_calc( 0 );

			[self mouseMoved:event];
		}
	} else
	if ( n_nyaurism->selection_shift_onoff )
	{
//NSLog( @"shift off" );

		n_nyaurism->selection_shift_onoff = n_false;

		n_nyaurism_tooltip_calc( n_nyaurism->selection_size_pixel );

		n_type_gfx f = n_nyaurism->selection_from_pixel;
		n_type_gfx s = n_nyaurism->selection_size_pixel;

		// [!] : clipper

		if ( f < 0 )
		{
			s = s + f;
			f = 0;
		}

		if ( ( f + s ) > 512 )
		{
			s -= ( f + s ) - 512;
		}

		n_nyaurism->selection_from_pixel = f;
		n_nyaurism->selection_loop_pixel = f;
		n_nyaurism->selection_size_pixel = s;

	}


	if ( flags & NSEventModifierFlagCommand )
	{
		n_nyaurism->selection_command_onoff = n_true;
	} else
	if ( n_nyaurism->selection_command_onoff )
	{
		n_nyaurism->selection_command_onoff = n_false;
	}


	n_nyaurism_slider_redraw( &n_nyaurism->wav );

	[self display];

}




- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog(@"rightMouseUp");

	[NSMenu popUpContextMenu:_n_popup_menu withEvent:theEvent forView:self];

}


@end
