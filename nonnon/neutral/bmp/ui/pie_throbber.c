// Nonnon Win
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_UI_PIE_THROBBER
#define _H_NONNON_NEUTRAL_BMP_UI_PIE_THROBBER




#include "../all.c"




void
n_bmp_ui_pie_throbber_white_on_black( n_bmp *bmp )
{

	n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_index i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];
		if ( color != n_bmp_black )
		{
			N_BMP_PTR( bmp )[ i ] = n_bmp_white;
		}


		i++;
		if ( i >= c ) { break; }
	}

	return;
}

void
n_bmp_ui_pie_throbber( n_bmp *bmp, n_type_gfx fx, n_type_gfx fy, n_type_gfx size, n_type_gfx size_thickness, u32 color_bg, u32 color_fg, n_type_real degree )
{

	// [!] : base

	n_bmp bmp_f; n_bmp_zero( &bmp_f ); n_bmp_new_fast( &bmp_f, size, size );
	n_bmp bmp_t; n_bmp_zero( &bmp_t ); n_bmp_new_fast( &bmp_t, size, size );
	n_bmp bmp_p; n_bmp_zero( &bmp_p ); n_bmp_new_fast( &bmp_p, size, size );

	n_bmp_fastcopy( bmp, &bmp_f, fx,fy, size,size, 0,0 );
	n_bmp_fastcopy( bmp, &bmp_t, fx,fy, size,size, 0,0 );
	n_bmp_flush( &bmp_p, n_bmp_black );

	if ( size_thickness > 0 )
	{
		n_bmp_hoop( &bmp_f, 0,0,size,size, size_thickness, color_bg );
		n_bmp_hoop( &bmp_t, 0,0,size,size, size_thickness, color_fg );
		n_bmp_hoop( &bmp_p, 0,0,size,size, size_thickness, n_bmp_white );
	} else {
		n_bmp_circle( &bmp_f, 0,0,size,size, color_bg );
		n_bmp_circle( &bmp_t, 0,0,size,size, color_fg );
		n_bmp_circle( &bmp_p, 0,0,size,size, n_bmp_white );
	}

	n_bmp_ui_pie_throbber_white_on_black( &bmp_p );


	// [!] : indicator

	if ( degree <= 0 )
	{
		n_bmp_transcopy( &bmp_f, bmp, 0,0,size,size, fx,fy );

		n_bmp_free_fast( &bmp_f );
		n_bmp_free_fast( &bmp_t );
		n_bmp_free_fast( &bmp_p );

		return;
	}


	u32 color_indicator = n_bmp_rgb_mac( 0,0,255 );

	n_type_gfx half = size / 2;

	n_bmp_line( &bmp_p, half, half, half, 0, color_indicator );

	n_type_gfx tx = half * sin( 2.0 * M_PI *         degree   / 360 );
	n_type_gfx ty = half * cos( 2.0 * M_PI * ( 180 + degree ) / 360 );

	if ( tx < 0 ) { tx--; }
	if ( ty < 0 ) { ty--; }

	n_bmp_line( &bmp_p, half, half, half+tx, half+ty, color_indicator );
	n_bmp_fill( &bmp_p, half+1, 1, color_indicator );

//n_bmp_transcopy( &bmp_p, bmp, 0,0,size,size, fx,fy ); return;

	n_bmp_flush_antialias( &bmp_p, 0.5 );


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{
		u32 color; n_bmp_ptr_get_fast( &bmp_p, x,y, &color );
		if ( ( color == n_bmp_black )||( color == n_bmp_white ) )
		{
			//
		} else {
			if ( color == color_indicator )
			{
				n_bmp_ptr_get_fast( &bmp_t, x,y, &color );
				n_bmp_ptr_set_fast( &bmp_f, x,y,  color );
			} else {
				u32 c_f; n_bmp_ptr_get_fast( &bmp_f, x,y, &c_f );
				u32 c_t; n_bmp_ptr_get_fast( &bmp_t, x,y, &c_t );
				u32 c_p; n_bmp_ptr_get_fast( &bmp_p, x,y, &c_p );

				n_type_real coeff = ( 255 - n_bmp_b( c_p ) ) * n_bmp_coeff_channel;

				color = n_bmp_blend_pixel( c_f, c_t, coeff );

				n_bmp_ptr_set_fast( &bmp_f, x,y, color );
			}
		}

		x++;
		if ( x >= size )
		{
			x = 0;

			y++;
			if ( y >= size ) { break; }
		}
	}


	// [!] : done!

	n_bmp_transcopy( &bmp_f, bmp, 0,0,size,size, fx,fy );

	n_bmp_free_fast( &bmp_f );
	n_bmp_free_fast( &bmp_t );
	n_bmp_free_fast( &bmp_p );


	return;
}




#endif // _H_NONNON_NEUTRAL_BMP_UI_PIE_THROBBER

