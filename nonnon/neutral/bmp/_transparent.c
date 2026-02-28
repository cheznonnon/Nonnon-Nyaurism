// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_TRANSPARENT
#define _H_NONNON_NEUTRAL_BMP_TRANSPARENT




#include "../posix.c"




#define n_bmp_trans n_bmp_black




n_posix_inline BOOL
n_bmp_is_trans( const n_bmp *bmp, u32 color )
{
//return FALSE;

	if ( bmp == NULL ) { return FALSE; }

	if ( ( bmp->transparent_onoff )&&( color == n_bmp_trans ) )
	{
		return TRUE;
	}


	return FALSE;
}

n_posix_inline BOOL
n_bmp_transparent_onoff( n_bmp *bmp, BOOL onoff )
{
//return FALSE;

	if ( bmp == NULL ) { return FALSE; }


	BOOL prev = bmp->transparent_onoff;

	bmp->transparent_onoff = onoff;


	return prev;
}




#endif // _H_NONNON_NEUTRAL_BMP_TRANSPARENT

