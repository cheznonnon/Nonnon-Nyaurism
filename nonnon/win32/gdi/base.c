// OrangeCat
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GDI_BASE
#define _H_NONNON_WIN32_GDI_BASE




#include "../../neutral/bmp/all.c"




#define n_gdi_base_luna_solid( gdi, bmp, color, sc ) n_gdi_base_luna( gdi, bmp, color, sc, n_posix_false )
#define n_gdi_base_luna_press( gdi, bmp, color, sc ) n_gdi_base_luna( gdi, bmp, color, sc, n_posix_true  )

void
n_gdi_base_luna( n_gdi *gdi, n_bmp *bmp, u32 color, int scaling, n_posix_bool is_pressed )
{

	u32 c1, c2, c3, c4;

	if ( is_pressed == n_posix_false )
	{

		c1 = n_bmp_white;
		c2 = n_bmp_blend_pixel( color, n_bmp_white, 0.75 );//n_bmp_rgb( 246,246,243 );
		c3 = n_bmp_blend_pixel( color, n_bmp_white, 0.50 );//n_bmp_rgb( 241,241,236 );
		c4 = n_bmp_blend_pixel( color, n_bmp_black, 0.10 );//n_bmp_rgb( 214,208,197 );

	} else {

		c1 = n_bmp_blend_pixel( color, n_bmp_black, 0.12 );//n_bmp_rgb( 209,204,193 );
		c2 = n_bmp_blend_pixel( color, n_bmp_black, 0.06 );//n_bmp_rgb( 228,227,220 );
		c3 = n_bmp_blend_pixel( color, n_bmp_black, 0.04 );//n_bmp_rgb( 225,224,216 );
		c4 = n_bmp_blend_pixel( color, n_bmp_white, 0.25 );//n_bmp_rgb( 242,241,238 );

	}

//c1 = n_bmp_rgb( 255,  0,  0 );
//c2 = n_bmp_rgb(   0,255,  0 );
//c3 = n_bmp_rgb(   0,255,255 );
//c4 = n_bmp_rgb(   0,  0,255 );

	int mode = N_BMP_GRADIENT_RECTANGLE;
	int type = N_BMP_GRADIENT_VERTICAL;

	if ( scaling <= 0 ) { scaling = 1; }

	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );
	n_type_gfx o  = 2 * scaling;

	if ( n_posix_true )
	{

		n_bmp_gradient( bmp, 0,0,sx,sy, c2,c3, mode | type );

		n_type_gfx offset = 4 * scaling;

		n_type_gfx  y = o;
		n_type_gfx ty = sy - o; if ( scaling != 1 ) { ty++; }
		n_posix_loop
		{

			if ( ( y >= o )&&( y <= ( o + offset ) ) )
			{
				u32 c = n_bmp_blend_pixel( c1, c2, (n_type_real) ( y - o ) / offset );
				n_bmp_line( bmp, 0,y,sx,y+1, c );
			} else
			if ( ( y >= ( ty - offset ) )&&( y <= ty ) )
			{
				u32 c = n_bmp_blend_pixel( c4, c3, (n_type_real) ( ty - y ) / offset );
				n_bmp_line( bmp, 0,y,sx,y+1, c );
			}

			y++;
			if ( y >= ty ) { break; }
		}
//n_bmp_save_literal( bmp, "ret.bmp" );

	} else {

		// [!] : this will be beautiful gradient but no way to use

		n_bmp_gradient( bmp, 0,0,sx,sy, c1,c4, mode | type );

	}


	return;
}


#endif // _H_NONNON_WIN32_GDI_BASE

