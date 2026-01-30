// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Link : -lole32 -lwinmm
//
//	gcc -c sound.c -o obj.o
//	gcc -mwindows obj.o -o test.exe -lole32 -lwinmm
//	pause


// [x] : not so simple
//
//	MCI doesn't support multi-thread
//
//	PlaySound() hasn't mixing function
//
//	Win9x          : MCI will be heavy at looping
//	Vista or later : MCI and waveOut will be too much heavy


// [ Mechanism ]
//
//	Device Name | Formats   | Data Types    | Mix  | suit for
//	-----------------------------------------------------------
//	MCI         | MIDI/WAVE | file only     |  1   | BGM
//	waveOut     |      WAVE | resource/file |  1   | Effects
//	DirectSound |      WAVE | resource/file | many | Effects


// [!] : HowTo : Win32 : loading from resources
//
//	when using n_game_rc_load_wav() in n_game_sound_init()
//
//	first  : #include "rc.c"
//	second : #include "sound.c"
//
//	resource section name needs to be "DATA"


// [!] : HowTo : Win64 : loading from resources
//
//	when using n_win64_resource_load_nwav() in n_game_sound_init()
//
//	first  : #include "nonnon/win64/resource.c"
//	second : #include "sound.c"
//
//	resource section name needs to be "DATA"


// [!] : 64-bit Version : Win10
//
//	put n_game_sound_init() at n_game_loop() or else playback will stop only once




#ifndef _H_NONNON_WIN32_GAME_SOUND
#define _H_NONNON_WIN32_GAME_SOUND




#ifdef N_POSIX_PLATFORM_MINGW

#include "../win32/sysinfo/version.c"

#endif


#include "./sound/directsound.c"
#include "./sound/mci.c"
#include "./sound/waveout.c"
#include "./sound/vfw.c"




#define N_GAME_SOUND_MODE_MCI         ( 0 )
#define N_GAME_SOUND_MODE_VFW         ( 1 )
#define N_GAME_SOUND_MODE_DIRECTSOUND ( 2 )
#define N_GAME_SOUND_MODE_WAVEOUT     ( 3 )




#pragma pack( push, 1 )

typedef struct {

	int           mode;
	n_mci         mci;
	n_vfw         vfw;
	n_waveout     wo;
	n_directsound ds;
	n_wav         wav;
	u32           tmr;
	u32           len;

} n_game_sound;

#pragma pack( pop )




#define n_game_sound_zero(      s    ) n_memory_zero( s, sizeof( n_game_sound )       )
#define n_game_sound_bulk_zero( s, c ) n_memory_zero( s, sizeof( n_game_sound ) * (c) )

n_posix_bool
n_game_sound_timer( n_game_sound *s )
{

	u32 cur = n_posix_tickcount();


	if ( ( cur - s->tmr ) >= s->len )
	{
		s->tmr = cur;
		return n_posix_true;
	}


	return n_posix_false;
}

#define n_game_sound_init_literal( a, b, c ) n_game_sound_init( a, b, n_posix_literal( c ) )

void
n_game_sound_init( n_game_sound *s, HWND hwnd_parent, const n_posix_char *fname )
{

	// [!] : if DirectSound does not work, set "hwnd_parent" NULL


	s->tmr = 0;


	n_posix_bool ret = n_posix_false;

#ifdef _H_NONNON_WIN32_GAME_RC
	ret = n_game_rc_load_wav( &s->wav, fname );
#endif

#ifdef _H_NONNON_WIN64_RESOURCE
	ret = n_win64_resource_load_nwav( fname, &s->wav );
#endif
	if ( ret ) { n_wav_load( &s->wav, fname ); }


	n_posix_bool n_wav_safemode_prev = n_wav_safemode; n_wav_safemode = n_posix_true;
	if ( n_wav_error( &s->wav, -100 ) )
	{

		if ( n_vfw_load( &s->vfw, hwnd_parent, fname ) )
		{
			s->mode = N_GAME_SOUND_MODE_MCI;
			n_mci_init( &s->mci, fname );
		} else {
			s->mode = N_GAME_SOUND_MODE_VFW;
		}

	} else {

		// [x] : Win9x/2000/XP : DirectSound is unstable

		n_posix_bool ret = n_posix_true;

#ifdef N_POSIX_PLATFORM_MINGW
		if ( n_sysinfo_version_vista_or_later() )
#endif
		{
			s->mode = N_GAME_SOUND_MODE_DIRECTSOUND;
			ret = n_directsound_init( &s->ds, hwnd_parent, &s->wav );
		}

		if ( ret )
		{
			s->mode = N_GAME_SOUND_MODE_WAVEOUT;
			n_waveout_init( &s->wo, &s->wav );
		}

	}
	n_wav_safemode = n_wav_safemode_prev;


	return;
}

void
n_game_sound_loop( n_game_sound *s )
{

	if ( s->mode == N_GAME_SOUND_MODE_MCI )
	{
		n_mci_loop( &s->mci );
		s->len = s->mci.length;
	} else
	if ( s->mode == N_GAME_SOUND_MODE_VFW )
	{
		n_vfw_rewind( &s->vfw );
		n_vfw_play( &s->vfw );
		s->len = MCIWndGetEnd( s->vfw.hwnd );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_DIRECTSOUND )
	{
		n_directsound_loop( &s->ds );
		s->len = N_WAV_MSEC( &s->wav );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_WAVEOUT )
	{
		n_waveout_loop( &s->wo );
		s->len = N_WAV_MSEC( &s->wav );
	}

	s->tmr = n_posix_tickcount();


	return;
}

void
n_game_sound_exit( n_game_sound *s )
{

	if ( s->mode == N_GAME_SOUND_MODE_MCI )
	{
		n_mci_exit( &s->mci );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_VFW )
	{
		n_vfw_free( &s->vfw );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_DIRECTSOUND )
	{
		n_directsound_exit( &s->ds );
		n_wav_free( &s->wav );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_WAVEOUT )
	{
		n_waveout_exit( &s->wo );
		n_wav_free( &s->wav );
	}

	n_game_sound_zero( s );


	return;
}

void
n_game_sound_stop( n_game_sound *s )
{

	if ( s->mode == N_GAME_SOUND_MODE_MCI )
	{
		n_mci_stop( &s->mci );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_VFW )
	{
		n_vfw_stop( &s->vfw );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_DIRECTSOUND )
	{
		n_directsound_stop( &s->ds );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_WAVEOUT )
	{
		n_waveout_stop( &s->wo );
	}


	return;
}

void
n_game_sound_pause( n_game_sound *s )
{

	if ( s->mode == N_GAME_SOUND_MODE_MCI )
	{
		n_mci_pause( &s->mci );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_VFW )
	{
		n_vfw_pause( &s->vfw );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_DIRECTSOUND )
	{
		n_directsound_pause( &s->ds );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_WAVEOUT )
	{
		n_waveout_pause( &s->wo );
	}


	return;
}

void
n_game_sound_resume( n_game_sound *s )
{

	if ( s->mode == N_GAME_SOUND_MODE_MCI )
	{
		n_mci_resume( &s->mci );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_VFW )
	{
		n_vfw_resume( &s->vfw );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_DIRECTSOUND )
	{
		n_directsound_resume( &s->ds );
	} else
	if ( s->mode == N_GAME_SOUND_MODE_WAVEOUT )
	{
		n_waveout_resume( &s->wo );
	}


	return;
}


#endif // _H_NONNON_WIN32_GAME_SOUND

