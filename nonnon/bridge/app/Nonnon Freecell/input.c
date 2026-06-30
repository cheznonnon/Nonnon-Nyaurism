// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_freecell_input_doubleclick( n_freecell *p )
{
//return;

	if ( p->animation_onoff ) { return; }


	BOOL found = FALSE;
	int  index = -1;


	// [!] : freecell area is putting on beginning of tableau[]

	n_type_gfx freecell_x = 0;
	n_posix_loop
	{
		n_game_chara *c = &p->tableau[ freecell_x ];

		if ( c->data != N_FREECELL_NOTHING )
		{
			if ( n_freecell_is_hovered_offset( p, c->x,c->y,c->sx,c->sy ) )
			{
				found = TRUE;
				index = freecell_x;
				break;
			}
		}

		freecell_x++;
		if ( freecell_x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	if ( found == FALSE )
	{

		n_type_gfx x = 0;
		n_type_gfx y = N_FREECELL_PILE_SY - 1;
		n_posix_loop
		{

			int target = n_freecell_pos2index( x,y );

			n_game_chara *c = &p->tableau[ target ];

			if ( c->data != N_FREECELL_NOTHING )
			{
				if ( n_freecell_is_hovered_offset( p, c->x,c->y,c->sx,c->sy ) )
				{
					found = TRUE;
					index = n_freecell_pos2index( x,y );
					break;
				}
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

		if ( found == FALSE ) { return; }
//NSLog( @"found : %d %d", x, y ); return;
	}


	int i  = index;
	int ii = 0;
	n_posix_loop
	{//break;

		if ( n_freecell_rule_stock_is_puttable( p, i, ii ) )
		{
			n_freecell_animation_go( p, i, ii );
		}

		ii++;
		if ( ii >= N_FREECELL_SUIT_MAX ) { break; }
	}


	return;
}

void
n_freecell_input( n_freecell *p )
{

	if ( p->animation_onoff ) { return; }


	// DnD

	int dnd = 0;

	int i = N_FREECELL_TABLEAU - 1;
	n_posix_loop
	{

		p->freespace = n_freecell_rule_freespace( p );

		BOOL break_needed = FALSE;
		if ( n_freecell_restore( p, i ) ) { break_needed = TRUE; }

		dnd += p->tableau[ i ].dnd_onoff;

		if ( break_needed ) { break; }

		i--;
		if ( i < 0 )
		{
			n_freecell_restore( p, N_FREECELL_NOTHING );
			break;
		}
	}

	if ( ( dnd == 0 )&&( n_game_chara_dnd_refcount != 0 ) ) { n_game_chara_dnd_refcount = 0; }


	return;
}

