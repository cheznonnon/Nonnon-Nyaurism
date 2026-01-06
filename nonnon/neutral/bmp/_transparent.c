// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_TRANSPARENT
#define _H_NONNON_NEUTRAL_BMP_TRANSPARENT




#include "../posix.c"




#define n_bmp_trans n_bmp_black




n_posix_inline n_posix_bool
n_bmp_is_trans( const n_bmp *bmp, u32 color )
{
//return n_posix_false;

	if ( bmp == NULL ) { return n_posix_false; }

	if ( ( bmp->transparent_onoff )&&( color == n_bmp_trans ) )
	{
		return n_posix_true;
	}


	return n_posix_false;
}

n_posix_inline n_posix_bool
n_bmp_transparent_onoff( n_bmp *bmp, n_posix_bool onoff )
{
//return n_posix_false;

	if ( bmp == NULL ) { return n_posix_false; }


	n_posix_bool prev = bmp->transparent_onoff;

	bmp->transparent_onoff = onoff;


	return prev;
}




#endif // _H_NONNON_NEUTRAL_BMP_TRANSPARENT

