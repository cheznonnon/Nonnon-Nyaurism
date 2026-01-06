// Nonnon Nyaurism
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV_DEPRECATED
#define _H_NONNON_NEUTRAL_WAV_DEPRECATED




#define n_wav_fade( w, hz, r_l, r_r ) n_wav_fade_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_fade_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r, d;
			n_wav_sample_get( wav, xx, &l, &r );

			d = (n_type_real) f / sx / 2;
			l = l * fabs( sin( N_WAV_2PI * d ) );
			r = r * fabs( sin( N_WAV_2PI * d ) );

			n_wav_sample_mix( wav, xx, l, r, ratio_l, ratio_r );
		}


		f++;

	}


	return;
}

#define n_wav_delay( w, hz, r_l, r_r ) n_wav_delay_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_delay_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	u32 offset = (u32) ( N_WAV_RATE( wav ) / hz );
	if ( offset >= sx ) { return; }


	n_wav w; n_wav_carboncopy( wav, &w );


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r;
			n_wav_sample_get( &w, xx, &l, &r );

			if ( n_wav_sample_is_accessible( wav, xx + offset ) )
			{
				n_wav_sample_mix( wav, xx + offset, l, r, ratio_l, ratio_r );
			}
		}

		f++;

	}


	n_wav_free( &w );


	return;
}




#define n_wav_feedback_left( w, hz, r_l, r_r ) n_wav_feedback_left_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_feedback_left_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	u32 offset = (u32) ( N_WAV_RATE( wav ) / hz );
	if ( offset >= sx ) { return; }


	n_wav w; n_wav_carboncopy( wav, &w );


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r;

			if ( n_wav_sample_is_accessible( &w, xx + offset ) )
			{
				n_wav_sample_get( &w, xx + offset, &l, &r );
			} else {
				l = r = 0;
			}

			n_wav_sample_mix( wav, xx, l, r, ratio_l, ratio_r );
		}

		f++;

	}


	n_wav_free( &w );


	return;
}

#define n_wav_feedback_right( w, hz, r_l, r_r ) n_wav_feedback_right_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_feedback_right_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	u32 offset = (u32) ( N_WAV_RATE( wav ) / hz );
	if ( offset >= sx ) { return; }


	n_wav w; n_wav_carboncopy( wav, &w );


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r;

			if ( n_wav_sample_is_accessible( &w, xx - offset ) )
			{
				n_wav_sample_get( &w, xx - offset, &l, &r );
			} else {
				l = r = 0;
			}

			n_wav_sample_mix( wav, xx, l, r, ratio_l, ratio_r );
		}

		f++;

	}


	n_wav_free( &w );


	return;
}




#define n_wav_tone_up( w, hz, r_l, r_r ) n_wav_tone_up_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_tone_up_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	const u32 turn = sx / 2;


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + ( ( xx ) > turn ) ], xx );

			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_tone_down( w, hz, r_l, r_r ) n_wav_tone_down_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_tone_down_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	const u32 turn = sx / 2;


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + ( ( xx ) < turn ) ], xx );

			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_tone_updown( w, hz, r_l, r_r ) n_wav_tone_updown_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_tone_updown_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	const u32 quarter = sx / 4;


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 p = x + f;
		n_type_real d;

		if ( ( p > quarter )&&( p < ( sx - quarter ) ) )
		{
			d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + 1 ], p );
		} else {
			d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + 0 ], p );
		}

		n_wav_sample_mix( wav, p, d, d, ratio_l, ratio_r );

		f++;

	}


	return;
}

#define n_wav_tone_downup( w, hz, r_l, r_r ) n_wav_tone_downup_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_tone_downup_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	const u32 quarter = sx / 4;


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 p = x + f;
		n_type_real d;

		if ( ( p > quarter )&&( p < ( sx - quarter ) ) )
		{
			d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + 0 ], p );
		} else {
			d = n_wav_sample_sine( wav, n_wav_piano[ N_WAV_PIANO_MIDDLE + 1 ], p );
		}

		n_wav_sample_mix( wav, p, d, d, ratio_l, ratio_r );

		f++;

	}


	return;
}




#endif // _H_NONNON_NEUTRAL_WAV_DEPRECATED

