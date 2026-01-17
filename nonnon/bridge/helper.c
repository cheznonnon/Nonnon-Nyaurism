// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GAME_HELPER
#define _H_NONNON_WIN32_GAME_HELPER




#include "../neutral/bmp.c"
#include "../neutral/random.c"




n_posix_inline BOOL
n_game_timer( u32 *prv, u32 interval )
{

	u32 cur = n_posix_tickcount();
	u32 msec;


	if ( (*prv) > cur )
	{
		msec = cur + ( 0xffffffff - (*prv) );
	} else {
		msec = cur - (*prv);
	}

	if ( msec >= interval )
	{
		(*prv) = cur;

		return TRUE;
	}


	return FALSE;
}

n_posix_inline BOOL
n_game_timer_once( u32 *prv, u32 interval )
{

	u32 cur = n_posix_tickcount();
	u32 msec;


	if ( (*prv) > cur )
	{
		msec = cur + ( 0xffffffff - (*prv) );
	} else {
		msec = cur - (*prv);
	}

	if ( msec >= interval )
	{
		//(*prv) = cur;

		return TRUE;
	}


	return FALSE;
}




#define n_game_random( range ) n_random_range( range )

n_posix_inline u32
n_game_randomcolor( void )
{

	u32 color = n_bmp_rgb
	(
		n_game_random( 256 ),
		n_game_random( 256 ),
		n_game_random( 256 )
	);


	return color;
}




n_posix_inline n_type_gfx
n_game_centering( n_type_gfx a, n_type_gfx b )
{

	// [!] : ( ( a / 2 ) - ( b / 2 ) ) == ( ( a - b ) / 2 )

	return ( a - b ) / 2;
}


#endif // _H_NONNON_WIN32_GAME_HELPER

