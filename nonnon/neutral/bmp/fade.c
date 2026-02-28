// Nonnon User Interface
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_FADE
#define _H_NONNON_NEUTRAL_BMP_FADE




#include "./color.c"




#define N_BMP_FADE_MSEC ( 200 )




typedef struct {

	u32  color_fg;
	u32  color_bg;
	u32  tick;
	BOOL stop;
	int  percent;

} n_bmp_fade;




#define n_bmp_fade_zero( p ) n_memory_zero( p, sizeof( n_bmp_fade ) )

#define n_bmp_fade_redraw( p ) ( ( p )->stop = FALSE )

void
n_bmp_fade_init( n_bmp_fade *p, u32 color )
{

	p->color_bg = color;
	p->color_fg = color;
	p->tick     = 0;
	p->stop     = TRUE;
	p->percent  = 100;


	return;
}

void
n_bmp_fade_go( n_bmp_fade *p, u32 color )
{

	if ( p->stop     == FALSE ) { return; }
	if ( p->color_fg == color ) { return; }


	p->color_bg = p->color_fg;
	p->color_fg = color;
	p->tick     = n_posix_tickcount();
	p->stop     = FALSE;
	p->percent  = 0;


	return;
}

void
n_bmp_fade_always_on( n_bmp_fade *p, u32 bg, u32 fg )
{

	p->color_bg = bg;
	p->color_fg = fg;
	p->tick     = n_posix_tickcount();
	p->stop     = FALSE;
	p->percent  = 0;


	return;
}

u32
n_bmp_fade_engine( n_bmp_fade *p, BOOL onoff )
{

	if ( p->stop ) { n_bmp_fade_init( p, p->color_fg ); return p->color_fg; }


	if ( onoff == FALSE )
	{
		p->stop    = TRUE;
		p->percent = 100;

		return p->color_fg;
	}


	n_type_real ratio = 1.0;

	{

		n_type_real delta = 0;

		if ( p->tick != 0 )
		{
			delta = n_posix_tickcount() - p->tick;
			ratio = delta / N_BMP_FADE_MSEC;
		}

	}


	u32 color_ret = n_bmp_blend_pixel( p->color_bg, p->color_fg, ratio );
	p->percent = n_posix_minmax( 0, 100, (int) ( ratio * 100 ) );

	if ( p->percent >= 100 ) { n_bmp_fade_init( p, p->color_fg ); }


	return color_ret;
}


#endif // _H_NONNON_NEUTRAL_BMP_FADE

