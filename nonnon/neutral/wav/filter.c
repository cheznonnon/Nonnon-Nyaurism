// Nonnon Nyaurism
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV_FILTER
#define _H_NONNON_NEUTRAL_WAV_FILTER




#include "../random.c"
#include "../wav.c"


#include "./_error.c"
#include "./_piano.c"
#include "./sample.c"




n_posix_bool
n_wav_sample_is_accessible( n_wav *wav, u32 x )
{
	if ( ( x >= 0 )&&( x < N_WAV_COUNT( wav ) ) ) { return n_posix_true; }

	return n_posix_false;
}




void
n_wav_peak_value( n_wav *wav, u32 x, u32 sx, n_type_real *ret_l, n_type_real *ret_r )
{

	n_type_real hi_l = 0.0;
	n_type_real hi_r = 0.0;

#ifdef N_POSIX_PLATFORM_MAC

	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	u32 cores = n_posix_cpu_count(); if ( sx < cores ) { cores = 1; }
	u32 byte  = ( cores + 1 ) * sizeof( n_type_real );

	__block n_type_real *hi_l_mt = n_memory_new_closed( byte );
	__block n_type_real *hi_r_mt = n_memory_new_closed( byte );

	n_memory_zero( hi_l_mt, byte );
	n_memory_zero( hi_r_mt, byte );

	u32 t = sx / cores;

	u32 i = 0;
	n_posix_loop
	{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			u32 f = 0;
			n_posix_loop
			{
				if ( f >= t ) { break; }

				u32 xx = x + f + ( t * i );

				if ( n_wav_sample_is_accessible( wav, xx ) )
				{
					n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

					l = fabs( l );
					r = fabs( r );

					if ( hi_l_mt[ i ] < l ) { hi_l_mt[ i ] = l; }
					if ( hi_r_mt[ i ] < r ) { hi_r_mt[ i ] = r; }
				}

				f++;
			}

		}];
		[n_wav_queue addOperation:o];

		i++;
		if ( i >= cores ) { break; }
	}


	[n_wav_queue waitUntilAllOperationsAreFinished];

	// [Needed] : rest
	{
		u32 f = t * i;
		n_posix_loop
		{
			if ( f >= sx ) { break; }

			u32 xx = x + f;

			if ( n_wav_sample_is_accessible( wav, xx ) )
			{
				n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

				l = fabs( l );
				r = fabs( r );

				if ( hi_l_mt[ i ] < l ) { hi_l_mt[ i ] = l; }
				if ( hi_r_mt[ i ] < r ) { hi_r_mt[ i ] = r; }
			}

			f++;
		}
	}


	i = 0;
	n_posix_loop
	{

		hi_l = n_posix_max_n_type_real( hi_l, hi_l_mt[ i ] );
		hi_r = n_posix_max_n_type_real( hi_r, hi_r_mt[ i ] );

		i++;
		if ( i >= cores ) { break; }
	}

	n_memory_free_closed( hi_l_mt );
	n_memory_free_closed( hi_r_mt );

#else

	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

			l = fabs( l );
			r = fabs( r );

			if ( hi_l < l ) { hi_l = l; }
			if ( hi_r < r ) { hi_r = r; }
		}

		f++;

	}

#endif

	if ( ret_l != NULL ) { (*ret_l) = hi_l; }
	if ( ret_r != NULL ) { (*ret_r) = hi_r; }


	return;
}




#define n_wav_cosine( w, hz, r_l, r_r ) n_wav_cosine_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_cosine_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
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
			n_type_real d = n_wav_sample_cosine( wav, hz, xx );
			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_sine( w, hz, r_l, r_r ) n_wav_sine_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_sine_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
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
			n_type_real d = n_wav_sample_sine( wav, hz, xx );
			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_sawtooth( w, hz, r_l, r_r ) n_wav_sawtooth_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_sawtooth_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
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
			n_type_real d = n_wav_sample_sawtooth( wav, hz, xx );
			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_square( w, hz, r_l, r_r ) n_wav_square_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_square_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
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
			n_type_real d = n_wav_sample_square( wav, hz, xx );
			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_sandstorm( w, hz, r_l, r_r ) n_wav_sandstorm_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_sandstorm_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : hz : random parameter

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	n_random_shuffle();


	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real d = n_wav_sample_sandstorm( wav, hz, xx );
			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_pinknoise( w, hz, r_l, r_r ) n_wav_pinknoise_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_pinknoise_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	n_random_shuffle();


	// [!] : this code comes from Audacity's source code

	n_type_real buf0 = 1, buf1 = 1, buf2 = 1, buf3 = 1, buf4 = 1, buf5 = 1, buf6 = 1;

	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{

			n_type_real white = ( rand() / ( ( (n_type_real) RAND_MAX ) / 2.0 ) ) - 1.0;

			buf0 = (  0.99886 * buf0 ) + ( 0.0555179 * white );
			buf1 = (  0.99332 * buf1 ) + ( 0.0750759 * white );
			buf2 = (  0.96900 * buf2 ) + ( 0.1538520 * white );
			buf3 = (  0.86650 * buf3 ) + ( 0.3104856 * white );
			buf4 = (  0.55000 * buf4 ) + ( 0.5329522 * white );
			buf5 = ( -0.76160 * buf5 ) - ( 0.0168980 * white );

			n_type_real d = buf0 + buf1 + buf2 + buf3 + buf4 + buf5 + buf6 + ( white * 0.5362 );

			if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) ) { d *= SHRT_MAX; }
			d *= 0.025;

			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );

			buf6 = white * 0.115926;

		}


		f++;

	}


	return;
}

#define n_wav_fade_in( w, hz, r_l, r_r ) n_wav_fade_in_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_fade_in_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }

	sx *= 2;

	u32 f = 0;
	u32 t = sx / 2;
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

#define n_wav_fade_out( w, hz, r_l, r_r ) n_wav_fade_out_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_fade_out_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }

	sx *= 2;

	u32 f = 0;
	u32 t = sx / 2;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r, d;
			n_wav_sample_get( wav, xx, &l, &r );

			d = (n_type_real) f / sx / 2;
			l = l * fabs( cos( N_WAV_2PI * d ) );
			r = r * fabs( cos( N_WAV_2PI * d ) );

			n_wav_sample_mix( wav, xx, l, r, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_monaural( w, hz, r_l, r_r ) n_wav_monaural_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_monaural_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

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
			n_type_real l,r;

			n_wav_sample_get( wav, xx, &l, &r );

			l = r = ( l + r ) / 2;

			n_wav_sample_mix( wav, xx, l, r, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}

#define n_wav_L2R( w, hz, r_l, r_r ) n_wav_L2R_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_L2R_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" "ratio_l" "ratio_r" are not used

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
			n_type_real l,r;

			n_wav_sample_get( wav, xx, &l, &r );

			n_wav_sample_set( wav, xx, l, l );
		}

		f++;

	}


	return;
}

#define n_wav_R2L( w, hz, r_l, r_r ) n_wav_R2L_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_R2L_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" "ratio_l" "ratio_r" are not used

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
			n_type_real l,r;

			n_wav_sample_get( wav, xx, &l, &r );

			n_wav_sample_set( wav, xx, r, r );
		}

		f++;

	}


	return;
}

#define n_wav_tremolo( w, hz, r_l, r_r ) n_wav_tremolo_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_tremolo_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
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
			n_type_real d = n_wav_sample_sine_coeff( wav, hz, xx );

			n_type_real l,r;
			n_wav_sample_get( wav, x + f, &l, &r );

			if ( ratio_l != 0 ) { l = d * l * ratio_l; }
			if ( ratio_r != 0 ) { r = d * r * ratio_r; }

			n_wav_sample_set( wav, xx, l, r );
		}

		f++;

	}


	return;
}

#define n_wav_distortion( w, hz, r_l, r_r ) n_wav_distortion_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_distortion_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	// Phase 1 : get peak value

	n_type_real hi_l = 0.0;
	n_type_real hi_r = 0.0;

	n_wav_peak_value( wav, x, sx, &hi_l, &hi_r );


	// Phase 2 : apply

	hi_l *= 1.0 - ratio_l;
	hi_r *= 1.0 - ratio_r;

	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

			if ( fabs( l ) >= hi_l ) { if ( l > 0 ) { l = hi_l; } else { l = -hi_l; } }
			if ( fabs( r ) >= hi_r ) { if ( r > 0 ) { r = hi_r; } else { r = -hi_r; } }

			n_wav_sample_set( wav, xx, l, r );
		}

		f++;

	}


	return;
}

#define n_wav_normalize( w, hz, r_l, r_r ) n_wav_normalize_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_normalize_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	// Phase 1 : get peak value

	n_type_real hi_l = 0.0;
	n_type_real hi_r = 0.0;

	n_wav_peak_value( wav, x, sx, &hi_l, &hi_r );


	// Phase 2 : apply

	n_type_real a_l = (n_type_real) n_wav_sample_amp( wav ) * ratio_l;
	n_type_real a_r = (n_type_real) n_wav_sample_amp( wav ) * ratio_r;

#ifdef N_POSIX_PLATFORM_MAC

	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	u32 cores = n_posix_cpu_count(); if ( sx < cores ) { cores = 1; }

	u32 t = sx / cores;


	u32 i = 0;
	n_posix_loop
	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			u32 f = 0;
			n_posix_loop
			{
				if ( f >= t ) { break; }

				u32 xx = x + f + ( t * i );

				if ( n_wav_sample_is_accessible( wav, xx ) )
				{
					n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

					if ( hi_l != 0 ) { l = a_l * ( l / hi_l ); }
					if ( hi_r != 0 ) { r = a_r * ( r / hi_r ); }

					n_wav_sample_set( wav, xx, l, r );
				}

				f++;
			}

		}];
		[n_wav_queue addOperation:o];

		i++;
		if ( i >= cores ) { break; }
	}

	[n_wav_queue waitUntilAllOperationsAreFinished];

	// [Needed] : rest
	{
		u32 f = t * i;
		n_posix_loop
		{
			if ( f >= sx ) { break; }

			u32 xx = x + f;

			if ( n_wav_sample_is_accessible( wav, xx ) )
			{
				n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

				if ( hi_l != 0 ) { l = a_l * ( l / hi_l ); }
				if ( hi_r != 0 ) { r = a_r * ( r / hi_r ); }

				n_wav_sample_set( wav, xx, l, r );
			}

			f++;
		}
	}

#else

	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real l,r; n_wav_sample_get( wav, xx, &l, &r );

			if ( hi_l != 0 ) { l = a_l * ( l / hi_l ); }
			if ( hi_r != 0 ) { r = a_r * ( r / hi_r ); }

			n_wav_sample_set( wav, xx, l, r );
		}

		f++;

	}

#endif


	return;
}

#define n_wav_marsian( w, hz, r_l, r_r ) n_wav_marsian_partial( w, hz, 0, N_WAV_COUNT( w ), r_l, r_r )

void
n_wav_marsian_partial( n_wav *wav, n_type_real hz, u32 x, u32 sx, n_type_real ratio_l, n_type_real ratio_r )
{

	// [!] : extra-terrestrial chatter

	// [!] : "hz" is not used

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	const u32 unit = (u32) ( (n_type_real) 44100 * 0.125 );


	n_random_shuffle();


	u32 i = 0;
	u32 j = 0;
	u32 f = 0;
	u32 t = sx;
	n_posix_loop
	{

		if ( f >= t ) { break; }

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_type_real d = n_wav_sample_sine( wav, n_wav_piano[ i ], xx );

			j++;
			if ( j >= unit ) { i = n_random_range( N_WAV_PIANO_MAX ); j = 0; }

			n_wav_sample_mix( wav, xx, d, d, ratio_l, ratio_r );
		}

		f++;

	}


	return;
}




#define n_wav_smoother( w ) n_wav_smoother_partial( w, 0, N_WAV_COUNT( w ) )

void
n_wav_smoother_partial( n_wav *wav, u32 x, u32 sx )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	// [!] : Thx : DeepSeek AI

	// [!] : Savitzky-Golay Filter

	int window_size = 9;
	int poly_order  = 3;
/*
	static const double coeffs_5_2[] = {
		-3.0/35.0, 12.0/35.0, 17.0/35.0, 
		12.0/35.0, -3.0/35.0
	};

	static const double coeffs_7_2[] = {
		-2.0/21.0, 3.0/21.0, 6.0/21.0, 7.0/21.0,
		6.0/21.0, 3.0/21.0, -2.0/21.0
	};
*/
	static const double coeffs_9_3[] = {
		-21.0/231.0, 14.0/231.0, 39.0/231.0, 
		54.0/231.0, 59.0/231.0, 54.0/231.0,
		39.0/231.0, 14.0/231.0, -21.0/231.0
	};

	const double *coeffs;
	         int  half_window;
/*
	if ( ( window_size == 5 )&&( poly_order == 2 ) )
	{
		coeffs      = coeffs_5_2;
		half_window = 2;
	} else
	if ( ( window_size == 7 )&&( poly_order == 2 ) )
	{
		coeffs      = coeffs_7_2;
		half_window = 3;
	} else
*/
	if ( ( window_size == 9 )&&( poly_order == 3 ) )
	{
		coeffs      = coeffs_9_3;
		half_window = 4;
	} else {
		return;
	}


	u32 count = sx;


	double *input_l = (double*) n_memory_new_closed( count * sizeof( double ) );
	double *input_r = (double*) n_memory_new_closed( count * sizeof( double ) );

	u32 j = x * 2;
	for ( int i = 0; i < count; i++ )
	{
		if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
		{
			s16 *p = (s16*) N_WAV_PTR( wav );

			input_l[ i ] = (double) p[ j + 0 ] / SHRT_MAX;
			input_r[ i ] = (double) p[ j + 1 ] / SHRT_MAX;
		} else {
			float *p = (float*) N_WAV_PTR( wav );

			input_l[ i ] = (double) p[ j + 0 ];
			input_r[ i ] = (double) p[ j + 1 ];
		}
		j += 2;
	}


	double *output_l = (double*) n_memory_new_closed( count * sizeof( double ) );
	double *output_r = (double*) n_memory_new_closed( count * sizeof( double ) );

	for ( int i = 0; i < count; i++ )
	{
		output_l[ i ] = 0.0;
		output_r[ i ] = 0.0;

		for ( int j = -half_window; j <= half_window; j++ )
		{
			int idx = i + j;
			if ( idx < 0 )
			{
				idx = -idx;
			} else
			if ( idx >= count )
			{
				idx = 2 * count - idx - 2;
			}

			output_l[ i ] += coeffs[ j + half_window ] * input_l[ idx ];
			output_r[ i ] += coeffs[ j + half_window ] * input_r[ idx ];
		}
	}


	j = x * 2;
	for ( int i = 0; i < count; i++ )
	{
		if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
		{
			s16 *p = (s16*) N_WAV_PTR( wav );

			p[ j + 0 ] = output_l[ i ] * SHRT_MAX;
			p[ j + 1 ] = output_r[ i ] * SHRT_MAX;
		} else {
			float *p = (float*) N_WAV_PTR( wav );

			p[ j + 0 ] = (float) output_l[ i ];
			p[ j + 1 ] = (float) output_r[ i ];
		}
		j += 2;
	}

	n_memory_free_closed( input_l );
	n_memory_free_closed( input_r );

	n_memory_free_closed( output_l );
	n_memory_free_closed( output_r );


	return;
}




// internal
void
n_wav_overdrive_channel( double *audio, int count )
{

	const int    factor = 1;
	const double drive  = 1.5;


	int     oversampled_len = count * factor;
	double *oversampled     = n_memory_new_closed( oversampled_len * sizeof( double ) );


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

		oversampled[ i ] = x;
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


	n_memory_free_closed( oversampled );


	return;
}

#define n_wav_overdrive( w ) n_wav_overdrive_partial( w, 0, N_WAV_COUNT( w ) )

void
n_wav_overdrive_partial( n_wav *wav, u32 x, u32 sx )
{

	int count = sx;
	int end   = x + sx;


	double *audio_l = (double*) n_memory_new_closed( count * sizeof( double ) );
	double *audio_r = (double*) n_memory_new_closed( count * sizeof( double ) );


	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );
		for( int i = x; i < end; i++ )
		{
			audio_l[ i ] = (double) ptr[ ( i * 2 ) + 0 ] / SHRT_MAX;
			audio_r[ i ] = (double) ptr[ ( i * 2 ) + 1 ] / SHRT_MAX;
		}
	} else {
		float *ptr = (float*) N_WAV_PTR( wav );
		for( int i = x; i < end; i++ )
		{
			audio_l[ i ] = (double) ptr[ ( i * 2 ) + 0 ];
			audio_r[ i ] = (double) ptr[ ( i * 2 ) + 1 ];
		}
	}


	n_wav_overdrive_channel( audio_l, count );
	n_wav_overdrive_channel( audio_r, count );


	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );
		for( int i = x; i < end; i++ )
		{
			ptr[ ( i * 2 ) + 0 ] = (float) audio_l[ i ] * SHRT_MAX;
			ptr[ ( i * 2 ) + 1 ] = (float) audio_r[ i ] * SHRT_MAX;
		}
	} else {
		float *ptr = (float*) N_WAV_PTR( wav );
		for( int i = x; i < end; i++ )
		{
			ptr[ ( i * 2 ) + 0 ] = (float) audio_l[ i ];
			ptr[ ( i * 2 ) + 1 ] = (float) audio_r[ i ];
		}
	}


	n_memory_free_closed( audio_l );
	n_memory_free_closed( audio_r );


	return;
}




void
n_wav_mute( n_wav *wav, u32 x, u32 sx )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	u32 f = 0;
	n_posix_loop
	{//break;

		u32 xx = x + f;

		if ( n_wav_sample_is_accessible( wav, xx ) )
		{
			n_wav_sample_set( wav, xx, 0, 0 );
		}

		f++;
		if ( f >= sx ) { break; }
	}


	return;
}

void
n_wav_delete( n_wav *wav, u32 x, u32 sx )
{

	if ( n_wav_error_format( wav ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	if ( sx == 0 ) { return; }


	n_wav ret; n_wav_zero( &ret ); n_wav_new_by_sample( &ret, N_WAV_COUNT( wav ) - sx );

	u32 i = 0;
	u32 j = 0;
	n_posix_loop
	{//break;

		if ( i >= x ) { break; }

		if ( i >= N_WAV_COUNT(  wav ) ) { break; }
		if ( j >= N_WAV_COUNT( &ret ) ) { break; }

		if (
			( n_wav_sample_is_accessible(  wav, i ) )
			&&
			( n_wav_sample_is_accessible( &ret, j ) )
		)
		{
			n_type_real l,r;
			n_wav_sample_get(  wav, i, &l, &r );
			n_wav_sample_set( &ret, j,  l,  r );
		}

		i++;
		j++;

	}

	i = 0;
	n_posix_loop
	{//break;

		u32 pos = x + sx + i;
		if ( pos >= N_WAV_COUNT(  wav ) ) { break; }
		if (   j >= N_WAV_COUNT( &ret ) ) { break; }

		if (
			( n_wav_sample_is_accessible(  wav, pos ) )
			&&
			( n_wav_sample_is_accessible( &ret,   j ) )
		)
		{
			n_type_real l,r;
			n_wav_sample_get(  wav, pos, &l, &r );
			n_wav_sample_set( &ret,   j,  l,  r );
		}

		i++;
		j++;

	}

	n_wav_free( wav );
	n_wav_alias( &ret, wav );


	return;
}

void
n_wav_insert( n_wav *wav, n_wav *ins, u32 x, u32 sx )
{

	if ( n_wav_error_format( wav ) ) { return; }
	if ( n_wav_error_format( ins ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( wav, x ) ) { return; }


	n_wav ret; n_wav_zero( &ret ); n_wav_new_by_sample( &ret, N_WAV_COUNT( wav ) + N_WAV_COUNT( ins ) - sx );


	u32 i = 0;
	u32 j = 0;
	n_posix_loop
	{//break;

		if ( i >= x ) { break; }

		if ( i >= N_WAV_COUNT(  wav ) ) { break; }
		if ( j >= N_WAV_COUNT( &ret ) ) { break; }

		if (
			( n_wav_sample_is_accessible(  wav, i ) )
			&&
			( n_wav_sample_is_accessible( &ret, j ) )
		)
		{
			n_type_real l,r;
			n_wav_sample_get(  wav, i, &l, &r );
			n_wav_sample_set( &ret, j,  l,  r );
		}

		i++;
		j++;

	}

	i = 0;
	n_posix_loop
	{//break;

		if ( i >= N_WAV_COUNT(  ins ) ) { break; }
		if ( j >= N_WAV_COUNT( &ret ) ) { break; }

		if (
			( n_wav_sample_is_accessible(  ins, i ) )
			&&
			( n_wav_sample_is_accessible( &ret, j ) )
		)
		{
			n_type_real l,r;
			n_wav_sample_get(  ins, i, &l, &r );
			n_wav_sample_set( &ret, j,  l,  r );
		}

		i++;
		j++;

	}

	i = x + sx;
	n_posix_loop
	{//break;

		if ( i >= N_WAV_COUNT(  wav ) ) { break; }
		if ( j >= N_WAV_COUNT( &ret ) ) { break; }

		if (
			( n_wav_sample_is_accessible(  wav, i ) )
			&&
			( n_wav_sample_is_accessible( &ret, j ) )
		)
		{
			n_type_real l,r;
			n_wav_sample_get(  wav, i, &l, &r );
			n_wav_sample_set( &ret, j,  l,  r );
		}

		i++;
		j++;

	}


	n_wav_free( wav );
	n_wav_alias( &ret, wav );


	return;
}




#define N_WAV_COPY_SET 0
#define N_WAV_COPY_MIX 1
#define N_WAV_COPY_ADD 2

#define n_wav_set( f,t, l,r ) n_wav_copy( f,t, 0,0, 0, l,r, N_WAV_COPY_SET )
#define n_wav_mix( f,t, l,r ) n_wav_copy( f,t, 0,0, 0, l,r, N_WAV_COPY_MIX )
#define n_wav_add( f,t, l,r ) n_wav_copy( f,t, 0,0, 0, l,r, N_WAV_COPY_ADD )

void
n_wav_copy( n_wav *f, n_wav *t, u32 fx, u32 sx, u32 tx, n_type_real ratio_l, n_type_real ratio_r, int mode )
{

	if ( n_wav_error_format( f ) ) { return; }
	if ( n_wav_error_format( t ) ) { return; }

	if ( n_posix_false == n_wav_sample_is_accessible( f, fx ) ) { return; }
	if ( n_posix_false == n_wav_sample_is_accessible( t, tx ) ) { return; }


	u32 count_f = N_WAV_COUNT( f );
	u32 count_t = N_WAV_COUNT( t );

	if ( sx == 0 ) { sx = count_f; }

	if ( fx >= count_f ) { return; }
	if ( fx <        0 ) { sx += fx; tx += ( fx * -1 ); fx = 0; }

	if ( tx >= count_t ) { return; }
	if ( tx <        0 ) { sx += tx; fx += ( fx * -1 ); tx = 0; }

	if ( sx <=       0 ) { return; }


	n_type_real l, r;


	u32 x = 0;
	n_posix_loop
	{//break;

		if ( ( fx + x ) >= count_f ) { break; }
		if ( ( tx + x ) >= count_t ) { break; }


		n_wav_sample_get( f, fx + x, &l, &r );

		if ( mode == N_WAV_COPY_SET )
		{
			n_wav_sample_set( t, tx + x, l * ratio_l, r * ratio_r );
		} else
		if ( mode == N_WAV_COPY_MIX )
		{
			n_wav_sample_mix( t, tx + x, l, r, ratio_l, ratio_r );
		} else
		if ( mode == N_WAV_COPY_ADD )
		{
			n_wav_sample_add( t, tx + x, l * ratio_l, r * ratio_r );
		}// else


		x++;
		if ( x >= sx ) { break; }
	}


	return;
}




#endif // _H_NONNON_NEUTRAL_WAV_FILTER

