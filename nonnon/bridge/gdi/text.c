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




void
n_gdi_bmp_alpha_visible(  n_bmp *bmp, u32 chroma_key )
{
//return;

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];
		if ( color != chroma_key )
		{
			N_BMP_PTR( bmp )[ i ] = n_bmp_alpha_visible_pixel( color );
		}

		i++;
		if ( i >= c ) { break; }
	}


	return;
}




// internal
SIZE
n_gdi_text_pixelsize( n_gdi *gdi, const n_posix_char *str, n_type_int cch )
{

	// [Needed] : multi-thread

	HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_pixelsize()" );


	HDC hdc = GetDC( gdi->hwnd );

	HFONT hfont = SelectObject( hdc, gdi->text_cache_hfont );

	SIZE size; GetTextExtentPoint32( hdc, str, (int) cch, &size );

	SelectObject( hdc, hfont );

	ReleaseDC( gdi->hwnd, hdc );


	hmutex = n_thread_mutex_exit( hmutex );


	return size;
}

// internal
SIZE
n_gdi_text_precalc( n_gdi *gdi, const n_posix_char *str, n_type_int cch )
{

	SIZE size = { 0, 0 };


	if ( gdi == NULL ) { return size; }


	if ( cch == 0 ) { cch = n_posix_strlen( str ); }


	if ( gdi->text_style & N_GDI_TEXT_MONOSPACE )
	{
		size.cx = gdi->text_cache_unit.cx * (n_type_gfx) n_string_cb2cch( str, cch * sizeof( n_posix_char ) );
		size.cy = gdi->text_cache_unit.cy;
	} else {
		size = n_gdi_text_pixelsize( gdi, str, cch );
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


	gdi->effect_margin = gdi->effect_size_sum_text;

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

	} else {

		size_ret = n_gdi_text_precalc( gdi, N_STRING_SPACE, 0 );

	}


	return size_ret;
}




// internal
void
n_gdi_text_draw_line
(
	const n_gdi        *gdi,
	      n_bmp        *bmp,
	      n_bmp        *text,
	      n_posix_char *str,
	      n_type_int    length,
	      n_type_gfx    sx,
	      n_type_gfx    sy
)
{

	if ( gdi == NULL ) { return; }

	if ( n_bmp_error( bmp ) ) { return; }

	if ( n_string_is_empty( str ) ) { return; }


	{
		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_draw_line()" );


		HDC hdc = GetDC( NULL );
		HDC hdc_compat = CreateCompatibleDC( hdc );

		HFONT hfont = SelectObject( hdc_compat, gdi->text_cache_hfont );


		n_bmp_flush( text, 0 );

		SetBkMode   ( hdc_compat, TRANSPARENT );
		SetTextColor( hdc_compat, RGB( 255,255,255 ) );


		BITMAPINFO bi = { N_BMP_INFOH( text ), { { 0,0,0,0 } } };

		HBITMAP hbmp   = CreateDIBitmap( hdc, &bi.bmiHeader, CBM_INIT, N_BMP_PTR( text ), &bi, DIB_RGB_COLORS );
		HBITMAP p_hbmp = SelectObject( hdc_compat, hbmp );


		if ( length == 0 ) { length = n_posix_strlen( str ); }


		//const int dt = DT_NOPREFIX | DT_CENTER | ( DT_SINGLELINE | DT_VCENTER );// | DT_NOCLIP;

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


				// [x] : Buggy : DrawText() has a bug at centering

				//RECT r = { x,y,gdi->text_cache_unit.cx,sy };
				//DrawText( hdc_compat, &str[ i ], cch, &r, dt );

				TextOut( hdc_compat, x, y, str, n_posix_strlen( str ) );


				x += gdi->text_cache_unit.cx;
				if ( x >= sx ) { break; }

				i += cch;
				if ( i >= length ) { break; }
			}

		} else {

			// [x] : Buggy : DrawText() has a bug at centering

			//RECT r = { 0,0,sx,sy };
			//DrawText( hdc_compat, str, length, &r, dt );

			n_type_gfx x = gdi->effect_margin;
			n_type_gfx y = gdi->effect_margin;

			TextOut( hdc_compat, x, y, str, n_posix_strlen( str ) );

		}


		SelectObject( hdc_compat, p_hbmp );
		GetDIBits( hdc_compat, hbmp, 0,N_BMP_SY( text ), N_BMP_PTR( text ), &bi, DIB_RGB_COLORS );
		DeleteObject( hbmp );


		SelectObject( hdc_compat, hfont );

		DeleteDC( hdc_compat );
		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );

	} 


	return;
}

// internal
void
n_gdi_text_draw_line_direct( const n_gdi *gdi, n_bmp *bmp, n_posix_char *str )
{

	if ( gdi == NULL ) { return; }

	if ( n_bmp_error( bmp ) ) { return; }

	if ( n_string_is_empty( str ) ) { return; }


	{
		// [Needed] : multi-thread

		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_text_draw_line_direct()" );


		HDC hdc = GetDC( NULL );
		HDC hdc_compat = CreateCompatibleDC( hdc );

		HFONT hfont = SelectObject( hdc_compat, gdi->text_cache_hfont );


		SetBkMode( hdc_compat, TRANSPARENT );

		u32 c = gdi->text_color_main;
		SetTextColor( hdc_compat, RGB( n_bmp_r( c ),n_bmp_g( c ),n_bmp_b( c ) ) );


		BITMAPINFO bi = { N_BMP_INFOH( bmp ), { { 0,0,0,0 } } };

		HBITMAP hbmp   = CreateDIBitmap( hdc, &bi.bmiHeader, CBM_INIT, N_BMP_PTR( bmp ), &bi, DIB_RGB_COLORS );
		HBITMAP p_hbmp = SelectObject( hdc_compat, hbmp );


		n_type_gfx x = gdi->text_x;
		n_type_gfx y = gdi->text_y;

//n_log( "%d %d", x, y );

		TextOut( hdc_compat, x, y, str, n_posix_strlen( str ) );


		SelectObject( hdc_compat, p_hbmp );
		GetDIBits( hdc_compat, hbmp, 0,N_BMP_SY( bmp ), N_BMP_PTR( bmp ), &bi, DIB_RGB_COLORS );
		DeleteObject( hbmp );


		SelectObject( hdc_compat, hfont );

		DeleteDC( hdc_compat );
		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );
	} 


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
n_gdi_text_draw( n_gdi *gdi, n_bmp *bmp, n_txt *txt, BOOL draw )
{

	// Phase 1 : initialization

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


	// Phase 2 : unit size calculation


	// [!] : 10% faster without strlen()

	const n_posix_char *ellipsis_dot     = n_posix_literal( "..." );
	const n_type_int    ellipsis_dot_cch = 3;//n_posix_strlen( ellipsis_dot );
	n_type_gfx          ellipsis_min     = 0;
	n_type_gfx          ellipsis_max     = 0;
	BOOL                ellipsis_onoff   = FALSE;

	n_type_gfx text_x  = 0;
	n_type_gfx text_sx = 0;
	n_type_gfx text_y  = gdi->text_y;
	n_type_gfx text_sy = 0;

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

	//const n_type_real scale = n_win64_scale_factor_ui( NULL );

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


			n_type_gfx sx = size.cx;
			n_type_gfx sy = size.cy;


			// [!] : n_gdi_bmp_effect_text() will shrink automatically

			if ( gdi->text_style & N_GDI_TEXT_SMOOTH )
			{
				sx *= n_gdi_smoothness;
				sy *= n_gdi_smoothness;
			}

			n_bmp text; n_bmp_zero( &text ); n_bmp_1st_fast( &text, sx,sy );

			n_gdi_text_draw_line( gdi, bmp, &text, s, len, sx,sy );

			n_gdi_bmp_alpha_visible( &text, n_bmp_black );
			//if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 ) { n_bmp_alpha_visible( &text ); }

			n_gdi_bmp_effect_text( gdi, bmp, &text );

			n_bmp_free_fast( &text );


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


			if ( text_sx < size.cx ) { text_sx = size.cx; }


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


	gdi->text_x  = text_x;
	gdi->text_y  = text_y;
	gdi->text_sx = text_sx;
	gdi->text_sy = text_sy;


	return;
}


#endif // _H_NONNON_WIN32_GDI_TEXT

