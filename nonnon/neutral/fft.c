// Nonnon Nyaurism
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Thx : DeepSeek AI


// [!] : you need to implemenmt n_fft_equalizer_apply() only
//
//	usage is written in n_fft_test()


// [!] : n_fft_histogram_main()
//
//	usage is written in n_fft_histogram_test()




#ifndef _H_NONNON_NEUTRAL_FFT
#define _H_NONNON_NEUTRAL_FFT




// [!] : Version 1 : simple and lightweight but always amplified
// [!] : Version 2 : normalizer and edge noise preventer are implemented

#define N_FFT_VERSION_2




#include "./wav.c"




#include <complex.h>




#define N_FFT_RATE    ( 44100 )
#define N_FFT_NYQUIST ( N_FFT_RATE / 2.0 )




typedef double complex n_fft_complex;




typedef struct {

	int            band_count;
	double        *band_gains;		// [!] : not db
	double        *band_freqs;
	n_fft_complex *fft_buffer_l;
	n_fft_complex *fft_buffer_r;
	double        *window;

#ifdef N_FFT_VERSION_2
	double        *gains_db;
	n_posix_bool   is_bypass;
#endif

	int            resolution;
	int            channel_count;
	int            sample_count;

} n_fft_equalizer;




#define n_fft_encode( b, n ) n_fft_engine( b, n, 0 )
#define n_fft_decode( b, n ) n_fft_engine( b, n, 1 )

void
n_fft_engine( n_fft_complex *buffer, int n, int inverse )
{

	// [!] : Cooley-Tukey algorithm


	if ( n <= 1 ) { return; }


	int half = n / 2;


	n_fft_complex *even = (n_fft_complex*) malloc( half * sizeof( n_fft_complex ) );
	n_fft_complex * odd = (n_fft_complex*) malloc( half * sizeof( n_fft_complex ) );

	for( int i = 0; i < half; i++ )
	{
		even[ i ] = buffer[ ( i * 2 ) + 0 ];
		 odd[ i ] = buffer[ ( i * 2 ) + 1 ];
	}


	n_fft_engine( even, half, inverse );
	n_fft_engine(  odd, half, inverse );


	for( int k = 0; k < n/2; k++ )
	{
		double        angle = 2 * M_PI * k / n * ( inverse ? -1 : 1 );
		n_fft_complex     t = cexp( I * angle ) * odd[ k ];

		buffer[ k        ] = even[ k ] + t;
		buffer[ k + half ] = even[ k ] - t;

		if ( inverse )
		{
			buffer[ k        ] /= 2;
			buffer[ k + half ] /= 2;
		}
	}


	free( even );
	free(  odd );


	return;
}




// internal
n_fft_equalizer*
n_fft_equalizer_init( int band_count, int channel_count, int sample_count, int resolution )
{

	n_fft_equalizer *eq = (n_fft_equalizer*) malloc( sizeof( n_fft_equalizer ) );

	eq->band_count = band_count;
	eq->band_gains = (double*) malloc( band_count * sizeof( double ) );

	eq->channel_count = channel_count;
	eq->sample_count  = sample_count;
	eq->resolution    = resolution;

#ifdef N_FFT_VERSION_2
	eq->is_bypass = n_posix_true;
	eq->gains_db  = (double*) malloc( band_count * sizeof( double ) );
#endif

	for( int i = 0; i < band_count; i++ )
	{
		eq->band_gains[ i ] = 1.0; // [!] : 1.0 = 0dB
#ifdef N_FFT_VERSION_2
		eq->gains_db[ i ] = 0.0;
#endif
	}


	eq->band_freqs = (double*) malloc( ( band_count + 1 ) * sizeof( double ) );

	double min_freq = 20.0;			// [!] : 20Hz : hard to implement under 20Hz
	double max_freq = N_FFT_NYQUIST;
	double log_min  = log10( min_freq );
	double log_max  = log10( max_freq );
	double step     = ( log_max - log_min ) / band_count;

	for( int i = 0; i <= band_count; i++ )
	{
		eq->band_freqs[ i ] = pow( 10.0, log_min + i * step );
	}


	eq->fft_buffer_l = (n_fft_complex*) malloc( eq->resolution * sizeof( n_fft_complex ) );
	eq->fft_buffer_r = (n_fft_complex*) malloc( eq->resolution * sizeof( n_fft_complex ) );


	// window function

	eq->window = (double*) malloc( eq->resolution * sizeof( double ) );

	for( int i = 0; i < eq->resolution; i++ )
	{
		eq->window[ i ] = 0.5 * ( 1.0 - cos( 2.0 * M_PI * i / ( eq->resolution - 1 ) ) );
	}


	return eq;
}

// internal
void
n_fft_equalizer_exit( n_fft_equalizer *eq )
{

	free( eq->band_gains );
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

	if ( ( band >= 0 )&&( band < eq->band_count ) )
	{
		eq->band_gains[ band ] = n_fft_equalizer_db2linear( gain_db );

#ifdef N_FFT_VERSION_2
		eq->gains_db[ band ] = gain_db;

		eq->is_bypass = n_posix_true;
		for( int i = 0; i < eq->band_count; i++ )
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
n_fft_equalizer_channel_process( n_fft_equalizer *eq, double *audio, int ch )
{

#ifdef N_FFT_VERSION_2
	if ( eq->is_bypass ) { return; }

	n_posix_bool all_unity = n_posix_true;
	for( int i = 0; i < eq->band_count; i++ )
	{
		if ( fabs( eq->band_gains[ i ] - 1.0 ) > 1e-6 )
		{
			all_unity = n_posix_false;
			break;
		}
	}
	if ( all_unity ) return;
#endif


	n_fft_complex *buffer;
	if ( ch == 0 ) { buffer = eq->fft_buffer_l; } else { buffer = eq->fft_buffer_r; }


	int hop_size = eq->resolution / 4; // [!] : 75%


	double *output = (double*) calloc( eq->sample_count, sizeof( double ) );

#ifdef N_FFT_VERSION_2
	double *window_sum = (double*) calloc( eq->sample_count, sizeof( double ) );

	int     padded_size       = eq->sample_count + ( eq->resolution * 2 );
	double *padded_input      = (double*) calloc( padded_size, sizeof( double ) );
	double *padded_output     = (double*) calloc( padded_size, sizeof( double ) );
	double *padded_window_sum = (double*) calloc( padded_size, sizeof( double ) );

	memcpy( padded_input + eq->resolution, audio, eq->sample_count * sizeof( double ) );

	int padded_blocks = ( ( padded_size - eq->resolution ) / hop_size ) + 1;
#endif

#ifdef N_FFT_VERSION_2
	for( int block = 0; block < padded_blocks; block++ )
#else
	int num_blocks = ( eq->sample_count - eq->resolution ) / hop_size + 1;

	for( int block = 0; block <    num_blocks; block++ )
#endif
	{
		int start_idx = block * hop_size;

		for( int i = 0; i < eq->resolution; i++ )
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

		n_fft_encode( buffer, eq->resolution );

#ifdef N_FFT_VERSION_2

		n_posix_bool modified = n_posix_false;

		double freq_resolution = (double) N_FFT_RATE / eq->resolution;

		for( int i = 0; i <= eq->resolution / 2; i++ )
		{
			double freq = i * freq_resolution;
			if ( freq > ( N_FFT_RATE / 2 ) ) { break; }

			int band_idx = 0;
			for( int b = 0; b < eq->band_count; b++ )
			{
				if ( ( freq >= eq->band_freqs[ b ] )&&( freq < eq->band_freqs[ b + 1 ] ) )
				{
					band_idx = b;
					break;
				}
			}

			if ( fabs( eq->band_gains[ band_idx ] - 1.0 ) > 1e-6 )
			{
				modified = n_posix_true;

				buffer[ i ] *= eq->band_gains[ band_idx ];
				if ( ( i > 0 )&&( i < ( eq->resolution / 2 ) ) )
				{
					buffer[ eq->resolution - i ] *= eq->band_gains[ band_idx ];
				}
			}
		}


		if ( modified )
		{
			n_fft_decode( buffer, eq->resolution );

			for( int i = 0; i < eq->resolution; i++ )
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
			for( int i = 0; i < eq->resolution; i++ )
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

		double freq_resolution = (double) eq->sample_rate / eq->resolution;

		for( int i = 0; i < eq->resolution/2; i++ )
		{
			double freq = i * freq_resolution;

			int band_idx = 0;
			for( int b = 0; b < eq->band_count; b++ )
			{
				if ( ( freq >= eq->band_freqs[ b ] )&&( freq <= eq->band_freqs[ b + 1 ] ) )
				{
					band_idx = b;
					break;
				}
			}

			double gain = eq->band_gains[ band_idx ];
			buffer[ i ] *= gain;

			if ( ( i > 0 )&&( i < eq->resolution/2 ) )
			{
				buffer[ eq->resolution - i ] *= gain;
			}

		}

		n_fft_ifft( buffer, eq->resolution );

		for( int i = 0; i < eq->resolution; i++ )
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

	for( int i = 0; i < padded_size; i++ )
	{
		if ( padded_window_sum[ i ] > 1e-6 )
		{
			padded_output[ i ] /= padded_window_sum[ i ];
		}
	}

	memcpy( audio, padded_output + eq->resolution, eq->sample_count * sizeof( double ) );

	free( output     );
	free( window_sum );

	free( padded_input      );
	free( padded_output     );
	free( padded_window_sum );

#else

	for( int i = 0; i < eq->sample_count; i++ )
	{
		audio[ i ] = output[ i ];
	}

	free( output );

#endif

	return;
}





// internal
void
n_fft_equalizer_apply_channel( n_fft_equalizer *eq, void *data, int ch )
{

	double *audio = (double*) malloc( eq->sample_count * sizeof( double ) );


	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		s16 *ptr = (s16*) data;
		for( int i = 0; i < eq->sample_count; i++ )
		{
			audio[ i ] = (double) ptr[ ( i * eq->channel_count ) + ch ] / SHRT_MAX;
		}
	} else {
		float *ptr = (float*) data;
		for( int i = 0; i < eq->sample_count; i++ )
		{
			audio[ i ] = (double) ptr[ ( i * eq->channel_count ) + ch ];
		}
	}

	n_fft_equalizer_channel_process( eq, audio, ch );



	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		s16 *ptr = (s16*) data;
		for( int i = 0; i < eq->sample_count; i++ )
		{
			double sample = n_wav_sample_clamp_normalized( audio[ i ] );
			ptr[ ( i * eq->channel_count ) + ch ] = sample * SHRT_MAX;
		}
	} else {
		float *ptr = (float*) data;
		for( int i = 0; i < eq->sample_count; i++ )
		{
			double sample = n_wav_sample_clamp_normalized( audio[ i ] );
			ptr[ ( i * eq->channel_count ) + ch ] = (float) sample;
		}
	}

	free( audio );


	return;
}

void
n_fft_equalizer_apply( n_wav *wav, double band[], int band_count, int resolution )
{

	// [!] : do nothing

	int check = 0;
	for( int i = 0; i < band_count; i++ )
	{
		if ( band[ i ] == 0 ) { check++; }
	}
	if ( band_count == check ) { return; }


	int channels = N_WAV_STEREO( wav );
	int samples  = N_WAV_COUNT( wav );

	n_fft_equalizer *eq = n_fft_equalizer_init( band_count, channels, samples, resolution );


	for( int i = 0; i < band_count; i++ )
	{
		n_fft_equalizer_gain_db_set( eq, i, band[ i ] );
	}


	float *data = (float*) N_WAV_PTR( wav );


#ifdef N_POSIX_PLATFORM_MAC

	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_equalizer_apply_channel( eq, data, 0 );

		}];
		[n_wav_queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_equalizer_apply_channel( eq, data, 1 );

		}];
		[n_wav_queue addOperation:o];
	}

	[n_wav_queue waitUntilAllOperationsAreFinished];

#else

	n_fft_equalizer_apply_channel( eq, data, 0 );
	n_fft_equalizer_apply_channel( eq, data, 1 );

#endif


	n_fft_equalizer_exit( eq );


	return;
}




void
n_fft_test( n_wav *wav )
{

	int band_count = 10;

	double band[] = {

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

	n_fft_equalizer_apply( wav, band, band_count, 2048 );


	return;
}




// internal
void
n_fft_histogram_channel_process( n_fft_equalizer *eq, float *data, int ch )
{

	// [!] : IEEE_FLOAT only

	double *audio = (double*) malloc( eq->sample_count * sizeof( double ) );

	for( int i = 0; i < eq->sample_count; i++ )
	{
		audio[ i ] = (double) data[ ( i * eq->channel_count ) + ch ];
	}


	n_fft_complex *buffer;
	if ( ch == 0 ) { buffer = eq->fft_buffer_l; } else { buffer = eq->fft_buffer_r; }


	int hop_size = eq->resolution / 4; // [!] : 75%


	int num_blocks = ( eq->sample_count - eq->resolution ) / hop_size + 1;

	for( int block = 0; block < num_blocks; block++ )
	{
		int start_idx = block * hop_size;

		for( int i = 0; i < eq->resolution; i++ )
		{
			int idx = start_idx + i;

			if ( idx < eq->sample_count )
			{
				buffer[ i ] = audio[ idx ];
			} else {
				buffer[ i ] = 0.0;
			}
		}

		n_fft_encode( buffer, eq->resolution );
	}


	free( audio );


	return;
}

// internal
n_fft_equalizer*
n_fft_histogram_make( n_wav *wav, int band_count, int resolution )
{

	// [!] : IEEE_FLOAT only

	float *data = (float*) N_WAV_PTR( wav );

	int channels = N_WAV_STEREO( wav );
	int samples  = N_WAV_COUNT ( wav );

	n_fft_equalizer *eq = n_fft_equalizer_init( band_count, channels, samples, resolution );


#ifdef N_POSIX_PLATFORM_MAC

	if ( n_wav_queue == NULL ) { n_wav_queue = [[NSOperationQueue alloc] init]; }

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_histogram_channel_process( eq, data, 0 );

		}];
		[n_wav_queue addOperation:o];
	}

	{
		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

			n_fft_histogram_channel_process( eq, data, 1 );

		}];
		[n_wav_queue addOperation:o];
	}

	[n_wav_queue waitUntilAllOperationsAreFinished];

#else

	n_fft_histogram_channel_process( eq, data, 0 );
	n_fft_histogram_channel_process( eq, data, 1 );

#endif


	return eq;
}

void
n_fft_histogram_main( n_wav *wav, double *histogram, int histogram_count, int resolution, n_posix_bool debug )
{

	int    N  = resolution;
	int    C  = N_WAV_COUNT( wav );
	double fs = N_FFT_RATE;
	double ny = N_FFT_RATE / 2;
	int    ch = N_WAV_STEREO( wav );

//NSLog( @"%d %d", C, N ); return;

	n_wav wav_copy; n_wav_carboncopy( wav, &wav_copy );

	if ( N > C )
	{
		double msec = N / fs * 1000;
//NSLog( @"%f", msec );
		n_wav_resizer( &wav_copy, msec, N_WAV_RESIZER_CENTER );
	}

	n_wav_normalize( &wav_copy, 0, 1.0, 1.0 );

	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		n_wav_s16_to_float( &wav_copy );
	}

	float *ptr = (float*) N_WAV_PTR( &wav_copy );


	if ( debug )
	{
		printf( "=== Power Spectrum Analyzer ===\n" );
		printf( "Sample Count: %d, Fraq.: %.0f Hz\n", N, fs );
	}


	if ( debug )
	{
		printf( "\n[ Phase 1 ] Preprrocess\n" );
		printf( "----------------------------------------\n" );
	}


	// 1.1 Remove DC Offset

	{
		double dc_offset_l = 0.0;
		double dc_offset_r = 0.0;

		for( int i = 0; i < C; i++ ) { dc_offset_l += ptr[ ( i * ch ) + 0 ]; }
		for( int i = 0; i < C; i++ ) { dc_offset_r += ptr[ ( i * ch ) + 1 ]; }

		dc_offset_l /= N;
		dc_offset_r /= N;

		for( int i = 0; i < C; i++ ) { ptr[ ( i * ch ) + 0 ] -= dc_offset_l; }
		for( int i = 0; i < C; i++ ) { ptr[ ( i * ch ) + 1 ] -= dc_offset_r; }

		if ( debug )
		{
			printf( "✓ DC offset is removed: %.6f %.6f\n", dc_offset_l, dc_offset_r );
		}
	}


	// 1.2 Apply Hann Window Function

	double window_correction = 0.0;

	for( int i = 0; i < C; i++ )
	{
		double window = 0.5 * ( 1.0 - cos( 2.0 * M_PI * i / ( N - 1 ) ) );

		ptr[ ( i * ch ) + 0 ] *= window;
		ptr[ ( i * ch ) + 1 ] *= window;

		window_correction += window;
	}

	window_correction = N / window_correction;

	if ( debug )
	{
		printf( "✓ Applied (coeff: %.6f)\n", window_correction );
	}


	if ( debug )
	{
		printf( "\n[ Phase 2 ] Run FFT\n" );
		printf( "----------------------------------------\n" );
	}

	n_fft_complex *freq_domain = malloc( N * sizeof( n_fft_complex ) );

	n_fft_equalizer *eq = n_fft_histogram_make( &wav_copy, histogram_count, resolution );

	for( int i = 0; i < N; i++ )
	{
		freq_domain[ i ]  = 0.0;
		freq_domain[ i ] += eq->fft_buffer_l[ i ] * I;
		freq_domain[ i ] += eq->fft_buffer_r[ i ] * I;
	}

	n_wav_free( &wav_copy );

	n_fft_equalizer_exit( eq );


	if ( debug )
	{
		printf( "\n[ Phase 3 ] Calculate Spectrum\n" );
		printf( "----------------------------------------\n" );
	}

	int     spectrum_size      = ( N / 2 ) + 1;
	double *amplitude_spectrum = malloc( spectrum_size * sizeof( double ) );
	double *    power_spectrum = malloc( spectrum_size * sizeof( double ) );

	for( int i = 0; i < spectrum_size; i++ )
	{
		double real = creal( freq_domain[ i ] );
		double imag = cimag( freq_domain[ i ] );

		double amplitude = sqrt( real * real + imag * imag ) * window_correction;

		amplitude_spectrum[ i ] = amplitude;
		    power_spectrum[ i ] = amplitude;//( amplitude * amplitude ) / N;

		if ( debug )
		{
			double freq = i * fs / N;
			printf(
				"  Bin %d (%.1f Hz): Re=%.3f, Im=%.3f, Amp=%.3f\n",
				i, freq, real, imag, amplitude
			);
		}
	}

	free( amplitude_spectrum );

	free( freq_domain );


	if ( debug )
	{
		printf( "\n[ Phase 5 ] Histogram\n" );
		printf( "----------------------------------------\n" );
	}

	if ( histogram != NULL )
	{
		for( int i = 0; i < histogram_count; i++ )
		{
			histogram[ i ] = 0.0;
		}

		double  log_min_freq = log10( 20.0 ); // 20Hz
		double  log_max_freq = log10( ny );
		double  log_range    = log_max_freq - log_min_freq;

		for( int i = 1; i < spectrum_size; i++ )
		{//break;
			double freq = i * fs / N;
			if ( freq >= 20.0 )
			{
				double log_freq = log10( freq );

				int bin_idx = (int) ( ( log_freq - log_min_freq ) / log_range * histogram_count );

				if ( ( bin_idx >= 0 )&&( bin_idx < histogram_count ) )
				{
					histogram[ bin_idx ] += power_spectrum[ i ];
				}
			}
		}
	}

	free( power_spectrum );


	double peak = 0.0;
	for( int i = 0; i < histogram_count; i++ )
	{
		if ( peak < histogram[ i ] ) { peak = histogram[ i ]; }
	}

	if ( peak != 0.0 )
	{
		for( int i = 0; i < histogram_count; i++ )
		{
			histogram[ i ] = histogram[ i ] / peak;
		}
	}


	return;
}

void
n_fft_histogram_test( n_wav *wav )
{
	int count = 10;

	double histogram[ count ];

	n_fft_histogram_main( wav, histogram, count, 2048, YES );

	for( int i = 0; i < count; i++ )
	{
		NSLog( @"%f", histogram[ i ] );
	}

}


#endif // _H_NONNON_NEUTRAL_FFT

