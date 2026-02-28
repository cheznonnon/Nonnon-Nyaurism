// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_DETECT
#define _H_NONNON_NEUTRAL_BMP_DETECT




#include <math.h>




#include "./_error.c"
#include "./_transparent.c"


#include "../type.c"




BOOL
n_bmp_grayscale_detect( u32 color )
{

	// [!] : alpha is not used

	int r = n_bmp_r( color );
	int g = n_bmp_g( color );
	int b = n_bmp_b( color );


	return ( ( r == g )&&( g == b ) );
}

#define n_bmp_hoop_detect n_bmp_circle_detect
#define n_bmp_circle_detect(  x,y,r     ) n_bmp_ellipse_detect_coeff( x,y, r, r, NULL )
#define n_bmp_ellipse_detect( x,y,rx,ry ) n_bmp_ellipse_detect_coeff( x,y,rx,ry, NULL )

BOOL
n_bmp_ellipse_detect_coeff( n_type_gfx x, n_type_gfx y, n_type_gfx rx, n_type_gfx ry, n_type_real *coeff )
{

	x = n_posix_abs_n_type_gfx( x );
	y = n_posix_abs_n_type_gfx( y );

	if ( ( rx < x )||( ry < y ) )
	{
		if ( coeff != NULL ) { (*coeff) = 0.0; }
		return FALSE;
	}


	n_type_real dx = x * x;
	n_type_real dy = y * y;


	n_type_real drx, dry;

	if ( rx == ry )
	{
		drx = dry = rx * rx;
	} else {
		drx = rx * rx;
		dry = ry * ry;
	}


	dx /= drx;
	dy /= dry;


	// [!] : this is slightly inaccurate

	if ( coeff != NULL )
	{
		(*coeff) = 2.0 - ( ( dx + dy ) * 2 );

		n_type_gfx r_min = n_posix_min_n_type_gfx( rx,ry );
		if ( r_min > 10 )
		{
			(*coeff) *= r_min * 0.25; 
		}
	}


	return ( ( dx + dy ) < 1.0 );
}

#define n_bmp_roundrect_detect( x,y,sx,sy,r ) n_bmp_roundrect_detect_coeff( x,y,sx,sy,r, NULL )

static BOOL n_bmp_roundrect_detect_coeff_patch_onoff = TRUE;

BOOL
n_bmp_roundrect_detect_coeff( n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_gfx r, n_type_real *coeff )
{

	if ( ( x < 0 )||( y < 0 ) )
	{
		if ( coeff != NULL ) { (*coeff) = 0.0; }

		return FALSE;
	}

	if ( ( x >= sx )||( y >= sy ) )
	{
		if ( coeff != NULL ) { (*coeff) = 0.0; }

		return FALSE;
	}

	if ( ( x > r )&&( y > r )&&( x < ( sx - r ) )&&( y < ( sy - r ) ) )
	{
		if ( coeff != NULL ) { (*coeff) = 1.0; }

		return TRUE;
	}


	// [!] : for pixel-wise accuracy

	// [x] : side-effect : a diagonal line will be emphasized

	if ( n_bmp_roundrect_detect_coeff_patch_onoff )
	{
		 x += 1;
		 y += 1;
		sx += 2;
		sy += 2;
	}


	n_type_gfx x_min = r;
	n_type_gfx y_min = r;
	n_type_gfx x_max = sx - 1 - r;
	n_type_gfx y_max = sy - 1 - r;


	if ( x <= x_min ) { x -= x_min; } else if ( x >= x_max ) { x -= x_max; } else { x = 0; }
	if ( y <= y_min ) { y -= y_min; } else if ( y >= y_max ) { y -= y_max; } else { y = 0; }


	n_type_gfx dx, dy, dr;

	if ( x == y )
	{
		dx = dy = x * x;
		dr = r * r;
	} else {
		dx = x * x;
		dy = y * y;
		dr = r * r;
	}


	if ( coeff != NULL )
	{
		(*coeff)  = ( dr + dr ) - ( ( dx + dy ) * 2 );
		(*coeff) /= dr;
		(*coeff) *= r * 0.25;

#ifdef N_POSIX_PLATFORM_MAC
		// [!] : Win32 : not inspected
		if ( isnan( (*coeff) ) ) { (*coeff) = 0.0; }
#endif // #ifdef N_POSIX_PLATFORM_MAC
	}


	return ( ( dx + dy ) < dr );
}

BOOL
n_bmp_squircle_detect( n_type_gfx x, n_type_gfx y, n_type_gfx size, n_type_real power, n_type_real *coeff )
{

	// [Mechanism]
	//
	//	power 2 : circle
	//	power 4 : squircle
	//	power 8 : round rect
	//
	//	use between 2.1 to 4.0 for iPhone/Android-like squircle

	// [!] : "size" : for performance
	//
	//	proceses like "size /= 2; size--;" first


	// [!] : change into polar co-ord system

	n_type_real a = pow( n_posix_abs_n_type_gfx( x - size ), power );
	n_type_real b = pow( n_posix_abs_n_type_gfx( y - size ), power );
	n_type_real r = pow(                             size  , power );


	if ( coeff != NULL )
	{
		(*coeff) = ( r + r ) - ( ( a + b ) * 2 );
		(*coeff) /= r; 
		(*coeff) *= size * 0.125; 
	}


	return ( ( a + b ) < r );
}

BOOL
n_bmp_moire_detect( n_type_gfx x, n_type_gfx y, int pattern )
{

	// [!] : Copilot Optimization

	switch( pattern )
	{
		case 0: return TRUE;
		case 1: return ( ( ( x & 1 ) ^ ( y & 1 ) ) != 0 );
		case 2: return ( ( ( x & 1 ) ^ ( y & 1 ) ) == 0 );
	}

/*
	if ( pattern == 0 )
	{

		return TRUE;

	} else
	if ( pattern == 1 )
	{

		// oxoxo
		// xoxox
		// oxoxo

		x &= 1;
		y &= 1;

		if (
			( ( x == 0 )&&( y != 0 ) )
			||
			( ( x != 0 )&&( y == 0 ) )
		)
		{
			return TRUE;
		}

	} else
	if ( pattern == 2 )
	{

		// xoxox
		// oxoxo
		// xoxox

		x &= 1;
		y &= 1;

		if (
			( ( x == 0 )&&( y == 0 ) )
			||
			( ( x != 0 )&&( y != 0 ) )
		)
		{
			return TRUE;
		}
	}

*/
	return FALSE;
}

#define N_BMP_EDGE_NONE  ( 0 << 0 )
#define N_BMP_EDGE_INNER ( 1 << 0 )
#define N_BMP_EDGE_OUTER ( 1 << 1 )
#define N_BMP_EDGE_INOUT ( N_BMP_EDGE_INNER | N_BMP_EDGE_OUTER )
#define N_BMP_EDGE_MASK  ( N_BMP_EDGE_INOUT | 0xffff0000 )
#define N_BMP_EDGE_U     ( 1 << 2 )
#define N_BMP_EDGE_D     ( 1 << 3 )
#define N_BMP_EDGE_L     ( 1 << 4 )
#define N_BMP_EDGE_R     ( 1 << 5 )
#define N_BMP_EDGE_UL    ( 1 << 6 )
#define N_BMP_EDGE_UR    ( 1 << 7 )
#define N_BMP_EDGE_DL    ( 1 << 8 )
#define N_BMP_EDGE_DR    ( 1 << 9 )
#define N_BMP_EDGE_ALL   ( ~N_BMP_EDGE_MASK )
#define N_BMP_EDGE_CROSS ( N_BMP_EDGE_U | N_BMP_EDGE_D | N_BMP_EDGE_L | N_BMP_EDGE_R )

// internal
n_type_gfx
n_bmp_edge_detect_check( n_bmp *bmp, n_type_gfx fx, n_type_gfx fy )
{

	n_type_gfx x,y,tx,ty;
	u32        color;
	n_type_gfx edge, count;
	int        i, ret;


	if ( n_bmp_error( bmp ) ) { return 0; }


	// Center

	if ( FALSE == n_bmp_ptr_get( bmp, fx,fy, &color ) )
	{

		if ( n_bmp_is_trans( bmp, color ) )
		{
			edge = N_BMP_EDGE_OUTER;
		} else {
			edge = N_BMP_EDGE_INNER;
		}

	} else {

		edge = N_BMP_EDGE_OUTER;

	}


	x = y = -1;


	i = ret = count = 0;
	n_posix_loop
	{

		tx = fx + x;
		ty = fy + y;

		ret = n_bmp_ptr_get( bmp, tx,ty, &color );
		if ( ret == 0 )
		{

			if (
				( ( edge & N_BMP_EDGE_INNER )&&( FALSE != n_bmp_is_trans( bmp, color ) ) )
				||
				( ( edge & N_BMP_EDGE_OUTER )&&( FALSE == n_bmp_is_trans( bmp, color ) ) )
			)
			{

				count++;

				if ( i == 0 ) { edge |= N_BMP_EDGE_UL; } else
				if ( i == 1 ) { edge |= N_BMP_EDGE_U;  } else
				if ( i == 2 ) { edge |= N_BMP_EDGE_UR; } else
				if ( i == 3 ) { edge |= N_BMP_EDGE_L;  } else
				// ( i == 4 ) : center
				if ( i == 5 ) { edge |= N_BMP_EDGE_R;  } else
				if ( i == 6 ) { edge |= N_BMP_EDGE_DL; } else
				if ( i == 7 ) { edge |= N_BMP_EDGE_D;  } else
				if ( i == 8 ) { edge |= N_BMP_EDGE_DR; }

			}

		}


		i++;


		x++;
		if ( x >= 2 )
		{

			x = -1;

			y++;
			if ( y >= 2 ) { break; }
		}
	}


	if ( edge & N_BMP_EDGE_INNER ) { count = 9 - count; }


	return edge | ( count << 16 );
}

BOOL
n_bmp_edge_detect( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx edge, n_type_gfx *count )
{

	if ( edge == N_BMP_EDGE_NONE ) { return FALSE; }


	n_type_gfx check = n_bmp_edge_detect_check( bmp, x,y );

	if ( count != NULL )
	{
		(*count) = check >> 16;
	}


	// [!] : different type

	if (
		( ( edge & N_BMP_EDGE_INOUT ) != N_BMP_EDGE_INOUT )
		&&
		( ( edge & N_BMP_EDGE_INOUT ) != ( check & N_BMP_EDGE_INOUT ) )
	)
	{
		return 0;
	}


	return ( ( edge & N_BMP_EDGE_ALL ) & ( check & N_BMP_EDGE_ALL ) );
}


#endif //_H_NONNON_NEUTRAL_BMP_DETECT

