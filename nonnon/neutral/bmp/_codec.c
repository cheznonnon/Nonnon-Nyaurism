// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_CODEC
#define _H_NONNON_NEUTRAL_BMP_CODEC




#include "../bmp.c"




n_posix_bool
n_bmp_24bit( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}


	if ( 32 != N_BMP_DEPTH( bmp ) ) { return n_posix_false; }

	if ( n_bmp_alpha_is_used( bmp ) ) { return n_posix_false; }


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );


	u8         *ptr      = (u8*) n_memory_new( n_bmp_size( sx,sy, 24 ) );
	n_type_gfx  skipbyte = n_bmp_skipbyte( sx, 24 );


	n_type_gfx i = 0;
	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x, sy - 1 - y, &color );

		ptr[ i + 0 ] = n_bmp_b( color );
		ptr[ i + 1 ] = n_bmp_g( color );
		ptr[ i + 2 ] = n_bmp_r( color );

		i += 3;


		x++;
		if ( x >= sx )
		{

			x = 0;

			n_type_gfx ii = 0;
			n_posix_loop
			{

				if ( ii >= skipbyte ) { break; }

				ptr[ i ] = 0;
				i++;

				ii++;

			}

			y++;
			if ( y >= sy ) { break; }
		}
	}


	n_memory_free( N_BMP_PTR( bmp ) );

	N_BMP_PTR  ( bmp ) = (u32*) ptr;
	N_BMP_DEPTH( bmp ) = 24;

	n_bmp_precalc( bmp );


	return n_posix_false;
}

n_posix_bool
n_bmp_32bit( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}


	if ( 24 != N_BMP_DEPTH( bmp ) ) { return n_posix_false; }


	const n_type_gfx bmpsx = N_BMP_SX( bmp );
	const n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_bmp bmp_t;
	n_bmp_zero( &bmp_t );
	n_bmp_1st_fast( &bmp_t, bmpsx, bmpsy );

	u8         *ptr      = (u8*) N_BMP_PTR( bmp );
	n_type_gfx  skipbyte = n_bmp_skipbyte( bmpsx, 24 );


	n_type_gfx i = 0;
	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		int r = ptr[ i + 2 ];
		int g = ptr[ i + 1 ];
		int b = ptr[ i + 0 ];

		u32 color = n_bmp_rgb( r,g,b );

		n_bmp_ptr_set_fast( &bmp_t, x, bmpsy - 1 - y, color );

		i += 3;


		x++;
		if ( x >= bmpsx )
		{

			x = 0;

			i += skipbyte;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	n_bmp_replace( &bmp_t, bmp );


	return n_posix_false;
}

n_posix_bool
n_bmp_rle8_decode( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}


	// [Condition] : 8bit palette mode only available by specification

	if (       8 != N_BMP_DEPTH( bmp ) ) { return n_posix_false; }
	if ( BI_RLE8 != N_BMP_RLE  ( bmp ) ) { return n_posix_false; }


	const n_type_gfx bmpsx = N_BMP_SX( bmp );
	const n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_gfx byte = n_bmp_size( bmpsx,bmpsy, 8 );

	u8 *f = (u8*) N_BMP_PTR( bmp );
	u8 *t = (u8*) n_memory_new( byte );

	n_memory_zero( t, byte );


	n_type_gfx linebyte = n_bmp_linebyte( bmpsx, 8 );
	n_type_gfx skipbyte = n_bmp_skipbyte( bmpsx, 8 );


	n_type_gfx len;
	n_type_gfx ox,oy;


	n_type_gfx align_unit = sizeof( s16 );
	n_type_gfx align;


	u32 i  = 0;
	u32 ii = 0;
	n_posix_loop
	{

		if ( f[ i ] == 0 )
		{

			i++;

			if ( f[ i ] == 0 )
			{

				// End of Line

				ii += skipbyte;

			} else
			if ( f[ i ] == 1 )
			{

				// End of Bitmap

				break;

			} else
			if ( f[ i ] == 2 )
			{

				// Offset / Move Position

				i++;
				ox = f[ i ];

				i++;
				oy = f[ i ];


				ii += ox;
				ii += oy * linebyte;

//n_posix_debug_literal( "%d %d %d", ox, oy );

			} else {

				// Length : 3 - 255

				len = f[ i ];

				align = align_unit - ( len % align_unit );
				if ( align == align_unit ) { align = 0; }

				n_posix_loop
				{

					// Not Compressed

					i++;
					t[ ii ] = f[ i ];
					ii++;

//if ( ( ii % linebyte ) >= N_BMP_SX(bmp) ) { ii += skipbyte; }

					len--;
					if ( len <= 0 ) { break; }
				}

				i += align;

			}

		} else {

			// Length : 1 - 255

			len = f[ i ];

			i++;

			n_posix_loop
			{

				// Unpack Original Value

				t[ ii ] = f[ i ];
				ii++;

				len--;
				if ( len <= 0 ) { break; }
			}

		}


		i++;
		if ( i >= N_BMP_ORIGSIZE( bmp ) ) { break; }
	}


	N_BMP_FILESIZE( bmp ) -= N_BMP_ORIGSIZE( bmp );

	N_BMP_SIZE    ( bmp )  = ii;
	N_BMP_RLE     ( bmp )  = 0;
	N_BMP_ORIGSIZE( bmp )  = 0;

	N_BMP_FILESIZE( bmp ) += (u32) N_BMP_SIZE( bmp );


	n_memory_free( N_BMP_PTR( bmp ) );
	N_BMP_PTR( bmp ) = (u32*) n_memory_resize( t, N_BMP_SIZE( bmp ) );


//n_posix_dump_literal( "dump", N_BMP_PTR( bmp ), N_BMP_SIZE( bmp ) );


	return n_posix_false;
}

n_posix_bool
n_bmp_rle8_encode( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}


	// [Condition] : 8bit palette mode only available by specification

	if (       8 != N_BMP_DEPTH( bmp ) ) { return n_posix_false; }
	if ( BI_RLE8 == N_BMP_RLE  ( bmp ) ) { return n_posix_false; }


	n_type_gfx skipbyte = n_bmp_skipbyte( N_BMP_SX( bmp ), 8 );

	n_type_int byte = N_BMP_SIZE( bmp ) * 3;
	if ( byte > LONG_MAX ) { return n_posix_true; }

	u8 *f = (u8*) N_BMP_PTR( bmp );
	u8 *t = (u8*) n_memory_new( byte );

	n_memory_zero( t, byte );


	int start = f[ 0 ];
	int rle   = 0;

	n_type_index i  = 0;
	n_type_index ii = 0;
	n_type_gfx   x  = 0;
	n_type_gfx   y  = 0;
	n_posix_loop
	{

		if ( start == f[ i ] )
		{

			rle++;

			if ( rle >= 255 )
			{
				t[ ii + 0 ] = (u8) rle;
				t[ ii + 1 ] = (u8) start;
				ii += 2;

				rle = 1;
			}

		} else {

			if ( rle )
			{
				t[ ii + 0 ] = (u8) rle;
				t[ ii + 1 ] = (u8) start;
				ii += 2;
			}

			start = f[ i ];
			rle   = 1;

		}


		x++; i++;
		if ( x >= N_BMP_SX( bmp ) )
		{

			if ( rle )
			{
				t[ ii + 0 ] = (u8) rle;
				t[ ii + 1 ] = (u8) start;
				ii += 2;
			}



			// End of Line

			t[ ii + 0 ] = 0;
			t[ ii + 1 ] = 0;
			ii += 2;

/*
// Offset Test

t[ ii + 0 ] = 0;
t[ ii + 1 ] = 2;
t[ ii + 2 ] = 0;
t[ ii + 3 ] = 1;
ii += 4; y++; i += n_bmp_linebyte( N_BMP_SX( bmp ), 8 );
*/

			i += skipbyte;

			start = f[ i ];
			rle   = 0;


			x = 0;

			y++;
			if ( y >= N_BMP_SY(bmp) ) { break; }
		}
	}


	// End of Bitmap

	t[ ii + 0 ] = 0;
	t[ ii + 1 ] = 1;
	ii += 2;


	N_BMP_FILESIZE( bmp ) -= (u32) N_BMP_SIZE( bmp );

	N_BMP_SIZE    ( bmp )  = (u32) ii;
	N_BMP_RLE     ( bmp )  = BI_RLE8;
	N_BMP_ORIGSIZE( bmp )  = (u32) N_BMP_SIZE( bmp );

	N_BMP_FILESIZE( bmp ) += (u32) N_BMP_SIZE( bmp );


	n_memory_free( N_BMP_PTR( bmp ) );
	N_BMP_PTR( bmp ) = (u32*) n_memory_resize( t, N_BMP_SIZE( bmp ) );


	return n_posix_false;
}

int
n_bmp_pal2full( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return 1; }
	}


	if ( BI_RLE8 == N_BMP_RLE( bmp ) )
	{
		if ( n_bmp_rle8_decode( bmp ) ) { return 5; }
	} else
	if ( BI_RGB != N_BMP_RLE( bmp ) )
	{
		return 1;
	}


	u8         *ptr      = (u8*) N_BMP_PTR( bmp );
	int         depth    = N_BMP_DEPTH( bmp );
	n_type_gfx  skipbyte = n_bmp_skipbyte( N_BMP_SX( bmp ), depth );

	u32  palette[ 256 ];
	n_memory_copy( N_BMP_PTR_PAL( bmp ), palette, N_BMP_PAL( bmp ) * sizeof( u32 ) );

	n_bmp b; n_bmp_zero( &b );
	n_bmp_1st_fast( &b, N_BMP_SX( bmp ), N_BMP_SY( bmp ) );


	u32 color;
	int bit;

	n_type_gfx align = 0;

	n_type_index i = 0;
	n_type_gfx   x = 0;
	n_type_gfx   y = 0;
	n_posix_loop
	{

		       int c    = ptr[ i ];
		n_type_gfx tmpy = N_BMP_SY( bmp ) - 1 - y;

		if ( depth == 1 )
		{ 

			bit = 7;
			n_posix_loop
			{

				color = palette[ 0 != ( (int) c & ( 1 << bit ) ) ];
				n_bmp_ptr_set_fast( &b, x, tmpy, color );

				align++;
				if ( align >= N_BMP_SX( bmp ) ) { break; }

				if ( bit != 0 ) { bit--; } else { break; }

				x++;
				if ( x >= N_BMP_SX( bmp ) ) { x = 0; y++; }
			}

		} else
		if ( depth == 4 )
		{ 

			// upper 4bit

			color = palette[ ( c >> 4 ) ];
			n_bmp_ptr_set_fast( &b, x, tmpy, color );

			x++;
			align++;

			if ( align < N_BMP_SX( bmp ) )
			{

				// [!] : lower 4bit

				color = palette[ ( c & 0x0f ) ];
				n_bmp_ptr_set_fast( &b, x, tmpy, color );

				align++;
			}

		} else
		if ( depth == 8 )
		{

			color = palette[ c ];
			n_bmp_ptr_set_fast( &b, x, tmpy, color );

			align++;

		}


		i++;

		if ( align >= N_BMP_SX( bmp ) ) 
		{
			align = 0;

			i += skipbyte;
		}

		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{

			x = 0;

			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}
	}


	n_bmp_replace( &b, bmp );


	return n_posix_false;
}

n_posix_bool
n_bmp_palette( n_bmp *bmp )
{

	if ( n_bmp_safemode )
	{
		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}


	if ( 32 != N_BMP_DEPTH( bmp ) ) { return n_posix_true; }


	u32        palette[ 256 ];
	n_type_gfx reg = 0;


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );


		n_type_gfx i = 0;
		n_posix_loop
		{

			// [!] : invalid bitmap will generate without this code
			//
			//	when black color is only used
			//	black == all zero == 0x00000000

			if ( ( reg != 0 )&&( palette[ i ] == color ) ) { break; }


			i++;
			if ( i >= reg )
			{

				palette[ reg ] = color;

				reg++;
				if ( reg >= 256 ) { return n_posix_true; }

				break;
			}
		}


		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{

			x = 0;

			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}

	}


	n_memory_free( N_BMP_PTR_PAL( bmp ) );

	N_BMP_PAL    ( bmp ) = (u32) reg;
	N_BMP_PTR_PAL( bmp ) = (u32*) n_memory_new( reg * sizeof( u32 ) );

	n_memory_copy( palette, N_BMP_PTR_PAL( bmp ), reg * sizeof( u32 ) );


	return n_posix_false;
}

void
n_bmp_full2pal( n_bmp *bmp )
{

	n_bmp b; n_bmp_carboncopy( bmp, &b );

	n_bmp_palette( &b );
	if ( 0 == N_BMP_PAL( &b ) ) { n_bmp_free( &b ); return; }

	N_BMP_DEPTH( &b ) = 8;
	if ( N_BMP_PAL( &b ) <=  2 ) { N_BMP_DEPTH( &b ) = 1; } else
	if ( N_BMP_PAL( &b ) <= 16 ) { N_BMP_DEPTH( &b ) = 4; }


	n_memory_free( N_BMP_PTR( &b ) );

	N_BMP_SIZE( &b ) = n_bmp_size( N_BMP_SX( &b ),N_BMP_SY( &b ), N_BMP_DEPTH( &b ) );
	N_BMP_PTR ( &b ) = (u32*) n_memory_new( N_BMP_SIZE( &b ) );


	// [!] : set skipbyte zero

	n_memory_zero( N_BMP_PTR( &b ), N_BMP_SIZE( &b ) );


	u8 *ptr = (u8*) N_BMP_PTR( &b );


	n_type_gfx   x  = 0;
	n_type_gfx   y  = 0;
	n_type_index sx = (n_type_gfx) N_BMP_SIZE( &b ) / N_BMP_SY( &b );
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );


		n_type_index pos   = sx * ( N_BMP_SY( &b ) - 1 - y );
		int          shift = 0;


		n_type_index i = 0;
		n_posix_loop
		{

			if ( color == N_BMP_PTR_PAL( &b )[ i ] )
			{

				if (   2 >= N_BMP_PAL( &b ) )
				{

					pos += x / 8;

					shift = 7 - ( x % 8 );

				} else
				if (  16 >= N_BMP_PAL( &b ) )
				{

					pos += x / 2;

					if ( ( x % 2 ) == 0 )
					{
						shift = 4;
					} else {
						shift = 0;
					}

				} else
				if ( 256 >= N_BMP_PAL( &b ) )
				{
					pos  += x;
					shift = 0;
				}


				ptr[ pos ] += (u8) ( i << shift );

				break;
			}

			i++;

		}

		x++;
		if ( x >= N_BMP_SX( &b ) )
		{

			x = 0;

			y++;
			if ( y >= N_BMP_SY( &b ) ) { break; }
		}
	}


	n_bmp_replace( &b, bmp );

	n_bmp_precalc( bmp );


	return;
}

// internal
n_posix_bool
n_bmp_is_loadable( n_bmp *bmp, n_type_int ptrsize )
{

	if ( bmp == NULL ) { return n_posix_false; }


	// [!] : Not Supported
	//
	//	[ Signature ]
	//
	//	Windows Bitmap file or not
	//
	//	[ Size ]
	//
	//	minus value is not acceptable
	//	too large bitmaps are not acceptable
	//
	//	[ Compression ]
	//
	//	BI_RGB       0
	//	BI_RLE8      1
	//	BI_RLE4      2
	//	BI_BITFIELDS 3
	//	BI_JPEG      4
	//	BI_PNG       5
	//
	//	[ V4 V5 Headers ]
	//
	//	BITMAPINFOHEADER :  40 byte : 28 00 00 00
	//	BITMAPV4HEADER   : 108 byte : 6c 00 00 00
	//	BITMAPV5HEADER   : 124 byte : 7c 00 00 00
	//
	//	[ Depth or Bitlength ]
	//
	//	0x0100 =   2 colors
	//	0x0400 =  16 colors
	//	0x0800 = 256 colors
	//	0x1800 =  24 bit full color
	//	0x2000 =  32 bit full color

	if (
		( N_BMP_TYPE_BM != N_BMP_TYPE( bmp ) )
		||
		( ( 0 >= N_BMP_SY( bmp ) )||( 0 >= N_BMP_SX( bmp ) ) )
		||
		( n_bmp_is_overflow( N_BMP_SX( bmp ), N_BMP_SY( bmp ) ) )
		||
		(
			( BI_RGB  != N_BMP_RLE( bmp ) )
			&&
			( BI_RLE8 != N_BMP_RLE( bmp ) )
		)
		||
		( N_BMP_SIZE_INFOH != N_BMP_CBSIZE( bmp ) )
		||
		(
			(  1 != N_BMP_DEPTH( bmp ) )
			&&
			(  4 != N_BMP_DEPTH( bmp ) )
			&&
			(  8 != N_BMP_DEPTH( bmp ) )
			&&
			( 24 != N_BMP_DEPTH( bmp ) )
			&&
			( 32 != N_BMP_DEPTH( bmp ) )
		)
	)
	{
		return n_posix_false;
	}


	// [!] : size integrity
	//
	//	offset will be zero when on-memory loading
	//	palette can be zero in 2/16/256 colors

	if ( 0 == N_BMP_OFFBITS( bmp ) )
	{
		N_BMP_OFFBITS( bmp ) = N_BMP_SIZE_HEADER + ( N_BMP_PAL( bmp ) * sizeof( u32 ) );
	}

	if ( 0 == N_BMP_PAL( bmp ) )
	{
		N_BMP_PAL( bmp ) = ( N_BMP_OFFBITS( bmp ) - N_BMP_SIZE_HEADER ) / sizeof( u32 );
	}

	n_bmp_precalc( bmp );

	{

		n_type_gfx needed_rgb  = N_BMP_OFFBITS( bmp ) + N_BMP_SIZE    ( bmp );
		n_type_gfx needed_rle8 = N_BMP_OFFBITS( bmp ) + N_BMP_ORIGSIZE( bmp );


		if (
			( ( BI_RGB  == N_BMP_RLE( bmp ) )&&( ptrsize < needed_rgb  ) )
			||
			( ( BI_RLE8 == N_BMP_RLE( bmp ) )&&( ptrsize < needed_rle8 ) )
		)
		{
			return n_posix_false;
		}
//n_posix_debug_literal( "%d : %d", (int) ptrsize, (int) needed_rgb );

	}


	return n_posix_true;
}

#define n_bmp_load(          bmp, filename  ) n_bmp_load_internal( bmp, (void*) filename,    0, n_posix_true  )
#define n_bmp_load_onmemory( bmp, ptr, size ) n_bmp_load_internal( bmp,              ptr, size, n_posix_false )

// internal
n_posix_bool
n_bmp_load_internal( n_bmp *bmp, u8 *ptr, n_type_int ptrsize, n_posix_bool is_file )
{

	// [!] : 4 byte alignment
	//
	//	calculation method is different in each softwares
	//	cannot support some patterns

	// [!] : file-wide 4byte alignment
	//
	//	some softwares might write 2 byte padding at the end of file
	//	because BITMAPFILEHEADER is not 4byte-align


	if ( n_bmp_safemode )
	{
		if ( ptr == NULL ) { return n_posix_true; }
		if ( bmp == NULL ) { return n_posix_true; }
	}


	n_posix_bool onmemory;


	// [!] : try to handle as bitmap

	n_bmp check;
	n_bmp_zero( &check );


	if ( is_file )
	{

		const n_posix_char *name = (n_posix_char*) ptr;


		onmemory = n_posix_false;


		ptrsize = n_posix_stat_size( name );
		if ( ptrsize < N_BMP_SIZE_HEADER ) { return n_posix_true; }

		if ( ptrsize > LONG_MAX ) { return n_posix_true; }


		FILE *fp = n_posix_fopen_read( name );
		if ( fp == NULL ) { return n_posix_true; }


		// [!] : sniffer

		n_posix_fread( &N_BMP_FILEH( &check ), N_BMP_SIZE_FILEH, 1, fp );
		n_posix_fread( &N_BMP_INFOH( &check ), N_BMP_SIZE_INFOH, 1, fp );

		if ( n_posix_false == n_bmp_is_loadable( &check, ptrsize ) )
		{

			n_posix_fclose( fp );

			return n_posix_true;
		}


		n_posix_fseek( fp, 0, SEEK_SET );

		ptr = (u8*) n_memory_new( ptrsize );
		n_posix_fread( ptr, ptrsize, 1, fp );


		n_posix_fclose( fp );

	} else {

		onmemory = n_posix_true;


		if ( ptrsize < N_BMP_SIZE_HEADER ) { return n_posix_true; }

		if ( ptrsize > LONG_MAX ) { return n_posix_true; }

		n_memory_copy( &ptr[                0 ], &N_BMP_FILEH( &check ), N_BMP_SIZE_FILEH );
		n_memory_copy( &ptr[ N_BMP_SIZE_FILEH ], &N_BMP_INFOH( &check ), N_BMP_SIZE_INFOH );

		N_BMP_TYPE( &check ) = N_BMP_TYPE_BM;

		if ( n_posix_false == n_bmp_is_loadable( &check, ptrsize ) )
		{
			return n_posix_true;
		}

	}


	// Decoder

	n_posix_bool ret = n_posix_false;


	if ( 24 <= N_BMP_DEPTH( &check ) )
	{

		N_BMP_PTR( &check ) = (u32*) n_memory_new( N_BMP_SIZE( &check ) );
		n_memory_copy( &ptr[ N_BMP_OFFBITS( &check ) ], N_BMP_PTR( &check ), N_BMP_SIZE( &check ) );

		if ( 24 == N_BMP_DEPTH( &check ) ) { n_bmp_32bit( &check ); }

	} else {

		n_type_int palsize = N_BMP_PAL( &check ) * sizeof( u32 );
		n_type_int datsize = ptrsize - N_BMP_OFFBITS( &check );


		N_BMP_PTR_PAL( &check ) = (u32*) n_memory_new( palsize );
		N_BMP_PTR    ( &check ) = (u32*) n_memory_new( datsize );

		n_memory_copy( &ptr[ N_BMP_SIZE_HEADER       ], N_BMP_PTR_PAL( &check ), palsize );
		n_memory_copy( &ptr[ N_BMP_OFFBITS( &check ) ], N_BMP_PTR    ( &check ), datsize );


		ret = n_bmp_pal2full( &check );

	}


	// Cleanup

	if ( onmemory == n_posix_false ) { n_memory_free( ptr ); }

	if ( ret )
	{

		n_bmp_free( &check );

	} else {

		n_bmp_replace( &check, bmp );

		// [!] : backward compatibility : 0RGB bitmap will be visible

		if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 )
		{
			if ( n_bmp_alpha_is_zero( bmp ) )
			{
				n_bmp_alpha_reverse( bmp );
			}
		}

	}


	return ret;
}

#define n_bmp_save_literal( bmp, filename ) n_bmp_save( bmp, n_posix_literal( filename ) )

n_posix_bool
n_bmp_save( const n_bmp *bmp, const n_posix_char *filename )
{

	if ( n_bmp_safemode )
	{
		if ( bmp == NULL ) { return n_posix_true; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }
	}



	// [!] : parameter non-breaking

	n_bmp save; n_bmp_carboncopy( bmp, &save );


	// [!] : stop to make an invalid bitmap

	if ( NULL == N_BMP_PTR ( &save ) ) { n_bmp_free( &save ); return n_posix_true; }
	if (    0 == N_BMP_SIZE( &save ) ) { n_bmp_free( &save ); return n_posix_true; }


	// [!] : backward compatibility : all visible will be 0RGB bitmap

	if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 )
	{
		if ( n_bmp_alpha_is_visible( &save ) )
		{
			n_bmp_alpha_reverse( &save );
		}
	}


	n_bmp_full2pal( &save );
	u32 byte_pal = N_BMP_PAL( &save ) * sizeof( u32 );

	//n_bmp_rle8_encode( &save );

	n_bmp_24bit( &save );


	n_memory_zero( &N_BMP_FILEH( &save ), N_BMP_SIZE_FILEH );

	N_BMP_TYPE    ( &save ) = N_BMP_TYPE_BM;
	N_BMP_FILESIZE( &save ) = N_BMP_SIZE_HEADER + byte_pal + (u32) N_BMP_SIZE( &save );
	N_BMP_OFFBITS ( &save ) = N_BMP_SIZE_HEADER + byte_pal;

	N_BMP_CBSIZE  ( &save ) = N_BMP_SIZE_INFOH;
	N_BMP_PLANE   ( &save ) = 1;
	N_BMP_RLE     ( &save ) = 0;
	N_BMP_ORIGSIZE( &save ) = 0;
	N_BMP_XRES    ( &save ) = 0;
	N_BMP_YRES    ( &save ) = 0;


	{

		FILE  *fp = n_posix_fopen_write( filename );
		if ( fp == NULL ) { n_bmp_free( &save ); return n_posix_true; }

		n_posix_fwrite( &N_BMP_FILEH  ( &save ),  N_BMP_SIZE_FILEH,    1, fp );
		n_posix_fwrite( &N_BMP_INFOH  ( &save ),  N_BMP_SIZE_INFOH,    1, fp );
		n_posix_fwrite(  N_BMP_PTR_PAL( &save ),  byte_pal,            1, fp ); 
		n_posix_fwrite(  N_BMP_PTR    ( &save ),  N_BMP_SIZE( &save ), 1, fp ); 

		n_posix_fclose( fp );

	}


	n_bmp_free( &save );


	return n_posix_false;
}


#endif // _H_NONNON_NEUTRAL_BMP_CODEC

