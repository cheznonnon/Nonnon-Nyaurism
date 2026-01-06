// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : don't use "_transparent.c" : the first layer only




#ifndef _H_NONNON_NEUTRAL_BMP_DRAW
#define _H_NONNON_NEUTRAL_BMP_DRAW




#include <math.h>




#include "../bmp.c"


#include "./_error.c"
#include "./_fastcopy.c"
#include "./color.c"
#include "./detect.c"




n_type_index
n_bmp_fill( n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 color )
{

	// [!] : return value : processed pixel count


	if ( n_bmp_error( bmp ) ) { return 0; }


	// [!] : fail-safe

	u32 color_to;

	n_bmp_ptr_get( bmp, x,y, &color_to );
	if ( color == color_to ) { return 0; }


	// [!] : the same performance between these two logics

	//int *map = n_memory_new_closed( N_BMP_SX( bmp ) * N_BMP_SY( bmp ) * sizeof( int ) );
	u8 *map = (u8*) n_memory_new_closed( N_BMP_SX( bmp ) * N_BMP_SY( bmp ) * sizeof( u8 ) );


	u32 t    = 0;
	int move = 0;
	int stop = 0;

	n_type_index count = 0;
	n_type_index i     = 0;
	n_posix_loop
	{

		if (
			( n_posix_false == n_bmp_ptr_get( bmp, x,y, &t ) )
			&&
			( t == color_to )
		)
		{

			stop = 0;

			n_bmp_ptr_set_fast( bmp, x,y, color ); count++;

			map[ i ] = move;

			i++;

		} else {

			if ( move == 0 ) { y++; } else
			if ( move == 1 ) { x--; } else
			if ( move == 2 ) { y--; } else
			if ( move == 3 ) { x++; }

			stop++;
			if ( stop >= 4 )
			{

				stop = 0;

				if ( i <= 1 ) { break; }


				i--;

				move = map[ i ];
				if ( move == 0 ) { y++; } else
				if ( move == 1 ) { x--; } else
				if ( move == 2 ) { y--; } else
				if ( move == 3 ) { x++; }

			}

			move++;
			if ( move >= 4 ) { move = 0; }

		}

		if ( move == 0 ) { y--; } else
		if ( move == 1 ) { x++; } else
		if ( move == 2 ) { y++; } else
		if ( move == 3 ) { x--; }

	}


	n_memory_free_closed( map );


	return count;
}

void
n_bmp_line( n_bmp *bmp, n_type_gfx fx, n_type_gfx fy, n_type_gfx tx, n_type_gfx ty, u32 color )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_real step = 0;
	n_type_real unit = (n_type_real) n_posix_abs_n_type_gfx( fy - ty ) / n_posix_max_n_type_gfx( 1, n_posix_abs_n_type_gfx( fx - tx ) );


	n_posix_loop
	{

		// [!] : need to draw the first pos

		n_bmp_ptr_set( bmp, fx,fy, color );


		// [!] : need to draw tx,ty once or more

		if ( ( fx == tx )&&( fy == ty ) ) { break; }


		// [!] : don't use "else" : avoid zig-zag

		if ( step < 1 )
		{
			step += unit;
			if ( fx > tx ) { fx--; } else if ( fx < tx ) { fx++; }
		}

		if ( step >= 1 )
		{
			step -= 1;
			if ( fy > ty ) { fy--; } else if ( fy < ty ) { fy++; }
		}

	}


	return;
}

void
n_bmp_line_dot( n_bmp *bmp, n_type_gfx fx, n_type_gfx fy, n_type_gfx tx, n_type_gfx ty, u32 color, int moire )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_real step = 0;
	n_type_real unit = (n_type_real) n_posix_abs_n_type_gfx( fy - ty ) / n_posix_max_n_type_gfx( 1, n_posix_abs_n_type_gfx( fx - tx ) );


	n_posix_loop
	{

		// [!] : need to draw the first pos

		if ( n_bmp_moire_detect( fx, fy, moire ) )
		{
			n_bmp_ptr_set( bmp, fx,fy, color );
		}


		// [!] : need to draw tx,ty once or more

		if ( ( fx == tx )&&( fy == ty ) ) { break; }


		// [!] : don't use "else" : avoid zig-zag

		if ( step < 1 )
		{
			step += unit;
			if ( fx > tx ) { fx--; } else if ( fx < tx ) { fx++; }
		}

		if ( step >= 1 )
		{
			step -= 1;
			if ( fy > ty ) { fy--; } else if ( fy < ty ) { fy++; }
		}

	}


	return;
}

void
n_bmp_frame( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int size, u32 color )
{

	if ( n_bmp_error( bmp ) ) { return; }


	s32 fx = x;
	s32 fy = y;
	s32 tx = fx + sx - 1;
	s32 ty = fy + sy - 1;

	int i = 0;
	n_posix_loop
	{

		n_bmp_line( bmp, fx,fy,tx,fy, color );
		n_bmp_line( bmp, fx,fy,fx,ty, color );
		n_bmp_line( bmp, tx,fy,tx,ty, color );
		n_bmp_line( bmp, fx,ty,tx,ty, color );

		fx++;
		fy++;
		tx--;
		ty--;

		i++;
		if ( i >= size ) { break; }
	}


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	n_type_gfx  bmpsx,bmpsy;
	u32         color;
	u32         oy, cores;

} n_bmp_box_thread_struct;

void
n_bmp_box_thread_main( n_bmp_box_thread_struct *p )
{

	n_type_gfx jump = p->bmpsx * p->cores;

	n_type_index line;
	if ( n_bmp_flip_onoff )
	{
		line = n_bmp_ptr_pos2index( p->bmp, 0, p->y - p->oy );
	} else {
		line = p->bmpsx * ( p->bmpsy - 1 - p->y - p->oy );
	}


	n_type_index ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		n_type_index pos = p->x + line;

		n_type_index tx = 0;
		n_posix_loop
		{

			N_BMP_PTR( p->bmp )[ pos + tx ] = p->color;

			tx++;
			if ( tx >= p->sx ) { break; }
		}

		if ( n_bmp_flip_onoff ) { line += jump; } else { line -= jump; }

		ty += p->cores;
		if ( ty >= p->sy ) { return; }
	}


	return;
}

n_thread_return
n_bmp_box_thread( n_thread_argument p )
{

	n_bmp_box_thread_main( (n_bmp_box_thread_struct*) p );

	return 0;
}

void
n_bmp_box( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [x] : Win9x : can run but not working

	if (
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_box() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                *h = (n_thread               *) n_memory_new( cores * sizeof( n_thread                ) );
		n_bmp_box_thread_struct *p = (n_bmp_box_thread_struct*) n_memory_new( cores * sizeof( n_bmp_box_thread_struct ) );


		n_type_gfx bmpsx = N_BMP_SX( bmp );
		n_type_gfx bmpsy = N_BMP_SY( bmp );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_box_thread_struct tmp = { bmp, x,y,sx,sy, bmpsx,bmpsy, color, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_box_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_box_thread, &p[ i ] );

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

		n_bmp_box_thread_struct p = { bmp, x,y,sx,sy, N_BMP_SX( bmp ),N_BMP_SY( bmp ), color, 0,1 };

		n_bmp_box_thread_main( &p );

	}


	return;
}

#define N_BMP_GRADIENT_CENTERING  ( 0 << 16 )
#define N_BMP_GRADIENT_HORIZONTAL ( 1 << 16 )
#define N_BMP_GRADIENT_VERTICAL   ( 2 << 16 )
#define N_BMP_GRADIENT_TOPLEFT    ( 3 << 16 )

#define n_bmp_flush_gradient( bmp, from,to, option ) \
	n_bmp_gradient( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), from,to, option )

typedef struct {

	n_bmp        *bmp;
	n_type_gfx    x,y,sx,sy;
	n_type_gfx    size;
	n_type_gfx    centerx, centery;
	int           type, mode;
	u32           from, to;
	n_posix_bool  fast_onoff;
	u32           oy, cores;

} n_bmp_gradient_thread_struct;

void
n_bmp_gradient_thread_main( n_bmp_gradient_thread_struct *p )
{

	// [!] : random-dot based dither

	const int randomdot_max  = 3;
	const int randomdot_half = randomdot_max / 2;


	if ( p->fast_onoff )
	{

		n_type_gfx tx = 0;
		n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
		n_posix_loop
		{

			n_type_gfx fx = 0;
			n_type_gfx fy = 0;

			if ( p->type != N_BMP_GRADIENT_VERTICAL   ) { fx = p->centerx - tx; }
			if ( p->type != N_BMP_GRADIENT_HORIZONTAL ) { fy = p->centery - ty; }


			u32 color = n_bmp_gradient_pixel_s32( p->from,p->to, fx,fy, p->size, p->mode );


			int d = (int) n_random_range( randomdot_max ) - randomdot_half;

			// [!] : alpha is not used

			//int a = n_bmp_a( color );
			//int r = n_bmp_clamp_channel( n_bmp_r( color ) + d );
			//int g = n_bmp_clamp_channel( n_bmp_g( color ) + d );
			//int b = n_bmp_clamp_channel( n_bmp_b( color ) + d );

			// [!] Copilot Optimization

			// Cache color components to avoid repeated bit shifts
			int ca = n_bmp_a( color );
			int cr = n_bmp_r( color );
			int cg = n_bmp_g( color );
			int cb = n_bmp_b( color );

			int a = ca;
			int r = n_bmp_clamp_channel( cr + d );
			int g = n_bmp_clamp_channel( cg + d );
			int b = n_bmp_clamp_channel( cb + d );

			color = n_bmp_argb( a,r,g,b );


			n_bmp_ptr_set_fast( p->bmp, tx + p->x, ty + p->y, color );


			tx++;
			if ( tx >= p->sx )
			{

				tx = 0;

				ty += p->cores;
				if ( ty >= p->sy ) { break; }
			}
		}

		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		n_type_gfx fx = 0;
		n_type_gfx fy = 0;

		if ( p->type != N_BMP_GRADIENT_VERTICAL   ) { fx = p->centerx - tx; }
		if ( p->type != N_BMP_GRADIENT_HORIZONTAL ) { fy = p->centery - ty; }


		u32 color = n_bmp_gradient_pixel_real( p->from,p->to, fx,fy, p->size, p->mode );


		int d = (int) n_random_range( randomdot_max ) - randomdot_half;

		// [!] : alpha is not used

		int a = n_bmp_a( color );
		int r = n_bmp_clamp_channel( n_bmp_r( color ) + d );
		int g = n_bmp_clamp_channel( n_bmp_g( color ) + d );
		int b = n_bmp_clamp_channel( n_bmp_b( color ) + d );

		color = n_bmp_argb( a,r,g,b );


		n_bmp_ptr_set_fast( p->bmp, tx + p->x, ty + p->y, color );


		tx++;
		if ( tx >= p->sx )
		{

			tx = 0;

			ty += p->cores;
			if ( ty >= p->sy ) { break; }
		}
	}


	return;
}

n_thread_return
n_bmp_gradient_thread( n_thread_argument p )
{

	n_bmp_gradient_thread_main( (n_bmp_gradient_thread_struct*) p );

	return 0;
}

void
n_bmp_gradient
(
	n_bmp *bmp,
	n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy,
	u32 from, u32 to,
	int option
)
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_bmp_gradient_thread_struct p_0;


	p_0.bmp = bmp;

	p_0.x  =  x;
	p_0.y  =  y;
	p_0.sx = sx;
	p_0.sy = sy;

	p_0.type = option & 0xffff0000;
	p_0.mode = option & 0x0000ffff;

	p_0.centerx = 0;
	p_0.centery = 0;

	if ( p_0.sx > p_0.sy ) { p_0.size = p_0.sx; } else { p_0.size = p_0.sy; }

	if ( p_0.type == N_BMP_GRADIENT_CENTERING  )
	{
		p_0.size    = p_0.size / 2;
		p_0.centerx = p_0.sx   / 2;
		p_0.centery = p_0.sy   / 2;
	} else
	if ( p_0.type == N_BMP_GRADIENT_HORIZONTAL )
	{
		p_0.size = p_0.sx;
	} else
	if ( p_0.type == N_BMP_GRADIENT_VERTICAL   )
	{
		p_0.size = p_0.sy;
	}

	p_0.from = from;
	p_0.to   =   to;

	p_0.fast_onoff = ( ( p_0.sx < 3000 )&&( p_0.sy < 3000 ) );

	p_0.oy    = 0;
	p_0.cores = 1;


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( p_0.size * p_0.size ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_gradient() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                     *h = (n_thread                    *) n_memory_new(cores * sizeof(n_thread));
		n_bmp_gradient_thread_struct *p = (n_bmp_gradient_thread_struct*) n_memory_new( cores * sizeof( n_bmp_gradient_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_memory_copy( &p_0, &p[ i ], sizeof( n_bmp_gradient_thread_struct ) );

			p[ i ].oy    = i;
			p[ i ].cores = cores;

			h[ i ] = n_thread_init( n_bmp_gradient_thread, &p[ i ] );

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

		n_bmp_gradient_thread_main( &p_0 );

	}


	return;
}

#define n_bmp_flush_roundrect( bmp, color, p ) \
	n_bmp_roundrect( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color, p )

#define n_bmp_roundrect_pixel( b, x,y,sx,sy, c, p ) n_bmp_roundrect( b, x,y,sx,sy, c,  (p) )
#define n_bmp_roundrect_ratio( b, x,y,sx,sy, c, p ) n_bmp_roundrect( b, x,y,sx,sy, c, -(p) )

#define n_bmp_flush_roundrect_pixel( b, c, p ) n_bmp_flush_roundrect( b, c,  (p) )
#define n_bmp_flush_roundrect_ratio( b, c, p ) n_bmp_flush_roundrect( b, c, -(p) )

#define n_bmp_flush_roundrect_coeff( bmp, color, p, coeff ) \
	n_bmp_roundrect_main( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color, p, coeff, 1.0 )

#define n_bmp_roundrect_pixel_coeff( b, x,y,sx,sy, c, p, coeff ) n_bmp_roundrect_coeff( b, x,y,sx,sy, c,  (p), coeff )
#define n_bmp_roundrect_ratio_coeff( b, x,y,sx,sy, c, p, coeff ) n_bmp_roundrect_coeff( b, x,y,sx,sy, c, -(p), coeff )

#define n_bmp_flush_roundrect_pixel_coeff( b, c, p, coeff ) n_bmp_flush_roundrect_coeff( b, c,  (p), coeff )
#define n_bmp_flush_roundrect_ratio_coeff( b, c, p, coeff ) n_bmp_flush_roundrect_coeff( b, c, -(p), coeff )

#define n_bmp_roundrect_blend( bmp, x,y,sx,sy, color, p,      blend ) \
	n_bmp_roundrect_main(  bmp, x,y,sx,sy, color, p, 0.0, blend )

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	n_type_gfx  r;
	u32         color;
	n_type_real coeff;
	n_type_real blend;
	u32         oy, cores;

} n_bmp_roundrect_thread_struct;

static n_bmp *n_bmp_roundrect_bmp_map = NULL;

void
n_bmp_roundrect_thread_main( n_bmp_roundrect_thread_struct *p )
{

	n_type_gfx fx = 0;
	n_type_gfx fy = p->oy; if ( fy >= p->sy ) { return; }

	n_type_index oy = n_bmp_ptr_pos2index( p->bmp, p->x + fx, p->y + fy );

	n_posix_loop
	{//break;

		if ( n_bmp_ptr_is_accessible( p->bmp, p->x + fx, p->y + fy ) )
		{

			n_posix_bool onoff = n_posix_true;
			if ( n_bmp_roundrect_bmp_map != NULL )
			{
				u32 map;
				n_bmp_ptr_get_fast( n_bmp_roundrect_bmp_map, p->x + fx, p->y + fy, &map );
				if ( map == n_bmp_white ) { onoff = n_posix_false; }
			}

			n_type_real coeff = 0;
			if ( onoff )
			{
				n_bmp_roundrect_detect_coeff( fx,fy, p->sx,p->sy, p->r, &coeff );
//NSLog( @"NaN Checker : %f %f %f", p->coeff, coeff, p->blend );

				u32 c;

				//n_bmp_ptr_get_fast( p->bmp, p->x + fx, p->y + fy, &c );
				c = N_BMP_PTR( p->bmp )[ fx + oy ];

				c = n_bmp_blend_pixel( c, p->color, ( p->coeff + coeff ) * p->blend );

//int v = (n_type_real) coeff * 100;
//c = n_bmp_rgb( v,v,v );

				//n_bmp_ptr_set_fast( p->bmp, p->x + fx, p->y + fy,  c );
				N_BMP_PTR( p->bmp )[ fx + oy ] = c;
			}

		}


		fx++;
		if ( fx >= p->sx )
		{

			fx = 0;

			fy += p->cores;
			if ( fy >= p->sy ) { break; }

			oy = n_bmp_ptr_pos2index( p->bmp, p->x + fx, p->y + fy );
		}
	}


	return;
}

n_thread_return
n_bmp_roundrect_thread( n_thread_argument p )
{

	n_bmp_roundrect_thread_main( (n_bmp_roundrect_thread_struct*) p );

	return 0;
}

#define n_bmp_roundrect(       b, x,y,sx,sy, color, param    ) n_bmp_roundrect_main( b, x,y,sx,sy, color, param, 0, 1.0 )
#define n_bmp_roundrect_coeff( b, x,y,sx,sy, color, param, c ) n_bmp_roundrect_main( b, x,y,sx,sy, color, param, c, 1.0 )

n_type_gfx
n_bmp_roundrect_param( n_type_gfx sx, n_type_gfx sy, n_type_gfx param )
{

	// [Mechanism]
	//
	//	a shorter side will be used for radius
	//
	//	param >= 0 : pixels
	//	param <  0 : percentage


	n_type_gfx r = n_posix_min_n_type_gfx( sx, sy ) / 2;
	n_type_gfx p = n_posix_abs_n_type_gfx( param );

	if ( param >= 0 )
	{

		if ( r > p ) { r = p; }

	} else {

		n_type_real percent = (n_type_real) p * 0.01;
		percent = n_posix_min_n_type_real( 1.0, percent );

		if ( percent != 0.0 ) { r = (n_type_gfx) ( (n_type_real) r * percent ); }

	}


	return r;
}

void
n_bmp_roundrect_main
(
	n_bmp       *bmp,
	n_type_gfx    x,
	n_type_gfx    y,
	n_type_gfx   sx,
	n_type_gfx   sy,
	u32          color,
	n_type_gfx   param,
	n_type_real  coeff,
	n_type_real  blend
)
{

	// [!] : don't use n_bmp_error_clipping() : drawing error happens

	//if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : nothing to do

	if ( ( sx < 0 )||( sy < 0 ) ) { return; }


	n_type_gfx r = n_bmp_roundrect_param( sx, sy, param );


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_roundrect() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                      *h = (n_thread                     *) n_memory_new( cores * sizeof( n_thread                      ) );
		n_bmp_roundrect_thread_struct *p = (n_bmp_roundrect_thread_struct*) n_memory_new( cores * sizeof( n_bmp_roundrect_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_roundrect_thread_struct tmp = { bmp, x,y,sx,sy, r, color, coeff, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_roundrect_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_roundrect_thread, &p[ i ] );

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

		n_bmp_roundrect_thread_struct p = { bmp, x,y,sx,sy, r, color, coeff, blend, 0,1 };

		n_bmp_roundrect_thread_main( &p );

	}


	return;
}

#define n_bmp_flush_circle( bmp, color ) \
	n_bmp_circle( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color )

void
n_bmp_circle( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color )
{

	// [!] : don't use n_bmp_error_clipping() : circle will be ellipse at canvas border

	//if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }
	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : ( size - 1 ) : don't use max size

	n_type_gfx rx = ( sx - 1 ) / 2;
	n_type_gfx ry = ( sy - 1 ) / 2;


	n_type_gfx fx = 0;
	n_type_gfx fy = 0;
	n_posix_loop
	{

		if ( n_bmp_ptr_is_accessible( bmp, x + fx, y + fy ) )
		{

			n_type_gfx polar_x = fx - rx;
			n_type_gfx polar_y = fy - ry;

			n_type_real coeff;
			if ( n_bmp_ellipse_detect_coeff( polar_x,polar_y, rx,ry, &coeff ) )
			{

				u32 c = color;

				if ( coeff < 1.0 )
				{
					n_bmp_ptr_get_fast( bmp, x + fx, y + fy, &c );
					c = n_bmp_blend_pixel( c, color, coeff );
				}

				if ( coeff > 0.0 )
				{
					n_bmp_ptr_set_fast( bmp, x + fx, y + fy,  c );
				}

			}

		}

		fx++;
		if ( fx >= sx )
		{

			fx = 0;

			fy++;
			if ( fy >= sy ) { break; }
		}
	}


	return;
}

#define n_bmp_flush_hoop( bmp, border, color ) \
	n_bmp_hoop( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), border, color )

void
n_bmp_hoop( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_gfx border, u32 color )
{

	// [!] : don't use n_bmp_error_clipping() : circle will be ellipse at canvas border

	//if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }
	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : ( size - 1 ) : don't use max size

	n_type_gfx rx_o = ( sx - 1 ) / 2;
	n_type_gfx ry_o = ( sy - 1 ) / 2;
	n_type_gfx rx_i = rx_o - border;
	n_type_gfx ry_i = ry_o - border;


	n_type_gfx fx = 0;
	n_type_gfx fy = 0;
	n_posix_loop
	{

		if ( n_bmp_ptr_is_accessible( bmp, x + fx, y + fy ) )
		{

			n_type_gfx polar_x = fx - rx_o;
			n_type_gfx polar_y = fy - ry_o;

			n_type_real coeff_o;
			if ( n_bmp_ellipse_detect_coeff( polar_x,polar_y, rx_o,ry_o, &coeff_o ) )
			{

				n_type_real coeff_i;
				if ( n_bmp_ellipse_detect_coeff( polar_x,polar_y, rx_i,ry_i, &coeff_i ) )
				{
					coeff_o = 1.0 - coeff_i;
				}


				u32 c = color;

				if ( coeff_o < 1.0 )
				{
					n_bmp_ptr_get_fast( bmp, x + fx, y + fy, &c );
					c = n_bmp_blend_pixel( c, color, coeff_o );
				}

				if ( coeff_o > 0.0 )
				{
					n_bmp_ptr_set_fast( bmp, x + fx, y + fy,  c );
				}

			}

		}

		fx++;
		if ( fx >= sx )
		{

			fx = 0;

			fy++;
			if ( fy >= sy ) { break; }
		}
	}


	return;
}

#define n_bmp_flush_squircle( bmp, color, power ) \
	n_bmp_squircle( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color, power )

void
n_bmp_squircle( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color, n_type_real power )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx polar = n_posix_min_n_type_gfx( sx,sy ) / 2;


	n_type_real coeff = 0;


	n_type_gfx xx = 0;
	n_type_gfx yy = 0;
	n_posix_loop
	{//break;

		n_type_gfx qx = xx;
		n_type_gfx qy = yy;

		// [!] : Corner

		if ( xx >= ( sx - polar ) ) { qx = sx - xx; } else { qx++; }
		if ( yy >= ( sy - polar ) ) { qy = sy - yy; } else { qy++; }

		// [!] : Not Corner

		if ( ( xx > polar )&&( xx < ( sx - polar ) ) ) { qx = polar; }
		if ( ( yy > polar )&&( yy < ( sy - polar ) ) ) { qy = polar; }

		if ( n_bmp_squircle_detect( qx, qy, polar, power, &coeff ) )
		{
			u32 c;
			n_bmp_ptr_get( bmp, x + xx, y + yy, &c );

			c = n_bmp_blend_pixel( c, color, coeff );

			n_bmp_ptr_set( bmp, x + xx, y + yy,  c );
		}

		xx++;
		if ( xx >= sx )
		{
			xx = 0;

			yy++;
			if ( yy >= sy ) { break; }
		}
	}


	return;
}


#endif //_H_NONNON_NEUTRAL_BMP_DRAW

