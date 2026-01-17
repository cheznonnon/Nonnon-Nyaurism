// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : trouble shooter
//
//	[ when engine seems to be misbehave ]
//
//	set n_bmp.transparent_onoff FALSE




#ifndef _H_NONNON_WIN32_GAME_TRANSITION
#define _H_NONNON_WIN32_GAME_TRANSITION




#include "../neutral/bmp/all.c"
#include "../neutral/random.c"


#include "./helper.c"




#define N_GAME_TRANSITION_NOTHING   0
#define N_GAME_TRANSITION_WIPE_X    1
#define N_GAME_TRANSITION_WIPE_Y    2
#define N_GAME_TRANSITION_SHUTTER   3
#define N_GAME_TRANSITION_INFLATE   4
#define N_GAME_TRANSITION_CIRCLE    5
#define N_GAME_TRANSITION_FADE      6
#define N_GAME_TRANSITION_DIZZY     7
#define N_GAME_TRANSITION_MOSAIC    8
#define N_GAME_TRANSITION_WHIRL     9
#define N_GAME_TRANSITION_SCROLL_U 10
#define N_GAME_TRANSITION_SCROLL_D 11
#define N_GAME_TRANSITION_SCROLL_L 12
#define N_GAME_TRANSITION_SCROLL_R 13
#define N_GAME_TRANSITION_LAST     13




static n_type_gfx n_game_transition_circle_x = 0;
static n_type_gfx n_game_transition_circle_y = 0;

static n_type_gfx n_game_transition_offset_x = 0;
static n_type_gfx n_game_transition_offset_y = 0;


static BOOL n_game_transition_percent_smooth = FALSE;




typedef struct {

	n_bmp       *bmp_old;
	n_bmp       *bmp_new;
	n_bmp       *bmp_ret;
	n_type_gfx   x,y,sx,sy;
	n_type_real  blend;
	u32          oy, cores;

} n_game_transition_fade_thread_struct;

void
n_game_transition_fade_thread_main( n_game_transition_fade_thread_struct *p )
{

	// [!] : no error check available

	n_type_gfx fx = 0;
	n_type_gfx fy = p->oy;
	n_type_gfx tx = 0;
	n_type_gfx ty = p->oy;
	n_posix_loop
	{

		u32 color_old; n_bmp_ptr_get_fast( p->bmp_old, fx,fy, &color_old );
		u32 color_new; n_bmp_ptr_get_fast( p->bmp_new, fx,fy, &color_new );

		u32 color = n_bmp_blend_pixel( color_old, color_new, p->blend );

		n_bmp_ptr_set_fast( p->bmp_ret, p->x + tx, p->y + ty, color );

		fx++; tx++;
		if ( fx >= p->sx )
		{
			fx = tx = 0;

			fy += p->cores;
			ty += p->cores;
			if ( fy >= p->sy ) { break; }
		}
	}


	return;
}

n_thread_return
n_game_transition_fade_thread( n_thread_argument p )
{

	n_game_transition_fade_thread_main( (void*) p );

	return 0;
}

// internal
void
n_game_transition_fade( n_bmp *bmp_old, n_bmp *bmp_new, n_bmp *bmp_ret, n_type_gfx x, n_type_gfx y, n_type_real blend )
{

	n_type_gfx sx = N_BMP_SX( bmp_old );
	n_type_gfx sy = N_BMP_SY( bmp_old );


#ifdef N_POSIX_PLATFORM_MAC

	BOOL prv = n_bmp_is_multithread;
	n_bmp_is_multithread = TRUE;


	NSOperationQueue *queue = [[NSOperationQueue alloc] init];
	u32               cores = n_posix_cpu_count();


	n_game_transition_fade_thread_struct *p = (void*) n_memory_new( cores * sizeof( n_game_transition_fade_thread_struct ) );


	NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

	u32 i = 0;
	n_posix_loop
	{

		n_game_transition_fade_thread_struct tmp = { bmp_old, bmp_new, bmp_ret, x,y,sx,sy, blend, i,cores };
		n_memory_copy( &tmp, &p[ i ], sizeof( n_game_transition_fade_thread_struct ) );

		n_game_transition_fade_thread_main( &p[ i ]);

		i++;
		if ( i >= cores ) { break; }
	}

	}];
	[queue addOperation:o];


	[queue waitUntilAllOperationsAreFinished];


	n_memory_free( p );


	n_bmp_is_multithread = prv;


#else  // #ifdef N_POSIX_PLATFORM_MAC

	// [x] : Win9x : can run but not working

	if (
//(0)&&
		( n_thread_onoff() )
		&&
		( ( sx * sy ) >= N_BMP_MULTITHREAD_GRANULARITY )
	)
	{

#ifdef N_BMP_MULTITHREAD_DEBUG

		n_posix_debug_literal( " n_game_transition_fade() " );

#endif // #ifdef N_BMP_MULTITHREAD_DEBUG


		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;


		u32 cores = n_thread_core_count;

		n_thread                             *h = (void*) n_memory_new( cores * sizeof( n_thread                             ) );
		n_game_transition_fade_thread_struct *p = (void*) n_memory_new( cores * sizeof( n_game_transition_fade_thread_struct ) );


		u32 i = 0;
		n_posix_loop
		{

			n_game_transition_fade_thread_struct tmp = { bmp_old, bmp_new, bmp_ret, x,y,sx,sy, blend, i,cores };
			n_memory_copy( &tmp, &p[ i ], sizeof( n_game_transition_fade_thread_struct ) );

			h[ i ] = n_thread_init( n_game_transition_fade_thread, &p[ i ] );

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

		n_game_transition_fade_thread_struct tmp = { bmp_old, bmp_new, bmp_ret, x,y,sx,sy, blend, 0,1 };
		n_game_transition_fade_thread_main( &tmp );

	}

#endif // #ifdef N_POSIX_PLATFORM_MAC


	return;
}




typedef struct {

	n_type_real unit;
	n_type_real x,y,sx,sy;

	n_type_real percent;
	n_type_real frame_msec;
	n_type_real tick_count;

	n_type_gfx  shutter_index;
	n_type_gfx  shutter_offset;

	n_type_gfx  circle_prev_size;

	n_type_real whirl_step;
	n_type_gfx  whirl_scale;

	int         error_code;

} n_game_transition_struct;

#define n_game_transition_struct_zero( p ) n_memory_zero( p, sizeof( n_game_transition_struct ) )




BOOL
n_game_transition_main( n_game_transition_struct *p, n_bmp *bmp, n_bmp *bmp_old, n_bmp *bmp_new, u32 msec, int type )
{

	// [Mechanism]
	//
	//	bmp     : canvas
	//	bmp_old : this will disappear
	//	bmp_new : this will appear

	// [!] : Return Value
	//
	//	FALSE : running
	//	TRUE  : stop or error


	if ( n_bmp_error( bmp     ) ) { p->error_code = 1; return TRUE; }
	if ( n_bmp_error( bmp_old ) ) { p->error_code = 2; return TRUE; }
	if ( n_bmp_error( bmp_new ) ) { p->error_code = 3; return TRUE; }

	if ( N_BMP_SX( bmp_old ) != N_BMP_SX( bmp_new ) ) { p->error_code = 4; return TRUE; }
	if ( N_BMP_SY( bmp_old ) != N_BMP_SY( bmp_new ) ) { p->error_code = 5; return TRUE; }


	// [!] : nothing to do

	if ( N_BMP_PTR( bmp_old ) == N_BMP_PTR( bmp_new ) ) { p->error_code = 6; return TRUE; }
	if ( N_BMP_PTR( bmp     ) == N_BMP_PTR( bmp_new ) ) { p->error_code = 7; return TRUE; }

	if ( msec == 0 ) { return p->error_code = 8; TRUE; }

	if ( ( type < 0 )||( type > N_GAME_TRANSITION_LAST ) ) { p->error_code = 9; return TRUE; }


	const n_type_real ox = n_game_transition_offset_x;
	const n_type_real oy = n_game_transition_offset_y;

	const n_type_gfx bmpsx = N_BMP_SX( bmp_new );
	const n_type_gfx bmpsy = N_BMP_SY( bmp_new );


	n_type_real pc = p->percent;

	if ( n_game_transition_percent_smooth )
	{
		pc = 100 * fabs( sin( 2.0 * M_PI * ( 0.25 / 100 * p->percent ) ) );
	}


	if ( type == N_GAME_TRANSITION_NOTHING )
	{

		// Performance : fastest

		if ( pc ==   0 )
		{
			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );
		} else
		if ( pc >= 100 )
		{
			n_bmp_fastcopy( bmp_new, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );
		}

	} else

	if ( type == N_GAME_TRANSITION_WIPE_X )
	{

		// Performance : WIPE_X:WIPE_X = 1:1


		if ( pc == 0 )
		{

			p->unit = ceil( (n_type_real) bmpsx * 0.01 );

			p->x  = ( bmpsx / 2 );
			p->y  =     0;
			p->sx =     0;
			p->sy = bmpsy;

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		p->sx = n_posix_min_n_type_real( (n_type_real) bmpsx, p->unit * pc );
		p->x  = ( bmpsx - p->sx ) / 2;

		n_type_gfx tx  = (n_type_gfx) p->x;
		n_type_gfx ty  = (n_type_gfx) p->y;
		n_type_gfx tsx = (n_type_gfx) p->sx;
		n_type_gfx tsy = (n_type_gfx) p->sy;

		n_bmp_fastcopy( bmp_new, bmp, tx,ty,tsx,tsy, (n_type_gfx) ox + tx, (n_type_gfx) oy + ty );

	} else

	if ( type == N_GAME_TRANSITION_WIPE_Y )
	{

		// Performance : WIPE_X:WIPE_Y = 1:1


		if ( pc == 0 )
		{

			// [!] : Copilot : Optimization

			//p->unit = ceil( (n_type_real) bmpsy * 0.01 );
			p->unit = (n_type_real) ( ( bmpsy + 99 ) / 100 );

			p->x  =     0;
			p->y  = ( bmpsy / 2 );
			p->sx = bmpsx;
			p->sy =     0;

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		p->sy = n_posix_min_n_type_real( (n_type_real) bmpsy, p->unit * pc );
		p->y  = ( (n_type_real) bmpsy - p->sy ) / 2;

		n_type_gfx tx  = (n_type_gfx) p->x;
		n_type_gfx ty  = (n_type_gfx) p->y;
		n_type_gfx tsx = (n_type_gfx) p->sx;
		n_type_gfx tsy = (n_type_gfx) p->sy;

		n_bmp_fastcopy( bmp_new, bmp, tx,ty,tsx,tsy, (n_type_gfx) ox + tx, (n_type_gfx) oy + ty );

	} else

	if ( type == N_GAME_TRANSITION_SHUTTER )
	{

		// Performance : WIPE_X:SHUTTER = 1:1.5


		if ( pc == 0 )
		{

			// [!] : Copilot : Optimization

			//p->unit = bmpsy;
			//p->unit = ceil( (n_type_real) p->unit * 0.01 );
			p->unit = (n_type_real) ( ( bmpsy + 99 ) / 100 );

			p->x  =     0;
			p->y  =     0;
			p->sx = bmpsx;
			p->sy =     1;

			p->shutter_index = p->shutter_offset = 0;

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		n_posix_loop
		{//break;

			n_type_gfx tx  = (n_type_gfx) p->x;
			n_type_gfx ty  = (n_type_gfx) p->y;
			n_type_gfx tsx = (n_type_gfx) p->sx;
			n_type_gfx tsy = (n_type_gfx) p->sy;

			n_bmp_fastcopy( bmp_new, bmp, tx,ty, tsx,tsy, (n_type_gfx) ox + tx, (n_type_gfx) ox + ty );

			p->y += 10;
			if ( p->y >= bmpsy )
			{
				p->shutter_offset++;

				p->y = p->shutter_offset;
			}


			p->shutter_index++;
			if ( p->shutter_index >= ( p->unit * pc ) ) { break; }
		}

	} else

	if ( type == N_GAME_TRANSITION_INFLATE )
	{

		// Performance : WIPE_X:INFLATE = 1:1.5


		if ( pc == 0 )
		{

			// [!] : Copilot : Optimization

			//p->unit = ceil( (n_type_real) n_posix_max_n_type_gfx( bmpsx, bmpsy ) * 0.01 );
			p->unit = (n_type_real) ( ( n_posix_max_n_type_gfx( bmpsx, bmpsy ) + 99 ) / 100 );

			p->x  = bmpsx / 2;
			p->y  = bmpsy / 2;
			p->sx = p->unit;
			p->sy = p->unit;

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		n_type_gfx size = (n_type_gfx) ( p->unit * pc );


		p->x  = ( bmpsx - size ) / 2;
		p->y  = ( bmpsy - size ) / 2;
		p->sx = size;
		p->sy = size;

		n_type_gfx tx  = (n_type_gfx) p->x;
		n_type_gfx ty  = (n_type_gfx) p->y;
		n_type_gfx tsx = (n_type_gfx) p->sx;
		n_type_gfx tsy = (n_type_gfx) p->sy;

		n_bmp_fastcopy( bmp_new, bmp, tx,ty, tsx,tsy, (n_type_gfx) ox + tx, (n_type_gfx) oy + ty );

	} else

	if ( type == N_GAME_TRANSITION_CIRCLE )
	{

		// Performance : WIPE_X:CIRCLE = 1:3

		if ( pc == 0 )
		{

			p->circle_prev_size = -1;

			n_type_gfx half_sx   = bmpsx / 2;
			n_type_gfx half_sy   = bmpsy / 2;
			n_type_gfx radius_sx = half_sx + abs( half_sx - n_game_transition_circle_x );
			n_type_gfx radius_sy = half_sy + abs( half_sy - n_game_transition_circle_y );
			n_type_gfx radius    = n_posix_max_n_type_gfx( radius_sx, radius_sy );

			// [!] : Copilot : Optimization

			/* approximate radius*sqrt(2) then compute 1% as integer */
			n_type_real approx = (n_type_real) radius * 1.41421356237f;
			p->unit = (n_type_real) ( ( (n_type_gfx) approx + 99 ) / 100 );

			//p->unit = ceil( (n_type_real) radius * sqrt( 2 ) );
			//p->unit = ceil( (n_type_real) p->unit * 0.01 );

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		const n_type_gfx size = (n_type_gfx) ( p->unit * pc );


#ifdef N_POSIX_PLATFORM_MAC

#define N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

#endif // #ifdef N_POSIX_PLATFORM_MAC


#ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

//NSLog( @"multihreaded" );

		BOOL prv = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		NSOperationQueue *queue = [[NSOperationQueue alloc] init];
		u32               cores = n_posix_cpu_count();

		u32 i = 0;
		n_posix_loop
		{

		NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{

		n_type_gfx y = i;
		n_posix_loop
		{

#else  // #ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

		n_type_gfx y = 0;
		n_posix_loop
		{

#endif // #ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

			n_type_gfx x = 0;
			n_posix_loop
			{

				n_type_gfx center_x = (n_type_gfx) x - n_game_transition_circle_x;
				n_type_gfx center_y = (n_type_gfx) y - n_game_transition_circle_y;

				BOOL outer = n_bmp_hoop_detect( center_x,center_y, size );
				BOOL inner = FALSE;//n_bmp_hoop_detect( center_x,center_y, p_size );

				while( outer )
				{//break;

					inner = n_bmp_hoop_detect( center_x,center_y, p->circle_prev_size );
					if ( inner == FALSE ) { break; }

					center_x++; x++;
					if ( x >= bmpsx ) { break; }
				}


				if ( ( inner == FALSE )&&( outer ) )
				{

					u32 color;

					n_bmp_ptr_get_fast( bmp_new, x, y, &color );
/*
					if ( FALSE == n_bmp_hoop_detect( center_x,center_y, size - 2 ) )
					{
//color = 0x00ff0000;
						u32 c; n_bmp_ptr_get_fast( bmp, (n_type_gfx) ox+x, (n_type_gfx) oy+y, &c );
						color = n_bmp_blend_pixel( c, color, 0.5 );
					}
*/
					n_bmp_ptr_set_fast( bmp, (n_type_gfx) ox + x, (n_type_gfx) oy + y, color );

				} else
				if ( ( inner == FALSE )&&( outer == FALSE ) )
				{

					if ( center_x >= 0 ) { x = bmpsx; }

				}


				x++;
				if ( x >= bmpsx ) { break; }
			}

#ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

			y += cores;
			if ( y >= bmpsy ) { break; }
		}

		}];
		[queue addOperation:o];

			i++;
			if ( i >= cores ) { break; }
		}

#else  // #ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

			y++;
			if ( y >= bmpsy ) { break; }
		}

#endif // #ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

#ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD

		[queue waitUntilAllOperationsAreFinished];

		n_bmp_is_multithread = prv;

#endif // #ifdef N_GAME_TRANSITION_CIRCLE_MAC_MULTITHREAD


		p->circle_prev_size = size;

	} else

	if ( type == N_GAME_TRANSITION_FADE )
	{

		// WIPE_X:FADE = 1:12


		if ( pc == 0 )
		{

			p->x  =     0;
			p->y  =     0;
			p->sx = bmpsx;
			p->sy = bmpsy;

		}


		// [!] : slow

		//n_bmp_fastcopy ( bmp_old, bmp, 0,0,bmpsx,bmpsy, ox,oy );
		//n_bmp_blendcopy( bmp_new, bmp, 0,0,bmpsx,bmpsy, ox,oy, (n_type_real) ( 100 - (*percent) ) * 0.01 );


		// [!] : fast

		n_game_transition_fade( bmp_old, bmp_new, bmp, (n_type_gfx) ox, (n_type_gfx) oy, (n_type_real) pc * 0.01 );

	} else

	if ( type == N_GAME_TRANSITION_DIZZY )
	{

		// Performance : WIPE_X:DIZZY = 1:4


		if ( pc == 0 )
		{

			// [!] : Copilot : Optimization

			//p->unit = (n_type_real) ( bmpsx * bmpsy ) * 0.01 ;
			p->unit = (n_type_real) ( ( (u32)bmpsx * (u32)bmpsy + 99 ) / 100 );

			p->x  =     0;
			p->y  =     0;
			p->sx = bmpsx;
			p->sy = bmpsy;

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		u32 color_old;
		u32 color_new;

		n_type_gfx i = 0;
		n_posix_loop
		{//break;

			n_type_gfx tx = n_random_range( bmpsx );
			n_type_gfx ty = n_random_range( bmpsy );

			n_bmp_ptr_get_fast( bmp    , tx,ty, &color_old );
			n_bmp_ptr_get_fast( bmp_new, tx,ty, &color_new );

			if ( color_old == color_new )
			{
				if ( ( ty - 1 ) >= 0 )
				{
					n_bmp_ptr_get_fast( bmp    , tx+0,ty-1, &color_old );
					n_bmp_ptr_get_fast( bmp_new, tx+0,ty-1, &color_new );
				}
			}

			if ( color_old == color_new )
			{
				if ( ( ty + 1 ) < bmpsy )
				{
					n_bmp_ptr_get_fast( bmp    , tx+0,ty+1, &color_old );
					n_bmp_ptr_get_fast( bmp_new, tx+0,ty+1, &color_new );
				}
			}

			if ( color_old == color_new )
			{
				if ( ( tx - 1 ) >= 0 )
				{
					n_bmp_ptr_get_fast( bmp    , tx-1,ty+0, &color_old );
					n_bmp_ptr_get_fast( bmp_new, tx-1,ty+0, &color_new );
				}
			}

			if ( color_old == color_new )
			{
				if ( ( tx + 1 ) < bmpsx )
				{
					n_bmp_ptr_get_fast( bmp    , tx+1,ty+0, &color_old );
					n_bmp_ptr_get_fast( bmp_new, tx+1,ty+0, &color_new );
				}
			}

			if ( color_old != color_new )
			{
				n_bmp_ptr_set_fast( bmp, (n_type_gfx) ox + tx, (n_type_gfx) oy + ty, color_new );
			}

			i++;
			if ( i >= p->unit ) { break; }

		}


		if ( pc >= 100 )
		{
			n_bmp_fastcopy ( bmp_new, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );
		}

	} else

	if ( type == N_GAME_TRANSITION_MOSAIC )
	{

		// Performance : WIPE_X:MOSAIC = 1:4


		if ( pc == 0 )
		{

			n_bmp_fastcopy( bmp_old, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}


		n_bmp  *bmp_sample;
		n_type_real  half;

		if ( pc <= 50 )
		{
			p->unit    = pc;
			bmp_sample = bmp_old;
		} else {
			p->unit    = 100 - pc;
			bmp_sample = bmp_new;
		}

		p->unit = n_posix_max_n_type_real( 1, p->unit );
		   half = p->unit / 2;


		n_type_gfx fx,fy, fsx,fsy, startx;
		u32        color;

		p->x = -( fmod( bmpsx, p->unit ) / 2 ); startx = (n_type_gfx) p->x;
		p->y = -( fmod( bmpsy, p->unit ) / 2 );
		n_posix_loop
		{

			fx = (n_type_gfx) ( p->x + half );
			fy = (n_type_gfx) ( p->y + half );

			n_bmp_error_clipping( bmp_sample,NULL, &fx,&fy,NULL,NULL, NULL,NULL );

			n_bmp_ptr_get( bmp_sample, fx,fy, &color );


			fx  = (n_type_gfx) ( ox + p->x );
			fy  = (n_type_gfx) ( oy + p->y );
			fsx = (n_type_gfx) p->unit;
			fsy = (n_type_gfx) p->unit;

			if ( fx < ox ) { fx = (n_type_gfx) ox; }
			if ( fy < oy ) { fy = (n_type_gfx) oy; }
			if ( ( fx + fsx ) >= ( ox + bmpsx ) ) { fsx = ( (n_type_gfx) ox + bmpsx ) - fx; }
			if ( ( fy + fsy ) >= ( oy + bmpsy ) ) { fsy = ( (n_type_gfx) oy + bmpsy ) - fy; }

			n_bmp_box( bmp, fx,fy,fsx,fsy, color );


			p->x += p->unit;
			if ( p->x >= bmpsx )
			{

				p->x = startx;

				p->y += p->unit;
				if ( p->y >= bmpsy ) { break; }
			}
		}


		if ( pc >= 100 )
		{

			n_bmp_fastcopy( bmp_new, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}

	} else

	if ( type == N_GAME_TRANSITION_WHIRL )
	{

		// Performance : WIPE_X:WHIRL = 1:8


		if ( pc == 0 )
		{

			n_type_real speed = 4.0;

			p->whirl_step  = (n_type_real) ( 360.0 * 10.0 ) / 100.0 / speed;
			p->whirl_scale = 5;

		}


		n_bmp_fastcopy( bmp_new, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );


		n_bmp b;
		int   c = (int) pc;

		u32 color_trans = n_bmp_black_invisible;

		n_bmp_carboncopy( bmp_old, &b );
		n_bmp_scaler_lil( &b, 2 + (n_type_gfx) ( c / p->whirl_scale ) );
		n_bmp_matrix_rotate( &b, (n_type_gfx) ( (n_type_real) c * p->whirl_step ), color_trans, TRUE );
		n_bmp_resizer( &b, bmpsx,bmpsy, color_trans, N_BMP_RESIZER_CENTER );

		n_bmp_transcopy
		(
			&b, bmp,
			0,0,bmpsx,bmpsy,
			(n_type_gfx) ox + n_game_centering( bmpsx, N_BMP_SX( &b ) ),
			(n_type_gfx) oy + n_game_centering( bmpsy, N_BMP_SY( &b ) )
		);

		n_bmp_free( &b );


		if ( pc >= 100 )
		{

			n_bmp_fastcopy( bmp_new, bmp, 0,0,bmpsx,bmpsy, (n_type_gfx) ox, (n_type_gfx) oy );

		}

	} else

	if ( ( type >= N_GAME_TRANSITION_SCROLL_U )&&( type <= N_GAME_TRANSITION_SCROLL_R ) )
	{

		// Performance : WIPE_X:SCROLL = 1:1


		if ( pc == 0 )
		{
			// [!] : Copilot : Optimization
			//p->sx = ceil( (n_type_real) bmpsx * 0.01 );
			//p->sy = ceil( (n_type_real) bmpsy * 0.01 );
			p->sx = (n_type_real) ( ( bmpsx + 99 ) / 100 );
			p->sy = (n_type_real) ( ( bmpsy + 99 ) / 100 );
		}


		p->x = n_posix_min_n_type_real( (n_type_real) bmpsx, p->sx * pc );
		p->y = n_posix_min_n_type_real( (n_type_real) bmpsy, p->sy * pc );

//n_bmp_flush( bmp, 0 );

		n_type_gfx ix  = (n_type_gfx) p->x;
		n_type_gfx iy  = (n_type_gfx) p->y;
		n_type_gfx iox = (n_type_gfx) ox;
		n_type_gfx ioy = (n_type_gfx) oy;

		if ( type == N_GAME_TRANSITION_SCROLL_U )
		{

			n_type_gfx fy  = (n_type_gfx) p->y;
			n_type_gfx fsy = (n_type_gfx) ( (n_type_real) bmpsy - p->y );
			n_type_gfx ty  = (n_type_gfx) 0;
			n_type_gfx tsy = (n_type_gfx) p->y;

			n_bmp_fastcopy( bmp_old, bmp, 0,fy,bmpsx,fsy, iox,  0 + ioy );
			n_bmp_fastcopy( bmp_new, bmp, 0,ty,bmpsx,tsy, iox,fsy + ioy );

		} else
		if ( type == N_GAME_TRANSITION_SCROLL_D )
		{

			n_type_gfx fy  = (n_type_gfx) 0;
			n_type_gfx fsy = (n_type_gfx) ( (n_type_real) bmpsy - p->y );
			n_type_gfx ty  = (n_type_gfx) ( (n_type_real) bmpsy - p->y );
			n_type_gfx tsy = (n_type_gfx) p->y;

			n_bmp_fastcopy( bmp_old, bmp, 0,fy,bmpsx,fsy, iox,iy + ioy );
			n_bmp_fastcopy( bmp_new, bmp, 0,ty,bmpsx,tsy, iox, 0 + ioy );

		} else
		if ( type == N_GAME_TRANSITION_SCROLL_L )
		{

			n_type_gfx fx  = (n_type_gfx) p->x;
			n_type_gfx fsx = (n_type_gfx) ( (n_type_real) bmpsx - p->x );
			n_type_gfx tx  = (n_type_gfx) 0;
			n_type_gfx tsx = (n_type_gfx) p->x;

			n_bmp_fastcopy( bmp_old, bmp, fx,0,fsx,bmpsy,   0 + iox,ioy );
			n_bmp_fastcopy( bmp_new, bmp, tx,0,tsx,bmpsy, fsx + iox,ioy );

		} else
		if ( type == N_GAME_TRANSITION_SCROLL_R )
		{

			n_type_gfx fx  = (n_type_gfx) 0;
			n_type_gfx fsx = (n_type_gfx) ( (n_type_real) bmpsx - p->x );
			n_type_gfx tx  = (n_type_gfx) ( (n_type_real) bmpsx - p->x );
			n_type_gfx tsx = (n_type_gfx) p->x;

			n_bmp_fastcopy( bmp_old, bmp, fx,0,fsx,bmpsy, ix + iox,ioy );
			n_bmp_fastcopy( bmp_new, bmp, tx,0,tsx,bmpsy,  0 + iox,ioy );

		}

	}// else


	if ( p->percent >= 100 )
	{

		p->percent = 0;

		return TRUE;

	} else {

		// Auto-adjuster

		static n_type_real step = 0;

		if ( p->percent == 0 )
		{
			p->frame_msec = (n_type_real) msec / 100;
			p->tick_count = n_posix_tickcount();
			         step = 0;
		}

		n_type_real tick_count_cur   = n_posix_tickcount();
		n_type_real tick_count_delta = tick_count_cur - p->tick_count;

		if ( p->frame_msec < tick_count_delta )
		{

			p->percent += ceil( tick_count_delta / p->frame_msec );

			n_posix_sleep( (u32) ( p->frame_msec - fmod( tick_count_delta, p->frame_msec ) ) );

		} else
		if ( p->frame_msec > tick_count_delta )
		{

			n_posix_sleep( (u32) ( p->frame_msec - tick_count_delta ) );

			p->percent++;

		} else {

			p->percent++;

		}

		p->tick_count = tick_count_cur;

		p->percent = n_posix_minmax_n_type_real( 0, 100, p->percent );

	}


	return FALSE;
}

BOOL
n_game_transition( n_bmp *bmp, n_bmp *bmp_old, n_bmp *bmp_new, u32 msec, n_type_real *percent, int type )
{

	static n_game_transition_struct t;

	if ( (*percent) == 0 )
	{
		n_game_transition_struct_zero( &t );
		t.percent = 0;
	}

	BOOL ret = n_game_transition_main( &t, bmp, bmp_old, bmp_new, msec, type );

	(*percent) = t.percent;


	return ret;
}


#endif // _H_NONNON_WIN32_GAME_TRANSITION

