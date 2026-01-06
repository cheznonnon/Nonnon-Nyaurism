// Nonnon Nyaurism
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WAV_ERROR
#define _H_NONNON_NEUTRAL_WAV_ERROR




#include "../wav.c"




n_posix_bool
n_wav_error_format( const n_wav *wav )
{

	if ( n_wav_error( wav, -2 ) ) { return n_posix_true; }


	if ( 2 != N_WAV_STEREO( wav ) ) { return n_posix_true; }

	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_PCM )
	{
		if ( 16 != N_WAV_BIT( wav ) ) { return n_posix_true; }
	} else
	if ( N_WAV_FORMAT_DEFAULT == N_WAV_FORMAT_FLOAT )
	{
		if ( 32 != N_WAV_BIT( wav ) ) { return n_posix_true; }
	}

	if ( 44100 != N_WAV_RATE  ( wav ) ) { return n_posix_true; }


	return n_posix_false;
}




#endif // _H_NONNON_NEUTRAL_WAV_ERROR

