// Nonnon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_PROJECT_CARDGENERATOR
#define _H_NONNON_PROJECT_CARDGENERATOR




#include "../../cardgenerator.c"




void
n_cardgenerator_init( n_cardgenerator *p, n_type_gfx piles )
{

	// Init

	p->is_init = TRUE;


	// Size

	n_type_gfx desktop_sx, desktop_sy; n_cardgenerator_desktop_size( &desktop_sx, &desktop_sy );

	//desktop_sx =  640; desktop_sy =  480; // VGA
	//desktop_sx =  800; desktop_sy =  600; // SVGA
	//desktop_sx = 1024; desktop_sy =  768; // XGA
	//desktop_sx = 1280; desktop_sy = 1024; // SXGA

	//desktop_sx = 1366; desktop_sy =  768; // Netbook
	//desktop_sx = 2736; desktop_sy = 1824; // Surface Pro 2017

	if ( desktop_sx >= 1000 )
	{
		desktop_sx = (n_type_gfx) ( (n_type_real) desktop_sx * 0.8 );
		desktop_sy = (n_type_gfx) ( (n_type_real) desktop_sy * 0.8 );
	} else {
		desktop_sx = (n_type_gfx) ( (n_type_real) desktop_sx * 0.9 );
		desktop_sy = (n_type_gfx) ( (n_type_real) desktop_sy * 0.9 );
	}

	p->unit_sx  = N_CARDGENERATOR_UNIT;
	p->unit_sy  = (n_type_gfx) ( (n_type_real) p->unit_sx * 1.5 );
	p->resample = (n_type_real) ( desktop_sx / piles ) / p->unit_sx;


	// DPI x1 : 180 270
	// DPI x2 : 180 270
//NSLog( @"%d %d", p->unit_sx, p->unit_sy );

	// DPI x1 : 1536 864
	// DPI x2 : 1176 764
//NSLog( @"%d %d", desktop_sx, desktop_sy );


	p->border   = (n_type_gfx) (  3 * p->resample );
	p->halo     = (n_type_gfx) ( 12 * p->resample );


//n_posix_debug_literal( " %f %d ", p->resample, p->border );


	// Blank Card Maker

	{

		const int blend   = 0;// p->option_blend;
		const u32 lighter = n_bmp_rgb( 222,222,222 );
		const u32 darker  = n_bmp_rgb(  50, 50, 50 );
		//const u32 black   = n_bmp_rgb(  10, 10, 10 );
		const u32 white   = n_bmp_white;

		if ( n_cardgenerator_darkmode_is_on() )
		{
			p->color_frame = n_bmp_rgb( 66, 66, 66 );
			n_cardgenerator_base( p, &p->bmp_blank, darker, blend );
		} else {
			p->color_frame = lighter;
			n_cardgenerator_base( p, &p->bmp_blank, white , blend );
		}

	}

	if ( n_cardgenerator_darkmode_is_on() )
	{
		p->color_margin = n_bmp_alpha_invisible_pixel( p->color_frame );
	} else {
		p->color_margin = n_bmp_white_invisible;
	}


	// Resampling Test

	{
		n_bmp b; n_bmp_carboncopy( &p->bmp_blank, &b );
		n_cardgenerator_resampler( p, &b, p->color_frame );

		p->card_sx = N_BMP_SX( &b );
		p->card_sy = N_BMP_SY( &b );
//n_posix_debug_literal( " %d : %f ", p->card_sx, p->unit_sx * p->resample );

		n_bmp_free_fast( &b );
	}

	p->csx_min = p->csx = p->card_sx * piles;
	p->csy_min = p->csy = n_posix_min_n_type_gfx( desktop_sy, p->card_sy * 4 );
	p->osy     = (n_type_gfx) ( p->card_sy * 0.18 );


	return;
}

void
n_cardgenerator_loop( n_cardgenerator *p )
{

	// Debug Center

	u32 tick = 0;//n_posix_tickcount();


	if ( p->is_init )
	{
		p->is_init = FALSE;
	} else {
		return;
	}


	// Load

	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_D"    , &p->bmp_font );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_NEKO" , &p->bmp_logo );

	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_0"  , &p->suit__1[ 0 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_1"  , &p->suit__1[ 1 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_2"  , &p->suit__1[ 2 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_3"  , &p->suit__1[ 3 ] );

	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_B_0", &p->suitbig[ 0 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_B_1", &p->suitbig[ 1 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_B_2", &p->suitbig[ 2 ] );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_S_B_3", &p->suitbig[ 3 ] );

	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_C_K"  , &p->bmp_crown_k );
	n_win64_resource_load_nbmp_literal( "N_CARDGENERATOR_BMP_C_Q"  , &p->bmp_crown_q );


	// Cat

	if ( 0 )
	{
		//
	} else
	if ( n_cardgenerator_darkmode_is_on() )
	{
		n_bmp_flush_replacer( &p->bmp_logo, n_bmp_argb(  64, 10, 10, 10 ), n_bmp_argb(  64, 10, 10, 10 ) );
		n_bmp_flush_replacer( &p->bmp_logo, n_bmp_argb( 128, 10, 10, 10 ), n_bmp_argb( 128, 10, 10, 10 ) );
	} else {
		n_bmp_flush_replacer( &p->bmp_logo, n_bmp_argb(  64,128,128,128 ), n_bmp_argb(  64,255,255,255 ) );
		n_bmp_flush_replacer( &p->bmp_logo, n_bmp_argb( 128,128,128,128 ), n_bmp_argb( 128,255,255,255 ) );
	}


	// Suit Mark and Digit Maker

//if(0)
	{

		n_bmp_transparent_onoff( &p->bmp_font, FALSE );

		n_bmp_flush_replacer( &p->bmp_font, n_bmp_rgb( 0,200,255 ), n_bmp_black );

		n_type_gfx i = 0;
		n_posix_loop
		{

			n_type_gfx size = N_BMP_SY( &p->bmp_font );
			n_bmp_new( &p->digit_1[ i ], size, size );
			n_bmp_new( &p->digit_2[ i ], size, size );

			n_bmp_fastcopy( &p->bmp_font, &p->digit_1[ i ], size * i,0,size,size, 0,0 );
			n_bmp_fastcopy( &p->bmp_font, &p->digit_2[ i ], size * i,0,size,size, 0,0 );

			n_bmp_flush_mirror( &p->digit_2[ i ], N_BMP_COPY_MIRROR_ROTATE180 );

			i++;
			if ( i >= N_CARDGENERATOR_CARD_UNIT ) { break; }
		}


		i = 0;
		n_posix_loop
		{

			n_bmp_carboncopy( &p->suit__1[ i ], &p->suit__2[ i ] );

			n_bmp_flush_mirror( &p->suit__2[ i ], N_BMP_COPY_MIRROR_ROTATE180 );

			i++;
			if ( i >= N_CARDGENERATOR_SUIT_MAX ) { break; }
		}

	}


	// Halo Maker : new card size is needed

	n_cardgenerator_dropshadow( &p->bmp_halo, p->card_sx, p->card_sy, p->halo, 1 );


	// Preload

	{

		// [x] : Win9x : can run but not working

		if ( n_thread_onoff() )
		{
//n_posix_debug_literal( "" );

			BOOL p_multithread = n_bmp_is_multithread;
			n_bmp_is_multithread = TRUE;

			n_thread thread_0 = n_thread_init( n_cardgenerator_thread_0, p );
			n_thread thread_1 = n_thread_init( n_cardgenerator_thread_1, p );
			n_thread thread_2 = n_thread_init( n_cardgenerator_thread_2, p );
			n_thread thread_3 = n_thread_init( n_cardgenerator_thread_3, p );

			n_thread_wait( thread_0 ); n_thread_exit( thread_0 );
			n_thread_wait( thread_1 ); n_thread_exit( thread_1 );
			n_thread_wait( thread_2 ); n_thread_exit( thread_2 );
			n_thread_wait( thread_3 ); n_thread_exit( thread_3 );

			n_bmp_is_multithread = p_multithread;

		} else {

			n_cardgenerator_card( p, 0 );
			n_cardgenerator_card( p, 1 );
			n_cardgenerator_card( p, 2 );
			n_cardgenerator_card( p, 3 );

		}

	}


	if ( tick ) { n_posix_debug_literal( "%d", (int) n_posix_tickcount() - tick ); }


	return;
}


#endif // _H_NONNON_PROJECT_CARDGENERATOR

