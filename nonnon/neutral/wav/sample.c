// Nonnon Wave
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV_SAMPLE
#define _H_NONNON_NEUTRAL_WAV_SAMPLE




#include "../wav.c"

#include "../random.c"


#include "./_error.c"




#include <complex.h>




#define N_WAV_2PI ( 2.0 * M_PI )




float
n_wav_sample_clamp_normalized( float f )
{

	if ( f >  1.0 )
	{
		f =  1.0;
	} else
	if ( f < -1.0 )
	{
		f = -1.0;
	}

	return f;
}

n_type_real
n_wav_sample_clamp( n_wav *wav, n_type_real d )
{

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		// [!] : don't use SHRT_MIN

		if ( d >  SHRT_MAX )
		{
			d =  SHRT_MAX;
		} else
		if ( d < -SHRT_MAX )
		{
			d = -SHRT_MAX;
		}
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		if ( d >  1.0 )
		{
			d =  1.0;
		} else
		if ( d < -1.0 )
		{
			d = -1.0;
		}
	}


	return d;
}

n_type_real
n_wav_sample_blend( n_type_real f, n_type_real t, n_type_real blend )
{

	if ( blend <= 0.0 ) { return f; }
	if ( blend >= 1.0 ) { return t; }

	n_type_real d = ( f - t ) * blend;

	if ( f > t )
	{
		return f - (int) n_posix_max_n_type_real(  1.0, d );
	} else {
		return f - (int) n_posix_min_n_type_real( -1.0, d );
	}
}

n_type_real
n_wav_sample_amp( n_wav *wav )
{

	n_type_real ret = 0;

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		ret = SHRT_MAX;
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		ret = 1.0;
	}


	return ret;
}

void
n_wav_sample_get( n_wav *wav, u32 i, n_type_real *l, n_type_real *r )
{

	const u32 p = i * N_WAV_STEREO( wav );

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );

		if ( l != NULL ) { (*l) = (n_type_real) ptr[ p + 0 ]; }
		if ( r != NULL ) { (*r) = (n_type_real) ptr[ p + 1 ]; }
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		float *ptr = (float*) N_WAV_PTR( wav );

		if ( l != NULL ) { (*l) = (n_type_real) ptr[ p + 0 ]; }
		if ( r != NULL ) { (*r) = (n_type_real) ptr[ p + 1 ]; }
	}

	if ( l != NULL ) { (*l) = n_wav_sample_clamp( wav, (*l) ); }
	if ( r != NULL ) { (*r) = n_wav_sample_clamp( wav, (*r) ); }


	return;
}

void
n_wav_sample_set( n_wav *wav, u32 i, n_type_real l, n_type_real r )
{

	const u32 p = i * N_WAV_STEREO( wav );

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );

		l = n_wav_sample_clamp( wav, l );
		r = n_wav_sample_clamp( wav, r );

		ptr[ p + 0 ] = (s16) l;
		ptr[ p + 1 ] = (s16) r;
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		float *ptr = (float*) N_WAV_PTR( wav );

		l = n_wav_sample_clamp( wav, l );
		r = n_wav_sample_clamp( wav, r );

		ptr[ p + 0 ] = (float) l;
		ptr[ p + 1 ] = (float) r;
	}


	return;
}

void
n_wav_sample_add( n_wav *wav, u32 i, n_type_real l, n_type_real r )
{

	const u32 p = i * N_WAV_STEREO( wav );

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );

		n_type_real ll = l + ptr[ p + 0 ];
		n_type_real rr = r + ptr[ p + 1 ];

		ll = n_wav_sample_clamp( wav, ll );
		rr = n_wav_sample_clamp( wav, rr );

		ptr[ p + 0 ] = (s16) ll;
		ptr[ p + 1 ] = (s16) rr;
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		float *ptr = (float*) N_WAV_PTR( wav );

		n_type_real ll = l + ptr[ p + 0 ];
		n_type_real rr = r + ptr[ p + 1 ];

		ll = n_wav_sample_clamp( wav, ll );
		rr = n_wav_sample_clamp( wav, rr );

		ptr[ p + 0 ] = (float) ll;
		ptr[ p + 1 ] = (float) rr;
	}


	return;
}

void
n_wav_sample_mix( n_wav *wav, u32 i, n_type_real l, n_type_real r, n_type_real ratio_l, n_type_real ratio_r )
{

	const u32 p = i * N_WAV_STEREO( wav );

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		s16 *ptr = (s16*) N_WAV_PTR( wav );

		l *= ratio_l;
		r *= ratio_r;

		l += (n_type_real) ptr[ p + 0 ] * ( 1.0 - ratio_l );
		r += (n_type_real) ptr[ p + 1 ] * ( 1.0 - ratio_r );

		l = n_wav_sample_clamp( wav, l );
		r = n_wav_sample_clamp( wav, r );

		ptr[ p + 0 ] = (s16) l;
		ptr[ p + 1 ] = (s16) r;
	} else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		float *ptr = (float*) N_WAV_PTR( wav );

		l *= ratio_l;
		r *= ratio_r;

		l += (n_type_real) ptr[ p + 0 ] * ( 1.0 - ratio_l );
		r += (n_type_real) ptr[ p + 1 ] * ( 1.0 - ratio_r );

		l = n_wav_sample_clamp( wav, l );
		r = n_wav_sample_clamp( wav, r );

		ptr[ p + 0 ] = (float) l;
		ptr[ p + 1 ] = (float) r;
	}


	return;
}

n_type_real
n_wav_sample_hz2sample( n_wav *wav, n_type_real hz )
{

	// [!] : cache for performance

	static n_type_real p_hz = 0;
	static n_type_real ret  = 0;

	if ( p_hz != hz ) { ret = N_WAV_RATE( wav ) / hz; p_hz = hz; }


	return ret;
}

n_type_real
n_wav_sample_cosine( n_wav *wav, n_type_real hz, u32 x )
{

	// [!] : I : <complex.h> complex number

	n_type_real t = n_wav_sample_hz2sample( wav, hz );
	n_type_real d;

#ifdef _MSC_VER

	d = (n_type_real) x / t;
	d = cos( N_WAV_2PI * d );
	d = n_wav_sample_amp( wav ) * d;

#else // #ifdef _MSC_VER

	d = (n_type_real) x / t;
	d = cexp( I * ( N_WAV_2PI * d ) );
	d = n_wav_sample_amp( wav ) * d;

#endif // #ifdef _MSC_VER

	return d;
}

n_type_real
n_wav_sample_sine_coeff( n_wav *wav, n_type_real hz, u32 x )
{

	n_type_real t = n_wav_sample_hz2sample( wav, hz );
	n_type_real d;

	d = (n_type_real) x / t;
	d = sin( N_WAV_2PI * d );


	return d;
}

n_type_real
n_wav_sample_sine( n_wav *wav, n_type_real hz, u32 x )
{

	n_type_real d = n_wav_sample_sine_coeff( wav, hz, x );

	d = n_wav_sample_amp( wav ) * d;


	return d;
}

n_type_real
n_wav_sample_sawtooth( n_wav *wav, n_type_real hz, u32 x )
{

	// [!] : currently cosine compatible curve will generate

	n_type_real t = n_wav_sample_hz2sample( wav, hz );
	n_type_real d;

	d = fmod( x, t );
	d = d / t;
	d = n_wav_sample_amp( wav ) * d;
	d = d - ( n_wav_sample_amp( wav ) / 2 );
	d = d * -1;


	return d;
}

n_type_real
n_wav_sample_square( n_wav *wav, n_type_real hz, u32 x )
{

	n_type_real d = n_wav_sample_sine_coeff( wav, hz, x );

	if ( d > 0 ) { d =  n_wav_sample_amp( wav ); } else
	if ( d < 0 ) { d = -n_wav_sample_amp( wav ); }


	return d;
}

n_type_real
n_wav_sample_whitenoise( n_wav *wav, n_type_real hz, u32 x )
{

	// [!] : human recognition
	//
	//	20Hz to 20,000Hz

	//n_type_real t = n_wav_sample_hz2sample( hz );
	n_type_real d = n_wav_sample_sine( wav, n_random_range( (u32) hz ), x );


	return d;
}


#endif // _H_NONNON_NEUTRAL_WAV_SAMPLE

