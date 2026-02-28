// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP
#define _H_NONNON_NEUTRAL_BMP




#include "./posix.c"
#include "./thread.c"
#include "./type.c"

#include <string.h>




// getset macro

#define N_BMP_FILEH(    bmp ) ( ( bmp )->fileh )
#define N_BMP_INFOH(    bmp ) ( ( bmp )->infoh )
#define N_BMP_PTR_PAL(  bmp ) ( ( bmp )->palette )
#define N_BMP_PTR(      bmp ) ( ( bmp )->ptr )
#define N_BMP_SIZE(     bmp ) ( ( bmp )->size )
#define N_BMP_LINEBYTE( bmp ) ( ( bmp )->linebyte )
#define N_BMP_BYTELEN(  bmp ) ( ( bmp )->bytelen )

#define N_BMP_TYPE(     bmp ) ( ( bmp )->fileh.bfType )
#define N_BMP_FILESIZE( bmp ) ( ( bmp )->fileh.bfSize )
#define N_BMP_OFFBITS(  bmp ) ( ( bmp )->fileh.bfOffBits )

#define N_BMP_CBSIZE(   bmp ) ( ( bmp )->infoh.biSize )
#define N_BMP_SX(       bmp ) ( ( bmp )->infoh.biWidth )
#define N_BMP_SY(       bmp ) ( ( bmp )->infoh.biHeight )
#define N_BMP_PLANE(    bmp ) ( ( bmp )->infoh.biPlanes )
#define N_BMP_DEPTH(    bmp ) ( ( bmp )->infoh.biBitCount )
#define N_BMP_RLE(      bmp ) ( ( bmp )->infoh.biCompression )
#define N_BMP_ORIGSIZE( bmp ) ( ( bmp )->infoh.biSizeImage )
#define N_BMP_XRES(     bmp ) ( ( bmp )->infoh.biXPelsPerMeter )
#define N_BMP_YRES(     bmp ) ( ( bmp )->infoh.biYPelsPerMeter )
#define N_BMP_PAL(      bmp ) ( ( bmp )->infoh.biClrUsed )
#define N_BMP_KEYPAL(   bmp ) ( ( bmp )->infoh.biClrImportant )


// constant

#define N_BMP_TYPE_BM        0x4d42
#define N_BMP_SIZE_FILEH     sizeof( BITMAPFILEHEADER )
#define N_BMP_SIZE_INFOH     sizeof( BITMAPINFOHEADER )
#define N_BMP_SIZE_HEADER    ( N_BMP_SIZE_FILEH + N_BMP_SIZE_INFOH )


// internal

#define N_BMP_U32( n ) ( (u32) ( n ) & 0x000000ff )




#ifndef N_POSIX_PLATFORM_WINDOWS


#pragma pack( push, 2 )

typedef struct {

	u16 bfType;
	u32 bfSize;
	u16 bfReserved1;
	u16 bfReserved2;
	u32 bfOffBits;

} BITMAPFILEHEADER;

#pragma pack( pop )

typedef struct {

	u32 biSize;
	s32 biWidth;
	s32 biHeight;
	u16 biPlanes;
	u16 biBitCount;
	u32 biCompression;
	u32 biSizeImage;
	s32 biXPelsPerMeter;
	s32 biYPelsPerMeter;
	u32 biClrUsed;
	u32 biClrImportant;

} BITMAPINFOHEADER;


// [!] : this is not used now

#define RGB( r,g,b ) ( ( N_BMP_U32( r ) << 0 ) + ( N_BMP_U32( g ) << 8 ) + ( N_BMP_U32( b ) << 16 ) + ( 0 << 24 ) )


#define BI_RGB       0
#define BI_RLE8      1
#define BI_RLE4      2
#define BI_BITFIELDS 3


#endif // #ifndef N_POSIX_PLATFORM_WINDOWS




// [!] : Alpha Value Format
//
//	  0 : full transparent
//	255 : no transparent
//
//	Vista or later : this format is used in Desktop Window Manager

#define N_BMP_ALPHA_MODE_STANDARD
#define N_BMP_ALPHA_CHANNEL_VISIBLE   ( 255 )
#define N_BMP_ALPHA_CHANNEL_INVISIBLE (   0 )
#define N_BMP_ALPHA_RATIO_VISIBLE     ( 1.0 )
#define N_BMP_ALPHA_RATIO_INVISIBLE   ( 0.0 )

/*
// [!] : Nonnon Original Mix-R-G-B Format : far easier to implement than ARGB

#define N_BMP_ALPHA_CHANNEL_VISIBLE   (   0 )
#define N_BMP_ALPHA_CHANNEL_INVISIBLE ( 255 )
#define N_BMP_ALPHA_RATIO_VISIBLE     ( 0.0 )
#define N_BMP_ALPHA_RATIO_INVISIBLE   ( 1.0 )
*/


// [!] : you cannot use RGB() macro with 32bit bitmaps

#define n_bmp_argb( a,r,g,b ) ( ( N_BMP_U32( b ) << 0 ) + ( N_BMP_U32( g ) << 8 ) + ( N_BMP_U32( r ) << 16 ) + ( N_BMP_U32( a ) << 24 ) )
#define n_bmp_rgb(    r,g,b ) n_bmp_argb( N_BMP_ALPHA_CHANNEL_VISIBLE, r,g,b )


// [!] : this is faster than below code

#define n_bmp_a( color ) ( ( color >> 24 ) & 0x000000ff )
#define n_bmp_r( color ) ( ( color >> 16 ) & 0x000000ff )
#define n_bmp_g( color ) ( ( color >>  8 ) & 0x000000ff )
#define n_bmp_b( color ) ( ( color       ) & 0x000000ff )
/*
#define n_bmp_a( color ) ( (u8) ( color >> 24 ) )
#define n_bmp_r( color ) ( (u8) ( color >> 16 ) )
#define n_bmp_g( color ) ( (u8) ( color >>  8 ) )
#define n_bmp_b( color ) ( (u8) ( color       ) )
*/


#define n_bmp_rgb_visible(   r,g,b ) n_bmp_argb( N_BMP_ALPHA_CHANNEL_VISIBLE,   r,g,b )
#define n_bmp_rgb_invisible( r,g,b ) n_bmp_argb( N_BMP_ALPHA_CHANNEL_INVISIBLE, r,g,b )


// [!] : this is faster than below code

#ifdef N_BMP_ALPHA_MODE_STANDARD

#define n_bmp_white           0xffffffff
#define n_bmp_black           0xff000000

#define n_bmp_white_invisible 0x00ffffff
#define n_bmp_black_invisible 0x00000000

#else  // #ifdef N_BMP_ALPHA_MODE_STANDARD

#define n_bmp_white           0x00ffffff
#define n_bmp_black           0x00000000

#define n_bmp_white_invisible 0xffffffff
#define n_bmp_black_invisible 0xff000000

#endif // #ifdef N_BMP_ALPHA_MODE_STANDARD

/*
#define n_bmp_white n_bmp_rgb( 255,255,255 )
#define n_bmp_black n_bmp_rgb(   0,  0,  0 )

#define n_bmp_white_invisible n_bmp_rgb_invisible( 255,255,255 )
#define n_bmp_black_invisible n_bmp_rgb_invisible(   0,  0,  0 )
*/



typedef struct {

	BITMAPFILEHEADER fileh;
	BITMAPINFOHEADER infoh;

	u32 *palette;
	u32 *ptr;

	// [!] : cache for speeding-up

	n_type_gfx size;
	n_type_gfx linebyte;
	n_type_gfx bytelen;

	// [!] : for multi-thread

	BOOL transparent_onoff;

} n_bmp;




static BOOL n_bmp_safemode_base = TRUE;
static BOOL n_bmp_safemode      = TRUE;


static BOOL n_bmp_transparent_onoff_default = TRUE;


static BOOL n_bmp_flip_onoff = FALSE;





// [!] : performance : Core i5 4300U @ 1.90GHz : 2-core 4-thread
//
//	[ OrangeCat : Home : a cats folder ]
//
//	OFF		212 1292
//	0 : 1 core	213 1345
//	0 : 2 cores	406 1361
//	0 : 4 cores	527 1451
//	1000		521 1375
//	10000		303  960
//	100000		207  779
//	1000000		210  684
//	10000000	215  542
//	100000000	216  653

//#define N_BMP_MULTITHREAD_DEBUG

#define N_BMP_MULTITHREAD_GRANULARITY ( 10000000 )


static BOOL n_bmp_is_multithread = FALSE;




#define N_BMP_LIMIT_SIZE SHRT_MAX

BOOL
n_bmp_size_is_overflow( n_type_index size )
{

	n_type_index base  = N_BMP_LIMIT_SIZE;
	n_type_index limit = base * base;

	if ( size > limit ) { return TRUE; }

	return FALSE;
}

BOOL
n_bmp_is_overflow( n_type_gfx sx, n_type_gfx sy )
{

	n_type_index size = sx * sy;

	return n_bmp_size_is_overflow( size );
}




// internal
n_type_gfx
n_bmp_realbyte( n_type_gfx sx, n_type_gfx depth )
{

	// [!] : don't use n_bmp structure


	n_type_gfx tmp = 0;


	if ( depth ==  1 )
	{
		if ( sx < 8 ) { tmp = 1; } else { tmp = ( sx / 8 ) + ( 0 != ( sx % 8 ) ); }
	} else
	if ( depth ==  4 )
	{
		if ( sx < 2 ) { tmp = 1; } else { tmp = ( sx / 2 ) + ( 0 != ( sx % 2 ) ); }
	} else
	if ( depth ==  8 ) { tmp = ( sx * 1 ); } else
	if ( depth == 24 ) { tmp = ( sx * 3 ); } else
	if ( depth == 32 ) { tmp = ( sx * 4 ); }


	return tmp;
}

// internal
n_type_gfx
n_bmp_skipbyte( n_type_gfx sx, n_type_gfx depth )
{

	// [!] : don't use n_bmp structure


	// [!] : not necessary in 32bit bitmap

	if ( depth == 32 ) { return 0; }


	// [!] : the return value is among 0 to 3

	n_type_gfx tmp = n_bmp_realbyte( sx, depth );
	if ( tmp < 4 ) { return ( 4 - tmp ); }


	n_type_gfx ret = 4 - ( tmp % 4 );
	if ( ret == 4 ) { return 0; }


	return ret;
}

n_type_gfx
n_bmp_linebyte( n_type_gfx sx, n_type_gfx depth )
{
	return n_bmp_realbyte( sx, depth ) + n_bmp_skipbyte( sx, depth );
}

n_type_gfx
n_bmp_size( n_type_gfx sx, n_type_gfx sy, n_type_gfx depth )
{
	return n_bmp_linebyte( sx, depth ) * sy;
}

// internal
void
n_bmp_precalc( n_bmp *bmp )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }
	}


	N_BMP_SIZE( bmp ) = n_bmp_size( N_BMP_SX( bmp ),N_BMP_SY( bmp ), N_BMP_DEPTH( bmp ) );


	N_BMP_LINEBYTE( bmp ) = n_bmp_linebyte( N_BMP_SX( bmp ), N_BMP_DEPTH( bmp ) );
	N_BMP_BYTELEN ( bmp ) = N_BMP_DEPTH( bmp ) / 8;
	if ( 0 == N_BMP_BYTELEN( bmp ) ) { N_BMP_BYTELEN( bmp ) = 1; }


	// [!] : compressed bitmap uses this member

	if ( BI_RGB == N_BMP_RLE( bmp ) ) { N_BMP_ORIGSIZE( bmp ) = 0; }


	// [!] : index color bitmap uses this member

	N_BMP_KEYPAL( bmp ) = 0;


	// these values are used in .NET Framework

	N_BMP_XRES( bmp ) = 0;
	N_BMP_YRES( bmp ) = 0;


	return;
}

void
n_bmp_header( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }
	}


	// [!] : parameter will be overwritten


	N_BMP_CBSIZE  ( bmp ) = N_BMP_SIZE_INFOH;
	N_BMP_SX      ( bmp ) = sx;
	N_BMP_SY      ( bmp ) = sy;
	N_BMP_PLANE   ( bmp ) =  1;
	N_BMP_DEPTH   ( bmp ) = 32;

	n_bmp_precalc ( bmp );

	N_BMP_TYPE    ( bmp ) = N_BMP_TYPE_BM;
	N_BMP_FILESIZE( bmp ) = N_BMP_SIZE_HEADER + (u32) N_BMP_SIZE( bmp );
	N_BMP_OFFBITS ( bmp ) = N_BMP_SIZE_HEADER;


	bmp->transparent_onoff = n_bmp_transparent_onoff_default;


	return;
}

n_posix_inline n_type_index
n_bmp_ptr_pos2index( const n_bmp *bmp, n_type_gfx x, n_type_gfx y )
{

	if ( n_bmp_flip_onoff )
	{
		return x + ( (n_type_index) N_BMP_SX( bmp ) * y );
	} else {
		return x + ( (n_type_index) N_BMP_SX( bmp ) * ( (n_type_index) N_BMP_SY( bmp ) - 1 - y ) );
	}

}

n_posix_inline void
n_bmp_ptr_get_fast( const n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 *color )
{

	// [!] : unsafe version : use carefully

 	(*color) = N_BMP_PTR( bmp )[ n_bmp_ptr_pos2index( bmp, x,y ) ];


	return;
}

n_posix_inline void
n_bmp_ptr_set_fast( const n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 color )
{

	// [!] : unsafe version : use carefully

	N_BMP_PTR( bmp )[ n_bmp_ptr_pos2index( bmp, x,y ) ] = color;


	return;
}

n_posix_inline BOOL
n_bmp_ptr_is_accessible( const n_bmp *bmp, n_type_gfx x, n_type_gfx y )
{

	// [!] : no error check for performance


	if (
		( x < 0 )||( x >= N_BMP_SX( bmp ) )
		||
		( y < 0 )||( y >= N_BMP_SY( bmp ) )
	)
	{
		return FALSE;
	}


	return TRUE;
}

BOOL
n_bmp_ptr_get( const n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 *color )
{

	if ( n_bmp_safemode_base )
	{
		if (   bmp == NULL ) { return TRUE; }
		if ( color == NULL ) { return TRUE; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return TRUE; }
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return TRUE; }
	}


	if ( n_bmp_ptr_is_accessible( bmp, x,y ) )
	{
		n_bmp_ptr_get_fast( bmp, x,y, color );
	} else {
		return TRUE;
	}


	return FALSE;
}

BOOL
n_bmp_ptr_set( n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 color )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return TRUE; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return TRUE; }
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return TRUE; }
	}


	if ( n_bmp_ptr_is_accessible( bmp, x,y ) )
	{
		n_bmp_ptr_set_fast( bmp, x,y, color );
	} else {
		return TRUE;
	}


	return FALSE;
}

#define N_BMP_ALPHA_TOOL_IS_ZERO      ( 0 )
#define N_BMP_ALPHA_TOOL_IS_VISIBLE   ( 1 )
#define N_BMP_ALPHA_TOOL_IS_INVISIBLE ( 2 )

#define n_bmp_alpha_is_zero(      b ) n_bmp_alpha_tool( b, N_BMP_ALPHA_TOOL_IS_ZERO      )
#define n_bmp_alpha_is_visible(   b ) n_bmp_alpha_tool( b, N_BMP_ALPHA_TOOL_IS_VISIBLE   )
#define n_bmp_alpha_is_invisible( b ) n_bmp_alpha_tool( b, N_BMP_ALPHA_TOOL_IS_INVISIBLE )
#define n_bmp_alpha_is_used(      b ) ( FALSE == n_bmp_alpha_is_visible( b ) )

BOOL
n_bmp_alpha_tool( n_bmp *bmp, int mode )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return FALSE; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return FALSE; }
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return FALSE; }
	}


	BOOL ret = FALSE;


	// [!] : 40% faster than using n_bmp_ptr_get_fast()

	n_type_index count = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_index i     = 0;


	if ( mode == N_BMP_ALPHA_TOOL_IS_ZERO )
	{

		n_posix_loop
		{

			u32 color = N_BMP_PTR( bmp )[ i ];

			int a = n_bmp_a( color );
			//int r = n_bmp_r( color );
			//int g = n_bmp_g( color );
			//int b = n_bmp_b( color );

			if ( a ) { break; }

			i++;
			if ( i >= count ) { ret = TRUE; break; }
		}

	} else
	if ( mode == N_BMP_ALPHA_TOOL_IS_VISIBLE )
	{

		n_posix_loop
		{

			u32 color = N_BMP_PTR( bmp )[ i ];

			int a = n_bmp_a( color );
			//int r = n_bmp_r( color );
			//int g = n_bmp_g( color );
			//int b = n_bmp_b( color );

			if ( a != N_BMP_ALPHA_CHANNEL_VISIBLE ) { break; }

			i++;
			if ( i >= count ) { ret = TRUE; break; }
		}

	} else
	if ( mode == N_BMP_ALPHA_TOOL_IS_INVISIBLE )
	{

		n_posix_loop
		{

			u32 color = N_BMP_PTR( bmp )[ i ];

			int a = n_bmp_a( color );
			//int r = n_bmp_r( color );
			//int g = n_bmp_g( color );
			//int b = n_bmp_b( color );

			if ( a != N_BMP_ALPHA_CHANNEL_INVISIBLE ) { break; }

			i++;
			if ( i >= count ) { ret = TRUE; break; }
		}

	}


	return ret;
}

#define N_BMP_ALPHA_PROCESS_REVERSE ( 0 )
#define N_BMP_ALPHA_PROCESS_VISIBLE ( 1 )

#define n_bmp_alpha_reverse( b ) n_bmp_alpha_process( b, N_BMP_ALPHA_PROCESS_REVERSE )
#define n_bmp_alpha_visible( b ) n_bmp_alpha_process( b, N_BMP_ALPHA_PROCESS_VISIBLE )

typedef struct {

	n_bmp        *bmp;
	n_type_index  count;
	int           mode;
	u32           oy, cores;

} n_bmp_alpha_process_thread_struct;

void
n_bmp_alpha_process_thread_main( n_bmp_alpha_process_thread_struct *p )
{

	n_type_gfx i = p->oy; if ( i >= p->count ) { return; }

	if ( p->mode == N_BMP_ALPHA_PROCESS_REVERSE )
	{

		n_posix_loop
		{

			u32 color = N_BMP_PTR( p->bmp )[ i ];

			int a = n_bmp_a( color );
			int r = n_bmp_r( color );
			int g = n_bmp_g( color );
			int b = n_bmp_b( color );

			N_BMP_PTR( p->bmp )[ i ] = n_bmp_argb( 255 - a, r, g, b );


			i += p->cores;
			if ( i >= p->count ) { break; }
		}

	} else
	if ( p->mode == N_BMP_ALPHA_PROCESS_VISIBLE )
	{

		n_posix_loop
		{

#ifdef N_BMP_ALPHA_MODE_STANDARD
			N_BMP_PTR( p->bmp )[ i ] = N_BMP_PTR( p->bmp )[ i ] | 0xff000000;
#else  // #ifdef N_BMP_ALPHA_MODE_STANDARD
			N_BMP_PTR( p->bmp )[ i ] = N_BMP_PTR( p->bmp )[ i ] & 0x00ffffff;
#endif // #ifdef N_BMP_ALPHA_MODE_STANDARD


			i += p->cores;
			if ( i >= p->count ) { break; }
		}

	}


	return;
}

n_thread_return
n_bmp_alpha_process_thread( n_thread_argument p )
{

	n_bmp_alpha_process_thread_main( (n_bmp_alpha_process_thread_struct*) p );

	return 0;
}

void
n_bmp_alpha_process( n_bmp *bmp, int mode )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return; }
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return; }
	}


	// [x] : Win9x : can run but not working

	n_type_index count = N_BMP_SX( bmp ) * N_BMP_SY( bmp );

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( count >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_alpha_process() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;


		u32 cores = n_thread_core_count;

		n_thread                          *h = (n_thread                         *) n_memory_new( cores * sizeof( n_thread                          ) );
		n_bmp_alpha_process_thread_struct *p = (n_bmp_alpha_process_thread_struct*) n_memory_new( cores * sizeof( n_bmp_alpha_process_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_alpha_process_thread_struct tmp = { bmp, count, mode, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_alpha_process_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_alpha_process_thread, &p[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_wait( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_exit( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}


		n_memory_free( h );
		n_memory_free( p );


		n_bmp_is_multithread = p_multithread;

	} else {

		n_bmp_alpha_process_thread_struct tmp = { bmp, count, mode, 0,1 };
		n_bmp_alpha_process_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	return;
}

typedef struct {

	n_bmp        *bmp;
	n_type_index  all;
	u32           color;
	u32           oy, cores;

} n_bmp_flush_thread_struct;

void
n_bmp_flush_thread_main( n_bmp_flush_thread_struct *p )
{

	n_type_index i = p->oy; if ( i >= p->all ) { return; }
	n_posix_loop
	{//break;

		N_BMP_PTR( p->bmp )[ i ] = p->color;

		i += p->cores;
		if ( i >= p->all ) { break; }
	}


	return;
}

n_thread_return
n_bmp_flush_thread( n_thread_argument p )
{

	n_bmp_flush_thread_main( (n_bmp_flush_thread_struct*) p );

	return 0;
}

void
n_bmp_flush( n_bmp *bmp, u32 color )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }

		if ( NULL == N_BMP_PTR( bmp ) ) { return; }
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return; }
	}


	// [!] : memset() is faster than n_bmp_flush() for single-byte patterns

	if ( color == 0x00000000 ) { memset( N_BMP_PTR( bmp ), 0x00, N_BMP_SIZE( bmp ) ); return; }
	if ( color == 0xffffffff ) { memset( N_BMP_PTR( bmp ), 0xff, N_BMP_SIZE( bmp ) ); return; }


	// [!] : this section is made by Copilot

	// On macOS, prefer memset_pattern4 which fills with a 4-byte pattern efficiently
#if defined(__APPLE__)
	{
		      void   *dst     = (void*) N_BMP_PTR( bmp );
		const void   *pattern = &color;
		      size_t  bytes   = (size_t) N_BMP_SIZE( bmp );

		// memset_pattern4 is optimized in libc on macOS
		extern void memset_pattern4( void * b, const void *pattern, size_t len );

		memset_pattern4( dst, pattern, bytes );

		return;
	}
#endif


	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( N_BMP_SX( bmp ) * N_BMP_SY( bmp ) ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_flush() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG

		// [!] : 2 cores, 4 threads : 40% faster than single-thread

		u32 cores = n_thread_core_count;

		n_thread                  *h = (n_thread                 *) n_memory_new( cores * sizeof( n_thread                  ) );
		n_bmp_flush_thread_struct *p = (n_bmp_flush_thread_struct*) n_memory_new( cores * sizeof( n_bmp_flush_thread_struct ) );


		n_type_gfx all = N_BMP_SX( bmp ) * N_BMP_SY( bmp );

		u32 i = 0;
		n_posix_loop
		{

			n_bmp_flush_thread_struct tmp = { bmp, all, color, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_flush_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_flush_thread, &p[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_wait( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_exit( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}


		n_memory_free( h );
		n_memory_free( p );

	} else {
/*
		n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
		n_type_index i = 0;
		n_posix_loop
		{

			N_BMP_PTR( bmp )[ i ] = color;

			i++;
			if ( i >= c ) { break; }
		}
*/

		// [!] : this section is made by Copilot

		// Single-threaded fast fill: use doubling via memcpy which is typically
		// faster than manual unrolling because it leverages optimized memcpy.
		n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );

		u32 *p = N_BMP_PTR( bmp );

		if ( c > 0 ) {
			// Initialize first element
			p[0] = color;

			// Use doubling: copy the initialized block repeatedly
			n_type_index filled = 1;
			while ( filled < c ) {
				n_type_index to_copy = (filled <= (c - filled)) ? filled : (c - filled);
				memcpy(&p[filled], p, to_copy * sizeof(u32));
				filled += to_copy;
			}
		}

	}


	return;
}

#define n_bmp_zero(  f    ) n_memory_zero( f,    sizeof( n_bmp ) )
#define n_bmp_alias( f, t ) n_memory_copy( f, t, sizeof( n_bmp ) )

#define n_bmp_free(      bmp ) n_bmp_free_internal( bmp, TRUE  )
#define n_bmp_free_fast( bmp ) n_bmp_free_internal( bmp, FALSE )

void
n_bmp_free_internal( n_bmp *bmp, BOOL zeroclear )
{

	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }
	}


	n_memory_free( N_BMP_PTR_PAL( bmp ) );
	n_memory_free( N_BMP_PTR    ( bmp ) );


	if ( zeroclear ) { n_bmp_zero( bmp ); }


	return;
}

void
n_bmp_carboncopy( const n_bmp *f, n_bmp *t )
{

	// [!] : parameter will be overwritten


	if ( n_bmp_safemode_base )
	{
		if ( f == NULL ) { return; }
		if ( t == NULL ) { return; }
	}


	if ( 0 >= N_BMP_SIZE( f ) ) { n_bmp_zero( t ); return; }


	n_bmp_alias( f, t );


	N_BMP_PTR( t ) = (u32*) n_memory_new( N_BMP_SIZE( f ) );
	n_memory_copy( N_BMP_PTR( f ), N_BMP_PTR( t ), N_BMP_SIZE( f ) );


	return;
}

void
n_bmp_replace( n_bmp *f, n_bmp *t )
{

	// [!] : parameter will be overwritten


	if ( n_bmp_safemode_base )
	{
		if ( f == NULL ) { return; }
		if ( t == NULL ) { return; }
	}


	n_bmp_free_fast( t );
	n_bmp_alias( f, t );


	return;
}

#define n_bmp_new(      bmp, sx, sy ) n_bmp_new_internal( bmp, sx, sy, TRUE , FALSE )
#define n_bmp_new_fast( bmp, sx, sy ) n_bmp_new_internal( bmp, sx, sy, FALSE, FALSE )

#define n_bmp_1st(      bmp, sx, sy ) n_bmp_new_internal( bmp, sx, sy, TRUE , TRUE )
#define n_bmp_1st_fast( bmp, sx, sy ) n_bmp_new_internal( bmp, sx, sy, FALSE, TRUE )

void
n_bmp_new_internal( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, BOOL zeroclear, BOOL is_first )
{

	// [Needed]
	//
	//	1 : clear memory at first time
	//	n_bmp_zero( &bmp );
	//
	//	2 : "static" may be needed


	if ( n_bmp_safemode_base )
	{
		if ( bmp == NULL ) { return; }
	}


	if ( is_first == FALSE ) { n_bmp_free( bmp ); }


	if ( n_bmp_is_overflow( sx,sy ) ) { return; }

	if ( ( sx <= 0 ) || ( sy <= 0 ) ) { return; }


	n_bmp_header( bmp, sx,sy );


	N_BMP_PTR( bmp ) = (u32*) n_memory_new( N_BMP_SIZE( bmp ) );

	if ( zeroclear )
	{
		//n_memory_zero( N_BMP_PTR( bmp ), N_BMP_SIZE( bmp ) );
		n_bmp_flush( bmp, n_bmp_argb( N_BMP_ALPHA_CHANNEL_VISIBLE, 0,0,0 ) );
	}


	return;
}

#define n_bmp_colorref2argb n_bmp_color_convert
#define n_bmp_argb2colorref n_bmp_color_convert

u32
n_bmp_color_convert( u32 color )
{

	// 24bit / RGB()   : 0BGR
	// 32bit / Palette : ARGB : BGRA on file

	int a = n_bmp_a( color );
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );


	return n_bmp_argb( a,b,g,r );
}


#endif // _H_NONNON_NEUTRAL_BMP

