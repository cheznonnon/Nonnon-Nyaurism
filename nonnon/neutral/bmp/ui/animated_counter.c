// Nonnon Win
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_UI_ANIMATED_COUNTER
#define _H_NONNON_NEUTRAL_BMP_UI_ANIMATED_COUNTER




#include "../../../bridge/gdi.c"
#include "../../../bridge/transition.c"




//#define N_BMP_UI_ANIMATED_COUNTER_MAX ( 8 )




typedef struct {

	// [!] : in

	u32           msec;
	n_type_real   blend;


	// [!] : tweaker : use carefully

	int           figure;


	// [!] : instance

	n_bmp         digit[ 10 + 1 ];

	n_posix_char  str_old[ 15 ];
	n_posix_char  str_new[ 15 ];

	n_bmp        *bmp_old[ 10 ];
	n_bmp        *bmp_new[ 10 ];

	n_game_transition_struct t[ 10 ];


	// [!] : cache for speeding up

	n_bmp         bmp_cache;
	n_type_gfx    bmp_sx;
	n_type_gfx    bmp_sy;


	// [!] : out

	n_type_gfx    offset;

} n_bmp_ui_animated_counter;

#define n_bmp_ui_animated_counter_zero( p ) n_memory_zero( p, sizeof( n_bmp_ui_animated_counter ) )




/*
void
n_bmp_ui_animated_counter_debug( n_bmp_ui_animated_counter *p )
{

	int i = 0;
	n_posix_loop
	{

		n_posix_char path[ 100 ];
		n_posix_sprintf( path, "/Users/nonnon2/Desktop/%d.bmp", i );

		//n_bmp_save( &p->digit[ i ], path );
		n_bmp_save( p->bmp_new[ i ], path );

		i++;
		if ( i >= 11 ) { break; }
	}


	return;
}
*/

int
n_bmp_ui_animated_counter_figure_count( int count )
{

	// [!] : is there a better way?

	n_posix_char str[ 100 ];

	n_posix_sprintf_literal( str, "%d", count );
//NSLog( @"%s", str );

	return (int) n_posix_strlen( str );
}

void
n_bmp_ui_animated_counter_init( n_bmp_ui_animated_counter *p, n_gdi *gdi, u32 msec )
{

	p->msec   = msec;
	p->figure = 8;


	n_posix_char str[ 10 ];

	int i = 0;
	n_posix_loop
	{

		n_posix_sprintf_literal( str, "%d", i );
//NSLog( @"%s", str );

		// [x] : currently, cropper is buggy

		gdi->text_style = N_GDI_TEXT_MAC_NO_CROP;

		gdi->text = str;
		n_gdi_bmp( gdi, &p->digit[ i ] );
		gdi->text = NULL;

		n_gdi_crop( &p->digit[ i ], &p->digit[ i ] );

		i++;
		if ( i >= 10 ) { break; }
	}


	p->bmp_sx = N_BMP_SX( &p->digit[ 0 ] );
	p->bmp_sy = N_BMP_SY( &p->digit[ 0 ] );


	// [!] : Stub

	n_bmp_new_fast( &p->digit[ 10 ], p->bmp_sx,p->bmp_sy );
	n_bmp_flush   ( &p->digit[ 10 ],  gdi->base_color_bg );


	// [!] : Cached Canvas

	n_bmp_new_fast( &p->bmp_cache, p->bmp_sx,p->bmp_sy );


	return;
}

void
n_bmp_ui_animated_counter_loop_count( n_bmp_ui_animated_counter *p, int count )
{
//NSLog( @"%f", pow( 10, 8 ) );

	if ( count >= pow( 10, p->figure ) ) { count = pow( 10, p->figure ) - 1; }


	n_posix_sprintf( p->str_old, "%s", p->str_new );
	n_posix_sprintf( p->str_new, "%*d", p->figure, count );
//NSLog( @"%s %s", p->str_old, p->str_new );

	int i = 0;
	n_posix_loop
	{

		p->t[ i ].percent = 0;

		if ( ( p->str_old[ i ] == N_STRING_CHAR_NUL )||( p->str_old[ i ] == N_STRING_CHAR_SPACE ) )
		{
			p->bmp_old[ i ] = &p->digit[ 10 ];
		} else {
			int n = p->str_old[ i ] - '0';
			if ( n < 0 ) { n = 10; }

			p->bmp_old[ i ] = &p->digit[ n ];
		}

		if ( ( p->str_new[ i ] == N_STRING_CHAR_NUL )||( p->str_new[ i ] == N_STRING_CHAR_SPACE ) )
		{
			p->bmp_new[ i ] = &p->digit[ 10 ];
		} else {
			int n = p->str_new[ i ] - '0';
			if ( n < 0 ) { n = 10; }
//NSLog( @"%c : %d %d", p->str_new[ i ], i, n );

			p->bmp_new[ i ] = &p->digit[ n ];
		}

		i++;
		if ( i >= p->figure ) { break; }
	}


	return;
}

BOOL
n_bmp_ui_animated_counter_loop_draw( n_bmp_ui_animated_counter *p, n_bmp *canvas, n_type_gfx x, n_type_gfx y )
{

	BOOL is_running = FALSE;


	n_type_gfx fx = 0;

	int i = 0;
	n_posix_loop
	{
//n_bmp_flush( &bmp2, n_game_randomcolor() );


		BOOL ret = n_game_transition_main
		(
			&p->t[ i ],
			&p->bmp_cache,
			 p->bmp_old[ i ],
			 p->bmp_new[ i ],
			 p->msec,
			 N_GAME_TRANSITION_SCROLL_U
		);

//if ( p->t[ i ].error_code ) { if ( p->t[ i ].error_code != 6 ) { NSLog( @"%d %d", i, p->t[ i ].error_code ); } }

		if ( ret )
		{

			p->t[ i ].percent = 100;
//NSLog( @"Stop %d %f", i, p->t[ i ].percent );

			n_bmp_blendcopy
			(
				p->bmp_new[ i ], canvas,
				0,0, p->bmp_sx,p->bmp_sy,
				x + fx, y,
				p->blend
			);

		} else {

			is_running = TRUE;

			n_bmp_blendcopy
			(
				&p->bmp_cache, canvas,
				0,0, p->bmp_sx,p->bmp_sy,
				x + fx, y,
				p->blend
			);

		}
//if ( i == 8 ) { NSLog( @"%f", p->percent[ i ] ); }

		if ( ( p->bmp_old[ i ] == &p->digit[ 10 ] )&&( p->bmp_new[ i ] == &p->digit[ 10 ] ) )
		{
			p->offset = fx;
		}

		fx += p->bmp_sx;


		i++;
		if ( i >= p->figure ) { break; }
	}


	return is_running;
}




#endif // _H_NONNON_NEUTRAL_BMP_UI_ANIMATED_COUNTER

