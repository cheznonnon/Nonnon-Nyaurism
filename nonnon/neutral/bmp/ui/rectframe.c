// Nonnon Win
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : currently experimental : include manually




#ifndef _H_NONNON_NEUTRAL_BMP_UI_RECTFRAME
#define _H_NONNON_NEUTRAL_BMP_UI_RECTFRAME




#include "../all.c"




void
n_bmp_ui_rectframe_drawedge( n_bmp *bmp, int scale, u32 color[ 8 ] )
{

	// [Mechanism] : Color Order
	//
	//	color[ 0 ] : Outer : top
	//	color[ 1 ] : Outer : bottom
	//	color[ 2 ] : Outer : left
	//	color[ 3 ] : Outer : right
	//	color[ 4 ] : Inner : top
	//	color[ 5 ] : Inner : bottom
	//	color[ 6 ] : Inner : left
	//	color[ 7 ] : Inner : right


	// [!] : user32 DrawEdge() compatible module
	//
	//	GetDIBits() based approach has some problems
	//
	//	+ alpha value will be lost when system color depth is 16bit
	//	+ DrawEdge() has no color parameter, use system color automatically
	//	+ black color will be transparent by neutral/bmp.c


	//if ( n_bmp_error( bmp ) ) { return; }

	const s32 bmpsx = N_BMP_SX( bmp );
	const s32 bmpsy = N_BMP_SY( bmp );


	s32 fx,fy,tx,ty;


	// Rendering Order : top - left - right - bottom

	// Outer

	fx = 0;
	fy = 0;
	tx = fx + bmpsx - 1;
	ty = fy + bmpsy - 1;

	int i = 0;
	n_posix_loop
	{

		n_bmp_line( bmp, fx,fy,tx,fy, color[ 0 ] );
		n_bmp_line( bmp, fx,fy,fx,ty, color[ 2 ] );
		n_bmp_line( bmp, tx,fy,tx,ty, color[ 3 ] );
		n_bmp_line( bmp, fx,ty,tx,ty, color[ 1 ] );

		fx++;
		fy++;
		tx--;
		ty--;

		i++;
		if ( i >= scale ) { break; }
	}


	// Inner
/*
	fx++;
	fy++;
	tx--;
	ty--;
*/
	i = 0;
	n_posix_loop
	{

		n_bmp_line( bmp, fx,fy,tx,fy, color[ 4 ] );
		n_bmp_line( bmp, fx,fy,fx,ty, color[ 6 ] );
		n_bmp_line( bmp, tx,fy,tx,ty, color[ 7 ] );
		n_bmp_line( bmp, fx,ty,tx,ty, color[ 5 ] );

		fx++;
		fy++;
		tx--;
		ty--;

		i++;
		if ( i >= scale ) { break; }
	}


	return;
}

void
n_bmp_ui_rectframe( n_bmp *bmp, int scale, u32 color[ 8 ] )
{

	if ( n_bmp_error( bmp ) ) { return; }


	const s32 sx = N_BMP_SX( bmp );
	const s32 sy = N_BMP_SY( bmp );


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st( &b, sx,sy );

	n_bmp_ui_rectframe_drawedge( &b, scale, color );

	n_bmp_flush_transcopy( &b, bmp );

//n_bmp_save_literal( &b, "b.bmp" );

	n_bmp_free_fast( &b );


	return;
}




#endif // _H_NONNON_NEUTRAL_BMP_UI_RECTFRAME

