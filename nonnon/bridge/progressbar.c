// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GAME_PROGRESSBAR
#define _H_NONNON_WIN32_GAME_PROGRESSBAR




#include "../neutral/bmp/all.c"


#include "./helper.c"




// [!] : currently handled as hidden parameters


// [!] : zero == OFF

static u32  n_game_progressbar_animation_interval = 10;


#define N_GAME_PROGRESSBAR_ANIMATION_OFF     0
#define N_GAME_PROGRESSBAR_ANIMATION_ON_UP   1
#define N_GAME_PROGRESSBAR_ANIMATION_ON_DOWN 2

static int n_game_progressbar_animation = N_GAME_PROGRESSBAR_ANIMATION_OFF;


static n_posix_bool n_game_progressbar_solid        = n_posix_false;
static n_posix_bool n_game_progressbar_no_round     = n_posix_false;
static n_type_gfx   n_game_progressbar_round_div    = 2;
static n_type_gfx   n_game_progressbar_border       = 0;
static u32          n_game_progressbar_border_color = n_bmp_black + 1;




n_posix_bool
n_game_progressbar_stripe_detect( n_type_gfx x, n_type_gfx y, n_type_gfx animation, n_type_gfx stripe )
{

	// [Mechanism]
	//
	//	Formula   : ( ( n % stripe_size ) < ( stripe_size / 2 ) )
	//	animation : 0 to stripe


	n_type_gfx n = x + y;


	// [Patch] : top-left corner is not rendered

	n += stripe;


	if ( n_game_progressbar_animation == N_GAME_PROGRESSBAR_ANIMATION_ON_UP   )
	{
		n -= animation;
	} else
	if ( n_game_progressbar_animation == N_GAME_PROGRESSBAR_ANIMATION_ON_DOWN )
	{
		n += animation;
	}


	if ( ( n % stripe ) < ( stripe / 2 ) ) { return n_posix_true; }


	return n_posix_false;
}

#define N_GAME_PROGRESSBAR_HORIZONTAL 0
#define N_GAME_PROGRESSBAR_VERTICAL   1

#define n_game_progressbar_vert( b, x,y,sx,sy, fg,bg, pc, s ) n_game_progressbar( b, x,y,sx,sy, fg,bg, pc, s, N_GAME_PROGRESSBAR_VERTICAL   )
#define n_game_progressbar_horz( b, x,y,sx,sy, fg,bg, pc, s ) n_game_progressbar( b, x,y,sx,sy, fg,bg, pc, s, N_GAME_PROGRESSBAR_HORIZONTAL )

void
n_game_progressbar
(
	n_bmp          *bmp,
	n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy,
	u32              fg,
	u32              bg,
	n_type_real percent,
	n_type_gfx   stripe,
	int            mode
)
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	if ( percent > 100 ) { percent = 100; }


	const n_type_real pc = percent * 0.01;


	if ( stripe == 0 )
	{

		n_type_gfx fx,fy,fsx,fsy;
		n_type_gfx tx,ty,tsx,tsy;


		if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
		{

			fx  = x;
			fy  = y;
			fsx = (n_type_gfx) ( (n_type_real) sx * pc );
			fsy = sy;
			tx  = x + fsx;
			ty  = y;
			tsx = sx - fsx;
			tsy = sy;

		} else {

			fx  = x;
			fy  = y;
			fsx = sx;
			fsy = (n_type_gfx) ( (n_type_real) sy * pc );
			tx  = x;
			ty  = y + fsy;
			tsx = sx;
			tsy = sy - fsy;

		}

		n_bmp_box( bmp, fx,fy,fsx,fsy, fg );
		n_bmp_box( bmp, tx,ty,tsx,tsy, bg );

	} else
	if ( n_game_progressbar_solid )
	{

		// [!] : metrics

		if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
		{
			sx = n_posix_max_n_type_gfx( sx, sy );
		} else {
			sy = n_posix_max_n_type_gfx( sx, sy );
		}

		if ( n_game_progressbar_round_div < 2 ) { n_game_progressbar_round_div = 2; }

		const n_type_gfx minim  = n_posix_min_n_type_gfx( sx, sy );
		const n_type_gfx round  = ( minim / n_game_progressbar_round_div ) + ( minim % 2 );
		const n_type_gfx border = n_game_progressbar_border;


		// [!] : color scheme

		const u32 color_fr = n_game_progressbar_border_color;
		const u32 color_bg = bg;
		const u32 color_fg = fg;


		// [!] : frame

		if ( n_game_progressbar_no_round )
		{

			n_bmp_box( bmp, x,y,sx,sy, color_fr );

			 x += border;
			 y += border;
			sx -= border * 2;
			sy -= border * 2;

			n_bmp_box( bmp, x,y,sx,sy, color_bg );

		} else {

			n_bmp_roundrect( bmp, x,y,sx,sy, color_fr, round );

			 x += border;
			 y += border;
			sx -= border * 2;
			sy -= border * 2;

			n_bmp_roundrect( bmp, x,y,sx,sy, color_bg, round );

		}


		// [!] : progress

		n_type_gfx fsx,fsy;
		if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
		{
			fsx = (n_type_gfx) ( (n_type_real) sx * pc );
			fsy = sy;
		} else {
			fsx = sx;
			fsy = (n_type_gfx) ( (n_type_real) sy * pc );
		}

		n_bmp_roundrect( bmp, x,y,fsx,fsy, color_fg, round );

	} else {

		// Debug Center

		//n_game_progressbar_animation = N_GAME_PROGRESSBAR_ANIMATION_ON_DOWN;
		//n_game_hwndprintf_literal( "%d", n_game_progressbar_animation );
		//percent = 0;


		// [!] : metrics

		if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
		{
			sx = n_posix_max_n_type_gfx( sx, sy );
		} else {
			sy = n_posix_max_n_type_gfx( sx, sy );
		}


		if ( n_game_progressbar_round_div < 2 ) { n_game_progressbar_round_div = 2; }


		const n_type_gfx bar_sx = (n_type_gfx) ( pc * sx );
		const n_type_gfx bar_sy = (n_type_gfx) ( pc * sy );


		const n_type_gfx    minim  = n_posix_min_n_type_gfx( sx, sy );
		//const n_type_gfx    maxim  = n_posix_max_n_type_gfx( sx, sy );
		const n_type_real   sugar  = (n_type_real) 1.0 / minim;
		const n_type_gfx    half   = minim / 2;
		const n_type_gfx    round  = ( minim / n_game_progressbar_round_div ) + ( minim % 2 );
		//const n_type_gfx    border = n_game_progressbar_border;


		static n_type_gfx animation = 0;


		n_type_real pipe = 0.0;


		n_type_gfx tx = 0;
		n_type_gfx ty = 0;
		n_posix_loop
		{//break;

			u32 color = bg;

			if ( n_game_progressbar_no_round )
			{

				if ( percent != 0 )
				{

					n_posix_bool onoff = n_posix_false;

					n_type_real d = 1.0;

					if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
					{
						onoff = ( tx < bar_sx );
					} else {
						onoff = ( ty < bar_sy );
					}

					if ( onoff ) { color = n_bmp_blend_pixel( bg, fg, d ); }

				}

			} else {

				if ( percent != 0 )
				{

					n_posix_bool onoff = n_posix_false;

					n_type_real d = 1.0;

					if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )
					{
						onoff = n_bmp_roundrect_detect_coeff( tx + round, ty, bar_sx + round, sy, round, &d );
					} else {
						onoff = n_bmp_roundrect_detect_coeff( tx, ty + round, sx, bar_sy + round, round, &d );
					}

					if ( onoff ) { color = n_bmp_blend_pixel( bg, fg, d ); }

				}

			}


			// [!] : stripe main

			if ( n_game_progressbar_stripe_detect( tx,ty, animation, stripe ) )
			{
				color = n_bmp_blend_pixel( color, n_bmp_white, 0.25 );
			}


			// [!] : pipe-like effect

			if (
				( ( mode == N_GAME_PROGRESSBAR_HORIZONTAL )&&( ty <= half ) )
				||
				( ( mode == N_GAME_PROGRESSBAR_VERTICAL   )&&( tx <= half ) )
			)
			{
				color = n_bmp_blend_pixel( color, n_bmp_white, 0.5 - pipe );
			} else {
				color = n_bmp_blend_pixel( color, n_bmp_white, pipe - 0.5 );
			}


			if ( n_game_progressbar_no_round )
			{
				n_bmp_ptr_set( bmp, tx + x, ty + y, color );
			} else {
				n_type_real coeff;
				u32         clr;

				n_bmp_roundrect_detect_coeff( tx,ty, sx,sy, round, &coeff );

				n_bmp_ptr_get( bmp, tx + x, ty + y, &clr   );

				color = n_bmp_blend_pixel( clr, color, coeff );

				n_bmp_ptr_set( bmp, tx + x, ty + y,  color );
			}


			if ( mode == N_GAME_PROGRESSBAR_VERTICAL ) { pipe += sugar; }

			tx++;
			if ( tx >= sx )
			{

				tx = 0;

				ty++;
				if ( ty >= sy ) { break; }

				if ( mode == N_GAME_PROGRESSBAR_HORIZONTAL ) { pipe += sugar; } else { pipe = 0; }

			}
		}

		static u32 timer = 0;

		if (
			( n_game_progressbar_animation_interval != 0 )
			&&
			( n_game_timer( &timer, n_game_progressbar_animation_interval ) )
		)
		{
			animation++;
			if ( animation >= stripe ) { animation = 0; }
		}

	}


	return;
}


#endif // _H_NONNON_WIN32_GAME_PROGRESSBAR

