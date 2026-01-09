// Nonnon Nyaurism for Mac
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nyaurism_shiftcopy( n_wav *f, n_wav *t, u32 tx, BOOL l_onoff, BOOL r_onoff )
{

	if ( n_wav_error_format( f ) ) { return; }
	if ( n_wav_error_format( t ) ) { return; }


	if ( tx == 0 ) { return; }


	u32 count_f = N_WAV_COUNT( f );
	//u32 count_t = N_WAV_COUNT( t );


	if ( n_nyaurism_mix_onoff )
	{

		// [!] : Feedbacker

		n_wav_mute( t, 0, N_WAV_COUNT( t ) );

		n_wav tmp; n_wav_carboncopy( f, &tmp );

		u32 x = 0;
		n_posix_loop
		{//break;

			if ( n_wav_sample_is_accessible( t, tx + x ) )
			{
				n_type_real l_1, r_1; n_wav_sample_get(    f,      x, &l_1, &r_1 );
				n_type_real l_2, r_2; n_wav_sample_get( &tmp, tx + x, &l_2, &r_2 );

				n_type_real l = ( l_1 + l_2 ) / 2;
				n_type_real r = ( r_1 + r_2 ) / 2;

				n_wav_sample_set( t, tx + x, l, r );
			}

			x++;
			if ( x >= count_f ) { break; }
		}

		//n_wav_normalize( t, 0, n_slider_value_l_global, n_slider_value_r_global );

		n_wav_free( &tmp );

	} else {

		n_wav_mute( t, 0, N_WAV_COUNT( t ) );

		n_type_real l, r;

		u32 x = 0;
		n_posix_loop
		{//break;

			if ( n_wav_sample_is_accessible( t, tx + x ) )
			{
				n_wav_sample_get( f,      x, &l, &r );
				n_wav_sample_set( t, tx + x,  l,  r );
			}

			x++;
			if ( x >= count_f ) { break; }
		}

	}


	// [!] : restore original

	if ( ( l_onoff )&&( r_onoff ) ) { return; }

	u32 x = 0;
	n_posix_loop
	{
		if ( n_wav_sample_is_accessible( t, x ) )
		{
			n_type_real l, r;

			n_wav_sample_get( f, x, &l, &r );

			if ( l_onoff ) { n_wav_sample_get( t, x, NULL, &r ); }
			if ( r_onoff ) { n_wav_sample_get( t, x, &l, NULL ); }

			n_wav_sample_set( t, x,  l,  r );
		}

		x++;
		if ( x >= count_f ) { break; }
	}


	return;
}




// internal
void
n_wav_filter_overdrive_channel( double *audio, int count, int factor, double drive )
{

	int     oversampled_len = count * factor;
	double *oversampled     = malloc( oversampled_len * sizeof( double ) );


	for( int i = 0; i < oversampled_len; i++ )
	{
		double pos = (double) i / factor;

		int     idx = (int) pos;
		double frac = pos - idx;

		if ( ( idx + 1 ) < count )
		{
			oversampled[ i ] = audio[ idx ] * ( 1.0 - frac ) + audio[ idx + 1 ] * frac;
		} else {
			oversampled[ i ] = audio[ count - 1 ];
		}
	}


	for( int i = 0; i < oversampled_len; i++ )
	{
		double x = oversampled[ i ] * drive;
 
		if ( x >  1.0 ) { x =  drive - x; }
		if ( x < -1.0 ) { x = -drive - x; }

		x = tanh( x * 3.0 ) / 3.0;

		//oversampled[ i ] = x * 0.5;
	}


	for ( int i = 0; i < count; i++ )
	{
		double sum = 0.0;

		for( int j = 0; j < factor; j++ )
		{
			sum += oversampled[ i * factor + j ];
		}

		audio[ i ] = sum / factor;
	}


	free( oversampled );


	return;
}

void
n_wav_filter_overdrive( n_wav *wav, int factor, double drive )
{

	float  *ptr   = (float*) N_WAV_PTR( wav );
	int     count = N_WAV_COUNT( wav );


	double *audio_l = (double*) n_memory_new_closed( count * sizeof( double ) );
	double *audio_r = (double*) n_memory_new_closed( count * sizeof( double ) );

	for( int i = 0; i < count; i++ )
	{
		audio_l[ i ] = (double) ptr[ ( i * 2 ) + 0 ];
		audio_r[ i ] = (double) ptr[ ( i * 2 ) + 1 ];
	}


factor = 4;
drive  = 1.5;

	n_wav_filter_overdrive_channel( audio_l, count, factor, drive );
	n_wav_filter_overdrive_channel( audio_r, count, factor, drive );


	for( int i = 0; i < count; i++ )
	{
		ptr[ ( i * 2 ) + 0 ] = (float) audio_l[ i ];
		ptr[ ( i * 2 ) + 1 ] = (float) audio_r[ i ];
	}


	n_memory_free_closed( audio_l );
	n_memory_free_closed( audio_r );


	return;
}




static n_posix_bool n_nyaurism_plotter_selection_reverse    = n_posix_false;
static n_posix_bool n_nyaurism_plotter_selection_drag_onoff = n_posix_false;
static n_type_gfx   n_nyaurism_plotter_selection_from       = 0;
static n_type_gfx   n_nyaurism_plotter_selection_loop       = 0;
static n_type_gfx   n_nyaurism_plotter_selection_size       = 0;
static n_type_gfx   n_nyaurism_plotter_selection_step       = 0;
static n_wav        n_nyaurism_plotter_selection_clip;

static n_posix_bool n_nyaurism_plotter_selection_shift_onoff = n_posix_false;
static int          n_nyaurism_plotter_selection_shift_ch    = 0;
static n_type_gfx   n_nyaurism_plotter_selection_shift       = 0;




static n_posix_char n_nyaurism_tooltip_str[ 100 ] = "0 msec.";

void
n_nyaurism_tooltip_calc( float x )
{

	n_type_real sample_per_msec = (n_type_real) N_WAV_RATE( &n_nyaurism_wav ) / 1000.0;

	n_posix_sprintf_literal( n_nyaurism_tooltip_str, "%0.0f msec", x / sample_per_msec );


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

	n_nyaurism_plotter_selection_reverse = n_posix_false;

	n_nyaurism_plotter_selection_from = 0;
	n_nyaurism_plotter_selection_loop = 0;
	n_nyaurism_plotter_selection_size = 0;


	return;
}

n_type_gfx
n_nyaurism_plotter_selection_left_edge( void )
{

	n_type_gfx ret;

	if ( n_nyaurism_plotter_selection_reverse )
	{
		ret = n_nyaurism_plotter_selection_loop;
	} else {
		ret = n_nyaurism_plotter_selection_from;
	}


	return ret;
}

void
n_nyaurism_plotter_selection_pixel2sample( n_wav *wav, n_type_gfx *ret_x, n_type_gfx *ret_sx )
{
//return;

	n_type_gfx st = n_nyaurism_plotter_selection_step;
	n_type_gfx  x = st * n_nyaurism_plotter_selection_left_edge();
	n_type_gfx sx = st * n_nyaurism_plotter_selection_size;

	if ( x < 0 ) { sx += x; x = 0; }


	n_type_gfx c = (n_type_gfx) N_WAV_COUNT( wav );
	if ( ( x + sx ) >= c ) { sx = c - x; }


	if ( ret_x  != NULL ) { (*ret_x ) =  x; }
	if ( ret_sx != NULL ) { (*ret_sx) = sx; }


	return;
}




void
n_nyaurism_plotter_undo( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav );
	n_wav_carboncopy( &n_nyaurism_wav_undo, &n_nyaurism_wav );

	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( &n_nyaurism_wav, &n_nyaurism_wav_slider_orig );

	//n_nyaurism_plotter_selection_off();

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_mute( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
//NSLog( @"%d %d", x, sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }

	n_wav_mute( wav, x, sx );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );

	n_nyaurism_slider_redraw( wav );


	n_nyaurism_tooltip_calc( sx );


	return;
}

void
n_nyaurism_plotter_cut( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_new_by_sample( &n_nyaurism_plotter_selection_clip, sx );

	n_wav_copy( wav, &n_nyaurism_plotter_selection_clip, x,sx, 0, 1.0,1.0, N_WAV_COPY_SET );

	n_wav_delete( wav, x, sx );


	// [x] : hard to implement

	n_nyaurism_plotter_selection_reverse = n_posix_false;

	n_nyaurism_plotter_selection_from = 0;
	n_nyaurism_plotter_selection_loop = 0;
	n_nyaurism_plotter_selection_size = 0;
//NSLog( @"%d %d %d", n_nyaurism_plotter_selection_from, n_nyaurism_plotter_selection_loop, n_nyaurism_plotter_selection_size );

//NSLog( @"%d", n_nyaurism_plotter_selection_step );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_copy( n_wav *wav )
{

	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_new_by_sample( &n_nyaurism_plotter_selection_clip, sx );

	n_wav_copy( wav, &n_nyaurism_plotter_selection_clip, x,sx, 0, 1.0,1.0, N_WAV_COPY_SET );


	return;
}

void
n_nyaurism_plotter_paste( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	if ( sx != 0 )
	{
		n_wav_delete( wav, x, sx );
	}

	n_wav_insert( wav, &n_nyaurism_plotter_selection_clip, x, sx );
/*
	if ( sx == 0 )
	{
		n_wav_insert( wav, &n_nyaurism_plotter_selection_clip, x, sx );
	} else {

		int option;
		if ( n_nyaurism_mix_onoff )
		{
			option = N_WAV_COPY_ADD;
		} else {
			option = N_WAV_COPY_SET;
		}

//NSLog( @"%d", n_nyaurism_mix_onoff );
//option = N_WAV_COPY_ADD;
		n_wav_copy( &n_nyaurism_plotter_selection_clip, wav, 0,0, x, 1.0,1.0, option );
	}
*/

	n_nyaurism_plotter_selection_off();


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_overwrite( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	int option;
	if ( n_nyaurism_mix_onoff )
	{
		option = N_WAV_COPY_ADD;
	} else {
		option = N_WAV_COPY_SET;
	}

//NSLog( @"%d", n_nyaurism_mix_onoff );
//option = N_WAV_COPY_ADD;
	n_wav_copy( &n_nyaurism_plotter_selection_clip, wav, 0,sx, x, 1.0,1.0, option );


	//n_nyaurism_plotter_selection_off();

	u32 sample = sx;//N_WAV_COUNT( &n_nyaurism_plotter_selection_clip );

	n_nyaurism_plotter_selection_reverse = n_posix_false;

	n_nyaurism_plotter_selection_from = x / n_nyaurism_plotter_selection_step;
	n_nyaurism_plotter_selection_loop = 0;
	n_nyaurism_plotter_selection_size = sample / n_nyaurism_plotter_selection_step;

	n_nyaurism_tooltip_calc( sample );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_delete( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_delete( wav, x, sx );

	n_nyaurism_plotter_selection_off();


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_fade_in( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_fade_in_partial( wav, 0, x, sx, 1.0, 1.0 );

	//n_nyaurism_plotter_selection_off();


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_fade_out( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_fade_out_partial( wav, 0, x, sx, 1.0, 1.0 );

	//n_nyaurism_plotter_selection_off();


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_L2R( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }


	n_wav_L2R_partial( wav, 0, x, sx, 1.0, 1.0 );

	n_nyaurism_slider_redraw( wav );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_R2L( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }


	n_wav_R2L_partial( wav, 0, x, sx, 1.0, 1.0 );

	n_nyaurism_slider_redraw( wav );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}

void
n_nyaurism_plotter_smoother( n_wav *wav )
{

	n_wav_free( &n_nyaurism_wav_undo );
	n_wav_carboncopy( wav, &n_nyaurism_wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }


	n_wav_smoother_partial( wav, x, sx );

	n_nyaurism_slider_redraw( wav );


	n_wav_free( &n_nyaurism_wav_slider_orig );
	n_wav_carboncopy( wav, &n_nyaurism_wav_slider_orig );


	return;
}




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

	n_nyaurism_plotter_selection_step = step_x;
//NSLog( @"Step %d", step_x );


	// [!] : Main Canvas

	static BOOL init = FALSE;
	if ( init == FALSE )
	{
		init = TRUE;
		n_bmp_zero( bmp );

		n_bmp_new( &n_nyaurism_seekbar_bmp, sx, sy );
	}

	n_bmp_new( bmp, sx,sy );


	// [!] : Grid

	n_type_gfx grid_1 = (n_type_gfx) ( (n_type_real) (   10 * per_ms ) * ratio_x );
	n_type_gfx grid_2 = (n_type_gfx) ( (n_type_real) ( 1000 * per_ms ) * ratio_x );


	n_posix_bool prv = n_bmp_is_multithread;
	n_bmp_is_multithread = n_posix_true;


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

	n_type_gfx size = n_nyaurism_plotter_selection_size;

	if ( n_nyaurism_plotter_selection_shift_onoff )
	{
		n_type_gfx ty  = 0;
		n_type_gfx tsy = sy;
		if ( n_nyaurism_plotter_selection_shift_ch == 1 )
		{
			ty  = sy / 2;
			tsy = sy / 2;
		} else
		if ( n_nyaurism_plotter_selection_shift_ch == 2 )
		{
			tsy = sy / 2;
		}

		n_bmp_mixer( bmp, 0, ty, sx, tsy, cselect, 0.2 );
	} else {
		n_type_gfx from = n_nyaurism_plotter_selection_left_edge();
//NSLog( @"%d %d", from, n_nyaurism_plotter_selection_size );

		n_bmp_mixer( bmp, from, 0, size, sy, cselect, 0.2 );


		if ( ( from * n_nyaurism_plotter_selection_step ) < N_WAV_COUNT( &n_nyaurism_wav )  )
		{
			if ( from != 0 )
			{
				n_bmp_line( bmp, from, 0, from, sy, cselect );
			}
		}

		if ( size >= 2 )
		{
			n_bmp_line( bmp, from + size, 0, from + size, sy, cselect );
		}
	}


	if ( ( n_nyaurism_plotter_selection_size )||( n_nyaurism_plotter_selection_shift_onoff ) )
	{
		n_nyaurism_tooltip_draw( bmp, n_nyaurism_tooltip_str );
	}

/*
	// [x] : button : not working
	if ( n_nyaurism_is_processing )
	{
		n_nyaurism_tooltip_draw( bmp, "Processing..." );
	}
*/

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
		n_bmp_safemode = n_posix_false;

		n_bmp_transparent_onoff_default = n_posix_false;

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

		n_nyaurism_plotter_seekbar_draw( &n_nyaurism_seekbar_bmp, &n_nyaurism_wav, bmpsx, bmpsy, n_nyaurism_seekbar_float_norm );

		n_bmp bmp_canvas; n_bmp_carboncopy( &n_nyaurism_bmp, &bmp_canvas );

		n_bmp_flush_transcopy( &n_nyaurism_seekbar_bmp, &bmp_canvas );

		n_mac_image_nbmp_direct_draw( &bmp_canvas, &r, YES );

		n_bmp_free( &bmp_canvas );
	} else {
		n_nyaurism_plotter_draw( &n_nyaurism_bmp, &n_nyaurism_wav, r.size.width, r.size.height );
		n_mac_image_nbmp_direct_draw( &n_nyaurism_bmp, &r, YES );
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

		//n_fft_histogram_test( &n_nyaurism_wav );
		n_wav_filter_overdrive( &n_nyaurism_wav, 1000, 2.0 );

		[self display];
	}
	break;
#endif

	case N_MAC_KEYCODE_F2:
	{
		n_nyaurism_fname = n_mac_fork_rename( n_nyaurism_fname );

		NSString *title = [NSString stringWithFormat:@"%@ - Nyaurism", n_nyaurism_fname];
		[n_mac_image_window setTitle:title];
	}
	break;

	case N_MAC_KEYCODE_UNDO: // [!] : 'Z'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			n_nyaurism_plotter_undo( &n_nyaurism_wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_CUT: // [!] : 'X'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			n_nyaurism_plotter_cut( &n_nyaurism_wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_COPY: // [!] : 'C'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			n_nyaurism_plotter_copy( &n_nyaurism_wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_PASTE: // [!] : 'V'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			n_nyaurism_plotter_paste( &n_nyaurism_wav );
			[self display];
		}

	break;

	case 11: // [!] : 'B'

		if ( theEvent.modifierFlags & NSEventModifierFlagCommand )
		{
			n_nyaurism_plotter_overwrite( &n_nyaurism_wav );
			[self display];
		}

	break;

	case N_MAC_KEYCODE_BACKSPACE:

		n_nyaurism_plotter_delete( &n_nyaurism_wav );
		[self display];

	break;

	} // switch

}




- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog( @"mouseDown : %ld", [theEvent clickCount] );

	if ( n_nyaurism_now_playing() ) { return; }

	n_nyaurism_plotter_selection_drag_onoff = n_posix_false;

	if ( [theEvent clickCount] >= 2 )
	{
//NSLog( @"double-click" );

		n_type_gfx size;
		if ( n_nyaurism_plotter_selection_step == 1 )
		{
			size = (n_type_gfx) N_WAV_COUNT( &n_nyaurism_wav );
		} else {
			size = 512;
		}

		n_nyaurism_plotter_selection_reverse = n_posix_false;

		n_nyaurism_plotter_selection_from =    0;
		n_nyaurism_plotter_selection_loop =    0;
		n_nyaurism_plotter_selection_size = size;

		n_wav_free( &n_nyaurism_wav_slider_orig );
		n_wav_carboncopy( &n_nyaurism_wav, &n_nyaurism_wav_slider_orig );

		n_nyaurism_tooltip_calc( n_nyaurism_plotter_selection_size * n_nyaurism_plotter_selection_step );

	} else {

		if ( n_nyaurism_plotter_selection_shift_onoff )
		{

			NSUInteger flags = [[NSApp currentEvent] modifierFlags];

			NSPoint pt = n_mac_cursor_position_get( self );

			if ( flags & NSEventModifierFlagCommand )
			{
				if ( pt.y < 128 )
				{
					n_nyaurism_plotter_selection_shift_ch = 1;
				} else {
					n_nyaurism_plotter_selection_shift_ch = 2;
				}
			} else {
				n_nyaurism_plotter_selection_shift_ch = 0;
			}

 			n_nyaurism_plotter_selection_off();
			n_nyaurism_slider_redraw( &n_nyaurism_wav );

			n_nyaurism_plotter_selection_shift = pt.x;

			n_wav_free( &n_nyaurism_wav_slider_orig );
			n_wav_carboncopy( &n_nyaurism_wav, &n_nyaurism_wav_slider_orig );
		} else
		if (
			( n_nyaurism_plotter_selection_left_edge() )
			||
			( n_nyaurism_plotter_selection_size )
		)
		{
			n_nyaurism_plotter_selection_off();

			n_nyaurism_tooltip_calc( 0 );

			n_nyaurism_slider_redraw( &n_nyaurism_wav );
		} else {
			NSPoint pt = n_mac_cursor_position_get( self );
			n_nyaurism_plotter_selection_from = pt.x;

			if ( ( pt.x * n_nyaurism_plotter_selection_step ) < N_WAV_COUNT( &n_nyaurism_wav )  )
			{
				n_wav_free( &n_nyaurism_wav_slider_orig );
				n_wav_carboncopy( &n_nyaurism_wav, &n_nyaurism_wav_slider_orig );

				n_nyaurism_plotter_selection_drag_onoff = n_posix_true;
			}
		}

	}

	[self display];

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp : %ld", [theEvent clickCount] );

	if ( n_nyaurism_now_playing() ) { return; }

	if ( n_nyaurism_plotter_selection_drag_onoff )
	{
		n_nyaurism_plotter_selection_drag_onoff = n_posix_false;


		NSPoint pt = n_mac_cursor_position_get( self );

		if ( pt.x < n_nyaurism_plotter_selection_from )
		{
			n_nyaurism_plotter_selection_from = n_nyaurism_plotter_selection_loop;
		}

		n_nyaurism_slider_redraw( &n_nyaurism_wav );

		[self display];
	}

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );

	if ( n_nyaurism_now_playing() ) { return; }


	NSPoint pt = n_mac_cursor_position_get( self );

	if ( n_nyaurism_plotter_selection_shift_onoff )
	{
		n_type_gfx x = n_nyaurism_plotter_selection_shift - pt.x;
//NSLog( @"%d", x );
		x *= n_nyaurism_plotter_selection_step;
		x *= -1;

		n_nyaurism_tooltip_calc( x );

		BOOL l = TRUE; if ( n_nyaurism_plotter_selection_shift_ch == 2 ) { l = FALSE; }
		BOOL r = TRUE; if ( n_nyaurism_plotter_selection_shift_ch == 1 ) { r = FALSE; }

		n_nyaurism_shiftcopy( &n_nyaurism_wav_slider_orig, &n_nyaurism_wav, x, l, r );
	} else
	if ( n_nyaurism_plotter_selection_drag_onoff )
	{
		if ( pt.x < 0 ) { pt.x = 0; }

		if ( ( n_nyaurism_plotter_selection_step == 1 )&&( pt.x > (n_type_gfx) N_WAV_COUNT( &n_nyaurism_wav ) ) )
		{
			n_nyaurism_plotter_selection_size = (n_type_gfx) N_WAV_COUNT( &n_nyaurism_wav ) - n_nyaurism_plotter_selection_left_edge();
		} else
		if ( pt.x < n_nyaurism_plotter_selection_from )
		{
			n_nyaurism_plotter_selection_reverse =  n_posix_true;

			n_nyaurism_plotter_selection_size = n_nyaurism_plotter_selection_from - pt.x;
			n_nyaurism_plotter_selection_loop = pt.x;
		} else {
			n_nyaurism_plotter_selection_reverse = n_posix_false;

			n_type_gfx size = 512 - n_nyaurism_plotter_selection_left_edge();

			n_nyaurism_plotter_selection_size = n_posix_min_n_type_gfx( size, pt.x - n_nyaurism_plotter_selection_left_edge() );
		}

		n_nyaurism_tooltip_calc( n_nyaurism_plotter_selection_size * n_nyaurism_plotter_selection_step );
	}

	[self display];

}
/*
-(void)n_timer_method
{
	if ( FALSE == n_mac_window_is_hovered( self ) )
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


	if ( n_nyaurism_plotter_selection_shift_onoff )
	{

		NSUInteger flags = [[NSApp currentEvent] modifierFlags];

		if ( flags & NSEventModifierFlagCommand )
		{
			NSPoint pt = n_mac_cursor_position_get( self );
			if ( pt.y < 128 )
			{
				n_nyaurism_plotter_selection_shift_ch = 1;
			} else {
				n_nyaurism_plotter_selection_shift_ch = 2;
			}
		} else {
			n_nyaurism_plotter_selection_shift_ch = 0;
		}

		[self display];

	}

}

-(void)flagsChanged:(NSEvent *)event
{
//NSLog( @"flagsChanged" );

	if ( n_nyaurism_now_playing() ) { return; }


	NSUInteger flags = [event modifierFlags];

	if ( flags & NSEventModifierFlagShift )
	{
//NSLog( @"shift on" );
		n_nyaurism_plotter_selection_shift_onoff = n_posix_true;
		[self mouseMoved:event];
	} else
	if ( n_nyaurism_plotter_selection_shift_onoff )
	{
//NSLog( @"shift off" );
		n_nyaurism_plotter_selection_shift_onoff = n_posix_false;

		n_nyaurism_tooltip_calc( 0 );

		n_wav_free( &n_nyaurism_wav_slider_orig );
		n_wav_carboncopy( &n_nyaurism_wav, &n_nyaurism_wav_slider_orig );

		[self display];
	}


}




- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog(@"rightMouseUp");

	[NSMenu popUpContextMenu:_n_popup_menu withEvent:theEvent forView:self];

}


@end
