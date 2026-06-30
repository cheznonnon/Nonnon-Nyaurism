// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#define N_FREECELL_PILE_ERASE 0
#define N_FREECELL_PILE_DRAW  1

#define n_freecell_pile_erase( p, x,y ) n_freecell_pile( p, x,y, N_FREECELL_PILE_ERASE )
#define n_freecell_pile_draw(  p, x,y ) n_freecell_pile( p, x,y, N_FREECELL_PILE_DRAW  )

void
n_freecell_pile( n_freecell *p, int x, int y, int mode )
{

	// [!] : draw always for alpha blending

	if ( y == 0 )
	{

		if ( mode == N_FREECELL_PILE_ERASE )
		{
			n_game_chara_erase( &p->bottom[ x ] );
		} else {
			// [Needed] : alpha value of foundation needs this

			BOOL p_multithread = n_bmp_is_multithread;
			n_bmp_is_multithread = TRUE;

			n_game_chara_draw ( &p->bottom[ x ] );

			n_bmp_is_multithread = p_multithread;
		}

	}

#ifdef N_FREECELL_DEBUG_FOUNDATION

return;

#endif

	n_posix_loop
	{

		n_game_chara *c = &p->tableau[ n_freecell_pos2index( x, y + 0 ) ];

		if ( c->data == N_FREECELL_NOTHING ) { break; }


		// [!] : dragged cards are handled as overlay

		if ( p->drag_index != N_FREECELL_NOTHING )
		{
			if ( p->drag_index == n_freecell_pos2index( x,y ) ) { break; }
		}


		if ( mode == N_FREECELL_PILE_ERASE )
		{
			n_game_chara_erase( c );
		} else {
			n_game_chara_draw ( c );
		}


		y++;
		if ( y >= N_FREECELL_PILE_SY ) { break; }
	}


	return;
}

typedef struct {

	n_freecell *freecell;
	int         offset, cores;

} n_freecell_pile_draw_all_thread_struct;

#ifdef N_POSIX_PLATFORM_WINDOWS
DWORD WINAPI
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
u32
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
n_freecell_pile_draw_all_thread( LPVOID lpParameter )
{

	n_freecell_pile_draw_all_thread_struct *p = (void*) lpParameter;


	int x = p->offset;
	n_posix_loop
	{

		if ( p->freecell->redraw[ x ] )
		{
			p->freecell->redraw[ x ] = FALSE;
			n_freecell_pile_draw( p->freecell, x, 0 );
/*
n_game_chara *c = &p->freecell->tableau[ n_freecell_pos2index( x, 0 ) ];
if ( 0 == ( x % p->cores ) ) { n_bmp_box( &game.bmp, c->x,c->y, c->sx,10, n_bmp_rgb( 0,200,255 ) ); }
if ( 1 == ( x % p->cores ) ) { n_bmp_box( &game.bmp, c->x,c->y, c->sx,10, n_bmp_rgb( 255,0,200 ) ); }
if ( 2 == ( x % p->cores ) ) { n_bmp_box( &game.bmp, c->x,c->y, c->sx,10, n_bmp_rgb( 0,  0,255 ) ); }
if ( 3 == ( x % p->cores ) ) { n_bmp_box( &game.bmp, c->x,c->y, c->sx,10, n_bmp_rgb( 255,0,  0 ) ); }
*/
		}

		x += p->cores;
		if ( x >= N_FREECELL_PILE_SX ) { break; }
	}


	return 0;
}

void
n_freecell_pile_draw_all( n_freecell *p )
{

#ifdef N_POSIX_PLATFORM_MAC

	{

		// [!] : for Mac

		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;

		u32 cores = N_FREECELL_PILE_SX;

		NSOperationQueue *q = p->queue;//[[NSOperationQueue alloc] init];

		int i = 0;
		n_posix_loop
		{

			n_freecell_pile_draw_all_thread_struct tmp = { p, i, cores };

			NSOperation *o = [NSBlockOperation blockOperationWithBlock:^{
				n_freecell_pile_draw_all_thread( (void*) &tmp );
			}];

			[q addOperation:o];

			i++;
			if ( i >= cores ) { break; }
		}

		[q waitUntilAllOperationsAreFinished];

		n_bmp_is_multithread = p_multithread;

	}

#else

	if ( n_thread_onoff() )
	{

		BOOL p_multithread = n_bmp_is_multithread;
		n_bmp_is_multithread = TRUE;


		u32 cores = n_posix_min( n_thread_core_count, N_FREECELL_PILE_SX );
		//if ( cores > 1 ) { cores--; }
//NSLog( @"%d", cores );


		n_thread                               *h = n_memory_new( cores * sizeof( n_thread                               ) );
		n_freecell_pile_draw_all_thread_struct *f = n_memory_new( cores * sizeof( n_freecell_pile_draw_all_thread_struct ) );

		u32 i = 0;
		n_posix_loop
		{

			n_freecell_pile_draw_all_thread_struct tmp = { p, i, cores };
			n_memory_copy( &tmp, &f[ i ], sizeof( n_freecell_pile_draw_all_thread_struct ) );

			h[ i ] = n_thread_init( n_freecell_pile_draw_all_thread, &f[ i ] );

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
		n_memory_free( f );


		n_bmp_is_multithread = p_multithread;

	} else {

		int x = 0;
		n_posix_loop
		{

			if ( p->redraw[ x ] )
			{
//LARGE_INTEGER li_f; QueryPerformanceCounter( &li_f );

				p->redraw[ x ] = FALSE;
				n_freecell_pile_draw( p, x, 0 );

//LARGE_INTEGER li_t; QueryPerformanceCounter( &li_t );
//if ( x == 0 )  { n_game_hwndprintf_literal( " %d ", li_t.QuadPart - li_f.QuadPart ); }
			}


			x++;
			if ( x >= N_FREECELL_PILE_SX ) { break; }
		}

	}

#endif


	return;
}
