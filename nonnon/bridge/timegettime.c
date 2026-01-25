// Nonnon Game Layer
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism]
//
//	1 : project/define_unicode.c
//	2 : game/timegettime.c
//	3 : option enabler files (like neutral/png.c)
//	4 : other files
//
//	n_posix_tickcount() will use timeGetTime() instead of GetTickCount()
//
//	Link : -lwinmm




#ifndef _H_NONNON_GAME_TIMEGETTIME
#define _H_NONNON_GAME_TIMEGETTIME




#define N_POSIX_TIMEGETTIME
#include "../neutral/posix.c"




static TIMECAPS n_timegettime_instance;

n_posix_inline void
n_game_timegettime_init( void )
{

	ZeroMemory( &n_timegettime_instance, sizeof( TIMECAPS ) );

	timeGetDevCaps( &n_timegettime_instance, sizeof( TIMECAPS ) );
	timeBeginPeriod( n_timegettime_instance.wPeriodMin );


	return;
}

n_posix_inline void
n_game_timegettime_exit( void )
{

	timeEndPeriod( n_timegettime_instance.wPeriodMin );

	return;
}


#endif // _H_NONNON_GAME_TIMEGETTIME

