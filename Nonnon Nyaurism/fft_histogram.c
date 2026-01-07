// Nonnon Nyaurism for Mac
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Thx : DeepSeek AI


// [x] : currently unstable with some data




// internal
void
n_fft_histogram_channel_process( n_fft_equalizer *eq, float *data, int ch )
{

	double *audio = (double*) malloc( eq->sample_count * sizeof( double ) );

	for( int i = 0; i < eq->sample_count; i++ )
	{
		audio[ i ] = (double) data[ ( i * eq->channel_count ) + ch ];
	}


	n_fft_complex *buffer;
	if ( ch == 0 ) { buffer = eq->fft_buffer_l; } else { buffer = eq->fft_buffer_r; }


	int hop_size = N_FFT_SIZE / 4; // [!] : 75%


	int num_blocks = ( eq->sample_count - N_FFT_SIZE ) / hop_size + 1;

	for( int block = 0; block < num_blocks; block++ )
	{
		int start_idx = block * hop_size;

		for( int i = 0; i < N_FFT_SIZE; i++ )
		{
			int idx = start_idx + i;

			if ( idx < eq->sample_count )
			{
				buffer[ i ] = audio[ idx ];
			} else {
				buffer[ i ] = 0.0;
			}
		}

		n_fft_encode( buffer, N_FFT_SIZE );
	}


	free( audio );


	return;
}

// internal
n_fft_equalizer*
n_fft_histogram_make( n_wav *wav )
{

	int band_count = 10;

	float *data = (float*) N_WAV_PTR( wav );

	int channels = N_WAV_STEREO( wav );
	int samples  = N_WAV_COUNT ( wav );

	n_fft_equalizer *eq = n_fft_equalizer_init( band_count, channels, samples );


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
n_fft_histogram_main( n_wav *wav, double *histogram, int histogram_count, BOOL debug )
{

	int    N  = N_FFT_SIZE;
	double fs = N_FFT_RATE;
	double ny = N_FFT_RATE / 2;
	int    ch = N_WAV_STEREO( wav );

	n_wav wav_copy; n_wav_carboncopy( wav, &wav_copy );

	if ( N > N_WAV_COUNT( wav ) )
	{
		n_wav_resizer( &wav_copy, 1000, N_WAV_RESIZER_CENTER );
	}

	n_wav_normalize( &wav_copy, 0, 1.0, 1.0 );

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

		for( int i = 0; i < N; i++ ) { dc_offset_l += ptr[ ( i * ch ) + 0 ]; }
		for( int i = 0; i < N; i++ ) { dc_offset_r += ptr[ ( i * ch ) + 1 ]; }

		dc_offset_l /= N;
		dc_offset_r /= N;

		for( int i = 0; i < N; i++ ) { ptr[ ( i * ch ) + 0 ] -= dc_offset_l; }
		for( int i = 0; i < N; i++ ) { ptr[ ( i * ch ) + 1 ] -= dc_offset_r; }

		if ( debug )
		{
			printf( "✓ DC offset is removed: %.6f %.6f\n", dc_offset_l, dc_offset_r );
		}
	}


	// 1.2 Apply Hann Window Function

	double window_correction = 0.0;

	for( int i = 0; i < N; i++ )
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

	n_fft_equalizer *eq = n_fft_histogram_make( &wav_copy );

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

	int     spectrum_size      = N / 2 + 1;
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


	if ( debug )
	{
		printf( "\n[ Completed ] Clean-up\n" );
		printf( "----------------------------------------\n" );
	}


	return;
}

