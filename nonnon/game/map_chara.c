// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism] : auto-moving objects or NPC(Non-Player Character)
//
//	n_game_chara.x/.y have the range between n_game_map.x/.y to .sx/.sy
//
//		n_game_chara_draw() doesn't work
//		use n_game_map_chara_draw() instead
//
//
//	focus and moving are separated
//
//		player-followed camera work is like this
//
//		1 : n_game_map_chara_autofocus()
//		2 : n_game_map_draw()
//		3 : n_game_map_chara_draw()


// [Mechanism] : n_game_map_chara_fit_detect_*() : auto-detection
//
//	[ left <=> right detection ]
//
//		a character bitmap needs to have center-line pixels at bottom-most line




#ifndef _H_NONNON_WIN32_GAME_MAP_CHARA
#define _H_NONNON_WIN32_GAME_MAP_CHARA




#include "./chara.c"
#include "./map.c"
#include "./game.c"




#define N_GAME_MAP_CHARA_NOMOVE            ( 0 << 0 )
#define N_GAME_MAP_CHARA_UP                ( 1 << 0 )
#define N_GAME_MAP_CHARA_DOWN              ( 1 << 1 )
#define N_GAME_MAP_CHARA_LEFT              ( 1 << 2 )
#define N_GAME_MAP_CHARA_RIGHT             ( 1 << 3 )
#define N_GAME_MAP_CHARA_UD                ( N_GAME_MAP_CHARA_UP   | N_GAME_MAP_CHARA_DOWN  )
#define N_GAME_MAP_CHARA_LR                ( N_GAME_MAP_CHARA_LEFT | N_GAME_MAP_CHARA_RIGHT )
#define N_GAME_MAP_CHARA_UDLR              ( N_GAME_MAP_CHARA_UD   | N_GAME_MAP_CHARA_LR    )

#define N_GAME_MAP_CHARA_DASH_OFF          0
#define N_GAME_MAP_CHARA_DASH_ON           1

#define N_GAME_MAP_CHARA_JUMP_OFF          0
#define N_GAME_MAP_CHARA_JUMP_ON_RISE      1
#define N_GAME_MAP_CHARA_JUMP_ON_FALL      2
#define N_GAME_MAP_CHARA_JUMP_ON_FREEFALL  3


typedef struct {

	n_game_map   *m;
	n_game_chara *c;


	s32 redraw;

	s32 lr;

	s32 dash;
	s32 dash_cur;
	s32 dash_min;
	s32 dash_max;
	s32 dash_step;
	s32 dash_srcx;
	s32 dash_frame;
	s32 dash_frame_msec;
	u32 dash_frame_timer;
	s32 slip_srcx;

	s32 jump;
	s32 jump_cur;
	s32 jump_max;
	s32 jump_step;
	s32 jump_srcx;

} n_game_map_chara;


// [!] : for smooth animation

extern n_game sys;


// [!] : error handler on/off

extern n_posix_bool n_game_map_safemode;


// [!] : you need to implement this
//
//	n_posix_false = no block
//	n_posix_true  =    block

extern n_posix_bool n_game_map_chara_is_block( u32 );




#define N_GAME_MAP_CHARA_LOOP_STOP 0
#define N_GAME_MAP_CHARA_LOOP_LOOP 1

void
n_game_map_chara_loop( n_game_map_chara *mc, int offscreen )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	s32 sx    = c->sx - ( c->mx / 2 );
	s32 sy    = c->sy - ( c->my / 2 );
	s32 min_x = -c->mx;
	s32 min_y = -c->my;
	s32 max_x = m->sx - sx;
	s32 max_y = m->sy - sy;


	if ( offscreen == N_GAME_MAP_CHARA_LOOP_STOP )
	{

		if ( c->x < min_x )
		{
			c->x = min_x;
		} else
		if ( c->x > max_x )
		{
			c->x = m->x = max_x;
		}
		if ( c->y < min_y )
		{
			c->y = min_y;
		} else
		if ( c->y > max_y )
		{
			c->y = m->y = max_y;
		}

	} else
	if ( offscreen == N_GAME_MAP_CHARA_LOOP_LOOP )
	{

		if ( mc->lr == N_GAME_MAP_CHARA_LEFT )
		{
			if ( c->x < min_x ) { c->x += m->sx; }
		} else
		if ( mc->lr == N_GAME_MAP_CHARA_RIGHT )
		{
			if ( c->x > max_x ) { c->x -= m->sx; }
		}
		if ( mc->jump == N_GAME_MAP_CHARA_JUMP_ON_RISE )
		{
			if ( c->y < min_y ) { c->y += m->sy; }
		} else
		if ( mc->jump != N_GAME_MAP_CHARA_JUMP_OFF )
		{
			if ( c->y > max_y ) { c->y -= m->sy; }
		}

	}


	return;
}

void
n_game_map_chara_autofocus( n_game_map_chara *mc, int offscreen )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	n_game_map_chara_loop( mc, offscreen );

	m->x = c->x + ( ( c->sx - game.sx ) / 2 );
	m->y = c->y + ( ( c->sy - game.sy ) / 2 );


	return;
}

n_posix_bool
n_game_map_chara_onscreen( n_game_map_chara *mc )
{

	// [x] : currently buggy

	return n_posix_true;


	// [!]
	//
	//	currently, margin is not processed
	//	but maybe no problem

	// [!]
	//
	//	don't use n_game_map_chara_autofocus() with many objects
	//	n_posix_true will be returned always


	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return n_posix_false; }
		if ( mc->m == NULL ) { return n_posix_false; }
		if ( mc->c == NULL ) { return n_posix_false; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	s32 a_fx = m->x;
	s32 a_fy = m->y;
	s32 a_tx = m->x + game.sx;
	s32 a_ty = m->y + game.sy;

	s32 b_fx = c->x;
	s32 b_fy = c->y;
	s32 b_tx = c->x + c->sx;
	s32 b_ty = c->y + c->sy;

//n_game_hwndprintf_literal( "%d %d %d %d", a_fx, a_fy, a_tx, a_ty );
//n_game_hwndprintf_literal( "%d %d %d %d", b_fx, b_fy, b_tx, b_ty );


	if ( m->mode & N_GAME_MAP_MODE_LOOP )
	{
		if ( b_tx < a_fx )
		{
			a_fx  = 0;
			a_tx -= m->sx;
		}
		if ( b_ty < a_fy )
		{
			a_fy  = 0;
			a_ty -= m->sy;
		}
	}


	// [Patch] : for redraw error at right side
/*
	b_fx -= c->sx;
	b_fy -= c->sy;
	b_tx += c->sx;
	b_ty += c->sy;
*/
	if (
		(
			( ( b_fx >= a_fx )&&( b_fx <= a_tx ) )
			||
			( ( b_tx >= a_fx )&&( b_tx <= a_tx ) )
		)
		&&
		(
			( ( b_fy >= a_fy )&&( b_fy <= a_ty ) )
			||
			( ( b_ty >= a_fy )&&( b_ty <= a_ty ) )
		)
	)
	{
		return n_posix_true;
	}


	return n_posix_false;
}

#define n_game_map_chara_draw( mc ) n_game_map_chara_draw_full( mc,0,0,0,0 )

void
n_game_map_chara_draw_full( n_game_map_chara *mc, n_type_real alpha, int mirror, int rotate, int edge )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}

	if ( mc->redraw == n_posix_false ) { return; }


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	mc->redraw = n_posix_false;


	s32 ox = m->x;
	s32 oy = m->y;

	if ( m->mode & N_GAME_MAP_MODE_LOOP )
	{
		if (
			( ( c->x + c->sx ) < ox )
			&&
			( ( ox + game.sx ) >= m->sx )
		)
		{
			ox = -( m->sx - m->x );
		}

		if (
			( ( c->y + c->sy ) < oy )
			&&
			( ( oy + game.sy ) >= m->sy )
		)
		{
			oy = -( m->sy - m->y );
		}
	}


	// [!] : reverse lr

	s32 lr = N_BMP_COPY_MIRROR_NONE;

	if ( mc->lr == N_GAME_MAP_CHARA_RIGHT )
	{
		lr = N_BMP_COPY_MIRROR_LEFTSIDE_RIGHT;
	}


	c->x -= ox;
	c->y -= oy;

	n_game_chara_draw_full( c, alpha, lr | mirror, rotate, edge );

	c->x += ox;
	c->y += oy;


	return;
}

void
n_game_map_chara_set( n_game_map_chara *mc, s32 cellx, s32 celly )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	c->x = cellx * m->unit_sx;
	c->y = celly * m->unit_sy;


	return;
}

void
n_game_map_chara_autoset( n_game_map_chara *mc, int number_param, int number_event )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }

		if ( mc->m->main == NULL ) { return; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	const s32 bmpsx = N_BMP_SX(  m->main );
	const s32 bmpsy = N_BMP_SY(  m->main );
	const s32 mapsx = N_BMP_SX( &m->map  );
	const s32 mapsy = N_BMP_SY( &m->map  );


	s32 x,y;
	u32 number;


	x = y = 0;
	n_posix_loop
	{

		n_bmp_ptr_get_fast( &m->map, x,y, &number );

		if (
			( ( number_param == 1 )&&( number_event == n_game_map_prm1_get( number ) ) )
			||
			( ( number_param == 2 )&&( number_event == n_game_map_prm2_get( number ) ) )
			||
			( ( number_param == 3 )&&( number_event == n_game_map_prm3_get( number ) ) )
		)
		{
			break;
		}

		x++;
		if ( x >= mapsx )
		{

			x = 0;

			y++;
			if ( y >= mapsy )
			{
				return;
			}
		}

	}

	x *= m->unit_sx;
	y *= m->unit_sy;


	m->x = x - ( bmpsx / 2 );
	m->y = y - ( bmpsy / 2 );


	// [!] : don't add margin

	c->x = x;
	c->y = y;


	return;
}

s32
n_game_map_chara_fit_detect_outer( n_game_map_chara *mc, int udlr )
{

	//n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	s32 sx = c->sx - ( c->mx * 2 );
	s32 sy = c->sy - ( c->my * 2 );


	s32 ret = 0;

	if ( udlr == N_GAME_MAP_CHARA_UP   )
	{

		s32 x = c->mx + mc->jump_srcx + ( sx / 2 );
		s32 y = c->my;
		n_posix_loop
		{
			u32 color = n_bmp_trans + 1; n_bmp_ptr_get( c->chara, x,y, &color );
			if ( n_posix_false == n_bmp_is_trans( c->chara, color ) ) { break; }
			y++;
		}

		ret = ( y - c->my );

	} else
	if ( udlr == N_GAME_MAP_CHARA_DOWN )
	{

		s32 x = c->mx + mc->jump_srcx + ( sx / 2 );
		s32 y = c->my + sy;
		n_posix_loop
		{
			u32 color = n_bmp_trans + 1; n_bmp_ptr_get( c->chara, x,y, &color );
			if ( n_posix_false == n_bmp_is_trans( c->chara, color ) ) { break; }
			y--;
		}

		ret = ( y - c->my );

	} else
	if ( udlr == N_GAME_MAP_CHARA_LEFT  )
	{

		n_posix_bool is_found = n_posix_false;

		s32 x = c->mx + mc->dash_srcx;
		s32 y = c->my + sy - 1;
		n_posix_loop
		{
			u32 color = n_bmp_trans + 1; n_bmp_ptr_get( c->chara, x,y, &color );
			if ( n_posix_false == n_bmp_is_trans( c->chara, color ) ) { is_found = n_posix_true; break; }
			x++;
		}

		if ( ( is_found )&&( x != ( c->mx + mc->dash_srcx ) ) ) { x--; }

		ret = ( x - ( c->mx + mc->dash_srcx ) );

	} else
	if ( udlr == N_GAME_MAP_CHARA_RIGHT )
	{

		n_posix_bool is_found = n_posix_false;

		// [!] : for mirroring

		s32 x = c->mx + mc->dash_srcx;
		s32 y = c->my + sy - 1;
		n_posix_loop
		{
			u32 color = n_bmp_trans + 1; n_bmp_ptr_get( c->chara, x,y, &color );
			if ( n_posix_false == n_bmp_is_trans( c->chara, color ) ) { is_found = n_posix_true; break; }
			x++;
		}

		ret = ( x - ( c->mx + mc->dash_srcx ) );

		if ( ( is_found )&&( ret != 0 ) ) { ret = sx - ret; }

	}


	return ret;
}

void
n_game_map_chara_fit( n_game_map_chara *mc, u32 value, int udlr )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->m == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}

	if ( value == 0 ) { return; }


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	s32 sx = c->sx - ( c->mx * 2 );
	s32 sy = c->sy - ( c->my * 2 );

	s32 half_sx = sx / 2;
	s32 half_sy = sy / 2;


	u32 i = 0;
	n_posix_loop
	{

		s32 fx = ( c->x + c->mx );
		s32 fy = ( c->y + c->my );
		s32 tx = 0;
		s32 ty = 0;
		s32 zx = 0;
		s32 zy = 0;


		if (
			( udlr == N_GAME_MAP_CHARA_UP   )
			||
			( udlr == N_GAME_MAP_CHARA_DOWN )
		)
		{

			// Inner

			fx = tx = zx = fx + half_sx;


			// Outer

			fy = ty = zy = fy + n_game_map_chara_fit_detect_outer( mc, udlr );

		} else
		if (
			( udlr == N_GAME_MAP_CHARA_LEFT  )
			||
			( udlr == N_GAME_MAP_CHARA_RIGHT )
		)
		{

			// Inner

			zy = fy + half_sy;
			fy = ty = fy + ( sy - 1 );


			// Outer

			fx = tx = zx = fx + n_game_map_chara_fit_detect_outer( mc, udlr );

		} else {

			break;

		}


		u32 d1 = n_game_map_get( m, fx,fy );
		u32 d2 = n_game_map_get( m, tx,ty );
		u32 d3 = n_game_map_get( m, zx,zy );

		if (
			( n_posix_false == n_game_map_chara_is_block( d1 ) )
			&&
			( n_posix_false == n_game_map_chara_is_block( d2 ) )
			&&
			( n_posix_false == n_game_map_chara_is_block( d3 ) )
		)
		{

			if ( udlr == N_GAME_MAP_CHARA_UP    )
			{
				c->y--;
			} else
			if ( udlr == N_GAME_MAP_CHARA_DOWN  )
			{
				c->y++;
			} else
			if ( udlr == N_GAME_MAP_CHARA_LEFT  )
			{
				c->x--;
			} else
			if ( udlr == N_GAME_MAP_CHARA_RIGHT )
			{
				c->x++;
			}

			i++;
			if ( i >= value ) { break; }

		} else {

			break;

		}

	}


	return;
}

int
n_game_map_chara_wall( n_game_map_chara *mc, int udlr )
{

	int ret = N_GAME_MAP_CHARA_NOMOVE;


	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return ret; }
		if ( mc->m == NULL ) { return ret; }
		if ( mc->c == NULL ) { return ret; }
	}


	n_game_map   *m = mc->m;
	n_game_chara *c = mc->c;


	s32 sx = c->sx - ( c->mx * 2 );
	s32 sy = c->sy - ( c->my * 2 );

	s32 half_sx = sx / 2;
	s32 half_sy = sy / 2;


	//u32 i = 0;
	n_posix_loop
	{

		s32 fx = ( c->x + c->mx );
		s32 fy = ( c->y + c->my );
		s32 tx = 0;
		s32 ty = 0;

		if (
			( udlr == N_GAME_MAP_CHARA_UP   )
			||
			( udlr == N_GAME_MAP_CHARA_DOWN )
		)
		{

			// Inner

			fx = tx = fx + half_sx;


			// Outer

			fy = ty = fy + n_game_map_chara_fit_detect_outer( mc, udlr );

		} else
		if (
			( udlr == N_GAME_MAP_CHARA_LEFT  )
			||
			( udlr == N_GAME_MAP_CHARA_RIGHT )
		)
		{

			// Inner

			fy = ty = fy + half_sy;


			// Outer

			fx = tx = fx + n_game_map_chara_fit_detect_outer( mc, udlr );

		} else {

			break;

		}


		u32 d1 = n_game_map_get( m, fx,fy );
		u32 d2 = n_game_map_get( m, tx,ty );

		if (
			( n_posix_false == n_game_map_chara_is_block( d1 ) )
			&&
			( n_posix_false == n_game_map_chara_is_block( d2 ) )
		)
		{

			if ( udlr == N_GAME_MAP_CHARA_UP    )
			{
				ret |= N_GAME_MAP_CHARA_UP;
			} else
			if ( udlr == N_GAME_MAP_CHARA_DOWN  )
			{
				ret |= N_GAME_MAP_CHARA_DOWN;
			} else
			if ( udlr == N_GAME_MAP_CHARA_LEFT  )
			{
				ret |= N_GAME_MAP_CHARA_LEFT;
			} else
			if ( udlr == N_GAME_MAP_CHARA_RIGHT )
			{
				ret |= N_GAME_MAP_CHARA_RIGHT;
			}

			break;

		} else {

			break;

		}

	}


	return ret;
}

int
n_game_map_chara_collision( n_game_map_chara *mc_a, n_game_map_chara *mc_b, int udlr_move )
{

	if ( n_game_map_safemode )
	{
		if ( mc_a    == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }
		if ( mc_a->m == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }
		if ( mc_a->c == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }

		if ( mc_b    == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }
		if ( mc_b->m == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }
		if ( mc_b->c == NULL ) { return N_GAME_MAP_CHARA_NOMOVE; }
	}


	n_game_map   *m = mc_a->m;
	n_game_chara *a = mc_a->c;
	n_game_chara *b = mc_b->c;

	if ( mc_a->m != mc_b->m ) { return N_GAME_MAP_CHARA_NOMOVE; }


	// Init

	int udlr_ret = N_GAME_MAP_CHARA_NOMOVE;


	s32 a_fit_x = n_game_map_chara_fit_detect_outer( mc_a, N_GAME_MAP_CHARA_LEFT );
	s32 a_fit_y = 0;
	s32 b_fit_x = n_game_map_chara_fit_detect_outer( mc_b, N_GAME_MAP_CHARA_LEFT );
	s32 b_fit_y = 0;


	s32 a_mx = a->mx + a_fit_x - mc_a->dash_cur;
	s32 a_my = a->my + a_fit_y;
	s32 a_sx = a->sx - ( a_mx * 2 );
	s32 a_sy = a->sy - ( a_my * 2 );
//n_game_hwndprintf_literal( " %d %d ", a->mx + a_fit_x, mc_a->dash_cur );

	s32 a_fx = a->x + a_mx;
	s32 a_fy = a->y + a_my;
	s32 a_tx = a_fx + a_sx;
	s32 a_ty = a_fy + a_sy;


	s32 b_mx = b->mx + b_fit_x - mc_b->dash_cur;
	s32 b_my = b->my + b_fit_y;
	s32 b_sx = b->sx - ( b_mx * 2 );
	s32 b_sy = b->sy - ( b_my * 2 );
//n_game_hwndprintf_literal( " %d %d ", b->mx + b_fit_x, mc_b->dash_cur );

	s32 b_fx = b->x + b_mx;
	s32 b_fy = b->y + b_my;
	s32 b_tx = b_fx + b_sx;
	s32 b_ty = b_fy + b_sy;


	if ( m->mode & N_GAME_MAP_MODE_LOOP )
	{
		if ( ( a_fx >= ( m->sx - a_sx ) )&&( b_fx <= ( b_sx ) ) )
		{
			b_fx += m->sx;
			b_tx += m->sx;
		}
		if ( ( b_fx >= ( m->sx - b_sx ) )&&( a_fx <= ( a_sx ) ) )
		{
			a_fx += m->sx;
			a_tx += m->sx;
		}

		if ( ( a_fy >= ( m->sy - a_sy ) )&&( b_fy <= ( b_sy ) ) )
		{
			b_fy += m->sy;
			b_ty += m->sy;
		}
		if ( ( b_fy >= ( m->sy - b_sy ) )&&( a_fy <= ( a_sy ) ) )
		{
			a_fy += m->sy;
			a_ty += m->sy;
		}
	}


	// don't use "Outer" detection for n_game_map_chara_fit()


	// Inner

	a_tx--;
	a_ty--;

	b_tx--;
	b_ty--;


	if (
		( a_tx < b_fx )
		||
		( a_fx > b_tx )
		||
		( a_ty < b_fy )
		||
		( a_fy > b_ty )
	)
	{
		return udlr_ret;
	}

//n_game_title( "Inner" );


	// Outer

	a_fx--;
	a_fy--;
	a_tx++;
	a_ty++;


	// [ Mechanism ]
	//
	//	when N_GAME_MAP_CHARA_LEFT
	//
	//	moved : |m|f| a |f|m|==========>
	//	fixed :     |m|f| b |f|m|======>

	a_mx = a->mx + a_fit_x;
	b_mx = b->mx + b_fit_x;

	if ( ( a_ty >= b_fy )&&( a_ty <= b_ty ) )
	{

		if ( udlr_move & N_GAME_MAP_CHARA_UP )
		{
			a->y = ( b->y + b_my ) + ( a_my );
			n_game_map_chara_fit( mc_a, a->sy, N_GAME_MAP_CHARA_UP   );

			b->y = ( a->y - a_my ) - ( b_my );
			n_game_map_chara_fit( mc_b, a->sy, N_GAME_MAP_CHARA_DOWN );
		}

		udlr_ret |= N_GAME_MAP_CHARA_UP;
	}

	if ( ( a_fy >= b_fy )&&( a_fy <= b_ty ) )
	{

		if ( udlr_move & N_GAME_MAP_CHARA_DOWN )
		{
			a->y = ( b->y - b_my ) - ( a_my );
			n_game_map_chara_fit( mc_a, b->sy, N_GAME_MAP_CHARA_DOWN );

			b->y = ( a->y + a_my ) + ( b_my );
			n_game_map_chara_fit( mc_b, b->sy, N_GAME_MAP_CHARA_UP   );
		}

		udlr_ret |= N_GAME_MAP_CHARA_DOWN;
	}

	if ( ( a_tx >= b_fx )&&( a_tx <= b_tx ) )
	{

		if ( udlr_move & N_GAME_MAP_CHARA_LEFT )
		{
//n_game_hwndprintf_literal( " N_GAME_MAP_CHARA_LEFT " );

			a->x = ( b->x + b_mx ) + ( a_mx );
			n_game_map_chara_fit( mc_a, a->sx, N_GAME_MAP_CHARA_LEFT  );

			b->x = ( a->x - a_mx ) - ( b_mx );
			n_game_map_chara_fit( mc_b, a->sx, N_GAME_MAP_CHARA_RIGHT );
		}

		udlr_ret |= N_GAME_MAP_CHARA_LEFT;
	}

	if ( ( a_fx >= b_fx )&&( a_fx <= b_tx ) )
	{

		if ( udlr_move & N_GAME_MAP_CHARA_RIGHT )
		{
//n_game_hwndprintf_literal( " N_GAME_MAP_CHARA_RIGHT " );

			a->x = ( b->x - b_mx ) - ( a_mx );
			n_game_map_chara_fit( mc_a, b->sx, N_GAME_MAP_CHARA_RIGHT );

			b->x = ( a->x + a_mx ) + ( b_mx );
			n_game_map_chara_fit( mc_b, b->sx, N_GAME_MAP_CHARA_LEFT  );
		}

		udlr_ret |= N_GAME_MAP_CHARA_RIGHT;
	}


	return udlr_ret;
}

void
n_game_map_chara_stop( n_game_map_chara *mc )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}

	n_game_chara *c = mc->c;


	mc->dash     = N_GAME_MAP_CHARA_DASH_OFF;
	mc->dash_cur = mc->dash_min;


	if (
		( mc->jump == N_GAME_MAP_CHARA_JUMP_OFF )
		&&
		( c->srcx != 0 )
	)
	{
		mc->redraw = n_posix_true;

		c->srcx = 0;
	}


	return;
}

void
n_game_map_chara_dash( n_game_map_chara *mc, s32 lr )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_chara *c = mc->c;


	n_posix_bool slip = n_posix_false;


	if ( mc->lr != lr )
	{

		slip = n_posix_true;


		mc->dash_cur--;

		if ( mc->dash_cur < mc->dash_min )
		{

			mc->dash_cur = mc->dash_min;

			if ( mc->lr == N_GAME_MAP_CHARA_LEFT  )
			{
				mc->lr = N_GAME_MAP_CHARA_RIGHT;
			} else
			if ( mc->lr == N_GAME_MAP_CHARA_RIGHT )
			{
				mc->lr = N_GAME_MAP_CHARA_LEFT;
			}

		}

	} else {

		if ( mc->dash == N_GAME_MAP_CHARA_DASH_OFF )
		{

			mc->dash_cur--;

			if ( mc->dash_cur < mc->dash_min )
			{
				mc->dash_cur = mc->dash_min;
			}

		} else {

			mc->dash_cur++;

			if ( mc->dash_cur > mc->dash_max )
			{
				mc->dash_cur = mc->dash_max;
			}
		}
	}


	n_game_map_chara_fit
	(
		mc,
		mc->dash_step + ( mc->dash_step * mc->dash_cur ),
		mc->lr
	);


	if (
		( mc->jump == N_GAME_MAP_CHARA_JUMP_OFF )
		&&
		( n_game_refresh_is_off() )
	)
	{

		if ( slip )
		{

			c->srcx = mc->slip_srcx;

		} else {

			if ( n_game_timer( &mc->dash_frame_timer, mc->dash_frame_msec ) )
			{
				c->srcx += c->sx;
				if ( c->srcx >= ( mc->dash_srcx + ( c->sx * mc->dash_frame ) ) )
				{
					c->srcx = mc->dash_srcx;
				}
			}

		}

	}

	mc->redraw = n_posix_true;


	return;
}

void
n_game_map_chara_jump( n_game_map_chara *mc )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_chara *c = mc->c;


	if ( mc->jump == N_GAME_MAP_CHARA_JUMP_OFF )
	{

		if ( n_game_refresh_is_off() )
		{

			mc->redraw = n_posix_true;

			mc->jump     = N_GAME_MAP_CHARA_JUMP_ON_RISE;
			mc->jump_cur = 0;

			c->srcx = mc->jump_srcx;

		}

	} else
	if ( mc->jump == N_GAME_MAP_CHARA_JUMP_ON_RISE )
	{

		if ( mc->jump_cur <= mc->jump_max )
		{

			mc->redraw = n_posix_true;

			mc->jump_cur += mc->jump_step;

			n_game_map_chara_fit( mc, mc->jump_step, N_GAME_MAP_CHARA_UP );

		} else {

			mc->jump = N_GAME_MAP_CHARA_JUMP_ON_FALL;

		}

	}


	return;
}

void
n_game_map_chara_gravity( n_game_map_chara *mc )
{

	if ( n_game_map_safemode )
	{
		if ( mc    == NULL ) { return; }
		if ( mc->c == NULL ) { return; }
	}


	n_game_chara *c = mc->c;


	if ( mc->jump == N_GAME_MAP_CHARA_JUMP_ON_RISE ) { return; }


	s32 tmpy = c->y;
	n_game_map_chara_fit( mc, mc->jump_step, N_GAME_MAP_CHARA_DOWN );

	if ( tmpy == c->y )
	{

		if ( mc->jump == N_GAME_MAP_CHARA_JUMP_OFF ) { return; }


		if ( n_game_refresh_is_off() )
		{
			// Landing

			mc->jump = N_GAME_MAP_CHARA_JUMP_OFF;

			c->srcx = 0;
		}

	} else {

		if ( mc->jump != N_GAME_MAP_CHARA_JUMP_ON_FALL )
		{
			mc->jump = N_GAME_MAP_CHARA_JUMP_ON_FREEFALL;

			c->srcx = mc->jump_srcx;
		}

	}

	mc->redraw = n_posix_true;


	return;
}


#endif // _H_NONNON_WIN32_GAME_MAP_CHARA

