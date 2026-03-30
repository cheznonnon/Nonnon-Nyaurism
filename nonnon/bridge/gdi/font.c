// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifndef _H_NONNON_WIN32_GDI_FONT
#define _H_NONNON_WIN32_GDI_FONT




#define n_gdi_font_logfont2hfont( ptr_lf ) CreateFontIndirect( ptr_lf )

// internal
LOGFONT
n_gdi_font_hfont2logfont( HFONT hfont )
{

	LOGFONT lf; ZeroMemory( &lf, sizeof( LOGFONT ) );

	GetObject( hfont, sizeof( LOGFONT ), &lf );

	return lf;
}

// internal
int CALLBACK
n_gdi_font_enumfontsproc( const LOGFONT *lf, const TEXTMETRIC *tm, DWORD type, LPARAM lparam )
{

	// [Mechanism] : type
	//
	//	DEVICE_FONTTYPE
	//	RASTER_FONTTYPE
	//	TRUETYPE_FONTTYPE

	// [!] : lfPitchAndFamily : FIXED_PITCH : bitmap fonts are only returned


	LOGFONT *query = (LOGFONT*) lparam;


	if ( n_string_is_same( lf->lfFaceName, query->lfFaceName ) )
	{
//n_posix_debug_literal( "%s : %s", lf->lfFaceName, query->lfFaceName );

		n_memory_copy( lf, query, sizeof( LOGFONT ) );


		return FALSE;
	}


	return TRUE;
}

#define n_gdi_font_is_exist_literal( name ) n_gdi_font_is_exist( n_posix_literal( name ) )

BOOL
n_gdi_font_is_exist( const n_posix_char *name )
{

	HWND hwnd = NULL;
	HDC  hdc  = GetDC( hwnd );


	LOGFONT lf; ZeroMemory( &lf, sizeof( LOGFONT ) );

	if ( LF_FACESIZE > n_posix_strlen( name ) ) { n_string_copy( name, lf.lfFaceName ); }


	BOOL ret = EnumFonts( hdc, NULL, n_gdi_font_enumfontsproc, (LPARAM) &lf );


	ReleaseDC( hwnd, hdc );


	return ( ret == FALSE );
}

n_posix_char*
n_gdi_font_find( n_posix_char *first, ... )
{

	n_posix_char *ret = NULL;


	va_list vl; va_start( vl, first );

	n_posix_loop
	{

		if ( ret == NULL )
		{
			ret = first;
		} else {
			ret = va_arg( vl, n_posix_char* );
		}
		if ( ret == NULL ) { break; }

		if ( n_gdi_font_is_exist( ret ) ) { break; }

	}

	va_end( vl );


	return ret;
}

// internal
HFONT
n_gdi_font_process( const n_gdi *gdi, LOGFONT *lf )
{

	// [!] : see "win32/gdi/text.c" for details

/*
	if ( gdi->text_style & N_GDI_FONT_MONOSPACE )
	{

		const int monospace = FIXED_PITCH;
		const int sansserif = FF_MODERN;

		if ( n_sysinfo_version_9x() )
		{
			(*lf) = n_gdi_font_hfont2logfont( GetStockObject( SYSTEM_FIXED_FONT ) );
		} else {
			(*lf) = n_gdi_font_hfont2logfont( GetStockObject( DEFAULT_GUI_FONT  ) );
			lf->lfPitchAndFamily = monospace | sansserif;
		}

	} else {
*/
		if ( LF_FACESIZE > n_posix_strlen( gdi->text_font ) )
		{
			n_string_copy( gdi->text_font, lf->lfFaceName );
		}


		HANDLE hmutex = n_thread_mutex_init_and_wait_literal( NULL, "n_gdi_font_process()" );


		HDC hdc = GetDC( NULL );

		BOOL ret = EnumFonts( hdc, NULL, n_gdi_font_enumfontsproc, (LPARAM) lf );
		if ( ret ) { (*lf) = n_gdi_font_hfont2logfont( GetStockObject( DEFAULT_GUI_FONT ) ); }

		ReleaseDC( NULL, hdc );


		hmutex = n_thread_mutex_exit( hmutex );


//	}


	// [x] : Win9x : an inaccurate font might be selected

	//lf->lfCharSet = DEFAULT_CHARSET;


	lf->lfWidth  = 0;
	lf->lfHeight = gdi->text_size;


	// [!] : Win95 default
	//
	//	not available : ANTIALIASED_QUALITY
	//	extensible with "w95gray.exe"

	if ( gdi->text_style & N_GDI_TEXT_SYS_SMOOTH )
	{
		lf->lfQuality = CLEARTYPE_NATURAL_QUALITY;
	} else
	if ( gdi->text_style & N_GDI_TEXT_PRINTER )
	{
		lf->lfQuality =    ANTIALIASED_QUALITY;
	} else {
		lf->lfQuality = NONANTIALIASED_QUALITY;
	}


	if ( n_gdi_fakebold_onoff == FALSE )
	{
		if ( gdi->text_style & N_GDI_FONT_BOLD ) { lf->lfWeight = FW_BOLD; }
	}

	if ( gdi->text_style & N_GDI_FONT_ITALIC    ) { lf->lfItalic    = TRUE; }
	if ( gdi->text_style & N_GDI_FONT_UNDERLINE ) { lf->lfUnderline = TRUE; }
	if ( gdi->text_style & N_GDI_FONT_STRIKEOUT ) { lf->lfStrikeOut = TRUE; }


	return n_gdi_font_logfont2hfont( lf );
}

HFONT
n_gdi_font( const n_gdi *gdi )
{

	// [!] : returned HFONT needs DeleteObject()


	// [!] : Cache : 20% faster when the same font is requested

	if ( FALSE == n_thread_onoff() )
	{

		static BOOL    init  = FALSE;
		static int     style = 0;
		static LOGFONT lf;


		if ( init == FALSE )
		{
			init = TRUE;
			ZeroMemory( &lf, sizeof( LOGFONT ) );
		}


		if (
			( gdi->text_style == style )
			&&
			( gdi->text_size  == lf.lfHeight )
			&&
			( n_string_is_same( gdi->text_font, lf.lfFaceName ) )
		)
		{
			return n_gdi_font_logfont2hfont( &lf );
		} else {
			ZeroMemory( &lf, sizeof( LOGFONT ) );
			style = gdi->text_style;
		}


		return n_gdi_font_process( gdi, &lf );
	}


	LOGFONT lf; ZeroMemory( &lf, sizeof( LOGFONT ) );


	return n_gdi_font_process( gdi, &lf );
}


#endif // _H_NONNON_WIN32_GDI_FONT

