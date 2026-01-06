// Nonnon HTML : Parser
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_HTML
#define _H_NONNON_NEUTRAL_HTML




#ifdef N_POSIX_PLATFORM_MAC

#include "../mac/n_txtbox.c"

#else  // #ifdef N_POSIX_PLATFORM_MAC

#include "./txt.c"

#endif // #ifdef N_POSIX_PLATFORM_MAC




// inherit

#define n_html_zero         n_txt_zero
#define n_html_alias        n_txt_alias
#define n_html_error        n_txt_error
#define n_html_debug        n_txt_debug
#define n_html_free         n_txt_free
#define n_html_new          n_txt_new
#define n_html_save         n_txt_save
#define n_html_save_literal n_txt_save_literal
#define n_html_add          n_txt_add
#define n_html_add_literal  n_txt_add_literal
#define n_html_mod          n_txt_mod
#define n_html_mod_literal  n_txt_mod_literal
#define n_html_del          n_txt_del
#define n_html_swap         n_txt_swap


typedef n_txt n_html;




#define n_html_element_literal( a,b,c,d ) n_html_element( a, n_posix_literal( b ), n_posix_literal( c ), d )

n_posix_char*
n_html_element
(
	const n_posix_char *stream,
	const n_posix_char *start,
	const n_posix_char *end,
	      n_type_int   *cch_resume
)
{

	// [!] : you need to free() a returned variable

	n_posix_char *ret = NULL;


	if ( n_string_is_empty( stream ) ) { return ret; }


	n_posix_bool found   = n_posix_false;
	n_type_int   cch_end = n_posix_strlen( end );


	n_type_int f = 0;
	n_type_int t = 0;
	n_type_int i = 0; if ( cch_resume != NULL ) { i = (*cch_resume); }
	n_posix_loop
	{

		if (
			( found == n_posix_false )
			&&
			( n_string_match( &stream[ i ], start ) )
		)
		{

			found = n_posix_true;

			f = i;

		} else
		if (
			( found )
			&&
			( n_string_match( &stream[ i ], end ) )
		)
		{

			t = i + cch_end;

			n_type_int cch = t - f;

			ret = n_string_new( cch );
			n_string_copy_partial( &stream[ f ], ret, cch );


			break;
		}


		i++;
		if ( stream[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	if ( cch_resume != NULL ) { (*cch_resume) = i; }


	return ret;
}

#define n_html_element_match_literal( a,b ) n_html_element_match( a, n_posix_literal( b ) )

n_posix_bool
n_html_element_match( const n_posix_char *stream, const n_posix_char *elementname )
{

	if ( n_string_is_empty( stream ) ) { return 0; }


	n_posix_char *s = n_string_carboncopy( stream );
	n_posix_bool  ret;


	n_string_anglebracket_del( s, s );

	n_string_commentout( s,s, N_STRING_SPACE );
	n_string_commentout( s,s, N_STRING_TAB   );

//n_posix_debug( s );

	ret = n_string_is_same( s, elementname );


	n_memory_free( s );


	return ret;
}

#define n_html_attribute_literal( a,b,c,d ) n_html_attribute( a, n_posix_literal( b ), c, d )

void
n_html_attribute
(
	const n_posix_char *stream,
	      n_posix_char  delimiter,
	      n_html       *lval,
	      n_html       *rval
)
{

	// [Mechanism]
	//
	//	a : <! and <? will not be handled here
	//	b : HTML/CSS compatible


	// [!] : stream = "<Element LValue delim RValue ... >"


	if ( n_string_is_empty( stream ) ) { return; }


	if ( n_html_error( lval ) ) { n_html_new( lval ); }
	if ( n_html_error( rval ) ) { n_html_new( rval ); }


	n_posix_char *s, *v;
	n_type_int    i, len, phase,quote, start;


	// Phase 1 : "<Element LValue delim RValue ... >" => "Lvalue delim Rvalue ..."

	s = n_string_carboncopy( stream );
	n_string_anglebracket_del( s, s );

	i = 0;
	n_posix_loop
	{

		if ( s[ i ] == N_STRING_CHAR_SPACE )
		{

			n_string_copy( &s[ i + 1 ], s );

			break;
		}


		i++;
		if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }
	}

//n_posix_debug( s );


	i = phase = quote = start = 0;
	n_posix_loop
	{//break;

		if ( ( phase == 0 ) && ( s[ i ] != N_STRING_CHAR_SPACE ) )
		{

			phase++;

			start = i;

		} else
		if ( ( phase == 1 ) && ( s[ i ] == delimiter ) )
		{

			phase++;

			len = i - start;
			v   = n_string_alloccopy( len, &s[ start ] );

			n_string_remove_blank( v, v );
			n_html_add( lval, lval->sy, v );

			n_memory_free( v );

		} else
		if ( ( phase == 2 ) && ( s[ i ] != N_STRING_CHAR_SPACE ) )
		{

			phase++;
			start = i;

			if ( s[ i ] == N_STRING_CHAR_DQUOTE ) { start++; quote = s[ i ]; } else
			if ( s[ i ] == N_STRING_CHAR_SQUOTE ) { start++; quote = s[ i ]; }

		} else
		if (
			( phase == 3 )
			&&
			(
				( s[ i ] == N_STRING_CHAR_NUL  )
				||
				( ( quote == 0 ) && ( s[ i ] == N_STRING_CHAR_SPACE ) )
				||
				( ( quote == 0 ) && ( s[ i ] == N_STRING_CHAR_SEMICOLON ) )
				||
				( ( quote != 0 ) && ( s[ i ] == quote ) )
			)
		)
		{

			phase = quote = 0;

			len = i - start;
			v   = n_string_alloccopy( len, &s[ start ] );

			n_string_remove_blank( v, v );
			n_html_add( rval, rval->sy, v );

			n_memory_free( v );

		}



		i++;
		if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	n_memory_free( s );


	return;
}

void
n_html_whitespace_remove( n_posix_char *s )
{

	if ( n_string_is_empty( s ) ) { return; }


	n_type_int i = 0;
	n_posix_loop
	{

		if (
			( s[ i + 0 ] == N_STRING_CHAR_SPACE )
			&&
			( s[ i + 1 ] == N_STRING_CHAR_SPACE )
		)
		{
			n_string_copy( &s[ i + 1 ], &s[ i + 0 ] );
			i--;
		}


		i++;
		if ( s[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

n_posix_bool
n_html_load( n_html *html, const n_posix_char *fname )
{

#ifdef N_POSIX_PLATFORM_MAC

	if ( n_txt_load_utf8( (n_txt*) html, fname ) ) { return n_posix_true; }

	if ( html->readonly ) { return n_posix_true; }

#else  // #ifdef N_POSIX_PLATFORM_MAC

	if ( n_txt_load( (n_txt*) html, fname ) ) { return n_posix_true; }

#endif // #ifdef N_POSIX_PLATFORM_MAC


	n_html ret; n_html_zero( &ret ); n_html_new( &ret );


	n_type_int cch = (u32) html->byte / sizeof( n_posix_char );


	n_posix_bool pre = n_posix_false;


	n_type_int i  = 0;
	n_type_int ii = 0;
	n_posix_loop
	{

		n_posix_char *s = n_html_element_literal( html->stream, "<", ">", &i );

		n_string_replace( s,s, N_STRING_CR,  N_STRING_SPACE );
		n_string_replace( s,s, N_STRING_LF,  N_STRING_SPACE );
		n_string_replace( s,s, N_STRING_TAB, N_STRING_SPACE );

		n_html_whitespace_remove( s );

//n_posix_debug( s );

		if ( n_html_element_match_literal( s, "pre" ) )
		{
			pre = n_posix_true;
		} else
		if ( n_html_element_match_literal( s, "/pre" ) )
		{
			pre = n_posix_false;
		}

		n_html_add( &ret, ii, s );
		ii++;

		n_memory_free( s );


		s = n_html_element_literal( html->stream, ">", "<", &i );
		n_string_quote_del_literal( s,s, '>','<' );
//n_posix_debug( s );

		if ( pre == n_posix_false )
		{

			n_string_replace( s,s, N_STRING_CRLF, N_STRING_EMPTY );
			n_string_replace( s,s, N_STRING_TAB,  N_STRING_EMPTY );

			n_html_whitespace_remove( s );

		} else {

			n_string_quote_del( s,s, N_STRING_CHAR_CR,N_STRING_CHAR_LF );
			n_string_quote_del( s,s, N_STRING_CHAR_LF,N_STRING_CHAR_CR );

		}

		if ( n_posix_false == n_string_is_empty( s ) )
		{
			n_html_add( &ret, ii, s );
			ii++;
		}

		n_memory_free( s );


		if ( i >= cch ) { break; }
	}
//n_posix_debug_literal( " ! " );


	n_html_free( html );
	n_html_alias( &ret, html );


	return n_posix_false;
}

void
n_html_debug_iotest( const n_posix_char *cmdline )
{

	n_html html, lval,rval;


	n_html_zero( &html );
	if ( n_html_load( &html, cmdline ) ) { return; }


	n_html_zero( &lval );
	n_html_zero( &rval );

	n_type_int i = 0;
	n_posix_loop
	{

		n_html_attribute_literal( html.line[ i ], '=', &lval,&rval );


		if (
			( lval.sy != 0 )
			&&
			( n_html_element_match_literal( lval.line[ lval.sy - 1 ], "style" ) )
		)
		{
			n_html_attribute_literal( rval.line[ rval.sy - 1 ], ':', &lval,&rval );
		}


		i++;
		if ( i >= html.sy ) { break; }
	}

	n_html_save_literal( &lval, "lval.txt" );
	n_html_save_literal( &rval, "rval.txt" );


	n_html_save_literal( &html, "result.txt" );
	n_html_free( &html );


	return;
}


#endif // _H_NONNON_NEUTRAL_HTML

