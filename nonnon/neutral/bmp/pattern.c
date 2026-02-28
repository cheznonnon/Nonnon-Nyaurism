// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_PATTERN
#define _H_NONNON_NEUTRAL_BMP_PATTERN




#include "../random.c"


#include "./_error.c"
#include "./draw.c"
#include "./filter.c"
#include "./transform.c"




#define N_BMP_PATTERN_POLKADOT_NONE ( 0 )
#define N_BMP_PATTERN_POLKADOT_FOAM ( 1 )
#define N_BMP_PATTERN_POLKADOT_POP  ( 2 )
#define N_BMP_PATTERN_POLKADOT_RND  ( 3 )

#define n_bmp_pattern_polkadot(      b, sz, mrgn, bg, fg ) n_bmp_pattern_polkadot_main( b, sz, mrgn, bg, fg, N_BMP_PATTERN_POLKADOT_NONE )
#define n_bmp_pattern_polkadot_foam( b, sz, mrgn, bg, fg ) n_bmp_pattern_polkadot_main( b, sz, mrgn, bg, fg, N_BMP_PATTERN_POLKADOT_FOAM )
#define n_bmp_pattern_polkadot_pop(  b, sz, mrgn, bg     ) n_bmp_pattern_polkadot_main( b, sz, mrgn, bg,  0, N_BMP_PATTERN_POLKADOT_POP  )
#define n_bmp_pattern_polkadot_rnd(  b, sz, mrgn, bg     ) n_bmp_pattern_polkadot_main( b, sz, mrgn, bg,  0, N_BMP_PATTERN_POLKADOT_RND  )

void
n_bmp_pattern_polkadot_main( n_bmp *bmp, n_type_gfx size_circle, n_type_gfx size_margin, u32 bg, u32 fg, int option )
{

	if ( n_bmp_error( bmp ) ) { return; }

/*
	// [x] : this code cannot draw full transparent circle

	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );
	n_type_gfx mapsx = bmpsx + size_margin;
	n_type_gfx mapsy = bmpsy + size_margin;
	n_type_gfx unit  = size_margin;


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, size_circle, size_circle );

	n_bmp_flush( &b, n_bmp_white_invisible );
	n_bmp_flush_circle( &b, N_BMP_SX( &b ), fg );


	n_bmp_resizer( &b,  unit, unit, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );
	n_bmp_resizer( &b, mapsx,mapsy, n_bmp_white_invisible, N_BMP_RESIZER_TILE   );


	n_type_gfx o = 0;

	n_bmp_flush( bmp, bg );
	n_bmp_transcopy( &b, bmp, 0,0,mapsx,mapsy, o,o ); o -= unit / 2;
	n_bmp_transcopy( &b, bmp, 0,0,mapsx,mapsy, o,o );


	n_bmp_free( &b );
*/

//option = N_BMP_PATTERN_POLKADOT_FOAM;


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );
	n_type_gfx unit  = abs( size_margin ); // [!] : anti-fault
	n_type_gfx half  = unit / 2;


	n_bmp_flush( bmp, bg );


	n_type_gfx x = half % bmpsx;
	n_type_gfx y = half % bmpsy;

	if ( x != 0 ) { x = -( bmpsx - ( x / 2 ) ); }
	if ( y != 0 ) { y = -( bmpsy - ( y / 2 ) ); }

	if ( option == N_BMP_PATTERN_POLKADOT_NONE )
	{

		n_type_gfx shift  = 0;
		n_type_gfx startx = x;
		n_posix_loop
		{

			n_bmp_circle( bmp, x,y, size_circle,size_circle, fg );

			x += unit;
			if ( x >= bmpsx )
			{

				shift++;
				if ( shift & 1 ) { x = startx; } else { x = startx - half; }

				y += half;
				if ( y >= bmpsy ) { break; }
			}

		}

	} else
	if ( option == N_BMP_PATTERN_POLKADOT_FOAM )
	{

		n_type_gfx shift  = 0;
		n_type_gfx startx = x;
		n_posix_loop
		{

			n_type_real zero_one = (n_type_real) y / bmpsy;
			n_type_gfx  step     = size_circle - ( (n_type_real) size_circle * zero_one );

			n_bmp_circle( bmp, x,y, step, step, fg );

			x += unit;
			if ( x >= bmpsx )
			{

				shift++;
				if ( shift & 1 ) { x = startx; } else { x = startx - half; }

				y += half;
				if ( y >= bmpsy ) { break; }
			}

		}

	} else
	if ( option == N_BMP_PATTERN_POLKADOT_POP )
	{

		int color_max = 6;
		u32 color[] = {

			n_bmp_rgb( 255,200,  0 ),
			n_bmp_rgb(   0,255,200 ),
			n_bmp_rgb( 200,  0,255 ),
			n_bmp_rgb( 200,255,  0 ),
			n_bmp_rgb(   0,200,255 ),
			n_bmp_rgb( 255,  0,200 ),

		};


		int        clr_i  = 0;
		n_type_gfx shift  = 0;
		n_type_gfx startx = x;
		n_posix_loop
		{

			n_bmp_circle( bmp, x,y, size_circle,size_circle, color[ clr_i ] );

			clr_i++;
			if ( clr_i >= color_max ) { clr_i = 0; }

			x += unit;
			if ( x >= bmpsx )
			{

				shift++;
				if ( shift & 1 ) { x = startx; } else { x = startx - half; }

				y += half;
				if ( y >= bmpsy ) { break; }
			}

		}

	} else
	if ( option == N_BMP_PATTERN_POLKADOT_RND )
	{

		n_type_gfx shift  = 0;
		n_type_gfx startx = x;
		n_posix_loop
		{

			u32 color = n_bmp_rgb( n_random_range( 256 ), n_random_range( 256 ), n_random_range( 256 ) );

			n_bmp_circle( bmp, x,y, size_circle,size_circle, color );

			x += unit;
			if ( x >= bmpsx )
			{

				shift++;
				if ( shift & 1 ) { x = startx; } else { x = startx - half; }

				y += half;
				if ( y >= bmpsy ) { break; }
			}

		}

	}


	return;
}

void
n_bmp_pattern_roundrect( n_bmp *bmp, n_type_gfx size_rectangle, n_type_gfx size_margin, u32 bg, u32 fg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );
	n_type_gfx unit  = size_margin;
	n_type_gfx half  = unit / 2;


	n_bmp_flush( bmp, bg );


	n_type_gfx x = half % bmpsx;
	n_type_gfx y = half % bmpsy;

	if ( x != 0 ) { x = -( bmpsx - ( x / 2 ) ); }
	if ( y != 0 ) { y = -( bmpsy - ( y / 2 ) ); }

	{

		n_type_gfx shift  = 0;
		n_type_gfx startx = x;
		n_posix_loop
		{

			n_type_gfx o   = unit / 20;
			n_type_gfx rx  = x + o;
			n_type_gfx ry  = y + o;
			n_type_gfx rsx = size_rectangle - ( o * 2 );
			n_type_gfx rsy = size_rectangle - ( o * 2 );

			n_bmp_roundrect_ratio( bmp, rx,ry,rsx,rsy, fg, 25 );

			x += unit;
			if ( x >= bmpsx )
			{

				shift++;
				if ( shift & 1 ) { x = startx; } else { x = startx - half; }

				y += half;
				if ( y >= bmpsy ) { break; }
			}

		}

	}


	return;
}

void
n_bmp_pattern_check( n_bmp *bmp, n_type_gfx size_unit, n_type_real ratio_stripe, n_type_real tweak_sharpness, u32 bg, u32 fg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [ Mechanism ]
	//
	//	[ ratio_stripe ]
	//
	//	0.0/0.5/1.0 : Gingham
	//	others      : Tartan


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, size_unit,1 );

	n_bmp_flush( &b, n_bmp_white_invisible );


	n_type_real o  = 2 * M_PI;
	n_type_real hz = (n_type_real) o / size_unit;
	n_type_real d1 = (n_type_real) hz * 1;
	n_type_real d2 = (n_type_real) hz * size_unit;

	d2 *= ratio_stripe;

	n_type_real x = 0;
	n_type_real y = 0;
	n_posix_loop
	{

		n_type_real d = cos( d1 * x ) + sin( d2 * x );

		u32 color = n_bmp_blend_pixel( bg, fg, d * tweak_sharpness );
//if ( x == 0 ) { color = n_bmp_rgb(255,0,0); }
		n_bmp_ptr_set( &b, (n_type_gfx) x, (n_type_gfx) y, color );

		x++;
		if ( x >= size_unit ) { break; }
	}

	n_bmp_resizer( &b, size_unit,size_unit, n_bmp_white_invisible, N_BMP_RESIZER_TILE );
	n_bmp_flush_copy( &b, &b, FALSE, 0.5, 0, N_BMP_COPY_ROTATE_LEFT, 0 );
//n_bmp_save_literal( &b, "check.bmp" );


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );

	n_bmp_resizer( &b, bmpsx,bmpsy, n_bmp_white_invisible, N_BMP_RESIZER_TILE );

	n_bmp_flush_fastcopy( &b, bmp );

	n_bmp_free( &b );


	return;
}

void
n_bmp_pattern_stripe( n_bmp *bmp, n_type_gfx size_unit, BOOL is_horz, u32 bg, u32 fg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx sx = size_unit * 2;
	n_type_gfx sy = size_unit * 2;


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy ); n_bmp_flush( &b, bg );


	if ( is_horz )
	{
		n_bmp_box( &b, 0,size_unit, sx,size_unit, fg );
	} else {
		n_bmp_box( &b, size_unit,0, size_unit,sy, fg );
	}


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );

	n_bmp_resizer( &b, bmpsx,bmpsy, n_bmp_white_invisible, N_BMP_RESIZER_TILE );

	n_bmp_flush_fastcopy( &b, bmp );

	n_bmp_free( &b );


	return;
}

void
n_bmp_pattern_checker( n_bmp *bmp, n_type_gfx size_unit, u32 bg, u32 fg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx sx = size_unit * 2;
	n_type_gfx sy = size_unit * 2;


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy ); n_bmp_flush( &b, bg );


	n_bmp_box( &b,         0,        0, size_unit,size_unit, fg );
	n_bmp_box( &b, size_unit,size_unit, size_unit,size_unit, fg );


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );

	n_bmp_resizer( &b, bmpsx,bmpsy, n_bmp_white_invisible, N_BMP_RESIZER_TILE );

	n_bmp_flush_fastcopy( &b, bmp );

	n_bmp_free( &b );


	return;
}




#endif // _H_NONNON_NEUTRAL_BMP_PATTERN

