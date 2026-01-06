// Nonnon Endian Converter
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// How to Use
//
//	only needed in file read/wirte
//
//	automatically detect CPU's endianness at runtime
//
//	n_endian_big()
//		 input : for little endian formatted files (bmp, wav)
//		output : for    big endian formatted files (mid)
//
//	n_endian_lil()
//		 input : for    big endian formatted files (mid)
//		output : for little endian formatted files (bmp, wav)




#ifndef _H_NONNON_NEUTRAL_ENDIAN
#define _H_NONNON_NEUTRAL_ENDIAN




#include "./type.c"




// internal
u32
n_endian_swap( u32 n, int bytealign )
{

	u32 tmp;


	if (
		( bytealign <  1 )
		||
		( bytealign > 32 )
	)
	{
		return 0;
	}

	tmp  = ( n >> 24 ) & 0x000000ff;
	tmp += ( n >>  8 ) & 0x0000ff00;
	tmp += ( n <<  8 ) & 0x00ff0000;
	tmp += ( n << 24 ) & 0xff000000;
/*
	tmp  = ( (u8) ( n >> 24 ) );
	tmp += ( (u8) ( n >> 16 ) ) <<  8;
	tmp += ( (u8) ( n >>  8 ) ) << 16;
	tmp += ( (u8) ( n       ) ) << 24;
*/

	return tmp >> ( 32 - ( bytealign * 8 ) );
}

u32
n_endian_big( u32 n, int bytealign )
{

	u32 checker = 1;


	// already big

	if ( ( (u8*) &checker )[ 0 ] != 1 ) { return n; }


	return n_endian_swap( n, bytealign );
}

u32
n_endian_lil( u32 n, int bytealign )
{

	u32 checker = 1;


	// already little

	if ( ( (u8*) &checker )[ 0 ] == 1 ) { return n; }


	return n_endian_swap( n, bytealign );
}


#endif // _H_NONNON_NEUTRAL_ENDIAN

