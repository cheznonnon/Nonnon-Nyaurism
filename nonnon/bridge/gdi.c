// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_BRIDGE_GDI
#define _H_NONNON_BRIDGE_GDI




#ifdef N_POSIX_PLATFORM_WINDOWS


#include "../neutral/ini.c"

#include "../neutral/txt.c"

#include "../bridge/pmr.c"
#include "../bridge/progressbar.c"


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


#include "../neutral/ini.c"

#include "../mac/image.c"

#include "../mac/window.c"
#include "../mac/n_txtbox/00_codec.c"


// [x] : Debug "Main Thread Checker" : NSWindow.backingScaleFactor is main thread only
//
//	please set at awakeFromNib

static CGFloat n_gdi_scale_factor = 1.0;


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#ifdef N_POSIX_PLATFORM_WINDOWS

void
n_gdi_stdsize_text( HWND hwnd, const n_posix_char *str, n_type_gfx *sx, n_type_gfx *sy )
{

	// [Mechanism]
	//
	//	WM_GETFONT returns real hfont, not copy of it
	//	WM_GETFONT returns NULL when system font
	//	so don't n_win_font_exit() with it


	// [Needed] : multi-thread

	HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_stdsize_text()" );


	HDC   hdc = GetDC( hwnd );
	HFONT pf  = SelectObject( hdc, (HFONT) SendMessage( hwnd, WM_GETFONT, 0, 0 ) );

	SIZE size;
	GetTextExtentPoint32( hdc, str, (int) n_posix_strlen( str ), &size );

	SelectObject( hdc, pf );
	ReleaseDC( hwnd, hdc );


	hmutex = n_thread_mutex_exit( hmutex );


	if ( sx != NULL ) { (*sx) = size.cx; }
	if ( sy != NULL ) { (*sy) = size.cy; }


	return;
}

#endif




void
n_gdi_crop( n_bmp *bmp, n_bmp *bmp_Aj )
{

	if ( n_bmp_error( bmp    ) ) { return; }
	if ( n_bmp_error( bmp_Aj ) ) { return; }


	n_type_gfx bmpsx = N_BMP_SX( bmp );
	n_type_gfx bmpsy = N_BMP_SY( bmp );


	n_type_gfx fx = 0;
	n_type_gfx tx = 0;
	n_type_gfx fy = bmpsx;
	n_type_gfx ty = bmpsy;


	n_type_gfx x,y;


	// [!] : Left
	
	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != n_bmp_black ) { fx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x++;
			if ( x >= bmpsx ) { break; }
		}
	}

	// [!] : Top
	
	x = y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp_Aj, x,y, &color );
		if ( color != n_bmp_black ) { fy = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}

	// [!] : Right
	
	x = bmpsx - 1;
	y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( color != n_bmp_black ) { tx = x; break; }

		y++;
		if ( y >= bmpsy )
		{
			y = 0;

			x--;
			if ( x < 0 ) { break; }
		}
	}

	// [!] : Bottom
	
	x = 0;
	y = bmpsy - 1;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp_Aj, x,y, &color );
		if ( color != n_bmp_black ) { ty = y; break; }

		x++;
		if ( x >= bmpsx )
		{
			x = 0;

			y--;
			if ( y < 0 ) { break; }
		}
	}

	if ( fx >     0 ) { fx--; }
	if ( fy >     0 ) { fy--; }
	if ( tx < bmpsx ) { tx++; }
	if ( ty < bmpsx ) { ty++; }

	n_type_gfx fsx = tx - fx; if ( fsx < 0 ) { return; }
	n_type_gfx fsy = ty - fy; if ( fsy < 0 ) { return; }

	n_bmp ret; n_bmp_zero( &ret ); n_bmp_new( &ret, fsx, fsy );

	n_bmp_fastcopy( bmp, &ret, fx,fy,fsx,fsy, 0,0 );

	n_bmp_free( bmp );
	n_bmp_alias( &ret, bmp );


	return;
}




#define N_GDI_DEFAULT            ( 0 << 0 )
#define N_GDI_GRAY               ( 1 << 0 )
#define N_GDI_CLARITY            ( 1 << 1 )
#define N_GDI_NOMARGIN           ( 1 << 2 )
#define N_GDI_AUTOMARGIN         ( 1 << 3 )
#define N_GDI_TEXTLOADER         ( 1 << 4 )
#define N_GDI_CALCONLY           ( 1 << 5 )
#define N_GDI_PRESSED            ( 1 << 6 )
#define N_GDI_SYSTEMCOLOR        ( 1 << 7 )
#define N_GDI_LAST               N_GDI_SYSTEMCOLOR
#define N_GDI_SHADOW             ( N_GDI_LAST << 1 )
#define N_GDI_SHADOW_FOG         ( N_GDI_LAST << 2 )
#define N_GDI_CONTOUR            ( N_GDI_LAST << 3 )
#define N_GDI_CONTOUR_FOG        ( N_GDI_LAST << 4 )
#define N_GDI_SINK               ( N_GDI_LAST << 5 )
#define N_GDI_SMOOTH             ( N_GDI_LAST << 6 )

#define N_GDI_LAYOUT_HORIZONTAL  0
#define N_GDI_LAYOUT_VERTICAL    1

#define N_GDI_ALIGN_CENTER       ( 0 << 0 )
#define N_GDI_ALIGN_LEFT         ( 1 << 0 )
#define N_GDI_ALIGN_RIGHT        ( 1 << 1 )
#define N_GDI_ALIGN_TOP          ( 1 << 2 )
#define N_GDI_ALIGN_BOTTOM       ( 1 << 3 )

#define N_GDI_BASE_DEFAULT       0
#define N_GDI_BASE_SOLID         0
#define N_GDI_BASE_VERTICAL      1
#define N_GDI_BASE_HORIZONTAL    2
#define N_GDI_BASE_RECTANGLE     3
#define N_GDI_BASE_CIRCLE        4
#define N_GDI_BASE_POLKADOT      5
#define N_GDI_BASE_POLKADOT_POP  6
#define N_GDI_BASE_POLKADOT_RND  7
#define N_GDI_BASE_POLKADOT_FOAM 8
#define N_GDI_BASE_RECT_TILE     9
#define N_GDI_BASE_GINGHAM      10
#define N_GDI_BASE_TARTAN       11
#define N_GDI_BASE_STRIPE       12
#define N_GDI_BASE_CHECKER      13
#define N_GDI_BASE_LUNA         14
#define N_GDI_BASE_LUNA_PRESS   15
#define N_GDI_BASE_IMAGE        16
#define N_GDI_BASE_IMAGE_TILE   17
#define N_GDI_BASE_PROGRESS_H   18
#define N_GDI_BASE_PROGRESS_V   19

#define N_GDI_FRAME_DEFAULT      0
#define N_GDI_FRAME_NOFRAME      0
#define N_GDI_FRAME_TRANS        1
#define N_GDI_FRAME_SIMPLE       2
#define N_GDI_FRAME_FLAT         3
#define N_GDI_FRAME_BUTTON       4
#define N_GDI_FRAME_SCROLLARROW  5
#define N_GDI_FRAME_PUSH         6
#define N_GDI_FRAME_SINK         7
#define N_GDI_FRAME_ETCH         8
#define N_GDI_FRAME_LUNA         9
#define N_GDI_FRAME_ROUND       10
#define N_GDI_FRAME_RPG         11
#define N_GDI_FRAME_AQUA        12

#define N_GDI_FONT_DEFAULT       ( 0 << 0 )
#define N_GDI_FONT_BOLD          ( 1 << 0 )
#define N_GDI_FONT_ITALIC        ( 1 << 1 )
#define N_GDI_FONT_UNDERLINE     ( 1 << 2 )
#define N_GDI_FONT_STRIKEOUT     ( 1 << 3 )
#define N_GDI_FONT_MONOSPACE     ( 1 << 4 )
#define N_GDI_FONT_MONOSPACE_2   ( 1 << 5 )

#define N_GDI_TEXT_DEFAULT       N_GDI_FONT_DEFAULT
#define N_GDI_TEXT_BOLD          N_GDI_FONT_BOLD
#define N_GDI_TEXT_ITALIC        N_GDI_FONT_ITALIC
#define N_GDI_TEXT_UNDERLINE     N_GDI_FONT_UNDERLINE
#define N_GDI_TEXT_STRIKEOUT     N_GDI_FONT_STRIKEOUT
#define N_GDI_TEXT_MONOSPACE     N_GDI_FONT_MONOSPACE
#define N_GDI_TEXT_MONOSPACE_2   N_GDI_FONT_MONOSPACE_2
#define N_GDI_TEXT_LAST          N_GDI_TEXT_MONOSPACE_2
#define N_GDI_TEXT_SHADOW        ( N_GDI_TEXT_LAST <<  1 )
#define N_GDI_TEXT_SHADOW_FOG    ( N_GDI_TEXT_LAST <<  2 )
#define N_GDI_TEXT_CONTOUR       ( N_GDI_TEXT_LAST <<  3 )
#define N_GDI_TEXT_CONTOUR_FOG   ( N_GDI_TEXT_LAST <<  4 )
#define N_GDI_TEXT_SINK          ( N_GDI_TEXT_LAST <<  5 )
#define N_GDI_TEXT_SMOOTH        ( N_GDI_TEXT_LAST <<  6 )
#define N_GDI_TEXT_CLEAR         ( N_GDI_TEXT_LAST <<  7 )
#define N_GDI_TEXT_GRADIENT      ( N_GDI_TEXT_LAST <<  8 )
#define N_GDI_TEXT_ELLIPSIS      ( N_GDI_TEXT_LAST <<  9 )
#define N_GDI_TEXT_PRINTER       ( N_GDI_TEXT_LAST << 10 )
#define N_GDI_TEXT_NO_MARGIN     ( N_GDI_TEXT_LAST << 11 )
#define N_GDI_TEXT_MAC_NO_CROP   ( N_GDI_TEXT_LAST << 12 )
#define N_GDI_TEXT_MAC_BASELINE  ( N_GDI_TEXT_LAST << 13 )
#define N_GDI_TEXT_POP           ( N_GDI_TEXT_LAST << 14 )

#define N_GDI_ICON_DEFAULT       N_GDI_TEXT_DEFAULT
#define N_GDI_ICON_SHADOW        N_GDI_TEXT_SHADOW
#define N_GDI_ICON_SHADOW_FOG    N_GDI_TEXT_SHADOW_FOG
#define N_GDI_ICON_CONTOUR       N_GDI_TEXT_CONTOUR
#define N_GDI_ICON_CONTOUR_FOG   N_GDI_TEXT_CONTOUR_FOG
#define N_GDI_ICON_SINK          N_GDI_TEXT_SINK
#define N_GDI_ICON_SMOOTH        N_GDI_TEXT_SMOOTH
#define N_GDI_ICON_LAST          N_GDI_ICON_SMOOTH
#define N_GDI_ICON_IMAGELOADER   ( N_GDI_ICON_LAST << 1 )
#define N_GDI_ICON_NOFINALIZE    ( N_GDI_ICON_LAST << 2 )
#define N_GDI_ICON_FASTMODE      ( N_GDI_ICON_LAST << 3 )
#define N_GDI_ICON_STRETCH       ( N_GDI_ICON_LAST << 4 )
#define N_GDI_ICON_RESOURCE      ( N_GDI_ICON_LAST << 5 )
#define N_GDI_ICON_RC_RESOLVE    ( N_GDI_ICON_LAST << 6 )
#define N_GDI_ICON_SYSTEM        ( N_GDI_ICON_LAST << 7 )




// [!] : auto calculation

#define N_GDI_SCALE_AUTO         ( -10 ) // [!] : for debugging


// [!] : use for placeholder

#define N_GDI_ICON_NAME_EMPTY    n_posix_literal( "?" )




typedef struct {

	// In

	n_type_gfx    sx,sy;
	n_type_gfx    scale;
	int           style;
	int           layout;
	int           align;


	n_posix_char *base;
	n_type_gfx    base_index;
	u32           base_color_bg;
	u32           base_color_fg;
	int           base_style;
	n_type_gfx    base_unit;
	n_type_gfx    base_polkadot_margin;
	n_type_gfx    base_gap_between_icon_and_text;

	int           frame_style;
	BOOL          frame_is_hot;

	n_posix_char *icon;
	n_type_gfx    icon_index;
	int           icon_bpp;
	int           icon_rsrc;
	int           icon_style;
	u32           icon_color_bg;
	u32           icon_color_shadow;
	u32           icon_color_contour;
	u32           icon_color_sink_tl;
	u32           icon_color_sink_br;
	n_type_gfx    icon_fxsize1;
	n_type_gfx    icon_fxsize2;
	n_posix_char *icon_clip;
	n_type_gfx    icon_clip_x;
	n_type_gfx    icon_clip_y;

	n_posix_char *text;
	n_posix_char *text_font;
	n_type_gfx    text_size;
	int           text_style;
	u32           text_color_main;
	u32           text_color_gradient;
	u32           text_color_shadow;
	u32           text_color_contour;
	u32           text_color_sink_tl;
	u32           text_color_sink_br;
	n_type_gfx    text_fxsize1;
	n_type_gfx    text_fxsize2;

	BOOL          debug_output;


	// In/Out
	//
	//	icon_sx, icon_sy   : zero means "auto calculation"
	//	frame_round        : only in N_GDI_FRAME_LUNA, _ROUND, _RPG and _AQUA
	//	frame_corner_color : corner color
	//	percent            : use N_GDI_BASE_PROGRESS_H/V

	n_type_gfx    icon_sx, icon_sy;
	n_type_gfx    frame_round;
	u32           frame_corner_color;
	int           percent;


	// Out

	n_type_gfx    offset_x, offset_y;
	n_type_gfx    icon_x, icon_y;
	n_type_gfx    text_x, text_y, text_sx, text_sy;
	n_type_gfx    frame_size;
	n_type_gfx    effect_margin;
	n_type_gfx    icon_loaded_bpp, icon_loaded_size;


	// Internal

#ifdef N_POSIX_PLATFORM_WINDOWS

	BOOL          pmr_onoff;

	n_type_int   *text_cache_len;
	SIZE         *text_cache_size;
	SIZE          text_cache_unit;
	HFONT         text_cache_hfont;
	n_type_real   text_cache_ratio;

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

} n_gdi;




// Constants

const n_type_gfx n_gdi_smoothness = 5;


// [!] : not beautiful but GDI-independent

const BOOL n_gdi_fakebold_onoff = FALSE;


// [!] : currently hidden option

static BOOL n_gdi_gradient_vertical_up_side_down = FALSE;




// Components

#define n_gdi_zero(  f    ) n_memory_zero( f,    sizeof( n_gdi ) )
#define n_gdi_alias( f, t ) n_memory_copy( f, t, sizeof( n_gdi ) )




#ifdef N_POSIX_PLATFORM_WINDOWS

#include "./gdi/base.c"
#include "./gdi/effect.c"
#include "./gdi/font.c"
#include "./gdi/frame.c"
#include "./gdi/text.c"

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

#include "./gdi/base.c"
#include "./gdi/effect.c"
#include "./gdi/frame.c"

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#define n_gdi_base_load( gdi, icon ) n_gdi_image_load( gdi, icon, gdi->base )

BOOL
n_gdi_image_load( const n_gdi *gdi, n_bmp *icon, n_posix_char *path )
{

	if (
		( n_bmp_load   ( icon, path ) )
#ifdef _H_NONNON_NEUTRAL_PNG
		&&
		( n_png_png2bmp( path, icon ) )
#endif // #ifdef _H_NONNON_NEUTRAL_PNG
#ifdef _H_NONNON_NEUTRAL_JPG
		&&
		( n_jpg_jpg2bmp( path, icon ) )
#endif // #ifdef _H_NONNON_NEUTRAL_JPG
#ifdef _H_NONNON_WIN32_OLE_IPICTURE
		&&
		( n_IPicture_load( path, icon ) )
#endif // #ifdef _H_NONNON_WIN32_OLE_IPICTURE
#ifdef _H_NONNON_WIN64_RESOURCE
		&&
		( n_win64_resource_load_nbmp( path, icon ) )
#endif // #ifdef _H_NONNON_WIN64_RESOURCE
	)
	{
		if ( n_bmp_error( icon ) ) { return TRUE; }
	} else {
		//return FALSE;
	}


	return FALSE;
}

void
n_gdi_bmp( n_gdi *gdi, n_bmp *bmp )
{

	// [Needed] : n_gdi_zero() and n_bmp_zero()


	if ( gdi == NULL ) { return; }


	if ( gdi->sx < 0 ) { return; }
	if ( gdi->sy < 0 ) { return; }




	// Phase 1 : Initialization


	// [Needed] : multi-thread : you need to manage this

	BOOL p_trans_onoff = n_bmp_transparent_onoff_default;
	if ( n_bmp_is_multithread == FALSE )
	{
		n_bmp_transparent_onoff_default = TRUE;
	}


	BOOL      onoff = ( FALSE == ( gdi->style & N_GDI_CALCONLY ) );
	BOOL icon_onoff = ( FALSE == n_string_is_empty( gdi->icon ) );
	BOOL text_onoff = ( FALSE == n_string_is_empty( gdi->text ) );
//NSLog( @"%s", gdi->text );

	if ( ( onoff )&&( bmp == NULL ) ) { return; }


	// [!] : use INI for Mac support

	n_ini txt; if ( text_onoff ) { n_ini_zero( &txt ); }


	// Override

	if ( gdi->scale == N_GDI_SCALE_AUTO )
	{
#ifdef N_POSIX_PLATFORM_WINDOWS

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_bmp()" );


		HDC hdc = GetDC( NULL );

		gdi->scale = GetDeviceCaps( hdc, LOGPIXELSX ) / 96;

		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
	} else {
		gdi->scale = n_posix_max_n_type_gfx( 1, gdi->scale );
	}

	if ( gdi->style & N_GDI_SHADOW )
	{
		gdi->icon_style |= N_GDI_ICON_SHADOW;
		gdi->text_style |= N_GDI_TEXT_SHADOW;
	}

	if ( gdi->style & N_GDI_SHADOW_FOG )
	{
		gdi->icon_style |= N_GDI_ICON_SHADOW_FOG;
		gdi->text_style |= N_GDI_TEXT_SHADOW_FOG;
	}

	if ( gdi->style & N_GDI_CONTOUR )
	{
		gdi->icon_style |= N_GDI_ICON_CONTOUR;
		gdi->text_style |= N_GDI_TEXT_CONTOUR;
	}

	if ( gdi->style & N_GDI_CONTOUR_FOG )
	{
		gdi->icon_style |= N_GDI_ICON_CONTOUR_FOG;
		gdi->text_style |= N_GDI_TEXT_CONTOUR_FOG;
	}

	if ( gdi->style & N_GDI_SINK )
	{
		gdi->icon_style |= N_GDI_ICON_SINK;
		gdi->text_style |= N_GDI_TEXT_SINK;
	}

	if ( gdi->style & N_GDI_SMOOTH )
	{
		gdi->icon_style |= N_GDI_ICON_SMOOTH;
		gdi->text_style |= N_GDI_TEXT_SMOOTH;
	}


	// Adjustment

	if ( gdi->base_unit <= 0 )
	{
		gdi->base_unit = 64;
	}

	if ( ( n_gdi_fakebold_onoff )&&( gdi->text_style & N_GDI_TEXT_BOLD ) )
	{
		gdi->text_style |= N_GDI_TEXT_SMOOTH;
	}

	if ( gdi->text_font == NULL )
	{
		gdi->text_font = N_STRING_EMPTY;
	}

	if ( gdi->text_style & N_GDI_TEXT_CLEAR )
	{
		gdi->text_style |= N_GDI_TEXT_SMOOTH;
	}

	if ( gdi->icon_bpp <= 0 )
	{
		gdi->icon_bpp = 32;
	}

	if ( gdi->text_style & N_GDI_TEXT_PRINTER )
	{
		gdi->text_style &= ~N_GDI_TEXT_SMOOTH;
	}

	gdi->base_polkadot_margin = gdi->base_unit + gdi->base_polkadot_margin;

#ifndef N_POSIX_PLATFORM_WINDOWS

	NSFont *nsfont;
	{
		NSString *font_name;
		if ( n_string_is_empty( gdi->text_font ) )
		{
			font_name = @"Helvetica";
		} else {
			font_name = n_mac_str2nsstring( gdi->text_font );
		}


		CGFloat scale = n_gdi_scale_factor;
//NSLog( @"%f", scale );

		CGFloat font_size = gdi->text_size / scale;

		if (
			( gdi->text_style & N_GDI_TEXT_BOLD )
			||
			( gdi->text_style & N_GDI_TEXT_ITALIC )
		)
		{
			NSUInteger option = 0;
			if ( gdi->text_style & N_GDI_TEXT_BOLD )
			{
				option |= NSBoldFontMask;
			}

			if ( gdi->text_style & N_GDI_TEXT_ITALIC )
			{
				option |= NSItalicFontMask;
			}

			NSFontManager *fontManager = [NSFontManager sharedFontManager];
			nsfont = [fontManager
				fontWithFamily:font_name
				        traits:option
					weight:0
				          size:font_size
			];
		} else {
			nsfont = [NSFont fontWithName:font_name size:font_size];
		}

		if ( nsfont == nil )
		{
			font_name = @"Helvetica";
			nsfont = [NSFont fontWithName:font_name size:font_size];
		}
	}

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS

	gdi->base_gap_between_icon_and_text = 0;
	if ( ( icon_onoff )&&( text_onoff ) )
	{
		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{
#ifdef N_POSIX_PLATFORM_WINDOWS
			n_gdi_stdsize_text( NULL, n_posix_literal( " " ), &gdi->base_gap_between_icon_and_text, NULL );
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
			CGSize sz = n_mac_image_text_pixelsize( @" ", nsfont );
			gdi->base_gap_between_icon_and_text = sz.width;
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
		}
	}


	// Frame Border

	if ( gdi->frame_style == N_GDI_FRAME_LUNA  )
	{
		gdi->frame_round = n_posix_max_n_type_gfx( 3 * gdi->scale, gdi->frame_round );
		gdi->frame_size  = n_posix_max_n_type_gfx( 1 * gdi->scale, gdi->frame_size  );
	} else
	if ( gdi->frame_style == N_GDI_FRAME_ROUND )
	{
		gdi->frame_round = n_posix_max_n_type_gfx( 4 * gdi->scale, gdi->frame_round );
		gdi->frame_size  = 2 * gdi->scale;
	} else
	if ( gdi->frame_style == N_GDI_FRAME_RPG   )
	{
		gdi->frame_round = n_posix_max_n_type_gfx( 2 * gdi->scale, gdi->frame_round );
		gdi->frame_size  = n_posix_max_n_type_gfx( 2 * gdi->scale, gdi->frame_size  );
	} else
	if ( gdi->frame_style == N_GDI_FRAME_AQUA  )
	{
		gdi->frame_round = -100;
		gdi->frame_size  = 2 * gdi->scale;
	} else
	if ( gdi->frame_style )
	{
		gdi->frame_round = 0;
		gdi->frame_size  = 2 * gdi->scale;
	} else {
		gdi->frame_round = 0;
		gdi->frame_size  = 0;
	}

	if ( gdi->text_style & N_GDI_TEXT_POP )
	{
		gdi->frame_size  = 4 * gdi->scale;
	}


	// PMR Support

#ifdef N_POSIX_PLATFORM_MINGW

	if ( n_posix_stat_is_dir( gdi->text_font ) ) { gdi->pmr_onoff = TRUE; }

	if ( gdi->pmr_onoff )
	{
		// [!] : not supported
		if ( gdi->text_style & N_GDI_TEXT_SMOOTH )
		{
			gdi->text_style &= ~N_GDI_TEXT_SMOOTH;
		}
	}

#endif // #ifdef N_POSIX_PLATFORM_MINGW




	// Phase 2 : Size Calculation


	n_type_gfx sx = 0;
	n_type_gfx sy = 0;

	if ( icon_onoff )
	{

		// [!] : overridable

//n_posix_debug_literal( " %d ", gdi->icon_rsrc );

		if ( gdi->icon_rsrc <= 0 )
		{
			n_type_gfx icon_sx = 32;
			n_type_gfx icon_sy = 32;
		
#ifdef N_POSIX_PLATFORM_MINGW
			n_win_stdsize_icon_large( &icon_sx, &icon_sy );
#endif // #ifdef N_POSIX_PLATFORM_MINGW

			gdi->icon_rsrc = n_posix_max_n_type_gfx( icon_sx, icon_sy );

			gdi->icon_sx = n_posix_max_n_type_gfx( gdi->icon_sx, icon_sx );
			gdi->icon_sy = n_posix_max_n_type_gfx( gdi->icon_sy, icon_sy );

		} else {
//gdi->icon_rsrc = 256;
			if ( gdi->icon_sx <= 0 ) { gdi->icon_sx = gdi->icon_rsrc; }
			if ( gdi->icon_sy <= 0 ) { gdi->icon_sy = gdi->icon_rsrc; }
		}

//n_posix_debug_literal( " %d ", gdi->icon_rsrc );

		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{
			sx = sx + gdi->icon_sx;
			sy = n_posix_max_n_type_gfx( sy, gdi->icon_sy );
		} else
		if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
		{
			sx = n_posix_max_n_type_gfx( sx, gdi->icon_sx );
			sy = sy + gdi->icon_sy;
		}

//NSLog( @"%d %d", gdi->icon_sy, sy );
	}

#ifdef N_POSIX_PLATFORM_MAC

	n_bmp bmp_text; n_bmp_zero( &bmp_text ); 

#endif // #ifdef N_POSIX_PLATFORM_MAC

	if ( text_onoff )
	{

		if (
			( FALSE == ( gdi->style & N_GDI_TEXTLOADER ) )
			||
			( n_ini_load( &txt, gdi->text ) )
		)
		{
//NSLog( @"!" );
			// [!] : this code is single line only

			//n_ini_new( &txt );
			//n_ini_mod( &txt, 0, gdi->text );


			// [!] : don't free( s );

			n_posix_char *s = n_string_carboncopy( gdi->text );
			n_type_int    cb = n_string_cch2cb( s, n_posix_strlen( s ) );

//n_posix_debug_literal( "%s %d", s, cb );

			// [!] : don't use n_txt_load_onmemory()

			n_vector_load_onmemory( (void*) &txt, s, cb );

//n_memory_free( s );
//n_posix_debug_literal( "%s", txt.line[ 0 ] );
		}


		if ( gdi->frame_style == N_GDI_FRAME_AQUA )
		{
			gdi->sx -= gdi->sy;
		}

#ifdef N_POSIX_PLATFORM_WINDOWS

		n_gdi_text_draw( gdi, NULL, (void*) &txt, &gdi->text_sx, &gdi->text_sy );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

//NSLog( @"%s", gdi->text );

		n_posix_char *text;
		if ( n_ini_error( &txt ) )
		{
			text = gdi->text;
		} else {
			text = txt.stream;
		}

		NSString *s = n_mac_str2nsstring( text );
//NSLog( @"%@", s );


		// [!] : "Bounding Rect" does not contain descender like "g" "y", and width like "j"
		//
		//	HiDPI 1x : 144 : 126.720257 -17.280035 111.024000
		//	HiDPI 2x : 126 :  55.440112 - 7.560015  48.573000
//NSLog( @"%d : %f %f %f", gdi->text_size, nsfont.ascender, nsfont.descender, nsfont.capHeight );

		CGSize     sz = n_mac_image_text_pixelsize( s, nsfont );
		n_type_gfx  o = n_posix_max_n_type_gfx( gdi->text_fxsize1, gdi->text_fxsize2 ) * 2;


		n_type_gfx mult = 2;
		if ( gdi->text_style & N_GDI_TEXT_MAC_NO_CROP ) { mult = 1; }

		n_bmp_new( &bmp_text, sz.width * mult, sz.height * mult );

		NSImage  *img = n_mac_image_nbmp2nsimage( &bmp_text );
		NSString *str = n_mac_str2nsstring( text );
		NSColor  *clr = [NSColor whiteColor];

		n_mac_image_text_native( img, str, nsfont, clr, 0,0 );
		n_mac_image_nsimage2nbmp( img, &bmp_text );
//n_bmp_save( &bmp_text, "/Users/nonnon/Desktop/ret.bmp" );

		if ( gdi->text_style & N_GDI_TEXT_MAC_NO_CROP )
		{
			//
		} else
		if ( gdi->text_style & N_GDI_TEXT_MAC_BASELINE )
		{
			n_bmp bmp_Aj; n_bmp_zero( &bmp_Aj ); n_bmp_new( &bmp_Aj, sz.width * 2, sz.height * 2 );

			NSImage  *img = n_mac_image_nbmp2nsimage( &bmp_Aj );
			NSString *str = @"Aj";
			NSColor  *clr = [NSColor whiteColor];

			n_mac_image_text_native( img, str, nsfont, clr, 0,0 );
			n_mac_image_nsimage2nbmp( img, &bmp_Aj );

			n_gdi_crop( &bmp_text, &bmp_Aj );
			n_bmp_free_fast( &bmp_Aj );
		} else {
			n_gdi_crop( &bmp_text, &bmp_text );
		}

//if ( gdi->debug_output ) { n_bmp_save( &bmp_text, "/Users/nonnon2/Desktop/ret.bmp" ); }

		gdi->text_sx = N_BMP_SX( &bmp_text ) + o;
		gdi->text_sy = N_BMP_SY( &bmp_text ) + o;
//if ( gdi->debug_output ) { NSLog( @"%d %d %d", o, gdi->text_sx, gdi->text_sy ); }


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

		if ( gdi->frame_style == N_GDI_FRAME_AQUA )
		{
			gdi->sx += gdi->sy;
		}


		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{
			sx = sx + gdi->text_sx + gdi->base_gap_between_icon_and_text;
			sy = n_posix_max_n_type_gfx( sy, gdi->text_sy );
		} else
		if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
		{
			sx = n_posix_max_n_type_gfx( sx, gdi->text_sx );
			sy = sy + gdi->text_sy;
		}

	}


	n_type_gfx border  = gdi->frame_size;
	n_type_gfx borders = border * 2;


	n_type_gfx margin = 0;

	{

		n_type_gfx icon_margin = n_posix_min_n_type_gfx( gdi->icon_sx, gdi->icon_sy );
		n_type_gfx text_margin = gdi->text_size;

		if ( icon_onoff == FALSE ) { icon_margin = 0; }
		if ( text_onoff == FALSE ) { text_margin = 0; }

		if ( gdi->style & N_GDI_NOMARGIN )
		{
			//
		} else
		if ( gdi->style & N_GDI_AUTOMARGIN )
		{
			margin = n_posix_max_n_type_gfx( icon_margin, text_margin ) / 1;
		} else
		if ( gdi->frame_style )
		{
			margin = n_posix_max_n_type_gfx( icon_margin, text_margin ) / 4;
		}

	}

	n_type_gfx margins = margin * 2;


	n_type_gfx gap = 0;

	if ( ( text_onoff )&&( icon_onoff ) )
	{

		gap = margin / 2;

		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{
			sx += gap;
		} else
		if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
		{
			sy += gap;
		}

//NSLog( @"Margin %d : Gap %d", margin, gap );
	}


	BOOL is_center_x =
	(
		( gdi->align == N_GDI_ALIGN_CENTER )
		||
		(
			( FALSE == ( gdi->align & N_GDI_ALIGN_LEFT  ) )
			&&
			( FALSE == ( gdi->align & N_GDI_ALIGN_RIGHT ) )
		)
	);

	BOOL is_center_y =
	(
		( gdi->align == N_GDI_ALIGN_CENTER )
		||
		(
			( FALSE == ( gdi->align & N_GDI_ALIGN_TOP    ) )
			&&
			( FALSE == ( gdi->align & N_GDI_ALIGN_BOTTOM ) )
		)
	);


	n_type_gfx ox = 0;
	n_type_gfx oy = 0;

	if ( gdi->sx != 0 )
	{

		n_type_gfx base_sx = gdi->sx - ( borders + margins );

		if ( is_center_x )
		{
			ox = ( base_sx - sx ) / 2;
		} else
		if ( gdi->align & N_GDI_ALIGN_LEFT  )
		{
			ox = 0;
		} else
		if ( gdi->align & N_GDI_ALIGN_RIGHT )
		{
			ox = base_sx - sx;
		}

		sx = base_sx;

	}

	if ( gdi->sy != 0 )
	{

		n_type_gfx base_sy = gdi->sy - ( borders + margins );

		if ( is_center_y )
		{
			oy = ( base_sy - sy ) / 2;
		} else
		if ( gdi->align & N_GDI_ALIGN_TOP    )
		{
			oy = 0;
		} else
		if ( gdi->align & N_GDI_ALIGN_BOTTOM )
		{
			oy = base_sy - sy;
		}

		sy = base_sy;

	}




	// Phase 3 : Basement


	if ( ( sx <= 0 )||( sy <= 0 ) )
	{

		if ( text_onoff ) { n_ini_free( &txt ); }

		if ( n_bmp_is_multithread == FALSE )
		{
			n_bmp_transparent_onoff_default = p_trans_onoff;
		}

		return;
	}


#ifdef N_POSIX_PLATFORM_MAC

	//BOOL autosize_sx = ( gdi->sx == 0 );
	//BOOL autosize_sy = ( gdi->sy == 0 );

//NSLog( @"%d %d", autosize_sx, autosize_sy );

#endif // #ifdef N_POSIX_PLATFORM_MAC

	gdi->sx = sx + ( borders + margins );
	gdi->sy = sy + ( borders + margins );

//NSLog( @"%d %d", gdi->sx, gdi->sy );


	n_bmp bmp_ret; n_bmp_zero( &bmp_ret );

	if ( onoff )
	{

		u32 bg = gdi->base_color_bg;
		u32 fg = gdi->base_color_fg;

		n_bmp_1st_fast( &bmp_ret, gdi->sx,gdi->sy );

		if ( gdi->base_style == N_GDI_BASE_SOLID )
		{

			n_bmp_flush( &bmp_ret, bg );

		} else
		if ( gdi->base_style == N_GDI_BASE_POLKADOT )
		{

			n_type_gfx c  = gdi->base_unit / 2;
			n_type_gfx m  = gdi->base_polkadot_margin;

			n_bmp_pattern_polkadot( &bmp_ret, c,m, bg,fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_POLKADOT_FOAM )
		{

			n_type_gfx c  = gdi->base_unit / 2;
			n_type_gfx m  = gdi->base_polkadot_margin;

			n_bmp_pattern_polkadot_foam( &bmp_ret, c,m, bg,fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_POLKADOT_POP )
		{

			n_type_gfx c  = gdi->base_unit / 2;
			n_type_gfx m  = gdi->base_polkadot_margin;

			n_bmp_pattern_polkadot_pop( &bmp_ret, c,m, bg );

		} else
		if ( gdi->base_style == N_GDI_BASE_POLKADOT_RND )
		{

			n_type_gfx c  = gdi->base_unit / 2;
			n_type_gfx m  = gdi->base_polkadot_margin;

			n_bmp_pattern_polkadot_rnd( &bmp_ret, c,m, bg );

		} else
		if ( gdi->base_style == N_GDI_BASE_RECT_TILE )
		{

			n_type_gfx r  = gdi->base_unit / 2;
			n_type_gfx m  = gdi->base_polkadot_margin;

			n_bmp_pattern_roundrect( &bmp_ret, r,m, bg, fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_GINGHAM )
		{

			n_type_real strp = 0.5;
			int         shrp = 5;

			n_bmp_pattern_check( &bmp_ret, gdi->base_unit, strp, shrp, bg, fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_TARTAN )
		{

			n_type_real strp = 0.25;
			int         shrp = 5;

			n_bmp_pattern_check( &bmp_ret, gdi->base_unit, strp, shrp, bg, fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_STRIPE )
		{

			n_bmp_pattern_stripe( &bmp_ret, gdi->base_unit, TRUE, bg, fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_CHECKER )
		{

			n_bmp_pattern_checker( &bmp_ret, gdi->base_unit, bg, fg );

		} else
		if ( gdi->base_style == N_GDI_BASE_LUNA )
		{

			n_gdi_base_luna_solid( gdi, &bmp_ret, bg, gdi->scale );

			if ( fg != 0 )
			{

				n_bmp bmp_tmp; n_bmp_zero( &bmp_tmp ); n_bmp_1st_fast( &bmp_tmp, gdi->sx,gdi->sy );
				n_bmp_flush( &bmp_tmp, n_bmp_white_invisible );

				n_gdi_frame_roundframe_luna( gdi, &bmp_tmp, gdi->sx,gdi->sy, gdi->frame_size * 2, fg, n_bmp_white_invisible, bg, n_bmp_white_invisible, gdi->scale );

				n_bmp_flush_transcopy( &bmp_tmp, &bmp_ret );

				n_bmp_free( &bmp_tmp );

			}

		} else
		if ( gdi->base_style == N_GDI_BASE_LUNA_PRESS )
		{

			n_gdi_base_luna_press( gdi, &bmp_ret, bg, gdi->scale );

		} else
		if ( gdi->base_style == N_GDI_BASE_IMAGE )
		{

			n_gdi_base_load( gdi, &bmp_ret );

		} else
		if ( gdi->base_style == N_GDI_BASE_IMAGE_TILE )
		{

			n_gdi_base_load( gdi, &bmp_ret );
			n_bmp_resizer( &bmp_ret, gdi->sx, gdi->sy, bg, N_BMP_RESIZER_TILE );

		} else
		if ( gdi->base_style == N_GDI_BASE_PROGRESS_H )
		{
#ifdef N_POSIX_PLATFORM_MINGW

			n_bmp_flush( &bmp_ret, bg );

			n_game_progressbar_horz( &bmp_ret, 0,0,gdi->sx,gdi->sy, fg,bg, gdi->percent, gdi->sy );

			if ( n_game_progressbar_no_round )
			{
				u32 color_frame = n_bmp_blend_pixel( fg, n_bmp_rgb( 10,10,10 ), 0.125 );
				n_gdi_frame_lineframe( gdi, &bmp_ret, 0,0,gdi->sx,gdi->sy, color_frame );
			}

#endif // #ifdef N_POSIX_PLATFORM_MINGW
		} else
		if ( gdi->base_style == N_GDI_BASE_PROGRESS_V )
		{
#ifdef N_POSIX_PLATFORM_MINGW

			n_bmp_flush( &bmp_ret, bg );

			n_game_progressbar_vert( &bmp_ret, 0,0,gdi->sx,gdi->sy, fg,bg, gdi->percent, gdi->sx );

			if ( n_game_progressbar_no_round )
			{
				u32 color_frame = n_bmp_blend_pixel( fg, n_bmp_rgb( 10,10,10 ), 0.125 );
				n_gdi_frame_lineframe( gdi, &bmp_ret, 0,0,gdi->sx,gdi->sy, color_frame );
			}

#endif // #ifdef N_POSIX_PLATFORM_MINGW
		} else
		//
		{

			int mode, type;

			if ( gdi->base_style == N_GDI_BASE_CIRCLE )
			{

				mode = N_BMP_GRADIENT_CIRCLE;
				type = N_BMP_GRADIENT_CENTERING;

			} else {

				mode = N_BMP_GRADIENT_RECTANGLE;
				type = N_BMP_GRADIENT_CENTERING;

				if ( gdi->base_style == N_GDI_BASE_VERTICAL )
				{
					type = N_BMP_GRADIENT_VERTICAL;
				} else
				if ( gdi->base_style == N_GDI_BASE_HORIZONTAL )
				{
					type = N_BMP_GRADIENT_HORIZONTAL;
				}// else

			}

			n_bmp_flush_gradient( &bmp_ret, bg,fg, mode | type );

			if ( gdi->base_style == N_GDI_BASE_VERTICAL )
			{
				if ( n_gdi_gradient_vertical_up_side_down )
				{
					n_bmp_flush_mirror( &bmp_ret, N_BMP_MIRROR_UPSIDE_DOWN );
				}
			}

		}

	}




	// Phase 4 : Icon and Text


	if ( gdi->frame_style == N_GDI_FRAME_AQUA )
	{
		n_gdi_frame_draw( gdi, &bmp_ret );
	}


	if ( gdi->style & N_GDI_CLARITY )
	{
		n_gdi_frame_draw_highlight( gdi, &bmp_ret );
	}


	gdi->offset_x = ox + ( border + margin );
	gdi->offset_y = oy + ( border + margin );


//gdi->layout = N_GDI_LAYOUT_VERTICAL;
//gdi->align  = N_GDI_ALIGN_CENTER;

	if ( icon_onoff )
	{

		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{

			gdi->icon_x = gdi->offset_x;

			if ( is_center_y )
			{
				gdi->icon_y = ( gdi->sy - gdi->icon_sy ) / 2;
			} else {
				gdi->icon_y = gdi->offset_y;
			}

		} else
		if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
		{

			if ( is_center_x )
			{
				gdi->icon_x = ( gdi->sx - gdi->icon_sx ) / 2;
			} else {
				gdi->icon_x = gdi->offset_x;
			}

			gdi->icon_y = gdi->offset_y;

		}

		if ( gdi->style & N_GDI_PRESSED )
		{
			gdi->icon_x += gdi->scale;
			gdi->icon_y += gdi->scale;
		}

#ifdef N_POSIX_PLATFORM_MINGW

		n_gdi_icon_draw( gdi, &bmp_ret );

#else  // #ifdef N_POSIX_PLATFORM_MINGW

		n_bmp icon; n_bmp_zero( &icon );

		BOOL ret = n_gdi_image_load( gdi, &icon, gdi->icon );
//NSLog( @"%s : Ret %d : SX %d : Error %d", gdi->icon, ret, N_BMP_SX( &icon ), n_bmp_error( &icon ) );
//NSLog( @"%d %d", N_BMP_SX( &icon ), N_BMP_SY( &icon ) );

//n_bmp_fastcopy( &icon, bmp, 0,0,64,64, 0,0 );

		if ( ret == FALSE )
		{
			n_gdi_bmp_effect_icon( gdi, &bmp_ret, &icon );
		}

		n_bmp_free_fast( &icon );

#endif // #ifdef N_POSIX_PLATFORM_MINGW

	}

	if ( text_onoff )
	{

		if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
		{

			n_type_gfx icon_offset = 0;
			if ( icon_onoff ) { icon_offset = gdi->icon_sx + gap + gdi->base_gap_between_icon_and_text; }

			gdi->text_x = gdi->offset_x + icon_offset;

			if ( is_center_y )
			{
				gdi->text_y = ( gdi->sy - gdi->text_sy ) / 2;
			} else {
				gdi->text_y = gdi->offset_y;
			}

		} else
		if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
		{
//NSLog( @"%d", gap );
			n_type_gfx icon_offset = 0;

			if ( icon_onoff ) { icon_offset = gdi->icon_sy + gap; }

#ifdef N_POSIX_PLATFORM_MAC
			if ( gdi->frame_style == N_GDI_FRAME_NOFRAME )
			{
				if ( icon_onoff ) { icon_offset += 3; }
			}
#endif // #ifdef N_POSIX_PLATFORM_MAC

			if ( is_center_x )
			{
				gdi->text_x = ( gdi->sx - gdi->text_sx ) / 2;
			} else {
				gdi->text_x = gdi->offset_x;
			}

			gdi->text_y = gdi->offset_y + icon_offset;

		}


		if ( gdi->frame_style == N_GDI_FRAME_AQUA )
		{
			gdi->sx -= gdi->sy;
		}

		if ( gdi->style & N_GDI_PRESSED )
		{
			gdi->text_x += gdi->scale;
			gdi->text_y += gdi->scale;
		}

#ifdef N_POSIX_PLATFORM_WINDOWS

		n_gdi_text_draw( gdi, &bmp_ret, (void*) &txt, NULL,NULL );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

//NSLog( @"%d", n_bmp_flip_onoff );

		n_type_gfx fx_size = n_posix_max_n_type_gfx( gdi->text_fxsize1, gdi->text_fxsize2 );

		gdi->text_x += fx_size;
		gdi->text_y += fx_size;

		n_gdi_bmp_effect_text( gdi, &bmp_ret, &bmp_text );

		n_bmp_free_fast( &bmp_text );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

		if ( gdi->frame_style == N_GDI_FRAME_AQUA )
		{
			gdi->sx += gdi->sy;
		}

	}


	if ( gdi->style & N_GDI_GRAY )
	{
		n_bmp_flush_grayscale( &bmp_ret );
	}


	n_gdi_frame_draw( gdi, &bmp_ret );

	if ( gdi->frame_style == N_GDI_FRAME_ROUND )
	{
		n_bmp_cornermask( &bmp_ret, gdi->frame_round, 0, gdi->frame_corner_color );
	}


	// Phase 5 : Cleanup

/*
	if ( gdi->text_x < 0 ) { gdi->text_sx += abs( gdi->text_x ); gdi->text_x = 0; }
	if ( gdi->text_y < 0 ) { gdi->text_sy += abs( gdi->text_y ); gdi->text_y = 0; }
n_bmp_box( &bmp_ret, gdi->text_x, gdi->text_y, gdi->text_sx, gdi->text_sy, n_bmp_white );
//n_posix_debug_literal( " %d %d %d %d ", gdi->text_x, gdi->text_y, gdi->text_sx, gdi->text_sy );
*/

	if ( onoff )
	{
		n_bmp_free_fast( bmp );
		n_bmp_alias( &bmp_ret, bmp );
	}

	if ( text_onoff ) { n_ini_free( &txt ); }

	if ( n_bmp_is_multithread == FALSE )
	{
		n_bmp_transparent_onoff_default = p_trans_onoff;
	}


	return;
}


#endif // _H_NONNON_BRIDGE_GDI

