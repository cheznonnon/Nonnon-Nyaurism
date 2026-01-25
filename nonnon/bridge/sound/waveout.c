// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Link : -lwinmm


// [ Mechanism ]
//
//	Win95   : available only one HWAVEOUT
//	Win2000 : available multiple HWAVEOUTs




#ifndef _H_NONNON_WIN32_GAME_SOUND_WAVEOUT
#define _H_NONNON_WIN32_GAME_SOUND_WAVEOUT




#include "../../neutral/posix.c"
#include "../../neutral/wav.c"

#include "../../win32/sysinfo/version.c"

#ifdef _MSC_VER
#pragma comment( lib, "winmm" )
#endif // #ifdef _MSC_VER




typedef struct {

	HWAVEOUT     hwo;
	WAVEHDR      wh;
	n_posix_bool is_chicago;

} n_waveout;


static HWAVEOUT n_waveout_hwo = NULL;




#define n_waveout_hwaveout_exit( hwo ) waveOutClose( hwo )

HWAVEOUT
n_waveout_hwaveout_init( void )
{

	const WAVEFORMATEX fmt = {

		1,
		2,
		44100,
		44100 * 4,
		4,
		16,
		sizeof( WAVEFORMATEX )

	};

	HWAVEOUT hwo; waveOutOpen( &hwo, WAVE_MAPPER, &fmt, 0,0,CALLBACK_NULL );


	return hwo;
}




#define n_waveout_zero( p ) n_memory_zero( p, sizeof( n_waveout ) )

n_posix_bool
n_waveout_init( n_waveout *p, n_wav *wav )
{

	p->wh         = N_WAV_WH( wav );
	p->is_chicago = n_sysinfo_version_9x();

	if ( p->is_chicago )
	{

		if ( n_waveout_hwo == NULL )
		{
			n_waveout_hwo = n_waveout_hwaveout_init();
		}

		p->hwo = n_waveout_hwo;

	} else {

		p->hwo = n_waveout_hwaveout_init();

	}


	N_WAV_LOOP( wav ) = 1;
	waveOutPrepareHeader( p->hwo, &p->wh, sizeof( WAVEHDR ) );


	return n_posix_false;
}

n_posix_bool
n_waveout_loop( n_waveout *p )
{

	waveOutReset( p->hwo );
	waveOutWrite( p->hwo, &p->wh, sizeof( WAVEHDR ) );


	return n_posix_false;
}

n_posix_bool
n_waveout_stop( n_waveout *p )
{

	waveOutReset( p->hwo );


	return n_posix_false;
}

n_posix_bool
n_waveout_exit( n_waveout *p )
{

	// [!] : anti-fault : before waveOutUnprepareHeader()

	waveOutReset( p->hwo );
	waveOutUnprepareHeader( p->hwo, &p->wh, sizeof( WAVEHDR ) );


	if ( p->is_chicago )
	{

		if ( n_waveout_hwo != NULL )
		{
			n_waveout_hwaveout_exit( n_waveout_hwo );
			n_waveout_hwo = NULL;
		}

	} else {

		n_waveout_hwaveout_exit( p->hwo );

	}

	n_waveout_zero( p );


	return n_posix_false;
}

n_posix_bool
n_waveout_pause( n_waveout *p )
{

	waveOutPause( p->hwo );


	return n_posix_false;
}

n_posix_bool
n_waveout_resume( n_waveout *p )
{

	waveOutRestart( p->hwo );


	return n_posix_false;
}


#endif // _H_NONNON_WIN32_GAME_SOUND_WAVEOUT

