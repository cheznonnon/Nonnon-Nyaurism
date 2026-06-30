// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism] : terminology
//
//	[ Stock ]
//
//	+ upper-right corner
//	+ place where stack A to K
//
//	[ Freecell ]
//
//	+ upper-left corner
//	+ 4 cards only puttable
//
//	[ Tableau ]
//
//	+ main play area



#include "extern.c"


// [!] : order is important

#include "sound.c"
#include "shadow.c"
#include "shared.c"
#include "rule.c"
#include "automove.c"
#include "input.c"
#include "pile.c"




void
n_freecell_init( n_freecell *p )
{

	// Global #1

	n_bmp_safemode_base = n_bmp_safemode = FALSE;

	n_random_shuffle();

	n_freecell_style_change( p );

	p->shadow_global_onoff = TRUE;

	//n_game_transition_percent_smooth = TRUE;


	// [Needed] : reset() sets too but initializer calls earlier

	p->drag_index = N_FREECELL_NOTHING;


	// System

	p->sx       = p->cardgen.csx;
	p->sy       = p->cardgen.csy;
	p->color_bg = n_bmp_white;


#ifdef N_POSIX_PLATFORM_MAC

	p->queue = [[NSOperationQueue alloc] init];

	n_bmp_new( &p->canvas_base, p->sx, p->sy );
	p->rep = n_mac_image_NSBitmapImageRep( &p->canvas_base );
	n_mac_image_imagerep_alias( p->rep, &p->canvas_alias );
	p->canvas = &p->canvas_alias;

	// Resource

	n_mac_sound2_init( &n_freecell_snd_click  , @"click"  , @"wav" );
	n_mac_sound2_init( &n_freecell_snd_fanfare, @"fanfare", @"wav" );

#else

	p->canvas = &game.bmp;

	n_bmp_new_fast( p->canvas, p->sx, p->sy );

	p->is_first = TRUE;

#endif


#ifdef _H_NONNON_WIN32_GAME_CLICK

	n_game_click_init( &p->doubleclick, VK_LBUTTON );

#endif


	// New Game

	int i = 0;
	n_posix_loop
	{

		p->shuffle[ i ] = N_FREECELL_CARD_ALL - 1 - i;

		i++;
		if ( i >= N_FREECELL_CARD_ALL ) { break; }
	}

	n_freecell_shuffle( p );


	// Debug : Automove

	    i = 0;
	int j = 0;
	while( p->debug_onoff )
	{//break;

		p->shuffle[ i + 3 ] = ( ( 1 + N_FREECELL_SUIT_HEARTS   ) * N_FREECELL_CARD_UNIT ) - 1 - j;
		p->shuffle[ i + 2 ] = ( ( 1 + N_FREECELL_SUIT_DIAMONDS ) * N_FREECELL_CARD_UNIT ) - 1 - j;
		p->shuffle[ i + 1 ] = ( ( 1 + N_FREECELL_SUIT_SPADES   ) * N_FREECELL_CARD_UNIT ) - 1 - j;
		p->shuffle[ i + 0 ] = ( ( 1 + N_FREECELL_SUIT_CLUBS    ) * N_FREECELL_CARD_UNIT ) - 1 - j;

		j++;

		i += 4;
		if ( i >= N_FREECELL_CARD_ALL ) { break; }
	}

	if ( p->debug_onoff ) { p->restore_count = 4; }


	return;
}

void
n_freecell_exit( n_freecell *p )
{

	n_cardgenerator_exit( &p->cardgen );


	n_freecell_placeholder_free( p );


	n_bmp_free_fast( &p->bmp_bg       );
	n_bmp_free_fast( &p->bmp_freecell );

	int i = 0;
	n_posix_loop
	{

		n_bmp_free_fast( &p->shadow_bmp_cache[ i ] );

		i++;
		if ( i >= N_FREECELL_SHADOW_COUNT ) { break; }
	}


	n_freecell_sound_exit( p );


	n_freecell_zero( p );


	//n_memory_debug_refcount();


	return;
}


void
n_freecell_loop( n_freecell *p )
{

	n_cardgenerator_loop( &p->cardgen );

	if ( p->is_first )
	{
//NSLog( @"is_first" );

		p->is_first = FALSE;

#ifdef N_POSIX_PLATFORM_MAC

		n_bmp_flip_onoff = FALSE;

		n_freecell_cardgenerator( p );

		n_bmp_flip_onoff = TRUE;

#else

		n_freecell_cardgenerator( p );

#endif


		p->splash_onoff = TRUE;

		n_freecell_flush_background( p );

		n_freecell_reset( p, TRUE );
		n_freecell_reposition( p );
		n_freecell_redraw( p );


		n_bmp_carboncopy( &p->bmp_bg, &p->transition_bmp_old );

		n_bmp_carboncopy( &p->bmp_bg, &p->transition_bmp_new );
		n_freecell_redirect( p, &p->transition_bmp_new );

		p->refresh = TRUE;
		extern void n_freecell_loop( n_freecell* );
		n_freecell_loop( p );

		n_freecell_redirect( p, p->canvas );


		p->transition_stage = 0;
		p->transition_type  = N_BMP_UI_TRANSITION_FADE;
		p->transition_onoff = TRUE;

//n_bmp_save_literal( &p->transition_bmp_old, "old.bmp" );
//n_bmp_save_literal( &p->transition_bmp_new, "new.bmp" );

	}


	if ( p->transition_onoff )
	{
//NSLog( @"transition_onoff" );

#ifdef N_POSIX_PLATFORM_MAC
		p->self.window.styleMask &= ~NSWindowStyleMaskResizable;
#endif

		u32 msec = N_FREECELL_FADE_MSEC;
		if ( p->transition_type == N_BMP_UI_TRANSITION_NOTHING )
		{
			msec = 1000;
		} else
		if ( p->transition_type != N_BMP_UI_TRANSITION_FADE )
		{
			msec = 2000;
		}

		BOOL ret = n_bmp_ui_transition
		(
			p->canvas,
			&p->transition_bmp_old,
			&p->transition_bmp_new,
			msec,
			&p->transition_percent,
			p->transition_type
			//N_BMP_UI_TRANSITION_INFLATE
		);
		if ( ret )
		{
			if ( p->transition_stage == 0 )
			{
				p->transition_stage = 1;
				p->transition_type  = N_BMP_UI_TRANSITION_NOTHING;
				n_bmp_flush_fastcopy( p->canvas, &p->transition_bmp_old );
				n_bmp_flush_fastcopy( p->canvas, &p->transition_bmp_new );
			} else {
				p->transition_onoff = FALSE;

				n_bmp_free( &p->transition_bmp_old );
				n_bmp_free( &p->transition_bmp_new );

#ifdef N_POSIX_PLATFORM_MAC
				p->self.window.styleMask |= NSWindowStyleMaskResizable;
#endif
			}
		}

		p->refresh = TRUE;
		return;
	}


	// Input
//n_game_hwndprintf_literal( " %d ", p->animation_onoff );

#ifdef N_POSIX_PLATFORM_MAC
	if ( [n_mac_image_window isKeyWindow] )
#endif
	{

#ifdef _H_NONNON_WIN32_GAME_CLICK

		if ( n_game_click_double( &p->doubleclick ) )
		{
			n_freecell_input_doubleclick( p );
		} else
		if ( FALSE == ( p->doubleclick.phase == N_GAME_CLICK_PHASE_DOUBLE ) )
		{
			n_freecell_input( p );
		}

#else

		if ( p->doubleclick )
		{
			n_freecell_input_doubleclick( p );
		} else
		if ( p->click_phase == 1 )
		{
			n_freecell_input( p );
		}

#endif

	}


	// Automove

	if (
		( p->         refresh == FALSE )
		&&
		( p-> animation_onoff == FALSE )
		&&
		( p->transition_onoff == FALSE )
	)
	{
		n_freecell_automove_engine( p );
	}


//n_game_hwndprintf_literal( " %d %d %d %d ", p->stock[ 0 ].data, p->stock[ 1 ].data, p->stock[ 2 ].data, p->stock[ 3 ].data );

	if ( p->is_done == FALSE )
	{
		if (
			( p->stock[ 0 ].data == 12 )
			&&
			( p->stock[ 1 ].data == 25 )
			&&
			( p->stock[ 2 ].data == 38 )
			&&
			( p->stock[ 3 ].data == 51 )
		)
		{
			p->is_done = TRUE;
			n_freecell_sound_play( p, N_FREECELL_SOUND_FANFARE );
		}
	}


	// Auto Animation

	if ( ( p->refresh == FALSE )&&( p->animation_onoff ) )
	{
//NSLog( @"Auto Animation" );

//static int i = 0;
//n_game_hwndprintf_literal( " %d : %f %f ", i, p->animation_step_x, p->animation_step_y );
//i++;

		static u32 timer = 0;
		if ( n_bmp_ui_timer( &timer, N_FREECELL_ANIMATION_INTERVAL ) )
		{

			n_type_gfx ox = p->cardgen.card_sx / 2;
			n_type_gfx oy = p->cardgen.card_sy / 2;

			if ( n_game_chara_is_hit_offset( &p->tableau[ p->animation_index_f ], &p->stock[ p->animation_index_t ], ox,oy, 0,0 ) )
			{
				p->animation_onoff = FALSE;
				p->drag_index      = N_FREECELL_NOTHING;
				n_freecell_rule_stock( p, p->animation_index_f, p->animation_index_t, NULL );

				n_freecell_automove_engine( p );
			} else {
				n_game_chara *c = &p->tableau[ p->animation_index_f ];

				p->animation_base_x += p->animation_step_x;
				p->animation_base_y += p->animation_step_y;

				c->x = (n_type_gfx) p->animation_base_x;
				c->y = (n_type_gfx) p->animation_base_y;
			}

			//n_freecell_redraw( p );
			p->refresh = TRUE;

		}

	}


	if ( p->refresh )
	{

//n_game_hwndprintf_literal( "%d", p->restore_count );


		// Erase

		// [!] : Safe Mode

		//n_freecell_redraw( p );
		//n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );


		// [!] : Fast Mode

		{

			if ( p->drag_index != N_FREECELL_NOTHING )
			{

				n_freecell_redraw_detect( p );

				int i = 0;
				n_posix_loop
				{

					int upper = p->drag_index + n_freecell_index_upper( i );

					n_game_chara *c = &p->tableau[ upper ];

					n_freecell_chara_erase( p, c );

					i++;
					if ( i >= p->restore_count ) { break; }
				}

			}

		}


		{

			int x = 0;
			n_posix_loop
			{

				n_game_chara_erase( &p->freecell[ x ] );

				x++;
				if ( x >= N_FREECELL_SUIT_MAX ) { break; }
			}

		}


		{

			int x = 0;
			n_posix_loop
			{

				n_game_chara_erase( &p->stock[ x ] );

				x++;
				if ( x >= N_FREECELL_SUIT_MAX ) { break; }
			}

		}


		n_freecell_shadow_erase( p );

#ifdef N_POSIX_PLATFORM_MAC

		{

			BOOL prv = n_bmp_is_multithread;
			n_bmp_is_multithread = TRUE;

			int x = 0;
			n_posix_loop
			{

				NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

				if ( p->redraw[ x ] ) { n_freecell_pile_erase( p, x, 0 ); }

				}];
				[p->queue addOperation:o];


				x++;
				if ( x >= N_FREECELL_PILE_SX ) { break; }
			}

			[p->queue waitUntilAllOperationsAreFinished];

			n_bmp_is_multithread = prv;

		}

#else

		{

			int x = 0;
			n_posix_loop
			{

				if ( p->redraw[ x ] ) { n_freecell_pile_erase( p, x, 0 ); }

				x++;
				if ( x >= N_FREECELL_PILE_SX ) { break; }
			}

		}

#endif


		// Draw

		{

			int x = 0;
			n_posix_loop
			{

				n_game_chara_draw( &p->freecell[ x ] );

				x++;
				if ( x >= N_FREECELL_SUIT_MAX ) { break; }
			}

		}

		{

			int x = 0;
			n_posix_loop
			{

				n_game_chara_draw( &p->stock[ x ] );

				x++;
				if ( x >= N_FREECELL_SUIT_MAX ) { break; }
			}

		}


		{

			int x = 0;
			n_posix_loop
			{

				n_game_chara_draw( &p->tableau[ x ] );

				x++;
				if ( x >= N_FREECELL_SUIT_MAX ) { break; }
			}

		}


		n_freecell_pile_draw_all( p );


		if ( p->drag_index != N_FREECELL_NOTHING )
		{

			n_freecell_shadow_draw( p );

			int i = 0;
			n_posix_loop
			{//break;

				int upper = p->drag_index + n_freecell_index_upper( i );

				n_game_chara *c = &p->tableau[ upper ];

				n_freecell_chara_draw( p, c );
//n_bmp_box( &game.bmp, c->x, c->y, c->sx, c->sy, n_bmp_rgb( 0,200,255 ) );

				i++;
				if ( i >= p->restore_count ) { break; }
			}

		}

	}


	return;
}

