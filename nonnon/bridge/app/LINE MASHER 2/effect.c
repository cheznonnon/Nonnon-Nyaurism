// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define n_lm2_effect_maxcount 20




typedef struct {

	n_type_gfx cur,prv;

	n_type_gfx step;
	n_type_gfx hoop;

	u32        color;

} n_game_effect;


typedef struct {

	n_game_chara  chara[ n_lm2_effect_maxcount ];
	n_game_effect state[ n_lm2_effect_maxcount ];

} n_lm2_effect;


static n_lm2_effect n_lm2_effect_instance;




// internal
void
n_lm2_effect_erase( n_lm2 *p, int index )
{

	n_game_chara  *c = &n_lm2_effect_instance.chara[ index ];
	n_game_effect *e = &n_lm2_effect_instance.state[ index ];

	if ( e->prv == 0 ) { return; }


	n_type_gfx sx = e->prv;
	n_type_gfx sy = e->prv;
	n_type_gfx  x = c->x + ( ( c->sx - sx ) / 2 );
	n_type_gfx  y = c->y + ( ( c->sy - sy ) / 2 );

	if ( c->bg != NULL )
	{
		n_bmp_fastcopy( c->bg, c->main, x,y,sx,sy, x,y );
	} else {
		n_bmp_box( c->main, x,y,sx,sy, c->bgcolor );
	}


	p->refresh = n_posix_true;
	return;
}

// internal
void
n_lm2_effect_draw( n_lm2 *p, int index )
{

	n_game_chara  *c = &n_lm2_effect_instance.chara[ index ];
	n_game_effect *e = &n_lm2_effect_instance.state[ index ];

	if ( e->cur == 0 ) { return; }


	n_type_gfx sx = e->cur;
	n_type_gfx sy = e->cur;
	n_type_gfx  x = c->x + ( ( c->sx - sx ) / 2 );
	n_type_gfx  y = c->y + ( ( c->sy - sy ) / 2 );

	n_bmp_hoop( c->main, x,y,sx,sy, e->hoop, e->color );


	e->prv = e->cur;

	e->cur += e->step;
	if ( e->cur >= c->sx ) { e->cur = 0; }


	p->refresh = n_posix_true;
	return;
}

void
n_lm2_effect_go( n_game_chara *target )
{

	static int index = 0;

	index++;
	if ( index >= n_lm2_effect_maxcount ) { index = 0; }


	// [!] : turn ON

	n_game_chara  *c = &n_lm2_effect_instance.chara[ index ];
	n_game_effect *e = &n_lm2_effect_instance.state[ index ];

	e->prv = e->cur = 1;

	c->x = target->x + ( ( target->sx - c->sx ) / 2 );
	c->y = target->y + ( ( target->sy - c->sy ) / 2 );


	return;
}

void
n_lm2_effect_init( n_lm2 *p, n_bmp *bmp_bg )
{

	const n_type_gfx size = p->csx /  8;
	const n_type_gfx step =   size / 16;
	const n_type_gfx hoop =   size / 16;
	const u32        fg   = n_bmp_rgb_mac( 222, 255, 255 );


	int i = 0;
	n_posix_loop
	{

		n_game_chara  *c = &n_lm2_effect_instance.chara[ i ];
		n_game_effect *e = &n_lm2_effect_instance.state[ i ];

		n_game_chara_zero( c );
		n_game_chara_bmp ( c, p->canvas, NULL, bmp_bg, p->color );
		n_game_chara_src ( c, 0,0,size,size, 0,0 );

		n_memory_zero( e, sizeof( n_game_effect ) );
		e->step  = step;
		e->hoop  = hoop;
		e->color = fg;

		i++;
		if ( i >= n_lm2_effect_maxcount ) { break; }
	}


	return;
}

void
n_lm2_effect_loop_erase( n_lm2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_lm2_effect_erase( p, i );

		i++;
		if ( i >= n_lm2_effect_maxcount ) { break; }
	}


	return;
}

void
n_lm2_effect_loop_draw( n_lm2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_lm2_effect_draw( p, i );

		i++;
		if ( i >= n_lm2_effect_maxcount ) { break; }
	}


	return;
}

void
n_lm2_effect_exit( void )
{
	return;
}

