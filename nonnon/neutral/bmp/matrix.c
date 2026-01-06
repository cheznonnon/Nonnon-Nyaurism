// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism]
//
//	basic matrix : { 1, 0, 0, 1 }
//
//	x == ( x * 1 ) + ( y * 0 )
//	y == ( x * 0 ) + ( y * 1 )




#ifndef _H_NONNON_NEUTRAL_BMP_MATRIX
#define _H_NONNON_NEUTRAL_BMP_MATRIX




#include "../bmp.c"


#include "./_error.c"
#include "./_fastcopy.c"
#include "./transform.c"




typedef struct {

	n_bmp        *bmp_f, *bmp_t;
	n_type_real  *matrix;
	n_type_real   ofx,ofy, otx,oty;
	n_type_real   tsx,tsy;
	n_type_gfx    bmpsx,bmpsy;
	n_posix_bool  minimal_size;
	u32           color_bg;

	n_type_real   min_x,min_y;
	n_type_real   max_x,max_y;

	u32           oy, cores;

} n_bmp_matrix_rotate_thread_struct;

void
n_bmp_matrix_rotate_thread_main( n_bmp_matrix_rotate_thread_struct *p )
{

	n_type_real tx = 0;
	n_type_real ty = p->oy; if ( ty >= p->tsy ) { return; }
	n_posix_loop
	{

		if ( ty >= p->tsy ) { break; }


		n_type_real fx = p->ofx + ( ( tx - p->otx ) * p->matrix[ 0 ] ) + ( ( ty - p->oty ) * p->matrix[ 2 ] );
		n_type_real fy = p->ofy + ( ( tx - p->otx ) * p->matrix[ 1 ] ) + ( ( ty - p->oty ) * p->matrix[ 3 ] );

		n_type_real ceil_fx = ceil( fx );
		n_type_real ceil_fy = ceil( fy );

		if (
			( ( 0 == ceil_fx )||( p->bmpsx == ceil_fx ) )
			||
			( ( 0 == ceil_fy )||( p->bmpsy == ceil_fy ) )
		)
		{

			// [!] : edge

			u32 color = p->color_bg;
			n_bmp_ptr_get( p->bmp_f, (n_type_gfx) fx, (n_type_gfx) fy, &color );

			if ( 0 == ceil_fx )
			{
				color = n_bmp_blend_pixel( p->color_bg, color, 1.0 - ( ceil_fx - fx ) );
//color = n_bmp_rgb( 255,0,0 );
			} else
			if ( 0 == ceil_fy )
			{
				color = n_bmp_blend_pixel( p->color_bg, color, 1.0 - ( ceil_fy - fy ) );
//color = n_bmp_rgb( 0,0,255 );
			} else
			if ( p->bmpsx == ceil_fx )
			{
				color = n_bmp_blend_pixel( p->color_bg, color, ceil_fx - fx );
//color = n_bmp_rgb( 0,255,0 );
			} else
			if ( p->bmpsy == ceil_fy )
			{
				color = n_bmp_blend_pixel( p->color_bg, color, ceil_fy - fy );
//color = n_bmp_rgb( 0,0,0 );
			}

			n_bmp_ptr_set( p->bmp_t, (n_type_gfx) tx, (n_type_gfx) ty, color );

			if ( n_bmp_ptr_is_accessible( p->bmp_f, (n_type_gfx) fx, (n_type_gfx) fy ) )
			{
				if ( p->minimal_size )
				{
					if ( p->min_x > tx ) { p->min_x = tx; }
					if ( p->min_y > ty ) { p->min_y = ty; }
					if ( p->max_x < tx ) { p->max_x = tx; }
					if ( p->max_y < ty ) { p->max_y = ty; }
				}
			}

		} else
		if ( n_bmp_ptr_is_accessible( p->bmp_f, (n_type_gfx) fx, (n_type_gfx) fy ) )
		{

			// [!] : use "enlarge" mode

			n_type_real trunc_fx = (n_type_gfx) fx;//trunc( fx );
			n_type_real trunc_fy = (n_type_gfx) fy;//trunc( fy );

			int mode  = N_BMP_BILINEAR_PIXEL_ALWAYS_ENLARGE;
			u32 color = n_bmp_bilinear_pixel( p->bmp_f, (n_type_gfx) fx, (n_type_gfx) fy, 2.0,2.0, fx - trunc_fx, fy - trunc_fy, mode );
			n_bmp_ptr_set( p->bmp_t, (n_type_gfx) tx, (n_type_gfx) ty, color );

			if ( p->minimal_size )
			{
				if ( p->min_x > tx ) { p->min_x = tx; }
				if ( p->min_y > ty ) { p->min_y = ty; }
				if ( p->max_x < tx ) { p->max_x = tx; }
				if ( p->max_y < ty ) { p->max_y = ty; }
			}

		}


		tx++;
		if ( tx >= p->tsx )
		{

			tx = 0;

			ty += p->cores;
			if ( ty >= p->tsy ) { break; }
		}
	}


	return;
}

n_thread_return
n_bmp_matrix_rotate_thread( n_thread_argument p )
{

	n_bmp_matrix_rotate_thread_main( (n_bmp_matrix_rotate_thread_struct*) p );

	return 0;
}

void
n_bmp_matrix_rotate( n_bmp *bmp, int degree, u32 color_bg, n_posix_bool minimal_size )
{

	if ( n_bmp_error( bmp ) ) { return; }


	degree = degree % 360;

	if ( degree < 0 ) { degree = 360 + degree; }


	// [!] : faster mode is used when degree is bigger
	//
	//	1 : faster mode for a    big rotation
	//	2 : slower mode for a little rotation if needed

	if ( degree >= 270 )
	{
		degree -= 270;
		n_bmp_rotate( bmp, N_BMP_ROTATE_LEFT );
	} else
	if ( degree >= 180 )
	{
		degree -= 180;
		n_bmp_flush_mirror( bmp, N_BMP_MIRROR_ROTATE180 );
	}
	if ( degree >= 90 )
	{
		degree -= 90;
		n_bmp_rotate( bmp, N_BMP_ROTATE_RIGHT );
	}
//n_posix_debug_literal( "%d", degree );


	// [!] : nothing to do

	if ( degree == 0 ) { return; }


	// Phase 1 : size calculation

	const n_type_gfx bmpsx = N_BMP_SX( bmp );
	const n_type_gfx bmpsy = N_BMP_SY( bmp );
	const n_type_gfx size  = (n_type_gfx) ceil( (n_type_real) n_posix_max_n_type_gfx( bmpsx, bmpsy ) * sqrt( 2 ) );


	n_type_real fsx = bmpsx;
	n_type_real fsy = bmpsy;
	n_type_real tsx =  size;
	n_type_real tsy =  size;

	// [Needed] : rounding to odd numbers

	fsx += 1 - fmod( fsx, 2 );
	fsy += 1 - fmod( fsy, 2 );
	tsx += 1 - fmod( tsx, 2 );
	tsy += 1 - fmod( tsy, 2 );


	// Phase 2 : convert to polar coordinate system

	n_type_real ofx = trunc( fsx / 2 );
	n_type_real ofy = trunc( fsy / 2 );
	n_type_real otx = trunc( tsx / 2 );
	n_type_real oty = trunc( tsy / 2 );


	// Phase 3 : apply matrices

	n_type_real theta = ( 2.0 * M_PI ) * ( (n_type_real) degree / 360 );

	n_type_real matrix[] = {

		cos( theta ), -sin( theta ),
		sin( theta ),  cos( theta ),

	};


	// [!] : for "minimal_size" option

	n_type_real min_x = otx;
	n_type_real min_y = oty;
	n_type_real max_x = otx;
	n_type_real max_y = oty;


	// [!] : some pixels are not accessed

	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, (n_type_gfx) tsx, (n_type_gfx) tsy ); n_bmp_flush( &b, color_bg );


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		(
			( ( fsx * fsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
			||
			( ( tsx * tsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
		)
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_matrix_rotate() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                          *h = (n_thread                         *) n_memory_new( cores * sizeof( n_thread                          ) );
		n_bmp_matrix_rotate_thread_struct *p = (n_bmp_matrix_rotate_thread_struct*) n_memory_new( cores * sizeof( n_bmp_matrix_rotate_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_matrix_rotate_thread_struct tmp = { bmp, &b, matrix, ofx,ofy, otx,oty, tsx,tsy, bmpsx,bmpsy, minimal_size, color_bg, otx,oty, otx,oty, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_matrix_rotate_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_matrix_rotate_thread, &p[ i ] );

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

		i = 0;
		n_posix_loop
		{

			min_x = n_posix_min_n_type_real( min_x, p[ i ].min_x );
			min_y = n_posix_min_n_type_real( min_y, p[ i ].min_y );
			max_x = n_posix_max_n_type_real( max_x, p[ i ].max_x );
			max_y = n_posix_max_n_type_real( max_y, p[ i ].max_y );

			i++;
			if ( i >= cores ) { break; }
		}


		n_memory_free( h );
		n_memory_free( p );


		n_bmp_is_multithread = p_multithread;

	} else {

		n_bmp_matrix_rotate_thread_struct p =
		{
			bmp, &b,
			matrix,
			ofx,ofy, otx,oty,
			tsx,tsy,
			bmpsx,bmpsy,
			minimal_size,
			color_bg,

			min_x,min_y,
			max_x,max_y,

			0,1
		};

		n_bmp_matrix_rotate_thread_main( &p );

		min_x = p.min_x;
		min_y = p.min_y;
		max_x = p.max_x;
		max_y = p.max_y;

	}

//n_posix_debug_literal( "%d", n_posix_tickcount() - tick );


	n_bmp_replace( &b, bmp );

	if ( minimal_size )
	{

		n_type_gfx sx = (n_type_gfx) ceil( max_x - min_x + 1 );
		n_type_gfx sy = (n_type_gfx) ceil( max_y - min_y + 1 );

		n_bmp_resizer( bmp, sx,sy, color_bg, N_BMP_RESIZER_CENTER );

	}


	return;
}

void
n_bmp_matrix_mirror( n_bmp *bmp, int mode )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : three times slower than n_bmp_mirror()


	const n_type_gfx bmpsx = N_BMP_SX( bmp );
	const n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_real matrix[ 4 ] = { 1, 0, 0, 1 };
	n_type_real ox = 0;
	n_type_real oy = 0;


	if ( mode == N_BMP_MIRROR_LEFTSIDE_RIGHT ) 
	{

		n_type_real m[ 4 ] = { -1, 0, 0, 1 };
		n_memory_copy( m, matrix, sizeof( n_type_real ) * 4 );

		ox = bmpsx - 1;
		oy = 0;

	} else
	if ( mode == N_BMP_MIRROR_UPSIDE_DOWN ) 
	{

		n_type_real m[ 4 ] = { 1, 0, 0, -1 };
		n_memory_copy( m, matrix, sizeof( n_type_real ) * 4 );

		ox = 0;
		oy = bmpsy - 1;

	} else

	if ( mode == N_BMP_MIRROR_ROTATE180 ) 
	{

		n_bmp_flush_mirror( bmp, N_BMP_MIRROR_ROTATE180 );

		return;

	} else

	//
	{
		return;
	}


	// [!] : don't use polar system : output will be inaccurate

	n_bmp b; n_bmp_zero( &b ); n_bmp_1st( &b, bmpsx, bmpsy );


	n_type_real tx  = 0;
	n_type_real ty  = 0;
	n_type_real tsx = bmpsx;
	n_type_real tsy = bmpsy;
	n_posix_loop
	{

		n_type_real fx = ( tx * matrix[ 0 ] ) + ( ty * matrix[ 2 ] ) + ox;
		n_type_real fy = ( tx * matrix[ 1 ] ) + ( ty * matrix[ 3 ] ) + oy;

		u32  clr = 0;
		n_posix_bool ret = n_bmp_ptr_get( bmp, (n_type_gfx) fx, (n_type_gfx) fy, &clr );
		if ( ret == n_posix_false )
		{
			n_bmp_ptr_set( &b, (n_type_gfx) tx, (n_type_gfx) ty, clr );
		}


		tx++;
		if ( tx >= tsx )
		{

			tx = 0;

			ty++;
			if ( ty >= tsy ) { break; }
		}
	}


	n_bmp_free_fast( bmp );
	n_bmp_alias( &b, bmp );


	return;
}


#endif // _H_NONNON_NEUTRAL_BMP_MATRIX

