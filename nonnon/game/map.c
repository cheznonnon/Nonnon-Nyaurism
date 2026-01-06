// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GAME_MAP
#define _H_NONNON_WIN32_GAME_MAP




#include "../neutral/bmp/all.c"




#define N_GAME_MAP_MODE_STOP  ( 0      )
#define N_GAME_MAP_MODE_XLOOP ( 1 << 0 )
#define N_GAME_MAP_MODE_YLOOP ( 1 << 1 )
#define N_GAME_MAP_MODE_TRANS ( 1 << 2 )
#define N_GAME_MAP_MODE_LOOP  ( N_GAME_MAP_MODE_XLOOP | N_GAME_MAP_MODE_YLOOP )

typedef struct {

	n_bmp      *main;

	n_bmp       map, chip;

	n_type_gfx  x,y,sx,sy;
	n_type_gfx  unit_sx, unit_sy;

	int         mode;

	// internal : cache

	u32         chip_max;
	n_type_gfx  chip_sx, chip_sy;

} n_game_map;


static n_posix_bool n_game_map_safemode = n_posix_false;




#define n_game_map_chip_get( data    ) n_bmp_b( data )
#define n_game_map_prm1_get( data    ) n_bmp_g( data )
#define n_game_map_prm2_get( data    ) n_bmp_r( data )
#define n_game_map_prm3_get( data    ) n_bmp_a( data )

#define n_game_map_chip_set( data, n ) ( n_game_map_chip_get( data ) = n )
#define n_game_map_prm1_set( data, n ) ( n_game_map_prm1_get( data ) = n )
#define n_game_map_prm2_set( data, n ) ( n_game_map_prm2_get( data ) = n )
#define n_game_map_prm3_set( data, n ) ( n_game_map_prm3_get( data ) = n )


void
n_game_map_exit( n_game_map *map )
{

	if ( n_game_map_safemode )
	{
		if ( map == NULL ) { return; }
	}


	n_bmp_free( &map->map  );
	n_bmp_free( &map->chip );

	n_memory_zero( map, sizeof( n_game_map ) );


	return;
}

void
n_game_map_init
(
	n_game_map   *map,
	n_posix_char *name_map,
	n_posix_char *name_chip,
	n_type_gfx    unit_sx,
	n_type_gfx    unit_sy,
	int           mode,
	n_bmp        *main
)
{

	n_game_map_exit( map );


	if ( n_game_map_safemode )
	{
		if ( map       == NULL ) { return; }
		if ( name_map  == NULL ) { return; }
		if ( name_chip == NULL ) { return; }
		if ( main      == NULL ) { return; }
	}


	// [!] : Loading Order : RC_BMP => RC_PNG => FILE_BMP

	n_posix_bool ret_m = n_posix_true;
	n_posix_bool ret_c = n_posix_true;

#ifdef _H_NONNON_WIN32_GAME_RC


	if ( ret_m ) { ret_m = n_game_rc_load_bmp( &map->map,  name_map  ); }
	if ( ret_c ) { ret_c = n_game_rc_load_bmp( &map->chip, name_chip ); }

#ifdef _H_NONNON_NEUTRAL_PNG

	if ( ret_m ) { ret_m = n_game_rc_load_png2bmp( &map->map,  name_map  ); }
	if ( ret_c ) { ret_c = n_game_rc_load_png2bmp( &map->chip, name_chip ); }

#endif // #ifdef _H_NONNON_NEUTRAL_PNG


#endif // #ifdef _H_NONNON_WIN32_GAME_RC

	if ( ret_m ) { ret_m = n_bmp_load( &map->map,  name_map  ); }
	if ( ret_c ) { ret_c = n_bmp_load( &map->chip, name_chip ); }


	map->unit_sx  = n_posix_max_n_type_gfx( 1, unit_sx );
	map->unit_sy  = n_posix_max_n_type_gfx( 1, unit_sy );

	if ( ret_m ) { n_bmp_new( &map->map,  map->unit_sx, map->unit_sy ); }
	if ( ret_c ) { n_bmp_new( &map->chip, map->unit_sx, map->unit_sy ); }

	map->main     = main;
	map->sx       = n_posix_max_n_type_gfx( 1, N_BMP_SX( &map->map  ) * map->unit_sx );
	map->sy       = n_posix_max_n_type_gfx( 1, N_BMP_SY( &map->map  ) * map->unit_sy );
	map->mode     = mode;
	map->chip_sx  = n_posix_max_n_type_gfx( 1, N_BMP_SX( &map->chip ) / map->unit_sx );
	map->chip_sy  = n_posix_max_n_type_gfx( 1, N_BMP_SY( &map->chip ) / map->unit_sy );
	map->chip_max = map->chip_sx * map->chip_sy;


	return;
}

void
n_game_map_draw( n_game_map *m, u32 chip_offset, n_posix_bool layer )
{

	if ( n_game_map_safemode )
	{
		if ( m          == NULL ) { return; }
		if ( m->main    == NULL ) { return; }
		if ( m->unit_sx ==    0 ) { return; }
		if ( m->unit_sy ==    0 ) { return; }
		if ( m->chip_sx ==    0 ) { return; }
		if ( m->chip_sy ==    0 ) { return; }
	}


	const n_posix_bool xloop_onoff = ( m->mode & N_GAME_MAP_MODE_XLOOP );
	const n_posix_bool yloop_onoff = ( m->mode & N_GAME_MAP_MODE_YLOOP );

	const n_type_gfx bmpsx = N_BMP_SX( m->main );
	const n_type_gfx bmpsy = N_BMP_SY( m->main );
	const n_type_gfx mapsx = N_BMP_SX( &m->map );
	const n_type_gfx mapsy = N_BMP_SY( &m->map );


	n_type_gfx max_x = 0;
	n_type_gfx max_y = 0;

	if ( m->sx > bmpsx ) { max_x = m->sx - bmpsx; }
	if ( m->sy > bmpsy ) { max_y = m->sy - bmpsy; }

	if ( xloop_onoff )
	{
		if ( m->x < 0 ) { m->x += m->sx; } else if ( m->x >= m->sx ) { m->x -= m->sx; }
	} else {
		if ( m->x < 0 ) { m->x  =     0; } else if ( m->x >  max_x ) { m->x  = max_x; }
	}

	if ( yloop_onoff )
	{
		if ( m->y < 0 ) { m->y += m->sy; } else if ( m->y >= m->sy ) { m->y -= m->sy; }
	} else {
		if ( m->y < 0 ) { m->y  =     0; } else if ( m->y >  max_y ) { m->y  = max_y; }
	}


	// [!] : modulo is very slow

	static n_type_gfx p_x[ 4 ] = { 0,0,0,0 };
	static n_type_gfx p_y[ 4 ] = { 0,0,0,0 };

	if ( ( p_x[ 0 ] != m->x )||( p_x[ 1 ] != m->unit_sx ) )
	{
		p_x[ 0 ] = m->x;
		p_x[ 1 ] = m->unit_sx;
		p_x[ 2 ] = p_x[ 0 ] / p_x[ 1 ];
		p_x[ 3 ] = p_x[ 0 ] - ( p_x[ 1 ] * p_x[ 2 ] );
	}

	if ( ( p_y[ 0 ] != m->y )||( p_y[ 1 ] != m->unit_sy ) )
	{
		p_y[ 0 ] = m->y;
		p_y[ 1 ] = m->unit_sy;
		p_y[ 2 ] = p_y[ 0 ] / p_y[ 1 ];
		p_y[ 3 ] = p_y[ 0 ] - ( p_y[ 1 ] * p_y[ 2 ] );
	}


	n_type_gfx x  =  p_x[ 2 ];
	n_type_gfx xx = -p_x[ 3 ];
	n_type_gfx y  =  p_y[ 2 ];
	n_type_gfx yy = -p_y[ 3 ];

//n_game_hwndprintf_literal( "%d %d : %d %d", x, y, xx, yy );


	// Centering

	if ( xloop_onoff == n_posix_false )
	{
		if ( m->sx < bmpsx ) { xx += ( bmpsx - m->sx ) / 2; }
	}
	if ( yloop_onoff == n_posix_false )
	{
		if ( m->sy < bmpsy ) { yy += ( bmpsy - m->sy ) / 2; }
	}


	u32        p_number = 0;
	n_type_gfx fx       = 0;
	n_type_gfx fy       = 0;


	n_type_gfx startx  =  x;
	n_type_gfx startxx = xx;

	n_posix_loop
	{

		if ( xloop_onoff )
		{
			if ( x < 0 ) { x += mapsx; } else if ( x >= mapsx ) { x -= mapsx; }
		}
		if ( yloop_onoff )
		{
			if ( y < 0 ) { y += mapsy; } else if ( y >= mapsy ) { y -= mapsy; }
		}


		u32 number; n_bmp_ptr_get_fast( &m->map, x,y, &number );

		number = n_game_map_chip_get( number ) + chip_offset;

		if ( number >= m->chip_max ) { number = chip_offset; }


		if ( ( layer == n_posix_false )||( number != 0 ) )
		{

			if ( p_number != number )
			{

				n_type_gfx n_div = ( number / m->chip_sx );
				n_type_gfx n_mod = ( number - ( n_div * m->chip_sx ) );

				fx = n_mod * m->unit_sx;
				fy = n_div * m->unit_sy;

			}

			if ( m->mode & N_GAME_MAP_MODE_TRANS )
			{
				n_bmp_transcopy( &m->chip, m->main, fx,fy, m->unit_sx, m->unit_sy, xx, yy );
			} else {
				n_bmp_fastcopy ( &m->chip, m->main, fx,fy, m->unit_sx, m->unit_sy, xx, yy );
			}

		}


		p_number = number;


		x++;
		xx += m->unit_sx;

		if (
			( xx >= bmpsx )
			||
			(
				( xloop_onoff == n_posix_false )
				&&
				( x >= mapsx )
			)
		)
		{

			 x = startx;
			xx = startxx;


			y++;
			yy += m->unit_sy;

			if (
				( yy >= bmpsy )
				||
				(
					( yloop_onoff == n_posix_false )
					&&
					( y >= mapsy )
				)
			)
			{
				break;
			}
		}

	}


	return;
}

void
n_game_map_move( n_game_map *m, n_type_gfx cell_x, n_type_gfx cell_y )
{

	if ( n_game_map_safemode )
	{
		if ( m == NULL ) { return; }
	}


	m->x = cell_x * m->unit_sx;
	m->y = cell_y * m->unit_sy;


	return;
}

#define n_game_map_get( m, x,y       ) n_game_map_getset( m, x,y,    0, n_posix_true  )
#define n_game_map_set( m, x,y, data ) n_game_map_getset( m, x,y, data, n_posix_false )

u32
n_game_map_getset( n_game_map *m, n_type_gfx x, n_type_gfx y, u32 data, n_posix_bool is_get )
{

	if ( n_game_map_safemode )
	{
		if ( m          == NULL ) { return 0; }
		if ( m->unit_sx ==    0 ) { return 0; }
		if ( m->unit_sy ==    0 ) { return 0; }
	}


	// Loop : always process for map_chara.c

	if ( x < 0 ) { x += m->sx; } else if ( x >= m->sx ) { x -= m->sx; }
	if ( y < 0 ) { y += m->sy; } else if ( y >= m->sy ) { y -= m->sy; }


	static n_type_gfx p_x[ 3 ] = { 0,0,0 };
	static n_type_gfx p_y[ 3 ] = { 0,0,0 };


	if ( ( p_x[ 0 ] != x )||( p_x[ 1 ] != m->unit_sx ) )
	{
		p_x[ 0 ] = x;
		p_x[ 1 ] = m->unit_sx;
		p_x[ 2 ] = p_x[ 0 ] / p_x[ 1 ];
	}

	if ( ( p_y[ 0 ] != y )||( p_y[ 1 ] != m->unit_sy ) )
	{
		p_y[ 0 ] = y;
		p_y[ 1 ] = m->unit_sy;
		p_y[ 2 ] = p_y[ 0 ] / p_y[ 1 ];
	}

	x = p_x[ 2 ];
	y = p_y[ 2 ];


	if ( is_get )
	{
		n_bmp_ptr_get_fast( &m->map, x,y, &data );
	} else {
		n_bmp_ptr_set_fast( &m->map, x,y,  data );
	}


	return data;
}


#endif // _H_NONNON_WIN32_GAME_MAP

