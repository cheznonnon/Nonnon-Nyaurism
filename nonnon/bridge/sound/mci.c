// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Link : -lwinmm




#ifndef _H_NONNON_WIN32_GAME_SOUND_MCI
#define _H_NONNON_WIN32_GAME_SOUND_MCI




#include "../../neutral/posix.c"
#include "../../neutral/wav.c"

#ifdef _MSC_VER
#pragma comment( lib, "winmm" )
#endif // #ifdef _MSC_VER





typedef struct {

	MCIDEVICEID id;
	DWORD       length;

} n_mci;




#define n_mci_zero( p ) n_memory_zero( p, sizeof( n_mci ) )

n_posix_bool
n_mci_init( n_mci *p, const n_posix_char *fname )
{

	MCI_OPEN_PARMS open; ZeroMemory( &open, sizeof( MCI_OPEN_PARMS ) );

	open.lpstrElementName = fname;

	mciSendCommand( 0, MCI_OPEN, MCI_WAIT | MCI_OPEN_ELEMENT, (DWORD_PTR) &open );

	p->id = open.wDeviceID;


	return n_posix_false;
}

n_posix_bool
n_mci_loop( n_mci *p )
{
//n_posix_debug_literal( "MCI : Play" );

	// [Needed] : Win9x : MCI_PLAY_PARMS

	MCI_PLAY_PARMS   play; ZeroMemory( &play, sizeof( MCI_PLAY_PARMS   ) );
	MCI_SET_PARMS     set; ZeroMemory( &set,  sizeof( MCI_SET_PARMS    ) );
	MCI_STATUS_PARMS stat; ZeroMemory( &stat, sizeof( MCI_STATUS_PARMS ) );

	play.dwTo         = (u32) -1;
	 set.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	stat.dwItem       = MCI_STATUS_LENGTH;

	mciSendCommand( p->id, MCI_PLAY,   MCI_FROM,            (DWORD_PTR) &play );
	mciSendCommand( p->id, MCI_SET,    MCI_SET_TIME_FORMAT, (DWORD_PTR) &set  );
	mciSendCommand( p->id, MCI_STATUS, MCI_STATUS_ITEM,     (DWORD_PTR) &stat );

	p->length = (DWORD) stat.dwReturn;

//n_posix_debug_literal( "%lu", stat.dwReturn );


	return n_posix_false;
}

n_posix_bool
n_mci_stop( n_mci *p )
{
///n_posix_debug_literal( "MCI : Stop" );

	mciSendCommand( p->id, MCI_STOP, 0, 0 );


	return n_posix_false;
}

n_posix_bool
n_mci_exit( n_mci *p )
{
///n_posix_debug_literal( "MCI : Stop" );

	mciSendCommand( p->id, MCI_STOP,         0,                0 );
	mciSendCommand( p->id, MCI_CLOSE, MCI_WAIT, (DWORD_PTR) NULL );

	n_mci_zero( p );


	return n_posix_false;
}

n_posix_bool
n_mci_pause( n_mci *p )
{
///n_posix_debug_literal( "MCI : Pause" );

	mciSendCommand( p->id, MCI_PAUSE, 0, 0 );


	return n_posix_false;
}

n_posix_bool
n_mci_resume( n_mci *p )
{
///n_posix_debug_literal( "MCI : Resume" );

	mciSendCommand( p->id, MCI_RESUME, 0, 0 );


	return n_posix_false;
}


#endif // _H_NONNON_WIN32_GAME_SOUND_MCI

