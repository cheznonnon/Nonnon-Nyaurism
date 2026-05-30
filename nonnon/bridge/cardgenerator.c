// Nonnon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_BRIDGE_CARDGENERATOR
#define _H_NONNON_BRIDGE_CARDGENERATOR




#include "../../nonnon/neutral/bmp/all.c"


#include "../../nonnon/bridge/gdi.c"




n_type_real
n_game_centering_real( n_type_real a, n_type_real b )
{

	// [!] : ( ( a / 2 ) - ( b / 2 ) ) == ( ( a - b ) / 2 )

	return ( a - b ) / 2;
}

n_type_gfx
n_game_centering( n_type_gfx a, n_type_gfx b )
{

	// [!] : ( ( a / 2 ) - ( b / 2 ) ) == ( ( a - b ) / 2 )

	return ( a - b ) / 2;
}




#define N_CARDGENERATOR_FONT_NAME n_posix_literal( "Trebuchet MS" )




void
n_cardgenerator_flush_stencil_copy( n_bmp *bmp, n_bmp *bg, n_bmp *ret, u32 chroma_key )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color;

		n_bmp_ptr_get_fast( bmp, x, y, &color );

		//if ( color == chroma_key )
		if ( n_bmp_color_similarity( color, chroma_key, 128 ) )
		{
			u32 t;
			n_bmp_ptr_get_fast( bg, x, y, &t );

			int a = n_bmp_a( color );
			int r = n_bmp_r( t );
			int g = n_bmp_g( t );
			int b = n_bmp_b( t );

			t = n_bmp_argb( a,r,g,b );
			n_bmp_ptr_set_fast( ret, x, y,  t );
		} else {
			n_bmp_ptr_set_fast( ret, x, y,  color );
		}


		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	return;
}

void
n_cardgenerator_flush_for_rasterizer( n_bmp *bmp, u32 color_bg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx  x = 0;
	n_type_gfx  y = 0;
	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		if ( color == color_bg )
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




#ifdef N_POSIX_PLATFORM_MAC

BOOL
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


#include "../win64/_windows.c"
#include "../win64/resource.c"


BOOL
n_cardgenerator_darkmode_is_on( void )
{
	return n_win64_darkmode_is_on();
}

void
n_cardgenerator_desktop_size( n_type_gfx *desktop_sx, n_type_gfx *desktop_sy )
{

	n_type_gfx sx = GetSystemMetrics( SM_CXMAXIMIZED );
	n_type_gfx sy = GetSystemMetrics( SM_CYMAXIMIZED );

	if ( desktop_sx != NULL ) {  (*desktop_sx) = sx; }
	if ( desktop_sy != NULL ) {  (*desktop_sy) = sy; }


	return;
}


#endif // #ifdef N_POSIX_PLATFORM_MAC




#define n_cardgenerator_gdi_literal( b, s, z ) n_cardgenerator_gdi( b, n_posix_literal( s ), z )

void
n_cardgenerator_gdi( n_bmp *bmp, n_posix_char *str, n_type_gfx size )
{

	n_gdi gdi; n_gdi_zero( &gdi );


	gdi.base_color_bg       = n_bmp_black;

	gdi.text                = str;
	gdi.text_font           = N_CARDGENERATOR_FONT_NAME;
	gdi.text_size           = size;
	gdi.text_style          = N_GDI_TEXT_SYS_SMOOTH;
	gdi.text_color_main     = n_bmp_white;


	n_bmp_free( bmp );
	n_gdi_bmp( &gdi, bmp );


	return;
}

#define n_cardgenerator_gdi_bg_literal( b, s, z, bg, fg ) n_cardgenerator_gdi_bg( b, n_posix_literal( s ), z, bg, fg )

void
n_cardgenerator_gdi_bg( n_bmp *bmp, n_posix_char *str, n_type_gfx size, u32 bg, u32 fg )
{

	n_gdi gdi; n_gdi_zero( &gdi );


	gdi.base_color_bg       = bg;

	gdi.text                = str;
	gdi.text_font           = N_CARDGENERATOR_FONT_NAME;
	gdi.text_size           = size;
	gdi.text_style          = N_GDI_TEXT_SYS_SMOOTH;
	gdi.text_color_main     = fg;


	n_bmp_free( bmp );
	n_gdi_bmp( &gdi, bmp );


	return;
}




#define N_CARDGENERATOR_SUIT_HEARTS   ( 0 )
#define N_CARDGENERATOR_SUIT_DIAMONDS ( 1 )
#define N_CARDGENERATOR_SUIT_SPADES   ( 2 )
#define N_CARDGENERATOR_SUIT_CLUBS    ( 3 )
#define N_CARDGENERATOR_SUIT_MAX      ( 4 )

#define N_CARDGENERATOR_CARD_UNIT     ( 13 )
#define N_CARDGENERATOR_CARD_ALL      ( N_CARDGENERATOR_CARD_UNIT * N_CARDGENERATOR_SUIT_MAX )

#define N_CARDGENERATOR_UNIT          ( 180 )




typedef struct {

	BOOL is_init;

	n_type_gfx unit_sx, unit_sy;
	n_type_gfx card_sx, card_sy;
	n_type_gfx csx_min, csy_min;
	n_type_gfx csx    , csy    ;
	n_type_gfx osx    , osy    ;
	n_type_gfx border;
	n_type_gfx halo;

	u32    color_margin;
	u32    color_frame;
	u32    color_card_main;

	n_type_real resample;

	n_bmp  cards  [ N_CARDGENERATOR_CARD_ALL  ];
	n_bmp  digit_1[ N_CARDGENERATOR_CARD_UNIT ];
	n_bmp  digit_2[ N_CARDGENERATOR_CARD_UNIT ];
	n_bmp  suit__1[ N_CARDGENERATOR_SUIT_MAX  ];
	n_bmp  suit__2[ N_CARDGENERATOR_SUIT_MAX  ];
	n_bmp  suitbig[ N_CARDGENERATOR_SUIT_MAX  ];
	n_bmp  suit_bg[ N_CARDGENERATOR_SUIT_MAX  ];

	n_bmp  bmp_blank;
	n_bmp  bmp_logo;
	n_bmp  bmp_font;
	n_bmp  bmp_foundation;
	n_bmp  bmp_halo;
	n_bmp  bmp_crown_k;
	n_bmp  bmp_crown_q;

} n_cardgenerator;


#define n_cardgenerator_zero( p ) n_memory_zero( p, sizeof( n_cardgenerator ) )




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


	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];

		if ( ( color != n_bmp_trans )&&( 0 != n_bmp_a( color ) ) )
		{
			color = n_bmp_grayscale_pixel( color );

			int alpha = n_bmp_a( color );

			if ( FALSE == n_cardgenerator_darkmode_is_on() )
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


	i = 0;
	n_posix_loop
	{

		n_bmp_flush_antialias( bmp, 1.0 );

		i++;
		if ( i >= m ) { break; }
	}

//n_bmp_save_literal( bmp, "ret.bmp" );

	return;
}




void
n_cardgenerator_resampler( n_cardgenerator *p, n_bmp *bmp, u32 color_frame )
{
//return;

	//if ( p->resample < 1.0 ) { n_bmp_flush_antialias( bmp, 1.0 - p->resample ); }

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

		n_bmp_rasterizer( &frame, bmp, 0,0, color_frame );

		n_bmp_cornermask( bmp, -25, 0, p->color_margin );

		n_bmp_free_fast( &frame );

	}


	return;
}

void
n_cardgenerator_outline( n_cardgenerator *p, n_bmp *bmp_f, n_bmp *bmp_t, n_type_gfx x, n_type_gfx y )
{
//return;

	n_type_gfx m = p->border;


	n_type_gfx fx = -m;
	n_type_gfx fy = -m;
	n_posix_loop
	{

		n_bmp_rasterizer_main( bmp_f, bmp_t, x + fx, y + fy, NULL, n_bmp_white );


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
		n_cardgenerator_outline( p, bmp_f, bmp_t, tx,ty );
	}

	n_bmp_rasterizer_main( bmp_f, bmp_t, tx,ty, bmp_grad, 0 );


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

	n_bmp bmp_bg; n_bmp_carboncopy( bmp, &bmp_bg );

	int gradient = N_BMP_GRADIENT_CIRCLE | N_BMP_GRADIENT_CENTERING;
	n_bmp_flush_gradient( &bmp_bg, color2, color1, gradient );

	n_bmp_carboncopy( bmp, bmp_ret );
	n_cardgenerator_flush_stencil_copy( bmp, &bmp_bg, bmp_ret, n_bmp_black );

	n_bmp_flush_antialias( bmp_ret, 0.5 );

	n_bmp_free_fast( &bmp_bg );


	return;
}

void
n_cardgenerator_logo_rasterizer( n_cardgenerator *p, n_bmp *bmp_f, n_bmp *bmp_t, u32 color1, u32 color2, n_type_gfx x, n_type_gfx y, int number )
{
//return;

	n_bmp b; n_bmp_zero( &b ); n_cardgenerator_neko( p, &p->bmp_logo, color1, color2, &b, number );
//n_bmp_save_literal( &b, "ret.bmp" );


	{
		n_bmp bmp_r; n_bmp_carboncopy( &p->bmp_logo, &bmp_r );
		n_cardgenerator_flush_for_rasterizer( &bmp_r, n_bmp_white_invisible );

		n_bmp_flush_antialias( &bmp_r, 1.0 );
		n_bmp_flush_antialias( &bmp_r, 1.0 );
		n_bmp_flush_antialias( &bmp_r, 1.0 );
		n_bmp_flush_antialias( &bmp_r, 1.0 );

		n_cardgenerator_outline( p, &bmp_r, bmp_t, x,y );
//n_bmp_save_literal( bmp_t, "ret.bmp" );

		n_bmp_free_fast( &bmp_r );
	}


	const n_type_gfx sx = N_BMP_SX( &p->bmp_logo );
	const n_type_gfx sy = N_BMP_SX( &p->bmp_logo );

	{
		n_bmp tmp; n_bmp_carboncopy( &b, &tmp );
		n_bmp_fill( &tmp, 0,0, n_bmp_white_invisible );

		n_bmp_flush_antialias( &tmp, 1.0 );
		n_bmp_flush_antialias( &tmp, 1.0 );
		n_bmp_flush_antialias( &tmp, 1.0 );
		n_bmp_flush_antialias( &tmp, 1.0 );

		n_bmp_transcopy( &tmp, bmp_t, 0,0,sx,sy, x,y );
//n_bmp_save_literal( &tmp, "ret.bmp" );

		n_bmp_free_fast( &tmp );
	}


	n_bmp_transcopy( &b, bmp_t, 0,0,sx,sy, x,y );
//n_bmp_save_literal( &b, "ret.bmp" );

	n_bmp_free_fast( &b );


	if ( number == 12 )
	{
		n_bmp_transcopy( &p->bmp_crown_q, bmp_t, 0,0,sx,sy, x,y );
	} else
	if ( number == 13 )
	{
		n_bmp_transcopy( &p->bmp_crown_k, bmp_t, 0,0,sx,sy, x,y );
	}


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
	n_bmp  *bmp_s1 = &p->suit__1[ suit ];
	n_bmp  *bmp_s2 = &p->suit__2[ suit ];
	n_bmp  *bmpbig = &p->suitbig[ suit ];
	n_bmp  *bmp_bg = &p->suit_bg[ suit ];


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

	// [!] : big suit background

	{
		n_type_gfx xx = n_game_centering( N_BMP_SX( &bmp_c[ 0 ] ), N_BMP_SX( bmp_bg ) );
		n_type_gfx yy = n_game_centering( N_BMP_SY( &bmp_c[ 0 ] ), N_BMP_SY( bmp_bg ) );

		n_bmp_rasterizer_main( bmp_bg, &bmp_c[ 11 - 1 ], xx,yy, bmp_bg, 0 );
		n_bmp_rasterizer_main( bmp_bg, &bmp_c[ 12 - 1 ], xx,yy, bmp_bg, 0 );
		n_bmp_rasterizer_main( bmp_bg, &bmp_c[ 13 - 1 ], xx,yy, bmp_bg, 0 );
	}

	x = n_game_centering( N_BMP_SX( &bmp_c[ 0 ] ), N_BMP_SX( &p->bmp_logo ) );
	y = n_game_centering( N_BMP_SY( &bmp_c[ 0 ] ), N_BMP_SY( &p->bmp_logo ) );

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

		//if ( p->resample < 1.0 ) { n_bmp_flush_antialias( &p->bmp_foundation, 1.0 - p->resample ); }
		n_bmp_resampler( &p->bmp_foundation, p->resample,p->resample );

#else  // #ifdef N_POSIX_PLATFORM_MAC

		u32 lighter = n_bmp_rgb( 222,222,222 );
		u32 dark    = n_bmp_rgb( 100,100,100 );

		if ( n_cardgenerator_darkmode_is_on() )
		{
			dark    = n_bmp_rgb(  66, 66, 66 );
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

#ifdef N_CARDGENERATOR_DIGIT_PATCH
		x += ( N_BMP_SX( &bmp_d1[ 10 - 1 ] ) - N_BMP_SX( &bmp_d1[ i ] ) ) / 2;
#endif
		n_cardgenerator_rasterizer( p, &bmp_d1[ i ], &bmp_c[ i ], x,y, &bmp_gradient );

		n_bmp_flush_mirror( &bmp_gradient, N_BMP_MIRROR_ROTATE180 );
		n_bmp_flush_mirror( &bmp_c[ i ]  , N_BMP_MIRROR_ROTATE180 );

		n_cardgenerator_rasterizer( p, &bmp_d1[ i ], &bmp_c[ i ], x,y, &bmp_gradient );

		n_bmp_flush_mirror( &bmp_gradient, N_BMP_MIRROR_ROTATE180 );
		n_bmp_flush_mirror( &bmp_c[ i ]  , N_BMP_MIRROR_ROTATE180 );

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

	n_cardgenerator_zero( p );


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




#endif // _H_NONNON_BRIDGE_CARDGENERATOR

