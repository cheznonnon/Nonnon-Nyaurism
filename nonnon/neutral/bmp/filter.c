// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_FILTER
#define _H_NONNON_NEUTRAL_BMP_FILTER




#include "./color.c"
#include "./draw.c"
#include "./transform.c"




typedef struct {

	n_bmp      *bmp;
	n_bmp      *target;
	n_type_gfx  x,y,sx,sy;
	n_type_real blend;
	u32         oy, cores;

} n_bmp_antialias_thread_struct;

void
n_bmp_antialias_thread_main( n_bmp_antialias_thread_struct *p )
{

	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }

	// [!] : 10% faster

	n_type_index oy = n_bmp_ptr_pos2index( p->target, tx, ty );

	// [!] : Copilot Optimization

	u32 *tbase = N_BMP_PTR( p->target );
	u32 *trow  = tbase + oy;

	if ( p->bmp->transparent_onoff )
	{

		n_posix_loop
		{

			u32 color = n_bmp_antialias_pixel( p->bmp, p->x + tx, p->y + ty, p->blend );

			if ( color != n_bmp_trans )
			{
				//n_bmp_ptr_set_fast( p->target, tx,ty, color );
				//N_BMP_PTR( p->target )[ tx + oy ] = color;
				trow[ tx ] = color;
			}


			tx++;
			if ( tx >= p->sx ) 
			{

				tx = 0;

				ty += p->cores;
				if ( ty >= p->sy ) { break; }

				oy = n_bmp_ptr_pos2index( p->target, tx, ty );
				trow = tbase + oy;
			}

		}

	} else {

		n_posix_loop
		{

			u32 color = n_bmp_antialias_pixel( p->bmp, p->x + tx, p->y + ty, p->blend );

			//n_bmp_ptr_set_fast( p->target, tx,ty, color );
			//N_BMP_PTR( p->target )[ tx + oy ] = color;
			trow[ tx ] = color;


			tx++;
			if ( tx >= p->sx ) 
			{

				tx = 0;

				ty += p->cores;
				if ( ty >= p->sy ) { break; }

				oy = n_bmp_ptr_pos2index( p->target, tx, ty );
				trow = tbase + oy;
			}

		}

	}


	return;
}

n_thread_return
n_bmp_antialias_thread( n_thread_argument p )
{

	n_bmp_antialias_thread_main( (n_bmp_antialias_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_antialias( bmp, blend ) \
	n_bmp_antialias( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), blend )

void
n_bmp_antialias( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_real blend )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_bmp target; n_bmp_zero( &target ); n_bmp_1st_fast( &target, sx,sy );

	n_bmp_fastcopy( bmp,&target, x,y,sx,sy, 0,0 );


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= ( N_BMP_MULTITHREAD_GRANULARITY / 10 ) )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_antialias() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                      *h = (n_thread                     *) n_memory_new( cores * sizeof( n_thread                      ) );
		n_bmp_antialias_thread_struct *p = (n_bmp_antialias_thread_struct*) n_memory_new( cores * sizeof( n_bmp_antialias_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_antialias_thread_struct tmp = { bmp,&target, x,y,sx,sy, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_antialias_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_antialias_thread, &p[ i ] );

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

		n_bmp_antialias_thread_struct tmp = { bmp,&target, x,y,sx,sy, blend, 0,1 };
		n_bmp_antialias_thread_main( &tmp );

	}


	n_bmp_fastcopy( &target,bmp, 0,0,sx,sy, x,y );

	n_bmp_free_fast( &target );


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_bmp      *target;
	n_type_gfx  x,y,sx,sy;
	n_type_gfx  size;
	int         weight;
	n_type_real blend;
	u32         oy, cores;

} n_bmp_blur_thread_struct;

void
n_bmp_blur_thread_main( n_bmp_blur_thread_struct *p )
{

	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color = n_bmp_blur_pixel( p->bmp, p->x + tx, p->y + ty, p->size, p->weight, p->blend );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			n_bmp_ptr_set_fast( p->target, tx,ty, color );
		}


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
n_bmp_blur_thread( n_thread_argument p )
{

	n_bmp_blur_thread_main( (n_bmp_blur_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_blur( bmp, size, weight, blend ) \
	n_bmp_blur( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), size, weight, blend )

void
n_bmp_blur( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_gfx size, int weight, n_type_real blend )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_bmp target; n_bmp_zero( &target ); n_bmp_1st_fast( &target, sx,sy );

	n_bmp_fastcopy( bmp,&target, x,y,sx,sy, 0,0 );


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		//&&
		//( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_blur() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                 *h = (n_thread                *) n_memory_new( cores * sizeof( n_thread                 ) );
		n_bmp_blur_thread_struct *p = (n_bmp_blur_thread_struct*) n_memory_new( cores * sizeof( n_bmp_blur_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_blur_thread_struct tmp = { bmp,&target, x,y,sx,sy, size, weight, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_blur_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_blur_thread, &p[ i ] );

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

		n_bmp_blur_thread_struct tmp = { bmp,&target, x,y,sx,sy, size, weight, blend, 0,1 };
		n_bmp_blur_thread_main( &tmp );

	}


	n_bmp_fastcopy( &target,bmp, 0,0,sx,sy, x,y );

	n_bmp_free( &target );


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	int         param;
	u32         oy, cores;

} n_bmp_contrast_thread_struct;

void
n_bmp_contrast_thread_main( n_bmp_contrast_thread_struct *p )
{

	n_posix_bool is_flush = ( ( p->x == 0 )&&( p->y == 0 )&&( p->sx == N_BMP_SX( p->bmp ) )&&( p->sy == N_BMP_SY( p->bmp ) ) );
	if ( is_flush )
	{
//n_posix_debug_literal( "fast mode" );

		n_type_index c = N_BMP_SX( p->bmp ) * N_BMP_SY( p->bmp );
		n_type_index i = p->oy; if ( i >= c ) { return; }
		n_posix_loop
		{

			u32 color = N_BMP_PTR( p->bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
			{
				N_BMP_PTR( p->bmp )[ i ] = n_bmp_contrast_pixel( color, p->param );
			}

			i += p->cores;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( p->bmp, p->x + tx, p->y + ty, &color );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			color = n_bmp_contrast_pixel( color, p->param );

			n_bmp_ptr_set_fast( p->bmp, p->x + tx, p->y + ty, color );
		}


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
n_bmp_contrast_thread( n_thread_argument p )
{

	n_bmp_contrast_thread_main( (n_bmp_contrast_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_contrast( bmp, param ) \
	n_bmp_contrast( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), param )

void
n_bmp_contrast( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int param )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_contrast() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                     *h = (n_thread                    *) n_memory_new( cores * sizeof( n_thread                     ) );
		n_bmp_contrast_thread_struct *p = (n_bmp_contrast_thread_struct*) n_memory_new( cores * sizeof( n_bmp_contrast_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_contrast_thread_struct tmp = { bmp, x,y,sx,sy, param, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_contrast_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_contrast_thread, &p[ i ] );

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

		n_bmp_contrast_thread_struct tmp = { bmp, x,y,sx,sy, param, 0,1 };
		n_bmp_contrast_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_bmp      *target;
	n_type_gfx  x,y,sx,sy;
	n_type_real blend;
	u32         oy, cores;

} n_bmp_sharpen_thread_struct;

void
n_bmp_sharpen_thread_main( n_bmp_sharpen_thread_struct *p )
{

	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color = n_bmp_sharpen_pixel( p->bmp, p->x + tx, p->y + ty, p->blend );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			n_bmp_ptr_set_fast( p->target, tx,ty, color );
		}


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
n_bmp_sharpen_thread( n_thread_argument p )
{

	n_bmp_sharpen_thread_main( (n_bmp_sharpen_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_sharpen( bmp, blend ) \
	n_bmp_sharpen( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), blend )

void
n_bmp_sharpen( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_real blend )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_bmp target; n_bmp_zero( &target ); n_bmp_1st_fast( &target, sx,sy );

	n_bmp_fastcopy( bmp,&target, x,y,sx,sy, 0,0 );


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_sharpen() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                    *h = (n_thread                   *) n_memory_new( cores * sizeof( n_thread                    ) );
		n_bmp_sharpen_thread_struct *p = (n_bmp_sharpen_thread_struct*) n_memory_new( cores * sizeof( n_bmp_sharpen_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_sharpen_thread_struct tmp = { bmp, &target, x,y,sx,sy, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_sharpen_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_sharpen_thread, &p[ i ] );

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

		n_bmp_sharpen_thread_struct tmp = { bmp, &target, x,y,sx,sy, blend, 0,1 };
		n_bmp_sharpen_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	n_bmp_fastcopy( &target,bmp, 0,0,sx,sy, x,y );

	n_bmp_free_fast( &target );


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	n_type_real gamma;
	u32         oy, cores;

} n_bmp_gamma_thread_struct;

void
n_bmp_gamma_thread_main( n_bmp_gamma_thread_struct *p )
{

	n_posix_bool is_flush = ( ( p->x == 0 )&&( p->y == 0 )&&( p->sx == N_BMP_SX( p->bmp ) )&&( p->sy == N_BMP_SY( p->bmp ) ) );
	if ( is_flush )
	{
//n_posix_debug_literal( "fast mode" );

		n_type_index c = N_BMP_SX( p->bmp ) * N_BMP_SY( p->bmp );
		n_type_index i = p->oy; if ( i >= c ) { return; }
		n_posix_loop
		{

			u32 color = N_BMP_PTR( p->bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
			{
				N_BMP_PTR( p->bmp )[ i ] = n_bmp_gamma_pixel( color, p->gamma );
			}

			i += p->cores;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( p->bmp, p->x + tx, p->y + ty, &color );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			color = n_bmp_gamma_pixel( color, p->gamma );
			n_bmp_ptr_set_fast( p->bmp, p->x + tx, p->y + ty, color );
		}


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
n_bmp_gamma_thread( n_thread_argument p )
{

	n_bmp_gamma_thread_main( (n_bmp_gamma_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_gamma( bmp, gamma ) \
	n_bmp_gamma( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), gamma )

void
n_bmp_gamma( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, n_type_real gamma )
{

	// [Mechanism]
	//
	//	gamma = 0.0 : black
	//	gamma = 1.0 : nothing to do
	//	gamma = 2.0 : white


	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [!] : nothing to do

	if ( gamma == 1.0 ) { return; }


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_gamma() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                  *h = (n_thread                 *) n_memory_new( cores * sizeof( n_thread                  ) );
		n_bmp_gamma_thread_struct *p = (n_bmp_gamma_thread_struct*) n_memory_new( cores * sizeof( n_bmp_gamma_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_gamma_thread_struct tmp = { bmp, x,y,sx,sy, gamma, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_gamma_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_gamma_thread, &p[ i ] );

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

		n_bmp_gamma_thread_struct tmp = { bmp, x,y,sx,sy, gamma, 0,1 };
		n_bmp_gamma_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	return;
}

#define N_BMP_COPY_MIRROR_NONE             N_BMP_MIRROR_NONE
#define N_BMP_COPY_MIRROR_LEFTSIDE_RIGHT   N_BMP_MIRROR_LEFTSIDE_RIGHT
#define N_BMP_COPY_MIRROR_UPSIDE_DOWN      N_BMP_MIRROR_UPSIDE_DOWN
#define N_BMP_COPY_MIRROR_ROTATE180        N_BMP_MIRROR_ROTATE180

#define N_BMP_COPY_ROTATE_NONE             N_BMP_ROTATE_NONE
#define N_BMP_COPY_ROTATE_LEFT             N_BMP_ROTATE_LEFT
#define N_BMP_COPY_ROTATE_RIGHT            N_BMP_ROTATE_RIGHT

#define n_bmp_copy(          f,t, fx,fy, fsx,fsy, tx,ty,                              fnlz, blend, mirror, rotate, edge ) \
	n_bmp_copy_internal( f,t, fx,fy, fsx,fsy, tx,ty, n_posix_false,n_posix_false, fnlz, blend, mirror, rotate, edge )

#define n_bmp_mirrorcopy( f,t, fx,fy, fsx,fsy, tx,ty, fnlz,      m       ) \
	n_bmp_copy(       f,t, fx,fy, fsx,fsy, tx,ty, fnlz, 0.0, m, 0, 0 )

#define n_bmp_rotatecopy( f,t, fx,fy, fsx,fsy, tx,ty, fnlz,         r    ) \
	n_bmp_copy(       f,t, fx,fy, fsx,fsy, tx,ty, fnlz, 0.0, 0, r, 0 )

#define n_bmp_smoothcopy( f,t, fx,fy, fsx,fsy, tx,ty, fnlz,            s ) \
	n_bmp_copy(       f,t, fx,fy, fsx,fsy, tx,ty, fnlz, 0.0, 0, 0, s )

#define n_bmp_flush_copy( f,t,                                       fnlz, blend, mirror, rotate, edge ) \
	n_bmp_copy(       f,t, 0,0,N_BMP_SX( f ),N_BMP_SY( f ), 0,0, fnlz, blend, mirror, rotate, edge )

#define n_bmp_flush_mirrorcopy( f,t, fnlz,      m       ) \
	n_bmp_flush_copy(       f,t, fnlz, 0.0, m, 0, 0 )

#define n_bmp_flush_rotatecopy( f,t, fnlz,         r    ) \
	n_bmp_flush_copy(       f,t, fnlz, 0.0, 0, r, 0 )

#define n_bmp_flush_smoothcopy( f,t, fnlz,            s ) \
	n_bmp_flush_copy(       f,t, fnlz, 0.0, 0, 0, s )

// internal
void
n_bmp_copy_internal
(
	const n_bmp *fr, n_bmp *to,
	n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy,
	n_type_gfx tx, n_type_gfx ty,
	n_posix_bool perpixel_override,
	n_posix_bool perpixel_override_onoff,
	n_posix_bool finalize,
	n_type_real  blend,
	int          mirror,
	int          rotate,
	n_type_gfx   edge
)
{

	if ( n_bmp_error_clipping( fr,to, &fx,&fy,&fsx,&fsy, &tx,&ty ) ) { return; }


	const n_type_real coef_edge = 1.0 / 9.0;


	n_type_gfx bmpsx = N_BMP_SX( fr );
	n_type_gfx bmpsy = N_BMP_SY( fr );


	n_bmp target; n_bmp_zero( &target );

	if (
		( mirror == N_BMP_COPY_MIRROR_NONE )
		&&
		( rotate == N_BMP_COPY_ROTATE_NONE )
	)
	{

		n_bmp_1st_fast( &target, bmpsx,bmpsy );
		n_bmp_fastcopy( fr, &target, 0,0,bmpsx,bmpsy, 0,0 );

	} else {

		if ( bmpsx == bmpsy )
		{

			n_bmp b; n_bmp_carboncopy( fr, &b );

			if ( mirror != N_BMP_COPY_MIRROR_NONE )
			{
				n_bmp_flush_mirror( &b, mirror );
			}

			if ( rotate != N_BMP_COPY_ROTATE_NONE )
			{
				n_bmp_rotate( &b, rotate );
			}

			n_bmp_1st_fast( &target, bmpsx,bmpsy );
			n_bmp_fastcopy( &b, &target, 0,0,bmpsx,bmpsy, 0,0 );

			n_bmp_free( &b );

		} else {

			n_bmp b; n_bmp_carboncopy( fr, &b );

			if ( mirror != N_BMP_COPY_MIRROR_NONE )
			{
				n_bmp_flush_mirror( &b, mirror );
			}

			if ( rotate != N_BMP_COPY_ROTATE_NONE )
			{
				// [x] : not supported
				//n_bmp_rotate( &b, rotate );
			}

			n_bmp_1st_fast( &target, bmpsx,bmpsy );
			n_bmp_fastcopy( &b, &target, 0,0,bmpsx,bmpsy, 0,0 );

			n_bmp_free( &b );

		}

	}


	n_posix_bool write_needed = n_posix_false;

	n_posix_bool trans_onoff  = target.transparent_onoff;


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		n_type_gfx edge_count;
		if ( n_bmp_edge_detect( &target, fx + x, fy + y, edge, &edge_count ) )
		{

			u32 f,t; n_bmp_ptr_get_fast( &target, fx + x, fy + y, &f );

			if ( edge & N_BMP_EDGE_INNER )
			{
				f = n_bmp_antialias_pixel( &target, fx + x, fy + y, 1.0 );
			} else {
				f = n_bmp_antialias_pixel( to, tx + x, ty + y, 1.0 );
			}

			if ( n_posix_false == n_bmp_is_trans( &target, f ) )
			{
				n_bmp_ptr_get_fast( to, tx + x, ty + y, &t );

				n_type_real d = (n_type_real) edge_count * coef_edge * ( 1.0 - blend );

				t = n_bmp_blend_pixel( t,f, d );

				n_bmp_ptr_set( to, tx + x, ty + y, t );
			}

		} else {

			u32 color = n_bmp_composite_pixel_fast
			(
				&target, to,
				fx + x, fy + y, tx + x, ty + y,
				perpixel_override,
				perpixel_override_onoff,
				finalize,
				trans_onoff,
				blend,
				&write_needed
			);

			if ( write_needed )
			{
				n_bmp_ptr_set( to, tx + x, ty + y, color );
			}

		}


		x++;
		if ( x >= fsx )
		{

			x = 0;

			y++;
			if ( y >= fsy ) { break; }
		}
	}


	n_bmp_free( &target );


	return;
}

#define n_bmp_blendcopy(          f,t, fx,fy, fsx,fsy, tx,ty,               blend ) \
        n_bmp_blendcopy_internal( f,t, fx,fy, fsx,fsy, tx,ty, n_posix_true, blend )

#define n_bmp_transcopy( f,t, fx,fy, fsx,fsy, tx,ty      ) \
        n_bmp_blendcopy( f,t, fx,fy, fsx,fsy, tx,ty, 0.0 )

#define n_bmp_flush_blendcopy( f,t, blend ) n_bmp_blendcopy( f,t, 0,0, N_BMP_SX( f ),N_BMP_SY( f ), 0,0, blend )
#define n_bmp_flush_transcopy( f,t        ) n_bmp_flush_blendcopy( f,t, 0.0 )

#define n_bmp_blendcopy_no_finalize( f,t, fx,fy, fsx,fsy, tx,ty,                blend ) \
        n_bmp_blendcopy_internal(    f,t, fx,fy, fsx,fsy, tx,ty, n_posix_false, blend )

#define n_bmp_transcopy_no_finalize( f,t, fx,fy, fsx,fsy, tx,ty      ) \
        n_bmp_blendcopy_no_finalize( f,t, fx,fy, fsx,fsy, tx,ty, 0.0 )

typedef struct {

	const n_bmp        *bmp_f;
	      n_bmp        *bmp_t;
	      n_type_gfx    fx,fy,fsx,fsy, tx,ty;
	      n_posix_bool  perpixel_override;
	      n_posix_bool  perpixel_override_onoff;
	      n_posix_bool  finalize;
	      n_type_real   blend;
	      u32           oy, cores;

} n_bmp_blendcopy_thread_struct;

void
n_bmp_blendcopy_thread_main( n_bmp_blendcopy_thread_struct *p )
{

	n_posix_bool write_needed = n_posix_false;
	n_posix_bool trans_needed = p->bmp_f->transparent_onoff;


	n_posix_bool is_flush =
	(
		( p->fx == 0 )&&( p->fy == 0 )
		&&
		( N_BMP_SX( p->bmp_f ) == N_BMP_SX( p->bmp_t ) )
		&&
		( N_BMP_SY( p->bmp_f ) == N_BMP_SY( p->bmp_t ) )
		&&
		( N_BMP_SX( p->bmp_f ) == p->fsx )
		&&
		( N_BMP_SY( p->bmp_f ) == p->fsy )
		&&
		( p->tx == 0 )&&( p->ty == 0 )
	);

	if ( is_flush )
	{
//n_posix_debug_literal( "fast mode" );

		n_type_index c = p->fsx * p->fsy;
		n_type_index i = p->oy; if ( i >= c ) { return; }
		n_posix_loop
		{//break;

			u32 color = n_bmp_composite_pixel_ultrafast
			(
				p->bmp_f, p->bmp_t,
				       i,        i,
				p->perpixel_override,
				p->perpixel_override_onoff,
				p->finalize,
				trans_needed,
				p->blend,
				&write_needed
			);

			if ( write_needed )
			{
				N_BMP_PTR( p->bmp_t )[ i ] = color;
			}

			i += p->cores;
			if ( i >= c ) { break; }
		}


		return;
	}

/*
	n_type_gfx x = 0;
	n_type_gfx y = p->oy; if ( y >= p->fsy ) { return; }
	n_posix_loop
	{


		u32 color = n_bmp_composite_pixel_fast
		(
			p->bmp_f, p->bmp_t,
			p->fx + x, p->fy + y, p->tx + x, p->ty + y,
			p->perpixel_override,
			p->perpixel_override_onoff,
			p->finalize,
			p->trans_onoff,
			p->blend,
			&write_needed
		);

		if ( write_needed )
		{
			n_bmp_ptr_set( p->bmp_t, p->tx + x, p->ty + y, color );
//if ( ( p->tx + x ) >= N_BMP_SX( p->bmp_t ) ) { n_posix_debug_literal( "" ); }
		}


		x++;
		if ( x >= p->fsx )
		{

			x = 0;

			y += p->cores;
			if ( y >= p->fsy ) { break; }

//if ( ( p->ty + y ) >= N_BMP_SY( p->bmp_t ) ) { n_posix_debug_literal( "" ); }
		}
	}
*/

	n_type_gfx x = 0;
	n_type_gfx y = p->oy; if ( y >= p->fsy ) { return; }

	n_type_index index_f = n_bmp_ptr_pos2index( p->bmp_f, p->fx + x, p->fy + y );
	n_type_index index_t = n_bmp_ptr_pos2index( p->bmp_t, p->tx + x, p->ty + y );

	n_posix_loop
	{

		u32 color = n_bmp_composite_pixel_ultrafast
		(
			p->bmp_f, p->bmp_t,
			 index_f,  index_t,
			p->perpixel_override,
			p->perpixel_override_onoff,
			p->finalize,
			trans_needed,
			p->blend,
			&write_needed
		);

		if ( write_needed )
		{
			N_BMP_PTR( p->bmp_t )[ index_t ] = color;
		}


		index_f++;
		index_t++;

		x++;
		if ( x >= p->fsx )
		{

			x = 0;

			y += p->cores;
			if ( y >= p->fsy ) { break; }

			index_f = n_bmp_ptr_pos2index( p->bmp_f, p->fx + x, p->fy + y );
			index_t = n_bmp_ptr_pos2index( p->bmp_t, p->tx + x, p->ty + y );
		}
	}


	return;
}

n_thread_return
n_bmp_blendcopy_thread( n_thread_argument p )
{

	n_bmp_blendcopy_thread_main( (n_bmp_blendcopy_thread_struct*) p );

	return 0;
}

#define n_bmp_blendcopy_internal( fr,to, fx,fy,fsx,fsy, tx,ty,                              fnlz, blend ) \
        n_bmp_blendcopy_all(      fr,to, fx,fy,fsx,fsy, tx,ty, n_posix_false,n_posix_false, fnlz, blend )

// internal
void
n_bmp_blendcopy_all
(
	const n_bmp *fr, n_bmp *to,
	n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy,
	n_type_gfx tx, n_type_gfx ty,
	n_posix_bool perpixel_override,
	n_posix_bool perpixel_override_onoff,
	n_posix_bool finalize,
	n_type_real  blend
)
{

	// [Mechanism] : simple/original/faster version of n_bmp_copy()
	//
	//	300% faster than n_bmp_copy()


	if ( n_bmp_error_clipping( fr,to, &fx,&fy,&fsx,&fsy, &tx,&ty ) ) { return; }


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( fsx * fsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_blendcopy() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                      *h = (n_thread                     *) n_memory_new( cores * sizeof( n_thread                      ) );
		n_bmp_blendcopy_thread_struct *p = (n_bmp_blendcopy_thread_struct*) n_memory_new( cores * sizeof( n_bmp_blendcopy_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_blendcopy_thread_struct tmp = { fr,to, fx,fy,fsx,fsy, tx,ty, perpixel_override,perpixel_override_onoff, finalize, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_blendcopy_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_blendcopy_thread, &p[ i ] );

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

		n_bmp_blendcopy_thread_struct p = { fr,to, fx,fy,fsx,fsy, tx,ty, perpixel_override,perpixel_override_onoff, finalize, blend, 0,1 };

		n_bmp_blendcopy_thread_main( &p );

	}


	return;
}

typedef struct {

	const n_bmp        *bmp_f;
	      n_bmp        *bmp_t;
	      n_type_gfx    fx,fy,fsx,fsy, tx,ty;
	const n_bmp        *bmp_clip;
	      n_type_gfx    cx,cy;
	      n_posix_bool  finalize;
	      n_posix_bool  trans_onoff;
	      n_type_real   blend;
	      u32           oy, cores;

} n_bmp_clipcopy_thread_struct;

void
n_bmp_clipcopy_thread_main( n_bmp_clipcopy_thread_struct *p )
{

	n_posix_bool write_needed = n_posix_false;


	n_type_gfx x = 0;
	n_type_gfx y = p->oy; if ( y >= p->fsy ) { return; }
	n_posix_loop
	{

		u32 color_clip = n_bmp_black_invisible;
		n_bmp_ptr_get( p->bmp_clip, p->cx + x, p->cy + y, &color_clip );
//n_bmp_ptr_set( (void*) p->bmp_clip, p->cx + x, p->cy + y, n_bmp_white );

		int alpha = n_bmp_a( color_clip );

		if ( N_BMP_ALPHA_CHANNEL_VISIBLE != alpha )
		{

			n_type_real blend = p->blend;

			if ( N_BMP_ALPHA_CHANNEL_INVISIBLE != alpha )
			{
				blend = 1.0 - n_bmp_blend_alpha2ratio( alpha );
			}

			u32 color = n_bmp_composite_pixel_fast
			(
				p->bmp_f, p->bmp_t,
				p->fx + x, p->fy + y, p->tx + x, p->ty + y,
				n_posix_false,//p->perpixel_override,
				n_posix_false,//p->perpixel_override_onoff,
				p->finalize,
				p->trans_onoff,
				blend,
				&write_needed
			);

			if ( write_needed )
			{
//color = n_bmp_rgb( 0,200,255 );
				n_bmp_ptr_set( p->bmp_t, p->tx + x, p->ty + y, color );
			}
		}


		x++;
		if ( x >= p->fsx )
		{

			x = 0;

			y += p->cores;
			if ( y >= p->fsy ) { break; }
		}
	}

	return;
}

n_thread_return
n_bmp_clipcopy_thread( n_thread_argument p )
{

	n_bmp_clipcopy_thread_main( (n_bmp_clipcopy_thread_struct*) p );

	return 0;
}

#define n_bmp_clipcopy(             f,t, fx,fy, fsx,fsy, tx,ty, clip,cx,cy,               blend ) \
        n_bmp_clipcopy_internal(    f,t, fx,fy, fsx,fsy, tx,ty, clip,cx,cy, n_posix_true, blend )

#define n_bmp_clipcopy_no_finalize( f,t, fx,fy, fsx,fsy, tx,ty, clip,cx,cy,                blend ) \
        n_bmp_clipcopy_internal(    f,t, fx,fy, fsx,fsy, tx,ty, clip,cx,cy, n_posix_false, blend )

// internal
void
n_bmp_clipcopy_internal
(
	const n_bmp *fr, n_bmp *to,
	n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy,
	n_type_gfx tx, n_type_gfx ty,
	const n_bmp *clip,
	n_type_gfx cx, n_type_gfx cy,
	n_posix_bool finalize,
	n_type_real  blend
)
{

	// [Mechanism] : simple/original/faster version of n_bmp_copy()
	//
	//	300% faster than n_bmp_copy()


	n_type_gfx ptx = tx;
	n_type_gfx pty = ty;

	if ( n_bmp_error_clipping( fr,to, &fx,&fy,&fsx,&fsy, &tx,&ty ) ) { return; }

	if ( tx == 0 ) { cx -= ptx; }
	if ( ty == 0 ) { cy -= pty; }


	n_posix_bool trans = fr->transparent_onoff;


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( fsx * fsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_clipcopy() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                     *h = (n_thread                    *) n_memory_new( cores * sizeof( n_thread                     ) );
		n_bmp_clipcopy_thread_struct *p = (n_bmp_clipcopy_thread_struct*) n_memory_new( cores * sizeof( n_bmp_clipcopy_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_clipcopy_thread_struct tmp = { fr,to, fx,fy,fsx,fsy, tx,ty, clip,cx,cy, finalize, trans, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_clipcopy_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_clipcopy_thread, &p[ i ] );

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

		n_bmp_clipcopy_thread_struct p = { fr,to, fx,fy,fsx,fsy, tx,ty, clip,cx,cy, finalize, trans, blend, 0,1 };

		n_bmp_clipcopy_thread_main( &p );

	}


	return;
}

void
n_bmp_flush_monochrome( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_index i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];


		// [!] : alpha is reserved

		if ( bmp->transparent_onoff )
		{

			if ( color != n_bmp_trans )
			{
				N_BMP_PTR( bmp )[ i ] = n_bmp_white | ( color & 0xff000000 );
			}

		} else {

			if ( color != n_bmp_white )
			{
				N_BMP_PTR( bmp )[ i ] = n_bmp_black | ( color & 0xff000000 );
			}

		}


		i++;
		if ( i >= c ) { break; }
	}


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	u32         color_f;
	u32         color_t;
	u32         oy, cores;

} n_bmp_replacer_thread_struct;

void
n_bmp_replacer_thread_main( n_bmp_replacer_thread_struct *p )
{

	n_posix_bool is_flush = ( ( p->x == 0 )&&( p->y == 0 )&&( p->sx == N_BMP_SX( p->bmp ) )&&( p->sy == N_BMP_SY( p->bmp ) ) );
	if ( is_flush )
	{
//n_posix_debug_literal( "fast mode" );

		n_type_index c = N_BMP_SX( p->bmp ) * N_BMP_SY( p->bmp );
		n_type_index i = p->oy; if ( i >= c ) { return; }
		n_posix_loop
		{

			u32 color = N_BMP_PTR( p->bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
			{
				if ( color == p->color_f )
				{
					N_BMP_PTR( p->bmp )[ i ] = p->color_t;
				}
			}

			i += p->cores;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( p->bmp, p->x + tx, p->y + ty, &color );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			if ( color == p->color_f )
			{
				n_bmp_ptr_set_fast( p->bmp, p->x + tx, p->y + ty, p->color_t );
			}
		}


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
n_bmp_replacer_thread( n_thread_argument p )
{

	n_bmp_replacer_thread_main( (n_bmp_replacer_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_replacer( bmp, color_f, color_t ) \
	n_bmp_replacer( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color_f, color_t )

void
n_bmp_replacer( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color_f, u32 color_t )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_replacer() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                     *h = (n_thread                    *) n_memory_new( cores * sizeof( n_thread                     ) );
		n_bmp_replacer_thread_struct *p = (n_bmp_replacer_thread_struct*) n_memory_new( cores * sizeof( n_bmp_replacer_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_replacer_thread_struct tmp = { bmp, x,y,sx,sy, color_f,color_t, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_replacer_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_replacer_thread, &p[ i ] );

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

		n_bmp_replacer_thread_struct tmp = { bmp, x,y,sx,sy, color_f,color_t, 0,1 };
		n_bmp_replacer_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	return;
}

void
n_bmp_flush_grayscale( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_index i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];

		if ( n_posix_false == n_bmp_is_trans( bmp, color ) )
		{
			N_BMP_PTR( bmp )[ i ] = n_bmp_grayscale_pixel( color );
		}


		i++;
		if ( i >= c ) { break; }
	}


	return;
}

#define n_bmp_flush_tweaker( bmp, a, r, g, b ) \
	n_bmp_tweaker( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), a, r, g, b )

void
n_bmp_tweaker( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int a, int r, int g, int b )
{

	// [!] : nothing to do

	if ( 0 == n_bmp_argb( a,r,g,b ) ) { return; }


	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_posix_bool is_flush = ( ( x == 0 )&&( y == 0 )&&( sx == N_BMP_SX( bmp ) )&&( sy == N_BMP_SY( bmp ) ) );
	if ( is_flush )
	{

		n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
		n_type_index i = 0;
		n_posix_loop
		{

			u32 color = N_BMP_PTR( bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( bmp, color ) )
			{

				int aa = n_bmp_clamp_channel( n_bmp_a( color ) + a );
				int rr = n_bmp_clamp_channel( n_bmp_r( color ) + r );
				int gg = n_bmp_clamp_channel( n_bmp_g( color ) + g );
				int bb = n_bmp_clamp_channel( n_bmp_b( color ) + b );

				N_BMP_PTR( bmp )[ i ] = n_bmp_argb( aa,rr,gg,bb );

			}

			i++;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, tx + x, ty + y, &color );

		if ( n_posix_false == n_bmp_is_trans( bmp, color ) )
		{

			int aa = n_bmp_clamp_channel( n_bmp_a( color ) + a );
			int rr = n_bmp_clamp_channel( n_bmp_r( color ) + r );
			int gg = n_bmp_clamp_channel( n_bmp_g( color ) + g );
			int bb = n_bmp_clamp_channel( n_bmp_b( color ) + b );

			color = n_bmp_argb( aa,rr,gg,bb );

			n_bmp_ptr_set_fast( bmp, tx + x, ty + y, color );

		}


		tx++;
		if ( tx >= sx )
		{

			tx = 0;

			ty++;
			if ( ty >= sy ) { break; }
		}
	}


	return;
}

typedef struct {

	n_bmp      *bmp;
	n_type_gfx  x,y,sx,sy;
	int         hue, saturation, lightness;
	u32         oy, cores;

} n_bmp_tweaker_hsl_thread_struct;

void
n_bmp_tweaker_hsl_thread_main( n_bmp_tweaker_hsl_thread_struct *p )
{

	n_posix_bool is_flush = ( ( p->x == 0 )&&( p->y == 0 )&&( p->sx == N_BMP_SX( p->bmp ) )&&( p->sy == N_BMP_SY( p->bmp ) ) );
	if ( is_flush )
	{
//n_posix_debug_literal( "fast mode" );

		n_type_index c = N_BMP_SX( p->bmp ) * N_BMP_SY( p->bmp );
		n_type_index i = p->oy; if ( i >= c ) { return; }
		n_posix_loop
		{

			u32 color = N_BMP_PTR( p->bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
			{
				N_BMP_PTR( p->bmp )[ i ] = n_bmp_hsl_tweak_pixel( color, p->hue, p->saturation, p->lightness );
			}

			i += p->cores;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy; if ( ty >= p->sy ) { return; }
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( p->bmp, p->x + tx, p->y + ty, &color );

		if ( n_posix_false == n_bmp_is_trans( p->bmp, color ) )
		{
			color = n_bmp_hsl_tweak_pixel( color, p->hue, p->saturation, p->lightness );

			n_bmp_ptr_set_fast( p->bmp, p->x + tx, p->y + ty, color );
		}


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
n_bmp_tweaker_hsl_thread( n_thread_argument p )
{

	n_bmp_tweaker_hsl_thread_main( (n_bmp_tweaker_hsl_thread_struct*) p );

	return 0;
}

#define n_bmp_flush_hue_wheel( bmp, h ) n_bmp_flush_tweaker_hsl( bmp, h, 0, 0 )
#define n_bmp_flush_vividness( bmp, s ) n_bmp_flush_tweaker_hsl( bmp, 0, s, 0 )
#define n_bmp_flush_lightness( bmp, l ) n_bmp_flush_tweaker_hsl( bmp, 0, 0, l )

#define n_bmp_flush_tweaker_hsl( bmp, h, s, l ) \
	n_bmp_tweaker_hsl( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), h, s, l )

void
n_bmp_tweaker_hsl( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int hue, int saturation, int lightness )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [!] : nothing to do

	if ( ( hue == 0 )&&( saturation == 0 )&&( lightness == 0 ) ) { return; }


	// [x] : Win9x : can run but not working

//u32 tick = n_posix_tickcount();

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_tweaker_hsl() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                        *h = (n_thread                       *) n_memory_new( cores * sizeof( n_thread                        ) );
		n_bmp_tweaker_hsl_thread_struct *p = (n_bmp_tweaker_hsl_thread_struct*) n_memory_new( cores * sizeof( n_bmp_tweaker_hsl_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_tweaker_hsl_thread_struct tmp = { bmp, x,y,sx,sy, hue, saturation, lightness, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_tweaker_hsl_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_tweaker_hsl_thread, &p[ i ] );

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

		n_bmp_tweaker_hsl_thread_struct tmp = { bmp, x,y,sx,sy, hue, saturation, lightness, 0,1 };
		n_bmp_tweaker_hsl_thread_main( &tmp );

	}

//n_posix_debug_literal( " %d ", (int) n_posix_tickcount() - tick );


	return;
}

#define n_bmp_flush_mixer( bmp, color_mix, blend ) \
	n_bmp_mixer( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), color_mix, blend )

void
n_bmp_mixer( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 color_mix, n_type_real blend )
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	n_posix_bool is_flush = ( ( x == 0 )&&( y == 0 )&&( sx == N_BMP_SX( bmp ) )&&( sy == N_BMP_SY( bmp ) ) );
	if ( is_flush )
	{

		n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
		n_type_index i = 0;
		n_posix_loop
		{

			u32 color_f = N_BMP_PTR( bmp )[ i ];
			if ( n_posix_false == n_bmp_is_trans( bmp, color_f ) )
			{
				u32 color_t = n_bmp_blend_pixel( color_f, color_mix, blend );
				if ( color_f != color_t )
				{
					N_BMP_PTR( bmp )[ i ] = color_t;
				}
			}

			i++;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = 0;
	n_posix_loop
	{

		u32 color_f; n_bmp_ptr_get_fast( bmp, tx + x, ty + y, &color_f );

		if ( n_posix_false == n_bmp_is_trans( bmp, color_f ) )
		{
			u32 color_t = n_bmp_blend_pixel( color_f, color_mix, blend );
			if ( color_f != color_t )
			{
				n_bmp_ptr_set_fast( bmp, tx + x, ty + y, color_t );
			}
		}


		tx++;
		if ( tx >= sx )
		{

			tx = 0;

			ty++;
			if ( ty >= sy ) { break; }
		}
	}


	return;
}

#define n_bmp_flush_reducer( bmp, factor ) \
	n_bmp_reducer( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), factor )

// internal
n_posix_inline int
n_bmp_reducer_helper( int v, int v_min, int v_mid, int v_max, int factor )
{

	if ( v == v_max ) { return n_bmp_simplify_channel( v_max, factor * 1 ); }
	if ( v == v_min ) { return n_bmp_simplify_channel( v_min, factor * 4 ); }

	return v_mid;
}

void
n_bmp_reducer( n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, int factor )
{

	// [!] : nothing to do

	if ( factor == 0 ) { return; }


	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [!] : for backward compatibility

	factor = abs( factor );


	n_posix_bool is_flush = ( ( x == 0 )&&( y == 0 )&&( sx == N_BMP_SX( bmp ) )&&( sy == N_BMP_SY( bmp ) ) );
	if ( is_flush )
	{

		n_type_index c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
		n_type_index i = 0;
		n_posix_loop
		{

			u32 color = N_BMP_PTR( bmp )[ i ];

			int a = n_bmp_a( color );
			int r = n_bmp_r( color );
			int g = n_bmp_g( color );
			int b = n_bmp_b( color );

			if ( n_bmp_moire_detect( x,y, 1 ) )
			{

				int max = n_posix_max( r, n_posix_max( g, b ) );
				int min = n_posix_min( r, n_posix_min( g, b ) );
				int mid = ( r + g + b ) / 3;

				// [!] : alpha is reserved
				r = n_bmp_reducer_helper( r, min, mid, max, 2 * factor );
				g = n_bmp_reducer_helper( g, min, mid, max, 1 * factor );
				b = n_bmp_reducer_helper( b, min, mid, max, 4 * factor );

			} else {

				// [!] : alpha is reserved
				r = n_bmp_simplify_channel( r, factor );
				g = n_bmp_simplify_channel( g, factor );
				b = n_bmp_simplify_channel( b, factor );

			}

			N_BMP_PTR( bmp )[ i ] = n_bmp_argb( a,r,g,b );

			i++;
			if ( i >= c ) { break; }
		}


		return;
	}


	n_type_gfx tx = 0;
	n_type_gfx ty = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, tx + x, ty + y, &color );

		int a = n_bmp_a( color );
		int r = n_bmp_r( color );
		int g = n_bmp_g( color );
		int b = n_bmp_b( color );

		if ( n_bmp_moire_detect( x,y, 1 ) )
		{

			int max = n_posix_max( r, n_posix_max( g, b ) );
			int min = n_posix_min( r, n_posix_min( g, b ) );
			int mid = ( r + g + b ) / 3;

			// [!] : alpha is reserved
			r = n_bmp_reducer_helper( r, min, mid, max, 2 * factor );
			g = n_bmp_reducer_helper( g, min, mid, max, 1 * factor );
			b = n_bmp_reducer_helper( b, min, mid, max, 4 * factor );

		} else {

			// [!] : alpha is reserved
			r = n_bmp_simplify_channel( r, factor );
			g = n_bmp_simplify_channel( g, factor );
			b = n_bmp_simplify_channel( b, factor );

		}

		n_bmp_ptr_set_fast( bmp, tx + x, ty + y, n_bmp_argb( a,r,g,b ) );


		tx++;
		if ( tx >= sx )
		{

			tx = 0;

			ty++;
			if ( ty >= sy ) { break; }
		}
	}


	return;
}

#define n_bmp_rasterizer(      fr, to, tx, ty,        color, clear ) n_bmp_rasterizer_clip( fr, to, tx, ty,  NULL, color, NULL,0,0, clear )
#define n_bmp_rasterizer_main( fr, to, tx, ty, bmp_c, color, clear ) n_bmp_rasterizer_clip( fr, to, tx, ty, bmp_c, color, NULL,0,0, clear )

#define n_bmp_rasterizer_clip( fr, to, tx, ty, bmp_c,color, bmp_clip,cx,cy, clear ) n_bmp_rasterizer_all( fr, to, 0, 0, N_BMP_SX( fr ), N_BMP_SY( fr ), tx, ty, bmp_c,color, bmp_clip,cx,cy, clear )

typedef struct {

	n_bmp        *bmp_f;
	n_bmp        *bmp_t;
	n_type_gfx    fx,fy,fsx,fsy;
	n_type_gfx    tx,ty;
	n_bmp        *bmp_color;
	u32           color;
	n_bmp        *bmp_clip;
	n_type_gfx    cx;
	n_type_gfx    cy;
	n_posix_bool  clear_onoff;
	u32           oy, cores;

} n_bmp_rasterizer_thread_struct;

void
n_bmp_rasterizer_thread_main( n_bmp_rasterizer_thread_struct *p )
{

	// [!] : cache for performance : +5%

	n_type_real blend = 0;

	n_type_gfx sx = p->fsx;
	n_type_gfx sy = p->fsy;
	n_type_gfx  x = 0;
	n_type_gfx  y = p->oy; if ( y >= sy ) { return; }

	n_posix_bool clip_onoff = ( n_posix_false == n_bmp_error( p->bmp_clip ) );

	if ( p->clear_onoff )
	{

		n_type_real value = 0;

		//const u32    color_red     = n_bmp_rgb( 255,  0,  0 );
		const u32    color_blue    = n_bmp_rgb(   0,  0,255 );
		const u32    color_green   = n_bmp_rgb(   0,255,  0 );
		const u32    color_cyan    = n_bmp_rgb(   0,255,255 );
		const u32    color_magenta = n_bmp_rgb( 255,  0,255 );
		//const u32    color_yellow  = n_bmp_rgb( 255,255,  0 );

		const u32    color_default = 0;//0xffffffff;
		const u32    color_u       = color_green;
		const u32    color_d       = color_blue;
		const u32    color_l       = color_magenta;
		const u32    color_r       = color_cyan;
		const int    cutoff        = 128;

		const n_type_real ratio = 0.25;

		n_posix_loop
		{//break;

			int alpha = N_BMP_ALPHA_CHANNEL_VISIBLE;

			if ( clip_onoff )
			{
				u32 color_clip = n_bmp_black_invisible;
				n_bmp_ptr_get( p->bmp_clip, p->cx + x, p->cy + y, &color_clip );
				alpha = n_bmp_a( color_clip );
			}

			if (
				( clip_onoff == n_posix_false )
				||
				( N_BMP_ALPHA_CHANNEL_VISIBLE != alpha )
			)
			{

				u32 f,t;
				n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y, &f );

				if (
					( n_posix_false == n_bmp_is_trans( p->bmp_f, f ) )
					&&
					( n_posix_false == n_bmp_ptr_get( p->bmp_t, p->tx + x, p->ty + y, &t ) )
				)
				{

					int v = n_bmp_r( f );
					if ( v != value )
					{
						value = v;

						blend = n_bmp_table_coeff_channel( value );
						if ( clip_onoff ) { blend *= n_bmp_blend_alpha2ratio( alpha ); }
					} else {
						if ( N_BMP_ALPHA_CHANNEL_INVISIBLE != alpha )
						{
							if ( clip_onoff ) { blend = 0.0; }
						}
					}


					u32 tt;
					if ( p->bmp_color != NULL )
					{
						u32 c; n_bmp_ptr_get( p->bmp_color, p->fx + x, p->fy + y, &c );
						tt = n_bmp_blend_pixel( t,        c, blend );
					} else {
						tt = n_bmp_blend_pixel( t, p->color, blend );
					}


					// [!] : Engine

					u32 u = color_default; n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y - 1, &u );
					u32 d = color_default; n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y + 1, &d );

					n_posix_bool is_u = ( cutoff > (int) n_bmp_r( u ) );
					n_posix_bool is_d = ( cutoff > (int) n_bmp_r( d ) );

					if ( ( is_u )&&( is_d ) )
					{
						//
					} else
					if ( is_u )
					{
						tt = n_bmp_blend_pixel( tt, color_u, ratio );
					} else
					if ( is_d )
					{
						tt = n_bmp_blend_pixel( tt, color_d, ratio );
					}


					u32 l = color_default; n_bmp_ptr_get( p->bmp_f, p->fx + x - 1, p->fy + y, &l );
					u32 r = color_default; n_bmp_ptr_get( p->bmp_f, p->fx + x + 1, p->fy + y, &r );

					n_posix_bool is_l = ( cutoff > (int) n_bmp_r( l ) );
					n_posix_bool is_r = ( cutoff > (int) n_bmp_r( r ) );

					if ( ( is_l )&&( is_r ) )
					{
						//
					} else
					if ( is_l )
					{
						tt = n_bmp_blend_pixel( tt, color_l, ratio );
					} else
					if ( is_r )
					{
						tt = n_bmp_blend_pixel( tt, color_r, ratio );
					}


					if ( t != tt ) { n_bmp_ptr_set_fast( p->bmp_t, p->tx + x, p->ty + y,  tt ); }

				}

			}


			x++;
			if ( x >= sx )
			{

				x = 0;

				y += p->cores;
				if ( y >= sy ) { break; }
			}
		}

	} else
	if ( clip_onoff )
	{

		if ( p->bmp_f->transparent_onoff )
		{

			n_posix_loop
			{//break;

				int alpha = N_BMP_ALPHA_CHANNEL_VISIBLE;

				u32 color_clip = n_bmp_black_invisible;
				n_bmp_ptr_get( p->bmp_clip, p->cx + x, p->cy + y, &color_clip );
				alpha = n_bmp_a( color_clip );

				if ( N_BMP_ALPHA_CHANNEL_VISIBLE != alpha )
				{

					u32 f,t;
					n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y, &f );

					if (
						( f != n_bmp_trans )
						&&
						( n_posix_false == n_bmp_ptr_get( p->bmp_t, p->tx + x, p->ty + y, &t ) )
					)
					{

						blend = n_bmp_table_coeff_channel( n_bmp_r( f ) );

						blend *= n_bmp_blend_alpha2ratio( alpha );

						u32 tt;
						if ( p->bmp_color != NULL )
						{
							u32 c; n_bmp_ptr_get( p->bmp_color, p->fx + x, p->fy + y, &c );
							tt = n_bmp_blend_pixel( t,        c, blend );
						} else {
							tt = n_bmp_blend_pixel( t, p->color, blend );
						}

//if ( p->oy == 0 ) { tt = n_bmp_rgb( 0,200,255 ); }
//if ( p->oy == 1 ) { tt = n_bmp_rgb( 255,0,200 ); }

						if ( t != tt ) { n_bmp_ptr_set_fast( p->bmp_t, p->tx + x, p->ty + y,  tt ); }

					}

				}


				x++;
				if ( x >= sx )
				{

					x = 0;

					y += p->cores;
					if ( y >= sy ) { break; }
				}
			}

		} else {

			n_posix_loop
			{//break;


				int alpha = N_BMP_ALPHA_CHANNEL_VISIBLE;

				u32 color_clip = n_bmp_black_invisible;
				n_bmp_ptr_get( p->bmp_clip, p->cx + x, p->cy + y, &color_clip );
				alpha = n_bmp_a( color_clip );

				if ( N_BMP_ALPHA_CHANNEL_VISIBLE != alpha )
				{

					u32 f,t;
					n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y, &f );

					if ( n_posix_false == n_bmp_ptr_get( p->bmp_t, p->tx + x, p->ty + y, &t ) )
					{

						blend = n_bmp_table_coeff_channel( n_bmp_r( f ) );

						blend *= n_bmp_blend_alpha2ratio( alpha );

						u32 tt;
						if ( p->bmp_color != NULL )
						{
							u32 c; n_bmp_ptr_get( p->bmp_color, p->fx + x, p->fy + y, &c );
							tt = n_bmp_blend_pixel( t,        c, blend );
						} else {
							tt = n_bmp_blend_pixel( t, p->color, blend );
						}

//if ( p->oy == 0 ) { tt = n_bmp_rgb( 0,200,255 ); }
//if ( p->oy == 1 ) { tt = n_bmp_rgb( 255,0,200 ); }

						if ( t != tt ) { n_bmp_ptr_set_fast( p->bmp_t, p->tx + x, p->ty + y,  tt ); }

					}

				}


				x++;
				if ( x >= sx )
				{

					x = 0;

					y += p->cores;
					if ( y >= sy ) { break; }
				}
			}

		}

	} else {

		if ( p->bmp_f->transparent_onoff )
		{

			n_posix_loop
			{//break;

				u32 f,t;
				n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y, &f );

				if (
					( f != n_bmp_trans )
					&&
					( n_posix_false == n_bmp_ptr_get( p->bmp_t, p->tx + x, p->ty + y, &t ) )
				)
				{

					blend = n_bmp_table_coeff_channel( n_bmp_r( f ) );

					u32 tt;
					if ( p->bmp_color != NULL )
					{
						u32 c; n_bmp_ptr_get( p->bmp_color, p->fx + x, p->fy + y, &c );
						tt = n_bmp_blend_pixel( t,        c, blend );
					} else {
						tt = n_bmp_blend_pixel( t, p->color, blend );
					}

//if ( p->oy == 0 ) { tt = n_bmp_rgb( 0,200,255 ); }
//if ( p->oy == 1 ) { tt = n_bmp_rgb( 255,0,200 ); }

					if ( t != tt ) { n_bmp_ptr_set_fast( p->bmp_t, p->tx + x, p->ty + y,  tt ); }

				}


				x++;
				if ( x >= sx )
				{

					x = 0;

					y += p->cores;
					if ( y >= sy ) { break; }
				}
			}

		} else {

			n_posix_loop
			{//break;

				u32 f,t;
				n_bmp_ptr_get( p->bmp_f, p->fx + x, p->fy + y, &f );

				if ( n_posix_false == n_bmp_ptr_get( p->bmp_t, p->tx + x, p->ty + y, &t ) )
				{

					blend = n_bmp_table_coeff_channel( n_bmp_r( f ) );

					u32 tt;
					if ( p->bmp_color != NULL )
					{
						u32 c; n_bmp_ptr_get( p->bmp_color, p->fx + x, p->fy + y, &c );
						tt = n_bmp_blend_pixel( t,        c, blend );
					} else {
						tt = n_bmp_blend_pixel( t, p->color, blend );
					}

//if ( p->oy == 0 ) { tt = n_bmp_rgb( 0,200,255 ); }
//if ( p->oy == 1 ) { tt = n_bmp_rgb( 255,0,200 ); }

					if ( t != tt ) { n_bmp_ptr_set_fast( p->bmp_t, p->tx + x, p->ty + y,  tt ); }

				}

				x++;
				if ( x >= sx )
				{

					x = 0;

					y += p->cores;
					if ( y >= sy ) { break; }
				}
			}

		}

	}

	return;
}

n_thread_return
n_bmp_rasterizer_thread( n_thread_argument p )
{

	n_bmp_rasterizer_thread_main( (n_bmp_rasterizer_thread_struct*) p );

	return 0;
}

void
n_bmp_rasterizer_all
(
	n_bmp *fr,
	n_bmp *to,
	n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy,
	n_type_gfx tx, n_type_gfx ty,
	n_bmp *bmp_color, u32 color,
	n_bmp *bmp_clip, n_type_gfx cx, n_type_gfx cy,
	n_posix_bool clear_onoff
)
{

	// [Mechanism]
	//
	//	fr          : gray-scaled + antialiased bitmap
	//	to          : canvas
	//	color       : color to mix
	//	clear_onoff : use ClearType-like technique
	//
	//	alpha value is not supported for performance


	if ( n_bmp_error( fr ) ) { return; }
	if ( n_bmp_error( to ) ) { return; }

	if ( n_bmp_is_trans( fr, color ) ) { return; }
	if ( n_bmp_is_trans( to, color ) ) { return; }


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		(
			( ( N_BMP_SX( fr ) * N_BMP_SY( fr ) ) >= N_BMP_MULTITHREAD_GRANULARITY )
			||
			( ( N_BMP_SX( to ) * N_BMP_SY( to ) ) >= N_BMP_MULTITHREAD_GRANULARITY )
		)
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_rasterizer_clip() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                       *h = (n_thread                      *) n_memory_new( cores * sizeof( n_thread                       ) );
		n_bmp_rasterizer_thread_struct *p = (n_bmp_rasterizer_thread_struct*) n_memory_new( cores * sizeof( n_bmp_rasterizer_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_rasterizer_thread_struct tmp = { fr,to, fx,fy,fsx,fsy, tx,ty, bmp_color, color, bmp_clip,cx,cy, clear_onoff, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_rasterizer_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_rasterizer_thread, &p[ i ] );

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

		n_bmp_rasterizer_thread_struct p = { fr,to, fx,fy,fsx,fsy, tx,ty, bmp_color, color, bmp_clip,cx,cy, clear_onoff, 0,1 };

		n_bmp_rasterizer_thread_main( &p );

	}


	return;
}

#define n_bmp_cornermask(      b, r, f, bg             ) n_bmp_cornermask_internal( b, 0, 0, N_BMP_SX( b ), N_BMP_SY( b ), r, f, bg, bg, bg, bg )
#define n_bmp_cornermask_rich( b, r, f, ul, ur, dl, dr ) n_bmp_cornermask_internal( b, 0, 0, N_BMP_SX( b ), N_BMP_SY( b ), r, f, ul, ur, dl, dr )

#define n_bmp_rasterizer_cornermask( fr, to, fx, fy, fsx, fsy, tx, ty, color )\
	n_bmp_rasterizer_all(        fr, to, fx, fy, fsx, fsy, tx, ty, NULL, color, NULL,0,0, n_posix_false )

// internal
void
n_bmp_cornermask_internal
(
	n_bmp      *bmp,
	n_type_gfx  x,
	n_type_gfx  y,
	n_type_gfx  sx,
	n_type_gfx  sy,
	n_type_gfx  round_size,
	n_type_gfx  frame_size,
	u32         bg_ul,
	u32         bg_ur,
	u32         bg_dl,
	u32         bg_dr
)
{
//return;

	if ( n_bmp_error( bmp ) ) { return; }

	if ( round_size == 0 ) { return; }


	extern n_posix_bool n_bmp_save( const n_bmp *bmp, const n_posix_char *filename );


//bg_ul = bg_ur = bg_dl = bg_dr = n_bmp_rgb( 255,0,0 );


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy ); n_bmp_flush( &b, n_bmp_white );

	n_type_gfx tx  = frame_size;
	n_type_gfx ty  = frame_size;
	n_type_gfx tsx = sx - ( frame_size * 2 );
	n_type_gfx tsy = sy - ( frame_size * 2 );

	n_bmp_roundrect( &b, tx,ty,tsx,tsy, n_bmp_black, round_size );
//n_bmp_save( &b, L"ret.bmp" );


	if ( frame_size != 0 )
	{

		// [!] : slow mode only

		n_bmp_rasterizer( &b, bmp, x,y, bg_ul, n_posix_false );

	} else {

		round_size = n_bmp_roundrect_param( sx, sy, round_size );

		n_type_gfx sz = round_size;
		n_type_gfx ff = sz;
		n_type_gfx tt = sz;
//n_posix_debug_literal( "%d", sz );

		n_bmp fragment; n_bmp_zero( &fragment ); n_bmp_1st_fast( &fragment, sz,sz );

		n_bmp_fastcopy( &b, &fragment, 0,0,sz,sz, 0,0 );
//n_bmp_save( &fragment, L"ul.bmp" );
//n_bmp_flush( &fragment, n_bmp_white ); bg_ul = n_bmp_rgb( 255,0,0 );
		n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,ff, x      ,      y, bg_ul );

		n_bmp_fastcopy( &b, &fragment, sx-sz,0,sz,sz, 0,0 );
//n_bmp_save( &fragment, L"ur.bmp" );
//n_bmp_flush( &fragment, n_bmp_white ); bg_ur = n_bmp_rgb( 255,255,0 );
		n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,ff, x+sx-sz,      y, bg_ur );

		n_bmp_fastcopy( &b, &fragment, 0,sy-sz,sz,sz, 0,0 );
//n_bmp_save( &fragment, L"dl.bmp" );
//n_bmp_flush( &fragment, n_bmp_white ); bg_dl = n_bmp_rgb( 0,255,0 );
		n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,tt, x      ,y+sy-sz, bg_dl );

		n_bmp_fastcopy( &b, &fragment, sx-sz,sy-sz,sz,sz, 0,0 );
//n_bmp_save( &fragment, L"dr.bmp" );
//n_bmp_flush( &fragment, n_bmp_white ); bg_dr = n_bmp_rgb( 0,255,255 );
		n_bmp_rasterizer_cornermask( &fragment, bmp, 0,0,sz,tt, x+sx-sz,y+sy-sz, bg_dr );

		n_bmp_free( &fragment );

	}


	n_bmp_free( &b );


	return;
}

#define n_bmp_flush_balance( bmp ) n_bmp_flush_balance_internal( bmp, 3, n_bmp_white )

void
n_bmp_flush_balance_internal( n_bmp *bmp, n_type_real power, u32 color_target )
{

	// [x] : parameters are flaky, use macro n_bmp_flush_balance()


	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	// Phase 1 : make curve by nonliear function

	n_type_real maxv = pow( 255, power );
	n_type_real ramp[ 256 ];

	int i = 0;
	n_posix_loop
	{

		ramp[ i ] = pow( i, power ) / maxv;

		i++;
		if ( i >= 256 ) { break; }
	}


	// Phase 2 : apply

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x, y, &color );

		if ( N_BMP_ALPHA_CHANNEL_VISIBLE == n_bmp_a( color ) )
		{
			u32 gray = n_bmp_r( n_bmp_grayscale_pixel( color ) );
			//u32 gray = n_bmp_l( n_bmp_argb2ahsl( color ) );

			n_type_real ratio = ramp[ gray ];

			color = n_bmp_blend_pixel( color, color_target, ratio );
			n_bmp_ptr_set_fast( bmp, x, y, color );
		}


		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	return;
}


#endif // _H_NONNON_NEUTRAL_BMP_FILTER

