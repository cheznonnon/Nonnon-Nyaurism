// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_PEN
#define _H_NONNON_NEUTRAL_BMP_PEN




#include "../bmp.c"




typedef void (*n_bmp_pen_callback)( n_type_gfx fx, n_type_gfx fy, n_type_real blend );




void
n_bmp_pen_bresenham( n_type_gfx fx, n_type_gfx fy, n_type_gfx tx, n_type_gfx ty, n_type_real blend, n_bmp_pen_callback callback )
{

	// Bresenham's line algorithm


	if ( ( fx == tx )&&( fy == ty ) )
	{
		callback( fx,fy, blend );
		return;
	}


	n_type_real step = (n_type_real) n_posix_abs_n_type_gfx( fy - ty ) / n_posix_abs_n_type_gfx( fx - tx );
	n_type_real d    = 0;
	n_posix_loop
	{

		// [!] : the first pixel will be skipped


		// [!] : don't do "else" : avoid zig-zag

		if ( d <  1 )
		{
			d += step;
			if ( fx > tx ) { fx--; } else
			if ( fx < tx ) { fx++; }
		}
		if ( d >= 1 )
		{
			d -= 1;
			if ( fy > ty ) { fy--; } else
			if ( fy < ty ) { fy++; }
		}


		callback( fx,fy, blend );


		if ( ( fx == tx )&&( fy == ty ) ) { break; }

//n_win_hwndprintf_literal( hwnd_main, "%f : %f", d, step );

	}


	return;
}

void
n_bmp_pen_wu( n_type_real x1, n_type_real y1, n_type_real x2, n_type_real y2, n_type_real blend, n_bmp_pen_callback callback )
{

	// Xiaolin Wu's line algorithm


	//n_type_real start_x = x1;
	//n_type_real start_y = y1;


	n_type_real dx = x2 - x1;
	n_type_real dy = y2 - y1;


	n_type_gfx abs_dx = (n_type_gfx) fabs( dx );
	n_type_gfx abs_dy = (n_type_gfx) fabs( dy );

	if (
		( abs_dx == abs_dy )
		||
		( x1 == x2 )||( y1 == y2 )
	)
	{
//color = n_bmp_rgb( 255, 200, 0 );
//blend = 1;

		// [!] : this will be a straight line

		n_bmp_pen_bresenham( (n_type_gfx) x1, (n_type_gfx) y1, (n_type_gfx) x2, (n_type_gfx) y2, blend, callback );

		return;

	}


	// [Mechanism]
	//
	//	continuous line needs 0.0 to 1.0 for "step"
	//	swap temporarily when slope is steep
	//
	//	"step" may have minus value

	n_type_real step_x = 0;
	n_type_real step_y = 0;

	if ( abs_dx == abs_dy )
	{
		if ( dx > 0 ) { step_x = 1; } else { step_x = -1; }
		if ( dy > 0 ) { step_y = 1; } else { step_y = -1; }
	} else
	if ( abs_dy < abs_dx )
	{
		step_y = dy / abs_dx;
		if ( dx > 0 ) { step_x = 1; } else { step_x = -1; }
	} else
	if ( abs_dx < abs_dy )
	{
		step_x = dx / abs_dy;
		if ( dy > 0 ) { step_y = 1; } else { step_y = -1; }
	}


	n_type_real x = x1;
	n_type_real y = y1;
	n_posix_loop
	{

		n_type_real blend1 = blend;
		n_type_real blend2 = blend;

		if ( x == x1 )
		{
			blend1 *= 0.5;
			blend2 *= 0.0;
		} else {
			blend1 *= 1 - ( y - trunc( y ) );
			blend2 *=     ( y - trunc( y ) );
		}


		n_type_real xx1 = trunc( x );
		n_type_real yy1 = trunc( y );
		n_type_real xx2 = trunc( x );
		n_type_real yy2 = trunc( y ) + 1;

		if ( x != x1 )
		{
			callback( (n_type_gfx) xx1, (n_type_gfx) yy1, blend1 );
			callback( (n_type_gfx) xx2, (n_type_gfx) yy2, blend2 );
		}

//if ( ( start_x == xx1 )&&( start_y == yy1 ) ) { callback( xx1, yy1, 1.0 ); }
//if ( ( start_x == xx2 )&&( start_y == yy2 ) ) { callback( xx2, yy2, 1.0 ); }


//if ( xx1 == x2 ) { break; }

		if ( abs_dx == abs_dy )
		{
			if ( ( xx1 == x2 )&&( yy1 == y2 ) ) { break; }
		} else
		if ( abs_dy < abs_dx )
		{
			if ( xx1 == x2 ) { break; }
		} else
		if ( abs_dx < abs_dy )
		{
			if ( yy1 == y2 ) { break; }
		}

		x += step_x;
		y += step_y;

	}


	return;
}


#endif //_H_NONNON_NEUTRAL_BMP_PEN

