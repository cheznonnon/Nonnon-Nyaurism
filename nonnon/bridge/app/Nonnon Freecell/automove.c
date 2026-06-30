// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_freecell_automove( n_freecell *p )
{
//n_game_hwndprintf_literal( " n_freecell_automove() " );

	if ( p->splash_onoff == FALSE ) { return; }

	if ( FALSE != p->refresh ) { return; }

	if ( p->animation_onoff ) { return; }


	int threshold = n_freecell_rule_stock_minimum( p );
//n_game_hwndprintf_literal( " %d ", threshold );


	// [!] : Freecell Area

	{

		int x = 0;
		n_posix_loop
		{

			n_game_chara *c = &p->tableau[ x ];
			if ( c->data != N_FREECELL_NOTHING )
			{
//NSLog( @"n_freecell_rule_automove() : Freecell Area" );
				n_freecell_rule_automove( p, threshold, x );
			}

			x++;
			if ( x >= N_FREECELL_SUIT_MAX ) { break; }
		}

	}


	// [!] : Tableau Area

	int x = 0;
	int y = N_FREECELL_PILE_SY - 1;
	n_posix_loop
	{

		int target = n_freecell_pos2index( x, y );

		n_game_chara *c = &p->tableau[ target ];
		if ( c->data != N_FREECELL_NOTHING )
		{
//NSLog( @"n_freecell_rule_automove() : Tableau Area" );
			n_freecell_rule_automove( p, threshold, target );
			y = 0;
		}

		y--;
		if ( y < 0 )
		{

			y = N_FREECELL_PILE_SY - 1;

			x++;
			if ( x >= N_FREECELL_PILE_SX ) { break; }
		}
	}


	return;
}

void
n_freecell_automove_engine( n_freecell *p )
{

//p->debug_automove_count = 0;

	int done = 0;

	int x = 0;
	n_posix_loop
	{

		if (
			( p->animation_onoff == FALSE )
			&&
			( p->animation_server[ x ] != N_FREECELL_NOTHING )
		)
		{
			done++;

			n_freecell_animation_go( p, p->animation_server[ x ], x );

			p->animation_server[ x ] = N_FREECELL_NOTHING;

			break;
		}

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	if ( done == 0 )
	{
//n_posix_debug_literal( "" );
		n_freecell_automove( p );
	}


	return;
}

