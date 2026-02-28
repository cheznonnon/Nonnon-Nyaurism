// Nonnon String Filter
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism] : ANSI Version : DBCS Support
//
//	double-byte code : second byte might include \(0x5C)
//
//	IsDBCSLeadByte() returns TRUE when second byte has some value
//
//	maybe 0x83 or higher
//	full-width katakana "ru" (0x838b) returns TRUE




#ifndef _H_NONNON_NEUTRAL_STRING
#define _H_NONNON_NEUTRAL_STRING




#include "./posix.c"




#define N_STRING_CHAR_NUL       n_posix_literal( '\0' )
#define N_STRING_CHAR_CR        n_posix_literal( '\r' )
#define N_STRING_CHAR_LF        n_posix_literal( '\n' )
#define N_STRING_CHAR_SPACE     n_posix_literal(  ' ' )
#define N_STRING_CHAR_TAB       n_posix_literal( '\t' )
#define N_STRING_CHAR_SQUOTE    n_posix_literal( '\'' )
#define N_STRING_CHAR_DQUOTE    n_posix_literal( '\"' )
#define N_STRING_CHAR_SLASH     n_posix_literal(  '/' )
#define N_STRING_CHAR_BSLASH    n_posix_literal( '\\' )
#define N_STRING_CHAR_COLON     n_posix_literal(  ':' )
#define N_STRING_CHAR_SEMICOLON n_posix_literal(  ';' )
#define N_STRING_CHAR_DOT       n_posix_literal(  '.' )
#define N_STRING_CHAR_COMMA     n_posix_literal(  ',' )
#define N_STRING_CHAR_TILDE     n_posix_literal(  '~' )
#define N_STRING_CHAR_ASTERISK  n_posix_literal(  '*' )

#define N_STRING_NUL            n_posix_literal( "\0" )
#define N_STRING_EMPTY          n_posix_literal( "" )
#define N_STRING_CR             n_posix_literal( "\r" )
#define N_STRING_LF             n_posix_literal( "\n" )
#define N_STRING_CRLF           n_posix_literal( "\r\n" )
#define N_STRING_SPACE          n_posix_literal( " " )
#define N_STRING_TAB            n_posix_literal( "\t" )
#define N_STRING_SQUOTE         n_posix_literal( "\'" )
#define N_STRING_DQUOTE         n_posix_literal( "\"" )
#define N_STRING_SLASH          n_posix_literal(  "/" )
#define N_STRING_BSLASH         n_posix_literal( "\\" )
#define N_STRING_COLON          n_posix_literal( ":" )
#define N_STRING_SEMICOLON      n_posix_literal( ";" )
#define N_STRING_DOT            n_posix_literal( "." )
#define N_STRING_DOTDOT         n_posix_literal( ".." )
#define N_STRING_COMMA          n_posix_literal(  "," )
#define N_STRING_TILDE          n_posix_literal(  "~" )
#define N_STRING_ASTERISK       n_posix_literal(  "*" )


// [!] : for string_path.c compatibility
#define N_STRING_NUL_CCH ( 2 )




//#define n_string_zero( str, len ) n_string_padding( str, N_STRING_CHAR_NUL, len )
#define n_string_zero( str, len ) n_memory_zero( str, len * sizeof( n_posix_char ) )

void
n_string_padding( n_posix_char *str, n_posix_char c, n_type_int len )
{

	if ( str == NULL ) { return; }


	n_type_int i = 0;
	n_posix_loop
	{

		str[ i ] = c;

		i++;
		if ( i >= len ) { break; }
	}


	return;
}

BOOL
n_string_is_empty( const n_posix_char *str )
{

	if ( str == NULL ) { return TRUE; }

	if ( str[ 0 ] == N_STRING_CHAR_NUL ) { return TRUE; }


	return FALSE;
}

#define n_string_is_blank(         s, i ) n_string_char_is_blank( s[ i ] )
#define n_string_char_is_blank(    c    ) ( ( c == N_STRING_CHAR_TAB )||( c == N_STRING_CHAR_SPACE ) )

#define n_string_is_digit(         s, i ) n_string_char_is_digit( s[ i ] )
#define n_string_char_is_digit(    c    ) ( ( c >= n_posix_literal( '0' ) )&&( c <= n_posix_literal( '9' ) ) )

#define n_string_is_ascii(         s, i ) n_string_char_is_ascii( s[ i ] )
#define n_string_char_is_ascii(    c    ) ( ( c >= N_STRING_CHAR_SPACE )&&( c <= N_STRING_CHAR_TILDE ) )

#define n_string_char_is_upper(    c    ) ( ( ( c ) >= n_posix_literal( 'A' ) )&&( ( c ) <= n_posix_literal( 'Z' ) ) )
#define n_string_char_is_lower(    c    ) ( ( ( c ) >= n_posix_literal( 'a' ) )&&( ( c ) <= n_posix_literal( 'z' ) ) )
#define n_string_is_upper(         s, i ) n_string_char_is_upper( s[ i ] )
#define n_string_is_lower(         s, i ) n_string_char_is_lower( s[ i ] )

#define n_string_char_is_alphabet( c    ) ( ( n_string_char_is_upper( c ) )||( n_string_char_is_lower( c ) ) )
#define n_string_is_alphabet(      s, i ) n_string_char_is_alphabet( s[ i ] )

#define n_string_is_hex( s, i ) n_string_char_is_hex( s[ i ] )

n_posix_inline BOOL
n_string_char_is_hex( n_posix_char c )
{

	return
	(
		( n_string_char_is_digit( c ) )
		||
		(
			( ( c >= n_posix_literal( 'A' ) )&&( c <= n_posix_literal( 'F' ) ) )
			||
			( ( c >= n_posix_literal( 'a' ) )&&( c <= n_posix_literal( 'f' ) ) )
		)
	);

}

BOOL
n_string_tab_is_exist( const n_posix_char *str, n_type_int *ret_cch )
{

	if ( n_string_is_empty( str ) ) { return FALSE; }


	BOOL ret = FALSE;


	n_type_int i = 0;
	n_posix_loop
	{

		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( str[ i ] == N_STRING_CHAR_TAB )
		{
			ret = TRUE;
			if ( ret_cch == NULL ) { break; }
		}

		i++;

	}


	if ( ret_cch != NULL ) { (*ret_cch) = i; }


	return ret;
}

n_posix_char
n_string_binary2hexchar( u8 b )
{

	// [Mechanism] : Usage
	//
	//	t[ ii ] = n_string_binary2hexchar( f[ i ] / 16 ); ii++;
	//	t[ ii ] = n_string_binary2hexchar( f[ i ] % 16 ); ii++;
	//
	//	use u8* for a variable "f"


	n_posix_char c = 0;


	if ( b < 10 )
	{
		c = b + n_posix_literal( '0' );
	} else {
		c = ( b - 10 ) + n_posix_literal( 'a' );
	}


	return c;
}

u8
n_string_hexchar2binary( n_posix_char c )
{

	// [Mechanism] : Usage
	//
	//	t[ i ]  = n_string_hexchar2binary( f[ ii + 0 ] ) * 16;
	//	t[ i ] += n_string_hexchar2binary( f[ ii + 1 ] );


	u8 b = 0;


	if ( n_string_char_is_digit( c ) )
	{
		b = c - n_posix_literal( '0' );
	} else
	if ( n_string_char_is_alphabet( c ) )
	{
		b = ( c - n_posix_literal( 'a' ) ) + 10;
	}


	return b;
}

#define n_string_truncate( s ) n_string_terminate( s, 0 )

void
n_string_terminate( n_posix_char *str, n_type_int index )
{

	if ( n_string_is_empty( str ) ) { return; }


	// [!] : no index length chacking available

	//if ( index >= n_posix_strlen( str ) ) { return; }


	str[ index ] = N_STRING_CHAR_NUL;


	return;
}

// internal
BOOL
n_string_error( const n_posix_char *arg, n_posix_char *ret )
{

	if ( ret == NULL ) { return TRUE; }

	if ( n_string_is_empty( arg ) )
	{

		n_string_truncate( ret );

		return TRUE;
	}


	return FALSE;
}

#define n_string_copy_literal( a,b ) n_string_copy( n_posix_literal( a ), b )

// internal
void
n_string_copy( const n_posix_char *f, n_posix_char *t )
{

	if ( f ==    t ) { return; }
	if ( t == NULL ) { return; }

	n_string_truncate( t );

	if ( n_string_is_empty( f ) ) { return; }


	n_type_int i = 0;
	n_posix_loop
	{

		if ( f[ i ] == N_STRING_CHAR_NUL ) { break; }

		t[ i ] = f[ i ];

		i++;

	}

	n_string_terminate( t, i );


	return;
}

// internal
void
n_string_copy_partial( const n_posix_char *f, n_posix_char *t, n_type_int len )
{

	// [!] : no length check available


	if ( f ==    t ) { return; }
	if ( t == NULL ) { return; }

	n_string_truncate( t );

	if ( n_string_is_empty( f ) ) { return; }


	if ( len <= 0 ) { return; }

	n_memory_copy_slow( f, t, len * sizeof( n_posix_char ) );

	n_string_terminate( t, len );


	return;
}

#define n_string_free( str ) n_memory_free( str )

#define n_string_new(      cch ) n_string_new_internal( cch, TRUE  )
#define n_string_new_fast( cch ) n_string_new_internal( cch, FALSE )

// internal
n_posix_char*
n_string_new_internal( n_type_int length, BOOL zeroclear )
{
//return NULL;

	// [!] : you need to n_string_free() a returned variable


	n_type_int    b = ( length + N_STRING_NUL_CCH ) * sizeof( n_posix_char );
	n_posix_char *s = (n_posix_char*) n_memory_new( b );


	if ( zeroclear )
	{
		n_memory_zero( s, b );
	} else {
		n_string_truncate( s );
	}


	return s;
}

#define n_string_carboncopy( s ) n_string_carboncopy_length( s, 0 )

// internal
n_posix_char*
n_string_carboncopy_length( const n_posix_char *str, n_type_int length )
{
//return NULL;

	// [!] : you need to n_string_free() a returned variable


	if ( str == NULL )
	{
		//
	} else {
		if ( length == 0 ) { length = n_posix_strlen( str ); }
	}

	n_posix_char *s = (n_posix_char*) n_memory_new( ( length + N_STRING_NUL_CCH ) * sizeof( n_posix_char ) );


	n_string_copy_partial( str, s, length );


	return s;
}

// internal
n_posix_char*
n_string_alloccopy( n_type_int length, const n_posix_char *str )
{

	// [!] : you need to n_string_free() a returned variable


	n_posix_char *s = (n_posix_char*) n_memory_new( ( length + N_STRING_NUL_CCH ) * sizeof( n_posix_char ) );


	// [!] : truncate

	n_type_int len = n_posix_strlen( str );
	if ( length > len ) { length = len; }


	n_string_copy_partial( str, s, length );


	return s;
}

n_type_int
n_string_doublebyte_increment( n_posix_char c )
{

#if defined( N_POSIX_PLATFORM_WINDOWS ) && !defined( UNICODE )

	if ( IsDBCSLeadByte( c ) ) { return 2; }

#endif


	return 1;
}

n_type_int
n_string_cch2cb( const n_posix_char *stream, n_type_int cch )
{

	// [!] : character count to byte count


	n_type_int count_byte = 0;
	n_type_int count_char = 0;
	n_posix_loop
	{

		if ( count_char >= cch ) { break; }

		count_byte += n_string_doublebyte_increment( stream[ count_byte ] );
		count_char += 1;

	}


	return count_byte * sizeof( n_posix_char );
}

n_type_int
n_string_cb2cch( const n_posix_char *stream, n_type_int cb )
{

	// [!] : byte count to character count


	n_type_int count_byte = 0;
	n_type_int count_char = 0;
	n_posix_loop
	{

		n_type_int byte = count_byte * sizeof( n_posix_char );
		if ( byte >= cb ) { break; }

		count_byte += n_string_doublebyte_increment( stream[ count_byte ] );
		count_char += 1;

	}


	return count_char;
}

#define n_string_match( s, w )         n_string_match_fast( s, w, 0, 0 )
#define n_string_match_literal( s, w ) n_string_match( s, n_posix_literal( w ) )

// internal
BOOL
n_string_match_fast( const n_posix_char *stream, const n_posix_char *word, n_type_int streamlen, n_type_int wordlen )
{

	if ( stream == NULL ) { return FALSE; }
	if ( word   == NULL ) { return FALSE; }


	// nothing to do

	if ( stream[ 0 ] != word[ 0 ] ) { return FALSE; }


	if ( streamlen == 0 ) { streamlen = n_posix_strlen( stream ); }
	if (   wordlen == 0 ) {   wordlen = n_posix_strlen( word   ); }


	// [!] : anti-fault

	if ( streamlen < wordlen ) { return FALSE; }


	return n_memory_is_same( stream, word, wordlen * sizeof( n_posix_char ) );
}

#define N_STRING_UPPER 0
#define N_STRING_LOWER 1

#define n_string_upper( a, r ) n_string_upperlower( a, r, N_STRING_UPPER )
#define n_string_lower( a, r ) n_string_upperlower( a, r, N_STRING_LOWER )

void
n_string_upperlower( const n_posix_char *arg, n_posix_char *ret, int mode )
{

	if ( n_string_error( arg, ret ) ) { return; }


	// [!] : don't use this function except for ASCII area comparing
	//
	//	problematic in some languages like turkish language


	const n_posix_char offset = n_posix_literal( 'a' ) - n_posix_literal( 'A' );


	n_string_copy( arg, ret );


	n_type_int i = 0;
	n_posix_loop
	{

		if ( ( mode == N_STRING_UPPER )&&( n_string_is_lower( ret, i ) ) )
		{
			ret[ i ] -= offset;
		} else
		if ( ( mode == N_STRING_LOWER )&&( n_string_is_upper( ret, i ) ) )
		{
			ret[ i ] += offset;
		}


		// [!] : for invalid character sequence

		n_type_int step = n_string_doublebyte_increment( ret[ i ] );
		if ( ( step == 2 )&&( ret[ i + 1 ] == N_STRING_CHAR_NUL ) ) { break; }


		i += step;
		if ( ret[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

// [!] : From = Literal, To = Variable

#define n_string_is_same_strict(         f, t ) ( 0 == n_posix_strcmp( f, t ) )
#define n_string_is_same_strict_literal( f, t ) n_string_is_same_strict( n_posix_literal( f ), t )

#define n_string_is_same_literal( f, t ) n_string_is_same( n_posix_literal( f ), t )

BOOL
n_string_is_same( const n_posix_char *str1, const n_posix_char *str2 )
{

	n_posix_char *s1, *s2;

	if ( str1 == NULL )
	{
		s1 = n_string_carboncopy( N_STRING_EMPTY );
	} else {
		s1 = n_string_carboncopy( str1 );
	}

	if ( str2 == NULL )
	{
		s2 = n_string_carboncopy( N_STRING_EMPTY );
	} else {
		s2 = n_string_carboncopy( str2 );
	}


	// [!] for special case

	if ( s1 == NULL )
	{
		s1 = (n_posix_char*) n_memory_new( 1 * sizeof( n_posix_char ) );
		s1[ 0 ] = N_STRING_CHAR_NUL;
	}

	if ( s2 == NULL )
	{
		s2 = (n_posix_char*) n_memory_new( 1 * sizeof( n_posix_char ) );
		s2[ 0 ] = N_STRING_CHAR_NUL;
	}


	n_string_lower( s1, s1 );
	n_string_lower( s2, s2 );

//n_posix_debug( str1 );
//n_posix_debug( str2 );

	BOOL ret = ( 0 == n_posix_strcmp( s1, s2 ) );

	n_string_free( s1 );
	n_string_free( s2 );


	return ret;
}

void
n_string_remove_blank( const n_posix_char *arg, n_posix_char *ret )
{

	if ( n_string_error( arg, ret ) ) { return; }


	// [Mechanism] : remove multiple SPACE and TAB
	//
	//	"\t test \t test \t" => "test \t test"


	n_type_int i;


	i = 0;
	n_posix_loop
	{
		if ( n_string_is_blank( arg, i ) ) { i++; } else { break; }
	}


	n_string_copy( &arg[ i ], ret );

	if ( n_string_is_empty( ret ) ) { return; }


	i = n_posix_strlen( ret ) - 1;
	n_posix_loop
	{

		if ( n_string_is_blank( ret, i ) )
		{

			ret[ i ] = N_STRING_CHAR_NUL;

			if ( i == 0 ) { break; }
			i--;

		} else {

			break;

		}

	}


	return;
}

void
n_string_safepath( const n_posix_char *arg, n_posix_char *ret )
{

	if ( n_string_error( arg, ret ) ) { return; }


	n_string_copy( arg, ret );


	n_type_int i = 0;
	n_posix_loop
	{

		if (
			( ret[ i ] == n_posix_literal( '\"' ) )
			||
			( ret[ i ] == n_posix_literal( ':' )  )
			||
			( ret[ i ] == n_posix_literal( ';' )  )
			||
			( ret[ i ] == n_posix_literal( '<' )  )
			||
			( ret[ i ] == n_posix_literal( '>' )  )
			||
			( ret[ i ] == n_posix_literal( '?' )  )
			||
			( ret[ i ] == n_posix_literal( '*' )  )
			||
			( ret[ i ] == n_posix_literal( '|' )  )
		)
		{
			ret[ i ] = n_posix_literal( '_' );
		}


		// [!] : for invalid character sequence

		n_type_int step = n_string_doublebyte_increment( ret[ i ] );
		if ( ( step == 2 )&&( ret[ i + 1 ] == N_STRING_CHAR_NUL ) ) { break; }


		i += step;
		if ( ret[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

void
n_string_safename( const n_posix_char *arg, n_posix_char *ret )
{

	if ( n_string_error( arg, ret ) ) { return; }


	n_string_copy( arg, ret );


	n_string_safepath( ret, ret );


	n_type_int i = 0;
	n_posix_loop
	{

		if (
			( ret[ i ] == n_posix_literal( '\\' ) )
			||
			( ret[ i ] == n_posix_literal( '/' )  )
		)
		{
			ret[ i ] = n_posix_literal( '_' );
		}


		// [!] : for invalid character sequence

		n_type_int step = n_string_doublebyte_increment( ret[ i ] );
		if ( ( step == 2 )&&( ret[ i + 1 ] == N_STRING_CHAR_NUL ) ) { break; }


		i += step;
		if ( ret[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

#define N_STRING_QUOTE_ADD 0
#define N_STRING_QUOTE_DEL 1

#define n_string_quote_literal( a,b,c,d,e ) n_string_quote( a, b, n_posix_literal( c ), n_posix_literal( d ), e )

#define n_string_quote_add( a,b,c,d ) n_string_quote( a, b, c, d, N_STRING_QUOTE_ADD )
#define n_string_quote_del( a,b,c,d ) n_string_quote( a, b, c, d, N_STRING_QUOTE_DEL )
#define n_string_quote_add_literal( a,b,c,d ) n_string_quote_add( a, b, n_posix_literal( c ), n_posix_literal( d ) )
#define n_string_quote_del_literal( a,b,c,d ) n_string_quote_del( a, b, n_posix_literal( c ), n_posix_literal( d ) )

#define n_string_doublequote_add( a,r ) n_string_quote_add_literal( a,r, '"','"' )
#define n_string_doublequote_del( a,r ) n_string_quote_del_literal( a,r, '"','"' )

#define n_string_curlybracket_add( a,r ) n_string_quote_add_literal( a,r, '{','}' )
#define n_string_curlybracket_del( a,r ) n_string_quote_del_literal( a,r, '{','}' )

#define n_string_squarebracket_add( a,r ) n_string_quote_add_literal( a,r, '[',']' )
#define n_string_squarebracket_del( a,r ) n_string_quote_del_literal( a,r, '[',']' )

#define n_string_roundbracket_add( a,r ) n_string_quote_add_literal( a,r, '(',')' )
#define n_string_roundbracket_del( a,r ) n_string_quote_del_literal( a,r, '(',')' )

#define n_string_anglebracket_add( a,r ) n_string_quote_add_literal( a,r, '<','>' )
#define n_string_anglebracket_del( a,r ) n_string_quote_del_literal( a,r, '<','>' )

// internal
n_posix_inline BOOL
n_string_quote_is_samechar( const n_posix_char *stream, n_type_int index, n_posix_char c )
{

	if ( stream[ index ] == c ) { return TRUE; }


	return FALSE;
}

void
n_string_quote
(
	const n_posix_char *arg,
	      n_posix_char *ret,
	      n_posix_char  char_open,
	      n_posix_char  char_close,
	               int  mode
)
{

	if ( n_string_error( arg, ret ) ) { return; }


	if ( mode == N_STRING_QUOTE_ADD )
	{

		// [!] : length is guaranteed : strlen( arg ) >= 1

		n_type_int f = 0;
		n_type_int t = n_posix_strlen( arg ) - 1;

		BOOL is_same_f = n_string_quote_is_samechar( arg, f, char_open  );
		BOOL is_same_t = n_string_quote_is_samechar( arg, t, char_close );


		// [!] : nothing to do / for performance

		if ( ( is_same_f )&&( is_same_t ) )
		{
			n_string_copy( arg, ret );
			return;
		}


		if ( is_same_f ) { char_open  = N_STRING_CHAR_NUL; }
		if ( is_same_t ) { char_close = N_STRING_CHAR_NUL; }

		n_type_int    c = n_posix_strlen( arg ) + 2;
		n_posix_char *s = n_string_new_fast( c );
		if ( s != NULL )
		{
			n_posix_snprintf_literal( s, c + 1, "%c%s%c", char_open, arg, char_close );
			n_string_copy( s, ret );

			n_string_free( s );
		}

//n_posix_debug( ret );

	} else {

		// [!] : length is guaranteed : strlen( arg ) >= 1

		n_type_int f = 0;
		n_type_int t = n_posix_strlen( arg ) - 1;

		BOOL is_same_f = n_string_quote_is_samechar( arg, f, char_open  );
		BOOL is_same_t = n_string_quote_is_samechar( arg, t, char_close );


		// [!] : nothing to do / for performance

		if ( ( is_same_f == FALSE )&&( is_same_t == FALSE ) )
		{
			n_string_copy( arg, ret );
			return;
		}


		if ( is_same_f )
		{
			n_posix_snprintf_literal( ret, t, "%s", &arg[ 1 ] );
			t--;
		} else {
			n_string_copy( arg, ret );
		}

		if ( is_same_t ) { ret[ t ] = N_STRING_CHAR_NUL; }

	}


	return;
}

#define N_STRING_ESCAPE_ADD 0
#define N_STRING_ESCAPE_DEL 1

#define n_string_escape_add( a,r ) n_string_escape( a,r, N_STRING_ESCAPE_ADD )
#define n_string_escape_del( a,r ) n_string_escape( a,r, N_STRING_ESCAPE_DEL )

void
n_string_escape( const n_posix_char *arg, n_posix_char *ret, int mode )
{

	if ( n_string_error( arg, ret ) ) { return; }


	n_type_int    l = n_posix_strlen( arg );
	n_posix_char *s = n_string_new_fast( l * 2 );
	if ( s == NULL ) { return; }


	if ( mode == N_STRING_ESCAPE_ADD )
	{

		n_type_int i  = 0;
		n_type_int ii = 0;
		n_posix_loop
		{

			if (
				( arg[ i ] == N_STRING_CHAR_DQUOTE )
				||
				( arg[ i ] == N_STRING_CHAR_BSLASH )
			)
			{
				s[ ii ] = N_STRING_CHAR_BSLASH; ii++;
			}

			n_type_int c = n_string_doublebyte_increment( arg[ i ] );

			s[ ii ] = arg[ i ]; ii++;
			if ( c == 2 )
			{

				// [!] : for invalid character sequence

				if ( arg[ i + 1 ] == N_STRING_CHAR_NUL ) { break; }


				s[ ii ] = arg[ i + 1 ]; ii++;

			}


			i += c;
			if ( arg[ i ] == N_STRING_CHAR_NUL ) { break; }
		}

		n_string_terminate( s, ii );

	} else {

		n_type_int i  = 0;
		n_type_int ii = 0;
		n_posix_loop
		{

			if (
				( arg[ i + 0 ] == N_STRING_CHAR_BSLASH )
				&&
				(
					( arg[ i + 1 ] == N_STRING_CHAR_BSLASH )
					||
					( arg[ i + 1 ] == N_STRING_CHAR_DQUOTE )
				)
			)
			{
				i++;
			}

			n_type_int c = n_string_doublebyte_increment( arg[ i ] );

			s[ ii ] = arg[ i ]; ii++;
			if ( c == 2 )
			{

				// [!] : for invalid character sequence

				if ( arg[ i + 1 ] == N_STRING_CHAR_NUL ) { break; }


				s[ ii ] = arg[ i + 1 ]; ii++;

			}


			i += c;
			if ( arg[ i ] == N_STRING_CHAR_NUL ) { break; }
		}

		n_string_terminate( s, ii );

	}

	n_string_copy( s, ret );

	n_string_free( s );


	return;
}

n_posix_char*
n_string_tab2space( const n_posix_char *arg, n_type_int tabstop, n_type_int *ret_len )
{

	// [!] : you need to n_string_free() a returned variable

	// [!] : multi-line available


	if ( arg == NULL ) { return NULL; }


	n_type_int len_f = 0;//n_posix_strlen( arg );

	if ( FALSE == n_string_tab_is_exist( arg, &len_f ) )
	{
		if ( ret_len != NULL ) { (*ret_len) = len_f; }
		return n_string_carboncopy_length( arg, len_f );
	}


	// [!] : enough size

	if ( tabstop <= 0 ) { tabstop = 8; }

	n_type_int    len_t = len_f * tabstop;
	n_posix_char *s     = n_string_new_fast( len_t );
	if ( s == NULL ) { return NULL; }


	n_type_int i,ii, tab, line;

	i = ii = line = 0;
	n_posix_loop
	{

		if ( arg[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( arg[ i ] == N_STRING_CHAR_TAB )
		{
//s[ii] = '_'; ii++;

			tab = tabstop - ( line % tabstop );

			n_posix_loop
			{

				// [DEBUG] : use '_'

				s[ ii ] = N_STRING_CHAR_SPACE;


				ii++; line++;
				if ( ii >= len_t ) { break; }

				tab--;
				if ( tab <= 0 ) { break; }
			}

		} else {

			s[ ii ] = arg[ i ];

			ii++; line++;
			if ( ii >= len_t ) { break; }
		}


		// Rewind

		if (
			( ( arg[ i ] == N_STRING_CHAR_CR )&&( arg[ i + 1 ] != N_STRING_CHAR_LF ) )
			||
			( arg[ i ] == N_STRING_CHAR_LF )
		)
		{
			line = 0;
			if ( arg[ i + 1 ] != N_STRING_CHAR_LF ) { i++; }
		}


		i++;

	}


	if ( ret_len != NULL ) { (*ret_len) = ii; }


	n_string_terminate( s, ii );


	return s;
}

void
n_string_replace
(
	const n_posix_char *arg,
	      n_posix_char *ret,
	const n_posix_char *from,
	const n_posix_char *to
)
{

	// [!] : multi-line available


	// [ Mechanism ] : ret : needed length
	//
	//	length = n_posix_strlen( arg ) * max( n_posix_strlen( to ), n_posix_strlen( from ) );
	//	byte   = length * sizeof( n_posix_char )


	// [!] : when "to" has "" or "\0"
	//
	//	n_string_replace() deletes "from" from "arg"
	//	for comment-out like behavior, use n_string_commentout()


	if ( n_string_error( arg, ret ) ) { return; }


	// [!] : empty string replacement is acceptable

	if ( n_string_is_empty( from ) ) { return; }
	if ( to == NULL ) { return; }


	// [!] : enough size

	n_type_int cch_f = n_posix_strlen( from );
	n_type_int cch_t = n_posix_strlen( to   );
	n_type_int byte  = n_posix_strlen( arg  ) * n_posix_max_n_type_int( cch_f, cch_t );


	n_type_int len_f = n_posix_strlen( arg );
	n_type_int len_t = byte;


	// [!] : non-break : arg and ret can be same pointer

	n_posix_char *s = n_string_new_fast( len_t );
	if ( s == NULL ) { return; }


	n_type_int  i = 0;
	n_type_int ii = 0;
	n_posix_loop
	{//break;

		n_type_int c = n_string_doublebyte_increment( arg[ i ] );


		if ( n_string_match_fast( &arg[ i ], from, 0, cch_f ) )
		{

			if ( to[ 0 ] == N_STRING_CHAR_NUL )
			{
				// [!] : nothing to do : skipping only
			} else {
				ii += n_posix_snprintf_literal( &s[ ii ], len_t - ii + 1, "%s", to );
			}

			i += cch_f;

		} else {

			s[ ii ] = arg[ i ]; ii++;
			if ( c == 2 ) { s[ ii ] = arg[ i + 1 ]; ii++; }

			if ( ii >= len_t ) { break; }

			i += c;

		}


		if ( i >= len_f ) { break; }
	}


	n_string_terminate( s, ii );
//n_posix_debug_literal( "%s", s );

	n_string_copy( s, ret );

	n_string_free( s );


	return;
}

void
n_string_search
(
	const n_posix_char *stream,
	const n_posix_char *query,
	      n_type_int   *fpos,
              n_type_int   *tpos
)
{

	if ( fpos == NULL ) { return; }
	if ( tpos == NULL ) { return; }


	if (
		( n_string_is_empty( stream ) )
		||
		( n_string_is_empty( query  ) )
	)
	{

		(*fpos) = (*tpos) = 0;

		return;
	}



	n_type_int slen = n_posix_strlen( stream );
	n_type_int qlen = n_posix_strlen( query  );

	if ( slen < qlen ) { return; }


	n_type_int i = (*fpos) = (*tpos);
	n_posix_loop
	{

		if ( n_string_match_fast( &stream[ i ], query, slen - i, qlen ) )
		{

			(*fpos) = i;
			(*tpos) = i + qlen;

			break;
		}


		i += n_string_doublebyte_increment( stream[ i ] );
		if ( i >= slen )
		{

			(*fpos) = i;
			(*tpos) = i;

			break;
		}
	}


	return;
}

#define n_string_search_simple_literal( a, b ) n_string_search_simple( a, n_posix_literal( b ) )

BOOL
n_string_search_simple( const n_posix_char *stream, const n_posix_char *query )
{

	// [Mechanism]
	//
	//	FALSE : not found
	//	TRUE  :     found : "stream" contains "query"


	n_type_int f = 0;
	n_type_int t = 0;


	n_string_search( stream, query, &f, &t );


	return ( f != t );
}

void
n_string_commentout
(
	const n_posix_char *arg,
	      n_posix_char *ret,
	const n_posix_char *commentout
)
{

	if ( n_string_error( arg, ret ) ) { return; }

	if ( n_string_is_empty( commentout ) ) { return; }


	n_string_copy( arg, ret );


	n_type_int len_f = 0;
	n_type_int len_t = n_posix_strlen( commentout );


	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_string_match_fast( &ret[ i ], commentout, len_f, len_t ) )
		{

			n_string_terminate( ret, i );

			break;
		}


		i++;
		if ( ret[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

u32
n_string_str2u32( const n_posix_char *str )
{

	if ( n_string_is_empty( str ) ) { return 0; }


	n_type_int count = 0;

	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_string_is_digit( str, i ) ) { count++; }

		i++;
		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }
	}
//n_posix_debug( "%d", count );

	if ( count == 0 ) { return 0; }


	u32 ret = 0;

	i = 0;
	n_posix_loop
	{//break;

		ret += ( str[ i ] - '0' ) * (u32) pow( 10, (double) count - 1 );
//n_posix_debug( "%d", ( str[ i ] - '0' ) );

		i++;

		count--;
		if ( count <= 0 ) { break; }
	}


	return ret;
}

u64
n_string_str2u64( const n_posix_char *str )
{

	if ( n_string_is_empty( str ) ) { return 0; }


	n_type_int count = 0;

	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_string_is_digit( str, i ) ) { count++; }

		i++;
		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }
	}
//n_posix_debug( "%d", count );

	if ( count == 0 ) { return 0; }


	u64 ret = 0;

	i = 0;
	n_posix_loop
	{//break;

		ret += ( str[ i ] - '0' ) * (u64) pow( 10, (double) count - 1 );
//n_posix_debug( "%d", ( str[ i ] - '0' ) );

		i++;

		count--;
		if ( count <= 0 ) { break; }
	}


	return ret;
}

n_type_real
n_string_str2real( const n_posix_char *str )
{
//n_posix_debug( str );

	if ( str == NULL ) { return 0; }


	// [ Mechanism ]
	//
	//	| blank +/- blank | number dot | real   | NUL    |
	//	|      Mode 0     |   Mode 1   | Mode 2 | break; |


	n_type_real ret  = 0;
	int         mode = 0;
	int         sign = 0;
	n_type_real nth  = 10;


	n_type_int i = 0;
	n_posix_loop
	{

		if ( str[ i ] == n_posix_literal( '\0' ) )
		{
			break;
		} else
		if ( str[ i ] == n_posix_literal( '+' ) )
		{
			if ( sign == 0 )  { sign = +1; } else { break; }
		} else
		if ( str[ i ] == n_posix_literal( '-' ) )
		{
			if ( sign == 0 )  { sign = -1; } else { break; }
		} else
		if ( str[ i ] == n_posix_literal( '.' ) )
		{
			if ( mode == 1 ) { mode = 2; } else { break; }
		} else
		if ( ( str[ i ] >= n_posix_literal( '0' ) )&&( str[ i ] <= n_posix_literal( '9' ) ) )
		{

			if ( mode == 0 )
			{

				mode = 1;
				ret  = (n_type_real) n_posix_atoi( &str[ i ] );

			} else
			if ( mode == 2 )
			{

				n_type_real n = (n_type_real) str[ i ] - n_posix_literal( '0' );

				ret += (n_type_real) n / nth;

				nth *= 10;

			}

		} else {

			if ( mode == 0 )
			{

				if (
					( str[ i ] != n_posix_literal( ' '  ) )
					&&
					( str[ i ] != n_posix_literal( '\t' ) )
				)
				{
					break;
				}

			} else {

				break;

			}

		}

		i++;

	}

	// [!] : default is plus

	if ( sign == 0 ) { sign = 1; }


	return ret * sign;
}

u32
n_string_hex2u32( const n_posix_char *hex, n_type_int count )
{

	// [x] : swscanf() is buggy
	//
	//	WinNT's swscanf() crashes

	// [ Mechanism ]
	//
	//	hex:0x777 = dec:1911 = ( 16 * 16 * 7 ) + ( 16 * 7 ) + ( 7 ) = 1792 + 112 + 7


	if ( n_string_is_empty( hex ) ) { return 0; }


	n_type_int offset = 0;
	if ( n_string_match_literal( hex, "0x" ) ) { offset = 2; }


	n_type_int index = 0;

	n_type_int i = offset;
	n_posix_loop
	{

		if ( i >= count ) { break; }

		if ( hex[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( n_string_is_hex( hex, i ) ) { index++; } else { break; }

		i++;

	}
//n_posix_debug_literal( "%d", index );

	if ( index == 0 ) { return 0; }


	u32 ret = 0;

	i = offset;
	n_posix_loop
	{

		if ( i >= count ) { break; }

		if ( hex[ i ] == N_STRING_CHAR_NUL ) { break; }

		u32 c = hex[ i ];

		if ( n_string_char_is_upper( c ) )
		{
			c = 10 + c - n_posix_literal( 'A' );
		} else
		if ( n_string_char_is_lower( c ) )
		{
			c = 10 + c - n_posix_literal( 'a' );
		} else {
			c = c - n_posix_literal( '0' );
		}

//n_posix_debug_literal( "%d", c );

		c = (u32) pow( 16, (n_type_real) ( index - 1 ) - ( i - offset ) ) * c;

//n_posix_debug_literal( "%d", c );

		ret += c;

		i++;

	}


	return ret;
}

#define N_STRING_INT2STR_CCH_MAX ( 25 )

void
n_string_int2str( n_posix_char *str, n_type_int n )
{

	if ( str == NULL ) { return; }

	if ( n == 0 )
	{
		n_posix_snprintf_literal( str, N_STRING_INT2STR_CCH_MAX, "%d", 0 );
		return;
	}

	const int count = N_STRING_INT2STR_CCH_MAX - 1;

	BOOL minus = ( n < 0 );
	if ( minus )
	{
		n *= -1;
		str[ 0 ] = n_posix_literal( '-' );
	} else {
		str[ 0 ] = n_posix_literal( '+' );
	}

	// [!] : "12345" will be like "+5432100000"

	int i = 1;
	int d = 10;
	n_posix_loop
	{

		str[ i + 0 ] = n_posix_literal( '0' ) + (n_posix_char) ( n % d );
		str[ i + 1 ] = N_STRING_CHAR_NUL;

		n /= d;

		i++;
		if ( i >= count ) { break; }
	}
//return;

	// [!] : find topmost zero

	int len = count - 1;
	n_posix_loop
	{
		if ( str[ len ] != n_posix_literal( '0' ) )
		{
			str[ len + 1 ] = N_STRING_CHAR_NUL;

			break;
		}

		len--;
		if ( len < 1 ) { break; }
	}
//n_posix_debug_literal( " %d : %s ", len, str ); return;

	// [!] : "+54321" will be "+12345"

	i = 1;
	while( len > 1 )
	{//break;

		if ( len < 0 ) { break; }

		n_posix_char swap       = str[ i   ];
		             str[ i   ] = str[ len ];
		             str[ len ] = swap;

		i++; len--;
		if ( i >= len ) { break; }
	}

	// [!] : "+12345" will be "12345"

	if ( minus )
	{
		//
	} else {
		// [x] : MinGW-w64 : overlap is forbidden

		n_posix_char *s = n_string_carboncopy( &str[ 1 ] );

		n_posix_snprintf_literal( str, N_STRING_INT2STR_CCH_MAX, "%s", s );

		n_string_free( s );
	}


	return;
}

// internal
BOOL
n_string_parameter_match( const n_posix_char *stream, const n_posix_char *query, n_type_int query_index )
{

	if ( n_string_is_empty( stream ) ) { return FALSE; }
	if ( n_string_is_empty( query  ) ) { return FALSE; }


	BOOL ret = FALSE;


	n_type_int i = 0;
	n_posix_loop
	{

		if ( N_STRING_CHAR_NUL == stream[ i ] ) { break; }

		n_type_int c = n_string_doublebyte_increment( stream[ i ] );

		if (
			( stream[ i ] == query[ query_index ] )
			&&
			(
				( c == 1 )
				||
				( stream[ i + 1 ] == query[ query_index + 1 ] )
			)
		)
		{
			ret = TRUE;
			break;
		}

		// [!] : for invalid character sequence

		if ( ( c == 2 )&&( N_STRING_CHAR_NUL == stream[ i + 1 ] ) ) { break; }

		i += c;

	}


	return ret;
}

#define n_string_parameter_count_literal( a, b, c ) n_string_parameter_count( a, n_posix_literal( b ), n_posix_literal( c ) )
#define n_string_parameter_count(         a, b, c ) n_string_parameter_internal( a, b, c, 0, NULL, TRUE )

#define n_string_parameter_literal( a, b, c, d, e ) n_string_parameter( a, n_posix_literal( b ), n_posix_literal( c ), d, e )

#define n_string_parameter( a, b, c, d, e ) n_string_parameter_internal( a, b, c, d, e, FALSE )

// internal
n_type_int
n_string_parameter_internal
(
	const n_posix_char *str,
	const n_posix_char *delimiter,
	const n_posix_char *quotation,
	      n_type_int    index,
	      n_posix_char *ret,
	      BOOL          is_count
)
{

	// [Mechanism]
	//
	//	str       : "test0 test1 'test2 test2'"
	//	delimiter : N_STRING_SPACE
	//	quotation : N_STRING_SQUOTE
	//	index     :  [ 0 ] [ 1 ] [     2     ]

	// [!]
	//	this module cannot handle a complicated input
	//
	//	use for a commandline input
	//
	//	this module is DBCS safe

	// [!] : "ret" needs strlen( str ) or larger


	// [!] : when str and ret are the same pointer

	n_posix_char *s = n_string_carboncopy( str );
	if ( ret != NULL ) { n_string_truncate( ret ); }


	if (
		( n_string_is_empty( s ) )
		||
		( delimiter == NULL )
	)
	{
		n_string_free( s );
		return 0;
	}

	BOOL quote_onoff = ( FALSE == n_string_is_empty( quotation ) );
	BOOL quote       = FALSE;
	BOOL first       = TRUE;

	// [!] : auto-skipping when space character is used
	BOOL is_space = n_string_parameter_match( N_STRING_SPACE, delimiter, 0 );

	n_type_int i = 0;
	n_type_int j = 0;
	n_type_int x = 0;
	n_posix_loop
	{

		if ( N_STRING_CHAR_NUL == s[ i ] ) { break; }

		// [!] : delimiter first, quotation second

		BOOL found = FALSE;

		if ( quote == FALSE )
		{

			n_type_int ii = 0;
			n_posix_loop
			{

				BOOL b = n_string_parameter_match( delimiter, s, i + ii );
				if ( b == FALSE ) { break; } else { found = b; }

				ii += n_string_doublebyte_increment( s[ i + ii ] );

				if ( is_space == FALSE ) { break; }

			}

			i += ii;
			if ( N_STRING_CHAR_NUL == s[ i ] ) { break; }

			if ( ( found )&&( first == FALSE ) ) { x++; }

		}

		BOOL q = FALSE;

		if ( quote_onoff )
		{

			BOOL pre = FALSE;

			if ( i == 0 )
			{
				pre = TRUE;
			} else {
				n_type_int k = 0;
				n_posix_loop
				{
					n_type_int c = n_string_doublebyte_increment( s[ k ] );
					if ( ( k + c ) >= i ) { break; }
					k += c;
				}
				pre = n_string_parameter_match( delimiter, s, k );
//n_posix_debug_literal( "%d %d", i - 1, k );
			}

			n_type_int ii = 0;
			n_posix_loop
			{

				BOOL b = n_string_parameter_match( quotation, s, i + ii );
				if ( b == FALSE ) { break; } else { q = b; }

				ii += n_string_doublebyte_increment( s[ i + ii ] );

			}

			i += ii;
			if ( N_STRING_CHAR_NUL == s[ i ] ) { break; }

			BOOL post = n_string_parameter_match( delimiter, s, i );


			if ( q )
			{

				if ( quote )
				{
					if ( post ) { quote = FALSE; } else { i -= ii; q = FALSE; }
				} else {
					if ( pre  ) { quote = TRUE ; } else { i -= ii; q = FALSE; }
				}

			}

		}

		if ( ( found == FALSE )&&( q == FALSE ) )
		{

			n_type_int c = n_string_doublebyte_increment( s[ i ] );

			if ( ( ret != NULL )&&( is_count == FALSE )&&( x == index ) )
			{
				ret[ j ] = s[ i ]; j++;
				if ( c != 1 ) { ret[ j ] = s[ i + 1 ]; j++; }
			}

			i += c;

			if ( is_count == FALSE )
			{
				if ( ( 0 <= index )&&( x > index ) ) { break; }
			}

		}

		first = FALSE;

	}

	if ( ret != NULL ) { n_string_terminate( ret, j ); }


	n_string_free( s );


//n_posix_debug_literal( "%s", ret );
	return 1 + x;
}

typedef int (*n_string_compare_custom_callback)( n_posix_char a, n_posix_char b );

const n_posix_char n_string_compare_table[] = {

	n_posix_literal( '\'' ),
	n_posix_literal(  '-' ),
	n_posix_literal(  '!' ),
	n_posix_literal(  '#' ),
	n_posix_literal(  '$' ),
	n_posix_literal(  '%' ),
	n_posix_literal(  '&' ),
	n_posix_literal(  '(' ),
	n_posix_literal(  ')' ),
	n_posix_literal(  ',' ),
	n_posix_literal(  ';' ),
	n_posix_literal(  '@' ),
	n_posix_literal(  '[' ),
	n_posix_literal(  ']' ),
	n_posix_literal(  '^' ),
	n_posix_literal(  '_' ),
	n_posix_literal(  '`' ),
	n_posix_literal(  '{' ),
	n_posix_literal(  '}' ),
	n_posix_literal(  '~' ),
	n_posix_literal(  '+' ),
	n_posix_literal(  '=' ),
	                   0
};

BOOL
n_string_compare_is_sign( n_posix_char c )
{

	BOOL is_found = FALSE;


	if (
		( ( c >= 0x0021 )&&( c <= 0x002f ) )
		||
		( ( c >= 0x003a )&&( c <= 0x0040 ) )
		||
		( ( c >= 0x005b )&&( c <= 0x0060 ) )
		||
		( ( c >= 0x007b )&&( c <= 0x007e ) )
	)
	{
		is_found = TRUE;
	}

/*
	int i = 0;
	n_posix_loop
	{
		if ( c == n_string_compare_table[ i ] ) { is_found = TRUE; break; }

		i++;
		if ( 0 == n_string_compare_table[ i ] ) { break; }
	}
*/

	return is_found;
}

int
n_string_compare_explorer_compatible( n_posix_char a, n_posix_char b )
{

	if ( n_string_compare_is_sign( a ) )
	{

		int i = 0;
		n_posix_loop
		{
			if ( a == n_string_compare_table[ i ] ) { a = i; break; }

			i++;
			if ( 0 == n_string_compare_table[ i ] ) { a = i; break; }
		}

	} else {

		a = 100;

	}

	if ( n_string_compare_is_sign( b ) )
	{

		int i = 0;
		n_posix_loop
		{
			if ( b == n_string_compare_table[ i ] ) { b = i; break; }

			i++;
			if ( 0 == n_string_compare_table[ i ] ) { b = i; break; }
		}

	} else {

		b = 100;

	}

	if ( a > b ) { return  1; }
	if ( b > a ) { return -1; }


	return 0;
}

#define n_string_compare(        a, b ) n_string_compare_internal( a, b, FALSE )
#define n_string_compare_strict( a, b ) n_string_compare_internal( a, b, TRUE  )

static BOOL                             n_string_compare_charcode_only        = TRUE;
static BOOL                             n_string_compare_CompareString_onoff  = FALSE;
static n_string_compare_custom_callback n_string_compare_custom_callback_func = NULL;

// internal
int
n_string_compare_character( n_posix_char a, n_posix_char b )
{

#ifdef UNICODE

	// [!] : katakana to hiragana

	if ( n_string_compare_charcode_only == FALSE )
	{
		if ( ( a >= 0x30a1 )&&( a <= 0x30fe ) ) { a -= 0x30a1 - 0x3041; }
		if ( ( b >= 0x30a1 )&&( b <= 0x30fe ) ) { b -= 0x30a1 - 0x3041; }
	}

#endif // #ifdef UNICODE


	int compare = 0;


	if ( n_string_compare_charcode_only == FALSE )
	{
		if ( n_string_compare_custom_callback_func != NULL )
		{
			compare = n_string_compare_custom_callback_func( a, b );
			if ( compare ) { return compare; }
		}
	}


	compare = 0;

	if ( a < b ) { compare--; }
	if ( a > b ) { compare++; }


	return compare;
}

// internal
int
n_string_compare_internal( const n_posix_char *arg_a, const n_posix_char *arg_b, BOOL is_strict )
{

#ifdef N_POSIX_PLATFORM_MAC

	// [!] : this from developer.apple.com

	NSString *string1 = [NSString stringWithUTF8String:arg_a];
	NSString *string2 = [NSString stringWithUTF8String:arg_b];

	static NSStringCompareOptions comparisonOptions =
		NSCaseInsensitiveSearch  | NSNumericSearch |
		NSWidthInsensitiveSearch | NSForcedOrderingSearch;

	NSRange string1Range = NSMakeRange( 0, [string1 length] );


	return (int) [string1
		compare:string2
		options:comparisonOptions
		  range:string1Range
		 locale:nil
	];

#endif // #ifdef N_POSIX_PLATFORM_MAC


	if ( n_string_compare_CompareString_onoff )
	{
#ifdef N_POSIX_PLATFORM_WINDOWS

		// [x] : buggy : ANSI version : broken sort
#ifdef UNICODE
		int n_SORT_DIGITSASNUMBERS = 8;
#else  // #ifdef UNICODE
		int n_SORT_DIGITSASNUMBERS = 0;
#endif // #ifdef UNICODE

		return CompareString( LOCALE_SYSTEM_DEFAULT, n_SORT_DIGITSASNUMBERS, arg_a,-1, arg_b,-1 ) - 2;
#endif // #ifndef N_POSIX_PLATFORM_WINDOWS
	}


	// [!] : strcmp() compatible

	// ret =  0 : same string / impossible
	// ret = -1 : a[] is smaller
	// ret =  1 : a[] is bigger  : need to swap


	int compare = 0;


	// [!] : empty checker

	compare = 0;

	if ( n_string_is_empty( arg_a ) ) { compare--; }
	if ( n_string_is_empty( arg_b ) ) { compare++; }

	if ( compare ) { return compare; }


	// [!] : dot-file checker

	compare = 0;

	if ( arg_a[ 0 ] == N_STRING_CHAR_DOT ) { compare--; }
	if ( arg_b[ 0 ] == N_STRING_CHAR_DOT ) { compare++; }

	if ( compare ) { return compare; }


	n_posix_char *a;
	n_posix_char *b;

	if ( is_strict == FALSE )
	{
		a = n_string_carboncopy( arg_a ); n_string_lower( a, a );
		b = n_string_carboncopy( arg_b ); n_string_lower( b, b );
	} else {
		a = (n_posix_char*) arg_a;
		b = (n_posix_char*) arg_b;
	}


	int is_end   = 0;
	int is_digit = 0;

	n_type_int i = 0;
	n_posix_loop
	{//break;

		// [!] : NUL("\0") checker

		is_end = compare = 0;

		if ( a[ i ] == N_STRING_CHAR_NUL ) { compare--; is_end++; }
		if ( b[ i ] == N_STRING_CHAR_NUL ) { compare++; is_end++; }

		if ( compare ) { break; }
		if ( is_end  ) { break; }


		// [!] : sort order : signs => digits => characters

		is_digit = 0;

		if ( n_string_char_is_digit( a[ i ] ) ) { is_digit++; }
		if ( n_string_char_is_digit( b[ i ] ) ) { is_digit++; }

		if ( is_digit == 0 )
		{

			if ( n_string_compare_charcode_only == FALSE )
			{
				compare = 0;

				if ( n_string_compare_is_sign( a[ i ] ) ) { compare--; }
				if ( n_string_compare_is_sign( b[ i ] ) ) { compare++; }

				if ( compare ) { break; }
			}


			compare = n_string_compare_character( a[ i ], b[ i ] );

			if ( compare ) { break; }

		} else
		if ( is_digit == 1 )
		{

			if ( n_string_compare_charcode_only == FALSE )
			{
				compare = 0;

				if ( n_string_compare_is_sign( a[ i ] ) ) { compare--; }
				if ( n_string_compare_is_sign( b[ i ] ) ) { compare++; }

				if ( compare ) { break; }
			}

		} else
		//if ( is_digit == 2 )
		{

			// [!] : digit comparison

			// [!] : length based comparison

			int digit_a = 0;
			int digit_b = 0;

			int ii = 0;
			n_posix_loop
			{

				is_end = compare = 0;

				if ( n_string_char_is_digit( a[ i + ii ] ) ) { digit_a++; } else { is_end++; compare--; }
				if ( n_string_char_is_digit( b[ i + ii ] ) ) { digit_b++; } else { is_end++; compare++; }

				if ( compare ) { break; }
				if ( is_end  ) { break; }

				ii++;

			}


			compare = 0;

			if ( digit_a < digit_b ) { compare--; }
			if ( digit_a > digit_b ) { compare++; }

			if ( compare ) { break; }


			// [!] : when same length

			n_posix_loop
			{

				if ( n_string_compare_charcode_only == FALSE )
				{
					compare = 0;

					if ( n_string_compare_is_sign( a[ i ] ) ) { compare--; }
					if ( n_string_compare_is_sign( b[ i ] ) ) { compare++; }

					if ( compare ) { break; }
				}


				compare = n_string_compare_character( a[ i ], b[ i ] );

				if ( compare ) { break; }

				i++;

			}

			break;

		}


		i++;

	}


	if ( is_strict == FALSE )
	{
		n_string_free( a );
		n_string_free( b );
	}


	return compare;
}

#define n_string_wildcard_literal( a, b ) n_string_wildcard( n_posix_literal( a ), b )

BOOL
n_string_wildcard( const n_posix_char *str_match, const n_posix_char *str_query )
{

	if ( n_string_is_same( str_match, str_query ) ) { return TRUE; }

	if ( n_string_is_same_literal( "*", str_match ) ) { return TRUE; }


	BOOL ret = FALSE;

	if ( n_string_is_empty( str_match ) ) { return ret; }
	if ( n_string_is_empty( str_query ) ) { return ret; }


	n_type_int cch_match = n_posix_strlen( str_match );
	n_type_int cch_query = n_posix_strlen( str_query );

	if ( cch_match > cch_query ) { return ret; }

	if (
		( cch_query == 1 )
		&&
		( n_string_is_same_literal( "?", str_match ) )
	)
	{
		return TRUE;
	}


	n_type_int m = 0;
	n_type_int q = 0;
	n_posix_loop
	{

		if ( str_match[ m ] == n_posix_literal( '?' ) )
		{
//n_posix_debug_literal( " ! " );

			// [!] : skip

		} else
		if ( str_match[ m ] == n_posix_literal( '*' ) )
		{

			if ( str_match[ m + 1 ] == N_STRING_CHAR_NUL ) { ret = TRUE; break; }
			m++;
/*
			n_posix_loop
			{

				// [x] : this cannot handle like "*color*" with "oc_color.c"
				//
				//	'c' in "oc_" will be TRUE

				if ( str_match[ m ] == str_query[ q ] ) { break; }

				q++;
				if ( str_query[ q ] == N_STRING_CHAR_NUL ) { break; }
			}

			if ( str_query[ q ] == N_STRING_CHAR_NUL ) { break; }
*/
//

			n_type_int longest = 0;
			n_type_int index_q = 0;

			n_type_int mm = 0;
			n_type_int qq = 0;
			n_posix_loop
			{
//n_posix_debug_literal( " %c %c ", str_match[ m + mm ], str_query[ q + qq ] );

				if ( str_match[ m + mm ] == n_posix_literal( '?' ) )
				{
//n_posix_debug_literal( " ! " );
					longest = 0;
					m = m + mm - 1;
					q = q + qq - 1;

					break;

				} else
				if ( str_match[ m + mm ] == n_posix_literal( '*' ) )
				{

					longest = 0;
					m = m + mm - 1;
					q = q + qq - 1;

					break;

				} else
				if ( str_match[ m + mm ] == str_query[ q + qq ] )
				{
//n_posix_debug_literal( " %c ", str_match[ m + mm ] );

					mm++;
					qq++;

					if (
						( str_match[ m + mm ] == N_STRING_CHAR_NUL )
						&&
						( str_query[ q + qq ] == N_STRING_CHAR_NUL )
					)
					{
						return TRUE;
					} else
					if ( str_match[ m + mm ] == N_STRING_CHAR_NUL )
					{
						return FALSE;
					}

				} else {

					if ( longest < mm ) { index_q = qq; longest = mm; }

					mm = 0;

					qq++;
					if ( str_query[ q + qq ] == N_STRING_CHAR_NUL )
					{
//n_posix_debug_literal( " %d %d ", cch_match, ( m + mm ) );
						if ( cch_match > ( m + mm ) ) { return FALSE; }
						break;
					}

				}

			}
//n_posix_debug_literal( " longest : %d ", longest );

			if ( longest != 0 )
			{
//n_posix_debug_literal( " index_q : %d ", index_q );
				m = m + longest - 1;
				q = q + index_q - 1;
			}
//

		} else
		if ( str_match[ m ] != str_query[ q ] )
		{
			m = q = 0;
			break;
		}

		m++;
		q++;
		if ( str_match[ m ] == N_STRING_CHAR_NUL ) { break; }
		if ( str_query[ q ] == N_STRING_CHAR_NUL ) { break; }
	}


//n_posix_debug_literal( " %d/%d : %d/%d ", m, cch_match, q, cch_query );

	if ( ( m == cch_match )&&( q == cch_query ) ) { ret = TRUE; }


	return ret;
}

#define n_string_commandline_option_literal( o, p ) n_string_commandline_option( n_posix_literal( o ), p )

BOOL
n_string_commandline_option( const n_posix_char *option, n_posix_char *path )
{

	// [Mechanism] : "-option C:\\Path"
	//
	//	ret = FALSE : path = "-option C:\\Path"
	//	ret = TRUE  : path = "C:\\Path"


	BOOL ret = FALSE;


//n_posix_debug_literal( "%s : %s", option, path );
//n_string_replace( path,path, N_STRING_SPACE, n_posix_literal( "_" ) );

	if ( n_string_match( path, option ) )
	{

		ret = TRUE;

		// [x] : MinGW-w64 : overlap is forbidden

		n_type_int    c = n_posix_strlen( &path[ n_posix_strlen( option ) ] );
		n_posix_char *s = n_string_carboncopy( &path[ n_posix_strlen( option ) ] );

		n_posix_snprintf_literal( path, c, "%s", s );

		n_string_free( s );

		n_string_remove_blank( path, path );

		if ( 1 == n_string_parameter_count_literal( path, " ", "\"" ) )
		{
			n_string_doublequote_del( path, path );
		}

	}


	return ret;
}


#endif // _H_NONNON_NEUTRAL_STRING

