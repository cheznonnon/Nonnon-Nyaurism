// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_UI_SEARCH_ICON
#define _H_NONNON_NEUTRAL_BMP_UI_SEARCH_ICON




#include "../all.c"
#include "../../png.c"




void
n_bmp_ui_search_icon_crop( n_bmp *bmp, u32 color_trans )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_gfx fx = 0;
	n_type_gfx tx = 0;
	n_type_gfx fy = bmpsx;
	n_type_gfx ty = bmpsy;


	n_type_gfx x,y;


	// [!] : Left
	
	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_trans ) { fx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x++;
			if ( x >= bmpsx ) { break; }
		}
	}

	// [!] : Top

	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_trans ) { fy = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}

	// [!] : Right

	x = bmpsx - 1;
	y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_trans ) { tx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x--;
			if ( x < 0 ) { break; }
		}
	}

	// [!] : Bottom

	x = 0;
	y = bmpsy - 1;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != color_trans ) { ty = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y--;
			if ( y < 0 ) { break; }
		}
	}

	if ( fx >     0 ) { fx--; }
	if ( fy >     0 ) { fy--; }
	if ( tx < bmpsx ) { tx++; }
	if ( ty < bmpsx ) { ty++; }

	n_type_gfx fsx = tx - fx; if ( fsx < 0 ) { return; }
	n_type_gfx fsy = ty - fy; if ( fsy < 0 ) { return; }

	n_bmp ret; n_bmp_zero( &ret ); n_bmp_new( &ret, fsx, fsy );

	n_bmp_fastcopy( bmp, &ret, fx,fy,fsx,fsy, 0,0 );

	n_bmp_free( bmp );
	n_bmp_alias( &ret, bmp );


	return;
}

void
n_bmp_ui_search_icon_png_save( n_bmp *bmp, n_posix_char *path )
{

	n_png png = n_png_template;

#ifdef N_POSIX_PLATFORM_MAC
	n_bmp_mac( bmp );
#endif

	n_png_compress( &png, bmp );
	n_png_save( &png, path );

	n_png_free( &png );


	return;
}

void
n_bmp_ui_search_icon_make( n_bmp *bmp_ret, n_type_gfx canvas_size, u32 color_contour, u32 color_body )
{

	BOOL prv_trans = n_bmp_transparent_onoff_default;
	n_bmp_transparent_onoff_default = FALSE;


	u32 color_trans   = n_bmp_black_invisible;
	//u32 color_contour = n_bmp_black;
	//u32 color_body    = n_bmp_argb( 255, 255,255,255 );


	n_type_gfx canvas = canvas_size;
	n_type_gfx thick  = canvas / 8;
	n_type_gfx body   = canvas / 3 * 2;
	n_type_gfx offset = canvas / 32;


	n_bmp bmp_contour; n_bmp_zero( &bmp_contour );
	n_bmp bmp_handle ; n_bmp_zero( &bmp_handle  );


	n_type_gfx size;
	n_type_gfx pos;

	{
		n_bmp_new_fast( &bmp_contour, canvas, canvas );
		n_bmp_flush( &bmp_contour, color_trans );

		size = body;
		pos  = ( canvas - size ) / 2;

		n_bmp_circle( &bmp_contour, pos,pos, size,size, color_contour );

		pos  += offset;
		size -= offset * 2;

	}


	if ( color_contour == 0 )
	{

		n_type_gfx handle_sx = (n_type_real) canvas / 2.5;
		n_type_gfx handle_sy = ( thick * sqrt( 2 ) ) - offset;

		n_bmp_new_fast( &bmp_handle, handle_sx, handle_sy );
		n_bmp_flush( &bmp_handle, color_trans );

		n_type_gfx pt = 0;
		n_type_gfx sx = N_BMP_SX( &bmp_handle );
		n_type_gfx sy = N_BMP_SY( &bmp_handle );

		n_bmp_roundrect_ratio( &bmp_handle, pt,pt, sx,sy, color_body, 100 );

		n_bmp_matrix_rotate( &bmp_handle, 45, color_trans, thick );

		n_bmp_resizer( &bmp_handle, canvas/2, canvas/2, color_trans, N_BMP_RESIZER_CENTER );

	} else {

		n_type_gfx handle_sx = canvas / 2;
		n_type_gfx handle_sy = thick * sqrt( 2 );


		n_bmp_new_fast( &bmp_handle, handle_sx, handle_sy );
		n_bmp_flush( &bmp_handle, color_trans );

		n_type_gfx pt = 0;
		n_type_gfx sx = N_BMP_SX( &bmp_handle );
		n_type_gfx sy = N_BMP_SY( &bmp_handle );

		n_bmp_flush_roundrect_ratio( &bmp_handle, color_contour, 100 );

		pt += offset;
		sx -= offset * 2;
		sy -= offset * 2;

		n_bmp_roundrect_ratio( &bmp_handle, pt,pt, sx,sy, color_body, 100 );

		n_bmp_matrix_rotate( &bmp_handle, 45, color_trans, thick );

	}


	{
		n_bmp bmp;

		n_bmp_carboncopy( &bmp_contour, &bmp );

		n_bmp_flush_transcopy( &bmp_handle, &bmp );

		n_bmp_circle( &bmp, pos,pos, size,size, color_body );

		pos  += thick;
		size -= thick * 2;
		n_bmp_circle( &bmp, pos,pos, size,size, color_contour );

		pos  += offset;
		size -= offset * 2;
		n_bmp_circle( &bmp, pos,pos, size,size, color_trans );


		n_bmp_ui_search_icon_crop( &bmp, color_trans );
		n_bmp_resizer( &bmp, canvas, canvas, color_trans, N_BMP_RESIZER_CENTER );


		n_bmp_flush_mirror( &bmp, N_BMP_MIRROR_LEFTSIDE_RIGHT );

#ifndef N_POSIX_PLATFORM_MAC
		n_bmp_flush_mirror( &bmp, N_BMP_MIRROR_UPSIDE_DOWN );
#endif

//n_bmp_ui_search_icon_png_save( &bmp, "/Users/nonnon2/Desktop/icon.png" );

		n_bmp_free_fast( bmp_ret );
		n_bmp_alias( &bmp, bmp_ret );
	}

	n_bmp_free_fast( &bmp_contour );
	n_bmp_free_fast( &bmp_handle  );


	n_bmp_transparent_onoff_default = prv_trans;


	return;
}


#endif // _H_NONNON_NEUTRAL_BMP_UI_SEARCH_ICON
