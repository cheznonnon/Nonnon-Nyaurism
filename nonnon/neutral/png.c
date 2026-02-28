// Nonnon Portable Network Graphics
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : MinGW-w64 : Link : -lz


// [Mechanism] : when using DLL : currently not needed
//
//	http://gnuwin32.sourceforge.net/packages/zlib.htm
//
//	[ How to Get ]
//
//	1 : click "Binaries"        to get "zlib-1.2.3-bin.zip"
//	2 : click "Developer files" to get "zlib123-dll.zip"
//	3 : done!
//
//	[ How to Install ]
//
//	1 : merge "include" and "lib" folders into "MinGW/include" "MinGW/lib" folders
//	2 : put "bin/zlib1.dll" into a "MinGW/bin" folder
//	3 : done!
//
//	[ How to include ]
//
//	#include <zlib.h>
//
//	[ How to Link ]
//
//	-lzdll


// Simplified PNG : Supported Format
//
//	Read
//		64bit
//		32bit
//		24bit
//		 8bit index-color
//		 4bit index-color
//
//	Write : auto-optimization
//		32bit
//		24bit
//		 8bit index-color
//		 4bit index-color




#ifndef _H_NONNON_NEUTRAL_PNG
#define _H_NONNON_NEUTRAL_PNG




#include <zlib.h>




#include "./bmp/_codec.c"
#include "./endian.c"
#include "./string_path.c"




#define N_PNG_EXT_PNG n_posix_literal( ".png\0\0" )
#define N_PNG_EXT_BMP n_posix_literal( ".bmp\0\0" )




#pragma pack( push, 1 )

typedef struct {

	u8   _png[ 8 ];

	u32  ihdr_byte;
	u8   ihdr_header[ 4 ];
	u32  ihdr_sx;
	u32  ihdr_sy;
	u8   ihdr_bit;
	u8   ihdr_mode;
	u8   ihdr_compress;
	u8   ihdr_filter;
	u8   ihdr_interlace;
	u32  ihdr_crc;

	u32  plte_byte;
	u8   plte_header[ 4 ];
	u8  *plte_ptr;
	u32  plte_crc;

	u32  trns_byte;
	u8   trns_header[ 4 ];
	u8  *trns_ptr;
	u32  trns_crc;

	u32  idat_byte;
	u8   idat_header[ 4 ];
	u8  *idat_ptr;
	u32  idat_crc;
/*
	u32  gama_byte;
	u8   gama_header[ 4 ];
	u32  gama_val;
	u32  gama_crc;

	u32  chrm_byte;
	u8   chrm_header[ 4 ];
	u32  chrm_val   [ 8 ];
	u32  chrm_crc;
*/
	u32  iend_byte;
	u8   iend_header[ 4 ];
	u32  iend_crc;

} n_png;

#pragma pack( pop )


static n_png n_png_template = {

	{ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a },

	13,
	{ 0x49, 0x48, 0x44, 0x52 },
	0,0,
	0,0,0,0,0,
	0,

	0,
	{ 0x50, 0x4c, 0x54, 0x45 },
	NULL,
	0,

	0,
	{ 0x74, 0x52, 0x4e, 0x53 },
	NULL,
	0,

	0,
	{ 0x49, 0x44, 0x41, 0x54 },
	NULL,
	0,
/*
	0,
	{ 0x67, 0x41, 0x4d, 0x41 },
	0,
	0,

	0,
	{ 0x63, 0x48, 0x52, 0x4d },
	{ 0,0, 0,0, 0,0, 0,0 },
	0,
*/
	0,
	{ 0x49, 0x45, 0x4e, 0x44 },
	0,

};




#define n_png_zero(  f    ) n_memory_zero( f,    sizeof( n_png ) )
#define n_png_alias( f, t ) n_memory_copy( f, t, sizeof( n_png ) )

void
n_png_free( n_png *png )
{

	if ( png == NULL ) { return; }


	n_memory_free( png->plte_ptr );
	n_memory_free( png->trns_ptr );
	n_memory_free( png->idat_ptr );

	n_png_zero( png );


	return;
}

int
n_png_paeth( int a, int b, int c )
{

	int pa,pb,pc, ret;


	pa = abs( b - c );
	pb = abs( a - c );
	pc = abs( ( b - c ) + ( a - c ) );

	if ( ( pa <= pb )&&( pa <= pc ) )
	{
		ret = a;
	} else
	if ( pb <= pc )
	{
		ret = b;
	} else {
		ret = c;
	}


	return ret;
}

// internal
BOOL
n_png_compress( n_png *png, n_bmp *bmp )
{

	// [Mechanism] : "bmp"
	//
	//	semi-const
	//	palette will be made temporarily
	//	no touch with other members


	if ( png == NULL ) { return TRUE; }
	if ( bmp == NULL ) { return TRUE; }


	// [!] : tuning is useless
	//
	//	Z_DEFAULT_COMPRESSION :   0%
	//	Z_BEST_COMPRESSION    : -10%
	//
	//	Z_DEFAULT_STRATEGY    :   0%
	//	Z_RLE                 : +50%
	//	Z_HUFFMAN_ONLY        : +50%

	z_stream z;
	int      ret;

	n_memory_zero( &z, sizeof( z_stream ) );

	//ret = deflateInit( &z, Z_DEFAULT_COMPRESSION );
	//ret = deflateInit( &z, Z_BEST_COMPRESSION );
	ret = deflateInit( &z, Z_BEST_SPEED );
	//ret = deflateInit2( &z, Z_BEST_SPEED, Z_DEFLATED, MAX_WBITS, 8, Z_HUFFMAN_ONLY );
	if ( ret != Z_OK ) { return TRUE; }


	int  fullcolor   = n_bmp_palette( bmp );
	BOOL alpha_onoff = n_bmp_alpha_is_used( bmp );

	if ( fullcolor   ) { fullcolor = 24; }
	if ( alpha_onoff ) { fullcolor = 32; }

//n_posix_debug_literal( "%d : %d", alpha_onoff, fullcolor );


	// [x] : encode error will occur in some width
	//
	//	allocate enough size
	//	zero-clear always


	u32  fbyte = 1 + (u32) n_bmp_linebyte( N_BMP_SX( bmp ), N_BMP_DEPTH( bmp ) );
	u32  tbyte = ( fbyte * N_BMP_SY( bmp ) ) * 2;
	u8  *fdata = n_memory_new( fbyte );
	u8  *tdata = n_memory_new( tbyte );
//n_posix_debug_literal( "%d", fbyte );


	n_memory_zero( fdata, fbyte );
	n_memory_zero( tdata, tbyte );


	// [!] : less colors make better compression
	//
	//	0 : PNG_FILTER_VALUE_NONE  :   0%
	//	1 : PNG_FILTER_VALUE_SUB   : - 1%
	//	2 : PNG_FILTER_VALUE_UP    : -10%
	//	3 : PNG_FILTER_VALUE_AVG   : +10%
	//	4 : PNG_FILTER_VALUE_PAETH : -10%

	int filter = 0;


	// [!] : set filter : set zero at first line for simple implementation

	fdata[ 0 ] = 0;

	n_type_int i   = 1;
	n_type_int ii  = 0;
	n_type_int bit = 0;


	n_type_gfx x  = 0;
	n_type_gfx y  = 0;
	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );
	n_posix_loop
	{

		// [!] : Copilot Optimization

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		// Cache color components for this pixel
		int cr = n_bmp_r( color );
		int cg = n_bmp_g( color );
		int cb = n_bmp_b( color );
		int ca = (fullcolor == 32) ? n_bmp_a( color ) : 0;

		if ( fullcolor )
		{
			if ( fdata[ 0 ] == 0 )
			{
				fdata[ i + 0 ] = cr;
				fdata[ i + 1 ] = cg;
				fdata[ i + 2 ] = cb;
				if ( fullcolor == 32 )
				{
					fdata[ i + 3 ] = ca;
				}
			} else
			if ( fdata[ 0 ] == 1 )
			{
				u32 left;
				if ( x == 0 )
				{
					left = 0;
				} else {
					n_bmp_ptr_get( bmp, x-1,y-0, &left );
				}
				int lr = n_bmp_r( left );
				int lg = n_bmp_g( left );
				int lb = n_bmp_b( left );
				int la = (fullcolor == 32) ? n_bmp_a( left ) : 0;
				fdata[ i + 0 ] = cr - lr;
				fdata[ i + 1 ] = cg - lg;
				fdata[ i + 2 ] = cb - lb;
				if ( fullcolor == 32 )
				{
					fdata[ i + 3 ] = ca - la;
				}
			} else
			if ( fdata[ 0 ] == 2 )
			{
				u32 upper;
				n_bmp_ptr_get( bmp, x-0,y-1, &upper );
				int ur = n_bmp_r( upper );
				int ug = n_bmp_g( upper );
				int ub = n_bmp_b( upper );
				int ua = (fullcolor == 32) ? n_bmp_a( upper ) : 0;
				fdata[ i + 0 ] = cr - ur;
				fdata[ i + 1 ] = cg - ug;
				fdata[ i + 2 ] = cb - ub;
				if ( fullcolor == 32 )
				{
					fdata[ i + 3 ] = ca - ua;
				}
			} else
			if ( fdata[ 0 ] == 3 )
			{
				u32 upper,left;
				n_bmp_ptr_get( bmp, x-1,y-0, &left  );
				n_bmp_ptr_get( bmp, x-0,y-1, &upper );
				if ( x == 0 ) { left = 0; }
				int lr = n_bmp_r( left );
				int lg = n_bmp_g( left );
				int lb = n_bmp_b( left );
				int la = (fullcolor == 32) ? n_bmp_a( left ) : 0;
				int ur = n_bmp_r( upper );
				int ug = n_bmp_g( upper );
				int ub = n_bmp_b( upper );
				int ua = (fullcolor == 32) ? n_bmp_a( upper ) : 0;
				fdata[ i + 0 ] = cr - ( ( lr + ur ) / 2 );
				fdata[ i + 1 ] = cg - ( ( lg + ug ) / 2 );
				fdata[ i + 2 ] = cb - ( ( lb + ub ) / 2 );
				if ( fullcolor == 32 )
				{
					fdata[ i + 3 ] = ca - ( ( la + ua ) / 2 );
				}
			} else
			if ( fdata[ 0 ] == 4 )
			{
				u32 ul,l,u;
				n_bmp_ptr_get( bmp, x-0,y-1, &u  );
				n_bmp_ptr_get( bmp, x-1,y-1, &ul );
				n_bmp_ptr_get( bmp, x-1,y-0, &l  );
				int ur = n_bmp_r( u );
				int ug = n_bmp_g( u );
				int ub = n_bmp_b( u );
				int ua = (fullcolor == 32) ? n_bmp_a( u ) : 0;
				int ulr = n_bmp_r( ul );
				int ulg = n_bmp_g( ul );
				int ulb = n_bmp_b( ul );
				int ula = (fullcolor == 32) ? n_bmp_a( ul ) : 0;
				int lr = n_bmp_r( l );
				int lg = n_bmp_g( l );
				int lb = n_bmp_b( l );
				int la = (fullcolor == 32) ? n_bmp_a( l ) : 0;
				fdata[ i + 0 ] = (u8) ( cr - n_png_paeth( lr, ur, ulr ) );
				fdata[ i + 1 ] = (u8) ( cg - n_png_paeth( lg, ug, ulg ) );
				fdata[ i + 2 ] = (u8) ( cb - n_png_paeth( lb, ub, ulb ) );
				if ( fullcolor == 32 )
				{
					fdata[ i + 3 ] = (u8)( ca - n_png_paeth( la, ua, ula ) );
				}
			}

			i += 3;
			if ( fullcolor == 32 ) { i++; }

		} else {

			n_type_int index = 0;
			n_posix_loop
			{
				if ( color == N_BMP_PTR_PAL( bmp )[ index ] ) { break; }

				index++;
			}


			if ( 16 < N_BMP_PAL(bmp) )
			{

				fdata[ i ] = (u8) index;
				i++;

			} else {

				if ( bit == 0 )
				{
					bit = 4;
					fdata[ i ]  = (u8) ( index << bit );
				} else {
					bit = 0;
					fdata[ i ] += (u8) ( index << bit );
					i++;
				}

			}

		}


		x++;
		if ( x >= sx )
		{

			// [!] : when width has odd number

			if ( bit )
			{
				bit = 0;
				i++;
			}


			z.total_in  =  0;
			z.total_out =  0;
			z.avail_in  =  (uInt) i;
			z.avail_out =  (uInt) tbyte - (uInt) ii;
			z.next_in   =  fdata;
			z.next_out  = &tdata[ ii ];

			ret = deflate( &z, Z_SYNC_FLUSH );
			if ( ret != Z_OK ) { break; }


			ii += z.total_out;


			// [!] : reset filter

			fdata[ 0 ] = filter;
			i = 1;


			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}


	if ( ret == Z_OK )
	{

		// [Needed] : set .total_*  .avail_*, .next_*

		z.total_in  =  0;
		z.total_out =  0;
		z.avail_in  =  0;
		z.avail_out =  (uInt) tbyte - (uInt) ii;
		z.next_in   =  NULL;
		z.next_out  = &tdata[ ii ];

		ret = deflate( &z, Z_FINISH );
//n_posix_debug_literal( "%d %d %d", ret, tbyte, ii );

		if ( ret == Z_STREAM_END ) { ret = Z_OK; }


		// [!] : z.total_out will be 6byte

		ii += z.total_out;


		png->ihdr_sx   = sx;
		png->ihdr_sy   = sy;
		png->ihdr_bit  = 8;
		png->ihdr_mode = 2;
		png->idat_byte = (u32) ii;
		png->idat_ptr  = tdata;

		if ( fullcolor )
		{

			if ( fullcolor == 32 ) { png->ihdr_mode |= 4; }

		} else {

			if ( 16 >= N_BMP_PAL( bmp ) ) { png->ihdr_bit = 4; }

			png->ihdr_mode |= 1;

			png->plte_byte = 3 * N_BMP_PAL( bmp );
			png->plte_ptr  = n_memory_new( png->plte_byte );

			if ( alpha_onoff )
			{
				png->trns_byte = 1 * N_BMP_PAL( bmp );
				png->trns_ptr  = n_memory_new( png->trns_byte );
			}


			n_type_int i  = 0;
			n_type_int ii = 0;
			n_posix_loop
			{

				u32 color = N_BMP_PTR_PAL( bmp )[ i ];


				if ( alpha_onoff )
				{
					png->trns_ptr[ i ] = N_BMP_ALPHA_CHANNEL_INVISIBLE - n_bmp_a( color );
				}


				png->plte_ptr[ ii + 0 ] = n_bmp_r( color );
				png->plte_ptr[ ii + 1 ] = n_bmp_g( color );
				png->plte_ptr[ ii + 2 ] = n_bmp_b( color );

				ii += 3;


				i++;
				if ( i >= N_BMP_PAL( bmp ) ) { break; }
			}

		}

	}


	// Cleanup

	z.total_in = z.total_out = 0;
	z.avail_in = z.avail_out = 0;
	z.next_in  = z.next_out  = NULL;

	deflateEnd( &z );


	n_memory_free( fdata );

	if ( fullcolor == 0 )
	{

		// [!] : prevent double-free

		n_memory_free( N_BMP_PTR_PAL( bmp ) );

		N_BMP_PAL    ( bmp ) = 0;
		N_BMP_PTR_PAL( bmp ) = NULL;

	}


	return ret;
}

void
n_png_uncompress_engine( const n_png *png, n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, u8 *tdata )
{

	n_type_gfx x,y;
	u32        color;
	int        a,r,g,b;
	n_type_int i;
	int        filter;
	n_type_int bit, index;

	int        pass    = 1;
	n_type_gfx start_x = 0;
//n_bmp_flush( bmp, n_bmp_white );

	u32 filter3_pcolor = 0;

	i = 0;
	filter = tdata[ i ]; i++;

	x = y = 0;
	bit = 0;
	a = r = g = b = 0;
	n_posix_loop
	{

		if ( png->ihdr_mode & 1 )
		{

			index = tdata[ i ];

			if ( png->ihdr_bit == 4 )
			{

				if ( bit == 0 )
				{
					index = index >> 4;
				} else {
					index = index >> 0;
				}

				color = N_BMP_PTR_PAL( bmp )[ index & 0x0f ];

				if ( bit == 0 ) { bit = 1; } else { bit = 0; i++; }

			} else {

				color = N_BMP_PTR_PAL( bmp )[ index ]; i++;

			}

		} else {

			// [!] : filter is always used / no relation with .ihdr_filter member

			int aa,rr,gg,bb;
			if ( png->ihdr_bit == 16 )
			{
				int s1 = 0;
				int s2 = 8;

				rr = (int) tdata[ i ] << s1; i++; rr += (int) tdata[ i ] << s2; i++;
				gg = (int) tdata[ i ] << s1; i++; gg += (int) tdata[ i ] << s2; i++;
				bb = (int) tdata[ i ] << s1; i++; bb += (int) tdata[ i ] << s2; i++;
				aa = (int) tdata[ i ] << s1; i++; aa += (int) tdata[ i ] << s2; i++;
			} else {
				rr = tdata[ i ]; i++;
				gg = tdata[ i ]; i++;
				bb = tdata[ i ]; i++;
				if ( png->ihdr_mode & 4 )
				{
					aa = tdata[ i ]; i++;
				} else {
					aa = N_BMP_ALPHA_CHANNEL_VISIBLE;
				}
			}


			if ( filter == 0 )
			{
				a = aa;
				r = rr;
				g = gg;
				b = bb;
			} else
			if ( filter == 1 )
			{
				if ( png->ihdr_mode & 4 ) { a += aa; } else { a = aa; }
				r += rr;
				g += gg;
				b += bb;
			} else
			if ( filter == 2 )
			{

				n_type_gfx yy = y - 1;
				if ( png->ihdr_interlace != 0 )
				{
					if ( pass == 1 ) { yy = y - 8; } else
					if ( pass == 2 ) { yy = y - 8; } else
					if ( pass == 3 ) { yy = y - 8; } else
					if ( pass == 4 ) { yy = y - 4; } else
					if ( pass == 5 ) { yy = y - 4; } else
					if ( pass == 6 ) { yy = y - 2; } else
					if ( pass == 7 ) { yy = y - 2; }
				}

				color = 0;
				n_bmp_ptr_get( bmp, x,yy, &color );

				if ( png->ihdr_mode & 4 ) { a = aa + n_bmp_a( color ); } else { a = aa; }
				r = rr + n_bmp_r( color );
				g = gg + n_bmp_g( color );
				b = bb + n_bmp_b( color ) ;
//r = 255;
			} else
			if ( filter == 3 )
			{

				n_type_gfx yy = y - 1;
				if ( png->ihdr_interlace != 0 )
				{
					if ( pass == 1 ) { yy = y - 8; } else
					if ( pass == 2 ) { yy = y - 8; } else
					if ( pass == 3 ) { yy = y - 8; } else
					if ( pass == 4 ) { yy = y - 4; } else
					if ( pass == 5 ) { yy = y - 4; } else
					if ( pass == 6 ) { yy = y - 2; } else
					if ( pass == 7 ) { yy = y - 2; }
				}

				color = 0;
				n_bmp_ptr_get( bmp, x,yy, &color );
				if ( x == start_x ) { filter3_pcolor = 0; }

				if ( png->ihdr_mode & 4 ) { a = aa + ( ( n_bmp_a( filter3_pcolor ) + n_bmp_a( color ) ) / 2 ); } else { a = aa; }
				r = rr + ( ( n_bmp_r( filter3_pcolor ) + n_bmp_r( color ) ) / 2 );
				g = gg + ( ( n_bmp_g( filter3_pcolor ) + n_bmp_g( color ) ) / 2 );
				b = bb + ( ( n_bmp_b( filter3_pcolor ) + n_bmp_b( color ) ) / 2 );

				filter3_pcolor = n_bmp_argb( a,r,g,b );
//g = 255;
			} else
			if ( filter == 4 )
			{

				n_type_gfx xx = x - 1;
				n_type_gfx yy = y - 1;
				if ( png->ihdr_interlace != 0 )
				{
					if ( pass == 1 ) { xx = x - 8; yy = y - 8; } else
					if ( pass == 2 ) { xx = x - 8; yy = y - 8; } else
					if ( pass == 3 ) { xx = x - 4; yy = y - 8; } else
					if ( pass == 4 ) { xx = x - 4; yy = y - 4; } else
					if ( pass == 5 ) { xx = x - 2; yy = y - 4; } else
					if ( pass == 6 ) { xx = x - 2; yy = y - 2; } else
					if ( pass == 7 ) { xx = x - 1; yy = y - 2; }
				}

				color = 0;
				n_bmp_ptr_get( bmp, x,yy, &color );

				if ( x == start_x )
				{

					if ( png->ihdr_mode & 4 ) { a = aa + n_bmp_a( color ); } else { a = aa; }
					r = rr + n_bmp_r( color );
					g = gg + n_bmp_g( color );
					b = bb + n_bmp_b( color );

				} else {

					u32 ul = 0; n_bmp_ptr_get( bmp, xx,yy, &ul );
					u32 dl = 0; n_bmp_ptr_get( bmp, xx, y, &dl );

					if ( png->ihdr_mode & 4 ) { a = aa + n_png_paeth( n_bmp_a( dl ), n_bmp_a( color ), n_bmp_a( ul ) ); } else { a = aa; }
					r = rr + n_png_paeth( n_bmp_r( dl ), n_bmp_r( color ), n_bmp_r( ul ) );
					g = gg + n_png_paeth( n_bmp_g( dl ), n_bmp_g( color ), n_bmp_g( ul ) );
					b = bb + n_png_paeth( n_bmp_b( dl ), n_bmp_b( color ), n_bmp_b( ul ) );

				}
//b = 255;
			} else {
				n_bmp_free( bmp );
				n_memory_free( tdata );

				return;
			}


			// gAMA : how to implement?
/*
			if ( png->gama_val )
			{
				n_type_real d = (n_type_real) 100000 / png->gama_val;
				//a = pow( a, d );
				r = pow( r, d );
				g = pow( g, d );
				b = pow( b, d );
			}
*/

			// cHRM : how to implement?

			//


			color = n_bmp_argb( a,r,g,b );

		}


		if ( png->ihdr_interlace != 0 )
		{
			n_bmp_ptr_set( bmp, x,y, color );

			if ( pass == 1 ) { x += 8; } else
			if ( pass == 2 ) { x += 8; } else
			if ( pass == 3 ) { x += 4; } else
			if ( pass == 4 ) { x += 4; } else
			if ( pass == 5 ) { x += 2; } else
			if ( pass == 6 ) { x += 2; } else
			if ( pass == 7 ) { x += 1; }
		} else {
			n_bmp_ptr_set_fast( bmp, x,y, color );

			x++;
		}

		if ( x >= sx )
		{

//if ( y == 1 ) { n_posix_debug_literal( "Y=%d : %d", y + 1, tdata[ i ] ); }


			filter = tdata[ i ]; i++;


			// [!] : for alignment

			if ( bit ) { bit = 0; i++; }


			x = start_x = 0;
			a = r = g = b = 0;


			if ( png->ihdr_interlace != 0 )
			{
				if ( pass == 1 ) { x = 0; y += 8; } else
				if ( pass == 2 ) { x = 4; y += 8; } else
				if ( pass == 3 ) { x = 0; y += 8; } else
				if ( pass == 4 ) { x = 2; y += 4; } else
				if ( pass == 5 ) { x = 0; y += 4; } else
				if ( pass == 6 ) { x = 1; y += 2; } else
				if ( pass == 7 ) { x = 0; y += 2; }
				start_x = x;
			} else {
				y++;
			}


			if ( y >= sy )
			{
				if ( png->ihdr_interlace != 0 )
				{
					pass++;
					if ( pass >= 8 ) { break;        } else
					if ( pass == 2 ) { x = 4; y = 0; } else
					if ( pass == 3 ) { x = 0; y = 4; } else
					if ( pass == 4 ) { x = 2; y = 0; } else
					if ( pass == 5 ) { x = 0; y = 2; } else
					if ( pass == 6 ) { x = 1; y = 0; } else
					if ( pass == 7 ) { x = 0; y = 1; }
					start_x = x;
				} else {
					break;
				}
			}
		}
	}

//n_posix_debug_literal( "%d/%d", i, tbyte );

	return;
}

// internal
BOOL
n_png_uncompress( const n_png *png, n_bmp *bmp )
{

	if ( png == NULL ) { return TRUE; }
	if ( bmp == NULL ) { return TRUE; }


	n_type_gfx sx = png->ihdr_sx;
	n_type_gfx sy = png->ihdr_sy;

	if ( n_bmp_is_overflow( sx, sy ) ) { return TRUE; }


	// Phase 1

	// [!] : Mac : don't use u32

	uLong  fbyte = png->idat_byte;
	uLong  tbyte = (uLong) n_bmp_size( sx,sy, 32 ) * 2; if ( png->ihdr_bit == 16 ) { tbyte *= 2; }
	Bytef *fdata = png->idat_ptr;
	Bytef *tdata = n_memory_new( tbyte );

	if ( Z_OK != uncompress( tdata, &tbyte, fdata, fbyte ) )
	{
		n_memory_free( tdata );
		return TRUE;
	}
//n_memory_free( tdata ); return TRUE;


	{ // Phase 2

	n_bmp bmp_target; n_bmp_zero( &bmp_target ); n_bmp_1st_fast( &bmp_target, sx,sy );


	if ( png->plte_ptr != NULL )
	{

		N_BMP_PAL    ( &bmp_target ) = png->plte_byte / 3;
		N_BMP_PTR_PAL( &bmp_target ) = n_memory_new( N_BMP_PAL( &bmp_target ) * sizeof( u32 ) );

		u32  i = 0;
		u32 ii = 0;
		n_posix_loop
		{//break;

			int a;
			int r = png->plte_ptr[ ii + 0 ];
			int g = png->plte_ptr[ ii + 1 ];
			int b = png->plte_ptr[ ii + 2 ];

			ii += 3;


			// [!] : it is different between tRNS count and palette count 

			if ( ( png->trns_ptr != NULL )&&( png->trns_byte > i ) )
			{
				a = abs( N_BMP_ALPHA_CHANNEL_INVISIBLE - png->trns_ptr[ i ] );
			} else {
				a = N_BMP_ALPHA_CHANNEL_VISIBLE;
			}


			N_BMP_PTR_PAL( &bmp_target )[ i ] = n_bmp_argb( a,r,g,b );


			i++;
			if ( i >= N_BMP_PAL( &bmp_target ) ) { break; }
		}

	}
//n_bmp_free( &bmp_target ); n_memory_free( tdata ); return TRUE;


	n_png_uncompress_engine( png, &bmp_target, sx,sy, tdata );


	n_bmp_replace( &bmp_target, bmp );


	} // Phase 2


	// Phase 3 : Cleanup

	n_memory_free( tdata );

	if ( png->plte_ptr != NULL )
	{

		// [!] : prevent double-free

		n_memory_free( N_BMP_PTR_PAL( bmp ) );

		N_BMP_PAL    ( bmp ) = 0;
		N_BMP_PTR_PAL( bmp ) = NULL;

	}


	return FALSE;
}

#define n_png_load(          png, filename  ) n_png_load_internal( png, (void*) filename,    0, TRUE  )
#define n_png_load_onmemory( png, ptr, size ) n_png_load_internal( png,              ptr, size, FALSE )

// internal
BOOL
n_png_load_internal( n_png *arg_png, u8 *ptr, n_type_int ptrsize, BOOL is_file )
{

	if ( arg_png == NULL ) { return TRUE; }


	const n_type_int _png_byte = 8;
	const n_type_int unit_byte = 4 + 4;
	const n_type_int ihdr_byte = unit_byte + 13 + 4;
	const n_type_int minimbyte = _png_byte + ihdr_byte + unit_byte;


	// Phase 1 : _PNG : not a PNG file

	n_png      png_template = n_png_template;
	n_png      png          = n_png_template;
	BOOL       onmemory     = FALSE;
	n_type_int seek         = 0;


	if ( is_file )
	{

		const n_posix_char *name = (void*) ptr;

		ptrsize = n_posix_stat_size( name );
		if ( minimbyte > ptrsize ) { return TRUE; }

		FILE *fp = n_posix_fopen_read( name );
		if ( fp == NULL ) { return TRUE; }


		// [!] : sniffer

		n_posix_fread( &png._png, _png_byte, 1, fp );

		if ( FALSE == n_memory_is_same( &png, &png_template, _png_byte ) )
		{

			n_posix_fclose( fp );

			return TRUE;
		}


		n_posix_fseek( fp, 0, SEEK_SET );

		ptr = n_memory_new( ptrsize );
		n_posix_fread( ptr, ptrsize, 1, fp );


		n_posix_fclose( fp );

	} else {

		if ( minimbyte > ptrsize ) { return TRUE; }


		// [!] : sniffer

		n_memory_copy( &ptr[ seek ], &png._png, _png_byte );

		if ( FALSE == n_memory_is_same( &png, &png_template, _png_byte ) )
		{
			return TRUE;
		}


		onmemory = TRUE;

	}

	seek += _png_byte;


	// Phase 2 : IHDR

	n_memory_copy( &ptr[ seek ], &png.ihdr_byte, ihdr_byte ); seek += ihdr_byte;

	png.ihdr_byte = n_endian_big( png.ihdr_byte, 4 );
	png.ihdr_sx   = n_endian_big( png.ihdr_sx,   4 );
	png.ihdr_sy   = n_endian_big( png.ihdr_sy,   4 );

//n_posix_debug_literal( " %d %d ", png.ihdr_bit, png.ihdr_mode );

	if (
		( png_template.ihdr_byte != png.ihdr_byte )
		||
		( FALSE == n_memory_is_same( png.ihdr_header, png_template.ihdr_header, 4 ) )
		||
		( png.ihdr_sx == 0 )
		||
		( png.ihdr_sy == 0 )
		||
		(
			( 0 == ( ( png.ihdr_bit ==  4 )&&( png.ihdr_mode == ( 2 + 1 ) ) ) )
			&&
			( 0 == ( ( png.ihdr_bit ==  8 )&&( png.ihdr_mode == ( 2 + 1 ) ) ) )
			&&
			( 0 == ( ( png.ihdr_bit ==  8 )&&( png.ihdr_mode == ( 2     ) ) ) )
			&&
			( 0 == ( ( png.ihdr_bit ==  8 )&&( png.ihdr_mode == ( 2 + 4 ) ) ) )
			&&
			( 0 == ( ( png.ihdr_bit == 16 )&&( png.ihdr_mode == ( 2 + 4 ) ) ) )
		)
		||
		( png.ihdr_compress != 0 )
		||
		( png.ihdr_filter != 0 )
		//||
		//( png.ihdr_interlace != 0 )
	)
	{

		if ( onmemory == FALSE ) { n_memory_free( ptr ); }

		return TRUE;
	}


	// Phase 3

	BOOL error = FALSE;
	n_posix_loop
	{

		// PLTE

		n_memory_copy( &ptr[ seek ], &png.plte_byte, unit_byte );

//n_posix_debug_literal( "%d", n_memory_is_same( png.plte_header, png_template.plte_header, 4 ) );

		png.plte_byte = n_endian_big( png.plte_byte, 4 );

		if ( png.plte_byte >= ( 256 * 4 ) )
		{
//n_posix_debug_literal( "PLTE : invalid byte count" );
			png.plte_byte = ( 256 * 4 );
		}

		if ( n_memory_is_same( png.plte_header, png_template.plte_header, 4 ) )
		{
//n_posix_debug_literal( " PLTE " );

			seek += unit_byte;

			png.plte_ptr = n_memory_new( png.plte_byte );

			n_memory_copy( &ptr[ seek ],  png.plte_ptr, png.plte_byte ); seek += png.plte_byte;
			n_memory_copy( &ptr[ seek ], &png.plte_crc,             4 ); seek += 4;


			// [!] : fail-safe

			png.plte_byte = 256 * sizeof( u32 );
			png.plte_ptr  = n_memory_resize( png.plte_ptr, png.plte_byte );

		}


		// tRNS

		n_memory_copy( &ptr[ seek ], &png.trns_byte, unit_byte );

		png.trns_byte = n_endian_big( png.trns_byte, 4 );

		if ( png.trns_byte >= 256 )
		{
//n_posix_debug_literal( "tRNS : invalid byte count" );
			png.trns_byte = 256;
		}

		if ( n_memory_is_same( png.trns_header, png_template.trns_header, 4 ) )
		{
//n_posix_debug_literal( " tRNS " );

			seek += unit_byte;

			// [!] : fail-safe
			//
			//	255 means "no transparent"

			png.trns_ptr = n_memory_new( 256 );
			n_memory_padding_unsigned( png.trns_ptr, 255, png.trns_byte );


			n_memory_copy( &ptr[ seek ],  png.trns_ptr, png.trns_byte ); seek += png.trns_byte;
			n_memory_copy( &ptr[ seek ], &png.trns_crc,             4 ); seek += 4;

		}


		// gAMA
/*
		n_memory_copy( &ptr[ seek ], &png.gama_byte, unit_byte );

		png.gama_byte = n_endian_big( png.gama_byte, 4 );

		if ( n_memory_is_same( png.gama_header, png_template.gama_header, 4 ) )
		{

			seek += unit_byte;

			n_memory_copy( &ptr[ seek ], &png.gama_val, 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.gama_crc, 4 ); seek += 4;

			png.gama_val = n_endian_big( png.gama_val, 4 );

n_posix_debug_literal( "%d", png.gama_val );
		}
*/

		// cHRM
/*
		n_memory_copy( &ptr[ seek ], &png.chrm_byte, unit_byte );

		png.chrm_byte = n_endian_big( png.chrm_byte, 4 );

		if ( n_memory_is_same( png.chrm_header, png_template.chrm_header, 4 ) )
		{

			seek += unit_byte;

			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 0 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 1 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 2 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 3 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 4 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 5 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 6 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_val[ 7 ], 4 ); seek += 4;
			n_memory_copy( &ptr[ seek ], &png.chrm_crc,      4 ); seek += 4;

			png.chrm_val[ 0 ] = n_endian_big( png.chrm_val[ 0 ], 4 );
			png.chrm_val[ 1 ] = n_endian_big( png.chrm_val[ 1 ], 4 );
			png.chrm_val[ 2 ] = n_endian_big( png.chrm_val[ 2 ], 4 );
			png.chrm_val[ 3 ] = n_endian_big( png.chrm_val[ 3 ], 4 );
			png.chrm_val[ 4 ] = n_endian_big( png.chrm_val[ 4 ], 4 );
			png.chrm_val[ 5 ] = n_endian_big( png.chrm_val[ 5 ], 4 );
			png.chrm_val[ 6 ] = n_endian_big( png.chrm_val[ 6 ], 4 );
			png.chrm_val[ 7 ] = n_endian_big( png.chrm_val[ 7 ], 4 );

n_posix_debug_literal(
	"%d %d %d %d %d %d %d %d",
	png.chrm_val[ 0 ],png.chrm_val[ 1 ],png.chrm_val[ 2 ],png.chrm_val[ 3 ],
	png.chrm_val[ 4 ],png.chrm_val[ 5 ],png.chrm_val[ 6 ],png.chrm_val[ 7 ]
);
		}
*/

		// IDAT : some encoder writes multiple IDATs with 8192 byte unit

		u32 i, byte;
		u8  ihdr[ 4 ];
		u32 crc;

		i = png.idat_byte = 0;
		n_posix_loop
		{

			n_memory_copy( &ptr[ seek + 0 ], &byte, 4 ); byte = n_endian_big( byte, 4 );
			n_memory_copy( &ptr[ seek + 4 ], &ihdr, 4 );

			if ( FALSE == n_memory_is_same( &ihdr, png_template.idat_header, 4 ) )
			{
				break;
			} else {
				seek += unit_byte;
			}

			png.idat_byte += byte;
			png.idat_ptr = n_memory_resize( png.idat_ptr, png.idat_byte );

			n_memory_copy( &ptr[ seek ], &png.idat_ptr[ i ], byte ); seek += byte;
			i += byte;


			n_memory_copy( &ptr[ seek ], &crc, 4 ); seek += 4;

		}


		// IEND or Unknown or Not Supported

		// [!] : [ byte(4) + chunk_name(4) + data(n) + CRC32(4) ]

		n_memory_copy( &ptr[ seek ], &byte, sizeof( u32 ) );

		seek += unit_byte + n_endian_big( byte, 4 ) + 4;


		if ( seek >= ptrsize ) { break; }
	}


/*
// [!] : 8 or higher will be error, 6 or lower will be the same result

png.idat_ptr[ 0 ]  = 0x78;
png.idat_ptr[ 1 ] &= 0xe0;
png.idat_ptr[ 1 ] += (u8) ( 0x1f - ( ( png.idat_ptr[ 0 ] << 8 ) + png.idat_ptr[ 1 ] ) % 0x1f );
*/


	// Cleanup

	if ( error )
	{
		n_png_free( &png );
	} else {
		n_png_free( arg_png );
		n_png_alias( &png, arg_png );
	}


	if ( onmemory == FALSE ) { n_memory_free( ptr ); }


	return error;
}

#define n_png_save_literal( png, name ) n_png_save( png, n_posix_literal( name ) )

BOOL
n_png_save( const n_png *arg_png, const n_posix_char *pngname )
{

	if ( arg_png == NULL ) { return TRUE; }


	n_png      png = *arg_png;
	n_type_int p   =  png.plte_byte;
	n_type_int t   =  png.trns_byte;
	n_type_int i   =  png.idat_byte;


	// convert to big endian #1 : before CRC

	png.ihdr_sx   = n_endian_big( png.ihdr_sx, 4 );
	png.ihdr_sy   = n_endian_big( png.ihdr_sy, 4 );


	{ // CRC

	u8 *plte_stream = n_memory_new_closed( 4 + p );
	u8 *trns_stream = n_memory_new_closed( 4 + t );
	u8 *idat_stream = n_memory_new_closed( 4 + i );

	n_memory_copy( png.plte_header, &plte_stream[ 0 ], 4 );
	n_memory_copy( png.plte_ptr,    &plte_stream[ 4 ], p );

	n_memory_copy( png.trns_header, &trns_stream[ 0 ], 4 );
	n_memory_copy( png.trns_ptr,    &trns_stream[ 4 ], t );

	n_memory_copy( png.idat_header, &idat_stream[ 0 ], 4 );
	n_memory_copy( png.idat_ptr,    &idat_stream[ 4 ], i );

	png.ihdr_crc  = n_posix_crc32( png.ihdr_header, 4 + png.ihdr_byte );
	png.plte_crc  = n_posix_crc32( plte_stream,     4 + png.plte_byte );
	png.trns_crc  = n_posix_crc32( trns_stream,     4 + png.trns_byte );
	png.idat_crc  = n_posix_crc32( idat_stream,     4 + png.idat_byte );
	png.iend_crc  = n_posix_crc32( png.iend_header, 4 + png.iend_byte );

	n_memory_free_closed( plte_stream );
	n_memory_free_closed( trns_stream );
	n_memory_free_closed( idat_stream );

	} // CRC


	// convert to big endian #2 : after CRC

	png.ihdr_byte = n_endian_big( png.ihdr_byte, 4 );
	png.ihdr_crc  = n_endian_big( png.ihdr_crc,  4 );

	png.plte_byte = n_endian_big( png.plte_byte, 4 );
	png.plte_crc  = n_endian_big( png.plte_crc,  4 );

	png.trns_byte = n_endian_big( png.trns_byte, 4 );
	png.trns_crc  = n_endian_big( png.trns_crc,  4 );

	png.idat_byte = n_endian_big( png.idat_byte, 4 );
	png.idat_crc  = n_endian_big( png.idat_crc,  4 );

	png.iend_byte = n_endian_big( png.iend_byte, 4 );
	png.iend_crc  = n_endian_big( png.iend_crc,  4 );


	FILE *fp = n_posix_fopen_write( pngname );
	if ( fp == NULL ) { return TRUE; }

	n_posix_fwrite(  png._png,           8, 1, fp );
	n_posix_fwrite( &png.ihdr_byte,      4, 1, fp );
	n_posix_fwrite(  png.ihdr_header,    4, 1, fp );
	n_posix_fwrite( &png.ihdr_sx,        4, 1, fp );
	n_posix_fwrite( &png.ihdr_sy,        4, 1, fp );
	n_posix_fwrite( &png.ihdr_bit,       1, 1, fp );
	n_posix_fwrite( &png.ihdr_mode,      1, 1, fp );
	n_posix_fwrite( &png.ihdr_compress,  1, 1, fp );
	n_posix_fwrite( &png.ihdr_filter,    1, 1, fp );
	n_posix_fwrite( &png.ihdr_interlace, 1, 1, fp );
	n_posix_fwrite( &png.ihdr_crc,       4, 1, fp );

	if ( png.plte_ptr != NULL )
	{
		n_posix_fwrite( &png.plte_byte,      4, 1, fp );
		n_posix_fwrite(  png.plte_header,    4, 1, fp );
		n_posix_fwrite(  png.plte_ptr,       p, 1, fp );
		n_posix_fwrite( &png.plte_crc,       4, 1, fp );
	}

	if ( png.trns_ptr != NULL )
	{
		n_posix_fwrite( &png.trns_byte,      4, 1, fp );
		n_posix_fwrite(  png.trns_header,    4, 1, fp );
		n_posix_fwrite(  png.trns_ptr,       t, 1, fp );
		n_posix_fwrite( &png.trns_crc,       4, 1, fp );
	}

	n_posix_fwrite( &png.idat_byte,      4, 1, fp );
	n_posix_fwrite(  png.idat_header,    4, 1, fp );
	n_posix_fwrite(  png.idat_ptr,       i, 1, fp );
	n_posix_fwrite( &png.idat_crc,       4, 1, fp );

	n_posix_fwrite( &png.iend_byte,      4, 1, fp );
	n_posix_fwrite(  png.iend_header,    4, 1, fp );
	n_posix_fwrite( &png.iend_crc,       4, 1, fp );

	n_posix_fclose( fp );


	return FALSE;
}

#define n_png_bmp2png_autosave( a ) n_png_bmp2png( a, NULL )

BOOL
n_png_bmp2png( n_posix_char *name, n_png *png_ret )
{

	n_png png = n_png_template;
	n_bmp bmp;

	BOOL ret;


	n_bmp_zero( &bmp );

	ret = n_bmp_load( &bmp, name );
	if ( ret ) { return TRUE; }


	ret = n_png_compress( &png, &bmp );
	if ( ret == FALSE )
	{

		if ( png_ret == NULL )
		{

			n_posix_char *newname = n_string_path_ext_mod_new( name, N_PNG_EXT_PNG );
//n_posix_debug( newname );

			n_png_save( &png, newname );
			n_png_free( &png );

			n_string_path_free( newname );

		} else {

			n_png_free( png_ret );
			n_png_alias( &png, png_ret );

		}

	}


	n_bmp_free( &bmp );


	return ret;
}

#define n_png_png2bmp_autosave( a ) n_png_png2bmp( a, NULL )

BOOL
n_png_png2bmp( n_posix_char *name, n_bmp *bmp_ret )
{

	n_png png; n_png_zero( &png );
	n_bmp bmp; n_bmp_zero( &bmp );

	BOOL ret;


	ret = n_png_load( &png, name );
	if ( ret ) { return TRUE; }


	ret = n_png_uncompress( &png, &bmp );
	if ( ret == FALSE )
	{

		if ( bmp_ret == NULL )
		{

			n_posix_char *newname = n_string_path_ext_mod_new( name, N_PNG_EXT_BMP );

			n_bmp_save( &bmp, newname );
			n_bmp_free( &bmp );

			n_string_path_free( newname );

		} else {

			n_bmp_replace( &bmp, bmp_ret );

		}

	}


	n_png_free( &png );


	return ret;
}


#endif // _H_NONNON_NEUTRAL_PNG

