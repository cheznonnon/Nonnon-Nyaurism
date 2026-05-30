// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : n_bmp_*_channel()
//
//	use "int" for performance
//	10% faster than "u8"


// [!] : multi-thread : don't use "static" cache : data will break




#ifndef _H_NONNON_NEUTRAL_BMP_COLOR
#define _H_NONNON_NEUTRAL_BMP_COLOR




#include <math.h>




#include "../bmp.c"
#include "../random.c"


#include "./_error.c"
#include "./_transparent.c"




//const n_type_real n_bmp_coeff_channel = 1.0 / 255.0;

#define n_bmp_coeff_channel ( 0.003921568627451 )




#include "./table/coeff.c"

//#include "./table/blend.c"
#include "./table/gamma.c"




#define n_bmp_clamp_channel( v ) n_posix_minmax( 0, 255, (int) v )
/*
n_posix_inline int
n_bmp_clamp_channel( int v )
{

	// [!] : 0.5% slower than above macro

	if ( v < 0 ) { v = 0; } else if ( v > 255 ) { v = 255; }


	return v;
}
*/




#define n_bmp_simplify_channel( v, n ) ( 255 - ( ( 255 - ( v ) ) / n * n ) )




u32
n_bmp_grayscale_pixel( u32 color )
{

	if ( color == 0x00000000 ) { return 0x00000000; }
	if ( color == 0xffffffff ) { return 0xffffffff; }


	// [!] : alpha is reserved

	int a = n_bmp_a( color );
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );

	r = g = b = ( r + g + b ) / 3;


	return n_bmp_argb( a,r,g,b );
}




n_type_real
n_bmp_blend_alpha2ratio( int alpha )
{

	// [!] : don't use n_posix_inline : cache will be independent (maybe)

	if ( n_bmp_is_multithread )
	{

		n_type_real a = n_bmp_table_coeff_channel( alpha );

#ifdef N_BMP_ALPHA_MODE_STANDARD
		n_type_real p_ret = ( N_BMP_ALPHA_RATIO_VISIBLE - a );
#else  // #ifdef N_BMP_ALPHA_MODE_STANDARD
		n_type_real p_ret = a;
#endif // #ifdef N_BMP_ALPHA_MODE_STANDARD


		return p_ret;

	} else {

		static int         p_alpha = 0;
		static n_type_real p_ret   = 0;

		if ( p_alpha == alpha ) { return p_ret; }

		n_type_real a = n_bmp_table_coeff_channel( alpha );

		p_alpha = alpha;

#ifdef N_BMP_ALPHA_MODE_STANDARD
		p_ret = ( N_BMP_ALPHA_RATIO_VISIBLE - a );
#else  // #ifdef N_BMP_ALPHA_MODE_STANDARD
		p_ret = a;
#endif // #ifdef N_BMP_ALPHA_MODE_STANDARD


		return p_ret;

	}

}

#ifdef _H_NONNON_NEUTRAL_BMP_TABLE_BLEND

#define n_bmp_blend_channel n_bmp_table_blend_channel

#else  // #ifdef _H_NONNON_NEUTRAL_BMP_TABLE_BLEND

void
n_bmp_table_blend_init( void )
{
	return;
}

void
n_bmp_table_blend_exit( void )
{
	return;
}

// [!] : Mechanism
//
//	when you use no_force_move(), "f" will not reach the value of "t"
//	when you use    force_move(), some artifact will occur but "f" will be "t" at the end

#define n_bmp_blend_channel n_bmp_blend_channel_no_force_move
#define n_bmp_blend_pixel   n_bmp_blend_pixel_no_force_move

n_posix_inline int
n_bmp_blend_channel_force_move( int f, int t, n_type_real blend )
{

	// [!] : n_posix_inline : bloat but faster


	//int ret = n_bmp_table_blend_channel( f, t, blend );

//if ( ( ret < 0 )||( ret > 255 ) ) { n_posix_debug_literal( "" ); }

	//return ret;


	// [!] : shortcut

	if ( f == t ) { return t; }

	if ( blend <= 0 ) { return f; }
	if ( blend >= 1 ) { return t; }


	if ( n_bmp_is_multithread )
	{

		// [!] : 20% faster than comment-out'ed code
		//
		//	cast to int is very important
		//
		//	logic :  5%
		//	cast  : 15%

		// [!] : don't use calculus in min()/max() when macro

		n_type_real d = ( f - t ) * blend;

		if ( f > t )
		{
			return f - (int) n_posix_max_n_type_real(  1.0, d );
		} else {
			return f - (int) n_posix_min_n_type_real( -1.0, d );
		}

/*
		n_type_real d = ( t - f ) * blend;

		if ( ( d > -1.0 )&&( d < 1.0 ) )
		{
			if ( f < t ) { d = 1; } else { d = -1; }
		}

		return f + d;
*/
	} else {

		// [!] : 1% faster

		static int         p_v = 0;
		static int         p_f = 0;
		static int         p_t = 0;
		static n_type_real p_d = 0;

		if ( ( p_f == f )&&( p_t == t )&&( p_d == blend ) ) { return p_v; }

		p_f = f;
		p_t = t;
		p_d = blend;

		n_type_real d = ( f - t ) * blend;

		if ( f > t )
		{
			p_v = f - (int) n_posix_max_n_type_real(  1.0, d );
		} else {
			p_v = f - (int) n_posix_min_n_type_real( -1.0, d );
		}

		return p_v;
	}

}

n_posix_inline int
n_bmp_blend_channel_no_force_move( int f, int t, n_type_real blend )
{

	// [!] : n_posix_inline : bloat but faster


	//int ret = n_bmp_table_blend_channel( f, t, blend );

//if ( ( ret < 0 )||( ret > 255 ) ) { n_posix_debug_literal( "" ); }

	//return ret;


	// [!] : shortcut

	if ( f == t ) { return t; }

	if ( blend <= 0 ) { return f; }
	if ( blend >= 1 ) { return t; }

/*
#ifdef N_POSIX_PLATFORM_MAC

	// [!] : Win32 : not inspected
	if ( isnan( blend ) )
	{
//free( n_random_range( INT_MAX ) ); // [!] : break-point crasher
		return f;
	}

#endif // #ifdef N_POSIX_PLATFORM_MAC
*/

	if ( n_bmp_is_multithread )
	{

		n_type_real d = ( f - t ) * blend;

		return f - ( (int) d );

	} else {

		// [!] : 1% faster

		static int         p_v = 0;
		static int         p_f = 0;
		static int         p_t = 0;
		static n_type_real p_d = 0;

		if ( ( p_f == f )&&( p_t == t )&&( p_d == blend ) ) { return p_v; }

		p_f = f;
		p_t = t;
		p_d = blend;

		n_type_real d = ( f - t ) * blend;

		p_v = f - ( (int) d );

		return p_v;
	}

}

#endif // #ifdef _H_NONNON_NEUTRAL_BMP_TABLE_BLEND

u32
n_bmp_blend_pixel_no_force_move( u32 f, u32 t, n_type_real blend )
{

	// [!] : 10% faster

	if ( f == t ) { return f; }

	if ( blend <= 0.0 ) { return f; }
	if ( blend >= 1.0 ) { return t; }


	// [!] : cache : 5% faster but multithread unsafe

	if ( n_bmp_is_multithread )
	{

		return n_bmp_argb
		(
			n_bmp_blend_channel_no_force_move( n_bmp_a( f ), n_bmp_a( t ), blend ),
			n_bmp_blend_channel_no_force_move( n_bmp_r( f ), n_bmp_r( t ), blend ),
			n_bmp_blend_channel_no_force_move( n_bmp_g( f ), n_bmp_g( t ), blend ),
			n_bmp_blend_channel_no_force_move( n_bmp_b( f ), n_bmp_b( t ), blend )
		);

	}


	static u32 p_color = 0;

	{

		static u32         p_f = 0;
		static u32         p_t = 0;
		static n_type_real p_d = 0;

		if ( ( p_f == f )&&( p_t == t )&&( p_d == blend ) ) { return p_color; }

		p_f = f;
		p_t = t;
		p_d = blend;

	}


	int f_a = n_bmp_a( f );
	int f_r = n_bmp_r( f );
	int f_g = n_bmp_g( f );
	int f_b = n_bmp_b( f );
	int t_a = n_bmp_a( t );
	int t_r = n_bmp_r( t );
	int t_g = n_bmp_g( t );
	int t_b = n_bmp_b( t );


	int a,r,g,b;


	a = n_bmp_blend_channel_no_force_move( f_a, t_a, blend );

	if ( ( f_r == f_a )&&( t_r == t_a ) )
	{
		r = a;
	} else
	{
		r = n_bmp_blend_channel_no_force_move( f_r, t_r, blend );
	}

	if ( ( f_g == f_a )&&( t_g == t_a ) )
	{
		g = a;
	} else
	if ( ( f_g == f_r )&&( t_g == t_r ) )
	{
		g = r;
	} else
	{
		g = n_bmp_blend_channel_no_force_move( f_g, t_g, blend );
	}

	if ( ( f_b == f_a )&&( t_b == t_a ) )
	{
		b = a;
	} else
	if ( ( f_b == f_r )&&( t_b == t_r ) )
	{
		b = r;
	} else
	if ( ( f_b == f_g )&&( t_b == t_g ) )
	{
		b = g;
	} else
	{
		b = n_bmp_blend_channel_no_force_move( f_b, t_b, blend );
	}


	p_color = n_bmp_argb( a,r,g,b );


	return p_color;
}

u32
n_bmp_blend_pixel_force_move( u32 f, u32 t, n_type_real blend )
{

	// [!] : 10% faster

	if ( f == t ) { return f; }

	if ( blend <= 0.0 ) { return f; }
	if ( blend >= 1.0 ) { return t; }


	// [!] : cache : 5% faster but multithread unsafe

	if ( n_bmp_is_multithread )
	{

		return n_bmp_argb
		(
			n_bmp_blend_channel_force_move( n_bmp_a( f ), n_bmp_a( t ), blend ),
			n_bmp_blend_channel_force_move( n_bmp_r( f ), n_bmp_r( t ), blend ),
			n_bmp_blend_channel_force_move( n_bmp_g( f ), n_bmp_g( t ), blend ),
			n_bmp_blend_channel_force_move( n_bmp_b( f ), n_bmp_b( t ), blend )
		);

	}


	static u32 p_color = 0;

	{

		static u32         p_f = 0;
		static u32         p_t = 0;
		static n_type_real p_d = 0;

		if ( ( p_f == f )&&( p_t == t )&&( p_d == blend ) ) { return p_color; }

		p_f = f;
		p_t = t;
		p_d = blend;

	}


	int f_a = n_bmp_a( f );
	int f_r = n_bmp_r( f );
	int f_g = n_bmp_g( f );
	int f_b = n_bmp_b( f );
	int t_a = n_bmp_a( t );
	int t_r = n_bmp_r( t );
	int t_g = n_bmp_g( t );
	int t_b = n_bmp_b( t );


	int a,r,g,b;


	a = n_bmp_blend_channel_force_move( f_a, t_a, blend );

	if ( ( f_r == f_a )&&( t_r == t_a ) )
	{
		r = a;
	} else
	{
		r = n_bmp_blend_channel_force_move( f_r, t_r, blend );
	}

	if ( ( f_g == f_a )&&( t_g == t_a ) )
	{
		g = a;
	} else
	if ( ( f_g == f_r )&&( t_g == t_r ) )
	{
		g = r;
	} else
	{
		g = n_bmp_blend_channel_force_move( f_g, t_g, blend );
	}

	if ( ( f_b == f_a )&&( t_b == t_a ) )
	{
		b = a;
	} else
	if ( ( f_b == f_r )&&( t_b == t_r ) )
	{
		b = r;
	} else
	if ( ( f_b == f_g )&&( t_b == t_g ) )
	{
		b = g;
	} else
	{
		b = n_bmp_blend_channel_force_move( f_b, t_b, blend );
	}


	p_color = n_bmp_argb( a,r,g,b );


	return p_color;
}




#define N_BMP_GRADIENT_RECTANGLE  0
#define N_BMP_GRADIENT_CIRCLE     1

void
n_bmp_gradient_prepare_s32( n_type_gfx *x, n_type_gfx *y, n_type_gfx *r, n_type_real *p, int mode )
{

	// [Patch] : smaller circle will be drawn

	(*r)++;


	// [!] : "int" is 40% faster than "n_type_real"


	// [!] : pow( n, 2 ) is too much heavy

	(*x) *= (*x);
	(*y) *= (*y);
	(*r) *= (*r);


	if ( mode == N_BMP_GRADIENT_CIRCLE )
	{
		(*p) = (*x) + (*y);
	} else {
		if ( (*x) > (*y) ) { (*p) = (*x); } else { (*p) = (*y); }
	}

	if ( (*p) > (*r) ) { (*p) = (*r); }


	return;
}

n_posix_inline int
n_bmp_gradient_channel_s32( int f, int t, n_type_gfx x, n_type_gfx y, n_type_gfx r, n_type_real p )
{

	// [Mechanism]
	//
	//	from : thick : center
	//	  to : thin  : outside
	//
	//	x==0 :   vertical gradient
	//	y==0 : horizontal gradient


	// [!] : double is slower than n_type_gfx/int
	//
	//	but you cannot make an image with 3000px or more


	if ( r <= 0 ) { return f; }
	if ( f == t ) { return t; }


	// [!] : this is 2x slower than below code
	//return n_bmp_blend_channel( f, t, (n_type_real) p / r );

	n_type_real line_1 = f * ( r - p );
	n_type_real line_2 = t * (     p );


	return (int) ( ( line_1 + line_2 ) / r );
}

n_posix_inline u32
n_bmp_gradient_pixel_s32( u32 f,u32 t, n_type_gfx x,n_type_gfx y,n_type_gfx r, int mode )
{

	n_type_real p = 0.0;

	n_bmp_gradient_prepare_s32( &x, &y, &r, &p, mode );

	return n_bmp_argb
	(
		n_bmp_gradient_channel_s32( n_bmp_a( f ), n_bmp_a( t ), x,y,r,p ),
		n_bmp_gradient_channel_s32( n_bmp_r( f ), n_bmp_r( t ), x,y,r,p ),
		n_bmp_gradient_channel_s32( n_bmp_g( f ), n_bmp_g( t ), x,y,r,p ),
		n_bmp_gradient_channel_s32( n_bmp_b( f ), n_bmp_b( t ), x,y,r,p )
	);
}

void
n_bmp_gradient_prepare_real( n_type_real *x, n_type_real *y, n_type_real *r, n_type_real *p, int mode )
{

	// [Patch] : smaller circle will be drawn

	(*r)++;


	// [!] : "int" is 40% faster than "n_type_real"


	// [!] : pow( n, 2 ) is too much heavy

	(*x) *= (*x);
	(*y) *= (*y);
	(*r) *= (*r);


	if ( mode == N_BMP_GRADIENT_CIRCLE )
	{
		(*p) = (*x) + (*y);
	} else {
		if ( (*x) > (*y) ) { (*p) = (*x); } else { (*p) = (*y); }
	}

	if ( (*p) > (*r) ) { (*p) = (*r); }


	return;
}

n_posix_inline int
n_bmp_gradient_channel_real( int f, int t, n_type_real x, n_type_real y, n_type_real r, n_type_real p )
{

	// [Mechanism]
	//
	//	from : thick : center
	//	  to : thin  : outside
	//
	//	x==0 :   vertical gradient
	//	y==0 : horizontal gradient


	// [!] : n_type_real is slower than n_type_gfx/int
	//
	//	but you cannot make an image with 3000px or more


	if ( r <= 0 ) { return f; }
	if ( f == t ) { return t; }


	// [!] : this is 2x slower than below code
	//return n_bmp_blend_channel( f, t, (n_type_real) p / r );

	n_type_real line_1 = f * ( r - p );
	n_type_real line_2 = t * (     p );


	return (int) ( ( line_1 + line_2 ) / r );
}

n_posix_inline u32
n_bmp_gradient_pixel_real( u32 f,u32 t, n_type_gfx x,n_type_gfx y,n_type_gfx r, int mode )
{

	n_type_real p = 0.0;

	n_bmp_gradient_prepare_s32( &x, &y, &r, &p, mode );

	return n_bmp_argb
	(
		n_bmp_gradient_channel_real( n_bmp_a( f ), n_bmp_a( t ), x,y,r,p ),
		n_bmp_gradient_channel_real( n_bmp_r( f ), n_bmp_r( t ), x,y,r,p ),
		n_bmp_gradient_channel_real( n_bmp_g( f ), n_bmp_g( t ), x,y,r,p ),
		n_bmp_gradient_channel_real( n_bmp_b( f ), n_bmp_b( t ), x,y,r,p )
	);
}



/*
int
n_bmp_contrast_channel( int ch, int param )
{

	if ( ch < (       param ) ) { return   0; } else
	if ( ch > ( 255 - param ) ) { return 255; }


	n_type_real d0 = ch - param;
	n_type_real d1 = (n_type_real) 256 / ( (n_type_real) 256 - ( param * 2 ) );


	return d0 * d1;
}

u32
n_bmp_contrast_pixel( u32 color, int param )
{

	int a =                         n_bmp_a( color )         ;
	int r = n_bmp_contrast_channel( n_bmp_r( color ), param );
	int g = n_bmp_contrast_channel( n_bmp_g( color ), param );
	int b = n_bmp_contrast_channel( n_bmp_b( color ), param );

	return n_bmp_argb( a,r,g,b );
}
*/
u32
n_bmp_contrast_pixel( u32 color, int param )
{

	int a = n_bmp_a( color );
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );


	n_type_real factor = ( 259.0 * ( param + 255.0 ) ) / ( 255.0 * ( 259.0 - param ) );

	r = n_posix_minmax( 0, 255, (int) ( ( factor * ( r - 128 ) ) + 128 ) );
	g = n_posix_minmax( 0, 255, (int) ( ( factor * ( g - 128 ) ) + 128 ) );
	b = n_posix_minmax( 0, 255, (int) ( ( factor * ( b - 128 ) ) + 128 ) );


	return n_bmp_argb( a,r,g,b );
}




#ifdef N_BMP_ALPHA_MODE_STANDARD

#define n_bmp_alpha_visible_pixel(   color ) ( ( color ) | 0xff000000 )
#define n_bmp_alpha_invisible_pixel( color ) ( ( color ) & 0x00ffffff )

#else  // #ifdef N_BMP_ALPHA_MODE_STANDARD

#define n_bmp_alpha_visible_pixel(   color ) ( ( color ) & 0x00ffffff )
#define n_bmp_alpha_invisible_pixel( color ) ( ( color ) | 0xff000000 )

#endif // #ifdef N_BMP_ALPHA_MODE_STANDARD

u32
n_bmp_alpha_replace_pixel( u32 color, int alpha )
{
	// [!] : Copilot Optimization
/*
	int a = alpha;
	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );

	return n_bmp_argb( a,r,g,b );
*/
	return ( 0x00ffffff & color ) | ( alpha << 24 );
}




u32
n_bmp_div_pixel( int a, int r, int g, int b, int n )
{

	if ( n <= 1 ) { return n_bmp_argb( a,r,g,b ); }


	// [!] : division is too slow : 20% faster

	if ( a != 0 ) { a /= n; }

	if ( r == g )
	{

		if ( r == b )
		{
			if ( r != 0 ) { r = g = b = r / n; }
		} else {
			if ( r != 0 ) { r = g = r / n; }
			if ( b != 0 ) { b /= n; }
		}

	} else
	if ( r == b )
	{

		if ( r != 0 ) { r = b = r / n; }
		if ( g != 0 ) { g /= n; }

	} else {

		if ( r != 0 ) { r /= n; }
		if ( g != 0 ) { g /= n; }
		if ( b != 0 ) { b /= n; }

	}


	return n_bmp_argb( a,r,g,b );
}




u32
n_bmp_antialias_pixel( const n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_real blend )
{

	// [!] : Thx : DeepSeek AI : Gaussian Blur


	u32 center;

	if (
		( n_bmp_ptr_get( bmp, x,y, &center ) )
		||
		( n_bmp_is_trans( bmp, center ) )
	)
	{
		return n_bmp_trans;
	}


	// [!] : nothing to do

	if ( blend <= 0.0 ) { return center; }


	const int sum = 16;

	const int kernel[] = {

		1, 2, 1,
		2, 4, 2,
		1, 2, 1,

	};

	int a = 0;
	int r = 0;
	int g = 0;
	int b = 0;

	n_type_gfx tx = -1;
	n_type_gfx ty = -1;
	n_posix_loop
	{

		u32 c;
		if ( n_bmp_ptr_get( bmp, x + tx, y + ty, &c ) )
		{
			c = center;
		}

		int pos = ( tx + 1 ) + ( ( ty + 1 ) * 3 );

		a += n_bmp_a( c ) * kernel[ pos ];
		r += n_bmp_r( c ) * kernel[ pos ];
		g += n_bmp_g( c ) * kernel[ pos ];
		b += n_bmp_b( c ) * kernel[ pos ];

		tx++;
		if ( tx > 1 )
		{
			tx = -1;

			ty++;
			if ( ty > 1 ) { break; }
		}
	}


	a = a / sum;
	r = r / sum;
	g = g / sum;
	b = b / sum;

	a = n_bmp_clamp_channel( a );
	r = n_bmp_clamp_channel( r );
	g = n_bmp_clamp_channel( g );
	b = n_bmp_clamp_channel( b );


	return n_bmp_blend_pixel( center, n_bmp_argb( a,r,g,b ), blend );
}




u32
n_bmp_blur_pixel( const n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx size, int weight, n_type_real blend )
{

	// [!] : n_bmp_blend_pixel() is heavy


	u32 center;

	if (
		( n_bmp_ptr_get( bmp, x,y, &center ) )
		||
		( n_bmp_is_trans( bmp, center ) )
	)
	{
		return n_bmp_trans;
	}


	// [!] : nothing to do

	if ( blend <= 0.0 ) { return center; }

	if ( size <= 1 ) { return center; }


	int avr = 1 + weight;

	int a = n_bmp_a( center ) * avr;
	int r = n_bmp_r( center ) * avr;
	int g = n_bmp_g( center ) * avr;
	int b = n_bmp_b( center ) * avr;


	n_type_gfx half = size / 2;


	n_type_gfx tx = -half;
	n_type_gfx ty = -half;
	n_posix_loop
	{

		// [!] : exclude a center pixel : 10% faster

		u32 color;

		if (
			( ( tx != 0 )&&( ty != 0 ) )
			&&
			( FALSE == n_bmp_ptr_get( bmp, x + tx, y + ty, &color ) )
			&&
			( FALSE == n_bmp_is_trans( bmp, color ) )
		)
		{

			avr++;

			// [!] : Copilot Optimization

			//a += n_bmp_a( color );
			//r += n_bmp_r( color );
			//g += n_bmp_g( color );
			//b += n_bmp_b( color );

			// cache components to avoid repeated bit shifts
			int ca = n_bmp_a( color );
			int cr = n_bmp_r( color );
			int cg = n_bmp_g( color );
			int cb = n_bmp_b( color );

			a += ca;
			r += cr;
			g += cg;
			b += cb;

		}


		tx++;
		if ( tx >= half )
		{
			tx = -half;

			ty++;
			if ( ty >= half ) { break; }
		}
	}


	return n_bmp_blend_pixel( center, n_bmp_div_pixel( a,r,g,b, avr ), blend );
}




#ifdef _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA

#define n_bmp_gamma_channel n_bmp_table_gamma_channel

#else  // #ifdef _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA

void
n_bmp_table_gamma_init( void )
{
	return;
}

void
n_bmp_table_gamma_exit( void )
{
	return;
}

n_type_real
n_bmp_gamma_channel( n_type_real n, n_type_real gamma )
{
	return 255.0 * pow( n_bmp_table_coeff_channel( n ), gamma );
}

#endif // #ifdef _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA

u32
n_bmp_gamma_pixel( u32 color, n_type_real gamma )
{

	// [Mechanism]
	//
	//	gamma = 0.0 : black
	//	gamma = 1.0 : nothing to do
	//	gamma = 2.0 : white


	// [!] : FORMULA : pow( 0.0=>1.0, gamma_from / gamma_to );


	// [!] : nothing to do

	if ( gamma == 1.0 ) { return color; }


	// [!] : precalc

	if ( gamma < 0.0 ) { gamma = 0.0; }
	if ( gamma > 2.0 ) { gamma = 2.0; }

	if ( gamma < 1.0 )
	{
		gamma = 1.0 / gamma;
	} else {
		gamma = gamma -  2.0;
		gamma = gamma * -1.0;
	}


	// [!] : alpha is reserved

	n_type_real a = n_bmp_a( color );
	n_type_real r = n_bmp_r( color );
	n_type_real g = n_bmp_g( color );
	n_type_real b = n_bmp_b( color );

	// [!] : 30% faster

	if ( ( r == g )&&( g == b ) )
	{
		r = g = b = n_bmp_gamma_channel( r, gamma );
	} else
	if ( r == g )
	{
		r = n_bmp_gamma_channel( r, gamma );
		g = r;
		b = n_bmp_gamma_channel( b, gamma );
	} else
	if ( g == b )
	{
		r = n_bmp_gamma_channel( r, gamma );
		g = n_bmp_gamma_channel( g, gamma );
		b = g;
	} else {
		r = n_bmp_gamma_channel( r, gamma );
		g = n_bmp_gamma_channel( g, gamma );
		b = n_bmp_gamma_channel( b, gamma );
	}


	return n_bmp_argb( a,r,g,b );
}




BOOL
n_bmp_color_similarity( u32 f, u32 t, int threshold )
{

	// [!] : Thx : DeepSeek AI : Euclid Distance Method

	int da = n_bmp_a( f ) - n_bmp_a( t );
	int dr = n_bmp_r( f ) - n_bmp_r( t );
	int dg = n_bmp_g( f ) - n_bmp_g( t );
	int db = n_bmp_b( f ) - n_bmp_b( t );

	n_type_real distance = sqrt( da*da + dr*dr + dg*dg + db*db );


	return ( distance < threshold );
}




// internal
n_type_real
n_bmp_bicubic_weight( n_type_real x )
{

	// [!] : Thx : DeepSeek AI


	const n_type_real a = -0.5;


	x = fabs( x );

	if ( x <= 1.0 )
	{
		return ( a + 2.0 ) * x * x * x - ( a + 3.0 ) * x * x + 1.0;
	} else
	if ( x < 2.0 )
	{
		return a * x * x * x - 5.0 * a * x * x + 8.0 * a * x - 4.0 * a;
	}


	return 0.0;
}

u32
n_bmp_bicubic_pixel( n_bmp *bmp, n_type_real x, n_type_real y )
{

	// [!] : Thx : DeepSeek AI

	n_type_real ix = floor( x );
	n_type_real iy = floor( y );
	n_type_real fx = x - ix;
	n_type_real fy = y - iy;

	//n_type_gfx bmpsx = N_BMP_SX( bmp );
	//n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_real wx[ 4 ], wy[ 4 ];
	for ( int i = -1; i <= 2; i++ )
	{
		wx[ i + 1 ] = n_bmp_bicubic_weight( fx - i );
		wy[ i + 1 ] = n_bmp_bicubic_weight( fy - i );
	}


	n_type_real total_a = 0, total_r = 0, total_g = 0, total_b = 0;
	n_type_real total_weight = 0;

	for ( int dy = -1; dy <= 2; dy++ )
	{
		for ( int dx = -1; dx <= 2; dx++ )
		{
			u32 color;
			if ( FALSE == n_bmp_ptr_get( bmp, ix + dx, iy + dy, &color ) )
			{
				n_type_real weight = wx[ dx + 1 ] * wy[ dy + 1 ];
				total_weight += weight;

				total_a += (n_type_real) n_bmp_a( color ) * weight;
				total_r += (n_type_real) n_bmp_r( color ) * weight;
				total_g += (n_type_real) n_bmp_g( color ) * weight;
				total_b += (n_type_real) n_bmp_b( color ) * weight;
			}
		}
	}


	if ( total_weight > 0 )
	{
		total_a /= total_weight;
		total_r /= total_weight;
		total_g /= total_weight;
		total_b /= total_weight;
	}


	total_a = (int) ( fmax( 0, fmin( 255, total_a + 0.5 ) ) );
	total_r = (int) ( fmax( 0, fmin( 255, total_r + 0.5 ) ) );
	total_g = (int) ( fmax( 0, fmin( 255, total_g + 0.5 ) ) );
	total_b = (int) ( fmax( 0, fmin( 255, total_b + 0.5 ) ) );


	return n_bmp_argb( total_a, total_r, total_g, total_b );
}




#define n_bmp_ahsl n_bmp_argb
#define n_bmp_hsl  n_bmp_rgb

#define n_bmp_h n_bmp_r
#define n_bmp_s n_bmp_g
#define n_bmp_l n_bmp_b

#define n_bmp_hue        n_bmp_r
#define n_bmp_saturation n_bmp_g
#define n_bmp_lightness  n_bmp_b

n_posix_inline n_type_real
n_bmp_ahsl2argb_channel( n_type_real min, n_type_real max, n_type_real c )
{

	// [!] : 1% faster than division

	// [!] : Copilot Optimization

	/* Precompute (max-min)/60 once to avoid repeated multiplies. */
	const n_type_real delta = ( ( max - min ) * ( 1.0f / 60.0f ) );

	if ( c <  60.0f ) {
		return min + ( delta * c );
	} else if ( c < 180.0f ) {
		return max;
	} else if ( c < 240.0f ) {
		return min + ( delta * ( 240.0f - c ) );
	} else {
		return min;
	}
}

u32
n_bmp_ahsl2argb( u32 hsl )
{

	n_type_real a = (n_type_real)              n_bmp_a( hsl );
	n_type_real h = n_bmp_table_coeff_channel( n_bmp_h( hsl ) );
	n_type_real s = n_bmp_table_coeff_channel( n_bmp_s( hsl ) );
	n_type_real l = n_bmp_table_coeff_channel( n_bmp_l( hsl ) );


	n_type_real min,max;

	if ( l <= 0.5 )
	{
		max = ( l * ( 1.0 + s ) );
	} else {
		max = ( l * ( 1.0 - s ) ) + s;
	}

	min = ( 2 * l ) - max;


//n_posix_debug_literal( "%f %f %f", h,s,l );

	n_type_real r,g,b;

	if ( s == 0.0 )
	{

		r = g = b = l;

	} else {

		h *= 360;

		r = h + 120.0;
		g = h;
		b = h - 120.0;

		if ( r >= 360.0 ) { r -= 360.0; }
		if ( b <    0.0 ) { b += 360.0; }

		r = n_bmp_ahsl2argb_channel( min, max, r );
		g = n_bmp_ahsl2argb_channel( min, max, g );
		b = n_bmp_ahsl2argb_channel( min, max, b );

	}


	// Normalization

	// [!] : Copilot Optimization

	/* normalize once */
	const n_type_real _scale255 = 255.0f;
	r *= _scale255;
	g *= _scale255;
	b *= _scale255;

	/* [Needed] : Mac : cast to int; cast alpha to int as expected by n_bmp_argb */
	return n_bmp_argb( (int) a, (int) r, (int) g, (int) b );
}

u32
n_bmp_argb2ahsl( u32 color )
{

	// [!] : range of return value : 0 => 255

	n_type_real a = (n_type_real) n_bmp_a( color );
	n_type_real r = n_bmp_table_coeff_channel( n_bmp_r( color ) );
	n_type_real g = n_bmp_table_coeff_channel( n_bmp_g( color ) );
	n_type_real b = n_bmp_table_coeff_channel( n_bmp_b( color ) );


	// [!] : this code is slower than below : 0.5%

	//n_type_real min = n_posix_min_n_type_real( r, n_posix_min_n_type_real( g, b ) );
	//n_type_real max = n_posix_max_n_type_real( r, n_posix_max_n_type_real( g, b ) );

	n_type_real min,max;
	min = max = r;

	if ( min > g ) { min = g; }
	if ( min > b ) { min = b; }

	if ( max < g ) { max = g; }
	if ( max < b ) { max = b; }


	// Hue

	n_type_real h = 0.0;

	if ( max == min )
	{

		h = 0.0;

	} else
	if ( max == r )
	{

		h = 60.0 * ( ( g - b ) / ( max - min ) );
		h = fmod( h, 360.0 );

	} else
	if ( max == g )
	{

		h = 60.0 * ( ( b - r ) / ( max - min ) );
		h = h + 120.0;

	} else
	if ( max == b )
	{

		h = 60.0 * ( ( r - g ) / ( max - min ) );
		h = h + 240.0;

	}


	// Lightness

	n_type_real l = ( max + min ) / 2.0;


	// Saturation

	n_type_real s = 0;

	if ( max == min )
	{
		//s = 0;
	} else
	if ( l <= 0.5 )
	{
		s = ( max - min ) / ( max + min );
	} else {
		s = ( max - min ) / ( 2.0 - ( max + min ) );
	}


	// Normalization

	// [!] : Copilot Optimization

	//n_posix_debug_literal( "%f %f %f", h,s,l );

	/* Precompute common scales to avoid repeated multiplies */
	const n_type_real _scale255 = 255.0f;
	const n_type_real _hscale   = _scale255 / 360.0f;

	h *= _hscale;
	s *= _scale255;
	l *= _scale255;

	/* [Needed] : Mac : cast to int */
	return n_bmp_ahsl( (int) a, (int) h, (int) s, (int) l );
}

#define n_bmp_hue_wheel_tweak_pixel( color, h ) n_bmp_hsl_tweak_pixel( color, h, 0, 0 )
#define n_bmp_vividness_tweak_pixel( color, s ) n_bmp_hsl_tweak_pixel( color, 0, s, 0 )
#define n_bmp_lightness_tweak_pixel( color, l ) n_bmp_hsl_tweak_pixel( color, 0, 0, l )

u32
n_bmp_hsl_tweak_pixel( u32 color, int hue, int saturation, int lightness )
{

	color = n_bmp_argb2ahsl( color );

	int a = n_bmp_a( color );
	int h = n_bmp_h( color );
	int s = n_bmp_s( color );
	int l = n_bmp_l( color );


	// [!] : saturation : no touch when grayscale

	h = n_bmp_clamp_channel( h + hue );
	if ( s != 0 ) { s = n_bmp_clamp_channel( s + saturation ); }
	l = n_bmp_clamp_channel( l + lightness );


	color = n_bmp_ahsl2argb( n_bmp_ahsl( a,h,s,l ) );


	return color;
}

#define n_bmp_hue_wheel_replace_pixel( color, h ) n_bmp_hsl_replace_pixel( color,  h, -1, -1 )
#define n_bmp_vividness_replace_pixel( color, s ) n_bmp_hsl_replace_pixel( color, -1,  s, -1 )
#define n_bmp_lightness_replace_pixel( color, l ) n_bmp_hsl_replace_pixel( color, -1, -1,  l )

u32
n_bmp_hsl_replace_pixel( u32 color, int hue, int saturation, int lightness )
{

	color = n_bmp_argb2ahsl( color );

	int a = n_bmp_a( color );
	int h = n_bmp_h( color );
	int s = n_bmp_s( color );
	int l = n_bmp_l( color );

	if ( hue        != -1 ) { h =        hue; }
	if ( saturation != -1 ) { s = saturation; }
	if ( lightness  != -1 ) { l =  lightness; }

	color = n_bmp_ahsl2argb( n_bmp_ahsl( a,h,s,l ) );


	return color;
}




u32
n_bmp_sharpen_pixel( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_real blend )
{

	u32 center;
	int  a, r, g, b;
	int aa,rr,gg,bb;

	if (
		( FALSE == n_bmp_ptr_get( bmp, x, y, &center ) )
		&&
		( FALSE == n_bmp_is_trans( bmp, center ) )
	)
	{

		u32 ahsl = n_bmp_argb2ahsl( center );

		a = aa = n_bmp_a( ahsl );
		r = rr = n_bmp_r( ahsl );
		g = gg = n_bmp_g( ahsl );
		b = bb = n_bmp_b( ahsl );

	} else {

		return n_bmp_trans;

	}


	u32 color;

	if (
		( FALSE == n_bmp_ptr_get( bmp, x, y - 1, &color ) )
		&&
		( FALSE == n_bmp_is_trans( bmp, color ) )
	)
	{

		color = n_bmp_argb2ahsl( color );

		a += aa - (int) n_bmp_a( color );
		r += rr - (int) n_bmp_r( color );
		g += gg - (int) n_bmp_g( color );
		b += bb - (int) n_bmp_b( color );

	}

	if (
		( FALSE == n_bmp_ptr_get( bmp, x, y + 1, &color ) )
		&&
		( FALSE == n_bmp_is_trans( bmp, color ) )
	)
	{

		color = n_bmp_argb2ahsl( color );

		a += aa - (int) n_bmp_a( color );
		r += rr - (int) n_bmp_r( color );
		g += gg - (int) n_bmp_g( color );
		b += bb - (int) n_bmp_b( color );

	}

	if (
		( FALSE == n_bmp_ptr_get( bmp, x - 1, y, &color ) )
		&&
		( FALSE == n_bmp_is_trans( bmp, color ) )
	)
	{

		color = n_bmp_argb2ahsl( color );

		a += aa - (int) n_bmp_a( color );
		r += rr - (int) n_bmp_r( color );
		g += gg - (int) n_bmp_g( color );
		b += bb - (int) n_bmp_b( color );

	}

	if (
		( FALSE == n_bmp_ptr_get( bmp, x + 1, y, &color ) )
		&&
		( FALSE == n_bmp_is_trans( bmp, color ) )
	)
	{

		color = n_bmp_argb2ahsl( color );

		a += aa - (int) n_bmp_a( color );
		r += rr - (int) n_bmp_r( color );
		g += gg - (int) n_bmp_g( color );
		b += bb - (int) n_bmp_b( color );

	}


	a = n_bmp_clamp_channel( a );
	r = n_bmp_clamp_channel( r );
	g = n_bmp_clamp_channel( g );
	b = n_bmp_clamp_channel( b );


	// [!] : Hue and Saturation are not used

	u32 color_ret = n_bmp_argb( a,rr,gg,b );
	color_ret = n_bmp_ahsl2argb( color_ret );


	return n_bmp_blend_pixel( center, color_ret, blend );
}




n_posix_inline u32
n_bmp_composite_pixel_postprocess
(
	u32          color_lower,
	u32          color_upper,
	int          alpha_upper,
	BOOL         perpixel,
	BOOL         global,
	BOOL         finalize,
	n_type_real  blend,
	BOOL        *write_needed
)
{

	// [!] : no error ckecking for performance


	if ( perpixel )
	{

//f = n_bmp_blend_pixel( f, t, n_bmp_blend_alpha2ratio( alpha_upper ) );

		if ( alpha_upper == N_BMP_ALPHA_CHANNEL_INVISIBLE )
		{

			color_upper = color_lower;

		} else {
//color_upper = n_bmp_blend_pixel( color_upper, color_lower, n_bmp_blend_alpha2ratio( alpha_upper ) );

			// [!] : finalize : alpha will be fully resolved

			if ( finalize )
			{
				if ( N_BMP_ALPHA_CHANNEL_VISIBLE != n_bmp_a( color_lower ) )
				{
//color_upper = n_bmp_black;
					n_type_real ratio = n_bmp_blend_alpha2ratio( alpha_upper );
					color_upper  = n_bmp_blend_pixel( color_upper, color_lower, ratio );
				} else {
					n_type_real ratio = n_bmp_blend_alpha2ratio( alpha_upper );
					color_upper  = n_bmp_alpha_visible_pixel( color_upper );
					color_upper  = n_bmp_blend_pixel( color_upper, color_lower, ratio );
				}
			}

		}

	}


	if ( global ) { color_upper = n_bmp_blend_pixel( color_upper, color_lower, blend ); }


	if ( write_needed != NULL )
	{
		(*write_needed) = ( color_upper != color_lower );
	} 


	return color_upper;
}

#define n_bmp_composite_pixel( u, l, x_u, y_u, x_l, y_l, pp,pp_onoff, f, b ) n_bmp_composite_pixel_fast( u, l, x_u, y_u, x_l, y_l, pp,pp_onoff, f, TRUE, b, NULL ) 

n_posix_inline u32
n_bmp_composite_pixel_fast
(
	 const n_bmp *bmp_upper,
	 const n_bmp *bmp_lower,
	  n_type_gfx  x_upper,
	  n_type_gfx  y_upper,
	  n_type_gfx  x_lower,
	  n_type_gfx  y_lower,
	BOOL          perpixel_override,
	BOOL          perpixel_override_onoff,
	BOOL          finalize,
	BOOL          trans_onoff,
	n_type_real   blend,
	BOOL         *write_needed
)
{

	// [!] : no error ckecking for performance


	u32 color_upper; n_bmp_ptr_get_fast( bmp_upper, x_upper,y_upper, &color_upper );

	if ( trans_onoff )
	{
		if ( n_bmp_is_trans( bmp_upper, color_upper ) )
		{
			if ( write_needed != NULL ) { (*write_needed) = FALSE; }
			return color_upper;
		}
	}


	int alpha_upper = n_bmp_a( color_upper );


	BOOL perpixel = ( alpha_upper != N_BMP_ALPHA_CHANNEL_VISIBLE );
	BOOL global   = ( blend != 0.0 );

	if ( perpixel_override ) { perpixel = perpixel_override_onoff; }

	if ( ( perpixel == FALSE )&&( global == FALSE ) )
	{
		if ( write_needed != NULL ) { (*write_needed) = TRUE; }
		return color_upper;
	}


	u32 color_lower; n_bmp_ptr_get_fast( bmp_lower, x_lower,y_lower, &color_lower );


	// [!] : Copilot Optimization

	// Inline postprocess to avoid function call overhead in ultrafast path
	if ( perpixel )
	{
		if ( alpha_upper == N_BMP_ALPHA_CHANNEL_INVISIBLE )
		{
			color_upper = color_lower;
		}
		else
		{
			// finalize: alpha will be fully resolved
			if ( finalize )
			{
				n_type_real ratio = n_bmp_blend_alpha2ratio( alpha_upper );
				if ( N_BMP_ALPHA_CHANNEL_VISIBLE != n_bmp_a( color_lower ) )
				{
					color_upper = n_bmp_blend_pixel( color_upper, color_lower, ratio );
				} else {
					color_upper = n_bmp_alpha_visible_pixel( color_upper );
					color_upper = n_bmp_blend_pixel( color_upper, color_lower, ratio );
				}
			}
		}
	}

	if ( global ) { color_upper = n_bmp_blend_pixel( color_upper, color_lower, blend ); }

	if ( write_needed != NULL )
	{
		(*write_needed) = ( color_upper != color_lower );
	}

	return color_upper;
}

n_posix_inline u32
n_bmp_composite_pixel_ultrafast
(
	const n_bmp  *bmp_upper,
	const n_bmp  *bmp_lower,
	n_type_index  pos_upper,
	n_type_index  pos_lower,
	BOOL          perpixel_override,
	BOOL          perpixel_override_onoff,
	BOOL          finalize,
	BOOL          trans_onoff,
	n_type_real   blend,
	BOOL         *write_needed
)
{

	// [!] : no error ckecking for performance


	u32 color_upper = N_BMP_PTR( bmp_upper )[ pos_upper ];

	if ( trans_onoff )
	{
		if ( color_upper == n_bmp_trans )
		{
			if ( write_needed != NULL ) { (*write_needed) = FALSE; }
			return color_upper;
		}
	}

	int alpha_upper = n_bmp_a( color_upper );


	BOOL perpixel = ( alpha_upper != N_BMP_ALPHA_CHANNEL_VISIBLE );
	BOOL global   = ( blend != 0.0 );

	if ( perpixel_override ) { perpixel = perpixel_override_onoff; }

	if ( ( perpixel == FALSE )&&( global == FALSE ) )
	{
		if ( write_needed != NULL ) { (*write_needed) = TRUE; }
		return color_upper;
	}


	u32 color_lower = N_BMP_PTR( bmp_lower )[ pos_lower ];


	return n_bmp_composite_pixel_postprocess( color_lower, color_upper, alpha_upper, perpixel, global, finalize, blend, write_needed );
}




typedef struct {

	float L;
	float a;
	float b;

	int   alpha;

} n_bmp_oklab;

float
n_bmp_oklab_srgb_to_linear( float x )
{
	if ( x <= 0.04045f )
	{
		return x / 12.92f;
	} else {
		return powf( ( x + 0.055f ) / 1.055f, 2.4f );
	}
}

n_bmp_oklab
n_bmp_oklab_rgb2oklab( u32 color )
{

	float lr = n_bmp_oklab_srgb_to_linear( (float) n_bmp_r( color ) / 255.0f );
	float lg = n_bmp_oklab_srgb_to_linear( (float) n_bmp_g( color ) / 255.0f );
	float lb = n_bmp_oklab_srgb_to_linear( (float) n_bmp_b( color ) / 255.0f );
//NSLog( @"%f %f %f", lr, lg, lb );

	float l = 0.4122214708f * lr + 0.5363325363f * lg + 0.0514459929f * lb;
	float m = 0.2119034982f * lr + 0.6806995451f * lg + 0.1073969566f * lb;
	float s = 0.0883024619f * lr + 0.2817188376f * lg + 0.6299787005f * lb;
//NSLog( @"%f %f %f", l, m, s );

	float l_ = powf( l, 1.0f / 3.0f );
	float m_ = powf( m, 1.0f / 3.0f );
	float s_ = powf( s, 1.0f / 3.0f );
//NSLog( @"%f %f %f", l_, m_, s_ );

	n_bmp_oklab out;

	out.L = 0.2104542553f * l_ + 0.7936177850f * m_ - 0.0040720468f * s_;
	out.a = 1.9779984951f * l_ - 2.4285922050f * m_ + 0.4505937099f * s_;
	out.b = 0.0259040371f * l_ + 0.7827717662f * m_ - 0.8086757660f * s_;

	out.alpha = n_bmp_a( color );


	return out;
}

float
n_bmp_oklab_linear_to_srgb( float x )
{
	if ( x <= 0.0031308f )
	{
		return 12.92f * x;
	} else {
		return 1.055f * powf( x, 1.0f / 2.4f ) - 0.055f;
	}
}

u32
n_bmp_oklab_oklab2rgb( n_bmp_oklab ok )
{

	float l_ = ok.L + 0.3963377774f * ok.a + 0.2158037573f * ok.b;
	float m_ = ok.L - 0.1055613458f * ok.a - 0.0638541728f * ok.b;
	float s_ = ok.L - 0.0894841775f * ok.a - 1.2914855480f * ok.b;

	float l = l_ * l_ * l_;
	float m = m_ * m_ * m_;
	float s = s_ * s_ * s_;

	float lr = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
	float lg = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
	float lb = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

	float fr = n_bmp_oklab_linear_to_srgb( fmaxf( 0.0f, fminf( 1.0f, lr ) ) ) * 255;
	float fg = n_bmp_oklab_linear_to_srgb( fmaxf( 0.0f, fminf( 1.0f, lg ) ) ) * 255;
	float fb = n_bmp_oklab_linear_to_srgb( fmaxf( 0.0f, fminf( 1.0f, lb ) ) ) * 255;


	return n_bmp_argb( ok.alpha, (int) fr, (int) fg, (int) fb );
}

u32
n_bmp_oklab_tweak( u32 color, float zero_to_one )
{

	n_bmp_oklab ok = n_bmp_oklab_rgb2oklab( color );

	ok.L = zero_to_one;

	return n_bmp_oklab_oklab2rgb( ok );
}




#endif //_H_NONNON_NEUTRAL_BMP_COLOR

