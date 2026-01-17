// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




n_type_int
n_mac_txtbox_utf8_cb( n_posix_char *str )
{

	n_type_int cb;
	if ( (u8) str[ 0 ] >= 0xf0 )
	{
		cb = 4;
	} else
	if ( (u8) str[ 0 ] >= 0xe0 )
	{
		cb = 3;
	} else
	if ( (u8) str[ 0 ] >= 0xc0 )
	{
		cb = 2;
	} else {
		cb = 1;
	}


	return cb;
}

// internal
BOOL
n_mac_txtbox_is_fullwidth_detect( u16 c )
{

	if ( c <= 127 )
	{

		// [!] : ASCII compatible code

		return FALSE;

	} else
	if (
		(						//  8448 -  8703
			( c == 8470 )
			||
			( ( c >=  8544 )&&( c <=  8578 ) )
		)
		||
		(						//  8704 -  8959
			( ( c >=  8704 )&&( c <=  8748 ) )
			||
			( c == 8750 )
		)
		||
		( ( c >=  8960 )&&( c <=  9215 ) )		//  8960 -  9215 
		||
		( ( c >=  9312 )&&( c <=  9471 ) )		//  9216 -  9471
		||
		( ( c >= 0x25A0 )&&( c <= 0x25ff ) )		//  9472 -  9727
		||
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


	return FALSE;
}

// internal
BOOL
n_mac_txtbox_is_accentmark_detect( u16 c )
{
//NSLog( @"%04x", c );


	BOOL ret = FALSE;


	if ( ( c >= 0x0300 )&&( c <= 0x036f ) )
	{
		ret = TRUE;
	} else
	if ( ( c >= 0x1ab0 )&&( c <= 0x1ac0 ) )
	{
		ret = TRUE;
	} else
	if ( ( c >= 0x1dc0 )&&( c <= 0x1dff ) )
	{
		ret = TRUE;
	} else
	if ( ( c >= 0x20d0 )&&( c <= 0x20f0 ) )
	{
		ret = TRUE;
	} else
	if ( ( c >= 0xfe20 )&&( c <= 0xfeff ) )
	{
		ret = TRUE;
	} else
	if ( ( c == 0x3099 )||( c == 0x309a ) )
	{
		// [!] : for japanese combining characters : dakuten(0x3099) and handakuten(0x309a)
		//
		//	dakuten    : "ha" will be "ba"
		//	handakuten : "ha" will be "pa"
		//
		//	Finder : a default name of a new folder uses this

		ret = TRUE;
	}


	return ret;
}

BOOL
n_mac_txtbox_is_fullwidth( char *str, n_type_int cch )
{

	if ( str == NULL ) { return FALSE; }


	// [x] : enough size
	n_type_int cch_patch = cch * 10;


	char *ptr = n_string_new( cch_patch );

	n_type_int byte = n_mac_txtbox_utf8_cb( str );


	n_type_int i = 0;
	n_posix_loop
	{
		ptr[ i ] = str[ i ];

		i++;
		if ( i >= byte ) { break; }
	}


	n_unicode_utf8_decode_no_bom( ptr, cch_patch );
//NSLog( @"%S", ptr );

	// [!] : wchar_t is 32-bit
	u16 wc = ( (u16*) ptr )[ 0 ];
//NSLog( @"%x", wc );

	n_string_free( ptr );

	return n_mac_txtbox_is_fullwidth_detect( wc );
}

BOOL
n_mac_txtbox_is_accentmark( char *str, n_type_int cch )
{

	if ( str == NULL ) { return FALSE; }


	// [x] : enough size
	n_type_int cch_patch = cch * 10;


	char *ptr = n_string_new( cch_patch );

	n_type_int byte = n_mac_txtbox_utf8_cb( str );


	n_type_int i = 0;
	n_posix_loop
	{
		ptr[ i ] = str[ i ];

		i++;
		if ( i >= byte ) { break; }
	}

	ptr[ i ] = N_STRING_CHAR_NUL;

//NSLog( @"accentmark : %s", ptr );

	n_unicode_utf8_decode_no_bom( ptr, cch_patch );
//NSLog( @"%S", (unichar*) ptr );

	// [!] : wchar_t is 32-bit
	u16 wc = ( (u16*) ptr )[ 0 ];
//NSLog( @"%x", wc );

	n_string_free( ptr );

	return n_mac_txtbox_is_accentmark_detect( wc );
}

BOOL
n_mac_txtbox_is_digit_fullwidth( n_posix_char *str, n_type_int index )
{

	BOOL ret = FALSE;


	if ( n_string_is_empty( str ) ) { return ret; }

	if ( index < 0 ) { return ret; } 


	n_posix_char *fullwidth_digit[ 10 ] = {
		"０", "１", "２", "３", "４", "５", "６", "７", "８", "９"
	};

	int utf8 = 0;
	n_posix_loop
	{
		n_posix_char *cmp = fullwidth_digit[ utf8 ];

		if (
			(
				( str[ index + 0 ] != N_STRING_CHAR_NUL )
				&&
				( str[ index + 0 ] == cmp[ 0 ] )
			)
			&&
			(
				( str[ index + 1 ] != N_STRING_CHAR_NUL )
				&&
				( str[ index + 1 ] == cmp[ 1 ] )
			)
			&&
			(
				( str[ index + 2 ] != N_STRING_CHAR_NUL )
				&&
				( str[ index + 2 ] == cmp[ 2 ] )
			)
		)
		{
			ret = TRUE;
			break; 
		}

		utf8++;
		if ( utf8 >= 10 ) { break; }
	}


	return ret;
}

BOOL
n_mac_txtbox_digit_detect( n_posix_char *starting_char, n_posix_char *str, n_type_int index )
{

	// [!] : is_digit : set starting char status


	BOOL ret = FALSE;


	if (
		(
			( n_string_is_digit( starting_char, 0 ) )
			&&
			( FALSE == n_string_is_digit( str, index ) )
		)
		||
		(
			( FALSE == n_string_is_digit( starting_char, 0 ) )
			&&
			( n_string_is_digit( str, index ) )
		)
	)
	{
		ret = TRUE;
	}


	if (
		(
			( n_mac_txtbox_is_digit_fullwidth( starting_char, 0 ) )
			&&
			( FALSE == n_mac_txtbox_is_digit_fullwidth( str, index ) )
		)
		||
		(
			( FALSE == n_mac_txtbox_is_digit_fullwidth( starting_char, 0 ) )
			&&
			( n_mac_txtbox_is_digit_fullwidth( str, index ) )
		)
	)
	{
		ret = TRUE;
	}


	return ret;
}

static bool n_mac_txtbox_character_monospace_onoff = FALSE;

// internal
n_posix_char*
n_mac_txtbox_character
(
	      NSFont     *nsfont,
	      CGSize      nsfont_size,
	const u8         *str,
	      n_type_int  index,
	      CGSize     *ret_size,
	      n_type_int *ret_cch,
	      n_type_int *ret_tab
)
{

	// [ Mechanism ]
	//
	//	call this module from begining of a string
	//
	//	index    : element counter
	//	ret_size : pixel metrics of returned string
	//	ret_cch  : character count
	//	ret_tab  : tabbed character count


	static n_posix_char character[ 20 ];

	if ( ret_size == NULL ) { n_string_truncate( character ); }


	n_type_int _index = 0;
	n_type_int _tab   = 0;
	n_type_int _width = 1;

	if ( str[ index ] == N_STRING_CHAR_NUL )
	{

		n_string_truncate( character );

	} else
	if ( ( str[ index ] == N_STRING_CHAR_TAB )&&( ret_tab != NULL ) )
	{

		n_type_int tabstop = 8;
		if ( tabstop <= 0 ) { tabstop = 8; }

		const n_type_int tab = tabstop - ( (*ret_tab) % tabstop );

		if ( ret_size != NULL )
		{
			n_string_padding  ( character, N_STRING_CHAR_SPACE, tab );
			n_string_terminate( character, tab );
		}

		_index = 1;
		_tab   = tab;

	} else {

		// [!] : don't use n_mac_txtbox_utf8_cb()

		n_type_int byte;
		if ( str[ index ] >= 0xf0 )
		{
			byte = 4; _width = 2;
		} else
		if ( str[ index ] >= 0xe0 )
		{
			byte = 3; //_width = 2; // [!] : see below
		} else
		if ( str[ index ] >= 0xc0 )
		{
			byte = 2;
		} else {
			byte = 1;
		}

		if ( ret_size != NULL )
		{

			n_type_int i = 0;
			n_posix_loop
			{
				character[ i ] = str[ index + i ];

				i++;
				if ( i >= byte ) { break; }
			}

			character[ i ] = N_STRING_CHAR_NUL;

			if ( n_mac_txtbox_is_fullwidth( character, byte ) )
			{
				_width = 2;
			}

			if ( n_mac_txtbox_is_accentmark( (char*) &str[ index + byte ], 4 ) )
			{
//NSLog( @"true : %c", str[ index + byte ] );

				n_type_int byte2;
				if ( str[ index + byte ] >= 0xf0 )
				{
					byte2 = 4;
				} else
				if ( str[ index + byte ] >= 0xe0 )
				{
					byte2 = 3;
				} else
				if ( str[ index + byte ] >= 0xc0 )
				{
					byte2 = 2;
				} else {
					byte2 = 1;
				}

				n_type_int i = 0;
				n_posix_loop
				{
					character[ byte + i ] = str[ index + byte + i ];

					i++;
					if ( i >= byte2 ) { break; }
				}

				character[ byte + i ] = N_STRING_CHAR_NUL;

				byte += byte2;

			} else {
//NSLog( @"false : %c", str[ index + byte ] );
			}

		}

		_index = byte;
		_tab   = 1;

	}

	if ( ret_size != NULL )
	{
		if ( n_mac_txtbox_character_monospace_onoff )
		{
			CGSize size = n_mac_image_text_pixelsize( @"W", nsfont );

			CGFloat kaku = size.width;// * _width;
			CGFloat tabs = size.width * _tab  ;

			(*ret_size) = NSMakeSize( MAX( kaku, tabs ), nsfont_size.height );
		} else
		if ( nsfont.fixedPitch )
		{
			CGFloat kaku = nsfont_size.width * _width;
			CGFloat tabs = nsfont_size.width * _tab  ;

			(*ret_size) = NSMakeSize( MAX( kaku, tabs ), nsfont_size.height );
		} else {
			NSString *nsstr = n_mac_str2nsstring( character );
			(*ret_size) = n_mac_image_text_pixelsize( nsstr, nsfont );
		}
	}
	if ( ret_cch  != NULL ) { (*ret_cch)  = _index; }
	if ( ret_tab  != NULL ) { (*ret_tab) += _tab  ; }


	return character;
}

