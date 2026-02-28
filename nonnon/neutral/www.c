// Felis
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_WWW
#define _H_NONNON_NEUTRAL_WWW




#include "./posix.c"




#define n_www_percentencoding_encode( s ) n_www_percentencoding_codec( s, TRUE  )
#define n_www_percentencoding_decode( s ) n_www_percentencoding_codec( s, FALSE )

n_posix_char*
n_www_percentencoding_codec( const n_posix_char *str, BOOL is_encode )
{

	// [!] : you need to n_string_free() a returned variable


	if ( n_string_is_empty( str ) ) { return n_string_carboncopy( N_STRING_EMPTY ); }


	// [ Mechanism ]
	//
	//	[ needed length ]
	//
	//	strlen( "file:///" ) + [ strlen( path ) * UTF8 * strlen( "%00" ) ] + strlen( "\0" )
	//	     8(  4 + 1 + 3 ) + [ N              *    4 *               3 ] + 1
	//
	//	[ Encoder ]
	//
	//	file protocol is always used, replace with n_string_replace() if necessary
	//	a returned string needs to be handled as read-only
	//	"%" will make troubles with snprintf(), use n_posix_dump() to debug
	//
	//	[ Decoder ]
	//
	//	"str" needs to be a URL string
	//	"str" needs to be transparent with UTF-8


	// Phase 1 : ANSI/Unicode to UTF-8

#ifdef UNICODE
	wchar_t *wstr = n_string_carboncopy ( str );
#else  // #ifdef UNICODE
	wchar_t *wstr = n_posix_ansi2unicode( str );
#endif // #ifdef UNICODE

//MessageBoxW( NULL, wstr, L"Phase 1", 0 );


	// [!] : enough size

	n_type_int cb = ( 8 + ( wcslen( wstr ) * 4 * 3 ) + 1 ) * sizeof( wchar_t );
	if ( cb > UINT_MAX ) { return n_string_carboncopy( N_STRING_EMPTY ); }


	wstr = n_memory_resize( wstr, cb );

	n_unicode_bom_restore_utf16_le( wstr, cb );
	n_unicode_utf8_encode( wstr, cb );

//n_posix_dump_literal( "n_www_percentencoding_codec().txt", wstr, cb );
//n_memory_free( wstr ); return n_string_carboncopy( N_STRING_EMPTY );


	// Phase 2 : percent-encoding : encode/decode

	u8 *f = (u8*) wstr;
	u8 *t = n_memory_new( cb );
	n_memory_zero( t, cb );

	t[ 0 ] = f[ 0 ];
	t[ 1 ] = f[ 1 ];
	t[ 2 ] = f[ 2 ];
	t[ 3 ] =   '\0';

//MessageBoxA( NULL, &f[ 3 ], "Phase 2", 0 );
//n_memory_free( f ); n_memory_free( t ); return n_string_carboncopy( N_STRING_EMPTY );


	if ( is_encode )
	{

		n_type_int i  = 3;
		n_type_int ii = 3 + snprintf( (void*) &t[ 3 ], cb - 3, "file:///" );
		n_posix_loop
		{//break;

			if ( f[ i ] == '\\' ) { f[ i ] = '/'; }

			if (

				// [!] : RFC 3986 section 2.3 Unreserved Characters (January 2005)

				( ( f[ i ] >= 'a' )&&( f[ i ] <= 'z' ) )
				||
				( ( f[ i ] >= 'A' )&&( f[ i ] <= 'Z' ) )
				||
				( ( f[ i ] >= '0' )&&( f[ i ] <= '9' ) )
				||
				( f[ i ] == '-' ) || ( f[ i ] == '_' ) || ( f[ i ] == '.' ) || ( f[ i ] == '~' )


				// [!] : RFC 3986 section 2.2 Reserved Characters (January 2005)

				||
				( f[ i ] == '!' ) || ( f[ i ] == '*' ) || ( f[ i ] == '\'' )
				||
				( f[ i ] == '(' ) || ( f[ i ] == ')' ) || ( f[ i ] == ';' )
				||
				( f[ i ] == ':' ) || ( f[ i ] == '@' ) || ( f[ i ] == '&' )
				||
				( f[ i ] == '=' ) || ( f[ i ] == '+' ) || ( f[ i ] == '$' )
				||
				( f[ i ] == ',' ) || ( f[ i ] == '/' ) || ( f[ i ] == '?' )
				||
				( f[ i ] == '#' ) || ( f[ i ] == '[' ) || ( f[ i ] == ']' )

			)
			{

				t[ ii ] = f[ i ]; ii++;

			} else {

				t[ ii ] = '%'; ii++;
				t[ ii ] = (u8) n_string_binary2hexchar( f[ i ] / 16 ); ii++;
				t[ ii ] = (u8) n_string_binary2hexchar( f[ i ] % 16 ); ii++;

			}

			t[ ii ] = '\0';


			i++;
			if ( f[ i ] == '\0' ) { break; }
		}

//MessageBoxA( NULL, &t[ 3 ], "Phase 2", 0 );

	} else {

		n_type_int i  = 3;
		n_type_int ii = 3;
		n_posix_loop
		{

			if ( f[ i ] == '%' )
			{

				i++;

				int upper = f[ i + 0 ];
				int lower = f[ i + 1 ];

				if ( ( upper >= 'A' )&&( upper <= 'Z' ) ) { upper = ( upper - 'A' ) + 10; }
				if ( ( upper >= 'a' )&&( upper <= 'z' ) ) { upper = ( upper - 'a' ) + 10; }
				if ( ( upper >= '0' )&&( upper <= '9' ) ) { upper = ( upper - '0' );      }

				if ( ( lower >= 'A' )&&( lower <= 'Z' ) ) { lower = ( lower - 'A' ) + 10; }
				if ( ( lower >= 'a' )&&( lower <= 'z' ) ) { lower = ( lower - 'a' ) + 10; }
				if ( ( lower >= '0' )&&( lower <= '9' ) ) { lower = ( lower - '0' );      }

				int n = ( upper * 16 ) + lower;

				t[ ii ] = n; ii++;

				i++;

			} else {

				t[ ii ] = f[ i ]; ii++;

			}

			t[ ii ] = '\0';


			i++;
			if ( f[ i ] == '\0' ) { break; }
		}

	}


	// Phase 3 : write back

	n_posix_char *str_ret = NULL;

	if ( is_encode )
	{

		BOOL is_same = TRUE;

		if ( is_same )
		{

//n_posix_dump_literal( "__ret.txt", t, strlen( t ) );

//MessageBoxA( NULL, t, "Phase 3", 0 );
			n_unicode_bom_remove( t, cb );

#ifdef UNICODE

			str_ret = n_posix_ansi2unicode( (void*) t );
			n_memory_free( t );

#else  // #ifdef UNICODE

			str_ret = (void*) t;

#endif // #ifdef UNICODE

		}

	} else {

		// [!] : unpredictable : need to do round-trip

		BOOL is_same = FALSE;

		{

			s8 *utf8 = n_memory_new( cb ); n_memory_copy( t, utf8, cb );

			n_unicode_utf8_decode( t, cb );

			s8 *ucs2 = n_memory_new( cb ); n_memory_copy( t, ucs2, cb );

			n_unicode_utf8_encode( ucs2, cb );

			is_same = ( 0 == strcmp( utf8, ucs2 ) );

			n_memory_free( utf8 );
			n_memory_free( ucs2 );

		}


		if ( is_same )
		{

			n_unicode_bom_remove( t, cb );

#ifdef UNICODE

			str_ret = (void*) t;

#else  // #ifdef UNICODE

			str_ret = n_posix_unicode2ansi( (void*) t );
			n_memory_free( t );

#endif // #ifdef UNICODE

		} else {

			str_ret = n_string_carboncopy( N_STRING_EMPTY );

		}

	}


	// Phase 4 : cleanup

	n_memory_free( f );


	return str_ret;
}


#endif // _H_NONNON_NEUTRAL_WWW

