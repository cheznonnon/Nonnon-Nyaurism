// Nonnon User Interface
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_FADE
#define _H_NONNON_NEUTRAL_BMP_FADE




#include "./color.c"




#define N_BMP_FADE_MSEC ( 200 )




typedef struct {

	u32  msec;
	u32  color_fr;
	u32  color_to;
	u32  color_md;
	u32  tick;
	BOOL stop;
	int  percent;

} n_bmp_fade;




#define n_bmp_fade_zero( p ) n_memory_zero( p, sizeof( n_bmp_fade ) )

#define n_bmp_fade_redraw( p ) ( ( p )->stop = FALSE )

void
n_bmp_fade_init( n_bmp_fade *p, u32 color )
{

	p->msec     = N_BMP_FADE_MSEC;
	p->color_fr = color;
	p->color_to = color;
	p->color_md = color;
	p->tick     = 0;
	p->stop     = TRUE;
	p->percent  = 100;


	return;
}

void
n_bmp_fade_go( n_bmp_fade *p, u32 color )
{

	if ( p->stop     == FALSE ) { return; }
	if ( p->color_to == color ) { return; }


	p->color_fr = p->color_md;
	p->color_to = color;
	p->tick     = n_posix_tickcount();
	p->stop     = FALSE;
	p->percent  = 0;


	return;
}

void
n_bmp_fade_always_on( n_bmp_fade *p, u32 from, u32 to )
{

	p->color_fr = from;
	p->color_to = to;
	p->color_md = from;
	p->tick     = n_posix_tickcount();
	p->stop     = FALSE;
	p->percent  = 0;


	return;
}

u32
n_bmp_fade_engine( n_bmp_fade *p, BOOL onoff )
{

	if ( p->stop ) { n_bmp_fade_init( p, p->color_to ); return p->color_to; }


	if ( onoff == FALSE )
	{
		p->stop    = TRUE;
		p->percent = 100;

		return p->color_to;
	}


	n_type_real ratio = 1.0;

	{

		n_type_real delta = 0;

		if ( p->tick != 0 )
		{
			delta = n_posix_tickcount() - p->tick;
			ratio = delta / p->msec;
		}

	}


	p->color_md = n_bmp_blend_pixel( p->color_fr, p->color_to, ratio );
	p->percent  = n_posix_minmax( 0, 100, (int) ( ratio * 100 ) );

	if ( p->percent >= 100 ) { n_bmp_fade_init( p, p->color_to ); }


	return p->color_md;
}


#endif // _H_NONNON_NEUTRAL_BMP_FADE

