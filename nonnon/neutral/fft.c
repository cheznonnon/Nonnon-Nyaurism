// Nonnon Nyaurism
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Thx : DeepSeek AI


// [!] : you need to implemenmt n_fft_equalizer_apply() only
//
//	usage is written in n_fft_test()




#ifndef _H_NONNON_NEUTRAL_FFT
#define _H_NONNON_NEUTRAL_FFT




// [!] : Version 1 : simple and lightweight but always amplified
// [!] : Version 2 : normalizer and edge noise preventer are implemented

#define N_FFT_VERSION_2




#include "./wav.c"




#include <complex.h>




typedef double complex n_fft_complex;




typedef struct {

	int            sample_rate;
	int            num_bands;
	double        *gains;		// [!] : not db
	double        *band_freqs;
	int            fft_size;
	n_fft_complex *fft_buffer_l;
	n_fft_complex *fft_buffer_r;
	double        *window;

#ifdef N_FFT_VERSION_2
	double        *gains_db;
	n_posix_bool   is_bypass;
#endif

} n_fft_equalizer;




void
n_fft_engine( n_fft_complex *buffer, int n, int inverse )
{

	// [!] : Cooley-Tukey algorithm


	if ( n <= 1 ) { return; }


	n_fft_complex *even = (n_fft_complex*) malloc( n/2 * sizeof( n_fft_complex ) );
	n_fft_complex * odd = (n_fft_complex*) malloc( n/2 * sizeof( n_fft_complex ) );

	for ( int i = 0; i < n/2; i++ )
	{
		even[ i ] = buffer[ i * 2 + 0 ];
		 odd[ i ] = buffer[ i * 2 + 1 ];
	}


	n_fft_engine( even, n/2, inverse );
	n_fft_engine(  odd, n/2, inverse );


	for( int k = 0; k < n/2; k++ )
	{
		double        angle = 2 * M_PI * k / n * ( inverse ? -1 : 1 );
		n_fft_complex     t = cexp( I * angle ) * odd[ k ];

		buffer[ k       ] = even[ k ] + t;
		buffer[ k + n/2 ] = even[ k ] - t;

		if ( inverse )
		{
			buffer[ k       ] /= 2;
			buffer[ k + n/2 ] /= 2;
		}
	}


	free( even );
	free(  odd );


	return;
}

void
n_fft_ifft( n_fft_complex* buffer, int n )
{

	n_fft_engine( buffer, n, 1 );

	return;
}




// internal
n_fft_equalizer*
n_fft_equalizer_init( int sample_rate, int num_bands, int fft_size )
{

	n_fft_equalizer *eq = (n_fft_equalizer*) malloc( sizeof( n_fft_equalizer ) );

	eq->sample_rate = sample_rate;
	eq->num_bands   = num_bands;
	eq->fft_size    = fft_size;
	eq->gains       = (double*) malloc( num_bands * sizeof( double ) );


#ifdef N_FFT_VERSION_2
	eq->is_bypass = n_posix_true;
	eq->gains_db  = (double*) malloc( num_bands * sizeof( double ) );
#endif

	for ( int i = 0; i < num_bands; i++ )
	{
		eq->gains   [ i ] = 1.0; // [!] : 1.0 = 0dB
#ifdef N_FFT_VERSION_2
		eq->gains_db[ i ] = 0.0;
#endif
	}


	eq->band_freqs = (double*) malloc( ( num_bands + 1 ) * sizeof( double ) );

	double min_freq = 20.0;			// [!] : 20Hz : hard to implement under 20Hz
	double max_freq = sample_rate / 2.0;	// [!] : nyquist freq.
	double log_min  = log10( min_freq );
	double log_max  = log10( max_freq );
	double step     = ( log_max - log_min ) / num_bands;

	for ( int i = 0; i <= num_bands; i++ )
	{
		eq->band_freqs[ i ] = pow( 10.0, log_min + i * step );
	}


	eq->fft_buffer_l = (n_fft_complex*) malloc( fft_size * sizeof( n_fft_complex ) );
	eq->fft_buffer_r = (n_fft_complex*) malloc( fft_size * sizeof( n_fft_complex ) );


	// window function

	eq->window = (double*) malloc( fft_size * sizeof( double ) );

	for ( int i = 0; i < fft_size; i++ )
	{
		eq->window[ i ] = 0.5 * ( 1.0 - cos( 2.0 * M_PI * i / ( fft_size - 1 ) ) );
	}


	return eq;
}

// internal
void
n_fft_equalizer_exit( n_fft_equalizer *eq )
{

	free( eq->gains );
	free( eq->band_freqs );
	free( eq->fft_buffer_l );
	free( eq->fft_buffer_r );
	free( eq->window );
	free( eq );

	return;
}




// internal
double
n_fft_equalizer_db2linear( double db )
{
	return pow( 10.0, db / 20.0 );
}

// internal
void
n_fft_equalizer_gain_db_set( n_fft_equalizer *eq, int band, double gain_db )
{
	if ( ( band >= 0 )&&( band < eq->num_bands ) )
	{
		eq->gains[ band ] = n_fft_equalizer_db2linear( gain_db );

#ifdef N_FFT_VERSION_2
		eq->gains_db[ band ] = gain_db;

		eq->is_bypass = n_posix_true;
		for ( int i = 0; i < eq->num_bands; i++ )
		{
			if ( fabs( eq->gains_db[ i ]) > 0.1 )
			{
				eq->is_bypass = n_posix_false;
				break;
			}
		}
#endif
	}


	return;
}

// internal
void
n_fft_equalizer_channel_process( n_fft_equalizer *eq, double *audio, int count, int ch )
{

#ifdef N_FFT_VERSION_2
	if ( eq->is_bypass ) { return; }

	n_posix_bool all_unity = n_posix_true;
	for ( int i = 0; i < eq->num_bands; i++ )
	{
		if ( fabs( eq->gains[ i ] - 1.0 ) > 1e-6 )
		{
			all_unity = n_posix_false;
			break;
		}
	}
	if ( all_unity ) return;
#endif


	n_fft_complex *buffer;
	if ( ch == 0 ) { buffer = eq->fft_buffer_l; } else { buffer = eq->fft_buffer_r; }


	int fft_size = eq->fft_size;
	int hop_size = fft_size / 4;	// [!] : 75%


	double *output = (double*) calloc( count, sizeof( double ) );

#ifdef N_FFT_VERSION_2
	double *window_sum = (double*) calloc( count, sizeof( double ) );

	int     padded_size       = count + ( fft_size * 2 );
	double *padded_input      = (double*) calloc( padded_size, sizeof( double ) );
	double *padded_output     = (double*) calloc( padded_size, sizeof( double ) );
	double *padded_window_sum = (double*) calloc( padded_size, sizeof( double ) );

	memcpy( padded_input + fft_size, audio, count * sizeof( double ) );

	int padded_blocks = ( ( padded_size - fft_size ) / hop_size ) + 1;
#endif

#ifdef N_FFT_VERSION_2
	for ( int block = 0; block < padded_blocks; block++ )
#else
	int num_blocks = ( count - fft_size ) / hop_size + 1;

	for ( int block = 0; block <    num_blocks; block++ )
#endif
	{
		int start_idx = block * hop_size;

		for ( int i = 0; i < fft_size; i++ )
		{
			int idx = start_idx + i;
#ifdef N_FFT_VERSION_2
			if ( idx < padded_size )
			{
				buffer[ i ] = padded_input[ idx ] * eq->window[ i ];
			} else {
				buffer[ i ] = 0.0;
			}
#else
			if ( idx < count )
			{
				buffer[ i ] = audio[ idx ] * eq->window[ i ];
			} else {
				buffer[ i ] = 0.0;
			}
#endif
		}

		n_fft_engine( buffer, fft_size, 0 );

#ifdef N_FFT_VERSION_2

		n_posix_bool modified = n_posix_false;

		double freq_resolution = (double) eq->sample_rate / fft_size;

		for( int i = 0; i <= fft_size / 2; i++ )
		{
			double freq = i * freq_resolution;
			if ( freq > ( eq->sample_rate / 2 ) ) { break; }

			int band_idx = 0;
			for ( int b = 0; b < eq->num_bands; b++ )
			{
				if ( ( freq >= eq->band_freqs[ b ] )&&( freq < eq->band_freqs[ b + 1 ] ) )
				{
					band_idx = b;
					break;
				}
			}

			if ( fabs( eq->gains[ band_idx ] - 1.0 ) > 1e-6 )
			{
				modified = n_posix_true;

				buffer[ i ] *= eq->gains[ band_idx ];
				if ( ( i > 0 )&&( i < ( fft_size / 2 ) ) )
				{
					buffer[ fft_size - i ] *= eq->gains[ band_idx ];
				}
			}
		}


		if ( modified )
		{
			n_fft_ifft( buffer, fft_size );

			for ( int i = 0; i < fft_size; i++ )
			{
				int idx = start_idx + i;
				if ( idx < padded_size )
				{
					double w = eq->window[ i ];
					padded_output    [ idx ] += creal( buffer[ i ] ) * w;
					padded_window_sum[ idx ] += w * w;
				}
			}
		} else {
			for ( int i = 0; i < fft_size; i++ )
			{
				int idx = start_idx + i;
				if ( idx < padded_size )
				{
					double w = eq->window[ i ];
					padded_output    [ idx ] += padded_input[ i ] * w * w;
					padded_window_sum[ idx ] += w * w;
				}
			}
		}

#else

		double freq_resolution = (double) eq->sample_rate / fft_size;

		for ( int i = 0; i < fft_size / 2; i++ )
		{
			double freq = i * freq_resolution;

			int band_idx = 0;
			for ( int b = 0; b < eq->num_bands; b++ )
			{
				if ( ( freq >= eq->band_freqs[ b ] )&&( freq <= eq->band_freqs[ b + 1] ) )
				{
					band_idx = b;
					break;
				}
			}

			double gain = eq->gains[ band_idx ];
			buffer[ i ] *= gain;

			if ( ( i > 0 )&&( i < fft_size / 2 ) )
			{
				buffer[ fft_size - i ] *= gain;
			}

		}

		n_fft_ifft( buffer, fft_size );

		for ( int i = 0; i < fft_size; i++ )
		{
			int idx = start_idx + i;
			if ( idx < count )
			{
				output[ idx ] += creal( buffer[ i ] ) * eq->window[ i ];
			}
		}

#endif

	}


#ifdef N_FFT_VERSION_2

	for ( int i = 0; i < padded_size; i++ )
	{
		if ( padded_window_sum[ i ] > 1e-6 )
		{
			padded_output[ i ] /= padded_window_sum[ i ];
		}
	}

	memcpy( audio, padded_output + fft_size, count * sizeof( double ) );

	free( output     );
	free( window_sum );

	free( padded_input      );
	free( padded_output     );
	free( padded_window_sum );

#else

	for ( int i = 0; i < count; i++ )
	{
		audio[ i ] = output[ i ];
	}

	free( output );

#endif

	return;
}





// internal
void
n_fft_equalizer_apply_channel( n_fft_equalizer *eq, float *data32, int num_channels, int num_samples, int ch )
{

	double *audio = (double*) malloc( num_samples * sizeof( double ) );

	for ( int i = 0; i < num_samples; i++ )
	{
		audio[ i ] = (double) data32[ ( i * num_channels ) + ch ];
	}

	n_fft_equalizer_channel_process( eq, audio, num_samples, ch );

	for ( int i = 0; i < num_samples; i++ )
	{
		double sample = audio[ i ];
		if ( sample >  1.0 ) { sample =  1.0; } else
		if ( sample < -1.0 ) { sample = -1.0; }

		data32[ ( i * num_channels ) + ch ] = (float) sample;
	}

	free( audio );


	return;
}

void
n_fft_equalizer_apply( n_wav *wav, double gains_db[], int num_bands )
{

	// [!] : do nothing

	int check = 0;
	for ( int i = 0; i < num_bands; i++ )
	{
		if ( gains_db[ i ] == 0 ) { check++; }
	}
	if ( num_bands == check ) { return; }


	int num_channels    = N_WAV_STEREO( wav );
	//int bits_per_sample = N_WAV_BIT( wav );
	int sample_rate     = N_WAV_RATE( wav );
	int num_samples     = N_WAV_COUNT( wav );


	int fft_size = 2048;
	n_fft_equalizer *eq = n_fft_equalizer_init( sample_rate, num_bands, fft_size );


	for ( int i = 0; i < num_bands; i++ )
	{
		n_fft_equalizer_gain_db_set( eq, i, gains_db[ i ] );
	}


	float *data32 = (float*) N_WAV_PTR( wav );


#ifdef N_POSIX_PLATFORM_MAC

	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_equalizer_apply_channel( eq, data32, num_channels, num_samples, 0 );

		}];
		[n_wav_queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_equalizer_apply_channel( eq, data32, num_channels, num_samples, 1 );

		}];
		[n_wav_queue addOperation:o];
	}

	[n_wav_queue waitUntilAllOperationsAreFinished];

#else

	n_fft_equalizer_apply_channel( eq, data32, num_channels, num_samples, 0 );
	n_fft_equalizer_apply_channel( eq, data32, num_channels, num_samples, 1 );

#endif


	n_fft_equalizer_exit( eq );


	return;
}




void
n_fft_test( n_wav *wav )
{

	int num_bands = 10;

	double gains_db[] = {

		6.0,	// 20-50Hz
		3.0,	// 50-125Hz
		0.0,	// 125-315Hz
		0.0,	// 315-800Hz
		0.0,	// 800-2000Hz
		0.0,	// 2000-5000Hz
		0.0,	// 5000-12500Hz
		0.0,	// 12500-20000Hz
		-3.0,	// high freq
		-6.0	// highest freq

	};

	n_fft_equalizer_apply( wav, gains_db, num_bands );


	return;
}


#endif // _H_NONNON_NEUTRAL_FFT
