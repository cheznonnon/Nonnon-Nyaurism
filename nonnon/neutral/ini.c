// OrangeCat
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism] : Escapement
//
//	use n_ini_value_escape()
//
//	[ not needed : L = R ]
//
//		.URL / Internet Shortcut
//		.INI desktop.ini
//		.INI Integer
//
//	[ needed : ""C:\Program Files"" = "\"C:\\Program Files\"" ]
//
//		.REG / Registry File
//		.INI String with space
//		.INI String with double-quotation itself
//
//	[!] : Vista or later : Desktop.ini with double-quotation makes "not found" problem
//
//	[!] : don't use n_string_escape_add() + n_string_doublequote_add()
//
//		"" will be "\"\"
//		"\"\"" is accurate result


// [Mechanism] : Section Name
//
//	[!] : these are handled as same
//
//		[Section Name]
//		[ Section Name ]


// [Mechanism] : when not loaded
//
//	a relative path needs n_win_exedir2curdir()




#ifndef _H_NONNON_NEUTRAL_INI
#define _H_NONNON_NEUTRAL_INI




#include "./string_path.c"
#include "./txt.c"




// inherit

#define n_ini_error( p )       n_txt_error( (void*) p )
#define n_ini_debug( p )       n_txt_debug( (void*) p )
#define n_ini_new(   p )       n_txt_new(   (void*) p )

#define n_ini_add( p, i, s ) n_txt_add(       (void*) p, i, s )
#define n_ini_mod( p, i, s ) n_txt_mod(       (void*) p, i, s )
#define n_ini_del( p, i    ) n_txt_del(       (void*) p, i    )
#define n_ini_get( p, i    ) n_txt_get(       (void*) p, i    )
#define n_ini_set( p, i, s ) n_txt_set(       (void*) p, i, s )

#define n_ini_add_literal( p,i,s ) n_txt_add_literal( (void*) p, i, s )
#define n_ini_mod_literal( p,i,s ) n_txt_mod_literal( (void*) p, i, s )
#define n_ini_set_literal( p,i,s ) n_txt_set_literal( (void*) p, i, s )

#define n_ini_swap( p, a, b ) n_txt_swap( (void*) p, a, b )




typedef struct {

	// [!] : override n_vector

	void       **line;
	n_type_int   sy;

	void        *stream;
	n_type_int   sx;
	n_type_int   byte;


	// [!] : override n_txt

	int          newline;
	int          unicode;
	n_posix_bool readonly;

} n_ini;




#define n_ini_zero( p ) n_memory_zero( p, sizeof( n_ini ) )

void
n_ini_free( n_ini *p )
{

	if ( n_ini_error( p ) ) { return; }


	n_txt_free( (void*) p );


	n_ini_zero( p );


	return;
}

#define n_ini_save_literal( p, fname ) n_ini_save( p, n_posix_literal( fname ) )

n_posix_bool
n_ini_save( n_ini *p, const n_posix_char *fname )
{

	if ( n_ini_error( p ) ) { return n_posix_true; }


	if ( p->readonly ) { return n_posix_true; }


	n_txt_stream( (void*) p );


	return n_txt_save_main( (void*) p, fname );
}

void
n_ini_value_escape( n_posix_char *str )
{

	// Length : [ ( strlen(str) * 2 ) + 2 + 1 ]


	n_posix_char *s ;


	s = n_string_alloccopy( n_posix_strlen( str ) * 2, str );

	n_string_escape_add( s, s );

	n_posix_sprintf_literal( str, "%c%s%c", N_STRING_CHAR_DQUOTE, s, N_STRING_CHAR_DQUOTE );

	n_memory_free( s );


	return;
}

#ifdef N_POSIX_PLATFORM_MAC

n_posix_bool
n_ini_load( n_ini *ini, const n_posix_char *fname )
{

	if ( ini == NULL ) { return n_posix_true; }


	extern n_posix_bool n_txt_load_utf8_internal( n_txt *txt, const n_posix_char *fname, void *stream, n_type_int byte );

	if ( n_txt_load_utf8_internal( (void*) ini, fname, NULL, 0 ) )
	{

		n_ini_new( ini );

		return n_posix_true;
	}

	if ( ini->readonly )
	{

		n_ini_new( ini );

		return n_posix_true;
	}


	n_type_int i = 0;
	n_posix_loop
	{

		n_string_remove_blank( ini->line[ i ], ini->line[ i ] );


		i++;
		if ( i >= ini->sy ) { break; }
	}


	return n_posix_false;
}

#else  // #ifdef N_POSIX_PLATFORM_MAC

n_posix_bool
n_ini_load( n_ini *ini, const n_posix_char *fname )
{

	if ( ini == NULL ) { return n_posix_true; }


	if ( n_txt_load( (void*) ini, fname ) )
	{

		n_ini_new( ini );

		return n_posix_true;
	}


	n_type_int i = 0;
	n_posix_loop
	{

		n_string_remove_blank( ini->line[ i ], ini->line[ i ] );


		i++;
		if ( i >= ini->sy ) { break; }
	}


	return n_posix_false;
}

#endif // #ifdef N_POSIX_PLATFORM_MAC

#define N_INI_SECTION_ADD 0
#define N_INI_SECTION_DEL 1
#define N_INI_SECTION_CHK 2

#define n_ini_section_add( ini, sct ) n_ini_section( ini, sct, N_INI_SECTION_ADD )
#define n_ini_section_del( ini, sct ) n_ini_section( ini, sct, N_INI_SECTION_DEL )
#define n_ini_section_chk( ini, sct ) n_ini_section( ini, sct, N_INI_SECTION_CHK )

#define n_ini_section_add_literal( ini, sct ) n_ini_section_add( ini, n_posix_literal( sct ) )
#define n_ini_section_del_literal( ini, sct ) n_ini_section_del( ini, n_posix_literal( sct ) )
#define n_ini_section_chk_literal( ini, sct ) n_ini_section_chk( ini, n_posix_literal( sct ) )

// internal
n_posix_bool
n_ini_section( n_ini *ini, const n_posix_char *section, int mode )
{

	n_posix_bool is_exist = n_posix_false;


	if ( n_ini_error( ini ) ) { return is_exist; }


	n_posix_char *s = n_string_alloccopy( n_posix_strlen( section ), section );

	n_string_squarebracket_del( s, s );
	n_string_remove_blank( s, s );


	n_type_int i = 0;
	n_posix_loop
	{//break;

		n_posix_char *l = ini->line[ i ];


		// [!] : section is found

		if ( l[ 0 ] == n_posix_literal( '[' ) )
		{

			n_posix_char *sct_name = n_string_carboncopy( l );

			n_string_squarebracket_del( sct_name, sct_name );
			n_string_remove_blank( sct_name, sct_name );

			if ( n_string_is_same( sct_name, s ) )
			{

				is_exist = n_posix_true;

				n_memory_free( sct_name );

				if ( mode != N_INI_SECTION_DEL ) { break; }

				n_type_int j = 1;
				n_posix_loop
				{//break;

					n_type_int pos = i + j;
					if ( pos >= ini->sy ) { break; }

					l = ini->line[ pos ];
					if ( l[ 0 ] == n_posix_literal( '[' ) )
					{
//n_posix_debug_literal( "%s : %d", l, j );
						j--;
						break;
					}

					j++;

				}

				n_posix_loop
				{//break;

					n_type_int pos = i + j;
					if ( pos >= ini->sy ) { break; }

					if ( j <= 0 ) { break; }

					n_ini_del( ini, pos );

					j--;

				}
//n_posix_debug_literal( "%d", i );

			} else {

				n_memory_free( sct_name );

			}

		}


		i++;
		if ( i >= ini->sy )
		{

			// [!] : insert at the first line

			if ( mode == N_INI_SECTION_ADD )
			{

				n_posix_char *sct_name = n_string_alloccopy( n_posix_strlen( s ) + 2, s );

				n_string_squarebracket_add( sct_name, sct_name );

				n_ini_set( ini, 0, sct_name );
				n_ini_set( ini, 1, N_STRING_EMPTY );

				n_memory_free( sct_name );

			}

			break;
		}

	}


	n_memory_free( s );


	return is_exist;
}

n_posix_bool
n_ini_key_match( const n_posix_char *f, const n_posix_char *t )
{

	if ( n_string_is_empty( f ) ) { return n_posix_false; }
	if ( n_string_is_empty( t ) ) { return n_posix_false; }


	const n_posix_char a = n_posix_literal( 'a' );
	//const n_posix_char z = n_posix_literal( 'z' );
	const n_posix_char A = n_posix_literal( 'A' );
	const n_posix_char Z = n_posix_literal( 'Z' );

	const n_posix_char offset = a - A;


	n_type_int i = 0;
	n_posix_loop
	{

		n_posix_char c_f = f[ i ];
		n_posix_char c_t = t[ i ];

		if ( ( c_f >= A )&&( c_f <= Z ) ) { c_f += offset; }
		if ( ( c_t >= A )&&( c_t <= Z ) ) { c_t += offset; }

		if ( c_f != c_t ) { break; }

		i++;
		if ( f[ i ] == N_STRING_CHAR_NUL ) { return n_posix_false; }
	}


	if ( t[ i ] != N_STRING_CHAR_NUL ) { return n_posix_false; }

	if (
		( f[ i ] == n_posix_literal( '=' ) )
		||
		( f[ i ] == N_STRING_CHAR_SPACE )
		||
		( f[ i ] == N_STRING_CHAR_TAB   )
		||
		( f[ i ] == N_STRING_CHAR_CR    )
		||
		( f[ i ] == N_STRING_CHAR_LF    )
		||
		( f[ i ] == N_STRING_CHAR_NUL   )
	)
	{
		return n_posix_true;
	}


	return n_posix_false;
}

#define N_INI_KEY_ADD 0
#define N_INI_KEY_DEL 1
#define N_INI_KEY_GET 2
#define N_INI_KEY_CHK 3

#define n_ini_key_add( ini, sct, key, val      ) n_ini_key( ini, sct, key,  val, NULL, NULL, N_INI_KEY_ADD ) 
#define n_ini_key_del( ini, sct, key           ) n_ini_key( ini, sct, key, NULL, NULL, NULL, N_INI_KEY_DEL ) 
#define n_ini_key_get( ini, sct, key, val, len ) n_ini_key( ini, sct, key, NULL,  val,  len, N_INI_KEY_GET ) 
#define n_ini_key_chk( ini, sct, key           ) n_ini_key( ini, sct, key, NULL, NULL, NULL, N_INI_KEY_CHK ) 

#define n_ini_key_del_literal( i,s,k     ) n_ini_key_del( i, n_posix_literal( s ), n_posix_literal( k )       )
#define n_ini_key_get_literal( i,s,k,v,l ) n_ini_key_get( i, n_posix_literal( s ), n_posix_literal( k ), v, l )
#define n_ini_key_chk_literal( i,s,k     ) n_ini_key_chk( i, n_posix_literal( s ), n_posix_literal( k )       )

// internal
n_posix_bool
n_ini_key
(
	      n_ini        *ini,
	const n_posix_char *section,
	const n_posix_char *key,
	const n_posix_char *i_value,
	      n_posix_char *o_value,
	      n_type_int   *o_value_length,
	      int           mode
)
{

	// [!] : "o_value_length" : NUL included length


	if (
		( n_ini_error( ini ) )
		||
		( n_string_is_empty( section ) )
		||
		( n_string_is_empty( key     ) )
	)
	{

		if ( mode == N_INI_KEY_GET )
		{
			if ( o_value_length != NULL ) { (*o_value_length) = 0; }
			if ( o_value        != NULL ) { n_string_truncate( o_value ); }
		}

		return n_posix_false;
	}



	n_type_int sct_found, sct_next, key_found, tail;


	{ // lock-down

	n_posix_char *s = n_string_alloccopy( n_posix_strlen( section ), section );
	n_posix_char *k = n_string_alloccopy( n_posix_strlen( key     ), key     );

	n_string_squarebracket_del( s, s );

	n_string_remove_blank( s, s );
	n_string_remove_blank( k, k );


	n_type_int i = sct_found = sct_next = key_found = tail = 0;
	n_posix_loop
	{

		n_posix_char *l = ini->line[ i ];


		if ( sct_found == 0 )
		{

			// [!] : section is found

			if ( l[ 0 ] == n_posix_literal( '[' ) )
			{

				n_posix_char *sct_name = n_string_carboncopy( l );

				n_string_squarebracket_del( sct_name, sct_name );
				n_string_remove_blank( sct_name, sct_name );

				if ( n_string_is_same( sct_name, s ) )
				{
					sct_found = tail = i + 1;
					key_found = 0;
				}

				n_memory_free( sct_name );

			}

		} else {

//n_posix_debug( l );

			if ( l[ 0 ] == n_posix_literal( '[' ) )
			{

				// [!] : the next section is found

				sct_next = i;

				break;

			} else
			if ( n_ini_key_match( l, k ) )
			{
//n_posix_debug( l );
				// [!] : don't touch "l" now

				key_found = tail = i;

				break;

			} else
			if ( n_posix_false == n_string_is_empty( l ) )
			{

				tail = i + 1;

			} else {

				//tail = i;

			}

		}


		i++;
		if ( i >= ini->sy ) { break; }
	}


	n_memory_free( s );
	n_memory_free( k );

	if ( sct_found == 0 )
	{

		if ( mode == N_INI_KEY_GET )
		{
			if ( o_value_length != NULL ) { (*o_value_length) = 0; }
			if ( o_value        != NULL ) { n_string_truncate( o_value ); }
		}

		return n_posix_false;
	}

	} // lock-down


	// [!] :  don't use variables above if possible

	if ( mode == N_INI_KEY_ADD )
	{

		if ( key_found ) { n_ini_del( ini, key_found ); }

		// [!] : fail-safe
		if ( o_value == NULL ) { o_value = N_STRING_EMPTY; }


		n_posix_char *s = n_string_new_fast
		(
			n_posix_strlen( key                      ) +
			n_posix_strlen( n_posix_literal( " = " ) ) +
			n_posix_strlen( i_value                  )
		);

		if ( s != NULL )
		{
			n_posix_strcat( s, key );
			n_posix_strcat( s, n_posix_literal( " = " ) );
			n_posix_strcat( s, i_value );

			n_ini_add( ini, tail, s );

			if ( tail == sct_next )
			{
				n_ini_add( ini, tail + 1, n_posix_literal( "" ) );
			}
		}

		n_memory_free( s );

	} else
	if ( mode == N_INI_KEY_DEL )
	{

		if ( key_found ) { n_ini_del( ini, key_found ); }

	} else
	if ( mode == N_INI_KEY_GET )
	{

		n_posix_char *l = N_STRING_EMPTY;
		if ( key_found ) { l = ini->line[ key_found ]; }

		n_posix_char *s = n_string_carboncopy( l );


		n_type_int i = 0;
		n_posix_loop
		{

			// [!] : for empty string

			if ( s[ i ] == N_STRING_CHAR_NUL )
			{

				n_string_truncate( s );

				break;
			}

			if ( s[ i ] == n_posix_literal( '=' ) )
			{

				n_string_copy( &s[ i + 1 ], s );

				n_string_remove_blank   ( s, s );
				n_string_escape_del     ( s, s );
				n_string_doublequote_del( s, s );

				break;
			}

			i++;

		}


		if ( o_value_length != NULL )
		{

			n_type_int len = n_posix_strlen( s );

			if ( (*o_value_length) > len )
			{
				if ( o_value != NULL ) { n_string_copy( s, o_value ); }
//n_posix_debug( o_value );
			}

			(*o_value_length) = len;

		}


		n_memory_free( s );

	}


	return ( key_found != 0 );
}

n_posix_bool
n_ini_key_add_int
(
	      n_ini        *ini,
	const n_posix_char *section,
	const n_posix_char *key,
	               int  val
)
{
	n_posix_char str[ 100 ];

	n_posix_sprintf_literal( str, "%d", val );

	return n_ini_key_add( ini, section, key, str );
}

#define n_ini_key_add_str( ini, sct, key, val ) n_ini_key_add( ini, sct, key,                val   )

#define n_ini_key_add_literal(     i,s,k,v ) n_ini_key_add(     i, n_posix_literal( s ), n_posix_literal( k ), v )
#define n_ini_key_add_int_literal( i,s,k,v ) n_ini_key_add_int( i, n_posix_literal( s ), n_posix_literal( k ), v )
#define n_ini_key_add_str_literal( i,s,k,v ) n_ini_key_add_str( i, n_posix_literal( s ), n_posix_literal( k ), v )

#define n_ini_value_int_literal( i,s,k,d ) n_ini_value_int( i, n_posix_literal( s ), n_posix_literal( k ), d )

int
n_ini_value_int
(
	      n_ini        *ini,
	const n_posix_char *section,
	const n_posix_char *key,
	      int           defval
)
{

	n_posix_char str[ 100 ];
	n_type_int   len = 100;
	int          ret;


	n_ini_key_get( ini, section, key, str, &len );

	if ( len == 0 )
	{
		ret = defval;
	} else {
		ret = n_posix_atoi( str );
	}


	return ret;
}

#define n_ini_value_str_cch( ini, section, key, defval ) n_ini_value_str( ini, section, key, defval, NULL, 0 );

#define n_ini_value_str_literal( i,s,k,d,v,l ) n_ini_value_str( i, n_posix_literal( s ), n_posix_literal( k ), n_posix_literal( d ), v, l )

n_type_int
n_ini_value_str
(
	      n_ini        *ini,
	const n_posix_char *section,
	const n_posix_char *key,
	const n_posix_char *defval,
	      n_posix_char *value,
	      n_type_int    value_cch
)
{

	n_type_int cch;


	n_ini_key_get( ini, section, key, NULL, &cch );


	if ( cch == 0 )
	{

		if (
			( value != NULL )
			&&
			( value_cch > n_posix_strlen( defval ) )
		)
		{
			n_string_copy( defval, value );
		}

		cch = n_posix_strlen( defval );

	} else {

		if ( value_cch > cch )
		{
			n_ini_key_get( ini, section, key, value, &value_cch );
		}

	}


	return cch;
}

n_posix_char*
n_ini_value_str_new( n_ini *ini, const n_posix_char *section, const n_posix_char *key, const n_posix_char *defval )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_ini_value_str_cch( ini, section, key, defval ); cch += 2;
	n_posix_char *ret = n_string_path_new( cch );


	n_ini_value_str( ini, section, key, defval, ret, cch );


	return ret;
}


#endif // _H_NONNON_NEUTRAL_INI

