// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_UI_TIMER
#define _H_NONNON_NEUTRAL_BMP_UI_TIMER




#include "../../bmp.c"
#include "../../random.c"




BOOL
n_bmp_ui_timer( u32 *prv, u32 interval )
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

BOOL
n_bmp_ui_timer_once( u32 *prv, u32 interval )
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


#endif // _H_NONNON_NEUTRAL_BMP_UI_TIMER

