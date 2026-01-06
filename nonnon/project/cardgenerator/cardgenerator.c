// Nonnon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_PROJECT_CARDGENERATOR
#define _H_NONNON_PROJECT_CARDGENERATOR




#include "../../neutral/bmp/all.c"

#include "../../game/helper.c"




#ifdef N_POSIX_PLATFORM_MAC


u32 n_win_darkmode_bg_argb = n_bmp_rgb( 66, 66, 66 );

n_posix_bool
n_cardgenerator_darkmode_is_on( void )
{
	return n_mac_is_darkmode();
}

void
n_cardgenerator_desktop_size( n_type_gfx *desktop_sx, n_type_gfx *desktop_sy )
{

	NSRect rect = [[NSScreen mainScreen] frame];

	if ( desktop_sx != NULL ) {  (*desktop_sx) = NSWidth ( rect ); }
	if ( desktop_sy != NULL ) {  (*desktop_sy) = NSHeight( rect ); }


	return;
}


#else  // #ifdef N_POSIX_PLATFORM_MAC


#include "../../win32/win.c"

#include "../../game/rc.c"


inline n_posix_bool
n_cardgenerator_darkmode_is_on( void )
{
	return n_win_darkmode_onoff;
}

void
n_cardgenerator_desktop_size( n_type_gfx *desktop_sx, n_type_gfx *desktop_sy )
{

	n_win_desktop_size( desktop_sx, desktop_sy );

	return;
}


#endif // #ifdef N_POSIX_PLATFORM_MAC




#define N_CARDGENERATOR_SUIT_HEARTS   ( 0 )
#define N_CARDGENERATOR_SUIT_DIAMONDS ( 1 )
#define N_CARDGENERATOR_SUIT_SPADES   ( 2 )
#define N_CARDGENERATOR_SUIT_CLUBS    ( 3 )
#define N_CARDGENERATOR_SUIT_MAX      ( 4 )

#define N_CARDGENERATOR_CARD_UNIT     ( 13 )
#define N_CARDGENERATOR_CARD_ALL      ( N_CARDGENERATOR_CARD_UNIT * N_CARDGENERATOR_SUIT_MAX )

#define N_CARDGENERATOR_UNIT          ( 180 )




typedef struct {

	n_posix_bool is_init;

	n_posix_bool is_half;

	n_type_gfx unit_sx, unit_sy;
	n_type_gfx card_sx, card_sy;
	n_type_gfx csx_min, csy_min;
	n_type_gfx csx    , csy    ;
	n_type_gfx osx    , osy    ;
	n_type_gfx border;
	n_type_gfx halo;

	u32    color_margin;
	u32    color_halo;
	u32    color_frame;

	n_type_real resample;

	n_bmp  cards  [ N_CARDGENERATOR_CARD_ALL  ];
	n_bmp  digit_1[ N_CARDGENERATOR_CARD_UNIT ];
	n_bmp  digit_2[ N_CARDGENERATOR_CARD_UNIT ];
	n_bmp  suit__1[ N_CARDGENERATOR_SUIT_MAX  ];
	n_bmp  suit__2[ N_CARDGENERATOR_SUIT_MAX  ];
	n_bmp  suitbig[ N_CARDGENERATOR_SUIT_MAX  ];

	n_bmp  bmp_blank;
	n_bmp  bmp_logo;
	n_bmp  bmp_font;
	n_bmp  bmp_foundation;
	n_bmp  bmp_halo;
	n_bmp  bmp_crown_k;
	n_bmp  bmp_crown_q;

#ifdef N_POSIX_PLATFORM_MAC

	CGFloat backingScaleFactor;

#endif // #ifdef N_POSIX_PLATFORM_MAC

} n_cardgenerator;




n_posix_inline n_type_real
n_game_centering_real( n_type_real a, n_type_real b )
{

	// [!] : ( ( a / 2 ) - ( b / 2 ) ) == ( ( a - b ) / 2 )

	return ( a - b ) / 2;
}

n_posix_inline void
n_cardgenerator_copy( n_bmp *f, n_bmp *t, n_type_gfx fx, n_type_gfx fy, n_type_gfx sx, n_type_gfx sy, n_type_gfx tx, n_type_gfx ty )
{

	n_bmp_transcopy( f,t, fx,fy,sx,sy, tx,ty );


	return;
}

void
n_cardgenerator_box( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color, n_type_real coeff )
{

	// [x] : under DWM : round edge is not beautiful
	//
	//	when border color is bright
	//	see "Blank Card Maker" "middle"

	if (0)//( game.dwm_onoff )
	{
		n_bmp_box                  ( bmp, x,y,sx,sy, color            );
	} else {
		n_bmp_roundrect_ratio_coeff( bmp, x,y,sx,sy, color, 25, coeff );
	}


	return;
}

void
n_cardgenerator_gray( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }

/*
	n_type_gfx  x = 0;
	n_type_gfx  y = 0;
	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		if ( ( color != n_bmp_trans )&&( 0 != n_bmp_a( color ) ) )
		{
			color = n_bmp_grayscale_pixel( color );

			int alpha = n_bmp_a( color );
			color = n_bmp_blend_pixel( color, n_bmp_black, 0.2 );
			color = n_bmp_alpha_replace_pixel( color, alpha );

			n_bmp_ptr_set_fast( bmp, x,y, color );
		}


		x++;
		if ( x >= sx )
		{

			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}
*/

	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];

		if ( ( color != n_bmp_trans )&&( 0 != n_bmp_a( color ) ) )
		{
			color = n_bmp_grayscale_pixel( color );

			int alpha = n_bmp_a( color );

			if ( n_posix_false == n_cardgenerator_darkmode_is_on() )
			{
				color = n_bmp_blend_pixel( color, n_bmp_black, 0.2 );
			}

			color = n_bmp_alpha_replace_pixel( color, alpha );

			N_BMP_PTR( bmp )[ i ] = color;
		}

		i++;
		if ( i >= c ) { break; }
	}


	return;
}

void
n_cardgenerator_for_rasterizer( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx  x = 0;
	n_type_gfx  y = 0;
	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		if ( color == n_bmp_rgb( 0,200,255 ) )
		{
			color = n_bmp_black;
		} else {
			color = n_bmp_white;
		}

		n_bmp_ptr_set_fast( bmp, x,y, color );

		x++;
		if ( x >= sx )
		{

			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}


	return;
}

void
n_cardgenerator_dropshadow( n_bmp *bmp, n_type_gfx arg_sx, n_type_gfx arg_sy, n_type_gfx m, n_type_real thinness )
{

	const n_type_gfx mm = ( m * 2 ) + ( m % 2 );

	const n_type_gfx bmpsx = arg_sx + mm;
	const n_type_gfx bmpsy = arg_sy + mm;

	n_bmp_new_fast( bmp, bmpsx,bmpsy );
	n_bmp_flush( bmp, n_bmp_black );

	n_type_gfx i = 0;
	n_posix_loop
	{

		n_type_gfx  x = ( m / 2 ) + i;
		n_type_gfx  y = ( m / 2 ) + i;
		n_type_gfx sx = bmpsx - ( x * 2 );
		n_type_gfx sy = bmpsy - ( y * 2 );

		// [!] : d * thinness : high value causes thin shadow

		n_type_real d = 1.0 / ( ( 1 + i ) * 3 );

		int n = n_bmp_blend_pixel( n_bmp_white, n_bmp_black, d * thinness );

		n_cardgenerator_box( bmp, x,y,sx,sy, n_bmp_rgb( n,n,n ), 0.00 );

		i++;
		if ( i >= m ) { break; }
	}


	n_bmp_transparent_onoff( bmp, n_posix_false );

	i = 0;
	n_posix_loop
	{

		n_bmp_flush_antialias( bmp, 1.0 );

		i++;
		if ( i >= m ) { break; }
	}

	n_bmp_transparent_onoff( bmp, n_posix_true );

//n_bmp_save_literal( bmp, "ret.bmp" );

	return;
}




void
n_cardgenerator_resampler( n_cardgenerator *p, n_bmp *bmp, u32 color_frame )
{
//return;

	if ( p->resample < 1.0 ) { n_bmp_flush_antialias( bmp, 1.0 - p->resample ); }

	n_bmp_resampler( bmp, p->resample,p->resample );


	// Round Rect

	// [Needed] : after resampled

	n_type_gfx x,y,sx,sy;

	{

		n_type_gfx bmpsx = N_BMP_SX( bmp );
		n_type_gfx bmpsy = N_BMP_SY( bmp );

		n_bmp frame; n_bmp_zero( &frame ); n_bmp_new_fast( &frame, bmpsx, bmpsy );
		n_bmp_flush( &frame, n_bmp_black );

//n_game_hwndprintf_literal( " %d %d : %d %d : %d ", usx, usy, msx, msy, p->border );

		x  = 0     + ( p->border * 0 );
		y  = 0     + ( p->border * 0 );
		sx = bmpsx - ( p->border * 0 );
		sy = bmpsy - ( p->border * 0 );

		n_cardgenerator_box( &frame, x,y,sx,sy, n_bmp_white, 0.00 );

		x  = 0     + ( p->border * 1 );
		y  = 0     + ( p->border * 1 );
		sx = bmpsx - ( p->border * 2 );
		sy = bmpsy - ( p->border * 2 );

		n_cardgenerator_box( &frame, x,y,sx,sy, n_bmp_black, 0.25 );

//n_bmp_save_literal( &frame, "ret.bmp" );

		n_bmp_rasterizer( &frame, bmp, 0,0, color_frame, n_posix_false );

		n_bmp_cornermask( bmp, -25, 0, p->color_margin );

		n_bmp_free_fast( &frame );

	}


	return;
}

void
n_cardgenerator_contour( n_cardgenerator *p, n_bmp *bmp_f, n_bmp *bmp_t, n_type_gfx x, n_type_gfx y )
{
//return;

#ifdef N_POSIX_PLATFORM_MAC

	n_type_gfx m = p->border * p->backingScaleFactor;

#else  // #ifdef N_POSIX_PLATFORM_MAC

	n_type_gfx m = p->border;

#endif // #ifdef N_POSIX_PLATFORM_MAC

	n_type_gfx fx = -m;
	n_type_gfx fy = -m;
	n_posix_loop
	{

		n_bmp_rasterizer_main( bmp_f, bmp_t, x + fx, y + fy, NULL, n_bmp_white, n_posix_false );


		fx++;
		if ( fx > m )
		{

			fx = -m;

			fy++;
			if ( fy > m ) { break; }
		}
	}


	return;
}

void
n_cardgenerator_rasterizer( n_cardgenerator *p, n_bmp *bmp_f, n_bmp *bmp_t, n_type_real x, n_type_real y, n_bmp *bmp_grad )
{
//return;

	n_type_gfx tx = (n_type_gfx) x;
	n_type_gfx ty = (n_type_gfx) y;

	if ( n_cardgenerator_darkmode_is_on() )
	{
		n_cardgenerator_contour( p, bmp_f, bmp_t,  tx,ty );
	}

	n_bmp_rasterizer_main( bmp_f, bmp_t, tx,ty, bmp_grad, 0, n_posix_false );


	return;
}

void
n_cardgenerator_base( n_cardgenerator *p, n_bmp *bmp, u32 color, int blend )
{

	// Color

	if ( color == n_bmp_trans ) { color = n_bmp_rgb( 1,1,1 ); }

	if ( blend )
	{
		color = n_bmp_alpha_replace_pixel( color, 255 - blend );
	}


	// Base

	n_bmp_new_fast( bmp, p->unit_sx, p->unit_sy );

	n_bmp_flush( bmp, color );

//n_bmp_save_literal( bmp, "ret.bmp" );


	return;
}

void
n_cardgenerator_neko( n_cardgenerator *p, n_bmp *bmp, u32 color1, u32 color2, n_bmp *bmp_ret, int number )
{
//return;

	const u32 temp_bg = n_bmp_rgb( 0,200,255 );

	n_bmp_carboncopy( bmp, bmp_ret );

	int gradient = N_BMP_GRADIENT_CIRCLE | N_BMP_GRADIENT_CENTERING;
	n_bmp_flush_gradient( bmp_ret, color2, color1, gradient );

	n_bmp_flush_transcopy( bmp, bmp_ret );
	n_bmp_flush_replacer( bmp_ret, temp_bg, n_bmp_trans );

	if ( number == 12 )
	{
		n_bmp_flush_transcopy( &p->bmp_crown_q, bmp_ret );
	} else
	if ( number == 13 )
	{
		n_bmp_flush_transcopy( &p->bmp_crown_k, bmp_ret );
	}

	n_bmp_alpha_visible( bmp_ret );


	return;
}

void
n_cardgenerator_logo_rasterizer( n_cardgenerator *p, n_bmp *bmp_f, n_bmp *bmp_t, u32 color1, u32 color2, n_type_gfx x, n_type_gfx y, int number )
{
//return;

	n_bmp b; n_bmp_zero( &b ); n_cardgenerator_neko( p, &p->bmp_logo, color1, color2, &b, number );
//n_bmp_save_literal( &b, "ret.bmp" );

	//if ( n_cardgenerator_darkmode_is_on() )
	{
		n_bmp bmp_r; n_bmp_carboncopy( &p->bmp_logo, &bmp_r ); n_cardgenerator_for_rasterizer( &bmp_r );
		n_cardgenerator_contour( p, &bmp_r, bmp_t, x,y );
//n_bmp_save_literal( bmp_t, "ret.bmp" );
		n_bmp_free_fast( &bmp_r );
	}

	n_type_gfx sx = N_BMP_SX( &p->bmp_logo );
	n_type_gfx sy = N_BMP_SX( &p->bmp_logo );

	n_bmp_transcopy( &b, bmp_t, 0,0,sx,sy, x,y );

	n_bmp_antialias( bmp_t, x-2,y-2,sx+4,sy+4, 1.0 );
	n_bmp_antialias( bmp_t, x-2,y-2,sx+4,sy+4, 1.0 );

	n_bmp_transcopy( &b, bmp_t, 0,0,sx,sy, x,y );

	n_bmp_free_fast( &b );


	return;
}

void
n_cardgenerator_card( n_cardgenerator *p, int suit )
{
//return;

	const u32 color_dark[] = {

		n_bmp_rgb( 150,  0,100 ),
		n_bmp_rgb( 150,  0,  0 ),
		n_bmp_rgb(   0,100,150 ),
		n_bmp_rgb(   0,  0,150 )

	};
/*
	const u32 color_middle[] = {

		n_bmp_rgb( 200,  0,150 ),
		n_bmp_rgb( 200,  0,  0 ),
		n_bmp_rgb(   0,150,200 ),
		n_bmp_rgb(   0,  0,200 )

	};
*/
	const u32 color_light[] = {

		n_bmp_rgb( 255,  0,200 ),
		n_bmp_rgb( 255,  0,  0 ),
		n_bmp_rgb(   0,200,255 ),
		n_bmp_rgb(   0,  0,255 )

	};


	n_bmp  *bmp_c  = &p->cards  [ suit * N_CARDGENERATOR_CARD_UNIT ];
	n_bmp  *bmp_d1 =  p->digit_1;
	n_bmp  *bmp_d2 =  p->digit_2;
	n_bmp  *bmp_s1 = &p->suit__1[ suit ];
	n_bmp  *bmp_s2 = &p->suit__2[ suit ];
	n_bmp  *bmpbig = &p->suitbig[ suit ];


	n_type_real suit_sx = N_BMP_SX( bmp_s1 );
	n_type_real suit_sy = N_BMP_SY( bmp_s1 );
	n_type_real sbig_sx = N_BMP_SX( bmpbig );
	n_type_real sbig_sy = N_BMP_SY( bmpbig );
	n_type_real msx     = ( p->border * 4 ) + ( suit_sx / 4 );
	n_type_real msy     = ( p->border * 4 ) + ( suit_sy / 4 );
	n_type_real csx     = p->unit_sx - ( msx * 2 );
	n_type_real csy     = p->unit_sy - ( msy * 2 );
	n_type_real unit_sx = csx / 5;
	n_type_real unit_sy = csy / 7;
	n_type_real osx     = n_game_centering_real( unit_sx, suit_sx );
	n_type_real osy     = n_game_centering_real( unit_sy, suit_sy );
	n_type_real big_osx = n_game_centering_real( unit_sx, sbig_sx );
	n_type_real big_osy = n_game_centering_real( unit_sy, sbig_sy );


	u32 color_gradient_1 = color_light[ suit ];
	u32 color_gradient_2 = color_dark [ suit ];

	if ( n_cardgenerator_darkmode_is_on() )
	{
		color_gradient_1 = n_bmp_blend_pixel( color_gradient_1, n_bmp_black, 0.25 );
		color_gradient_2 = n_bmp_blend_pixel( color_gradient_2, n_bmp_black, 0.25 );
	}


//n_bmp_box( bmp_c,  ox, oy, suit_sx,suit_sy, n_bmp_black + 1 );
//n_bmp_box( bmp_c, msx,msy,     csx,    csy, n_bmp_black + 1 );


	// [!] : stencil

	n_bmp bmp_gradient; n_bmp_zero( &bmp_gradient ); n_bmp_new( &bmp_gradient, (n_type_gfx) suit_sx, (n_type_gfx) suit_sy );
	n_bmp bmp_grad_big; n_bmp_zero( &bmp_grad_big ); n_bmp_new( &bmp_grad_big, (n_type_gfx) sbig_sx, (n_type_gfx) sbig_sy );

	{
		int gradient = N_BMP_GRADIENT_RECTANGLE | N_BMP_GRADIENT_VERTICAL;
		n_bmp_flush_gradient( &bmp_gradient, color_gradient_1, color_gradient_2, gradient );
		n_bmp_flush_gradient( &bmp_grad_big, color_gradient_1, color_gradient_2, gradient );
	}


	n_type_real x,y;


	int i;


	i = 0;
	n_posix_loop
	{break;

		n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ i ] );

		i++;
		if ( i >= N_CARDGENERATOR_CARD_UNIT ) { break; }
	}


	// [!] : center : 1

	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 1 - 1 ] );

	x = msx + big_osx + ( unit_sx * 2 );
	y = msy + big_osy + ( unit_sy * 3 );

	n_cardgenerator_rasterizer( p, bmpbig, &bmp_c[ 1 - 1 ], x,y, &bmp_grad_big );


	// [!] : vertical : 2 and 3

	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 2 - 1 ] );


	x = msx + osx + ( unit_sx * 2 );


	y = msy + osy + ( unit_sy * 0 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 2 - 1 ], x,y, &bmp_gradient );


	y = msy + osy + ( unit_sy * 6 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 2 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 2 - 1 ], &bmp_c[ 3 - 1 ] );


	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 3 );

	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 3 - 1 ], x,y, &bmp_gradient );


	// [!] : four corners : 4 5 / 6 7 8 / 9 10

	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 4 - 1 ] );

	x = msx + osx + ( unit_sx * 1 );
	y = msy + osy + ( unit_sy * 0 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 4 - 1 ], x,y, &bmp_gradient );

	x = msx + osx + ( unit_sx * 3 );
	y = msy + osy + ( unit_sy * 0 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 4 - 1 ], x,y, &bmp_gradient );


	x = msx + osx + ( unit_sx * 1 );
	y = msy + osy + ( unit_sy * 6 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 4 - 1 ], x,y, &bmp_gradient );

	x = msx + osx + ( unit_sx * 3 );
	y = msy + osy + ( unit_sy * 6 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 4 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 4 - 1 ], &bmp_c[ 5 - 1 ] );


	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 3 );

	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 5 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 4 - 1 ], &bmp_c[ 6 - 1 ] );

	y = msy + osy + ( unit_sy * 3 );

	x = msx + osx + ( unit_sx * 1 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 6 - 1 ], x,y, &bmp_gradient );

	x = msx + osx + ( unit_sx * 3 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 6 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 6 - 1 ], &bmp_c[ 7 - 1 ] );

	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 1.5 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 7 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 7 - 1 ], &bmp_c[ 8 - 1 ] );

	y = msy + osy + ( unit_sy * 4.5 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 8 - 1 ], x,y, &bmp_gradient );


	//n_bmp_carboncopy( &bmp_c[ 7 - 1 ], &bmp_c[ 8 - 1 ] );

	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 4.5 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 8 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 4 - 1 ], &bmp_c[ 9 - 1 ] );

	y = msy + osy + ( unit_sy * 2 );

	x = msx + osx + ( unit_sx * 1 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 9 - 1 ], x,y, &bmp_gradient );

	x = msx + osx + ( unit_sx * 3 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 9 - 1 ], x,y, &bmp_gradient );


	y = msy + osy + ( unit_sy * 4 );

	x = msx + osx + ( unit_sx * 1 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 9 - 1 ], x,y, &bmp_gradient );

	x = msx + osx + ( unit_sx * 3 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 9 - 1 ], x,y, &bmp_gradient );


	n_bmp_carboncopy( &bmp_c[ 9 - 1 ], &bmp_c[ 10 - 1 ] );

	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 1 );
	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 10 - 1 ], x,y, &bmp_gradient );

	y = msy + osy + ( unit_sy * 5 );
	n_cardgenerator_rasterizer( p, bmp_s2, &bmp_c[ 10 - 1 ], x,y, &bmp_gradient );


	x = msx + osx + ( unit_sx * 2 );
	y = msy + osy + ( unit_sy * 3 );

	n_cardgenerator_rasterizer( p, bmp_s1, &bmp_c[ 9 - 1 ], x,y, &bmp_gradient );


	// [!] : logo : J Q K

	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 11 - 1 ] );
	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 12 - 1 ] );
	n_bmp_carboncopy( &p->bmp_blank, &bmp_c[ 13 - 1 ] );
/*
//if(0)
	{
		n_bmp b; n_bmp_carboncopy( bmpbig, &b );
		n_bmp_scaler_big( &b, 2 );
		//n_bmp_resampler( &b, 1.5, 1.5 );

		u32 color_bg;
		if ( n_cardgenerator_darkmode_is_on() )
		{
			color_bg = n_bmp_rgb( 111,111,111 );
		} else {
			color_bg = n_bmp_rgb( 222,222,222 );
		}

		n_bmp bg; n_bmp_carboncopy( bmpbig, &bg ); n_bmp_flush( &bg, color_bg );

		n_type_gfx xx = n_game_centering( N_BMP_SX( &bmp_c[ 0 ] ), N_BMP_SX( &b ) );
		n_type_gfx yy = n_game_centering( N_BMP_SY( &bmp_c[ 0 ] ), N_BMP_SY( &b ) );

		n_bmp_rasterizer_main( &b, &bmp_c[ 11 - 1 ], xx,yy, &bg, 0, n_posix_false );
		n_bmp_rasterizer_main( &b, &bmp_c[ 12 - 1 ], xx,yy, &bg, 0, n_posix_false );
		n_bmp_rasterizer_main( &b, &bmp_c[ 13 - 1 ], xx,yy, &bg, 0, n_posix_false );

		n_bmp_free_fast( &b );
	}
*/
	x = msx + big_osx + ( unit_sx * 2 );
	y = msy + big_osy + ( unit_sy * 3 );

	n_cardgenerator_logo_rasterizer( p, &p->bmp_logo, &bmp_c[ 11 - 1 ], color_gradient_1, color_gradient_2, (n_type_gfx) x,(n_type_gfx) y, 11 );
	n_cardgenerator_logo_rasterizer( p, &p->bmp_logo, &bmp_c[ 12 - 1 ], color_gradient_1, color_gradient_2, (n_type_gfx) x,(n_type_gfx) y, 12 );
	n_cardgenerator_logo_rasterizer( p, &p->bmp_logo, &bmp_c[ 13 - 1 ], color_gradient_1, color_gradient_2, (n_type_gfx) x,(n_type_gfx) y, 13 );


	// Foundation Maker

	if ( suit == 0 )
	{

#ifdef N_POSIX_PLATFORM_MAC

		n_bmp_carboncopy( &p->bmp_blank, &p->bmp_foundation );

		n_bmp_flush( &p->bmp_foundation, n_bmp_argb_mac( 0,128,128,128 ) );

		n_type_gfx sx = N_BMP_SX( &p->bmp_foundation );
		n_type_gfx sy = N_BMP_SY( &p->bmp_foundation );

		n_bmp_roundrect_ratio( &p->bmp_foundation, 0,0,sx,sy, n_bmp_argb_mac( 128,128,128,128 ), 25 );

		if ( p->resample < 1.0 ) { n_bmp_flush_antialias( &p->bmp_foundation, 1.0 - p->resample ); }
		n_bmp_resampler( &p->bmp_foundation, p->resample,p->resample );

#else  // #ifdef N_POSIX_PLATFORM_MAC

		u32 lighter = n_bmp_rgb( 222,222,222 );
		u32 dark    = n_bmp_rgb( 100,100,100 );

		if ( n_cardgenerator_darkmode_is_on() )
		{
			dark    = n_win_darkmode_bg_argb;
			lighter = n_bmp_blend_pixel( dark, n_bmp_white, 0.25 );
		}

		n_bmp_carboncopy( &p->bmp_blank, &p->bmp_foundation );
		n_cardgenerator_gray( &p->bmp_foundation );

		n_cardgenerator_logo_rasterizer( p, &p->bmp_logo, &p->bmp_foundation, lighter, dark, (n_type_gfx) x,(n_type_gfx) y, 11 );
		n_cardgenerator_gray( &p->bmp_foundation );

		n_cardgenerator_resampler( p, &p->bmp_foundation, n_bmp_rgb( 128,128,128 ) );

#endif // #ifdef N_POSIX_PLATFORM_MAC

//n_bmp_save_literal( &p->bmp_foundation, "ret.bmp" );
	}


	// [!] : digit

	i = 0;
	n_posix_loop
	{

		x = msx + osx + ( unit_sx * 0 ) - ( p->border * 2 );
		y = msy + osy + ( unit_sy * 0 ) - ( p->border * 2 );

		n_cardgenerator_rasterizer( p, &bmp_d1[ i ], &bmp_c[ i ], x,y, &bmp_gradient );


		x = msx + osx + ( unit_sx * 4 ) + ( p->border * 2 );
		y = msy + osy + ( unit_sy * 6 ) + ( p->border * 2 );

		n_cardgenerator_rasterizer( p, &bmp_d2[ i ], &bmp_c[ i ], x,y, &bmp_gradient );


		i++;
		if ( i >= N_CARDGENERATOR_CARD_UNIT ) { break; }
	}


	n_bmp_free_fast( &bmp_gradient );
	n_bmp_free_fast( &bmp_grad_big );


//if ( suit == 0 ) { n_bmp_save_literal( &bmp_c[ N_CARDGENERATOR_CARD_UNIT - 1 ], "256px_h.bmp" ); }
//if ( suit == 2 ) { n_bmp_save_literal( &bmp_c[ N_CARDGENERATOR_CARD_UNIT - 1 ], "256px_s.bmp" ); }


	// Resample

	i = 0;
	n_posix_loop
	{
#ifdef N_POSIX_PLATFORM_MAC

		n_bmp_mac( &bmp_c[ i ] );

#endif // #ifdef N_POSIX_PLATFORM_MAC


		n_cardgenerator_resampler( p, &bmp_c[ i ], p->color_frame );


		i++;
		if ( i >= N_CARDGENERATOR_CARD_UNIT ) { break; }
	}


	return;
}

#ifdef N_POSIX_PLATFORM_WINDOWS

DWORD WINAPI
n_cardgenerator_thread_0( LPVOID lpParameter )
{

	n_cardgenerator *cardgenerator = (void*) lpParameter;

	n_cardgenerator_card( cardgenerator, 0 );

	return 0;
}

DWORD WINAPI
n_cardgenerator_thread_1( LPVOID lpParameter )
{

	n_cardgenerator *cardgenerator = (void*) lpParameter;

	n_cardgenerator_card( cardgenerator, 1 );

	return 0;
}

DWORD WINAPI
n_cardgenerator_thread_2( LPVOID lpParameter )
{

	n_cardgenerator *cardgenerator = (void*) lpParameter;

	n_cardgenerator_card( cardgenerator, 2 );

	return 0;
}

DWORD WINAPI
n_cardgenerator_thread_3( LPVOID lpParameter )
{

	n_cardgenerator *cardgenerator = (void*) lpParameter;

	n_cardgenerator_card( cardgenerator, 3 );

	return 0;
}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

void
n_cardgenerator_init( n_cardgenerator *p, n_type_gfx piles )
{

	// Init

	p->is_init = n_posix_true;


#ifdef N_POSIX_PLATFORM_MAC

	if ( n_mac_image_window == nil )
	{
		p->backingScaleFactor = 1;
	} else {
		p->backingScaleFactor = n_mac_image_window.backingScaleFactor;
	}

#endif // #ifdef N_POSIX_PLATFORM_MAC


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


#ifdef N_POSIX_PLATFORM_MAC

	p->is_half = n_posix_false;

//NSLog( @"%f", p->unit_sx * p->resample );

	p->border = (n_type_gfx) n_posix_max_n_type_real( 2,  3 * p->resample );
	p->halo   = (n_type_gfx) n_posix_max_n_type_real( 6, 12 * p->resample );

#else  // #ifdef N_POSIX_PLATFORM_MAC

	p->is_half = n_posix_false;
	if ( 90 > ( p->unit_sx * p->resample ) )
	{
//NSLog( @"1" );
		p->is_half = n_posix_true;

		p->unit_sx  = (n_type_gfx) ( (n_type_real) N_CARDGENERATOR_UNIT / 2 );
		p->unit_sy  = (n_type_gfx) ( (n_type_real) p->unit_sx * 1.5 );
		p->resample = (n_type_real) ( desktop_sx / piles ) / p->unit_sx;

		p->border    =  1;
		p->halo      =  4;
	} else {
//NSLog( @"2" );
		p->border    = (n_type_gfx) (  3 * p->resample );
		p->halo      = (n_type_gfx) ( 12 * p->resample );
	}

//n_posix_debug_literal( " %f %d ", p->resample, p->border );

#endif // #ifdef N_POSIX_PLATFORM_MAC


	// Blank Card Maker

#ifdef N_POSIX_PLATFORM_MAC

	n_posix_bool dwm_onoff = n_posix_false;
	
#else   // #ifdef N_POSIX_PLATFORM_MAC

	n_posix_bool dwm_onoff = game.dwm_onoff;

#endif  // #ifdef N_POSIX_PLATFORM_MAC

	{

		const int blend   = 0;// p->option_blend;
		const u32 lighter = n_bmp_rgb( 222,222,222 );
		const u32 darker  = n_bmp_rgb(  50, 50, 50 );
		const u32 black   = n_bmp_rgb(  10, 10, 10 );
		const u32 white   = n_bmp_white;

		if ( dwm_onoff )
		{
			p->color_frame = black;
			n_cardgenerator_base( p, &p->bmp_blank, white , blend );
		} else
		if ( n_cardgenerator_darkmode_is_on() )
		{
			p->color_frame = n_win_darkmode_bg_argb;
			n_cardgenerator_base( p, &p->bmp_blank, darker, blend );
		} else {
			p->color_frame = lighter;
			n_cardgenerator_base( p, &p->bmp_blank, white , blend );
		}

	}

	if ( dwm_onoff )
	{
		p->color_margin = n_bmp_black_invisible;
	} else
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
		p->is_init = n_posix_false;
	} else {
		return;
	}


	// Load

#ifdef N_POSIX_PLATFORM_MAC

	n_mac_image_rc_load_bmp( @"180/digit"            , &p->bmp_font     );
	n_mac_image_rc_load_bmp( @"180/neko"             , &p->bmp_logo     );

	n_mac_image_rc_load_bmp( @"180/suit/hearts"      , &p->suit__1[ 0 ] );
	n_mac_image_rc_load_bmp( @"180/suit/diamonds"    , &p->suit__1[ 1 ] );
	n_mac_image_rc_load_bmp( @"180/suit/spades"      , &p->suit__1[ 2 ] );
	n_mac_image_rc_load_bmp( @"180/suit/clubs"       , &p->suit__1[ 3 ] );

	n_mac_image_rc_load_bmp( @"180/suit_big/hearts"  , &p->suitbig[ 0 ] );
	n_mac_image_rc_load_bmp( @"180/suit_big/diamonds", &p->suitbig[ 1 ] );
	n_mac_image_rc_load_bmp( @"180/suit_big/spades"  , &p->suitbig[ 2 ] );
	n_mac_image_rc_load_bmp( @"180/suit_big/clubs"   , &p->suitbig[ 3 ] );

	n_mac_image_rc_load_bmp( @"180/crown_k"          , &p->bmp_crown_k  );
	n_mac_image_rc_load_bmp( @"180/crown_q"          , &p->bmp_crown_q  );

#else  // #ifdef N_POSIX_PLATFORM_MAC


#ifdef N_POSIX_PLATFORM_WINDOWS

	if ( p->is_half == n_posix_false )
	{
		n_game_rc_load_bmp_literal( &p->bmp_font    , "NCARDGENERATOR_BMP_D"     );
		n_game_rc_load_bmp_literal( &p->bmp_logo    , "NCARDGENERATOR_BMP_NEKO"  );
		n_game_rc_load_bmp_literal( &p->suit__1[ 0 ], "NCARDGENERATOR_BMP_S_0"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 1 ], "NCARDGENERATOR_BMP_S_1"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 2 ], "NCARDGENERATOR_BMP_S_2"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 3 ], "NCARDGENERATOR_BMP_S_3"   );
		n_game_rc_load_bmp_literal( &p->suitbig[ 0 ], "NCARDGENERATOR_BMP_S_B_0" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 1 ], "NCARDGENERATOR_BMP_S_B_1" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 2 ], "NCARDGENERATOR_BMP_S_B_2" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 3 ], "NCARDGENERATOR_BMP_S_B_3" );
		n_game_rc_load_bmp_literal( &p->bmp_crown_k , "NCARDGENERATOR_BMP_C_K"   );
		n_game_rc_load_bmp_literal( &p->bmp_crown_q , "NCARDGENERATOR_BMP_C_Q"   );
	} else {
		n_game_rc_load_bmp_literal( &p->bmp_font    , "NCARDGENERATOR_BMP_90_D"     );
		n_game_rc_load_bmp_literal( &p->bmp_logo    , "NCARDGENERATOR_BMP_90_NEKO"  );
		n_game_rc_load_bmp_literal( &p->suit__1[ 0 ], "NCARDGENERATOR_BMP_90_S_0"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 1 ], "NCARDGENERATOR_BMP_90_S_1"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 2 ], "NCARDGENERATOR_BMP_90_S_2"   );
		n_game_rc_load_bmp_literal( &p->suit__1[ 3 ], "NCARDGENERATOR_BMP_90_S_3"   );
		n_game_rc_load_bmp_literal( &p->suitbig[ 0 ], "NCARDGENERATOR_BMP_90_S_B_0" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 1 ], "NCARDGENERATOR_BMP_90_S_B_1" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 2 ], "NCARDGENERATOR_BMP_90_S_B_2" );
		n_game_rc_load_bmp_literal( &p->suitbig[ 3 ], "NCARDGENERATOR_BMP_90_S_B_3" );
		n_game_rc_load_bmp_literal( &p->bmp_crown_k , "NCARDGENERATOR_BMP_90_C_K"   );
		n_game_rc_load_bmp_literal( &p->bmp_crown_q , "NCARDGENERATOR_BMP_90_C_Q"   );
	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_POSIX_PLATFORM_MAC


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

		n_bmp_transparent_onoff( &p->bmp_font, n_posix_false );

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

#ifdef N_POSIX_PLATFORM_MAC

	{

		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;

/*
		// [x] : broken image will be made

		dispatch_group_t group = dispatch_group_create();
		dispatch_queue_t queue = dispatch_get_main_queue();

		dispatch_async( queue, ^{ n_cardgenerator_card( p, 0 ); } );
		dispatch_async( queue, ^{ n_cardgenerator_card( p, 1 ); } );
		dispatch_async( queue, ^{ n_cardgenerator_card( p, 2 ); } );
		dispatch_async( queue, ^{ n_cardgenerator_card( p, 3 ); } );

		dispatch_group_notify( group, queue, ^{ ; } );
		dispatch_group_wait( group, DISPATCH_TIME_FOREVER );
*/

		NSOperationQueue *q = [[NSOperationQueue alloc] init];

		int i = 0;
		n_posix_loop
		{
			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{
				n_cardgenerator_card( p, i );
			}];

			[q addOperation:o];

			i++;
			if ( i >= 4 ) { break; }
		}

		[q waitUntilAllOperationsAreFinished];


		n_bmp_is_multithread = p_multithread;

	}

#else  // #ifdef N_POSIX_PLATFORM_MAC

	{

		// [x] : Win9x : can run but not working

		if ( n_thread_onoff() )
		{

			n_posix_bool p_multithread = n_bmp_is_multithread;
			n_bmp_is_multithread = n_posix_true;

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

#endif // #ifdef N_POSIX_PLATFORM_MAC


	if ( tick ) { n_posix_debug_literal( "%d", (int) n_posix_tickcount() - tick ); }


	return;
}

void
n_cardgenerator_exit( n_cardgenerator *p )
{

	int i = 0;
	n_posix_loop
	{

		n_bmp_free( &p->cards[ i ] );

		i++;
		if ( i >= N_CARDGENERATOR_CARD_ALL ) { break; }
	}

	i = 0;
	n_posix_loop
	{
		n_bmp_free( &p->digit_1[ i ] );
		n_bmp_free( &p->digit_2[ i ] );

		i++;
		if ( i >= N_CARDGENERATOR_CARD_UNIT ) { break; }
	}

	i = 0;
	n_posix_loop
	{

		n_bmp_free( &p->suit__1[ i ] );
		n_bmp_free( &p->suit__2[ i ] );

		n_bmp_free( &p->suitbig[ i ] );

		i++;
		if ( i >= N_CARDGENERATOR_SUIT_MAX ) { break; }
	}

	n_bmp_free( &p->bmp_blank      );
	n_bmp_free( &p->bmp_logo       );
	n_bmp_free( &p->bmp_font       );
	n_bmp_free( &p->bmp_foundation );
	n_bmp_free( &p->bmp_halo       );
	n_bmp_free( &p->bmp_crown_k    );
	n_bmp_free( &p->bmp_crown_q    );


	return;
}


#endif // _H_NONNON_PROJECT_CARDGENERATOR

