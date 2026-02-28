// Nonnon String Vector
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_VECTOR
#define _H_NONNON_NEUTRAL_VECTOR




#include "./string.c"




typedef struct {

	void       **line;
	n_type_int   sy;


	// [!] : internal cache
	//
	//	n_vector_load()/_save() fills these members automatically
	//	use n_vector_stream() to refresh

	void       *stream;
	n_type_int  sx;
	n_type_int  byte;

} n_vector;




#define n_vector_zero(  f    ) n_memory_zero( f,    sizeof( n_vector ) )
#define n_vector_alias( f, t ) n_memory_copy( f, t, sizeof( n_vector ) )

// internal
n_posix_inline BOOL
n_vector_error( const n_vector *v )
{

	if ( v            == NULL ) { return TRUE; }
	if ( v->line      == NULL ) { return TRUE; }
	if ( v->line[ 0 ] == NULL ) { return TRUE; }
	if ( v->sy        <=    0 ) { return TRUE; }


	return FALSE;
}

void
n_vector_debug( n_vector *v )
{

	if ( n_vector_error( v ) ) { return; }


	n_type_int i = 0;
	n_posix_loop
	{

		n_posix_debug( v->line[ i ] );


		i++;
		if ( i >= v->sy ) { break; }
	}


	return;
}

void
n_vector_stream( n_vector *v, const n_posix_char *newline )
{

	if ( n_vector_error( v ) ) { return; }


	n_type_int cch_newline = 0;
	if ( newline != NULL ) { cch_newline = n_posix_strlen( newline ); }


	n_type_int i, cch;


	i = cch = v->sx = 0;
	n_posix_loop
	{

//n_posix_debug( v->line[ i ] );

		n_posix_char *line = v->line[ i ];

		n_type_int sx     = 0;
		n_type_int sx_tab = 0;
		n_posix_loop
		{

			if ( line[ sx ] == N_STRING_CHAR_NUL ) { break; }

			if ( line[ sx ] == N_STRING_CHAR_TAB )
			{

				const n_type_int tabstop = 8;
				const n_type_int tab     = tabstop - ( sx_tab % tabstop );

				sx_tab += tab;

			} else {

				sx_tab++;

			}

			sx++;

		}

		if ( newline != NULL ) { sx += cch_newline; }


		if ( v->sx < sx_tab ) { v->sx = sx_tab; }


		cch += sx;


		i++;
		if ( i >= v->sy ) { break; }
	}

//n_posix_debug_literal( str, "%d : %d", v->byte, cch * sizeof( n_posix_char ) );

	n_memory_free( v->stream );

	v->stream = n_string_new_fast( cch );
	v->byte   = cch * sizeof( n_posix_char );

	if ( v->stream == NULL )
	{
		v->byte = 0;
		return;
	}


	n_posix_char *strm = (void*) v->stream;

	n_type_int start_cch = cch;

	i = cch = 0;
	n_posix_loop
	{

		cch += n_posix_snprintf_literal( &strm[ cch ], start_cch - cch + 1, "%s", (n_posix_char*) v->line[ i ] );

		if ( newline != NULL )
		{
			cch += n_posix_snprintf_literal( &strm[ cch ], start_cch - cch + 1, "%s", newline );
		}


		i++;
		if ( i >= v->sy ) { break; }
	}


	return;
}

// internal
n_posix_char*
n_vector_stream_enumline( const n_posix_char *stream, n_type_int length, n_type_int *sx_ret, n_type_int *resume )
{

	// [!] : you need to n_memory_free() a returned variable


	if ( n_string_is_empty( stream ) ) { return NULL; }


	if ( length <= 0 ) { return NULL; }

	if ( ( resume != NULL )&&( (*resume) >= length ) ) { return NULL; }


	n_type_int     i = 0;
	n_type_int start = 0;
	n_type_int    sx = 0;
	if ( resume != NULL ) { i = start = (*resume); }
	n_posix_loop
	{

		if ( stream[ i ] == N_STRING_CHAR_CR )
		{

			i++;
			if ( stream[ i ] == N_STRING_CHAR_LF ) { i++; }

			break;

		} else
		if ( stream[ i ] == N_STRING_CHAR_LF )
		{

			i++;

			break;

		}

		sx++;


		i++;
		if ( i >= length ) { break; }
	}


	n_posix_char *s = n_string_new_fast( sx );
	n_string_copy_partial( &stream[ start ], s, sx );


	if ( sx_ret != NULL ) { (*sx_ret) = sx; }
	if ( resume != NULL ) { (*resume) =  i; }


	return s;
}

void
n_vector_free( n_vector *v )
{

	if ( v == NULL ) { return; }


	if ( v->line != NULL )
	{

		n_type_int i = 0;
		n_posix_loop
		{

//n_posix_debug( v->line[ i ] );

			n_memory_free( v->line[ i ] );

			i++;
			if ( i >= v->sy ) { break; }
		}


		n_memory_free( v->line );

	}


	n_memory_free( v->stream );


	n_vector_zero( v );


	return;
}

void
n_vector_new( n_vector *v )
{

	// [!] : guarantee one line at least


	n_vector_free( v );


	v->sy        = 1;
	v->line      = n_memory_new( v->sy * sizeof( void**       ) );
	v->line[ 0 ] = n_memory_new(     1 * sizeof( n_posix_char ) );

	n_memory_zero( v->line[ 0 ], 1 * sizeof( n_posix_char ) );


	n_vector_stream( v, NULL );


	return;
}

#define n_vector_load(          v, fname        ) n_vector_load_internal( v, (void*) fname,    0, TRUE  )
#define n_vector_load_onmemory( v, stream, byte ) n_vector_load_internal( v,        stream, byte, FALSE )

// internal
BOOL
n_vector_load_internal( n_vector *v, void *stream, n_type_int byte, BOOL is_file )
{

	// [!] : load_onmemory() : don't free "stream" after calling this function


	if ( v == NULL ) { return TRUE; }


	if ( is_file )
	{

		FILE *fp = n_posix_fopen_read( stream );
		if ( fp == NULL ) { return TRUE; }

		byte   = n_posix_stat_size( stream );
		stream = n_memory_new( byte );
		n_posix_fread( stream, byte, 1, fp );

		n_posix_fclose( fp );

	}


	// [!] : add stream NUL
	//
	//	unicode : file size may have odd number byte

	{

		n_type_int unit = sizeof( n_posix_char );
		n_type_int pad  = ( byte % unit );
		n_type_int nul  = unit;

		// [!] : stream can be NULL
		stream = n_memory_resize( stream, byte + pad + nul );

		n_posix_char *s = stream;
		s[ ( byte + pad ) / unit ] = N_STRING_CHAR_NUL;

	}


	n_vector_free( v );


	n_type_int cch = byte / sizeof( n_posix_char );


	n_type_int i = 0;
	n_posix_loop
	{//break;

		n_type_int    sx = 0;
		n_posix_char *l  = n_vector_stream_enumline( stream, cch, &sx, &i );
		if ( l == NULL ) { break; }

		v->sy++;
		v->line = n_memory_resize( v->line, v->sy * sizeof( void** ) );

		v->line[ v->sy - 1 ] = l;

//n_posix_debug( v->line[ v->sy - 1 ] );


		if ( v->sx < sx ) { v->sx = sx; }

	}


	if ( v->sy == 0 )
	{

		n_memory_free( stream );

		n_vector_new( v );

	} else {

		v->stream = stream;
		v->byte   = byte;

	}


	return FALSE;
}

#define n_vector_save_literal( v, fname, nl ) n_vector_save( v, n_posix_literal( fname ), nl )

BOOL
n_vector_save( n_vector *v, const n_posix_char *fname, const n_posix_char *newline )
{

	if ( n_vector_error( v ) ) { return TRUE; }


	FILE *fp = n_posix_fopen_write( fname );
	if ( fp == NULL ) { return TRUE; }


	n_vector_stream( v, newline );
	n_posix_fwrite( v->stream, v->byte, 1, fp );


	n_posix_fclose( fp );


	return FALSE;
}

#define n_vector_add_literal( a,b,c ) n_vector_add( a, b, n_posix_literal( c ) )

void
n_vector_add( n_vector *v, n_type_int index, const n_posix_char *s )
{

	if ( n_vector_error( v ) ) { return; }


	if ( s == NULL ) { return; }


	// [!] : the last line is accessible

	if ( index <=     0 ) { index =     0; }
	if ( index >= v->sy ) { index = v->sy; }


	// [!] : prevent integer overflow

	if ( v->sy == INT_MAX ) { return; }

	v->sy++;
	v->line = n_memory_resize( v->line, v->sy * sizeof(void**) );


	void *p = v->line[ index ];
	v->line[ index ] = n_string_carboncopy( s );

	if ( v->line[ index ] == NULL )
	{
		n_type_int cb = 2 * sizeof( n_posix_char );

		v->line[ index ] = n_memory_new( cb );
		n_memory_zero( v->line[ index ], cb );
	}


	n_type_int i = index;
	n_posix_loop
	{

		i++;
		if ( i >= v->sy ) { break; }


		void *pp     = v->line[ i ];
		v->line[ i ] = p;
		p            = pp;

	}


	return;
}

void
n_vector_add_fast_cch( n_vector *v, n_type_int index, const n_posix_char *s, n_type_int cch )
{

	// [!] : faster when "cch" is already known


	if ( n_vector_error( v ) ) { return; }


	if ( s == NULL ) { return; }


	// [!] : the last line is accessible

	if ( index <=     0 ) { index =     0; }
	if ( index >= v->sy ) { index = v->sy; }


	// [!] : prevent integer overflow

	if ( v->sy == INT_MAX ) { return; }

	v->sy++;
	v->line = n_memory_resize( v->line, v->sy * sizeof(void**) );


	void *p = v->line[ index ];
	v->line[ index ] = n_string_new( cch ); n_string_copy( s, v->line[ index ] );

	if ( v->line[ index ] == NULL )
	{
		n_type_int cb = 2 * sizeof( n_posix_char );

		v->line[ index ] = n_memory_new( cb );
		n_memory_zero( v->line[ index ], cb );
	}


	n_type_int i = index;
	n_posix_loop
	{

		i++;
		if ( i >= v->sy ) { break; }


		void *pp     = v->line[ i ];
		v->line[ i ] = p;
		p            = pp;

	}


	return;
}

#define n_vector_add_fast_ptr n_vector_add_fast

void
n_vector_add_fast( n_vector *v, n_type_int index, n_posix_char *s )
{

	// [!] : why this module is not integrated with n_vector_add()
	//
	//	"const" makes a compile error


	// [!] : faster when "s" is already allocated


	if ( n_vector_error( v ) ) { return; }


	if ( s == NULL ) { return; }


	// [!] : the last line is accessible

	if ( index <=     0 ) { index =     0; }
	if ( index >= v->sy ) { index = v->sy; }


	// [!] : prevent integer overflow

	if ( v->sy == INT_MAX ) { return; }

	v->sy++;
	v->line = n_memory_resize( v->line, v->sy * sizeof(void**) );


	void *p = v->line[ index ];
	v->line[ index ] = s;


	n_type_int i = index;
	n_posix_loop
	{

		i++;
		if ( i >= v->sy ) { break; }


		void *pp     = v->line[ i ];
		v->line[ i ] = p;
		p            = pp;

	}


	return;
}

#define n_vector_mod_literal( a,b,c ) n_vector_mod( a, b, n_posix_literal( c ) )

void
n_vector_mod( n_vector *v, n_type_int index, const n_posix_char *s )
{

	if ( n_vector_error( v ) ) { return; }


	if ( s == NULL ) { return; }


	if ( index <      0 ) { return; }
	if ( index >= v->sy ) { return; }


	if ( s == v->line[ index ] ) { return; }

	n_memory_free( v->line[ index ] );

	v->line[ index ] = n_string_carboncopy( s );


	if ( v->line[ index ] == NULL )
	{
		n_type_int cb = 2 * sizeof( n_posix_char );

		v->line[ index ] = n_memory_new( cb );
		n_memory_zero( v->line[ index ], cb );
	}


	return;
}

void
n_vector_mod_fast( n_vector *v, n_type_int index, n_posix_char *s )
{

	// [!] : why this module is not integrated with n_vector_mod()
	//
	//	"const" makes a compile error


	if ( n_vector_error( v ) ) { return; }


	if ( s == NULL ) { return; }


	if ( index <      0 ) { return; }
	if ( index >= v->sy ) { return; }


	if ( s == v->line[ index ] ) { return; }

	n_memory_free( v->line[ index ] );

	v->line[ index ] = s;


	return;
}

void
n_vector_del( n_vector *v, n_type_int index )
{

	if ( n_vector_error( v ) ) { return; }


	// [!] : the last line is not accessible

	if ( index <=     0 ) { index =         0; }
	if ( index >= v->sy ) { index = v->sy - 1; }


	// [!] : guarantee one line at least

	if ( v->sy == 1 )
	{

		n_string_truncate( v->line[ 0 ] );

		return;
	}


	n_memory_free( v->line[ index ] );


	n_type_int i = index;
	n_posix_loop
	{

		i++;
		if ( i >= v->sy ) { break; }


		v->line[ i - 1 ] = v->line[ i ];

	}


	v->sy--;
	v->line = n_memory_resize( v->line, v->sy * sizeof(void**) );


	return;
}

//BOOL n_vector_get_debug_onoff = 0;

n_posix_char*
n_vector_get( const n_vector *v, n_type_int index )
{
//if ( n_vector_get_debug_onoff ) { n_posix_debug_literal( "%lld", index ); }

	if ( n_vector_error( v ) ) { return N_STRING_EMPTY; }


	if ( index <      0 ) { return N_STRING_EMPTY; }
	if ( index >= v->sy ) { return N_STRING_EMPTY; }


	return v->line[ index ];
}

#define n_vector_set_literal( a,b,c ) n_vector_set( a, b, n_posix_literal( c ) )

void
n_vector_set( n_vector *v, n_type_int index, const n_posix_char *s )
{

	// [ Mechanism ]
	//
	//	out-of-bound : Add to tail
	//	empty        : Modify / Replace
	//	not empty    : Add    / Insert
//NSLog( @"%s", s );

	if ( n_vector_error( v ) ) { return; }


	if ( index >= v->sy )
	{

		if ( ( v->sy == 1 )&&( n_string_is_empty( n_vector_get( v, 0 ) ) ) )
		{
			n_vector_mod( v,     0, s );
		} else {
			n_vector_add( v, v->sy, s );
		}

	} else {

		if ( n_string_is_empty( n_vector_get( v, index ) ) )
		{
			n_vector_mod( v, index, s );
		} else {
			n_vector_add( v, index, s );
		}

	}


	return;
}

BOOL
n_vector_is_empty( const n_vector *v )
{

	if ( n_vector_error( v ) ) { return FALSE; }


	if (
		( v->sy == 1 )
		&&
		( n_string_is_empty( n_vector_get( v, 0 ) ) )
	)
	{
		return TRUE;
	}


	return FALSE;
}

void
n_vector_swap( n_vector *v, n_type_int a, n_type_int b )
{

	if ( n_vector_error( v ) ) { return; }


	if ( a >= v->sy ) { return; }
	if ( b >= v->sy ) { return; }


	void *p      = v->line[ a ];
	v->line[ a ] = v->line[ b ];
	v->line[ b ] = p;


	return;
}

#define n_vector_sort_down( v ) n_vector_sort( v, -1 )
#define n_vector_sort_up(   v ) n_vector_sort( v,  1 )

void
n_vector_sort( n_vector *v, int updown )
{

	// updown
	//
	//	-1 : down / descending / decremental : c b a
	//	+1 : up   /  ascending / incremental : a b c


	if ( n_vector_error( v ) ) { return; }


	// [!] : nothing to do

	if ( 1 >= v->sy ) { return; }
	if ( updown == 0 ) { return; }


	if ( updown > 0 ) { updown =  1; } else
	if ( updown < 0 ) { updown = -1; }


	n_type_int i  = 0;
	n_type_int ii = i + 1;
	n_posix_loop
	{

		if (
			( ( i  >= 0 )&&( i  < v->sy ) )
			&&
			( ( ii >= 0 )&&( ii < v->sy ) )
			&&
			( updown == n_string_compare( v->line[ i ], v->line[ ii ] ) )
		)
		{
			n_vector_swap( v, i, ii );
		}


		ii++;
		if ( ii >= v->sy )
		{

			i++;
			if ( i >= v->sy ) { break; }

			ii = i + 1;

		}
	}


	return;
}

void
n_vector_carboncopy( const n_vector *p_old, n_vector *p_new, const n_posix_char *newline )
{

	if ( n_vector_error( p_old ) ) { return; }
	if ( p_new == NULL ) { return; }


	n_vector_new( p_new );


	n_type_int i = 0;
	n_posix_loop
	{

		n_vector_set( p_new, i, n_vector_get( p_old, i ) );

		i++;
		if ( i >= p_old->sy ) { break; }
	}


	n_vector_stream( p_new, newline );


	return;
}

void
n_vector_debug_iotest( const n_posix_char *fname )
{

	n_vector v;


	n_vector_zero( &v );
	//n_vector_new( &v );
	n_vector_load( &v, fname );


	n_vector_stream( &v, N_STRING_CRLF );
//n_posix_debug_literal( "%s", v.stream );


	n_vector_save_literal( &v, "./result.txt", N_STRING_CRLF );
	n_vector_free( &v );


	return;
}


#endif // _H_NONNON_NEUTRAL_VECTOR

