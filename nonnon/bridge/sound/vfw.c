// Marie
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Link : -lvfw32




#ifndef _H_NONNON_WIN32_GAME_SOUND_VFW
#define _H_NONNON_WIN32_GAME_SOUND_VFW



#include <vfw.h>

#ifdef _MSC_VER
#pragma comment( lib, "vfw32" )
#endif // #ifdef _MSC_VER





#define N_VFW_CCH_MAX ( 1024 )




RECT
n_vfw_rect_max( RECT *r, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy )
{

	// [!] : don't return "r" directly

	RECT ret = { 0,0,0,0 };

	if ( r != NULL ) { ret = (*r); }

	ret.left   = n_posix_max_n_type_gfx( ret.left,    x );
	ret.top    = n_posix_max_n_type_gfx( ret.top,     y );
	ret.right  = n_posix_max_n_type_gfx( ret.right,  sx );
	ret.bottom = n_posix_max_n_type_gfx( ret.bottom, sy );

	if ( r != NULL ) { (*r) = ret; }


	return ret;
}

void
n_vfw_rect_expand_size( RECT *r, n_type_gfx *x, n_type_gfx *y, n_type_gfx *sx, n_type_gfx *sy )
{

	RECT r_zero = { 0,0,0,0 };
	if ( r == NULL ) { r = &r_zero; }

	if (  x != NULL ) { (* x) = r->left;             }
	if (  y != NULL ) { (* y) = r->top;              }
	if ( sx != NULL ) { (*sx) = r->right  - r->left; }
	if ( sy != NULL ) { (*sy) = r->bottom - r->top;  }


	return;
}




typedef struct {

	HWND hwnd;
	s32  csx, csy;

} n_vfw;




// internal
n_posix_inline u32
n_vfw_tmsf( BYTE t, BYTE m, BYTE s, BYTE f )
{

	// [Patch] : MinGW hasn't MCI_MAKE_TMSF

	u32 tt = (u32) t <<  0;
	u32 mm = (u32) m <<  8;
	u32 ss = (u32) s << 16;
	u32 ff = (u32) f << 24;


	return ( tt | mm | ss | ff );
}




#define n_vfw_zero( vfw ) n_memory_zero( vfw, sizeof( n_vfw ) )

void
n_vfw_free( n_vfw *vfw )
{

	if ( vfw == NULL ) { return; }


	if ( vfw->hwnd != NULL )
	{
		MCIWndStop   ( vfw->hwnd );
		MCIWndClose  ( vfw->hwnd );
		MCIWndDestroy( vfw->hwnd );
	}


	n_vfw_zero( vfw );


	return;
}

#define n_vfw_is_audio( vfw ) ( (vfw)->csx == 0 )

n_posix_bool
n_vfw_load( n_vfw *vfw, HWND hwnd, const n_posix_char *fname )
{

	if ( vfw == NULL ) { return n_posix_true; }


	n_vfw_free( vfw );


	// [x] : XP or later : toolbarwindow32 : position and size are not beautiful

	vfw->hwnd = MCIWndCreate
	(
		hwnd,
		GetModuleHandle( NULL ),
		MCIWNDF_NOERRORDLG |
		MCIWNDF_NOOPEN | MCIWNDF_NOMENU |
		MCIWNDF_NOAUTOSIZEWINDOW |
		MCIWNDF_NOPLAYBAR |
		MCIWNDF_NOAUTOSIZEMOVIE,
		fname
	);

	if (
		( vfw->hwnd == NULL )
		||
		( n_posix_false == MCIWndCanPlay( vfw->hwnd ) )
	)
	{

		n_vfw_free( vfw );

		return n_posix_true;
	}

//if ( MCIWndCanRecord( vfw->hwnd ) ) { n_posix_debug_literal( "AUDIO" ); }
//if ( MCIWndCanWindow( vfw->hwnd ) ) { n_posix_debug_literal( "VIDEO" ); }


#ifdef N_POSIX_PLATFORM_MINGW
	n_win_style_del( vfw->hwnd, WS_BORDER );
#else
	n_win64_style_del( vfw->hwnd, WS_BORDER );
#endif


	n_posix_char str[ N_VFW_CCH_MAX ];

	MCIWndGetDevice( vfw->hwnd, str, N_VFW_CCH_MAX );

	if ( n_string_is_same_literal( "cdaudio", str ) )
	{
//n_posix_debug( str );

		n_posix_char format[ N_VFW_CCH_MAX ];
		n_posix_char  track[ N_VFW_CCH_MAX ];


		// [!] : "millisecond" will be set

		MCIWndGetTimeFormat( vfw->hwnd, format, N_VFW_CCH_MAX );


		MCIWndSetTimeFormat( vfw->hwnd, n_posix_literal( "tmsf" ) );


		// [!] : "Track%0d.cda"

		WIN32_FIND_DATA f; ZeroMemory( &f, sizeof( WIN32_FIND_DATA ) );

		FindClose( FindFirstFile( fname, &f ) );

		n_posix_sprintf_literal( track, "%s", f.cFileName );


		int number = n_posix_atoi( &track[ 5 ] );
		MCIWndPlayFromTo
		(
			vfw->hwnd,
			n_vfw_tmsf( number + 0, 0, 0, 0 ),
			n_vfw_tmsf( number + 1, 0, 0, 0 )
		);


		MCIWndSetTimeFormat( vfw->hwnd, format );

	} else {

		//MCIWndSetTimeFormat( vfw->hwnd, n_posix_literal( "ms" ) );

	}


	// [Mechanism]
	//
	//	MCIWndGetSource : offset and real size
	//	MCIWndGetDest   : adjusted client size
	//
	//	[!] : playbar size will not be included 
	//
	//	to set : MCIWndPutSource(), MCIWndPutDest()

	// [x]
	//
	//	small size movies will play with stretched size
	//	no effect : MCIWndSetZoom( vfw->hwnd, N_VFW_CCH_MAX );

	if ( MCIWndCanWindow( vfw->hwnd ) )
	{

		RECT r; MCIWndGetDest( vfw->hwnd, &r );
//n_win_rect_debug( &rc );

		n_vfw_rect_max( &r, 0, 0, 128, 128 );

		MCIWndPutDest( vfw->hwnd, &r );

		n_vfw_rect_expand_size( &r, NULL, NULL, &vfw->csx, &vfw->csy );

	} else {

		// [Patch] : GetClientRect() : eject button's size isn't included 

		vfw->csx = vfw->csy = 0;

	}


	MoveWindow( vfw->hwnd, 0,0, vfw->csx, vfw->csy, FALSE );


	return n_posix_false;
}

n_posix_bool
n_vfw_is_active( n_vfw *vfw )
{

	if ( vfw == NULL ) { return n_posix_false; }


	return ( vfw->hwnd != NULL );
}

void
n_vfw_refresh( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }


	MCIWndRealize( vfw->hwnd, n_posix_true );

	//UpdateWindow( vfw->hwnd );


	return;
}

n_posix_char*
n_vfw_string_status( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return N_STRING_EMPTY; }


	static n_posix_char status[ N_VFW_CCH_MAX ];


	MCIWndGetMode( vfw->hwnd, status, N_VFW_CCH_MAX );


	return status;
}

#define n_vfw_is_playing( vfw ) n_string_is_same_literal( "playing", n_vfw_string_status( vfw ) )
#define n_vfw_is_stopped( vfw ) n_string_is_same_literal( "stopped", n_vfw_string_status( vfw ) )
#define n_vfw_is_paused(  vfw ) n_string_is_same_literal( "paused",  n_vfw_string_status( vfw ) )

n_type_real
n_vfw_percent( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return 0; }


	n_type_real a = MCIWndGetPosition( vfw->hwnd );
	n_type_real b = MCIWndGetEnd( vfw->hwnd );
	n_type_real c = ( (n_type_real) ( (n_type_real) a / b ) * 100 );

//n_win_hwndprintf_literal( GetParent( vfw->hwnd ), "%d / %d : %d %%", (int) a, (int) b, (int) c );


	return c;
}

void
n_vfw_pause( n_vfw *vfw )
{

	if ( n_posix_false == n_vfw_is_active( vfw ) ) { return; }


	MCIWndPause( vfw->hwnd );


	return;
}

void
n_vfw_resume( n_vfw *vfw )
{

	// [x] : MCIWndResume() cannot resume MIDI files


	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }


	MCIWndPlayFrom( vfw->hwnd, MCIWndGetPosition( vfw->hwnd ) );


	return;
}

void
n_vfw_stop( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }


	MCIWndStop( vfw->hwnd );
	MCIWndSeek( vfw->hwnd, MCIWND_START );


	return;
}

void
n_vfw_rewind( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }


	MCIWndSeek( vfw->hwnd, MCIWND_START );


	return;
}

void
n_vfw_play( n_vfw *vfw )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }


	// [MSDN] : Default
	//
	//	Active   :  500
	//	Inactive : 2000

	MCIWndSetActiveTimer  ( vfw->hwnd,  500 );
	MCIWndSetInactiveTimer( vfw->hwnd, 1000 );


	//MCIWndSeek( vfw->hwnd, MCIWND_START );
	MCIWndPlay( vfw->hwnd );


	return;
}

void
n_vfw_seek( n_vfw *vfw, n_type_real ratio )
{

	if ( FALSE == n_vfw_is_active( vfw ) ) { return; }

	n_posix_bool resume = n_posix_false;
	if ( n_vfw_is_playing( vfw ) ) { resume = n_posix_true; }


	n_type_real length = (n_type_real) MCIWndGetEnd( vfw->hwnd );

//n_win_hwndprintf_literal( GetParent( vfw->hwnd ), "%f %f", length, ratio );

	MCIWndSeek( vfw->hwnd, (n_type_real) length * ratio );

	if ( resume ) { n_vfw_play( vfw ); }


	return;
}


#endif // _H_NONNON_WIN32_GAME_SOUND_VFW


