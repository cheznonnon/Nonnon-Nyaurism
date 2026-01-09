// Nonnon Wave
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV
#define _H_NONNON_NEUTRAL_WAV




#include "./posix.c"




// [!] : getset macro

#define N_WAV_FMT(        wav ) ( ( wav )->fmt )
#define N_WAV_WH(         wav ) ( ( wav )->wh )
#define N_WAV_MSEC(       wav ) ( ( wav )->msec )
#define N_WAV_COUNT(      wav ) ( ( wav )->count )

#define N_WAV_FORMAT(     wav ) ( ( wav )->fmt.wFormatTag )
#define N_WAV_STEREO(     wav ) ( ( wav )->fmt.nChannels )
#define N_WAV_RATE(       wav ) ( ( wav )->fmt.nSamplesPerSec )
#define N_WAV_BYTEPERSEC( wav ) ( ( wav )->fmt.nAvgBytesPerSec )
#define N_WAV_BYTELEN(    wav ) ( ( wav )->fmt.nBlockAlign )
#define N_WAV_BIT(        wav ) ( ( wav )->fmt.wBitsPerSample )
#define N_WAV_CBSIZE(     wav ) ( ( wav )->fmt.cbSize )

#define N_WAV_PTR(        wav ) ( ( wav )->wh.lpData )
#define N_WAV_SIZE(       wav ) ( ( wav )->wh.dwBufferLength )
#define N_WAV_LOOP(       wav ) ( ( wav )->wh.dwLoops )


// [!] : constant

#define N_WAV_FMTSIZE           ( sizeof( WAVEFORMATEX ) - 2 )


// [!] : WAVEFORMATEX.wFormatTag

#define N_WAV_FORMAT_PCM   1
#define N_WAV_FORMAT_FLOAT 3


#ifdef N_WAV_FORMAT_FLOAT_ON

#define N_WAV_FORMAT_DEFAULT N_WAV_FORMAT_FLOAT

#else  // #ifdef N_WAV_FORMAT_FLOAT_ON

#define N_WAV_FORMAT_DEFAULT N_WAV_FORMAT_PCM

#endif // #ifdef N_WAV_FORMAT_FLOAT_ON




#ifdef N_POSIX_PLATFORM_WINDOWS

#include <mmsystem.h>

#else  // #ifndef N_POSIX_PLATFORM_WINDOWS

typedef struct {

	u16 wFormatTag;
	u16 nChannels;
	u32 nSamplesPerSec;
	u32 nAvgBytesPerSec;
	u16 nBlockAlign;
	u16 wBitsPerSample;
	u16 cbSize;

} WAVEFORMATEX;

typedef struct wavehdr_tag {

	s8                 *lpData;
	u32                dwBufferLength;
	u32                dwBytesRecorded;
	u32                dwUser;
	u32                dwFlags;
	u32                dwLoops;
	struct wavehdr_tag *lpNext;
	u32                reserved;

} WAVEHDR;

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS


typedef struct {

	WAVEFORMATEX fmt;
	WAVEHDR      wh;

	// [!] : original file spec

	int          format;
	int          channel;
	int          bit;
	int          rate;

	// [!] : internal cache

#ifdef N_WAV_FORMAT_FLOAT_ON
	n_type_real  msec;
#else
	u32          msec;
#endif

	u32          count;

} n_wav;


static n_posix_bool n_wav_safemode = n_posix_true;




#ifdef N_POSIX_PLATFORM_MAC

static NSOperationQueue *n_wav_queue = NULL;

#endif




#define n_wav_zero(  f    ) n_memory_zero( f,    sizeof( n_wav ) )
#define n_wav_alias( f, t ) n_memory_copy( f, t, sizeof( n_wav ) )

void
n_wav_free( n_wav *wav )
{

	if ( wav == NULL ) { return; }


	n_memory_free( N_WAV_PTR( wav ) );

	n_wav_zero( wav );


	return;
}

void
n_wav_carboncopy( const n_wav *f, n_wav *t )
{

	// [!] : parameter will be overwritten


	{
		if ( f == NULL ) { return; }
		if ( t == NULL ) { return; }
	}


	n_wav_alias( f, t );


	N_WAV_PTR( t ) = (s8*) n_memory_new( N_WAV_SIZE( f ) );
	n_memory_copy( N_WAV_PTR( f ), N_WAV_PTR( t ), N_WAV_SIZE( f ) );


	return;
}

n_posix_bool
n_wav_is_loadable( const n_wav *wav, int id )
{

	if ( wav == NULL ) { return n_posix_false; }


	// [!] : supported : 1 = WAVE_FORMAT_PCM
	// [!] : supported : 3 = WAVE_FORMAT_IEEE_FLOAT

//NSLog( @"%d : %d", id, N_WAV_FORMAT( wav ) );
	if (
		( N_WAV_FORMAT_PCM   == N_WAV_FORMAT( wav ) )
		||
		( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	)
	{
		//
	} else {
		return n_posix_false;
	}


	// [!] : invalid parameters

//NSLog( @"%d : %d %d", id, N_WAV_FORMAT( wav ), N_WAV_BIT( wav ) );
//NSLog( @"%d : %d", id, ( 0 == N_WAV_SIZE( wav ) ) );
//NSLog( @"%d : %d", id, ( N_WAV_BYTEPERSEC( wav ) != ( N_WAV_RATE( wav ) * N_WAV_BYTELEN( wav ) ) ) );
//NSLog( @"%d : %d", id, ( N_WAV_BYTELEN( wav ) != ( ( N_WAV_STEREO( wav ) * N_WAV_BIT( wav ) ) / 8 ) ) );

	if (
		( 0 == N_WAV_SIZE( wav ) )
		||
		( N_WAV_BYTEPERSEC( wav ) != ( N_WAV_RATE( wav ) * N_WAV_BYTELEN( wav ) ) )
		||
		( N_WAV_BYTELEN( wav ) != ( ( N_WAV_STEREO( wav ) * N_WAV_BIT( wav ) ) / 8 ) )
		||
		(
			( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
			&&
			( ( N_WAV_BIT( wav ) != 8 )&&( N_WAV_BIT( wav ) != 16 ) )
		)
		||
		(
			( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
			&&
			( N_WAV_BIT( wav ) != 32 )
		)
	)
	{
		return n_posix_false;
	}


	return n_posix_true;
}

n_posix_bool
n_wav_error( const n_wav *wav, int id )
{

	if ( n_wav_safemode == n_posix_false ) { return n_posix_false; }


	if ( NULL ==            wav   ) { return n_posix_true; }
	if ( NULL == N_WAV_PTR( wav ) ) { return n_posix_true; }


	if ( n_posix_false == n_wav_is_loadable( wav, id ) ) { return n_posix_true; }


	return n_posix_false;
}

void
n_wav_precalc( n_wav *wav, n_type_int byte, int format, int channel, int bit, int rate )
{

	if ( wav == NULL ) { return; }

	if ( byte > UINT_MAX ) { return; }


	if ( byte    != -1 ) { N_WAV_SIZE  ( wav ) = (u32) byte; }
	if ( format  != -1 ) { N_WAV_FORMAT( wav ) =     format; }
	if ( channel != -1 ) { N_WAV_STEREO( wav ) =    channel; }
	if ( bit     != -1 ) { N_WAV_BIT   ( wav ) =        bit; }
	if ( rate    != -1 ) { N_WAV_RATE  ( wav ) =       rate; }

	N_WAV_CBSIZE    ( wav ) = sizeof( WAVEFORMATEX );

	N_WAV_BYTELEN   ( wav ) = ( ( N_WAV_STEREO( wav ) * N_WAV_BIT( wav ) ) / 8 );
	N_WAV_BYTEPERSEC( wav ) = ( N_WAV_RATE( wav ) * N_WAV_BYTELEN( wav ) );

	int unit = 0;
	if ( N_WAV_FORMAT_PCM   == N_WAV_FORMAT( wav ) ) { unit = sizeof(   s16 ); } else
	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) ) { unit = sizeof( float ); }

	n_type_real sample_per_msec = (n_type_real) N_WAV_RATE( wav ) / 1000.0;
	N_WAV_MSEC      ( wav ) = (n_type_real) N_WAV_SIZE( wav ) / N_WAV_BYTELEN( wav ) / sample_per_msec;
	N_WAV_COUNT     ( wav ) = N_WAV_SIZE( wav ) / ( unit * N_WAV_STEREO( wav ) );


	return;
}

void
n_wav_stereo( n_wav *wav )
{

	if ( n_wav_error( wav, 1 ) ) { return; }

	if ( 1 != N_WAV_STEREO( wav ) ) { return; }


	// [Mechanism]
	//
	//	f : mono
	//	t : stereo

	n_type_int check_size = N_WAV_SIZE( wav ) * 2;
	if ( check_size > UINT_MAX ) { return; }

	u32  fsize = N_WAV_SIZE( wav ) / ( N_WAV_BIT( wav ) / 8 );
	u8    *f8  = (   u8*) N_WAV_PTR( wav );
	s16   *f16 = (  s16*) f8;
	float *f32 = (float*) f8;

	u32  tsize = (u32) check_size;
	u8    *t8  = (   u8*)n_memory_new( tsize ); n_memory_zero( t8, tsize );
	s16   *t16 = (  s16*) t8;
	float *t32 = (float*) t8;

	int   step = N_WAV_STEREO( wav );


	u32  i = 0;
	u32 ii = 0;
	n_posix_loop
	{//break;

		if (  8 == N_WAV_BIT( wav ) )
		{
			t8 [ ii + 0 ] = f8 [ i + 0 ];
			t8 [ ii + 1 ] = f8 [ i + 0 ];
		} else
		if ( 16 == N_WAV_BIT( wav ) )
		{
			t16[ ii + 0 ] = f16[ i + 0 ];
			t16[ ii + 1 ] = f16[ i + 0 ];
		} else
		if ( 32 == N_WAV_BIT( wav ) )
		{
			t32[ ii + 0 ] = f32[ i + 0 ];
			t32[ ii + 1 ] = f32[ i + 0 ];
		}

		i  += step;
		if ( i >= fsize ) { break; }

		ii += step * 2;
	}


	n_wav_precalc( wav, tsize, -1, 2, -1, -1 );

	n_memory_free( N_WAV_PTR( wav ) );

	N_WAV_PTR ( wav ) = (s8*) t8;


	return;
}

void
n_wav_bit( n_wav *wav, int format )
{

	if ( n_wav_error( wav, 2 ) ) { return; }


	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		if ( 8 != N_WAV_BIT( wav ) ) { return; }
	} else
	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_FLOAT )
	{
		if ( ( 8 != N_WAV_BIT( wav ) )&&( 16 != N_WAV_BIT( wav ) ) ) { return; }
	}


	if ( 8 == N_WAV_BIT( wav ) )
	{

		n_type_int check_f = N_WAV_SIZE( wav );
		if ( check_f > UINT_MAX ) { return; }

		n_type_int check_t = N_WAV_SIZE( wav );
		if ( format == N_WAV_FORMAT_PCM   ) { check_t *= 2; }
		if ( format == N_WAV_FORMAT_FLOAT ) { check_t *= 4; }
		if ( check_t > UINT_MAX ) { return; }


		u32  fsize = (u32) check_f;
		u8  *f8    = (u8*) N_WAV_PTR( wav );

		u32  tsize = (u32) check_t;
		u8  *t8    = (u8*) n_memory_new( tsize ); n_memory_zero( t8, tsize );

		s16   *t16 = (  s16*) t8;
		float *t32 = (float*) t8;


		u32 i = 0;
		n_posix_loop
		{//break;

			// [!] : zero at 8bit = 0x80 = 128

			n_type_real d = (n_type_real) f8[ i ];


			// [!] : remapping

			d -= 128;
			d /= 127;

			if ( format == N_WAV_FORMAT_PCM )
			{
				d *= SHRT_MAX;
				t16[ i ] = (s16) d;
			} else
			if ( format == N_WAV_FORMAT_FLOAT )
			{
				t32[ i ] = (float) d;
			}

			i++;
			if ( i >= fsize ) { break; }
		}


		int to_bit = 16;
		if ( format == N_WAV_FORMAT_PCM   ) { to_bit = 16; } else
		if ( format == N_WAV_FORMAT_FLOAT ) { to_bit = 32; }

		n_wav_precalc( wav, tsize, format, -1, to_bit, -1 );

		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR ( wav ) = (void*) t8;

	} else
	if ( 16 == N_WAV_BIT( wav ) )
	{

		n_type_int check_f = N_WAV_SIZE( wav );
		if ( check_f > UINT_MAX ) { return; }

		n_type_int check_t = N_WAV_SIZE( wav ) * 2;
		if ( check_t > UINT_MAX ) { return; }


		u32   fsize = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
		s16  *f16   = (s16*) N_WAV_PTR( wav );

		u32   tsize = (u32) check_t;
		float *t32  = (float*) n_memory_new( tsize ); n_memory_zero( t32, tsize );


		u32 i = 0;
		n_posix_loop
		{//break;

			// [!] : remapping

			n_type_real d = (n_type_real) f16[ i ] / SHRT_MAX;

			t32[ i ] = (float) d;

			i++;
			if ( i >= fsize ) { break; }
		}


		n_wav_precalc( wav, tsize, format, -1, 32, -1 );

		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR ( wav ) = (void*) t32;

	}


	return;
}

void
n_wav_44100( n_wav *wav )
{

	if ( n_wav_error( wav, 3 ) ) { return; }


	// [Mechanism] : ratio
	//
	//	44100 : 1.0
	//	22050 : 2.0
	//	11025 : 4.0
	//	 8000 : 5.5

	n_type_real d     = 0;
	n_type_real ratio = (n_type_real) 44100 / N_WAV_RATE( wav );

	if ( ratio == 1.0 ) { return; }


	n_type_int check_t = N_WAV_SIZE( wav ) * (u32) ceil( ratio );
	if ( check_t > UINT_MAX ) { return; }


	u32  fsize = N_WAV_SIZE( wav ) / ( N_WAV_BIT( wav ) / 8 );
	u8    *f8  = (   u8*) N_WAV_PTR( wav );
	s16   *f16 = (  s16*) f8;
	float *f32 = (float*) f8;

	u32  tsize = (u32) check_t;
	u8    *t8  = (   u8*) n_memory_new( tsize ); n_memory_zero( t8, tsize );
	s16   *t16 = (  s16*) t8;
	float *t32 = (float*) t8;

	u32   step = N_WAV_STEREO( wav );


	u32  i = 0;
	u32 ii = 0;
	n_posix_loop
	{

		if ( 8 == N_WAV_BIT( wav ) )
		{

			t8[ ii + 0 ] = f8[ i + 0 ];

			if ( 2 == N_WAV_STEREO( wav ) )
			{
				t8[ ii + 1 ] = f8[ i + 1 ];
			}

		} else
		if ( 16 == N_WAV_BIT( wav ) )
		{

			t16[ ii + 0 ] = f16[ i + 0 ];

			if ( 2 == N_WAV_STEREO( wav ) )
			{
				t16[ ii + 1 ] = f16[ i + 1 ];
			}

		} else
		if ( 32 == N_WAV_BIT( wav ) )
		{

			t32[ ii + 0 ] = f32[ i + 0 ];

			if ( 2 == N_WAV_STEREO( wav ) )
			{
				t32[ ii + 1 ] = f32[ i + 1 ];
			}

		}


		d += 1;
		if ( d >= ratio )
		{

			d -= ratio;

			i += step;
			if ( i >= fsize ) { break; }
		}


		ii += step;
		if ( ii >= tsize ) { break; }
	}

//if ( size != ii ) { n_posix_debug_literal( "%lu/%lu", tsize, ii ); }


	n_wav_precalc( wav, tsize, -1, -1, -1, 44100 );

	n_memory_free( N_WAV_PTR( wav ) );

	N_WAV_PTR ( wav ) = (s8*) t8;


	return;
}

void
n_wav_float( n_wav *wav )
{
//NSLog( @"n_wav_float()" );

	if ( n_wav_error( wav, 4 ) ) { return; }

//NSLog( @"%d", N_WAV_STEREO( wav ) );


	if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) )
	{
		// [!] : FLOAT to PCM

		u32 c = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );

		float *f = (float*) N_WAV_PTR( wav );
		s16   *t = n_memory_new( c * sizeof( s16 ) );

		u32 i = 0;
		n_posix_loop
		{

			t[ i ] = (s16) ( (float) f[ i ] * SHRT_MAX ); i++;

			if ( 2 == N_WAV_STEREO( wav ) )
			{
				t[ i ] = (s16) ( (float) f[ i ] * SHRT_MAX ); i++;
			}

			if ( i >= c ) { break; }
		}


		n_memory_free( N_WAV_PTR( wav ) );
		N_WAV_PTR( wav ) = (void*) t;

		n_wav_precalc( wav, c * sizeof( s16 ), N_WAV_FORMAT_PCM, -1, 16, -1 );
	}


	return;
}

void
n_wav_enhancer( n_wav *wav )
{
//NSLog( @"n_wav_enhancer()" );

	if ( n_wav_error( wav, 5 ) ) { return; }

	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		// [!] : FLOAT to PCM

		if ( N_WAV_FORMAT_FLOAT == N_WAV_FORMAT( wav ) ) { n_wav_float( wav ); }

		if (     2 != N_WAV_STEREO( wav ) ) { n_wav_stereo( wav ); }
		if (    16 != N_WAV_BIT   ( wav ) ) { n_wav_bit   ( wav, N_WAV_FORMAT_PCM ); }
		if ( 44100 != N_WAV_RATE  ( wav ) ) { n_wav_44100 ( wav ); }
	} else {
		// [!] : PCM to FLOAT

		if (     2 != N_WAV_STEREO( wav ) ) { n_wav_stereo( wav ); }
		if (    32 != N_WAV_BIT   ( wav ) ) { n_wav_bit   ( wav, N_WAV_FORMAT_FLOAT ); }
		if ( 44100 != N_WAV_RATE  ( wav ) ) { n_wav_44100 ( wav ); }
	}


	return;
}

void
n_wav_s16_to_float( n_wav *wav )
{

	u32  fcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
	s16    *f16 = (s16*) N_WAV_PTR( wav );

	u32   tbyte = N_WAV_SIZE( wav ) * 2;
	float  *t32 = (float*) n_memory_new( tbyte ); n_memory_zero( t32, tbyte );


	u32 i = 0;
	n_posix_loop
	{//break;

		n_type_real d = (n_type_real) f16[ i ] / SHRT_MAX;

		t32[ i ] = (float) d;

		i++;
		if ( i >= fcount ) { break; }
	}


	n_wav_precalc( wav, tbyte, N_WAV_FORMAT_FLOAT, -1, 32, -1 );


	n_memory_free( N_WAV_PTR( wav ) );

	N_WAV_PTR( wav ) = (void*) t32;


	return;
}

void
n_wav_reducer( n_wav *wav )
{

	// [!] : crash : don't call twice or more with a single .wav

//NSLog( @"%d %d %d %d", wav->format, wav->channel, wav->bit, wav->rate ); return;


	if ( n_wav_error( wav, 6 ) ) { return; }


	if ( ( wav->channel != 1 )&&( wav->channel != 2 ) ) { return; }

	if ( wav->format == N_WAV_FORMAT_PCM )
	{
		if ( ( wav->bit != 8 )&&( wav->bit != 16 ) ) { return; }
	} else
	if ( wav->format == N_WAV_FORMAT_FLOAT )
	{
		if ( wav->bit != 32 ) { return; }
	}

	if ( ( wav->rate != 11 )&&( wav->rate != 22 )&&( wav->rate != 44 ) ) { return; }


	// [Needed] : rate first

	if ( wav->rate != 44 )
	{

		u32   ratio = 44 / wav->rate;
		u32    unit = N_WAV_BIT( wav ) / 8;

		u32  fcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
		u8     *f8  = (   u8*) N_WAV_PTR( wav );
		s16    *f16 = (  s16*) f8;
		float  *f32 = (float*) f8;

		u32  tcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav ) / ratio;
		u32   tbyte = tcount * unit;
		u8     *t8  = (   u8*) n_memory_new( tbyte ); n_memory_zero( t8, tbyte );
		s16    *t16 = (  s16*) t8;
		float  *t32 = (float*) t8;


		u32 f = 0;
		u32 t = 0;
		n_posix_loop
		{

			n_type_real sample_l = 0;
			n_type_real sample_r = 0;

			u32 i = 0;
			n_posix_loop
			{
				if (  8 == N_WAV_BIT( wav ) )
				{
					sample_l +=  f8[ f + i + 0 ];
					sample_r +=  f8[ f + i + 1 ];
				} else
				if ( 16 == N_WAV_BIT( wav ) )
				{
					sample_l += f16[ f + i + 0 ];
					sample_r += f16[ f + i + 1 ];
				} else
				if ( 32 == N_WAV_BIT( wav ) )
				{
					sample_l += f32[ f + i + 0 ];
					sample_r += f32[ f + i + 1 ];
				}

				i += N_WAV_STEREO( wav );
				if ( i >= ( ratio * N_WAV_STEREO( wav ) ) ) { break; }
			}

			sample_l /= ratio;
			sample_r /= ratio;

			if (  8 == N_WAV_BIT( wav ) )
			{
				 t8[ t + 0 ] =    (u8) sample_l;
				 t8[ t + 1 ] =    (u8) sample_l;
			} else
			if ( 16 == N_WAV_BIT( wav ) )
			{
				t16[ t + 0 ] =   (s16) sample_l;
				t16[ t + 1 ] =   (s16) sample_r;
			} else
			if ( 32 == N_WAV_BIT( wav ) )
			{
				t32[ t + 0 ] = (float) sample_l;
				t32[ t + 1 ] = (float) sample_r;
			}

			f += ratio * N_WAV_STEREO( wav );
			if ( f >= fcount ) { break; }

			t += N_WAV_STEREO( wav );
			if ( t >= tcount ) { break; }
		}


		n_wav_precalc( wav, tbyte, -1, -1, -1, 44100 / ratio );


		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR( wav ) = (void*) t8;

	}


	if ( ( wav->channel == 1 )&&( 2 == N_WAV_STEREO( wav ) ) )
	{

		// [Mechanism]
		//
		//	f : stereo
		//	t : mono

		u32  fcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
		u8     *f8  = (   u8*) N_WAV_PTR( wav );
		s16    *f16 = (  s16*) f8;
		float  *f32 = (float*) f8;

		u32  tbyte = N_WAV_SIZE( wav ) / 2;
		u8    *t8  = (   u8*) n_memory_new( tbyte ); n_memory_zero( t8, tbyte );
		s16   *t16 = (  s16*) t8;
		float *t32 = (float*) t8;

		u32   step = N_WAV_STEREO( wav );


		n_type_real d;


		u32 i  = 0;
		u32 ii = 0;
		n_posix_loop
		{//break;

			if ( 8 == N_WAV_BIT( wav ) )
			{

				d  = (n_type_real) f8[ i + 0 ];

				if ( 2 == N_WAV_STEREO( wav ) )
				{
					d += (n_type_real) f8[ i + 1 ];
				}

				t8[ ii ] = (u8) ( d / N_WAV_STEREO( wav ) );

			} else
			if ( 16 == N_WAV_BIT( wav ) )
			{

				d = (n_type_real) f16[ i + 0 ];

				if ( 2 == N_WAV_STEREO( wav ) )
				{
					d += (n_type_real) f16[ i + 1 ];
				}

				t16[ ii ] = (s16) ( d / N_WAV_STEREO( wav ) );

			} else
			if ( 32 == N_WAV_BIT( wav ) )
			{

				d = (n_type_real) f32[ i + 0 ];

				if ( 2 == N_WAV_STEREO( wav ) )
				{
					d += (n_type_real) f32[ i + 1 ];
				}

				t32[ ii ] = (float) ( d / N_WAV_STEREO( wav ) );

			}


			i += step;
			if ( i >= fcount ) { break; }

			ii++;
		}


		n_wav_precalc( wav, tbyte, -1, 1, -1, -1 );

		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR( wav ) = (void*) t8;

	}
//return;


//NSLog( @"%d -> %d", N_WAV_BIT( wav ), wav->bit );

	if ( ( wav->bit == 8 )&&( 8 != N_WAV_BIT( wav ) ) )
	{
//NSLog( @"8" );

		u32  fcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
		u8     *f8  = (   u8*) N_WAV_PTR( wav );
		s16    *f16 = (  s16*) f8;
		float  *f32 = (float*) f8;

		u32  tbyte = N_WAV_SIZE( wav ) / ( N_WAV_BIT( wav ) / 8 );
		u8  *t8    = (u8 *) n_memory_new( tbyte ); n_memory_zero( t8, tbyte );


		u32 i = 0;
		n_posix_loop
		{//break;

			n_type_real d = 0;


			// remapping

			if ( 16 == N_WAV_BIT( wav ) )
			{
				d = (n_type_real) f16[ i ];
				d /= SHRT_MAX;
			} else
			if ( 32 == N_WAV_BIT( wav ) )
			{
				d = (n_type_real) f32[ i ];
			}


			// zero at 8bit = 0x80 = 128

			d *= 127;
			d += 128;


			t8[ i ] = (u8) d;


			i++;
			if ( i >= fcount ) { break; }
		}


		n_wav_precalc( wav, tbyte, N_WAV_FORMAT_PCM, -1, 8, -1 );


		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR( wav ) = (void*) t8;

	} else
	if ( ( wav->bit == 16 )&&( 32 == N_WAV_BIT( wav ) ) )
	{
//NSLog( @"16 %d", N_WAV_BIT( wav ) );

		u32  fcount = N_WAV_COUNT( wav ) * N_WAV_STEREO( wav );
		u8     *f8  = (   u8*) N_WAV_PTR( wav );
		float  *f32 = (float*) f8;

		u32  tbyte = fcount * 2;
		s16 *t16   = (s16*) n_memory_new( tbyte ); n_memory_zero( t16, tbyte );

//NSLog( @"16 %d %d", fsize, N_WAV_COUNT( wav ) * N_WAV_STEREO( wav ) );

		u32 i = 0;
		n_posix_loop
		{//break;

			n_type_real d = (n_type_real) f32[ i ] * SHRT_MAX;

			t16[ i ] = (s16) d;

			i++;
			if ( i >= fcount ) { break; }
		}


		n_wav_precalc( wav, tbyte, N_WAV_FORMAT_PCM, -1, 16, -1 );
//NSLog( @"16 %d %d", N_WAV_FORMAT( wav ), N_WAV_BIT( wav ) );

		n_memory_free( N_WAV_PTR( wav ) );

		N_WAV_PTR( wav ) = (void*) t16;

	} else
	if ( ( wav->bit == 32 )&&( 16 == N_WAV_BIT( wav ) ) )
	{
//NSLog( @"to 32-bit float" );

		n_wav_s16_to_float( wav );

	}


	return;
}

n_posix_bool
n_wav_search_chunk_onmemory( const u8 *ptr, n_type_int ptrsize, const char *chunk, n_type_int *offset )
{

	if (    ptr == NULL ) { return n_posix_true; }
	if (  chunk == NULL ) { return n_posix_true; }
	if ( offset == NULL ) { return n_posix_true; }

	if ( ptrsize > UINT_MAX ) { return n_posix_true; }


	if ( 4 != strlen( chunk ) ) { return n_posix_true; }


	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_memory_is_same( chunk, &ptr[ i ], 4 ) ) { break; }


		i++;
		if ( i >= ( ptrsize - 4 ) ) { return n_posix_true; }
	}


	(*offset) = i;


	return n_posix_false;
}

#define n_wav_load(          wav, fname     ) n_wav_load_internal( wav, (void*) fname,    0, n_posix_true  )
#define n_wav_load_onmemory( wav, ptr, size ) n_wav_load_internal( wav,           ptr, size, n_posix_false )

// internal
n_posix_bool
n_wav_load_internal( n_wav *wav, void *ptr, n_type_int ptrsize, n_posix_bool is_file )
{
//NSLog( @"n_wav_load_internal()" );

	if ( ptr == NULL ) { return n_posix_true; }
	if ( wav == NULL ) { return n_posix_true; }


	n_posix_bool is_onmemory = n_posix_true;


	if ( is_file )
	{

		const n_posix_char *fname = (n_posix_char*) ptr;

		ptrsize = n_posix_stat_size( fname );
		if ( ptrsize > UINT_MAX ) { return n_posix_true; }

		FILE *fp = n_posix_fopen_read( fname );
		if ( fp == NULL ) { return n_posix_true; }

		ptr = n_memory_new( ptrsize );

		n_posix_fread( ptr, ptrsize, 1, fp );

		n_posix_fclose( fp );


		is_onmemory = n_posix_false;

	} else {

		if ( ptrsize > UINT_MAX ) { return n_posix_true; }

	}


	const u8 *p = (u8*) ptr;


	n_wav check; n_wav_zero( &check );


	n_type_int i;


	n_posix_bool ret = n_posix_false;


	// Subchunk : "fmt " : WAVEFORMATEX

	i = 0;
	ret += n_wav_search_chunk_onmemory( p, ptrsize, "fmt ", &i );

	n_memory_copy( &p[ i + 8 ], &N_WAV_FMT( &check ), N_WAV_FMTSIZE );


	// Subchunk : "data" : WAVEHDR

	i = 0;
	ret += n_wav_search_chunk_onmemory( p, ptrsize, "data", &i );

	if ( ret )
	{
		if ( is_onmemory == n_posix_false ) { n_memory_free( ptr ); }
		return n_posix_true;
	}

	n_memory_copy( &p[ i + 4 ], &N_WAV_SIZE( &check ), 4 );


	if ( n_posix_false == n_wav_is_loadable( &check, -1 ) )
	{
//NSLog( @"unloadable" );
		if ( is_onmemory == n_posix_false ) { n_memory_free( ptr ); }
		return n_posix_true;
	}


	n_wav_free( wav );
	n_wav_alias( &check, wav );


	u32 size; n_memory_copy( &p[ i + 4 ], &size, 4 );

	n_wav_precalc( wav, size, -1, -1, -1, -1 );

	N_WAV_PTR( wav ) = (s8*) n_memory_new( size );
	n_memory_copy( &p[ i + 8 ], N_WAV_PTR( wav ), size );


	if ( is_onmemory == n_posix_false ) { n_memory_free( ptr ); }


	// [!] : remember original file spec

	wav->format  = N_WAV_FORMAT( wav );
	wav->channel = N_WAV_STEREO( wav );
	wav->bit     = N_WAV_BIT   ( wav );
	wav->rate    = N_WAV_RATE  ( wav ) / 1000;


	n_wav_enhancer( wav );


	return n_posix_false;
}

#define n_wav_new(           wav, v ) n_wav_new_internal( wav, v, 0 )
#define n_wav_new_by_sample( wav, v ) n_wav_new_internal( wav, v, 1 )

// internal
void
n_wav_new_internal( n_wav *wav, u32 msec_or_sample, int mode )
{

	if ( wav == NULL ) { return; }

	if ( msec_or_sample == 0 ) { return; }


	int format;
	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		format = N_WAV_FORMAT_PCM;
	} else {
		format = N_WAV_FORMAT_FLOAT;
	}


	n_type_real bit;
	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		bit = 16;
	} else {
		bit = 32;
	}

	const n_type_real channel         = 2;
	const n_type_real rate            = 44100;
	const n_type_real sample_per_msec = 44100 / 1000;
	const n_type_real bytealign       = channel * ( bit / 8 );


	n_type_int byte;
	if ( mode == 0 )
	{
		byte = (n_type_int) ceil( sample_per_msec * msec_or_sample * bytealign );
	} else {
		byte = msec_or_sample * (n_type_int) bytealign;
	}
	if ( byte > UINT_MAX ) { return; }


	n_wav_free( wav );


	N_WAV_PTR( wav ) = (s8*) n_memory_new( byte );
	n_memory_padding( N_WAV_PTR( wav ), 0, byte );


	// [!] : original file spec

	wav->format  = N_WAV_FORMAT_DEFAULT;
	wav->channel = (u16) channel;
	wav->bit     = (u16) bit;
	wav->rate    = (u32) sample_per_msec;


	n_wav_precalc( wav, byte, format, (int) channel, (int) bit, (int) rate );


	return;
}

#define n_wav_save_literal( wav, fname ) n_wav_save( wav, n_posix_literal( fname ) )

n_posix_bool
n_wav_save( const n_wav *wav, const n_posix_char *fname )
{

	const u32 RIFF = 0x46464952;
	const u32 WAVE = 0x45564157;
	const u32 FMT  = 0x20746d66;
	const u32 DATA = 0x61746164;


	if ( n_wav_error( wav, 7 ) ) { return n_posix_true; }


	FILE *fp = n_posix_fopen_write( fname );
	if ( fp == NULL ) { return n_posix_true; }


	// [!] : parameter non-breaking

	n_wav save;
	n_wav_carboncopy( wav, &save );

	n_wav_reducer( &save );


	u32 riff[ 7 ];
	u32 size = N_WAV_SIZE( &save );

	riff[ 0 ] = RIFF;
	riff[ 1 ] = 4 + 8 + N_WAV_FMTSIZE + 8 + size;
	riff[ 2 ] = WAVE;
	riff[ 3 ] = FMT;
	riff[ 4 ] = N_WAV_CBSIZE( &save ) - 2;
	riff[ 5 ] = DATA;
	riff[ 6 ] = size;

	n_posix_fwrite( &riff[ 0 ],                  4, 1, fp );
	n_posix_fwrite( &riff[ 1 ],                  4, 1, fp );
	n_posix_fwrite( &riff[ 2 ],                  4, 1, fp );
	n_posix_fwrite( &riff[ 3 ],                  4, 1, fp );
	n_posix_fwrite( &riff[ 4 ],                  4, 1, fp );
	n_posix_fwrite( &N_WAV_FMT( &save ), riff[ 4 ], 1, fp );
	n_posix_fwrite( &riff[ 5 ],                  4, 1, fp );
	n_posix_fwrite( &riff[ 6 ],                  4, 1, fp );
	n_posix_fwrite(  N_WAV_PTR( &save ),      size, 1, fp );


	n_wav_free( &save );


	n_posix_fclose( fp );


	return n_posix_false;
}


#endif // _H_NONNON_NEUTRAL_WAV

