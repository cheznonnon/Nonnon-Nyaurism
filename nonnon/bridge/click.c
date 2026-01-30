// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : Mac Version : see game/chara.c to perform




#ifndef _H_NONNON_WIN32_GAME_CLICK
#define _H_NONNON_WIN32_GAME_CLICK




#ifdef N_POSIX_PLATFORM_WINDOWS

#include "../win64/_windows.c"

void
n_game_click_debug( char *str )
{
	return;
}


#else // #ifdef N_POSIX_PLATFORM_WINDOWS

typedef struct {

	n_type_gfx x, y;

} POINT;

u32
GetDoubleClickTime( void )
{
//NSLog( @"%f", [NSEvent doubleClickInterval] * 1000 );

	CGFloat t = [NSEvent doubleClickInterval] * 1000;

	return (u32) t;
}

void
GetCursorPos( POINT *pt_ret )
{

	NSPoint ns = n_game_chara_cursor_position;
	POINT   pt = { (n_type_gfx) ns.x, (n_type_gfx) ns.y };

//NSLog( @"%d %d", pt.x, pt.y );

	if ( pt_ret != NULL ) { (*pt_ret) = pt; }

	return;
}

void
n_game_click_debug( n_posix_char *str )
{

//NSLog( @"%s", str );

	return;
}


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#define N_GAME_CLICK_PHASE_LOCK     -1
#define N_GAME_CLICK_PHASE_NEUTRAL   0
#define N_GAME_CLICK_PHASE_DOWN      1
#define N_GAME_CLICK_PHASE_UP        2
#define N_GAME_CLICK_PHASE_DOUBLE    3




typedef struct {

	// public

	int   vk;

	// internal : auto-set by functions

	int   phase;
	u32   msec;

	POINT pt_prv;
	POINT pt_cur;
	POINT threshold;

} n_game_click;




BOOL
n_game_click_threshold( n_game_click *p )
{

	if ( p == NULL ) { return FALSE; }

	GetCursorPos( &p->pt_cur );

//NSLog( @"%d %d", p->pt_prv.x, p->pt_cur.x );

	if (
		( p->threshold.x >= abs( p->pt_prv.x - p->pt_cur.x ) )
		&&
		( p->threshold.y >= abs( p->pt_prv.y - p->pt_cur.y ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

void
n_game_click_mouse_threshold( n_game_click *p )
{

	if ( p == NULL ) { return; }


	n_type_gfx threshold_sx = 0;
	n_type_gfx threshold_sy = 0;

	threshold_sx = threshold_sy = 8;

	p->threshold.x = threshold_sx * 2;
	p->threshold.y = threshold_sy * 2;

//NSLog( @"%d %d", p->threshold.x, p->threshold.y );


	return;
}

#define n_game_click_zero( p ) n_memory_zero( p, sizeof( n_game_click ) )

void
n_game_click_init( n_game_click *p, int vk )
{

	if ( p == NULL ) { return; }


	n_game_click_zero( p );

	p->vk = vk;


	return;
}

BOOL
n_game_click_single( n_game_click *p )
{

	if ( p == NULL ) { return FALSE; }


	if ( n_win_is_input( p->vk ) )
	{

		if ( p->phase == N_GAME_CLICK_PHASE_NEUTRAL )
		{

			p->phase = N_GAME_CLICK_PHASE_DOWN;

			n_game_click_mouse_threshold( p );

			GetCursorPos( &p->pt_prv );

			p->pt_cur = p->pt_prv;

		} else
		if ( p->phase == N_GAME_CLICK_PHASE_DOWN )
		{

			if ( FALSE == n_game_click_threshold( p ) )
			{
				p->phase = N_GAME_CLICK_PHASE_LOCK;
			}

		}

	} else {

		if ( p->phase == N_GAME_CLICK_PHASE_LOCK )
		{

			p->phase = N_GAME_CLICK_PHASE_NEUTRAL;

		} else
		if ( p->phase == N_GAME_CLICK_PHASE_DOWN )
		{

			p->phase = N_GAME_CLICK_PHASE_NEUTRAL;

			if ( n_game_click_threshold( p ) )
			{
				return TRUE;
			}

		}

	}


	return FALSE;
}

BOOL
n_game_click_double( n_game_click *p )
{

	if ( p == NULL ) { return FALSE; }


	if ( n_win_is_input( p->vk ) )
	{

		if ( p->phase == N_GAME_CLICK_PHASE_NEUTRAL )
		{
n_game_click_debug( "N_GAME_CLICK_PHASE_NEUTRAL" );

			u32 msec = n_posix_tickcount();
			u32 gdct = GetDoubleClickTime();

#ifdef N_POSIX_PLATFORM_WINDOWS

			// [!] : prevent integer overflow

			if ( msec > ( ULONG_MAX - gdct ) ) { msec = ULONG_MAX; } else { msec += gdct; }

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

			msec += gdct;

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

			p->phase = N_GAME_CLICK_PHASE_DOWN;
			p->msec  = msec;

			n_game_click_mouse_threshold( p );

			GetCursorPos( &p->pt_prv );

			p->pt_cur = p->pt_prv;

		} else
		if ( p->phase == N_GAME_CLICK_PHASE_UP )
		{
n_game_click_debug( "N_GAME_CLICK_PHASE_UP" );

			p->phase = N_GAME_CLICK_PHASE_NEUTRAL;

			if (
				( p->msec >= n_posix_tickcount() )
				&&
				( n_game_click_threshold( p ) )
			)
			{
				p->phase = N_GAME_CLICK_PHASE_DOUBLE;
			}

		}

	} else {

		if ( p->phase == N_GAME_CLICK_PHASE_DOWN )
		{
n_game_click_debug( "N_GAME_CLICK_PHASE_DOWN" );

			p->phase = N_GAME_CLICK_PHASE_NEUTRAL;

			if ( n_game_click_threshold( p ) )
			{
				p->phase = N_GAME_CLICK_PHASE_UP;
			}

		} else
		if ( p->phase == N_GAME_CLICK_PHASE_DOUBLE )
		{
n_game_click_debug( "N_GAME_CLICK_PHASE_DOUBLE" );

			p->phase = N_GAME_CLICK_PHASE_NEUTRAL;

			if ( n_game_click_threshold( p ) )
			{
				return TRUE;
			}

		}
	}


	return FALSE;
}


#endif // _H_NONNON_WIN32_GAME_CLICK

