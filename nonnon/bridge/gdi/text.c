// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifndef _H_NONNON_WIN32_GDI_TEXT
#define _H_NONNON_WIN32_GDI_TEXT




#include "./effect.c"
#include "./font.c"




// internal
BOOL
n_gdi_text_is_fullwidth( n_posix_char c )
{

#ifdef UNICODE

	// [x] : why edit control uses too strict range
	//
	//	maybe compatibility with IME causes this 

	if ( c <= 127 )
	{

		// [!] : ASCII compatible code

		return FALSE;

	} else
	if (
		( ( c >= 0x2600 )&&( c <= 0x26ff ) )		//  9728 -  9983
		||
		( ( c >= 0x2700 )&&( c <= 0x27ff ) )		//  9984 - 10239
		||
		( ( c >= 0x2e00 )&&( c <= 0x2eff ) )		// 11766 - 12031
		||
		( ( c >= 0x2f00 )&&( c <= 0x2fff ) )		// 12032 - 12287
		||
		( ( c >= 0x3000 )&&( c <= 0xd7ff ) )		// 12288 - 55295
		||
		( ( c >= 0xff00 )&&( c <= 0xff60 ) )		// 65280 - 65376 : 0xff61 or above : includes "half-width kana"
	)
	{
		return TRUE;
	}


	wchar_t w[ 2 ] = { c, L'\0' };
	 char   a[ 3 ] = { '\0', '\0', '\0' };

	WideCharToMultiByte( CP_ACP, 0, w,2, a,3, NULL,NULL );

	if ( 1 < strlen( a ) ) { return TRUE; } 

#else  // #ifdef UNICODE

	if ( c < 127 )
	{

		// [!] : ASCII compatible code

		return FALSE;

	} else {

		return TRUE;

	}

#endif // #ifdef UNICODE


	return FALSE;
}

// internal
n_type_int
n_gdi_text_monospace_count( const n_posix_char *str ) 
{

	n_type_int ret = 0;


	n_type_int x = 0;
	n_posix_loop
	{
		if ( str[ x ] == N_STRING_CHAR_NUL ) { break; }

		if ( n_gdi_text_is_fullwidth( str[ x ] ) )
		{
			ret += 2;
		} else {
			ret += 1;
		}

		x += n_string_doublebyte_increment( str[ x ] );
	}


	return ret;
}

// internal
SIZE
n_gdi_text_precalc
(
	      n_gdi        *gdi,
	const n_posix_char *str,
	      n_type_int    cch
)
{


	SIZE size = { 0, 0 };


	if ( gdi == NULL ) { return size; }


	if ( cch == 0 ) { cch = n_posix_strlen( str ); }


	if ( gdi->text_style & N_GDI_TEXT_MONOSPACE )
	{

		size.cx = gdi->text_cache_unit.cx * (n_type_gfx) n_string_cb2cch( str, cch * sizeof( n_posix_char ) );
		size.cy = gdi->text_cache_unit.cy;

	} else
	if ( gdi->text_style & N_GDI_TEXT_MONOSPACE_2 )
	{

		size.cx = gdi->text_cache_unit.cx * (n_type_gfx) n_gdi_text_monospace_count( str );
		size.cy = gdi->text_cache_unit.cy;

	} else {

		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_precalc()" );


		HDC hdc = GetDC( NULL );;

		HFONT hfont = SelectObject( hdc, gdi->text_cache_hfont );

		GetTextExtentPoint32( hdc, str, (int) cch, &size );
		//size.cx *= 4;

		SelectObject( hdc, hfont );

		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

	}


	// [!] : italic needs more width

	if ( gdi->text_style & N_GDI_TEXT_ITALIC  )
	{
		size.cx += gdi->text_cache_unit.cx;
	}


	if ( gdi->text_style & N_GDI_TEXT_SMOOTH )
	{
		size.cx = ( size.cx / n_gdi_smoothness ) + 2;
		size.cy = ( size.cy / n_gdi_smoothness ) + 2;
	}


	gdi->effect_margin = 0;

	if ( ( n_gdi_fakebold_onoff )&&( gdi->text_style & N_GDI_TEXT_BOLD ) ) { gdi->effect_margin += 2; }

	if ( gdi->text_style & N_GDI_TEXT_CONTOUR ) { gdi->effect_margin += 2 * gdi->text_fxsize2; } else
	if ( gdi->text_style & N_GDI_TEXT_SINK    ) { gdi->effect_margin += 2 * gdi->text_fxsize2; }

	if ( gdi->text_style & N_GDI_TEXT_SHADOW  ) { gdi->effect_margin += 1 * gdi->text_fxsize1; }

	if ( gdi->text_style & N_GDI_TEXT_ELLIPSIS )
	{
		if ( gdi->text_style & N_GDI_TEXT_NO_MARGIN )
		{
			//
		} else {
			gdi->effect_margin += ( gdi->frame_size * 2 );
		}
	}

	if ( gdi->scale >= 1 )
	{
		size.cx *= gdi->scale;
		size.cx *= gdi->scale;

		gdi->effect_margin *= gdi->scale;
	}

	size.cx = size.cx + gdi->effect_margin;
	size.cy = size.cy + gdi->effect_margin;


	return size;
}

// internal
SIZE
n_gdi_text_precalc_unit( n_gdi *gdi, const n_txt *txt )
{

	SIZE size_ret = { 0, 0 };


	if ( gdi == NULL ) { return size_ret; }

	if ( n_txt_error( txt ) ) { return size_ret; }


	if ( gdi->text_style & N_GDI_TEXT_MONOSPACE )
	{

		// [!] : TEXTMETRIC.tmMaxCharWidth will be too large with ASCII code
		//
		//	"W" for GetTextExtentPoint32() is not almighty


		n_posix_char *s = txt->stream;
		if ( n_string_is_empty( s ) ) { s = N_STRING_SPACE; }


		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_precalc_unit()" );


		HDC hdc        = GetDC( NULL );
		HDC hdc_compat = CreateCompatibleDC( hdc );

		HFONT hfont = SelectObject( hdc_compat, gdi->text_cache_hfont );

		n_type_int i = 0;
		n_posix_loop
		{

			n_type_int cch = n_string_doublebyte_increment( s[ i ] );

			SIZE size;
			GetTextExtentPoint32( hdc_compat, &s[ i ], (int) cch, &size );

			if ( size.cx > size_ret.cx ) { size_ret.cx = size.cx; }
			if ( size.cy > size_ret.cy ) { size_ret.cy = size.cy; }

			if ( ( cch == 2 )&&( s[ i + 1 ] == N_STRING_CHAR_NUL ) ) { break; }

			i += cch;
			if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }
		}

		SelectObject( hdc_compat, hfont );

		DeleteDC( hdc_compat );
		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

	} else
	if ( gdi->text_style & N_GDI_TEXT_MONOSPACE_2 )
	{

		// [!] : TEXTMETRIC.tmMaxCharWidth will be too large with ASCII code
		//
		//	"W" for GetTextExtentPoint32() is not almighty


		n_posix_char *s = txt->stream;
		if ( n_string_is_empty( s ) ) { s = N_STRING_SPACE; }


		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_precalc_unit()" );


		HDC hdc        = GetDC( NULL );
		HDC hdc_compat = CreateCompatibleDC( hdc );

		HFONT hfont = SelectObject( hdc_compat, gdi->text_cache_hfont );

		n_type_int i = 0;
		n_posix_loop
		{

			n_type_int cch = n_string_doublebyte_increment( s[ i ] );

			if ( FALSE == n_gdi_text_is_fullwidth( s[ i ] ) )
			{
				SIZE size;
				GetTextExtentPoint32( hdc_compat, &s[ i ], (int) cch, &size );

				if ( size.cx > size_ret.cx ) { size_ret.cx = size.cx; }
				if ( size.cy > size_ret.cy ) { size_ret.cy = size.cy; }
			}

			if ( ( cch == 2 )&&( s[ i + 1 ] == N_STRING_CHAR_NUL ) ) { break; }

			i += cch;
			if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }
		}

		SelectObject( hdc_compat, hfont );

		DeleteDC( hdc_compat );
		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

	} else {

		size_ret = n_gdi_text_precalc( gdi, N_STRING_SPACE, 0 );

	}


	return size_ret;
}

// internal
void
n_gdi_text_draw_DrawText( const n_gdi *gdi, HDC hdc_compat, const n_posix_char *str, n_type_int length, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy )
{

	const int dt = DT_NOPREFIX | DT_CENTER | ( DT_SINGLELINE | DT_VCENTER );// | DT_NOCLIP;


	n_type_gfx dx  = 0;
	n_type_gfx dy  = 0;
	n_type_gfx dsx = 0;
	n_type_gfx dsy = 0;

	if ( ( n_gdi_fakebold_onoff )&&( gdi->text_style & N_GDI_TEXT_BOLD ) )
	{
		dsx = dsy = 2;
		if ( gdi->text_style & N_GDI_TEXT_SMOOTH ) { dx = dy = -1; }
	}

	n_type_gfx start_dx = dx;
	n_posix_loop
	{

		RECT r = { x + dx, y + dy, x + sx + dx, y + sy + dy };
		DrawText( hdc_compat, str, (int) length, &r, dt );

		dx++;
		if ( dx >= dsx )
		{
			dx = start_dx;
			dy++;
			if ( dy >= dsy ) { break; }
		}
	}

	return;
}

// internal
void
n_gdi_text_draw_line
(
	const n_gdi        *gdi,
	      n_bmp        *bmp,
	      n_posix_char *str,
	      n_type_int    length,
	      n_type_gfx    sx,
	      n_type_gfx    sy
)
{

	if ( gdi == NULL ) { return; }

	if ( n_bmp_error( bmp ) ) { return; }

	if ( n_string_is_empty( str ) ) { return; }


	// [!] : n_gdi_bmp_effect_text() will shrink automatically

	if ( gdi->text_style & N_GDI_TEXT_SMOOTH )
	{
		sx *= n_gdi_smoothness;
		sy *= n_gdi_smoothness;
	}


	n_bmp text; n_bmp_zero( &text ); n_bmp_1st_fast( &text, sx,sy );
	n_bmp_flush( &text, 0 );


	// [!]
	//
	//	CreateDIBitmap()     : use HDC returned by GetDC()
	//	GetDC()              : hbmp will be full-color bitmap
	//	CreateCompatibleDC() : hbmp will be monochrome bitmap
	//
	//	GDI functions        : use HDC returned by CreateCompatibleDC()
	//
	//	SelectObject()       : MSDN : unselect before GetDIBits()
	//
	// 	GetDIBits()          : different colors are returned when color depth is not 32bit


	{
		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_draw_line()" );


		HDC hdc = GetDC( NULL );
		HDC hdc_compat = CreateCompatibleDC( hdc );

		HFONT hfont = SelectObject( hdc_compat, gdi->text_cache_hfont );


		BITMAPINFO bi = { N_BMP_INFOH( &text ), { { 0,0,0,0 } } };

		HBITMAP hbmp   = CreateDIBitmap( hdc, &bi.bmiHeader, CBM_INIT, N_BMP_PTR( &text ), &bi, DIB_RGB_COLORS );
		HBITMAP p_hbmp = SelectObject( hdc_compat, hbmp );

		SetBkMode   ( hdc_compat, TRANSPARENT        );
		SetTextColor( hdc_compat, RGB( 255,255,255 ) );


		if ( length == 0 ) { length = n_posix_strlen( str ); }


		// [x] : ExtTextOut() is fastest but useless here
		//
		//	text will be rendered top-left always
		//	SetTextAlign( g.hdc_compat, TA_CENTER ); makes 0,0 as a center point
		//	manual handling is needed
		//
		//	ExtTextOut( gdi->hdc_compat, x,y, 0, &rect, str, length, NULL );
		//	TextOut( gdi->hdc_compat, x,y, str, length );

		if ( gdi->text_style & N_GDI_TEXT_MONOSPACE_2 )
		{

			// [!] : forced monospace rendering
			//
			//	because n_win_font_logfont2hfont() is useless

			n_posix_char *s = str;
			if ( n_string_is_empty( s ) ) { s = N_STRING_SPACE; }

			n_type_gfx x = gdi->effect_margin;
			n_type_gfx y = gdi->effect_margin;

			n_type_int i = 0;
			n_posix_loop
			{

				if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }


				n_type_int cch = n_string_doublebyte_increment( s[ i ] );

				n_type_gfx unit;
				if ( n_gdi_text_is_fullwidth( s[ i ] ) )
				{
					unit = gdi->text_cache_unit.cx * 2;
				} else {
					unit = gdi->text_cache_unit.cx;
				}

				n_gdi_text_draw_DrawText( gdi, hdc_compat, &s[ i ], cch, x,y,unit,sy );

				x += unit;
				i += cch;

			}

		} else
		if ( gdi->text_style & N_GDI_TEXT_MONOSPACE )
		{

			// [!] : forced monospace rendering
			//
			//	because n_win_font_logfont2hfont() is useless

			n_posix_char *s = str;
			if ( n_string_is_empty( s ) ) { s = N_STRING_SPACE; }

			n_type_gfx x = gdi->effect_margin;
			n_type_gfx y = gdi->effect_margin;

			n_type_int i = 0;
			n_posix_loop
			{

				n_type_int cch = n_string_doublebyte_increment( s[ i ] );

				n_gdi_text_draw_DrawText( gdi, hdc_compat, &s[ i ], cch, x,y,gdi->text_cache_unit.cx,sy );

				x += gdi->text_cache_unit.cx;
				if ( x >= sx ) { break; }

				i += cch;
				if ( i >= length ) { break; }
			}

		} else {

			n_gdi_text_draw_DrawText( gdi, hdc_compat, str, length, 0,0,sx,sy );

		}


		SelectObject( hdc_compat, p_hbmp );
		GetDIBits( hdc_compat, hbmp, 0,N_BMP_SY( &text ), N_BMP_PTR( &text ), &bi, DIB_RGB_COLORS );
		DeleteObject( hbmp );


		SelectObject( hdc_compat, hfont );

		DeleteDC( hdc_compat );
		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

	} 


	if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 ) { n_bmp_alpha_visible( &text ); }


	n_gdi_bmp_effect_text( gdi, bmp, &text );


	n_bmp_free_fast( &text );


	return;
}

void
n_gdi_text_cache_init( n_gdi *gdi, const n_txt *txt )
{

	if ( gdi == NULL ) { return; }

	if ( n_txt_error( txt ) ) { return; }


	if ( gdi->text_style & N_GDI_TEXT_SMOOTH ) { gdi->text_size *= n_gdi_smoothness; }

	gdi->text_cache_len   = n_memory_new_closed( txt->sy * sizeof( n_type_int ) );
	gdi->text_cache_size  = n_memory_new_closed( txt->sy * sizeof( SIZE       ) );
	gdi->text_cache_hfont = n_gdi_font( gdi );
	gdi->text_cache_unit  = n_gdi_text_precalc_unit( gdi, txt );

	if ( gdi->text_style & N_GDI_TEXT_SMOOTH ) { gdi->text_size /= n_gdi_smoothness; }


	return;
}

void
n_gdi_text_cache_exit( n_gdi *gdi )
{

	n_memory_free_closed( gdi->text_cache_len   );
	n_memory_free_closed( gdi->text_cache_size  );
	DeleteObject        ( gdi->text_cache_hfont );

	gdi->text_cache_len   = NULL;
	gdi->text_cache_size  = NULL;
	gdi->text_cache_hfont = NULL;

	n_memory_zero( &gdi->text_cache_unit, sizeof( SIZE ) );


	return;
}

void
n_gdi_text_draw
(
	n_gdi      *gdi,
	n_bmp      *bmp,
	n_txt      *txt,
	n_type_gfx *ret_sx,
	n_type_gfx *ret_sy
)
{

	// Phase 1 : initialization

	BOOL draw;

	if ( ( ret_sx != NULL )||( ret_sy != NULL ) )
	{
		draw = FALSE;
	} else {
		draw = TRUE;
	}


	if ( draw )
	{

		if (
			( gdi == NULL )
			||
			( n_txt_error( txt ) )
			||
			( n_bmp_error( bmp ) )
		)
		{

			n_gdi_text_cache_exit( gdi );

			return;
		}

	} else {

		if (
			( gdi == NULL )
			||
			( n_txt_error( txt ) )
		)
		{
			return;
		}

	}


	if ( ret_sx != NULL ) { (*ret_sx) = 0; }
	if ( ret_sy != NULL ) { (*ret_sy) = 0; }


	// Phase 2 : unit size calculation


	// [!] : 10% faster without strlen()

	const n_posix_char *ellipsis_dot     = n_posix_literal( "..." );
	const n_type_int    ellipsis_dot_cch = 3;//n_posix_strlen( ellipsis_dot );
	n_type_gfx          ellipsis_min     = 0;
	n_type_gfx          ellipsis_max     = 0;
	BOOL                ellipsis_onoff   = FALSE;

	n_type_gfx          text_y           = gdi->text_y;
	n_type_gfx          text_sy          = 0;

	if ( draw )
	{

		//

	} else {

		n_gdi_text_cache_init( gdi, txt );


		if ( gdi->text_style & N_GDI_TEXT_ELLIPSIS )
		{

			n_type_gfx gdi_sx = gdi->sx;

			if ( gdi->text_style & N_GDI_TEXT_NO_MARGIN )
			{
				//
			} else {
				gdi_sx -= ( gdi->frame_size * 2 );
			}

			if ( gdi->layout == N_GDI_LAYOUT_HORIZONTAL )
			{
				n_type_gfx pad = gdi->icon_sx / 2;
				ellipsis_min = ellipsis_max = n_posix_max_n_type_gfx( 0, gdi_sx - gdi->icon_sx - pad );
			} else
			if ( gdi->layout == N_GDI_LAYOUT_VERTICAL )
			{
				ellipsis_min = ellipsis_max = n_posix_max_n_type_gfx( 0, gdi_sx );
			}

			SIZE ellipsis_size = n_gdi_text_precalc( gdi, ellipsis_dot, 0 );
			ellipsis_min -= ellipsis_size.cx;

			if ( gdi_sx > 0 ) { ellipsis_onoff = TRUE; }

		}

	}


	// Phase 2 : main

	n_posix_char *s;
	n_type_int    len;
	SIZE          size;


	n_type_int i = 0;
	n_posix_loop
	{

		if ( draw )
		{

			s    = txt->line           [ i ];
			len  = gdi->text_cache_len [ i ];
			size = gdi->text_cache_size[ i ];

			n_gdi_text_draw_line( gdi, bmp, s, len, size.cx,size.cy );

			gdi->text_y += gdi->text_cache_size[ i ].cy;

		} else {

			len  = 0;
			s    = n_string_tab2space( txt->line[ i ], 8, &len );
			size = n_gdi_text_precalc( gdi, s, len );


			if ( ( gdi->text_style & N_GDI_TEXT_ELLIPSIS )&&( ellipsis_onoff )&&( size.cx > ellipsis_max ) )
			{

				n_type_int    ellipsis_len  = len + ellipsis_dot_cch;
				n_posix_char *ellipsis_str  = n_string_new_fast( ellipsis_len );
				n_posix_char *ellipsis_prv  = n_string_new_fast( ellipsis_len );
//n_string_copy( s, ellipsis_str );

				n_type_int cur_cch = 0;
				n_type_int prv_cch = 0;

				SIZE cur_size = { 0, 0 };
				SIZE prv_size = { 0, 0 };

				BOOL use_ellipsis_str = TRUE;

				n_type_int cch = 0;
				n_posix_loop
				{//break;

					cur_cch = n_posix_snprintf_literal( ellipsis_str, ellipsis_len + 1, "%.*s%s", (int) cch, s, ellipsis_dot );
//n_posix_debug( ellipsis_str );

					cur_size = n_gdi_text_precalc( gdi, ellipsis_str, cur_cch );

					if ( cur_size.cx > ellipsis_max )
					{
						if ( cch != 0 )
						{
							use_ellipsis_str = FALSE;
							cur_cch  = prv_cch;
							cur_size = prv_size;
						}
						break;
					}
					if ( ( cur_size.cx > ellipsis_min )&&( cur_size.cx < ellipsis_max ) )
					{
						break;
					} else {
						n_string_copy( ellipsis_str, ellipsis_prv );
						prv_cch  = cur_cch;
						prv_size = cur_size;
					}


					cch += n_string_doublebyte_increment( s[ cch ] );
					//if ( cch >= len ) { break; }
				}

				n_memory_free( s );

				if ( use_ellipsis_str )
				{
					s = ellipsis_str;
					n_memory_free( ellipsis_prv );
				} else {
					s = ellipsis_prv;
					n_memory_free( ellipsis_str );
				}

				len  = cur_cch;
				size = cur_size;

			}


			n_txt_mod_fast( txt, i, s );

			//n_memory_free( s );


			if ( ret_sx != NULL )
			{
				if ( (*ret_sx) < size.cx ) { (*ret_sx) = size.cx; }
			}

			if ( ret_sy != NULL ) { (*ret_sy) += size.cy; }


			gdi->text_cache_len [ i ] = len;
			gdi->text_cache_size[ i ] = size;

		}


		text_sy += gdi->text_cache_size[ i ].cy;


		i++;
		if ( i >= txt->sy ) { break; }
	}


	if ( draw )
	{
		n_gdi_text_cache_exit( gdi );
	}


	gdi->text_y  = text_y;
	gdi->text_sy = text_sy;


	return;
}


#endif // _H_NONNON_WIN32_GDI_TEXT

