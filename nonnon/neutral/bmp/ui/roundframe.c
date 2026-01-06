// Nonnon Win
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : currently experimental : include manually




#ifndef _H_NONNON_NEUTRAL_BMP_UI_ROUNDFRAME
#define _H_NONNON_NEUTRAL_BMP_UI_ROUNDFRAME




#include "../all.c"




typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	n_type_gfx  round;
	n_type_gfx  frame;
	u32         color_outer;
	u32         color_inner;
	u32         oy, cores;

} n_bmp_ui_roundframe_thread_struct;

void
n_bmp_ui_roundframe_thread_main( n_bmp_ui_roundframe_thread_struct *p )
{

	n_type_gfx fx = 0;
	n_type_gfx fy = (n_type_gfx) p->oy; if ( fy >= p->sy ) { return; }
	n_posix_loop
	{//break;

		if ( n_bmp_ptr_is_accessible( p->bmp, p->x + fx, p->y + fy ) )
		{

			n_type_gfx ox  = fx;
			n_type_gfx oy  = fy;
			n_type_gfx osx = p->sx;
			n_type_gfx osy = p->sy;

			n_type_gfx ix  = fx - p->frame;
			n_type_gfx iy  = fy - p->frame;
			n_type_gfx isx = p->sx - ( p->frame * 2 );
			n_type_gfx isy = p->sy - ( p->frame * 2 );

			n_type_real coeff_o = 0; 
			n_type_real coeff_i = 0;

			n_bmp_roundrect_detect_coeff( ox,oy, osx,osy, p->round, &coeff_o );
			n_bmp_roundrect_detect_coeff( ix,iy, isx,isy, p->round, &coeff_i );

			u32 c;

			n_bmp_ptr_get_fast( p->bmp, p->x + fx, p->y + fy, &c );

			if ( coeff_o >= 1.0 )
			{
				c = n_bmp_blend_pixel( c, p->color_outer, coeff_o );
				c = n_bmp_blend_pixel( c, p->color_inner, coeff_i );

				int alpha = n_bmp_a( c );
				if ( ( 0 != alpha )&&( 255 != alpha ) )
				{
					c = n_bmp_blend_pixel( c, p->color_inner, n_bmp_blend_alpha2ratio( alpha ) );
					c = n_bmp_alpha_replace_pixel( c, 255 );
				}
			} else {
				c = n_bmp_blend_pixel( c, p->color_outer, coeff_o );
			}

			n_bmp_ptr_set_fast( p->bmp, p->x + fx, p->y + fy,  c );

		}


		fx++;
		if ( fx >= p->sx )
		{

			fx = 0;

			fy += p->cores;
			if ( fy >= p->sy ) { break; }
		}
	}


	return;
}

n_thread_return
n_bmp_ui_roundframe_thread( n_thread_argument p )
{

	n_bmp_ui_roundframe_thread_main( (n_bmp_ui_roundframe_thread_struct*) p );

	return 0;
}

void
n_bmp_ui_roundframe
(
	n_bmp      *bmp,
	n_type_gfx   x,
	n_type_gfx   y,
	n_type_gfx  sx,
	n_type_gfx  sy,
	n_type_gfx  round,
	n_type_gfx  frame,
	u32         color_outer,
	u32         color_inner
)
{
//return;

	if ( n_bmp_error( bmp ) ) { return; }


	// [x] : Win9x : can run but not working

	n_type_gfx count = N_BMP_SX( bmp ) * N_BMP_SY( bmp ) ;

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( count >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_ui_roundframe() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                          *h = (n_thread                         *) n_memory_new( cores * sizeof( n_thread                          ) );
		n_bmp_ui_roundframe_thread_struct *p = (n_bmp_ui_roundframe_thread_struct*) n_memory_new( cores * sizeof( n_bmp_ui_roundframe_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_ui_roundframe_thread_struct tmp = { bmp, x,y,sx,sy, round, frame, color_outer, color_inner, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_ui_roundframe_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_ui_roundframe_thread, &p[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_wait( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			n_thread_exit( h[ i ] );

			i++;
			if ( i >= cores ) { break; }
		}


		n_memory_free( h );
		n_memory_free( p );


		n_bmp_is_multithread = p_multithread;

	} else {

		n_bmp_ui_roundframe_thread_struct tmp = { bmp, x,y,sx,sy, round, frame, color_outer, color_inner, 0,1 };
		n_bmp_ui_roundframe_thread_main( &tmp );

	}


	return;
}

void
n_bmp_ui_roundframe_classic
(
	n_bmp      *bmp,
	n_type_gfx   x,
	n_type_gfx   y,
	n_type_gfx  sx,
	n_type_gfx  sy,
	n_type_gfx  round,
	n_type_gfx  frame,
	u32         color_outer,
	u32         color_inner
)
{
//return;


	n_type_gfx m, mm;

	m  = 0;
	mm = m * 2;
	n_bmp_roundrect( bmp, x+m,y+m,sx-mm,sy-mm, color_outer, round );

	m  = frame;
	mm = m * 2;
	//n_bmp_roundrect( bmp, x+m,y+m,sx-mm,sy-mm, color_inner, round );

	n_type_real coeff = (n_type_real) 0.005 * round;
	n_bmp_roundrect_main( bmp, x+m,y+m,sx-mm,sy-mm, color_inner, round, coeff, 1.0 );


	return;
}

void
n_bmp_ui_roundframe_shadow
(
	n_bmp        *bmp,
	n_type_gfx    sx,
	n_type_gfx    sy,
	n_type_gfx    shadow_size,
	n_type_gfx    round_param,
	n_type_gfx    scale,
	n_posix_bool  darkmode_onoff,
	n_posix_bool  drop_onoff
)
{

	n_type_gfx x = 0;
	n_type_gfx y = 0;

	if ( sx <= 0 ) { sx = N_BMP_SX( bmp ); }
	if ( sy <= 0 ) { sy = N_BMP_SY( bmp ); }

//n_bmp_roundrect_pixel( bmp, x,y,sx,sy, n_bmp_black, round_param );
//return;


	n_bmp_roundrect_detect_coeff_patch_onoff = n_posix_false;


	n_type_gfx count = shadow_size / scale;

	round_param *= 2;


	n_type_gfx ox = 0;
	n_type_gfx oy = 0;
	n_type_gfx os = 0;
	if ( drop_onoff )
	{
		ox = oy = shadow_size / 2;
	} else {
		oy = os = shadow_size / 3;

		oy *= -1;
	}


	u32         color = n_bmp_rgb( 1,1,1 );
	n_type_real step  = 1.0 / (n_type_real) count;

	n_type_real coeff;
	if ( darkmode_onoff )
	{
		coeff = 0.050;
	} else {
		coeff = 0.025;
	}

	n_type_gfx i = 0;
	n_posix_loop
	{

		n_type_real d = (n_type_real) i * step * coeff;
		n_bmp_roundrect_blend( bmp, ox+x,oy+y,sx,sy+os, color, round_param, d );

		 x += scale;
		 y += scale;

		sx -= scale * 2;
		sy -= scale * 2;

		i++;
		if ( i >= count ) { break; }
	}


	n_bmp_roundrect_detect_coeff_patch_onoff = n_posix_true;


	return;
}




#endif // _H_NONNON_NEUTRAL_BMP_UI_ROUNDFRAME

