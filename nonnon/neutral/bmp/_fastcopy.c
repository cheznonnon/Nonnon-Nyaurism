// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Copilot : Optimization : Done




#ifndef _H_NONNON_NEUTRAL_BMP_FASTCOPY
#define _H_NONNON_NEUTRAL_BMP_FASTCOPY




#include "../bmp.c"


#include "./_error.c"




//#define n_bmp_flush_fastcopy( fr,to ) \
//	n_bmp_fastcopy( fr,to, 0,0,N_BMP_SX( fr ),N_BMP_SY( fr ), 0,0 )

#define n_bmp_flush_fastcopy( fr,to ) memcpy( N_BMP_PTR( to ), N_BMP_PTR( fr ), N_BMP_SIZE( fr ) )

typedef struct {

	const n_bmp        *bmp_f;
	      n_bmp        *bmp_t;
	      n_type_index  fx,fy,fsx,fsy, tx,ty;
	      u32           oy, cores;

} n_bmp_fastcopy_thread_struct;
/*
void
n_bmp_fastcopy_thread_main( n_bmp_fastcopy_thread_struct *p )
{

	if ( n_bmp_flip_onoff )
	{

		n_type_index fy = N_BMP_SX( p->bmp_f ) * ( p->fy - p->oy );
		n_type_index ty = N_BMP_SX( p->bmp_t ) * ( p->ty - p->oy );

		n_type_index offset_fy = N_BMP_SX( p->bmp_f ) * p->cores;
		n_type_index offset_ty = N_BMP_SX( p->bmp_t ) * p->cores;

		if ( p->bmp_f == p->bmp_t )
		{

			n_type_index y = p->oy; if ( y >= p->fsy ) { return; }
			n_posix_loop
			{

				memmove( &p->bmp_t->ptr[ p->tx + ty ], &p->bmp_f->ptr[ p->fx + fy ], p->fsx );

				fy += offset_fy;
				ty += offset_ty;

				y += p->cores;
				if ( y >= p->fsy ) { break; }
			}

		} else {

			n_type_index y = p->oy; if ( y >= p->fsy ) { return; }
			n_posix_loop
			{

				n_memcpy_fast( &p->bmp_t->ptr[ p->tx + ty ], &p->bmp_f->ptr[ p->fx + fy ], p->fsx );

				fy += offset_fy;
				ty += offset_ty;

				y += p->cores;
				if ( y >= p->fsy ) { break; }
			}

		}

	} else {

		n_type_index fy = N_BMP_SX( p->bmp_f ) * ( N_BMP_SY( p->bmp_f ) - 1 - p->fy - p->oy );
		n_type_index ty = N_BMP_SX( p->bmp_t ) * ( N_BMP_SY( p->bmp_t ) - 1 - p->ty - p->oy );

		n_type_index offset_fy = N_BMP_SX( p->bmp_f ) * p->cores;
		n_type_index offset_ty = N_BMP_SX( p->bmp_t ) * p->cores;

		if ( p->bmp_f == p->bmp_t )
		{

			n_type_index y = p->oy; if ( y >= p->fsy ) { return; }
			n_posix_loop
			{

				memmove( &p->bmp_t->ptr[ p->tx + ty ], &p->bmp_f->ptr[ p->fx + fy ], p->fsx );

				fy -= offset_fy;
				ty -= offset_ty;

				y += p->cores;
				if ( y >= p->fsy ) { break; }
			}

		} else {

			n_type_index y = p->oy; if ( y >= p->fsy ) { return; }
			n_posix_loop
			{

				n_memcpy_fast( &p->bmp_t->ptr[ p->tx + ty ], &p->bmp_f->ptr[ p->fx + fy ], p->fsx );

				fy -= offset_fy;
				ty -= offset_ty;

				y += p->cores;
				if ( y >= p->fsy ) { break; }
			}

		}

	}


	return;
}
*/
void
n_bmp_fastcopy_thread_main( n_bmp_fastcopy_thread_struct *p )
{

	// Localize frequently used values
	const n_bmp *bmp_f = p->bmp_f;
	n_bmp       *bmp_t = p->bmp_t;
	u32         *ptr_f = bmp_f->ptr;
	u32         *ptr_t = bmp_t->ptr;

	n_type_index fx  = p->fx;
	n_type_index fy0 = p->fy;
	n_type_index fsx = p->fsx; // already in bytes (set by caller)
	n_type_index fsy = p->fsy;
	n_type_index tx  = p->tx;
	n_type_index ty0 = p->ty;

	n_type_index cores = p->cores;
	n_type_index oy = p->oy;

	// compute starting line offsets (in pixels) and step per iteration
	n_type_index line_stride_f = N_BMP_SX( bmp_f );
	n_type_index line_stride_t = N_BMP_SX( bmp_t );

	n_type_index cur_fy, cur_ty;
	n_type_index step_fy = line_stride_f * cores;
	n_type_index step_ty = line_stride_t * cores;

	if ( n_bmp_flip_onoff )
	{
		cur_fy = line_stride_f * ( fy0 - oy );
		cur_ty = line_stride_t * ( ty0 - oy );
	}
	else
	{
		cur_fy = line_stride_f * ( N_BMP_SY( bmp_f ) - 1 - fy0 - oy );
		cur_ty = line_stride_t * ( N_BMP_SY( bmp_t ) - 1 - ty0 - oy );
		step_fy = -step_fy;
		step_ty = -step_ty;
	}

	// choose copy function: memmove when same buffer, memcpy otherwise
	void (*copy_fn)( void *, const void *, size_t );
	if ( bmp_f == bmp_t ) copy_fn = (void(*)(void*,const void*,size_t)) memmove;
	else                   copy_fn = (void(*)(void*,const void*,size_t)) n_memcpy_fast;

	// bounds check
	n_type_index y = oy;
	if ( y >= fsy ) return;

	size_t byte_count = (size_t) fsx;

	n_posix_loop
	{
		void *dst = &ptr_t[ tx + cur_ty ];
		const void *src = &ptr_f[ fx + cur_fy ];

		copy_fn( dst, src, byte_count );

		cur_fy += step_fy;
		cur_ty += step_ty;

		y += cores;
		if ( y >= fsy ) { break; }
	}

	return;
}

n_thread_return
n_bmp_fastcopy_thread( n_thread_argument p )
{

	n_bmp_fastcopy_thread_main( (n_bmp_fastcopy_thread_struct*) p );

	return 0;
}

void
n_bmp_fastcopy
(
	const n_bmp *f, n_bmp *t,
	n_type_gfx fx, n_type_gfx fy, n_type_gfx fsx, n_type_gfx fsy,
	n_type_gfx tx, n_type_gfx ty
)
{

	// [!] : is this really fast? => satisfaction guaranteed(?)


	if ( n_bmp_error_clipping( f,t, &fx,&fy,&fsx,&fsy, &tx,&ty ) ) { return; }


	//if ( f == t ) { return; }


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( fsx * fsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_fastcopy() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		// [!] : precalc

		fsx = fsx * sizeof( u32 );


		n_posix_bool p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = n_posix_true;


		u32 cores = n_thread_core_count;

		n_thread                     *h = (n_thread                    *) n_memory_new( cores * sizeof( n_thread                     ) );
		n_bmp_fastcopy_thread_struct *p = (n_bmp_fastcopy_thread_struct*) n_memory_new( cores * sizeof( n_bmp_fastcopy_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_fastcopy_thread_struct tmp = { f,t, fx,fy,fsx,fsy, tx,ty, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_fastcopy_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_fastcopy_thread, &p[ i ] );

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

		// [!] : precalc

		fsx = fsx * sizeof( u32 );

		n_bmp_fastcopy_thread_struct p = { f,t, fx,fy,fsx,fsy, tx,ty, 0,1 };

		n_bmp_fastcopy_thread_main( &p );

	}


	return;
}


#endif //_H_NONNON_NEUTRAL_BMP_FASTCOPY

