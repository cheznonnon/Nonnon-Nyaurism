// Nonnon Nyaurism for Mac
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




void
n_nyaurism_plotter_undo( n_wav *wav )
{

	n_nyaurism_backup( &n_nyaurism->wav_undo, &n_nyaurism->wav      );
	n_nyaurism_backup( &n_nyaurism->wav_undo, &n_nyaurism->wav_base );

	//n_nyaurism_plotter_selection_off();

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_select_all( n_wav *wav, BOOL tip )
{

	n_nyaurism->selection_select_all = TRUE;

	n_nyaurism->selection_reverse = FALSE;

	n_nyaurism->selection_from_pixel = 0;
	n_nyaurism->selection_loop_pixel = 0;
	n_nyaurism->selection_size_pixel = N_WAV_COUNT( wav ) / n_nyaurism->selection_step;

	if ( tip )
	{
		n_nyaurism_tooltip_calc( n_nyaurism->selection_size_pixel );
	}

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_mute( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
//NSLog( @"%d %d", x, sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }


	BOOL l = TRUE;
	BOOL r = TRUE;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			r = FALSE;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			l = FALSE;
		}
	}


	n_wav_mute_partial( wav, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_cut( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_new_by_sample( &n_nyaurism->wav_clip, sx );

	n_wav_copy_set( wav, &n_nyaurism->wav_clip, x,sx, 0 );

	n_wav_delete( wav, x, sx );


	// [x] : hard to implement

	n_nyaurism->selection_reverse = FALSE;

	n_nyaurism->selection_from_pixel = 0;
	n_nyaurism->selection_loop_pixel = 0;
	n_nyaurism->selection_size_pixel = 0;
//NSLog( @"%d %d %d", n_nyaurism->selection_from_pixel, n_nyaurism->selection_loop_pixel, n_nyaurism->selection_size_pixel );

//NSLog( @"%d", n_nyaurism_plotter_selection_step );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );


	return;
}

void
n_nyaurism_plotter_copy( n_wav *wav, n_wav *wav_clip )
{

	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_new_by_sample( &n_nyaurism->wav_clip, sx );

	n_wav_copy_set( wav, wav_clip, x,sx, 0 );


	return;
}

void
n_nyaurism_plotter_paste( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	if ( sx != 0 )
	{
		n_wav_delete( wav, x, sx );
	}

	n_wav_insert( wav, &n_nyaurism->wav_clip, x, sx );


	n_nyaurism_plotter_selection_off();


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_overwrite( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );


	if ( n_nyaurism->mix_onoff )
	{
		n_wav_copy_add( &n_nyaurism->wav_clip, wav, 0,sx, x );
	} else {
		n_wav_copy_set( &n_nyaurism->wav_clip, wav, 0,sx, x );
	}


	//n_nyaurism_plotter_selection_off();

	u32 sample = sx;//N_WAV_COUNT( &n_nyaurism->wav_clip );

	n_nyaurism->selection_reverse = FALSE;

	n_nyaurism->selection_from_pixel =      x / n_nyaurism->selection_step;
	n_nyaurism->selection_loop_pixel = 0;
	n_nyaurism->selection_size_pixel = sample / n_nyaurism->selection_step;


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_delete( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );

	n_wav_delete( wav, x, sx );

	n_nyaurism_plotter_selection_off();


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_fade_in( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_type_real l = 1.0;
	n_type_real r = 1.0;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = 0.0;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = 0.0;
		}
	}

	n_wav_fade_in_partial( wav, x, sx, l, r );

	//n_nyaurism_plotter_selection_off();


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_fade_out( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_type_real l = 1.0;
	n_type_real r = 1.0;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = 0.0;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = 0.0;
		}
	}

	n_wav_fade_out_partial( wav, x, sx, l, r );

	//n_nyaurism_plotter_selection_off();


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_monoaural( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	// [!] : L/R : not supported by spec

	n_wav_monaural_partial( wav, x, sx, 1.0, 1.0 );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_L2R( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	// [!] : L/R : not supported by spec

	n_wav_L2R_partial( wav, x, sx );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_R2L( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	// [!] : L/R : not supported by spec

	n_wav_R2L_partial( wav, x, sx );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_smoother( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	BOOL l = TRUE;
	BOOL r = TRUE;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = FALSE;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = FALSE;
		}
	}

	n_wav_smoother_partial( wav, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_overdrive( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	BOOL l = TRUE;
	BOOL r = TRUE;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = FALSE;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = FALSE;
		}
	}

	n_wav_overdrive_partial( wav, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_tremolo( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_type_real l = 1.0;
	n_type_real r = 1.0;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = 0.0;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = 0.0;
		}
	}

	n_type_real hz = 3.0;

	n_wav_tremolo_partial( wav, hz, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_whitenoise( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_type_real l = 1.0;
	n_type_real r = 1.0;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = 0.0;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = 0.0;
		}
	}

	n_wav_whitenoise_partial( wav, 44100, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

void
n_nyaurism_plotter_pinknoise( n_wav *wav )
{

	n_nyaurism_backup( wav, &n_nyaurism->wav_undo );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_type_real l = 1.0;
	n_type_real r = 1.0;

	if ( n_nyaurism->selection_shift_onoff )
	{
		if ( n_nyaurism->plotter_selection_channel == 1 )
		{
			l = 0.0;
		} else
		if ( n_nyaurism->plotter_selection_channel == 2 )
		{
			r = 0.0;
		}
	}

	n_wav_pinknoise_partial( wav, 0, x, sx, l, r );


	n_nyaurism_backup( wav, &n_nyaurism->wav_base );

	n_nyaurism_slider_redraw( wav );


	return;
}

