// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_TRANSFORM
#define _H_NONNON_NEUTRAL_BMP_TRANSFORM




#include "../bmp.c"


#include "./_error.c"
#include "./_fastcopy.c"
#include "./color.c"
#include "./draw.c"




#define N_BMP_MIRROR_NONE            ( 0      )
#define N_BMP_MIRROR_LEFTSIDE_RIGHT  ( 1 << 0 )
#define N_BMP_MIRROR_UPSIDE_DOWN     ( 1 << 1 )
#define N_BMP_MIRROR_ROTATE180       ( N_BMP_MIRROR_LEFTSIDE_RIGHT | N_BMP_MIRROR_UPSIDE_DOWN )

#define n_bmp_flush_mirror( bmp, mirror ) \
	n_bmp_mirror( bmp, 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ), mirror )

void
n_bmp_mirror
(
	n_bmp *bmp,
	n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy,
	int mirror
)
{

	if ( n_bmp_error_clipping( bmp,NULL, &x,&y,&sx,&sy, NULL,NULL ) ) { return; }


	// [!] : nothing to do

	if ( mirror == N_BMP_MIRROR_NONE ) { return; }


	n_type_gfx startx = x;
	n_type_gfx starty = y;


	n_type_gfx mx,my;
	u32        f,t;


	if ( mirror & N_BMP_MIRROR_LEFTSIDE_RIGHT )
	{

		mx = x + sx - 1;
		my = y;

		n_posix_loop
		{

			n_bmp_ptr_get_fast( bmp,  x, y, &f );
			n_bmp_ptr_get_fast( bmp, mx,my, &t );

			if ( f != t )
			{
				n_bmp_ptr_set_fast( bmp,  x, y, t );
				n_bmp_ptr_set_fast( bmp, mx,my, f );
			}


			x++; mx--;
			if ( x >= mx )
			{

				x  = startx;
				mx = x + sx - 1;


				y++; my++;
				if ( y >= ( starty + sy ) ) { break; }
			}
		}

	}

	if ( mirror & N_BMP_MIRROR_UPSIDE_DOWN )
	{

		x  = startx;
		mx = x;
		y  = starty;
		my = y + sy - 1;

		n_posix_loop
		{

			n_bmp_ptr_get_fast( bmp,  x, y, &f );
			n_bmp_ptr_get_fast( bmp, mx,my, &t );

			if ( f != t )
			{
				n_bmp_ptr_set_fast( bmp,  x, y, t );
				n_bmp_ptr_set_fast( bmp, mx,my, f );
			}


			x++; mx++;
			if ( x >= ( startx + sx ) )
			{

				x = mx = startx;

				y++; my--;
				if ( y >= my ) { break; }
			}
		}

	}


	return;
}

#define N_BMP_ROTATE_NONE  0
#define N_BMP_ROTATE_LEFT  1
#define N_BMP_ROTATE_RIGHT 2

void
n_bmp_rotate( n_bmp *bmp, int option )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : nothing to do

	if ( option == N_BMP_ROTATE_NONE ) { return; }


	n_type_gfx sx = N_BMP_SX( bmp );
	n_type_gfx sy = N_BMP_SY( bmp );


	// [!] : swap sx and sy

	n_bmp to;
	n_bmp_zero( &to );
	n_bmp_1st_fast( &to, sy,sx );


	n_type_gfx x,y, xx,yy;
	u32        color;


	if ( option == N_BMP_ROTATE_LEFT )
	{
		xx = 0;
		yy = N_BMP_SY( &to ) - 1;
	} else {
		xx = N_BMP_SX( &to ) - 1;
		yy = 0;
	}

	x = y = 0;
	n_posix_loop
	{

		n_bmp_ptr_get_fast(  bmp,  x, y, &color );
		n_bmp_ptr_set_fast( &to,  xx,yy,  color );


		if ( option == N_BMP_ROTATE_LEFT )
		{
			yy--;
		} else {
			yy++;
		}


		x++;
		if ( x >= sx )
		{

			x = 0;


			if ( option == N_BMP_ROTATE_LEFT )
			{
				yy = sx - 1;
				xx++;
			} else {
				yy = 0;
				xx--;
			}


			y++;
			if ( y >= sy ) { break; }
		}
	}


	n_bmp_replace( &to, bmp );


	return;
}

#define N_BMP_SCALER_LIL 0
#define N_BMP_SCALER_BIG 1

#define n_bmp_scaler_lil( b, r ) n_bmp_scaler( b, N_BMP_SCALER_LIL, r )
#define n_bmp_scaler_big( b, r ) n_bmp_scaler( b, N_BMP_SCALER_BIG, r )

void
n_bmp_scaler( n_bmp *bmp, int mode, int ratio )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : nothing to do

	if ( ratio <= 1 ) { return; }


	n_bmp      to;
	n_type_gfx fx,fy,fsx,fsy, tx,ty,tsx,tsy;
	u32        color;


	n_bmp_zero( &to );

	fx = fy = tx = ty = 0;

	fsx = N_BMP_SX( bmp );
	fsy = N_BMP_SY( bmp );


	if ( mode == N_BMP_SCALER_LIL )
	{

		// Shrink

		if ( ( fsx <= 1 )||( fsy <= 1 ) )
		{
			return;
		}

		tsx = fsx / ratio;
		tsy = fsy / ratio;

		n_bmp_1st_fast( &to, tsx,tsy );


		n_posix_loop
		{

			n_bmp_ptr_get_fast(  bmp, fx,fy, &color );
			n_bmp_ptr_set_fast( &to,  tx,ty,  color );

			fx += ratio;

			tx++;
			if ( tx >= tsx )
			{

				fx = tx = 0;


				fy += ratio;

				ty++;
				if ( ty >= tsy ) { break; }
			}

		}

	} else

	if ( mode == N_BMP_SCALER_BIG )
	{

		// Enlarge


		// [!] : <limits.h> : LONG_MAX

		const n_type_gfx maxsize = 0x7fffffff / ratio;


		n_type_gfx i, curline;


		if (
			( fsx > maxsize )
			||
			( fsy > maxsize )
			||
			( n_bmp_is_overflow( fsx, fsy ) )
		)
		{
			return;
		}

		tsx = fsx * ratio;
		tsy = fsy * ratio;

		if ( n_bmp_is_overflow( tsx, tsy ) ) { return; }

		n_bmp_1st_fast( &to, tsx,tsy );


		n_posix_loop
		{

			n_bmp_ptr_get_fast( bmp, fx,fy, &color );

			i = 0;
			n_posix_loop
			{

				n_bmp_ptr_set_fast( &to, tx,ty, color );

				i++; tx++;
				if ( i >= ratio ) { break; }
			}


			fx++;
			if ( fx >= fsx ) 
			{

				fx = tx = 0;


				curline = tsx * ( tsy - 1 - ty );

				i = 0;
				n_posix_loop
				{

					i++; ty++;
					if ( i >= ratio ) { break; }

					n_memcpy_fast
					(
						&N_BMP_PTR( &to )[ curline - ( tsx * i ) ], 
						&N_BMP_PTR( &to )[ curline               ], 
						tsx * sizeof( u32 )
					);

				}


				fy++;
				if ( fy >= fsy ) { break; }
			}

		}

	}


	n_bmp_replace( &to, bmp );


	return;
}




typedef struct {

	n_bmp       *bmp_f, *bmp_t;
	n_type_gfx   tsx,tsy;
	n_type_real  ratio_x, ratio_y;
	u32          oy, cores;

} n_bmp_resampler_thread_struct;

void
n_bmp_resampler_thread_main( n_bmp_resampler_thread_struct *p )
{

	// [!] : Thx : DeepSeek AI

	for ( int dy = 0; dy < p->tsy; dy++ )
	{
		for ( int dx = 0; dx < p->tsx; dx++ )
		{
			n_type_real src_x = ( dx + 0.5 ) * p->ratio_x - 0.5;
			n_type_real src_y = ( dy + 0.5 ) * p->ratio_y - 0.5;

			u32 color = n_bmp_bicubic_pixel( p->bmp_f, src_x, src_y );

			n_bmp_ptr_set( p->bmp_t, dx,dy, color );
		}
	}


	return;
}

n_thread_return
n_bmp_resampler_thread( n_thread_argument p )
{

	n_bmp_resampler_thread_main( (n_bmp_resampler_thread_struct*) p );

	return 0;
}

void
n_bmp_resampler( n_bmp *bmp, n_type_real ratio_x, n_type_real ratio_y )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : nothing to do

	if ( ( ratio_x == 1.0 )&&( ratio_y == 1.0 ) ) { return; }


	// [!] : <limits.h> : LONG_MAX

	const n_type_real maxsize = (n_type_real) 0x7fffffff;


	n_type_gfx fsx = N_BMP_SX( bmp );
	n_type_gfx fsy = N_BMP_SY( bmp );

	n_type_real dsx = (n_type_real) fsx * ratio_x;
	n_type_real dsy = (n_type_real) fsy * ratio_y;

	if ( maxsize < trunc( dsx ) ) { return; }
	if ( maxsize < trunc( dsy ) ) { return; }


	n_type_gfx tsx = (n_type_gfx) round( dsx );
	n_type_gfx tsy = (n_type_gfx) round( dsy );

	if ( tsx <= 0 ) { return; }
	if ( tsy <= 0 ) { return; }

	if ( n_bmp_is_overflow( tsx, tsy ) ) { return; }


	n_bmp to; n_bmp_zero( &to ); n_bmp_1st_fast( &to, tsx,tsy );


	ratio_x = 1.0 / ratio_x;
	ratio_y = 1.0 / ratio_y;


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		(
			( ( fsx * fsy ) >= 10000 )//N_BMP_MULTITHREAD_GRANULARITY )
			||
			( ( tsx * tsy ) >= 10000 )//N_BMP_MULTITHREAD_GRANULARITY )
		)
	)
	{

//NSLog( @" n_bmp_resampler() multi-thread " );

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_resampler() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;


		u32 cores = n_thread_core_count;

		n_thread                      *h = (n_thread                     *) n_memory_new( cores * sizeof( n_thread                      ) );
		n_bmp_resampler_thread_struct *p = (n_bmp_resampler_thread_struct*) n_memory_new( cores * sizeof( n_bmp_resampler_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_resampler_thread_struct tmp = { bmp, &to, tsx,tsy, ratio_x,ratio_y, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_resampler_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_resampler_thread, &p[ i ] );

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

		n_bmp_resampler_thread_struct p = { bmp, &to, tsx,tsy, ratio_x,ratio_y, 0,1 };

		n_bmp_resampler_thread_main( &p );

	}


	n_bmp_replace( &to, bmp );


	return;
}

#define N_BMP_RESIZER_NORMAL   0
#define N_BMP_RESIZER_TILE     1
#define N_BMP_RESIZER_CENTER   2

void
n_bmp_resizer( n_bmp *bmp, n_type_gfx sx, n_type_gfx sy, u32 color_margin, int mode )
{

	if ( n_bmp_error( bmp ) ) { return; }


	if ( n_bmp_is_overflow( sx, sy ) ) { return; }


	const n_type_gfx bmpsx = N_BMP_SX( bmp );
	const n_type_gfx bmpsy = N_BMP_SY( bmp );


	// [!] : nothing to do

	if ( ( sx <= 0 )||( sy <= 0 ) ) { return; }
	if ( ( sx == bmpsx )&&( sy == bmpsy ) ) { return; }


	n_bmp to; n_bmp_zero( &to ); n_bmp_1st_fast( &to, sx,sy );


	if ( mode == N_BMP_RESIZER_TILE )
	{

		n_type_gfx tx = sx % bmpsx;
		n_type_gfx ty = sy % bmpsy;

		if ( tx != 0 ) { tx = -( bmpsx - ( tx / 2 ) ); }
		if ( ty != 0 ) { ty = -( bmpsy - ( ty / 2 ) ); }

		n_type_gfx startx = tx;
		n_posix_loop
		{

			n_bmp_fastcopy( bmp,&to, 0,0,bmpsx,bmpsy, tx,ty );

			tx += bmpsx;
			if ( tx >= sx )
			{

				tx = startx;

				ty += bmpsy;
				if ( ty >= sy ) { break; }
			}

		}

	} else {

		n_type_gfx tx = 0;
		n_type_gfx ty = 0;

		if ( mode == N_BMP_RESIZER_CENTER )
		{
			tx = ( sx / 2 ) - ( bmpsx / 2 );
			ty = ( sy / 2 ) - ( bmpsy / 2 );
		}

		n_bmp_flush( &to, color_margin );

/*
		// [x] : not faster than n_bmp_flush()

		n_type_gfx margin_sx = sx - bmpsx;
		n_type_gfx margin_sy = sy - bmpsy;

		n_bmp_box( &to, 0,            0, sx, margin_sy, color_margin );
		n_bmp_box( &to, 0, sy-margin_sy, sx, margin_sy, color_margin );
		n_bmp_box( &to, 0,            0, margin_sx, sy, color_margin );
		n_bmp_box( &to, sx-margin_sx, 0, margin_sx, sy, color_margin );
*/

		n_bmp_fastcopy( bmp, &to, 0,0,bmpsx,bmpsy, tx,ty );

	}


	n_bmp_replace( &to, bmp );


	return;
}

typedef struct {

	n_bmp      *bmp_f, *bmp_t;
	n_type_gfx  fsx,fsy,tsx,tsy;
	int         n;
	u32         oy,cores;

} n_bmp_smoothshrink_thread_struct;

void
n_bmp_smoothshrink_thread_main( n_bmp_smoothshrink_thread_struct *p )
{

	n_type_gfx ofy = p->oy * p->n;
	n_type_gfx oty = p->oy;

	if ( ofy >= p->fsy ) { return; }
	if ( oty >= p->tsy ) { return; }

	n_type_gfx fx = 0;
	n_type_gfx fy = ofy;
	n_type_gfx tx = 0;
	n_type_gfx ty = oty;
	n_posix_loop
	{//break;

		const n_type_gfx step = 1;

		int count = 0;
		int a,r,g,b; a = r = g = b = 0;

		n_type_gfx  x = n_posix_max_n_type_gfx(      0, fx        );
		n_type_gfx  y = n_posix_max_n_type_gfx(      0, fy        );
		n_type_gfx sx = n_posix_min_n_type_gfx( p->fsx, fx + p->n );
		n_type_gfx sy = n_posix_min_n_type_gfx( p->fsy, fy + p->n );
		n_posix_loop
		{//break;

			u32 color; n_bmp_ptr_get_fast( p->bmp_f, x,y, &color );


			count++;

			a += n_bmp_a( color );
			r += n_bmp_r( color );
			g += n_bmp_g( color );
			b += n_bmp_b( color );


			x += step;
			if ( x >= sx )
			{

				x = fx;

				y += step;
				if ( y >= sy ) { break; }
			}
		}

		n_bmp_ptr_set( p->bmp_t, tx,ty, n_bmp_div_pixel( a,r,g,b, count ) );


		tx++; fx += p->n;
		if ( tx >= p->tsx )
		{

			tx = fx = 0;

			fy += p->cores * p->n;
			if ( fy >= p->fsy ) { break; }

			ty += p->cores;
			if ( ty >= p->tsy ) { break; }
		}
	}


	return;
}

n_thread_return
n_bmp_smoothshrink_thread( n_thread_argument p )
{

	n_bmp_smoothshrink_thread_main( (n_bmp_smoothshrink_thread_struct*) p );

	return 0;
}

void
n_bmp_smoothshrink( n_bmp *bmp, int n )
{

	if ( n_bmp_error( bmp ) ) { return; }


	if ( n <= 1 ) { return; }


	// [!] : clamp is needed when n is larger than width or height

	n_type_gfx fsx = N_BMP_SX( bmp );
	n_type_gfx fsy = N_BMP_SY( bmp );
	n_type_gfx tsx = n_posix_max_n_type_gfx( 1, (n_type_gfx) round( (n_type_real) fsx / n ) );
	n_type_gfx tsy = n_posix_max_n_type_gfx( 1, (n_type_gfx) round( (n_type_real) fsy / n ) );


	n_bmp to; n_bmp_zero( &to ); n_bmp_1st_fast( &to, tsx,tsy );
//n_bmp_flush( &to, 0x00ffff00 );


	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( fsx * fsy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_bmp_smoothshrink() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;


		u32 cores = n_thread_core_count;

		n_thread                         *h = (n_thread                        *) n_memory_new( cores * sizeof( n_thread                         ) );
		n_bmp_smoothshrink_thread_struct *p = (n_bmp_smoothshrink_thread_struct*) n_memory_new( cores * sizeof( n_bmp_smoothshrink_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_bmp_smoothshrink_thread_struct tmp = { bmp, &to, fsx,fsy, tsx,tsy, n, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_bmp_smoothshrink_thread_struct ) );

			h[ i ] = n_thread_init( n_bmp_smoothshrink_thread, &p[ i ] );

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

		n_bmp_smoothshrink_thread_struct p = { bmp, &to, fsx,fsy, tsx,tsy, n, 0,1 };

		n_bmp_smoothshrink_thread_main( &p );

	}


	n_bmp_replace( &to, bmp );


	return;
}

BOOL
n_bmp_scaler_big_pixelart_edge_detect( u32 color_0, u32 color_1, u32 color_2 )
{

	// [!] : 4 edges
	return ( ( color_1 == color_2 )&&( color_1 != color_0 ) );

/*
	// [x] : hard to tune : infinite loop easily

	int threshold_h = 16;
	int threshold_l = 16;

	u32 ahsl_0 = n_bmp_argb2ahsl( color_0 );
	u32 ahsl_1 = n_bmp_argb2ahsl( color_1 );
	u32 ahsl_2 = n_bmp_argb2ahsl( color_2 );

	int h_0 = n_bmp_h( ahsl_0 );
	int h_1 = n_bmp_h( ahsl_1 );
	int h_2 = n_bmp_h( ahsl_2 );

	int l_0 = n_bmp_l( ahsl_0 );
	int l_1 = n_bmp_l( ahsl_1 );
	int l_2 = n_bmp_l( ahsl_2 );


	return
	(
		( ( threshold_h < abs( h_0 - h_1 ) )||( threshold_h < abs( h_0 - h_2 ) ) )
		&&
		( ( threshold_l < abs( l_0 - l_1 ) )||( threshold_l < abs( l_0 - l_2 ) ) )
	);
*/
}

// internal
BOOL
n_bmp_scaler_big_pixelart_pixel( n_bmp *bmp_f, n_bmp *bmp_t, n_type_gfx x, n_type_gfx y, n_type_gfx dx, n_type_gfx dy, n_type_gfx blur, u32 color_target, u32 color_center )
{

	if ( n_bmp_is_trans( bmp_f, color_target ) ) { return TRUE; }
	if ( n_bmp_is_trans( bmp_f, color_center ) ) { return TRUE; }

	u32 c;
	if ( n_bmp_ptr_get( bmp_f, x + dx, y + dy, &c ) ) { return TRUE; }

	if ( n_bmp_is_trans( bmp_f, c ) ) { return TRUE; }

	if ( c != color_target ) { return TRUE; }

	u32 cc;
	if ( n_bmp_ptr_get( bmp_f, x,y, &cc ) )
	{
		return TRUE;
	} else {
		if ( cc != color_center ) { return TRUE; }
	}

	cc = n_bmp_blend_pixel( color_center, color_target, 1.0 / ( 1 + blur ) );


	n_bmp_ptr_get( bmp_t, x,y, &c );
	c = n_bmp_blend_pixel( c, cc, 0.5 );


	n_bmp_ptr_set( bmp_t, x,y, c );


	return FALSE;
}

void
n_bmp_scaler_big_pixelart( n_bmp *bmp, int zoom )
{

	if ( n_bmp_error( bmp ) ) { return; }

	if ( zoom <= 1 ) { return; }


	// [!] : nearest neighbor

	n_bmp_scaler_big( bmp, zoom ); //return;

	// [x] : too blurry
	//n_bmp_resampler( bmp, zoom, zoom );


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_bmp tmp; n_bmp_zero( &tmp ); n_bmp_carboncopy( bmp, &tmp );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		u32 color_0;              n_bmp_ptr_get( bmp, x+0,y+0, &color_0 );
		u32 color_u; BOOL ret_u = n_bmp_ptr_get( bmp, x+0,y-1, &color_u );
		u32 color_d; BOOL ret_d = n_bmp_ptr_get( bmp, x+0,y+1, &color_d );
		u32 color_l; BOOL ret_l = n_bmp_ptr_get( bmp, x-1,y+0, &color_l );
		u32 color_r; BOOL ret_r = n_bmp_ptr_get( bmp, x+1,y+0, &color_r );

		u32 color_z = color_0;

		if (
			( ret_u == FALSE )&&( ret_l == FALSE )
			&&
			( n_bmp_scaler_big_pixelart_edge_detect( color_0, color_u, color_l ) )
		)
		{

			n_type_gfx xx = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x+xx,y, 0,-1, xx, color_u, color_z );
				if ( ret ) { break; }

				xx++;
				if ( xx >= N_BMP_SX( bmp ) ) { break; }
			}

			n_type_gfx count_x = xx / 2;
			if ( count_x > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					xx = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_u, 1.0 / ( 1 + 1 + 1 + xx ) );
//c = n_bmp_rgb( 255,0,0 );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x+xx,y+1, c );
						}

						xx++;
						if ( xx >= count_x ) { break; }
						if ( xx >= N_BMP_SX( bmp ) ) { break; }
					}

					count_x /= 2;
					if ( count_x <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}


			n_type_gfx yy = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x,y+yy, -1,0, yy, color_l, color_z );
				if ( ret ) { break; }

				yy++;
				if ( yy >= N_BMP_SY( bmp ) ) { break; }
			}

			n_type_gfx count_y = yy / 2;
			if ( count_y > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					yy = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_l, 1.0 / ( 1 + 1 + 1 + yy ) );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x+1,y+yy, c );
						}

						yy++;
						if ( yy >= count_y ) { break; }
						if ( yy >= N_BMP_SY( bmp ) ) { break; }
					}

					count_y /= 2;
					if ( count_y <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}

		}

		if (
			( ret_u == FALSE )&&( ret_r == FALSE )
			&&
			( n_bmp_scaler_big_pixelart_edge_detect( color_0, color_u, color_r ) )
		)
		{

			n_type_gfx xx = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x-xx,y, 0,-1, xx, color_u, color_z );
				if ( ret ) { break; }

				xx++;
				if ( ( x - xx ) < 0 ) { break; }
			}

			n_type_gfx count_x = xx / 2;
			if ( count_x > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					xx = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_u, 1.0 / ( 1 + 1 + 1 + xx ) );
//c = n_bmp_rgb( 255,0,0 );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x-xx,y+1, c );
						}

						xx++;
						if ( xx >= count_x ) { break; }
						if ( ( x - xx ) < 0 ) { break; }
					}

					count_x /= 2;
					if ( count_x <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}


			n_type_gfx yy = 0;
			n_posix_loop
			{//break;

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x,y+yy, 1,0, yy, color_r, color_z );
				if ( ret ) { break; }

				yy++;
				if ( yy >= N_BMP_SY( bmp ) ) { break; }
			}

			n_type_gfx count_y = yy / 2;
			if ( count_y > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					yy = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_r, 1.0 / ( 1 + 1 + 1 + yy ) );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x-1,y+yy, c );
						}

						yy++;
						if ( yy >= count_y ) { break; }
						if ( yy >= N_BMP_SY( bmp ) ) { break; }
					}

					count_y /= 2;
					if ( count_y <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}

		}

		if (
			( ret_d == FALSE )&&( ret_l == FALSE )
			&&
			( n_bmp_scaler_big_pixelart_edge_detect( color_0, color_d, color_l ) )
		)
		{

			n_type_gfx xx = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x+xx,y, 0,1, xx, color_d, color_z );
				if ( ret ) { break; }

				xx++;
				if ( xx >= N_BMP_SX( bmp ) ) { break; }
			}

			n_type_gfx count_x = xx / 2;
			if ( count_x > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					xx = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_d, 1.0 / ( 1 + 1 + 1 + xx ) );
//c = n_bmp_rgb( 255,0,0 );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x+xx,y-1, c );
						}

						xx++;
						if ( xx >= count_x ) { break; }
						if ( xx >= N_BMP_SX( bmp ) ) { break; }
					}

					count_x /= 2;
					if ( count_x <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}


			n_type_gfx yy = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x,y-yy, -1,0, yy, color_l, color_z );
				if ( ret ) { break; }

				yy++;
				if ( ( y - yy ) < 0 ) { break; }
			}

			n_type_gfx count_y = yy / 2;
			if ( count_y > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					yy = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_l, 1.0 / ( 1 + 1 + 1 + yy ) );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x+1,y-yy, c );//n_bmp_rgb( 255,0,0 ) );
						}

						yy++;
						if ( yy >= count_y ) { break; }
						if ( ( y - yy ) < 0 ) { break; }
					}

					count_y /= 2;
					if ( count_y <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}

		}

		if (
			( ret_d == FALSE )&&( ret_r == FALSE )
			&&
			( n_bmp_scaler_big_pixelart_edge_detect( color_0, color_d, color_r ) )
		)
		{

			n_type_gfx xx = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x-xx,y, 0,1, xx, color_d, color_z );
				if ( ret ) { break; }

				xx++;
				if ( ( x - xx ) < 0 ) { break; }
			}

			n_type_gfx count_x = xx / 2;
			if ( count_x > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					xx = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_d, 1.0 / ( 1 + 1 + 1 + xx ) );
//c = n_bmp_rgb( 255,0,0 );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x-xx,y-1, c );
						}

						xx++;
						if ( xx >= count_x ) { break; }
						if ( ( x - xx ) < 0 ) { break; }
					}

					count_x /= 2;
					if ( count_x <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}


			n_type_gfx yy = 0;
			n_posix_loop
			{

				BOOL ret = n_bmp_scaler_big_pixelart_pixel( bmp, &tmp, x,y-yy, 1,0, yy, color_r, color_z );
				if ( ret ) { break; }

				yy++;
				if ( ( y - yy ) < 0 ) { break; }
			}

			n_type_gfx count_y = yy / 2;
			if ( count_y > 0 )
			{
				int i = 0;
				n_posix_loop
				{
					yy = 0;
					n_posix_loop
					{

						u32 c = n_bmp_blend_pixel( color_z, color_r, 1.0 / ( 1 + 1 + 1 + yy ) );
						if ( FALSE == n_bmp_is_trans( &tmp, c ) )
						{
							n_bmp_ptr_set( &tmp, x-1,y-yy, c );
						}

						yy++;
						if ( yy >= count_y ) { break; }
						if ( ( y - yy ) < 0 ) { break; }
					}

					count_y /= 2;
					if ( count_y <= 0 ) { break; }

					i++;
					if ( i >= zoom ) { break; }
				}
			}

		}


		x++;
		if ( x >= bmpsx )
		{

			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	n_bmp_replace( &tmp, bmp );


	return;
}


#endif // _H_NONNON_NEUTRAL_BMP_TRANSFORM

