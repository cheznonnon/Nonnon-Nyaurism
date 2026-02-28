// Nonnon Jpeg
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




// [ Mechanism ] : 32-bit/64-bit : when not using DLL : slower version : currently not used
//
//	directly embed
//
//	[ How to use : for system ]
//
//	1 : include "nonnon/floss/libjpeg.c"
//	2 : done!
//
//	[ How to use : for client ]
//
//	1 : include "nonnon/neutral/jpg.c"
//	2 : done!
//
//	[ How to Link ]
//
//	no link needed


// [ Mechanism ] : x64 : VC++ 2017 : currently used
//
//	2x or more faster than not turbo library
//
//	[ How to Use ]
//
//	1 : install libjpeg-turbo
//	2 : set path of "/include" and "/lib" by properties
//	3 : compile it
//	4 : copy jpeg62.dll to EXE folder
//
//	jpeg_stdio_src()/_dest() will crash, use jpeg_mem_src()/_dest()


// [ Mechanism ] : x86 : when not using DLL : faster version : currently used
//
//	http://cetus.sakura.ne.jp/softlab/jpeg-x86simd/jpegsimd.html
//	+ 2x faster than original libjpeg
//
//	+ static link is available
//
//	[ How to Get ]
//
//	1 : download : jpegsrc-6b-x86simd-1.02.zip
//	2 : done!
//
//	[ How to Install ]
//
//	1 : install MinGW MSYS
//	2 : copy : "jpeg-6bx" to "MinGW/msys/1.0/home/[UserName]"
//	3 : run MSYS
//	4 : run "configure --enable-uchar-boolean"
//	5 : copy "libjpeg.a" to "MinGW/lib" folder
//	6 : copy "jpeglib.h" "jconfig.h" "jmorecfg.h" to "MinGW/include" folder
//	7 : done!
//
//	+ don't mix other libjpeg library's header files
//
//	+ "nonnon/_sdk/jpeg-6bx" has compiled "libjpeg.a" and headers
//
//	[ How to use : for system ]
//
//	1 : #include <jpeglib.h>
//	2 : done!
//
//	[ How to use : for client ]
//
//	1 : include "nonnon/neutral/jpg.c"
//	2 : done!
//
//	[ How to Link ]
//
//	-ljpeg
//
//	[ tips ]
//
//	jpeg_mem_src()/_dest() are not exist in this version




#ifndef _H_NONNON_NEUTRAL_JPG
#define _H_NONNON_NEUTRAL_JPG




#ifdef N_POSIX_PLATFORM_MAC


//#define N_JPG_NO_MEM_API
//#include "../floss/libjpeg.c"


// [!] : hard to explain
//
//	1 : download .dmg from libjpeg-turbo site
//	2 : "/opt/libjpeg-turbo" is made
//	3 : drop "lib/libjpeg.a" left pane of Xcode
//	4 : include "include/*.h"
//
//	don't use "Homebrew" : compatibility is narrow

#include "../mac/libjpeg-turbo/jpeglib.h"


#else  // #ifdef N_POSIX_PLATFORM_MAC




#ifdef _MSC_VER


#pragma comment( lib, "jpeg" )
#include <jpeglib.h>


#else  // #ifdef _MSC_VER


// [!] : fallback

#define N_JPG_NO_MEM_API

#include "../floss/libjpeg.c"


#endif // #ifdef _MSC_VER


#endif // #ifdef N_POSIX_PLATFORM_MAC




#include "./bmp/_codec.c"
#include "./bmp/_error.c"
#include "./bmp.c"
#include "./string.c"




static int n_jpg_quality = 80;




void
n_jpg_swaprgb( n_bmp *bmp )
{

	// [ Mechanism ]
	//
	//	this module also does alpha-clearing
	//	see n_bmp_24bit() for details
/*
	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color;
		n_bmp_ptr_get_fast( bmp, x,y, &color );

		int r = n_bmp_b( color );
		int g = n_bmp_g( color );
		int b = n_bmp_r( color );

		color = n_bmp_rgb( r,g,b );

		n_bmp_ptr_set_fast( bmp, x,y,  color );


		x++;
		if ( x >= bmpsx )
		{

			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}
*/

	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];

		int r = n_bmp_b( color );
		int g = n_bmp_g( color );
		int b = n_bmp_r( color );

		color = n_bmp_rgb( r,g,b );

		N_BMP_PTR( bmp )[ i ] = color;


		i++;
		if ( i >= c ) { break; }
	}


	return;
}

BOOL
n_jpg_bmp2jpg( const n_posix_char *jpgname, const n_bmp *bmp_arg )
{

	if ( n_string_is_empty( jpgname ) ) { return TRUE; }

	if ( n_bmp_error( bmp_arg ) ) { return TRUE; }


	n_bmp bmp; n_bmp_carboncopy( bmp_arg, &bmp );
	n_jpg_swaprgb( &bmp );
	n_bmp_24bit  ( &bmp );

	n_type_gfx bmpsx = N_BMP_SX( &bmp );
	n_type_gfx bmpsy = N_BMP_SY( &bmp );


	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr       jem;

	jcs.err              = jpeg_std_error( &jem );
	jpeg_create_compress( &jcs );


#ifdef N_JPG_NO_MEM_API

	FILE *fp = n_posix_fopen_write( jpgname );
	jpeg_stdio_dest     ( &jcs, fp );

#else  // #ifdef N_JPG_NO_MEM_API

	size_t  byte = 0;
	u8     *ptr  = NULL;

	jpeg_mem_dest       ( &jcs, &ptr, &byte );

#endif // #ifdef N_JPG_NO_MEM_API


	jcs.image_width      = bmpsx;
	jcs.image_height     = bmpsy;
	jcs.input_components = 3;
	jcs.in_color_space   = JCS_RGB;

	jpeg_set_defaults   ( &jcs );
	jpeg_set_quality    ( &jcs, n_jpg_quality, TRUE );


	jpeg_start_compress ( &jcs, TRUE );

	u8         *row_ptr    = (void*) N_BMP_PTR( &bmp );
	JSAMPROW    row_pointer[ 1 ];
	n_type_int  row_stride = n_bmp_linebyte( bmpsx, 24 );

	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		row_pointer[ 0 ] = &row_ptr[ row_stride * ( bmpsy - 1 - y ) ];
		jpeg_write_scanlines( &jcs, row_pointer, 1 );

		y++;
		if ( y >= bmpsy ) { break; }
	}


#ifdef N_JPG_NO_MEM_API

	jpeg_finish_compress ( &jcs );
	jpeg_destroy_compress( &jcs );

	n_posix_fclose( fp );

#else  // #ifdef N_JPG_NO_MEM_API

	jpeg_finish_compress ( &jcs );

	FILE *fp = n_posix_fopen_write( jpgname );

	if ( fp != NULL )
	{
		n_posix_fwrite( ptr, byte, 1, fp ); 
	}

	n_posix_fclose( fp );

	jpeg_destroy_compress( &jcs );

#endif // #ifdef N_JPG_NO_MEM_API


	n_bmp_free( &bmp );


	return FALSE;
}

BOOL
n_jpg_jpg2bmp( const n_posix_char *jpgname, n_bmp *bmp_ret )
{

	if ( n_string_is_empty( jpgname ) ) { return TRUE; }

	if ( bmp_ret == NULL ) { return TRUE; }


	// [!] : sniffer

	FILE *fp = NULL;

	{

		u8 sniffer_jpg[] = { 0xff, 0xd8, 0xff };
		n_posix_structstat_size_t sniffer_byte = 3;

		if ( sniffer_byte > n_posix_stat_size( jpgname ) )
		{
			return TRUE;
		}

		fp = n_posix_fopen_read( jpgname );
		if ( fp == NULL )
		{
			return TRUE;
		}

		u8 sniffer_ptr[ 3 ];
		n_posix_fread( sniffer_ptr, sniffer_byte, 1, fp );
	
		if ( FALSE == n_memory_is_same( sniffer_ptr, sniffer_jpg, sniffer_byte ) )
		{
			n_posix_fclose( fp );

			return TRUE;
		}

		// [Needed] : rewind

		n_posix_fseek( fp, 0, SEEK_SET );

	}


	struct jpeg_decompress_struct jds;
	struct jpeg_error_mgr         jem;

	jds.err = jpeg_std_error( &jem );
	jpeg_create_decompress( &jds );


#ifdef N_JPG_NO_MEM_API

	jpeg_stdio_src        ( &jds, fp );

#else  // #ifdef N_JPG_NO_MEM_API

	n_type_int  byte = n_posix_stat_size( jpgname );
	u8         *ptr  = n_memory_new( byte );

	n_posix_fread( ptr, byte, 1, fp );

	jpeg_mem_src          ( &jds, ptr, (u32) byte );

#endif // #ifdef N_JPG_NO_MEM_API


	jpeg_read_header      ( &jds, TRUE );
	jpeg_start_decompress ( &jds );
//n_posix_debug_literal( " %d ", jds.output_components );
	if (
		( jds.output_components != 1 )
		&&
		( jds.output_components != 3 )
	)
	{
		jpeg_destroy_decompress( &jds );

#ifdef N_JPG_NO_MEM_API

		//

#else  // #ifdef N_JPG_NO_MEM_API

		n_memory_free( ptr );

#endif // #ifdef N_JPG_NO_MEM_API

		n_posix_fclose( fp );

		return TRUE;
	}


	n_type_gfx bmpsx = jds.image_width;
	n_type_gfx bmpsy = jds.image_height;

	n_bmp_new_fast( bmp_ret, bmpsx, bmpsy );


	u8      *row_ptr    = (void*) N_BMP_PTR( bmp_ret );
	JSAMPROW row_pointer[ 1 ];

	if ( jds.output_components == 1 )
	{

		n_type_int  linebyte = n_bmp_linebyte( bmpsx, 32 );
		n_type_int  scanline = ( bmpsy - 1 ) * linebyte;
		u8         *row_gray = n_memory_new_closed( linebyte );

		row_pointer[ 0 ] = row_gray;

		n_type_gfx y = 0;
		n_posix_loop
		{

			jpeg_read_scanlines( &jds, row_pointer, 1 );

			n_type_gfx x  = 0;
			n_type_gfx xx = 0;
			n_posix_loop
			{

				row_ptr[ scanline + xx ] = row_gray[ x ]; xx++;
				row_ptr[ scanline + xx ] = row_gray[ x ]; xx++;
				row_ptr[ scanline + xx ] = row_gray[ x ]; xx++;
				row_ptr[ scanline + xx ] = N_BMP_ALPHA_CHANNEL_VISIBLE; xx++;

				x++;
				if ( x >= bmpsx ) { break; }
			}

			y++;
			if ( y >= bmpsy ) { break; }

			scanline -= linebyte;
		}

		n_memory_free_closed( row_gray );

	} else
	if ( jds.output_components == 3 )
	{

		n_type_int row_stride = n_bmp_linebyte( bmpsx, 24 );

		n_type_gfx y = 0;
		n_posix_loop
		{

			row_pointer[ 0 ] = &row_ptr[ row_stride * ( bmpsy - 1 - y ) ];
			jpeg_read_scanlines( &jds, row_pointer, 1 );

			y++;
			if ( y >= bmpsy ) { break; }
		}


		N_BMP_DEPTH  ( bmp_ret ) = 24;
		n_bmp_32bit  ( bmp_ret );
		n_jpg_swaprgb( bmp_ret );

	}

	jpeg_finish_decompress ( &jds );
	jpeg_destroy_decompress( &jds );


#ifdef N_JPG_NO_MEM_API

	//

#else  // #ifdef N_JPG_NO_MEM_API

	n_memory_free( ptr );

#endif // #ifdef N_JPG_NO_MEM_API


	n_posix_fclose( fp );


	return FALSE;
}




#endif // _H_NONNON_NEUTRAL_JPG

