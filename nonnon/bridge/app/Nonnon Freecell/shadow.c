// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_freecell_shadow( n_freecell *p )
{

	if ( p->shadow_global_onoff == FALSE ) { return; }

	//if ( p->animation_onoff ) { return; }


	int index = 0;
	if ( p->restore_count >= 1 ) { index = p->restore_count - 1; }


	p->shadow_bmp = &p->shadow_bmp_cache[ index ];

	if ( NULL != N_BMP_PTR( p->shadow_bmp ) )
	{
		return;
	}


	if ( p->drag_index < 0 ) { return; }
	n_game_chara *c = &p->tableau[ p->drag_index ];

	n_type_gfx osy = 0; if ( p->restore_count >= 2 ) { osy = p->cardgen.osy * ( p->restore_count - 1 ); }

	p->shadow_x  = c->x + ( p->cardgen.halo * 2 );
	p->shadow_y  = c->y + ( p->cardgen.halo * 2 );
	p->shadow_sx = c->sx;
	p->shadow_sy = c->sy + osy;

	n_bmp bmp_tmp; n_bmp_zero( &bmp_tmp );
	n_cardgenerator_dropshadow( &bmp_tmp, p->shadow_sx, p->shadow_sy, p->cardgen.halo, p->cardgen.halo / 2 );

	n_bmp_new_fast( p->shadow_bmp, N_BMP_SX( &bmp_tmp ),N_BMP_SY( &bmp_tmp ) );
	n_bmp_flush( p->shadow_bmp, n_bmp_black_invisible );

	u32 shadow_color = n_bmp_blend_pixel( p->color_gradient_upper, n_bmp_black_invisible, 0.5 );
	n_bmp_rasterizer( &bmp_tmp, p->shadow_bmp, 0,0, shadow_color );
//n_bmp_save_literal( p->shadow_bmp, "ret.bmp" );

	n_bmp_free_fast( &bmp_tmp );


	return;
}

void
n_freecell_shadow_erase( n_freecell *p )
{

	if ( p->shadow_global_onoff == FALSE ) { return; }

	//if ( p->animation_onoff ) { return; }

	if ( p->shadow_onoff == FALSE ) { return; }


	p->shadow_onoff = FALSE;


	n_type_gfx  x = p->shadow_px;
	n_type_gfx  y = p->shadow_py;
	n_type_gfx sx = p->shadow_psx;
	n_type_gfx sy = p->shadow_psy;

//n_bmp_box( &game.bmp, x,y,sx,sy, n_bmp_rgb(0,200,255) );
	n_bmp_fastcopy( &p->bmp_bg, p->canvas, x,y,sx,sy, x,y );


	n_game_chara a; n_game_chara_zero( &a );
	n_game_chara_pos( &a, x,y );
	n_game_chara_src( &a, 0,0, sx,sy, 0,0 );

	n_type_gfx ix = 0;
	n_type_gfx iy = 0;
	n_posix_loop
	{//break;

		int target = n_freecell_pos2index( ix, iy );
		n_game_chara *b = &p->tableau[ target ];

		if ( n_game_chara_is_hit( &a, b ) )
		{
			p->redraw[ ix ] = TRUE;
		}

		ix++;
		if ( ix >= N_FREECELL_PILE_SX )
		{

			ix = 0;

			iy++;
			if ( iy >= N_FREECELL_PILE_SY ) { break; }
		}
	}


	return;
}

void
n_freecell_shadow_draw( n_freecell *p )
{

	if ( p->shadow_global_onoff == FALSE ) { return; }

	//if ( p->animation_onoff ) { return; }


	p->shadow_onoff = TRUE;

	n_game_chara *c = &p->tableau[ p->drag_index ];

	n_type_gfx ox = ( p->cardgen.halo * 1 );
	n_type_gfx oy = ( p->cardgen.halo * 1 );
	n_type_gfx  x = c->x + ox;
	n_type_gfx  y = c->y + oy;
	n_type_gfx sx = N_BMP_SX( p->shadow_bmp );
	n_type_gfx sy = N_BMP_SY( p->shadow_bmp );

	//n_bmp_rasterizer( p->shadow_bmp, &game.bmp, x,y, n_bmp_rgb( 50,50,50 ) );
	//n_bmp_transcopy( p->shadow_bmp, &game.bmp, 0,0,sx,sy, x,y );

	n_bmp_clipcopy( p->shadow_bmp, p->canvas, 0,0,sx,sy, x,y, c->chara,ox,oy, 0.0 );
//n_bmp_save_literal( c->chara, "ret.bmp" );


	p->shadow_px  = x;
	p->shadow_py  = y;
	p->shadow_psx = N_BMP_SX( p->shadow_bmp );
	p->shadow_psy = N_BMP_SY( p->shadow_bmp );


	return;
}


