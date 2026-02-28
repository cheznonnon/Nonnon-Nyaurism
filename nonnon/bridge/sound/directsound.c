// Nonnon DirectSound
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Link : -lole32




#ifndef _H_NONNON_WIN32_GAME_SOUND_DIRECTSOUND
#define _H_NONNON_WIN32_GAME_SOUND_DIRECTSOUND




#include "../../neutral/posix.c"
#include "../../neutral/wav.c"


#include <ole2.h>

#ifdef _MSC_VER
#pragma comment( lib, "ole32" )
#endif // #ifdef _MSC_VER


#include <dsound.h>




const GUID GUID_NULL               = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };
const GUID CLSID_DirectSound       = { 0x47d4d946, 0x62e8, 0x11cf, { 0x93, 0xbc, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };
const GUID IID_IDirectSoundBuffer  = { 0x279AFA85, 0x4981, 0x11CE, { 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60 } };
const GUID IID_IDirectSound        = { 0x279AFA83, 0x4981, 0x11CE, { 0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60 } };
const GUID CLSID_DirectSound8      = { 0x3901cc3f, 0x84b5, 0x4fa4, { 0xba, 0x35, 0xaa, 0x81, 0x72, 0xb8, 0xa0, 0x9b } };
const GUID IID_IDirectSoundBuffer8 = { 0x6825a449, 0x7524, 0x4d82, { 0x92, 0x0f, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e } };
const GUID IID_IDirectSound8       = { 0xC50A7E93, 0xF395, 0x4834, { 0x9E, 0xF6, 0x7F, 0xA9, 0x9D, 0xE5, 0x09, 0x66 } };
const GUID GUID_All_Objects        = { 0xaa114de5, 0xc262, 0x4169, { 0xa1, 0xc8, 0x23, 0xd6, 0x98, 0xcc, 0x73, 0xb5 } };
const GUID DSDEVID_DefaultPlayback = { 0xdef00000, 0x9c6d, 0x47ed, { 0xaa, 0xf1, 0x4d, 0xda, 0x8f, 0x2b, 0x5c, 0x03 } };




typedef struct {

	IDirectSound       *ds;
	IDirectSoundBuffer *dsb;

	DWORD                dw1, dw2;

} n_directsound;




#define n_directsound_zero( p ) n_memory_zero( p, sizeof( n_directsound ) )

BOOL
n_directsound_exit( n_directsound *p )
{

	// [!] : holding p->ds is needed to playback

	if ( p->dsb != NULL )
	{
		IDirectSoundBuffer_Stop   ( p->dsb );
		IDirectSoundBuffer_Release( p->dsb );
	}

	if ( p->ds != NULL )
	{
		IDirectSound_Release( p->ds );
	}

	n_directsound_zero( p );


	CoUninitialize();


	return FALSE;
}

BOOL
n_directsound_init( n_directsound *p, HWND hwnd, n_wav *wav )
{

	// [!] : hwnd is always nedded else no sound will be output

	if ( hwnd == NULL ) { return TRUE; }


	CoInitializeEx( NULL, 0 );


	// Device

	CoCreateInstance
	(
		&CLSID_DirectSound,
		NULL, 
		CLSCTX_INPROC_SERVER,
		&IID_IDirectSound,
		(LPVOID*) &p->ds
	);
	if ( p->ds == NULL )
	{
//n_posix_debug_literal( " IID_IDirectSound " );

		return TRUE;
	}

	IDirectSound_Initialize( p->ds, NULL );
	IDirectSound_SetCooperativeLevel( p->ds, hwnd, DSSCL_PRIORITY );


	// Buffer

	// [x] : error : DSBCAPS_LOCHARDWARE

	// [Needed] : DSBCAPS_LOCDEFER | DSBCAPS_GLOBALFOCUS
	//
	//	DSBCAPS_LOCDEFER    : important for playback
	//	DSBCAPS_GLOBALFOCUS : sound will be cut while WM_CREATE

	WAVEFORMATEX fmt   = N_WAV_FMT( wav );
	int          caps1 = DSBCAPS_LOCDEFER | DSBCAPS_GLOBALFOCUS;
	int          caps2 = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	int          byte  = N_WAV_SIZE( wav );
	DSBUFFERDESC bd    = { sizeof( DSBUFFERDESC ), caps1 | caps2, byte, 0, &fmt, GUID_NULL };

	IDirectSound_CreateSoundBuffer( p->ds, &bd, (void*) &p->dsb, NULL );

	if ( p->dsb == NULL )
	{
//n_posix_debug_literal( " IDirectSound_CreateSoundBuffer() " );

		n_directsound_exit( p );

		return TRUE;
	}


	VOID  *buf = NULL;
	DWORD  len = 0;

	IDirectSoundBuffer_Lock  ( p->dsb, 0,0, &buf,&len, NULL,NULL, DSBLOCK_ENTIREBUFFER );
//n_posix_debug_literal( "%x : %d %d", buf,len, byte );

	n_memory_copy( N_WAV_PTR( wav ), buf,len );

	IDirectSoundBuffer_Unlock( p->dsb, buf,len, NULL,0 );


	// Misc

	IDirectSoundBuffer_SetVolume( p->dsb, 0 );


	return FALSE;
}

BOOL
n_directsound_loop( n_directsound *p )
{

	if ( p->dsb == NULL ) { return TRUE; }

	IDirectSoundBuffer_SetCurrentPosition( p->dsb, 0 );
	IDirectSoundBuffer_Play( p->dsb, 0, 0xffffffff, 0 );


	return FALSE;
}

BOOL
n_directsound_stop( n_directsound *p )
{

	if ( p->dsb == NULL ) { return TRUE; }

	IDirectSoundBuffer_Stop( p->dsb );


	return FALSE;
}

BOOL
n_directsound_pause( n_directsound *p )
{

	if ( p->dsb == NULL ) { return TRUE; }

	IDirectSoundBuffer_GetCurrentPosition( p->dsb, &p->dw1, &p->dw2 );
	IDirectSoundBuffer_Stop( p->dsb );


	return FALSE;
}

BOOL
n_directsound_resume( n_directsound *p )
{

	if ( p->dsb == NULL ) { return TRUE; }

	IDirectSoundBuffer_SetCurrentPosition( p->dsb, p->dw1 );
	IDirectSoundBuffer_Play( p->dsb, 0, 0xffffffff, 0 );


	return FALSE;
}


#endif // _H_NONNON_WIN32_GAME_SOUND_DIRECTSOUND

