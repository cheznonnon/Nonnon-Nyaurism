// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GAME_CHARA
#define _H_NONNON_WIN32_GAME_CHARA




#include "../neutral/bmp/all.c"


#ifdef N_POSIX_PLATFORM_MAC

#define HWND       void*
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04

static NSPoint n_game_chara_cursor_position;

void
n_game_chara_cursor_position_mousemoved_init( NSView *view )
{

	// [!] : call this at initWithCoder

	[view.window setAcceptsMouseMovedEvents:YES];
	[view.window makeFirstResponder:view];


	return;
}

void
n_game_chara_cursor_position_refresh( NSEvent *theEvent, n_bmp *bmp )
{

	// [!] : call this at mouseMoved and mouseDragged

	NSPoint pt = [theEvent locationInWindow];
	n_game_chara_cursor_position = n_mac_image_position_flip( bmp, pt );

	return;
}

n_posix_bool
n_win_is_input( int vk )
{

	n_posix_bool ret = n_posix_false;
	
	if ( vk == VK_LBUTTON )
	{
		ret = CGEventSourceButtonState
		(
			kCGEventSourceStateCombinedSessionState, 
			kCGMouseButtonLeft
		);
	} else
	if ( vk == VK_RBUTTON )
	{
		ret = CGEventSourceButtonState
		(
			kCGEventSourceStateCombinedSessionState, 
			kCGMouseButtonRight
		);
	} else
	if ( vk == VK_MBUTTON )
	{
		ret = CGEventSourceButtonState
		(
			kCGEventSourceStateCombinedSessionState, 
			kCGMouseButtonCenter
		);
	}// else


	return ret;
}

#else  // #ifndef N_POSIX_PLATFORM_MAC

#include "../win32/win/input.c"

#endif // #ifndef N_POSIX_PLATFORM_MAC



typedef struct {

	// main canvas : bitmap

	n_bmp *main;


	// character : bitmap

	n_bmp *chara;


	// background : bitmap or color for n_game_chara_erase()

	n_bmp *bg;
	u32    bgcolor;


	// main canvas : current position / previous position

	n_type_gfx x,y, px,py;


	// character : source position / size / margin

	n_type_gfx srcx,srcy, sx,sy, mx,my;


	// Drag And Drop

	n_posix_bool dnd_onoff;
	n_type_gfx   dnd_ox, dnd_oy;


	// extra data : you can use freely

	u32 data;

} n_game_chara;




#define n_game_chara_zero( c ) n_memory_zero( c, sizeof( n_game_chara ) )

n_posix_inline void
n_game_chara_bmp( n_game_chara *c, n_bmp *main, n_bmp *chara, n_bmp *bg, u32 bgcolor )
{

	c->main    = main;
	c->chara   = chara;
	c->bg      = bg;
	c->bgcolor = bgcolor;


	return;
}

n_posix_inline void
n_game_chara_pos( n_game_chara *c, n_type_gfx x, n_type_gfx y )
{

	c->x = x;
	c->y = y;


	return;
}

n_posix_inline void
n_game_chara_prv( n_game_chara *c )
{

	c->px = c->x;
	c->py = c->y;


	return;
}

n_posix_inline void
n_game_chara_src( n_game_chara *c, n_type_gfx srcx, n_type_gfx srcy, n_type_gfx sx, n_type_gfx sy, n_type_gfx mx, n_type_gfx my )
{

	c->srcx = srcx;
	c->srcy = srcy;
	c->sx   = sx;
	c->sy   = sy;
	c->mx   = mx;
	c->my   = my;


	return;
}

n_posix_inline n_posix_bool
n_game_chara_is_moved( const n_game_chara *c )
{

	if (
		( c->x != c->px )
		||
		( c->y != c->py )
	)
	{
		return n_posix_true;
	}


	return n_posix_false;
}

n_posix_inline void
n_game_chara_erase( const n_game_chara *c )
{

	n_type_gfx fx  = c->px;
	n_type_gfx fy  = c->py;
	n_type_gfx fsx = c->sx;
	n_type_gfx fsy = c->sy;
	n_type_gfx tx  = fx;
	n_type_gfx ty  = fy;

	if ( c->mx ) { fx += c->mx; fsx -= c->mx * 2; tx += c->mx; }
	if ( c->my ) { fy += c->my; fsy -= c->my * 2; ty += c->my; }

	if ( c->bg == NULL )
	{
		n_bmp_box( c->main, fx,fy,fsx,fsy, c->bgcolor );
	} else {
		n_bmp_fastcopy( c->bg, c->main, fx,fy,fsx,fsy, tx,ty );
	}


	return;
}

n_posix_inline void
n_game_chara_draw_full( n_game_chara *c, n_type_real blend, int mirror, int rotate, int edge )
{

	n_game_chara_prv( c );


	const n_posix_bool simple = ( ( mirror == 0 )&&( rotate == 0 )&&( edge == 0 ) );


	n_type_gfx fx  = c->srcx;
	n_type_gfx fy  = c->srcy;
	n_type_gfx fsx = c->sx;
	n_type_gfx fsy = c->sy;
	n_type_gfx tx  = c->x;
	n_type_gfx ty  = c->y;

	if ( rotate == 0 )
	{
		if ( c->mx ) { fx += c->mx; fsx -= c->mx * 2; tx += c->mx; }
		if ( c->my ) { fy += c->my; fsy -= c->my * 2; ty += c->my; }
	} else {
		if ( c->mx ) { fy += c->mx; fsy -= c->mx * 2; ty += c->mx; }
		if ( c->my ) { fx += c->my; fsx -= c->my * 2; tx += c->my; }
	}

	if ( simple )
	{

		n_bmp_blendcopy( c->chara, c->main, fx,fy,fsx,fsy, tx,ty, blend );

	} else {

		if ( mirror & N_BMP_COPY_MIRROR_LEFTSIDE_RIGHT )
		{
			fx = N_BMP_SX( c->chara ) - fx - fsx;
		}

		n_bmp_copy( c->chara, c->main, fx,fy,fsx,fsy, tx,ty, n_posix_true, blend, mirror, rotate, edge );

	}


	return;
}

#define n_game_chara_draw( c ) n_game_chara_draw_simple( c )

n_posix_inline void
n_game_chara_draw_simple( n_game_chara *c )
{

	n_game_chara_prv( c );


	n_type_gfx fx  = c->srcx;
	n_type_gfx fy  = c->srcy;
	n_type_gfx fsx = c->sx;
	n_type_gfx fsy = c->sy;
	n_type_gfx tx  = c->x;
	n_type_gfx ty  = c->y;

	if ( c->mx ) { fx += c->mx; fsx -= c->mx * 2; tx += c->mx; }
	if ( c->my ) { fy += c->my; fsy -= c->my * 2; ty += c->my; }

	n_bmp_transcopy( c->chara, c->main, fx,fy,fsx,fsy, tx,ty );


	return;
}

#define n_game_chara_is_hit( a,b ) n_game_chara_is_hit_offset( a,b, 0,0, 0,0 )

n_posix_inline n_posix_bool
n_game_chara_is_hit_offset
(
	const n_game_chara *a,
	const n_game_chara *b,
	n_type_gfx a_osx, n_type_gfx a_osy,
	n_type_gfx b_osx, n_type_gfx b_osy
)
{

	// [Mechanism]
	//
	//	use n_game_chara.mx/.my for adjustment
	//	you can use *_osx and *_osy for additional margin


	n_type_gfx afx = a->x;
	n_type_gfx atx = a->x + a->sx;
	n_type_gfx bfx = b->x;
	n_type_gfx btx = b->x + b->sx;

	if ( a->mx ) { afx += a->mx; atx -= a->mx; }
	if ( a_osx ) { afx += a_osx; atx -= a_osx; }

	if ( b->mx ) { bfx += b->mx; btx -= b->mx; }
	if ( b_osx ) { bfx += b_osx; btx -= b_osx; }

	if ( afx > btx ) { return n_posix_false; }
	if ( atx < bfx ) { return n_posix_false; }


	n_type_gfx afy = a->y;
	n_type_gfx aty = a->y + a->sy;
	n_type_gfx bfy = b->y;
	n_type_gfx bty = b->y + b->sy;

	if ( a->my ) { afy += a->my; aty -= a->my; }
	if ( a_osy ) { afy += a_osy; aty -= a_osy; }

	if ( b->my ) { bfy += b->my; bty -= b->my; }
	if ( b_osy ) { bfy += b_osy; bty -= b_osy; }

	if ( afy > bty ) { return n_posix_false; }
	if ( aty < bfy ) { return n_posix_false; }


	return n_posix_true;
}

#define n_game_chara_bulk_zero( c, count ) n_memory_zero( c, sizeof(n_game_chara) * (count) )

n_posix_inline void
n_game_chara_bulk_bmp( n_game_chara *c, n_type_int count, n_bmp *main, n_bmp *chara, n_bmp *bg, u32 bgcolor )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_game_chara_bmp( &c[ i ], main, chara, bg, bgcolor );

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

n_posix_inline void
n_game_chara_bulk_pos( n_game_chara *c, n_type_int count, n_type_gfx x, n_type_gfx y )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_game_chara_pos( &c[ i ], x,y );

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

n_posix_inline void
n_game_chara_bulk_src( n_game_chara *c, n_type_int count, n_type_gfx srcx, n_type_gfx srcy, n_type_gfx sx, n_type_gfx sy, n_type_gfx mx, n_type_gfx my )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_game_chara_src( &c[ i ], srcx,srcy, sx,sy, mx,my );

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

n_posix_inline void
n_game_chara_bulk_erase( n_game_chara *c, n_type_int count )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_game_chara_erase( &c[ i ] );

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

n_posix_inline void
n_game_chara_bulk_draw( n_game_chara *c, n_type_int count )
{

	n_type_int i = 0;
	n_posix_loop
	{

		n_game_chara_draw( &c[ i ] );

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

static n_type_int n_game_chara_dnd_refcount = 0;

void
n_game_chara_dnd( n_game_chara *c, HWND hwnd, int vk )
{

	if ( c == NULL ) { return; }

	if ( n_game_chara_dnd_refcount == INT_MAX ) { return; }


	if ( n_win_is_input( vk ) )
	{
//n_win_hwndprintf_literal( hwnd, "Pressed", 0 );

#ifdef N_POSIX_PLATFORM_MAC

		NSPoint pt = n_game_chara_cursor_position;

#else  // #ifdef N_POSIX_PLATFORM_MAC

		POINT pt; GetCursorPos( &pt );
		ScreenToClient( hwnd, &pt );

#endif // #ifdef N_POSIX_PLATFORM_MAC


		n_game_chara cursor;
		n_game_chara_zero( &cursor );

		n_game_chara_pos( &cursor, pt.x,pt.y );
		n_game_chara_src( &cursor, 0,0, 1,1, 0,0 );


		if ( c->dnd_onoff == n_posix_false )
		{

			if ( n_game_chara_dnd_refcount != 0 ) { return; }


			if ( n_game_chara_is_hit( &cursor, c ) )
			{
//n_win_hwndprintf_literal( hwnd, "Hit", 0 );

				n_game_chara_dnd_refcount++;

				c->dnd_onoff = n_posix_true;
				c->dnd_ox    = cursor.x - c->x;
				c->dnd_oy    = cursor.y - c->y;

			}

		} else {

			pt.x -= c->dnd_ox;
			pt.y -= c->dnd_oy;

			n_game_chara_pos( c, pt.x,pt.y );

		}

	} else {
//n_win_hwndprintf_literal( hwnd, "Not Pressed", 0 );

		if ( c->dnd_onoff )
		{

			if ( n_game_chara_dnd_refcount >= 1 ) { n_game_chara_dnd_refcount--; }

			c->dnd_onoff = n_posix_false;
			c->dnd_ox    = 0;
			c->dnd_oy    = 0;

		}

	}


	return;
}


#endif // _H_NONNON_WIN32_GAME_CHARA

