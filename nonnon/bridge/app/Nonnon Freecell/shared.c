// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_freecell_pile_count( n_freecell *p, int redraw_map[ N_FREECELL_PILE_SX ] )
{

	int x = 0;
	n_posix_loop
	{

		redraw_map[ x ] = 0;

		int y = 0;
		n_posix_loop
		{

			int target = n_freecell_pos2index( x, y );
			n_game_chara *c = &p->tableau[ target ];

			if ( c->data == N_FREECELL_NOTHING ) { break; }

			//if ( p->redraw[ x ] )
			{
				redraw_map[ x ]++;
			}

			y++;
			if ( y >= N_FREECELL_PILE_SY ) { break; }
		}

		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	return;
}




void
n_freecell_redraw( n_freecell *p )
{

	int x = 0;
	n_posix_loop
	{

		p->redraw[ x ] = TRUE;

		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	return;
}




void
n_freecell_chara_erase( n_freecell *p, n_game_chara *c )
{

	const n_type_gfx m  = p->cardgen.halo;
	const n_type_gfx mm = ( p->cardgen.halo * 2 ) + ( p->cardgen.halo % 2 );


	c->px -= m;
	c->py -= m;
	c->sx += mm;
	c->sy += mm;

	n_game_chara_erase( c );

	c->px += m;
	c->py += m;
	c->sx -= mm;
	c->sy -= mm;


	return;
}

void
n_freecell_chara_draw( n_freecell *p, n_game_chara *c )
{

	if ( p->animation_onoff == FALSE )
	{

		n_type_gfx x = c->x - p->cardgen.halo;
		n_type_gfx y = c->y - p->cardgen.halo;

		n_bmp_rasterizer
		(
			&p->cardgen.bmp_halo, p->canvas, x,y, 
			p->color_halo_current
		);

	}


	n_game_chara_draw( c );


	return;
}




void
n_freecell_animation_go( n_freecell *p, int index_f, int index_t )
{

	p->animation_onoff   = TRUE;
	p->animation_index_f = index_f;
	p->animation_index_t = index_t;
	p->animation_base_x  = p->tableau[ index_f ].x;
	p->animation_base_y  = p->tableau[ index_f ].y;

	n_type_real step = N_FREECELL_ANIMATION_STEP;
	p->animation_step_x  = (n_type_real) ( p->stock[ index_t ].x - p->tableau[ index_f ].x ) / step;
	p->animation_step_y  = (n_type_real) ( p->stock[ index_t ].y - p->tableau[ index_f ].y ) / step;

	p->drag_index        = p->animation_index_f;


	p->restore_count     = 0;

	n_freecell_shadow( p );


//p->debug_automove_count++;

	return;
}




void
n_freecell_redraw_detect( n_freecell *p )
{

	n_game_chara card = p->tableau[ p->drag_index ];

	card.x = card.px;
	card.y = card.py;


	int x = 0;
	int y = 0;
	n_posix_loop
	{

		if ( y == 0 )
		{

			n_game_chara *b = &p->bottom[ x ];
			if ( n_game_chara_is_hit_offset( &card, b, -p->cardgen.halo,-p->cardgen.halo, 0,0 ) )
			{
				p->redraw[ x ] = TRUE;
			}

		} else {

			if ( ( x == p->drag_index )&&( p->restore_count != 0 ) )
			{

				p->redraw[ x ] = TRUE;

			} else {

				int target = n_freecell_pos2index( x, y );
				n_game_chara *c = &p->tableau[ target ];

				if (
					( c->data != N_FREECELL_NOTHING )
					&&
					( n_game_chara_is_hit_offset( &card, c, -p->cardgen.halo,-p->cardgen.halo, 0,0 ) )
				)
				{
					p->redraw[ x ] = TRUE;
				}

			}

		}


		x++;
		if ( x >= N_FREECELL_PILE_SX )
		{

			x = 0;

			y++;
			if ( y >= N_FREECELL_PILE_SY ) { break; }
		}
	}


	return;
}




void
n_freecell_reposition( n_freecell *p )
{

	// Reset : when resized

	int x,y;


	x = 0;
	n_posix_loop
	{

		n_game_chara *c;


		c = &p->freecell[ x ];

		n_game_chara_pos( c, p->cardgen.card_sx * x, 0 );
		n_game_chara_prv( c );


		c = &p->tableau[ x ];

		n_game_chara_pos( c, p->cardgen.card_sx * x, 0 );
		n_game_chara_prv( c );


		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	x = 0;
	n_posix_loop
	{
		n_game_chara *c = &p->stock[ x ];

		n_game_chara_pos( c, p->cardgen.csx - ( p->cardgen.card_sx * 4 ) + ( p->cardgen.card_sx * x ), 0 );
		n_game_chara_prv( c );

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	n_type_gfx tmp_sx = p->cardgen.csx / N_FREECELL_PILE_SX;
	if ( tmp_sx > p->cardgen.card_sx )
	{
		p->cardgen.osx = (n_type_gfx) ( fmod( tmp_sx, p->cardgen.card_sx ) / 2 );
	} else {
		p->cardgen.osx = 0;
	}

//n_game_hwndprintf_literal( "%d : %d %d", p->cardgen.osx, p->cardgen.csx, p->cardgen.card_sx );
//n_game_hwndprintf_literal( "%d %d", ( p->cardgen.csx / N_FREECELL_PILE_SX ), fmod( ( p->cardgen.csx / N_FREECELL_PILE_SX ), p->cardgen.card_sx ) );

	x = 0;
	y = 0;
	n_posix_loop
	{

		int target = n_freecell_pos2index( x,y );

		n_game_chara *c = &p->tableau[ target ];


		n_type_gfx tx = ( ( p->cardgen.osx + p->cardgen.card_sx + p->cardgen.osx ) * x ) + p->cardgen.osx;
		n_type_gfx ty = p->cardgen.card_sy + ( p->cardgen.osy * y );

		n_game_chara_pos( c, tx,ty );
		n_game_chara_prv( c );


		x++;
		if ( x >= N_FREECELL_PILE_SX )
		{

			x = 0;

			y++;
			if ( y >= N_FREECELL_PILE_SY ) { break; }
		}
	}

	x = 0;
	n_posix_loop
	{

		n_game_chara *c = &p->bottom[ x ];


		n_type_gfx tx = ( ( p->cardgen.osx + p->cardgen.card_sx + p->cardgen.osx ) * x ) + p->cardgen.osx;

		n_game_chara_pos( c, tx,p->cardgen.card_sy );
		n_game_chara_prv( c );


		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	return;
}




void
n_freecell_shuffle( n_freecell *p )
{

	if ( p->debug_onoff ) { return; }

	int i = 0;
	n_posix_loop
	{//break;

		int r = n_random_range_hq( N_FREECELL_CARD_ALL );

		int swap        = p->shuffle[ i ];
		p->shuffle[ i ] = p->shuffle[ r ];
		p->shuffle[ r ] = swap;


		i++;
		if ( i >= N_FREECELL_CARD_ALL ) { break; }
	}


	return;
}




void
n_freecell_reset( n_freecell *p, BOOL is_init )
{
//u32 tick = n_posix_tickcount();

	// Reset : once per session

	p->drag_index = N_FREECELL_NOTHING;


	p->is_done = FALSE;


	if ( is_init )
	{
		n_game_chara_bulk_zero( p->bottom,   N_FREECELL_PILE_SX  );
		n_game_chara_bulk_zero( p->tableau,  N_FREECELL_PILE_ALL );
		n_game_chara_bulk_zero( p->freecell, N_FREECELL_SUIT_MAX );
		n_game_chara_bulk_zero( p->stock,    N_FREECELL_SUIT_MAX );
	}


	int x,y;


	x = 0;
	n_posix_loop
	{

		n_bmp *bmp_bg = &p->bmp_bg;

		n_game_chara_bmp( &p->freecell[ x ], p->canvas, &p->bmp_freecell, bmp_bg, p->color_bg );
		n_game_chara_src( &p->freecell[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );

		p->freecell[ x ].data = N_FREECELL_NOTHING;

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}

	x = 0;
	n_posix_loop
	{

		n_bmp *bmp_bg = &p->bmp_bg;

		if ( is_init )
		{
			n_game_chara_bmp( &p->tableau[ x ], p->canvas, NULL, bmp_bg, p->color_bg );
			n_game_chara_src( &p->tableau[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );

			p->tableau[ x ].data = N_FREECELL_NOTHING;
		} else {
			n_game_chara_src( &p->tableau[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );
		}

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}

	x = 0;
	n_posix_loop
	{

		n_bmp *bmp_bg = &p->bmp_bg;

		if ( is_init )
		{
			n_game_chara_bmp( &p->stock[ x ], p->canvas, &p->bmp_stock[ x ], bmp_bg, p->color_bg );
			n_game_chara_src( &p->stock[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );

			p->stock[ x ].data = N_FREECELL_NOTHING;
		} else {
			n_game_chara_src( &p->stock[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );
		}

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}

	x = 0;
	n_posix_loop
	{

		p->animation_server[ x ] = N_FREECELL_NOTHING;

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	x = 0;
	n_posix_loop
	{

		n_bmp *bmp_bg = &p->bmp_bg;

		n_game_chara_bmp( &p->bottom[ x ], p->canvas, &p->cardgen.bmp_foundation, bmp_bg, p->color_bg );
		n_game_chara_src( &p->bottom[ x ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );

		p->bottom[ x ].data = N_FREECELL_NOTHING;


		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	x = y = 0;
	int i = 0;
	n_posix_loop
	{

		int    target = n_freecell_pos2index( x,y );
		n_bmp *bmp_bg = &p->bmp_bg;

		if ( is_init )
		{
			n_game_chara_bmp( &p->tableau[ target ], p->canvas, NULL, bmp_bg, p->color_bg );
			n_game_chara_src( &p->tableau[ target ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );


			if ( i < N_FREECELL_CARD_ALL )
			{
				p->tableau[ target ].chara = &p->cardgen.cards[ p->shuffle[ i ] ];
				p->tableau[ target ].data  = p->shuffle[ i ];
				i++;
			} else {
				p->tableau[ target ].chara = NULL;
				p->tableau[ target ].data  = N_FREECELL_NOTHING;
			}
		} else {
			n_game_chara_src( &p->tableau[ target ], 0,0, p->cardgen.card_sx,p->cardgen.card_sy, 0,0 );
		}

		x++;
		if ( x >= N_FREECELL_PILE_SX )
		{

			x = 0;

			y++;
			if ( y >= N_FREECELL_PILE_SY ) { break; }
		}
	}


	n_freecell_shadow( p );


	p->color_halo_current = p->color_halo;


//n_posix_debug_literal( " %d ", n_posix_tickcount() - tick );
	return;
}




void
n_freecell_redirect( n_freecell *p, n_bmp *bmp )
{

	int x,y;


	x = 0;
	n_posix_loop
	{

		p->freecell[ x ].main = bmp;

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}

	x = 0;
	n_posix_loop
	{

		p->tableau[ x ].main = bmp;

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}

	x = 0;
	n_posix_loop
	{

		p->stock[ x ].main = bmp;

		x++;
		if ( x >= N_FREECELL_SUIT_MAX ) { break; }
	}


	x = 0;
	n_posix_loop
	{

		p->bottom[ x ].main = bmp;


		x++;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	x = y = 0;
	n_posix_loop
	{

		int target = n_freecell_pos2index( x,y );

		p->tableau[ target ].main = bmp;

		x++;
		if ( x >= N_FREECELL_PILE_SX )
		{

			x = 0;

			y++;
			if ( y >= N_FREECELL_PILE_SY ) { break; }
		}
	}


	return;
}




void
n_freecell_newgame( n_freecell *p )
{

	n_bmp_carboncopy( p->canvas, &p->transition_bmp_old );


	n_freecell_shuffle( p );
	n_freecell_reset( p, TRUE );
	n_freecell_reposition( p );
	n_freecell_redraw( p );


	n_bmp_carboncopy( &p->bmp_bg, &p->transition_bmp_new );
	n_freecell_redirect( p, &p->transition_bmp_new );

	p->refresh = TRUE;
	extern void n_freecell_loop( n_freecell *p );
	n_freecell_loop( p );

	n_freecell_redirect( p, p->canvas );


	p->transition_stage = 0;
	p->transition_type  = N_BMP_UI_TRANSITION_MOSAIC;
	p->transition_onoff = TRUE;


	return;
}

void
n_freecell_replay( n_freecell *p )
{

	n_bmp_carboncopy( p->canvas, &p->transition_bmp_old );


	n_freecell_reset( p, TRUE );
	n_freecell_reposition( p );
	n_freecell_redraw( p );


	n_bmp_carboncopy( &p->bmp_bg, &p->transition_bmp_new );
	n_freecell_redirect( p, &p->transition_bmp_new );

	p->refresh = TRUE;
	extern void n_freecell_loop( n_freecell *p );
	n_freecell_loop( p );

	n_freecell_redirect( p, p->canvas );


	p->transition_stage = 0;
	p->transition_type  = N_BMP_UI_TRANSITION_WHIRL;
	p->transition_onoff = TRUE;


	return;
}




void
n_freecell_flush_background_cached( n_freecell *p )
{

	{

		n_bmp_flush( &p->bmp_bg, p->color_gradient_upper );

		n_type_gfx sx = p->sx;
		n_type_gfx sy = p->sy / 3;

		n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy );
		u32 color = n_bmp_alpha_replace_pixel( p->color_gradient_upper, 0 );
		n_bmp_flush_gradient( &b, color, p->color_gradient_lower, N_BMP_GRADIENT_VERTICAL );

		n_bmp_transcopy( &b, &p->bmp_bg, 0,0,sx,sy, 0, ( sy * 2 ) + 2 );
		n_bmp_free_fast( &b );

	}


	return;
}

void
n_freecell_flush_background( n_freecell *p )
{

	{

		n_bmp_flush( p->canvas, p->color_gradient_upper );

		n_type_gfx sx = p->sx;
		n_type_gfx sy = p->sy / 3;

		n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy );
		u32 color = n_bmp_alpha_replace_pixel( p->color_gradient_upper, 0 );
		n_bmp_flush_gradient( &b, color, p->color_gradient_lower, N_BMP_GRADIENT_VERTICAL );

		n_bmp_transcopy( &b, p->canvas, 0,0,sx,sy, 0, ( sy * 2 ) + 2 );
		n_bmp_free_fast( &b );

	}

	n_bmp_free( &p->bmp_bg );
	n_bmp_carboncopy( p->canvas, &p->bmp_bg );


	return;
}




void
n_freecell_accentcolor_changed( n_freecell *p )
{

	{

		n_bmp_flush( p->canvas, p->color_gradient_upper );

		n_type_gfx sx = p->sx;
		n_type_gfx sy = p->sy / 3;

		n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy );
		u32 color = n_bmp_alpha_replace_pixel( p->color_gradient_upper, 0 );
		n_bmp_flush_gradient( &b, color, p->color_gradient_lower, N_BMP_GRADIENT_VERTICAL );

		n_bmp_transcopy( &b, p->canvas, 0,0,sx,sy, 0, ( sy * 2 ) + 2 );
		n_bmp_free_fast( &b );

	}

	n_bmp_free( &p->bmp_bg );
	n_bmp_carboncopy( p->canvas, &p->bmp_bg );


	return;
}

void
n_freecell_style_change_color( n_freecell *p )
{
#ifdef N_POSIX_PLATFORM_MAC
	u32 accent = n_bmp_color_mac( n_mac_nscolor2argb( [NSColor controlAccentColor] ) );
#else
	u32 accent = n_win64_GetSysColor_nbmp( COLOR_ACTIVECAPTION );
#endif
//NSLog( @"%x", accent );

	if ( n_cardgenerator_darkmode_is_on() )
	{
		p->cardgen.color_margin     = n_bmp_black_invisible;
		p->        color_halo       = n_bmp_rgb_mac( 11,11,11 );
		p->        color_halo_focus = accent;

		p->color_gradient_lower = accent;
		p->color_gradient_upper = n_bmp_rgb_mac( 33,33,33 );
	} else {
		p->cardgen.color_margin     = n_bmp_white_invisible;
		p->        color_halo       = accent;
		p->        color_halo_focus = n_bmp_rgb_mac( 255,255,0 );

		p->color_gradient_upper = accent;
		p->color_gradient_lower = n_bmp_white;
	}

	p->color_halo_current = p->color_halo;


	return;
}

void
n_freecell_style_change( n_freecell *p )
{

	n_freecell_style_change_color( p );

	n_cardgenerator_exit( &p->cardgen );
	n_cardgenerator_init( &p->cardgen, 10 );


	return;
}




void
n_freecell_placeholder_free( n_freecell *p )
{

	int i = 0;
	n_posix_loop
	{

		n_bmp_free( &p->bmp_stock[ i ] );

		i++;
		if ( i >= N_FREECELL_SUIT_MAX ) { break; }
	}


	return;
}

void
n_freecell_placeholder_make( n_freecell *p )
{

	int i = 0;
	n_posix_loop
	{

		n_bmp_carboncopy( &p->cardgen.cards[ N_CARDGENERATOR_CARD_UNIT * i ], &p->bmp_stock[ i ] );
		n_cardgenerator_gray( &p->bmp_stock[ i ] );

		i++;
		if ( i >= N_FREECELL_SUIT_MAX ) { break; }
	}


	return;
}




void
n_freecell_cardgenerator( n_freecell *p )
{

	n_freecell_style_change_color( p );


	// [!] : Freecell Area

	const u32 freecell = n_bmp_argb( 50, 255,255,255 );

	n_cardgenerator_base( &p->cardgen, &p->bmp_freecell, freecell, 0 );

	u32 color_frame  = p->cardgen.color_frame ; p->cardgen.color_frame  = n_bmp_white_invisible;
	u32 color_margin = p->cardgen.color_margin; p->cardgen.color_margin = n_bmp_black_invisible;

	n_cardgenerator_resampler( &p->cardgen, &p->bmp_freecell, n_bmp_black_invisible );

	p->cardgen.color_frame  = color_frame;
	p->cardgen.color_margin = color_margin;


	// [!] : Placeholder

	n_freecell_placeholder_make( p );


	// [!] : Shadow

	int i = 0;
	n_posix_loop
	{

		n_bmp_free( &p->shadow_bmp_cache[ i ] );

		i++;
		if ( i >= N_FREECELL_SHADOW_COUNT ) { break; }
	}


	return;
}

