// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#define n_freecell_rule_is_hearts(   p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_HEARTS,   N_FREECELL_SUIT_DIAMONDS )
#define n_freecell_rule_is_diamonds( p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_DIAMONDS, N_FREECELL_SUIT_SPADES   )
#define n_freecell_rule_is_spades(   p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_SPADES,   N_FREECELL_SUIT_CLUBS    )
#define n_freecell_rule_is_clubs(    p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_CLUBS,    N_FREECELL_SUIT_MAX      )
#define n_freecell_rule_is_reds(     p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_HEARTS,   N_FREECELL_SUIT_SPADES   )
#define n_freecell_rule_is_blacks(   p, i ) n_freecell_rule_range( p, i, N_FREECELL_SUIT_SPADES,   N_FREECELL_SUIT_MAX      )

BOOL
n_freecell_rule_range( n_freecell *p, int index, int f, int t )
{

	int data = p->tableau[ index ].data;
	int unit = N_FREECELL_CARD_UNIT;

	if ( ( data >= ( unit * f ) )&&( data < ( unit * t ) ) ) { return TRUE; }


	return FALSE;
}

BOOL
n_freecell_rule_suit_is_ok( n_freecell *p, int a, int b )
{

	if ( ( n_freecell_rule_is_hearts  ( p, a ) ) && ( n_freecell_rule_is_hearts  ( p, b ) ) ) { return TRUE; }
	if ( ( n_freecell_rule_is_diamonds( p, a ) ) && ( n_freecell_rule_is_diamonds( p, b ) ) ) { return TRUE; }
	if ( ( n_freecell_rule_is_spades  ( p, a ) ) && ( n_freecell_rule_is_spades  ( p, b ) ) ) { return TRUE; }
	if ( ( n_freecell_rule_is_clubs   ( p, a ) ) && ( n_freecell_rule_is_clubs   ( p, b ) ) ) { return TRUE; }


	return FALSE;
}

BOOL
n_freecell_rule_half_is_ok( n_freecell *p, int a, int b )
{

	if ( ( n_freecell_rule_is_reds(   p, a ) ) && ( n_freecell_rule_is_blacks( p, b ) ) ) { return TRUE; }
	if ( ( n_freecell_rule_is_blacks( p, a ) ) && ( n_freecell_rule_is_reds  ( p, b ) ) ) { return TRUE; }


	return FALSE;
}

BOOL
n_freecell_rule_rank_is_ok( n_freecell *p, int a, int b )
{

	a = p->tableau[ a ].data;
	b = p->tableau[ b ].data;


	int rank;

	rank  = a % N_FREECELL_CARD_UNIT;
	rank -= b % N_FREECELL_CARD_UNIT;

	if ( rank == 1 )
	{
		return TRUE;
	}


	return FALSE;
}

int
n_freecell_rule_freespace( n_freecell *p )
{

	int freespace = 0;

	int x = 0;
	n_posix_loop
	{

		if ( p->tableau[ x ].data == N_FREECELL_NOTHING ) { freespace++; }

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	// [x] : difficult to determine

	int freespace_tableau = 0;

	x = 0;
	n_posix_loop
	{break;

		if ( p->tableau[ N_FREECELL_SUIT_MAX + x ].data == N_FREECELL_NOTHING ) { freespace_tableau++; }

		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	return freespace;
}

BOOL
n_freecell_rule_is_movable( n_freecell *p, int index )
{
//return FALSE;
//return TRUE;

	// Free cell

	if ( index < N_FREECELL_SUIT_MAX )
	{
		return ( p->tableau[ index ].data != N_FREECELL_NOTHING );
	}
 

	// Empty cell

	if ( p->tableau[ index ].data == N_FREECELL_NOTHING ) { return FALSE; }


	// Sequence Checker

	BOOL ret = FALSE;

	int sequence = 0;

	int i = 0;
	n_posix_loop
	{

		int f = index + n_freecell_index_upper( i + 0 );
 		int t = index + n_freecell_index_upper( i + 1 );


		// Single : top most of a pile

		if ( t >= N_FREECELL_PILE_ALL ) { return TRUE; }


		// Single : top of a stock

		if ( p->tableau[ t ].data == N_FREECELL_NOTHING ) { ret = TRUE; break; }


		// Multiple

		if (
			( n_freecell_rule_half_is_ok( p, f, t ) )
			&&
			( n_freecell_rule_rank_is_ok( p, f, t ) )
		)
		{
			sequence++;
		} else {
			break;
		}


		i++;
		if ( f >= N_FREECELL_PILE_ALL ) { break; }
	}


	if ( ( ret )&&( sequence > p->freespace ) ) { ret = FALSE; }


	return ret;
}

BOOL
n_freecell_rule_is_puttable( n_freecell *p, int index )
{
//return TRUE;

	// [Mechanism]
	//
	//	"index" : .data could have N_FREECELL_NOTHING


	n_posix_loop
	{

		// Cascading : Top to Bottom

		if ( p->tableau[ index ].data != N_FREECELL_NOTHING ) { break; }

		// Bottom-most

		index -= n_freecell_index_upper( 1 );
		if ( index < N_FREECELL_SUIT_MAX ) { return TRUE; }
	}


	return (
		( n_freecell_rule_rank_is_ok( p, index, p->drag_index ) )
		&&
		( n_freecell_rule_half_is_ok( p, index, p->drag_index ) )
	);
}

BOOL
n_freecell_rule_stock_is_puttable( n_freecell *p, int f, int t )
{

	BOOL ret = FALSE;

	int a = p->stock  [ t ].data;
	int b = p->tableau[ f ].data;
	int c = ( t + 0 ) * N_FREECELL_CARD_UNIT;
	int d = ( t + 1 ) * N_FREECELL_CARD_UNIT;
	int n = N_FREECELL_NOTHING;
//n_game_hwndprintf_literal( " Stock : %d %d %d ", a, b, c );

	if (
		( ( a == n )&&( c == b ) )
		||
		( ( a >= c )&&( ( a + 1 ) < d )&&( ( a + 1 ) == b ) )
	)
	{
		ret = TRUE;
	}


	return ret;
}

void
n_freecell_rule_stock( n_freecell *p, int index, int xxx, BOOL *restore )
{
//NSLog( @"n_freecell_rule_stock()" );

	if ( n_freecell_rule_stock_is_puttable( p, index, xxx ) )
	{

		p->stock  [   xxx ].data  = p->tableau[ index ].data;
		p->tableau[ index ].data  = N_FREECELL_NOTHING;

		p->stock  [   xxx ].chara = p->tableau[ index ].chara;
		p->tableau[ index ].chara = NULL;

		if ( p->tableau[ index ].dnd_onoff )
		{

			if ( n_game_chara_dnd_refcount >= 1 ) { n_game_chara_dnd_refcount--; }

			p->tableau[ index ].dnd_onoff = FALSE;
			p->tableau[ index ].dnd_ox    = 0;
			p->tableau[ index ].dnd_oy    = 0;

		}

		n_freecell_chara_erase( p, &p->tableau[ index ] );

		p->restore_count = 0;


		p->drag_index = N_FREECELL_NOTHING;

		n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );

		n_freecell_reposition( p );
		n_freecell_redraw( p );
		p->refresh = TRUE;

	} else {

		if ( restore != NULL ) { (*restore) = TRUE; }

	}


	return;
}

int
n_freecell_rule_number( n_freecell *p, int data )
{

	if ( data == -1 ) { return 0; }


	int h = N_FREECELL_SUIT_HEARTS   * N_FREECELL_CARD_UNIT;
	int d = N_FREECELL_SUIT_DIAMONDS * N_FREECELL_CARD_UNIT;
	int s = N_FREECELL_SUIT_SPADES   * N_FREECELL_CARD_UNIT;
	int c = N_FREECELL_SUIT_CLUBS    * N_FREECELL_CARD_UNIT;
	int m = N_FREECELL_SUIT_MAX      * N_FREECELL_CARD_UNIT;

	if ( ( data >= h )&&( data < d ) )
	{
		data -= h;
	} else
	if ( ( data >= d )&&( data < s ) )
	{
		data -= d;
	} else
	if ( ( data >= s )&&( data < c ) )
	{
		data -= s;
	} else
	if ( ( data >= c )&&( data < m ) )
	{
		data -= c;
	}


	return data;
}

int
n_freecell_rule_stock_minimum( n_freecell *p )
{

	// [!] : maximum based approach is problematic : dead lock will happen

	int ret = N_FREECELL_CARD_UNIT - 1;

	int x = 0;
	n_posix_loop
	{//break;

		int data = p->stock[ x ].data;
		if ( data != N_FREECELL_NOTHING ) { data = n_freecell_rule_number( p, data ); }

		if ( ret > data ) { ret = data; }

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	return ret + 1;
}

BOOL
n_freecell_rule_stock_minimum_is_puttable( n_freecell *p, int minimum, int index )
{
//n_game_hwndprintf_literal( " %d %d %d %d ", p->stock[ 0 ].data, p->stock[ 1 ].data, p->stock[ 2 ].data, p->stock[ 3 ].data );

	int data = n_freecell_rule_number( p, p->tableau[ index ].data );

	return ( minimum >= data );
}

void
n_freecell_rule_automove( n_freecell *p, int threshold, int target )
{
//NSLog( @"n_freecell_rule_automove()" );

	int xxx = 0;
	n_posix_loop
	{//break;
//n_freecell_rule_stock( p, i, xxx, NULL );

		if (
			( n_freecell_rule_stock_minimum_is_puttable( p, threshold, target ) )
			&&
			( n_freecell_rule_stock_is_puttable( p, target, xxx ) )
		)
		{
//n_freecell_animation_go( p, target, xxx );
			p->animation_server[ xxx ] = target;
		}

		xxx++;
		if ( xxx >= N_FREECELL_SUIT_MAX ) { break; }
	}

	return;
}

BOOL
n_freecell_restore( n_freecell *p, int index )
{

	if ( index != N_FREECELL_NOTHING )
	{

		if ( FALSE == n_freecell_rule_is_movable( p, index ) ) { return FALSE; }


		n_game_chara_dnd( &p->tableau[ index ], p->hwnd, VK_LBUTTON );
		if ( FALSE == p->tableau[ index ].dnd_onoff ) { return FALSE; }


		if ( p->drag_index == N_FREECELL_NOTHING )
		{
//n_game_title_literal( " Restore : ON " );


			n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );


			p->drag_index = index;
//n_game_hwndprintf_literal( " %d ", p->drag_index );

			if ( p->drag_index < N_FREECELL_SUIT_MAX )
			{

				p->restore[ 0 ]  = p->tableau[ p->drag_index ];
				p->restore_count = 1;

			} else {

				int y = 0;
				n_posix_loop
				{//break;

					int upper = p->drag_index + n_freecell_index_upper( y );
					if ( upper >= N_FREECELL_PILE_ALL ) { break; }
					if ( p->tableau[ upper ].data == N_FREECELL_NOTHING ) { break; }

					p->restore[ y ] = p->tableau[ upper ];

					y++;
					if ( y >= N_FREECELL_PILE_SY ) { break; }
				}

				p->restore_count = y;

			}
//n_game_hwndprintf_literal( " %d ", p->restore_count );

			n_freecell_shadow( p );

		} else {

			// Multiple Dragging

			if ( p->restore_count >= 2 )
			{
				int i = 1;
				n_posix_loop
				{

					int upper = p->drag_index + n_freecell_index_upper( i );

					p->tableau[ upper ].x = p->tableau[ p->drag_index ].x;
					p->tableau[ upper ].y = p->tableau[ p->drag_index ].y + ( p->cardgen.osy * i );


					i++;
					if ( i >= p->restore_count ) { break; }
				}
			}


			// [!] : change halo color when puttable

//n_win_debug_count( game.hwnd );

			p->color_halo_current = p->color_halo;

			n_type_gfx ox = p->cardgen.card_sx / 2;
			n_type_gfx oy = p->cardgen.card_sy / 2;

//static int i = 0;
//n_game_hwndprintf_literal( " %d : %d %d ", i, p->tableau[ p->drag_index ].x, p->tableau[ p->drag_index ].y );
//i++;

			int xxx = 0;
			n_posix_loop
			{//break;

				n_game_chara *f = &p->tableau[ p->drag_index ];
				n_game_chara *t = &p->stock[ xxx ];

//n_game_hwndprintf_literal( " %d %d %d ", f->x, t->x, ( t->x + t->sx ) );

				if (
					( p->restore_count == 1 )
					&&
					( n_game_chara_is_hit_offset( f, t, ox,oy, 0,0 ) )
					&&
					( n_freecell_rule_stock_is_puttable( p, p->drag_index, xxx ) )
				)
				{
//n_posix_debug_literal( " Stock : %d ", xxx );

//NSLog( @"n_freecell_restore()" );

					p->color_halo_current = p->color_halo_focus;

				}
//break;
				xxx++;
				if ( xxx >= N_FREECELL_SUIT_MAX ) { break; }
			}

			int xx = 0;
			n_posix_loop
			{

				n_game_chara *f = &p->tableau[ p->drag_index ];
				n_game_chara *t = &p->tableau[ xx ];

				if (
					( p->restore_count == 1 )
					&&
					( p->tableau[ xx ].data == N_FREECELL_NOTHING )
					&&
					( n_game_chara_is_hit_offset( f, t, ox,oy, 0,0 ) )
				)
				{

					p->color_halo_current = p->color_halo_focus;

					break;
				}

				xx++;
				if ( xx >= N_FREECELL_SUIT_MAX ) { break; }
			}


			int x = 0;
			int y = 0;
			n_posix_loop
			{

				int target = n_freecell_pos2index( x,y );

				if (
					( p->tableau[ target ].data == N_FREECELL_NOTHING )
					&&
					( n_game_chara_is_hit_offset( &p->tableau[ p->drag_index ], &p->tableau[ target ], ox,oy, 0,0 ) )
					&&
					( n_freecell_rule_is_puttable( p, target ) )
				)
				{

					p->color_halo_current = p->color_halo_focus;

					break;
				}

				x++;
				if ( x >= N_FREECELL_PILE_SX )
				{

					x = 0;

					y++;
					if ( y >= N_FREECELL_PILE_SY ) { break; }
				}
			}

		}


		// [!] : off for halo effect

		//if ( n_game_chara_is_moved( &p->tableau[ p->drag_index ] ) )
		{
			p->refresh = TRUE;
		}

	} else {

		if ( p->drag_index == N_FREECELL_NOTHING ) { return FALSE; }


		p->color_halo_current = p->color_halo;


		BOOL moved   = FALSE;
		BOOL restore = FALSE;


		n_type_gfx ox = p->cardgen.card_sx / 2;
		n_type_gfx oy = p->cardgen.card_sy / 2;


		int xxx = 0;
		n_posix_loop
		{

			if (
				( p->restore_count == 1 )
				&&
				( n_game_chara_is_hit_offset( &p->tableau[ p->drag_index ], &p->stock[ xxx ], ox,oy, 0,0 ) )
			)
			{
//n_posix_debug_literal( " Stock : %d ", xxx );

				n_freecell_rule_stock( p, p->drag_index, xxx, &restore );

			}

			xxx++;
			if ( xxx >= N_FREECELL_SUIT_MAX ) { break; }
		}


		int xx = 0;
		n_posix_loop
		{
	
			if (
				( p->restore_count == 1 )
				&&
				( p->tableau[ xx ].data == N_FREECELL_NOTHING )
				&&
				( n_game_chara_is_hit_offset( &p->tableau[ p->drag_index ], &p->tableau[ xx ], ox,oy, 0,0 ) )
			)
			{
//n_game_title_literal( "Collision" );
				moved = TRUE;

				break;
			}

			xx++;
			if ( xx >= N_FREECELL_SUIT_MAX ) { break; }
		}


		int x = 0;
		int y = 0;
		n_posix_loop
		{

			if ( moved ) { break; }


			int target = n_freecell_pos2index( x,y );

			if (
				( p->tableau[ target ].data == N_FREECELL_NOTHING )
				&&
				( n_game_chara_is_hit_offset( &p->tableau[ p->drag_index ], &p->tableau[ target ], ox,oy, 0,0 ) )
				&&
				( n_freecell_rule_is_puttable( p, target ) )
			)
			{
//n_game_title_literal( " Collision " );
				moved = TRUE;

				break;
			}

			x++;
			if ( x >= N_FREECELL_PILE_SX )
			{

				x = 0;

				y++;
				if ( y >= N_FREECELL_PILE_SY ) { break; }
			}
		}


		if ( ( moved == FALSE )||( restore ) )
		{

			if ( p->drag_index < N_FREECELL_SUIT_MAX )
			{
//NSLog( @"%d", p->drag_index );

				n_freecell_chara_erase( p, &p->tableau[ p->drag_index ] );

				if ( p->restore_count == 1 )
				{
					p->tableau[ p->drag_index ].data  = p->restore[ 0 ].data;
					p->tableau[ p->drag_index ].chara = p->restore[ 0 ].chara;

					n_game_chara_pos( &p->tableau[ p->drag_index ], p->cardgen.card_sx * p->drag_index, 0 );
					n_game_chara_prv( &p->tableau[ p->drag_index ] );
				}

				n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );

			} else {

				int i = 0;
				n_posix_loop
				{

					int upper = p->drag_index + n_freecell_index_upper( i );

					n_freecell_chara_erase( p, &p->tableau[ upper ] );

					p->tableau[ upper ] = p->restore[ i ];


					i++;
					if ( i >= p->restore_count ) { break; }
				}

				n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );

			}

		} else
		if ( xx != N_FREECELL_SUIT_MAX )
		{

			// Move

			int f = p->drag_index;
			int t = xx;


			// [!] : erase the last dragged position

			n_freecell_chara_erase( p, &p->tableau[ f ] );


			// [!] : restore x,y,px,py

			n_game_chara_pos( &p->tableau[ f ], p->restore[ 0 ].x, p->restore[ 0 ].y );
			n_game_chara_prv( &p->tableau[ f ] );

			n_game_chara_pos( &p->tableau[ t ], p->cardgen.card_sx * t, 0 );
			n_game_chara_prv( &p->tableau[ t ] );


			p->tableau[ t ].data  = p->tableau[ f ].data;
			p->tableau[ f ].data  = N_FREECELL_NOTHING;

			p->tableau[ t ].chara = p->tableau[ f ].chara;
			p->tableau[ f ].chara = NULL;

			n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );

		} else {

			// [!] : cascade to the bottom most

			n_posix_loop
			{

				int target = n_freecell_pos2index( x,y );

				if ( p->tableau[ target ].data != N_FREECELL_NOTHING ) { break; }

				y--;
				if ( y < 0 ) { break; }
			}
			y++;

			int i = 0;
			n_posix_loop
			{

				int f = n_freecell_index_upper( i ) + p->drag_index;
				int t = n_freecell_index_upper( i ) + n_freecell_pos2index( x,y );


				// [!] : erase the last dragged position

				n_freecell_chara_erase( p, &p->tableau[ f ] );


				// Move

				p->tableau[ t ].data  = p->tableau[ f ].data;
				p->tableau[ f ].data  = N_FREECELL_NOTHING;

				p->tableau[ t ].chara = p->tableau[ f ].chara;
				p->tableau[ f ].chara = NULL;


				i++;
				if ( i >= p->restore_count ) { break; }
			}

			n_freecell_sound_play( p, N_FREECELL_SOUND_CLICK );

		}


		p->tableau[ p->drag_index ].dnd_onoff = FALSE;
		p->drag_index = N_FREECELL_NOTHING;

		n_freecell_reposition( p );
		n_freecell_redraw( p );
		p->refresh = TRUE;

	}


	return TRUE;
}


