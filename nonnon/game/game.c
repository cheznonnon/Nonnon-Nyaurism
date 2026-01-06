// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_WIN32_GAME
#define _H_NONNON_WIN32_GAME




#include "../neutral/bmp/all.c"

#include "../win32/gdi/dibsection.c"
#include "../win32/win.c"
#include "../win32/win_simplemenu.c"


#include "./direct2d.c"
#include "./helper.c"




typedef struct {


	// [ Needed ] : you need to set at n_game_init()
	//
	//	needpos : initial position is needed
	//	x/y     : window position : default is centering
	//	sx/sy   : canvas size
	//	fps     : frames per sec
	//	color   : default back ground color
	//
	//	[ How to reset the system ? ]
	//
	//	1 : set these parameters
	//	2 : call n_game_reset()

	s32  x,y,sx,sy;
	u32  fps;
	u32  color;


	// [ Optional ]
	//
	//	icon     : an icon resource name, default name is "MAIN_ICON"
	//	ox/oy    : for shaking the screen, use n_game_screenshaker()
	//	esc2exit : an application will close when Esc key is pressed over 5sec.
	//	on_close : call at WM_CLOSE because you cannot use game.hwnd at n_game_exit()
	//	on_event : use as subclass, call DefWindowProc() manually

	n_posix_char *icon;
	s32           ox,oy;
	n_posix_bool  esc2exit;
	WNDPROC       on_close;
	WNDPROC       on_event;

	n_posix_bool  run_on_background;
	n_posix_bool  clip_children;

	n_win_simplemenu *simplemenu;
	n_posix_bool      simplemenu_refresh;


	// [ Internal ] : automatically manage by the game layer
	//
	//	hwnd      : a main window
	//	bmp       : a main canvas
	//	refresh   : painting/resizing request
	//
	//	is_active : is main window active or not
	//	is_closed : WM_CLOSE is called
	//	ime_onoff : is "Input Method Editor" running or not
	//	dwm_onoff : is Desktop Window Manager active or not
	//	fw7_onoff : onoff for fake windows 7 like windowing

	HWND  hwnd;
	n_bmp bmp;
	int   refresh;

	n_posix_bool is_active;
	n_posix_bool is_closed;
	n_posix_bool ime_onoff;
	n_posix_bool dwm_onoff;
	n_posix_bool fw7_onoff;
	n_posix_bool is_oldver;


	// [ Internal ] : cache : don't touch these members

	HBITMAP    hbmp;
	BITMAPINFO bi;

	u32        frame_msec;
	u32         misc_msec;
	u32         loop_msec;
	u32         loop_tmr;


} n_game;


static n_game game;


// [ Needed ]

// [ Mechanism ]
//
//	[ n_game_init() ]
//
//		Nonnon Game Layer will call this...
//		+ before back-buffer (game.bmp) is made
//		+ after a window (game.hwnd) is made
//
//	[ n_game_loop() ]
//
//		you need to call n_game_refresh_on() to redraw
//
//	[ n_game_exit() ]
//
//		Nonnon Game Layer will call this...
//		+ after game.hwnd and game.bmp are freed

extern void n_game_init( void );
extern void n_game_loop( void );
extern void n_game_exit( void );


// [ Optional ]

// [ Mechanism ] : n_game_wake()
//
//	system will call this first
//	usable for commandline options
//	return n_posix_true; for exit immediately

#ifdef N_GAME_WAKE

extern n_posix_bool n_game_wake( void );

#endif // #ifdef N_GAME_WAKE




// [!] : helper functions and macros

#define n_game_title(         str ) n_win_text_set(         game.hwnd, str )
#define n_game_title_literal( str ) n_win_text_set_literal( game.hwnd, str )

#define n_game_hwndprintf(         f, ... ) n_win_hwndprintf(         game.hwnd, f, ##__VA_ARGS__ )
#define n_game_hwndprintf_literal( f, ... ) n_win_hwndprintf_literal( game.hwnd, f, ##__VA_ARGS__ )
#define n_game_debug_count(               ) n_win_debug_count( game.hwnd )


#define N_GAME_REFRESH_OFF    0
#define N_GAME_REFRESH_ON     1
#define N_GAME_REFRESH_RESIZE 2

#define n_game_refresh_off()    game.refresh = N_GAME_REFRESH_OFF
#define n_game_refresh_on()     game.refresh = N_GAME_REFRESH_ON
#define n_game_refresh_resize() game.refresh = N_GAME_REFRESH_RESIZE

#define n_game_refresh_is_off()    ( game.refresh == N_GAME_REFRESH_OFF    )
#define n_game_refresh_is_on()     ( game.refresh == N_GAME_REFRESH_ON     )
#define n_game_refresh_is_resize() ( game.refresh == N_GAME_REFRESH_RESIZE )


#define N_GAME_WINDOW_FIXED     0
#define N_GAME_WINDOW_RESIZABLE 1

#define n_game_window_fixed()     n_game_window( N_GAME_WINDOW_FIXED     )
#define n_game_window_resizable() n_game_window( N_GAME_WINDOW_RESIZABLE )

void
n_game_window( int mode )
{

	const int resizable = WS_OVERLAPPEDWINDOW;
	const int fixed     = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;


	GetSystemMenu( game.hwnd, n_posix_true );


	// [!] : XP or later : UxTheme : n_win_style_del() => _add() combo will misbehave

	DWORD style = n_win_style_get( game.hwnd );

	if ( mode == N_GAME_WINDOW_FIXED )
	{

		style &= ~resizable;
		style |=  fixed;

		//n_win_style_del( game.hwnd, resizable );
		//n_win_style_add( game.hwnd, fixed     );

		n_win_sysmenu_disable( game.hwnd, 0,0,1, 0,1, 0, 0 );

		game.fw7_onoff = n_posix_false;

	} else {

		style &= ~fixed;
		style |=  resizable;

		//n_win_style_del( game.hwnd, fixed     );
		//n_win_style_add( game.hwnd, resizable );

		game.fw7_onoff = n_posix_true;

	}

	n_win_style_new( game.hwnd, style );


	// [Needed] : don't call SetWindowPos()

	n_win_set( game.hwnd, NULL, game.sx,game.sy, N_WIN_SET_DEFAULT );


	return;
}

void
n_game_screenshaker( int range )
{

	if ( range == 0 )
	{
		game.ox = game.oy = range;
	} else {
		game.ox = ( range / 2 ) - n_game_random( range );
		game.oy = ( range / 2 ) - n_game_random( range );
	}


	return;
}

#define n_game_dwm_on()  n_win_dwm_transparent_on(  game.hwnd )
#define n_game_dwm_off() n_win_dwm_transparent_off( game.hwnd )

void
n_game_dwm_onoff( void )
{

	// [Mechanism]
	//
	//	n_game_init() : put after game.color is set
	//
	//	GDI performance will be slow under DWM
	//	use Direct2D to fix it
	//
	//	users can turn off with VisualStyle setting


	// [Patch] : Win10 Technical Preview : DWM window color is borken
	//
	//	build 10130 : caption background will be white
	//	build 10130 : DwmExtendFrameIntoClientArea() works inaccurately
	//	build 10162 : DwmExtendFrameIntoClientArea() works inaccurately
	//	build 10240 : DwmExtendFrameIntoClientArea() works inaccurately


	if (
		( n_win_dwm_is_on() )
		&&
		( n_posix_false == n_win_style_is_classic() )
		&&
		( N_BMP_ALPHA_CHANNEL_INVISIBLE == 0 )
		&&
		( n_posix_false == n_sysinfo_version_8_or_later() )
	)
	{
		game.dwm_onoff = n_posix_true;
		game.color     = 0;
		n_game_dwm_on();
	} else {
		game.dwm_onoff = n_posix_false;
		n_game_dwm_off();
	}


	return;
}

void
n_game_clear( u32 color )
{

	n_bmp_flush( &game.bmp, color );

#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	n_direct2d_copy( game.hwnd, N_BMP_PTR( &game.bmp ),game.sx,game.sy, 0,0 );

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D


	return;
}

n_posix_bool
n_game_cursor_is_moved( void )
{
//return n_posix_true;

	n_posix_bool ret = n_posix_false;

	static POINT pp = { -1, -1 };

	POINT p; GetCursorPos( &p );

	if ( ( pp.x != p.x )||( pp.y != p.y ) )	
	{
		pp.x = p.x;
		pp.y = p.y;
		ret  = n_posix_true;
	}


	return ret;
}




// internal
void
n_game_bmp_exit( void )
{

	n_gdi_dibsection_exit( &game.hbmp, &game.bmp );


	return;
}

// internal
void
n_game_bmp_init( void )
{

	// [!] : anti-fault

	if ( game.sx <= 0 ) { game.sx = 1; }
	if ( game.sy <= 0 ) { game.sy = 1; }


	//n_bmp_new_fast( &game.bmp, game.sx, game.sy );
	n_gdi_dibsection_init( &game.hbmp, &game.bmp, game.hwnd, NULL, game.sx,game.sy );


#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	n_direct2d_make( game.hwnd, N_BMP_PTR( &game.bmp ), game.sx,game.sy );

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D


	n_game_clear( game.color );

	//BITMAPINFO bi = { N_BMP_INFOH( &game.bmp ), { { 0,0,0,0 } } };
	//game.bi = bi;


	return;
}

void
n_game_reset( void )
{

	n_game_bmp_exit();

	game.frame_msec = 1000 / game.fps;
	game.misc_msec  = 0;
	game.loop_msec  = 0;
	game.loop_tmr   = 0;
	game.is_closed  = n_posix_false;

	n_game_bmp_init();


	// [x] : Xp or later : UxTheme : this code doesn't function
	//
	//	SetClassLong( game.hwnd, GCL_HICON, (LONG) LoadIcon( GetModuleHandle( NULL ), game.icon ) );

	n_win_message_send( game.hwnd, WM_SETICON, ICON_BIG, LoadIcon( GetModuleHandle( NULL ), game.icon ) );


//n_posix_debug_literal( "%d %d", game.sx, game.sy );

	n_win win; n_win_zero( &win );
	n_win_set( game.hwnd, &win, game.sx, game.sy, N_WIN_SET_CALCONLY | N_WIN_SET_CENTERING );

	if ( game.x != -1 ) { win.posx = game.x; }
	if ( game.y != -1 ) { win.posy = game.y; }

	n_win_set( game.hwnd, &win, game.sx, game.sy, N_WIN_SET_NEEDPOS | N_WIN_SET_INNERPOS );


	n_game_refresh_on();
	return;
}

// internal
void
n_game_esc2exit( void )
{

	if ( game.esc2exit == n_posix_false ) { return; }


	n_posix_bool is_escape = n_win_is_input( VK_ESCAPE );

	if ( ( game.is_active )&&( game.ime_onoff == n_posix_false )&&( is_escape ) )
	{

		const u32 timeout_max = 0xffffffff;
		const u32 wait_msec   = 5000;

		// [x] : GCC4 complains when use "static u32 timeout = timeout_max;"

		static n_posix_bool is_start = n_posix_false;
		static u32          timeout  = 0xffffffff;

		if ( is_start == n_posix_false )
		{
			is_start = n_posix_true;
			timeout  = n_posix_tickcount() + wait_msec;
		}

		if ( timeout <= n_posix_tickcount() )
		{

			is_start = n_posix_false;
			timeout  = timeout_max;

			if ( ( game.ime_onoff == n_posix_false )&&( is_escape ) )
			{
				n_win_message_send( game.hwnd, WM_CLOSE, 0,0 );
			}

		}

	}


	return;
}

// internal
BOOL CALLBACK
n_game_on_paint_EnumChildProc( HWND hwnd, LPARAM lparam )
{

	s32 sx,sy; n_win_size( hwnd, &sx, &sy );


	HDC hdc = n_gdi_doublebuffer_simple_init( hwnd, sx,sy );

	n_gdi_doublebuffer_simple_fill( GetSysColor( COLOR_WINDOW ) );
	n_win_message_send( hwnd, WM_PRINTCLIENT, hdc, 0 );

	n_gdi_doublebuffer_simple_exit();


	return TRUE;
}

// internal
void
n_game_on_paint( void )
{
//u32 tick = n_posix_tickcount();


	// [!] : XP/Server2003 : 3 times slower than 2000/7
	//
	//	function            |    2000 | Srv2003 |       7 |
	//	SetDIBitsToDevice() |  5 msec | 14 msec |  5 msec |
	//	BitBlt()            |  ? msec |  ? msec | 10 msec |
	//	BitBlt()+DIBSection |  ? msec | 12 msec |  5 msec |


	// [!] : prevent crash at exit : title bar will be gray

	if ( n_direct2d_is_on() )
	{
		if ( n_posix_false == IsWindowVisible( game.hwnd ) )
		{
//n_posix_debug_literal( "" );
			return;
		}
	}


	if ( game.simplemenu_refresh )
	{
		game.simplemenu_refresh = n_posix_false;

		if ( game.simplemenu != NULL )
		{
			n_win_simplemenu_fluent_ui_sync( game.simplemenu, game.hwnd, &game.bmp );
		}
	}


#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	if ( n_direct2d_is_on() )
	{
//n_game_hwndprintf_literal( "Direct2D ON" );

		//n_direct2d_make( game.hwnd, N_BMP_PTR( &game.bmp ), game.sx,game.sy );

		n_direct2d_copy( game.hwnd, N_BMP_PTR( &game.bmp ), game.sx,game.sy, 0,0 );
		n_direct2d_draw( game.hwnd, game.ox,game.oy, game.sx,game.sy );

		if ( game.clip_children )
		{
			HDC hdc = GetDC( game.hwnd );

			EnumChildWindows( game.hwnd, n_game_on_paint_EnumChildProc, (LPARAM) hdc );

			ReleaseDC( game.hwnd, hdc );
		}

	} else

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	{

		HDC hdc = GetDC( game.hwnd );

/*
		SetDIBitsToDevice
		(
			hdc, 
			0,0, game.sx,game.sy, 
			game.ox,game.oy, 0,N_BMP_SY( &game.bmp ), 
			N_BMP_PTR( &game.bmp ),
			&game.bi,
			DIB_RGB_COLORS
		);
*/


		HDC     hdc_compat = CreateCompatibleDC( hdc );
		HBITMAP hbmp_old   = SelectObject( hdc_compat, game.hbmp );

		BitBlt( hdc, 0,0,game.sx,game.sy, hdc_compat, game.ox,game.oy, SRCCOPY );

		SelectObject( hdc_compat, hbmp_old );
		DeleteDC( hdc_compat );

		if ( game.clip_children )
		{
			EnumChildWindows( game.hwnd, n_game_on_paint_EnumChildProc, (LPARAM) hdc );
		}

		ReleaseDC( game.hwnd, hdc );

	}


	// [!] : Win9x/NT4 : flicker prevention

	// [x] : SetCursorPos() has a problem
	//
	//	1 : run OrangeCat
	//	2 : run Nonnon Paint
	//	3 : choose "Grabber"
	//	4 : double click to grab all
	//	5 : then area will be 1x1 always

	// [!] : spell a magic!

	if ( game.is_oldver )
	{
		// [x] : *CursorPos() for physical machines

		// [x] : VirtualPC patch : currently not needed
		//ShowCursor( n_posix_false );
		//ShowCursor(  n_posix_true );

		POINT p; GetCursorPos( &p );
		SetCursorPos( p.x, p.y );
	}


//n_game_hwndprintf_literal( "%d", (int) n_posix_tickcount() - tick );
	return;
}

// internal
void
n_game_on_paint_partial( s32 x, s32 y, s32 sx, s32 sy )
{

	HDC hdc = GetDC( game.hwnd );


#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	if ( n_direct2d_is_on() )
	{
//n_game_hwndprintf_literal( "Direct2D ON" );

		//n_direct2d_make( game.hwnd, N_BMP_PTR( &game.bmp ), game.sx,game.sy );

		n_direct2d_copy( game.hwnd, N_BMP_PTR( &game.bmp ), game.sx,game.sy, 0,0 );
		n_direct2d_draw( game.hwnd, game.ox,game.oy, game.sx,game.sy );

	} else

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	{

		HDC     hdc_compat = CreateCompatibleDC( hdc );
		HBITMAP hbmp_old   = SelectObject( hdc_compat, game.hbmp );

		BitBlt( hdc, x,y,sx,sy, hdc_compat, x,y, SRCCOPY );

		SelectObject( hdc_compat, hbmp_old );
		DeleteDC( hdc_compat );

	}


	ReleaseDC( game.hwnd, hdc );


	return;
}

LRESULT CALLBACK
n_game_wndproc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{

	switch( msg ) {


	case WM_IME_STARTCOMPOSITION :

		game.ime_onoff = n_posix_true;

	break;

	case WM_IME_ENDCOMPOSITION :

		game.ime_onoff = n_posix_false;

	break;


	case WM_ACTIVATE :

		//game.is_active = wparam;

	break;


	case WM_CREATE :

		game.hwnd = hwnd;

		n_win_init_literal( hwnd, "", "", "" );

		ShowWindow( hwnd, SW_HIDE );

	break;

/*
	case WM_PRINTCLIENT :
	{

		HDC     hdc_compat = CreateCompatibleDC( (HDC) wparam );
		HBITMAP hbmp_old   = SelectObject( hdc_compat, game.hbmp );

		BitBlt( (HDC) wparam, 0,0,game.sx,game.sy, hdc_compat, game.ox,game.oy, SRCCOPY );

		SelectObject( hdc_compat, hbmp_old );
		DeleteDC( hdc_compat );

	}
	break;
*/

	case WM_ERASEBKGND :

		return n_posix_true;

	break;

	case WM_PAINT :

		// [!] : multi-threaded rendering

		// [!] : WinXP/2003 : PeekMessage() never send WM_PAINT when WS_OVERLAPPEDWINDOW

		n_game_on_paint();

		// [!] : this causes redraw error with child windows

		//return 0;

	break;

	case WM_SIZE :
	{

		// [!] : don't put this code in WinMain()
		//
		//	1 : WM_SIZE never come
		//	2 : race condition is possible


		// [!] : anti-fault : with n_win_set()

		if ( wparam == SIZE_MINIMIZED ) { break; }


		n_win w;

		n_win_set( game.hwnd, &w, -1,-1, N_WIN_SET_CALCONLY );

//n_game_hwndprintf_literal( "%d : %d-%d", w.state, w.csx,w.csy );


		if (
			( game.sx != w.csx )
			||
			( game.sy != w.csy )
		)
		{

			game.sx = w.csx;
			game.sy = w.csy;

			n_game_bmp_init();
//n_game_clear( 0x00ffff00 );

			n_game_refresh_resize();
			n_game_loop();

			n_game_on_paint();

		}


		// [?] : prevent flickering

		return 0;

	}
	break;


	case WM_SYSCOMMAND :

		if ( wparam == SC_MONITORPOWER )
		{
//n_posix_debug_literal( "SC_MONITORPOWER : %d", lparam );

			if ( lparam == 2 ) { return 0; }

		} else

		if ( wparam == SC_SCREENSAVE )
		{
//n_posix_debug_literal( "SC_SCREENSAVE" );

			return 0;

		}// else

	break;


	case WM_CLOSE :

		ShowWindow( hwnd, SW_HIDE );

		game.is_closed = n_posix_true;

		if ( game.on_close != NULL ) { game.on_close( hwnd, msg, wparam, lparam ); }

		DestroyWindow( hwnd );

	break;

	case WM_DESTROY :

		PostQuitMessage( 0 );

	break;


	} // switch


	if ( game.fw7_onoff )
	{
		LRESULT ret = 0;
		if ( n_win_fake_win7_proc( hwnd, msg, wparam, lparam, &ret ) )
		{
			return ret;
		}
	}


	n_win_mbutton2centering_proc( hwnd, msg, wparam, lparam );


	if ( game.on_event != NULL )
	{
		return game.on_event( hwnd, msg, wparam, lparam );
	} else {
		return DefWindowProc( hwnd, msg, wparam, lparam );
	}
}

#ifdef N_GAME_NO_WINMAIN
int
n_game_main( void )
#else  // #ifdef N_GAME_NO_WINMAIN
int WINAPI
WinMain( HINSTANCE hinst, HINSTANCE hprv, LPSTR cmd, int show )
#endif // #ifdef N_GAME_NO_WINMAIN
{

#ifdef N_GAME_WAKE

	if ( n_game_wake() ) { return 0; }

#endif // #ifdef N_GAME_WAKE


#ifdef _H_NONNON_GAME_TIMEGETTIME

//n_posix_debug_literal( "Timer Enabled" );
	n_game_timegettime_init();

#endif // #ifdef _H_NONNON_GAME_TIMEGETTIME


	// [!] : Win2000/Server2003/7RC : this always returns 20

//n_posix_debug_literal( "%d", (int) GdiGetBatchLimit() );


	// Initialization


	UINT msg_mousewheel = RegisterWindowMessage( n_posix_literal( "MSWHEEL_ROLLMSG" ) );	


#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	if ( n_win_dwm_is_on() ) { n_direct2d_init(); }

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D


	n_random_shuffle();

	n_win_ghostwindow_disable();

	n_win_dpi_autoscale_disable();

	n_win_exedir2curdir();


	// Window

	n_memory_zero( &game, sizeof( n_game ) );

	game.x        =  -1;
	game.y        =  -1;
	game.sx       = 320;
	game.sy       = 240;
	game.fps      =  30;
	game.icon     = n_posix_literal( "MAIN_ICON" );
	game.esc2exit = n_posix_true;


	if ( n_sysinfo_version_9x () ) { game.is_oldver = n_posix_true; }
	if ( n_sysinfo_version_nt4() ) { game.is_oldver = n_posix_true; }


	n_win_gui( NULL, N_WIN_GUI_WINDOW, n_game_wndproc, &game.hwnd );


	DWORD style = n_win_style_get( game.hwnd );

	n_game_init();
	n_game_reset();

	if ( style == n_win_style_get( game.hwnd ) ) { n_game_window_fixed(); }


	// Start Game Loop

	n_game_loop();
	//n_win_message_send( game.hwnd, WM_PAINT, 0,0 );

	ShowWindowAsync( game.hwnd, SW_NORMAL );

	n_posix_bool sleep_onoff = n_posix_true;

	MSG msg;
	n_posix_loop
	{

		sleep_onoff = n_posix_true;


		// [x] : buggy : don't use GetFocus() and GetActiveWindow()
		//
		//	n_posix_true is returned when an inactive window

		game.is_active = ( game.hwnd == GetForegroundWindow() );
		game.misc_msec = n_posix_tickcount();


		// Forced Shutdown : PostMessage() may cause busy

		n_game_esc2exit();


		if ( n_win_message_peek( &msg ) )
		{

			if ( msg.message == WM_QUIT ) { break; }

			n_win_mousewheel_redirector( msg_mousewheel, &msg );

			TranslateMessage( &msg );
			DispatchMessage ( &msg );

			sleep_onoff = n_posix_false;

		}


		n_posix_bool frame_onoff =
		(
			(
				( game.run_on_background )
				||
				( game.is_active )
			)
			&&
			(
				( n_game_refresh_is_resize() )
				||
				( n_game_timer( &game.loop_tmr, game.loop_msec ) )
			)
		);


		game.misc_msec = n_posix_tickcount() - game.misc_msec;


		if ( frame_onoff )
		{

			game.loop_msec = n_posix_tickcount();

			n_game_loop();

			game.loop_msec = n_posix_tickcount() - game.loop_msec;


			game.loop_msec += game.misc_msec;

			if ( game.frame_msec > game.loop_msec )
			{
				game.loop_msec = game.frame_msec - game.loop_msec;
			} else {
				game.loop_msec = 0;
			}


			if ( game.refresh )
			{

				n_game_refresh_off();

				// [Mechanism] : slower to faster
				//
				//	1 : InvalidateRect()
				//	2 : n_game_on_paint()
				//	3 : SendMessage()
				//	4 : PostMessage()        : UI response will be slowdown 
				//	5 : SendMessageTimeout()

				SendMessageTimeout( game.hwnd, WM_PAINT, 0,0, SMTO_ABORTIFHUNG, game.loop_msec, NULL );

			}

		}


		if ( sleep_onoff )
		{
			n_posix_sleep( 1 );
		}


	} // n_posix_loop


	// Cleanup

	n_game_bmp_exit();

	n_game_exit();


#ifdef _H_NONNON_WIN32_GAME_DIRECT2D

	n_direct2d_exit();

#endif // #ifdef _H_NONNON_WIN32_GAME_DIRECT2D


#ifdef _H_NONNON_GAME_TIMEGETTIME

	n_game_timegettime_exit();

#endif // #ifdef _H_NONNON_GAME_TIMEGETTIME


	n_memory_debug_refcount();


	return (int) msg.wParam;
}


#endif // _H_NONNON_WIN32_GAME

