// OrangeCat
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : difficulty is high
//
//	if you have some skill in C, and need scalability, use this
//	or else use N_PATH_MAX and neutral/path.c


// [!] : mixture is dangerous
//
//	don't use  all of n_path_*   @ neutral/path.c
//	don't use some of n_string_* @ neutral/string.c


// [!] : string literal
//
//	be careful to use
//	double-NUL("\0\0") is needed




#ifndef _H_NONNON_NEUTRAL_STRING_PATH
#define _H_NONNON_NEUTRAL_STRING_PATH




#include "./string.c"
#include "./time.c"




n_type_int
n_string_path_cch( const n_posix_char *str )
{

	if ( str == NULL ) { return 0; }


	n_type_int i = 0;
	n_posix_loop
	{
		if (
			( str[ i + 0 ] == N_STRING_CHAR_NUL )
			&&
			( str[ i + 1 ] == N_STRING_CHAR_NUL )
		)
		{
			i++;
			break;
		}

		i++;

	}


	return i;
}

void
n_string_path_terminate( n_posix_char *str, n_type_int index )
{

	if ( str == NULL ) { return; }


	// [!] : no index length checking available

	//if ( index >= n_string_path_cch( str ) ) { return; }


	str[ index + 0 ] = N_STRING_CHAR_NUL;
	str[ index + 1 ] = N_STRING_CHAR_NUL;


	return;
}

n_type_int
n_string_path_copy( const n_posix_char *f, n_posix_char *t )
{

	if ( f ==    t ) { return 0; }
	if ( t == NULL ) { return 0; }

	n_string_path_terminate( t, 0 );

	if ( n_string_is_empty( f ) ) { return 0; }


	n_type_int i = 0;
	n_posix_loop
	{

		if (
			( f[ i + 0 ] == N_STRING_CHAR_NUL )
			&&
			( f[ i + 1 ] == N_STRING_CHAR_NUL )
		)
		{
			break;
		}

		t[ i ] = f[ i ]; i++;

		n_string_path_terminate( t, i );

	}


	return i;
}

n_posix_bool
n_string_path_slash_check( const n_posix_char *str, n_type_int index )
{

	if ( n_string_is_empty( str ) ) { return n_posix_false; }


#ifdef N_POSIX_PLATFORM_WINDOWS

	n_type_int i = 0;
	n_posix_loop
	{
		if ( i >= index ) { break; }
		i += n_string_doublebyte_increment( str[ i ] );
	}
//n_posix_debug_literal( "%d %d", i, index );

	if ( i != index ) { return n_posix_false; }

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	if ( str[ index ] == N_POSIX_CHAR_SLASH ) { return n_posix_true; }


	return n_posix_false;
}

n_posix_bool
n_string_path_is_abspath( const n_posix_char *path )
{

	if ( n_string_is_empty( path ) ) { return n_posix_false; }


#ifdef N_POSIX_PLATFORM_WINDOWS

	// [!] : don't use n_posix_strlen() for performance

	if (
		(
			( path[ 0 ] != N_STRING_CHAR_NUL )
			&&
			( n_string_is_alphabet( path, 0 ) )
		)
		&&
		(
			( path[ 1 ] != N_STRING_CHAR_NUL )
			&&
			( path[ 1 ] == N_STRING_CHAR_COLON )
		)
	)
	{
		return n_posix_true;
	}

#else

	if ( n_string_path_slash_check( path, 0 ) ) { return n_posix_true; }

#endif


	return n_posix_false;
}

#define n_string_path_is_drivename n_posix_is_drivename

#define n_string_path_drivename_slash_add( path ) n_string_path_drivename_slash_set( path, n_posix_true  )
#define n_string_path_drivename_slash_del( path ) n_string_path_drivename_slash_set( path, n_posix_false )

// internal
void
n_string_path_drivename_slash_set( n_posix_char *path, n_posix_bool is_add )
{

	if ( n_string_is_empty( path ) ) { return; }


	if ( n_posix_false == n_string_path_is_drivename( path ) ) { return; }


	n_type_int cch = n_posix_strlen( path );

	if ( is_add )
	{

		if ( n_posix_false == n_string_path_slash_check( path, cch - 1 ) )
		{
			n_posix_strcat( path, N_POSIX_SLASH );
			n_string_path_terminate( path, cch + 1 );
		}

	} else {

		if ( n_posix_false != n_string_path_slash_check( path, cch - 1 ) )
		{
			n_string_path_terminate( path, cch - 1 );
		}

	}


	return;
}

#define n_string_path_free( str ) n_string_free( str )

#define n_string_path_new(      cch ) n_string_path_new_internal( cch, n_posix_false )
#define n_string_path_new_fast( cch ) n_string_path_new_internal( cch, n_posix_true  )

n_posix_char*
n_string_path_new_internal( n_type_int cch, n_posix_bool is_fast )
{
//return NULL;

	// [!] : you need to n_string_path_free() a returned variable


	// [!] : +2 for double NUL('\0')

	n_type_int    b = ( cch + 2 ) * sizeof( n_posix_char );
	n_posix_char *s = (n_posix_char*) n_memory_new( b );


	if ( is_fast )
	{
		n_string_path_terminate( s, 0 );
	} else {
		n_memory_zero( s, b );
	}


	return s;
}

#define n_string_path_carboncopy(          path ) n_string_path_carboncopy_internal( path, n_posix_false )
#define n_string_path_carboncopy_multiple( path ) n_string_path_carboncopy_internal( path, n_posix_true  )

n_posix_char*
n_string_path_carboncopy_internal( const n_posix_char *str, n_posix_bool is_multi )
{
//return NULL;

	// [!] : you need to n_string_path_free() a returned variable


	n_type_int cch;

	if ( str == NULL )
	{

		cch = 0;

	} else {

		if ( is_multi )
		{
			cch = n_string_path_cch( str );
		} else {
			cch = n_posix_strlen   ( str );
		}

	}


	// [!] : +2 for double NUL('\0')

	n_type_int    b = ( cch + 2 ) * sizeof( n_posix_char );
	n_posix_char *s = (n_posix_char*) n_memory_new( b );


	if ( str == NULL )
	{

		n_memory_zero( s, b );

	} else {

		if ( is_multi )
		{
			n_string_path_copy( str, s );
		} else {
			n_string_copy     ( str, s );
			n_string_path_terminate( s, cch );
		}

	}


	return s;
}

n_posix_char*
n_string_path_cat( n_posix_char *first, ... )
{

	// [Mechanims]
	//
	//	the last parameter needs to be NULL
	//	this might be a GCC's bug


	n_type_int cch = 0;


	n_posix_char *str;


	{

		str = NULL;

		va_list vl; va_start( vl, first );

		n_posix_loop
		{

			if ( str == NULL )
			{
				str = first;
			} else {
				str = va_arg( vl, n_posix_char* );
				if ( str == NULL ) { break; }
			}
//n_posix_debug_literal( "%s", str );
			cch += n_posix_strlen( str );

		}

		va_end( vl );

	}
//n_posix_debug_literal( "%d", cch );

	// [x] : prevent crash : zero-clear is needed

	n_posix_char *ret = n_string_path_new( cch );

	{

		str = NULL;

		va_list vl; va_start( vl, first );

		n_type_int i = 0;
		n_posix_loop
		{

			if ( str == NULL )
			{
				str = first;
			} else {
				str = va_arg( vl, n_posix_char* );
				if ( str == NULL ) { break; }
			}

			i += n_posix_sprintf_literal( &ret[ i ], "%s", str );

		}

		va_end( vl );

	}


	return ret;
}

n_posix_char*
n_string_path_slash_new( const n_posix_char *path )
{

	// [!] : you need to n_string_path_free() a returned variable

	// [!] : 1 == n_posix_strlen( N_POSIX_SLASH )

	n_type_int cch = 1;
	if ( n_posix_false == n_string_is_empty( path ) ) { cch += n_posix_strlen( path ); }

	n_posix_char *ret = n_string_path_new_fast( cch );


	n_string_copy( path, ret );
	n_string_path_drivename_slash_add( ret );


	return ret;
}

n_type_int
n_string_path_multiple_cch( const n_posix_char *path, n_type_int index )
{

	// [ Mechanism ]
	//
	//	"C:\\path1\0C:\\path2\0\0"


	if ( n_string_is_empty( path ) ) { return 0; }


	n_type_int count = 0;


	n_type_int i = 0;
	n_type_int j = 0;
	n_posix_loop
	{
		if ( path[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( count == index )
		{
			j++;
		}

		i++;

		if ( path[ i ] == N_STRING_CHAR_NUL ) { count++; i++; }

	}


	return j;
}

#define n_string_path_multiple_count( str ) n_string_path_multiple_internal( str, 0, NULL, n_posix_true )

#define n_string_path_multiple( str, i, ret ) n_string_path_multiple_internal( str, i, ret, n_posix_false )

n_type_int
n_string_path_multiple_internal( const n_posix_char *path, n_type_int index, n_posix_char *ret, n_posix_bool is_count )
{

	// [ Mechanism ]
	//
	//	"C:\\path1\0C:\\path2\0\0"


	if ( n_string_is_empty( path ) ) { return 0; }


	n_type_int count = 0;


	n_type_int i = 0;
	n_type_int j = 0;
	n_posix_loop
	{
		if ( path[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( ( is_count == n_posix_false )&&( count == index ) )
		{
			if ( ret != NULL )
			{
				ret[ j ] = path[ i ]; j++;

				n_string_path_terminate( ret, j );
			}
		}

		i++;

		if ( path[ i ] == N_STRING_CHAR_NUL ) { count++; i++; }

	}


	return count;
}

n_posix_char*
n_string_path_multiple_new( const n_posix_char *path, n_type_int index )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_multiple_cch( path, index );
	n_posix_char *ret = n_string_path_new_fast( cch );


	n_string_path_multiple( path, index, ret );


	return ret;
}

n_type_int
n_string_path_make_cch( const n_posix_char *dir, const n_posix_char *file )
{
	// [!] : 1 == n_posix_strlen( N_POSIX_SLASH )

	return ( n_posix_strlen( dir ) + 1 + n_posix_strlen( file ) );
}

void
n_string_path_make( const n_posix_char *dir, const n_posix_char *file, n_posix_char *ret )
{

	if ( ret == NULL ) { return; }


	if ( n_string_is_empty( dir  ) ) { return; }
	if ( n_string_is_empty( file ) ) { return; }


	n_posix_char *tmp_dir  = n_string_path_carboncopy( dir  );
	n_posix_char *tmp_file = n_string_path_carboncopy( file );


	// [!] : "C:\" => "C:" if needed

	n_string_path_drivename_slash_del( tmp_dir );


	// [!] : "\file" => "file"

	n_type_int skip = 0;
	if ( n_string_path_slash_check( tmp_file, 0 ) )
	{
		skip = 1;
	}


	n_type_int cch = n_posix_sprintf_literal( ret, "%s%s%s", tmp_dir, N_POSIX_SLASH, &tmp_file[ skip ] );
	n_string_path_terminate( ret, cch );


	n_string_path_free( tmp_dir  );
	n_string_path_free( tmp_file );


	return;
}

n_posix_char*
n_string_path_make_new( const n_posix_char *dir, const n_posix_char *file )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_make_cch( dir, file );
	n_posix_char *ret = n_string_path_new_fast( cch );


	n_string_path_make( dir, file, ret );


	return ret;
}

n_type_int
n_string_path_split_cch( const n_posix_char *path, n_type_int index )
{

	// [!] : not a path name

	if ( n_posix_false == n_string_path_is_abspath( path ) ) { return 0; }


	// [!] : "C:\" => "C:" if needed

	n_posix_char *tmp_path = n_string_path_carboncopy( path );

	n_string_path_drivename_slash_del( tmp_path );


	n_type_int  i = 0;
	n_type_int ii = 0;
	n_type_int  n = 0;
	n_posix_loop
	{

		if ( index == n )
		{
			ii++;
		}


		i++;
		if ( tmp_path[ i ] == N_STRING_CHAR_NUL ) { break; }


		if ( n_string_path_slash_check( tmp_path, i ) )
		{

			n++;

			i++;
			if ( tmp_path[ i ] == N_STRING_CHAR_NUL ) { break; }
		}
	}


	n_string_path_free( tmp_path );


	return ii;
}

#define n_string_path_split_count( path ) n_string_path_split( path, NULL, 0 )

n_type_int
n_string_path_split( const n_posix_char *path, n_posix_char *ret, n_type_int index )
{

	// [!] : this module is based on n_path_divider()

	// [!] : Return Value
	//
	//	ret == 0 : no index
	//	ret >= 1 : index count


	// [!] : not a path name

	if ( n_posix_false == n_string_path_is_abspath( path ) ) { return 0; }


	// [!] : "C:\" => "C:" if needed

	n_posix_char *tmp_path = n_string_path_carboncopy( path );
	if ( tmp_path == NULL ) { return 0; }


	n_string_path_drivename_slash_del( tmp_path );


	n_type_int  i = 0;
	n_type_int ii = 0;
	n_type_int  n = 0;
	n_posix_loop
	{

		if ( index == n )
		{

			if ( ret != NULL )
			{
				ret[ ii ] = tmp_path[ i ];
			}

			ii++;

			if ( ret != NULL )
			{
				n_string_path_terminate( ret, ii );
			}

		}


		i++;
		if ( tmp_path[ i ] == N_STRING_CHAR_NUL ) { break; }


		if ( n_string_path_slash_check( tmp_path, i ) )
		{

			n++;

			i++;
			if ( tmp_path[ i ] == N_STRING_CHAR_NUL ) { break; }
		}
	}


	n_string_path_free( tmp_path );


	return ( 1 + n );
}

n_posix_char*
n_string_path_split_new( const n_posix_char *path, n_type_int index )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_split_cch( path, index );
	n_posix_char *pth = n_string_path_new_fast( cch );


	n_string_path_split( path, pth, index );


	return pth;
}

n_type_int
n_string_path_name_cch( const n_posix_char *path )
{

	n_type_int cch = 0;


	n_type_int count = n_string_path_split( path, NULL, 0 );
	if ( count == 0 )
	{
		cch = n_posix_strlen( path );
	} else {
		cch = n_string_path_split_cch( path, count - 1 );
	}


	return cch;
}

// internal
void
n_string_path_name( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	// [!] : don't check whether file or dir

	n_posix_char *path = n_string_path_carboncopy( arg_path );


	n_type_int count = n_string_path_split( path, NULL, 0 );
	if ( count == 0 )
	{
		n_type_int cch = n_string_path_copy( path, arg_ret );
		n_string_path_terminate( arg_ret, cch );
	} else {
		n_string_path_split( path, arg_ret, count - 1 );
	}


	n_string_path_free( path );


	return;
}

n_posix_char*
n_string_path_name_new( const n_posix_char *path )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_name_cch( path );
	n_posix_char *nam = n_string_path_new_fast( cch );


	n_string_path_name( path, nam );


	return nam;
}

#define n_string_path_upperfolder_cch( path ) n_string_path_upperfolder( path, NULL )

n_type_int
n_string_path_upperfolder( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	// [!] : don't check whether a file or a folder


	if ( n_string_is_empty( arg_path ) ) { return 0; }


	n_posix_char *path = n_string_path_carboncopy( arg_path ); n_string_path_drivename_slash_del( path );
	n_posix_char *name = n_string_path_name_new( path );

//n_posix_debug_literal( "Path:%s\nName:%s", path, name );

	if ( n_posix_false == n_string_is_same( path, name ) )
	{
		n_type_int index = n_posix_strlen( path ) - n_posix_strlen( name );
		if ( index >= 1 ) { index--; }
		n_string_path_terminate( path, index );
//n_posix_debug_literal( " %s ", path );
	}


	if ( arg_ret != NULL )
	{
		n_type_int cch = n_string_path_copy( path, arg_ret );
		n_string_path_terminate( arg_ret, cch );
	}


	n_type_int cch_ret = 0;
	if ( arg_ret == NULL ) { cch_ret = n_posix_strlen( path ); }


	n_string_path_free( path );
	n_string_path_free( name );


	return cch_ret;
}

n_posix_char*
n_string_path_upperfolder_new( const n_posix_char *path )
{

	// [!] : you need to n_string_path_free() a returned variable

	// [!] : 1 == n_posix_strlen( N_POSIX_SLASH )

	n_type_int    cch = n_string_path_upperfolder_cch( path ) + 1;
	n_posix_char *dir = n_string_path_new_fast( cch );


	n_string_path_upperfolder( path, dir );


	return dir;
}

// internal
n_posix_bool
n_string_path_short2long_is_shortname( const n_posix_char *path, n_type_int *cch )
{

	if ( n_string_is_empty( path ) ) { return n_posix_false; }


	n_posix_bool ret = n_posix_false;

	n_type_int i = 0;
	n_posix_loop
	{

		if ( path[ i ] == N_STRING_CHAR_NUL ) { break; }

		if ( path[ i ] == N_STRING_CHAR_TILDE ) { ret = n_posix_true; }

		i++;

	}


	if ( cch != NULL ) { (*cch) = i; }


	return ret;
}

#define n_string_path_short2long_cch( path, ret_is_shortname ) n_string_path_short2long( path, NULL, ret_is_shortname )

n_type_int
n_string_path_short2long( const n_posix_char *arg_path, n_posix_char *arg_ret, n_posix_bool *ret_is_shortname )
{

	if ( n_string_is_empty( arg_path ) ) { return 0; }


	n_type_int cch = 0;


#ifdef N_POSIX_PLATFORM_WINDOWS


	if ( ret_is_shortname != NULL ) { (*ret_is_shortname) = n_posix_false; }


	// [!] : not a short name

	if ( n_posix_false == n_string_path_short2long_is_shortname( arg_path, &cch ) )
	{

		if ( arg_ret != NULL )
		{
			n_string_path_copy( arg_path, arg_ret );
			cch = 0;
		}

		return cch;
	}


	if ( n_posix_false == n_string_path_is_abspath( arg_path ) )
	{

		// [!] : not a path : send back

		if ( arg_ret != NULL )
		{
			n_string_path_copy( arg_path, arg_ret );
			cch = 0;
		}

		return cch;

	} else
	if ( n_string_path_is_drivename( arg_path ) )
	{

		// [!] : drive name

		if ( arg_ret != NULL )
		{
			n_string_path_copy( arg_path, arg_ret );
			cch = 0;
		}

		return cch;

	}


	// [!] : add a drive name

	if ( ret_is_shortname != NULL ) { (*ret_is_shortname) = n_posix_true; }

	n_posix_char *longname = n_string_path_split_new( arg_path, 0 );
	n_type_int    count    = n_string_path_split( arg_path, NULL, 0 );

	n_type_int i = 1;
	n_posix_loop
	{

		n_posix_char *  tmpname = n_string_path_split_new( arg_path, i );
		n_posix_char *shortname = n_string_path_make_new( longname, tmpname );

		n_string_path_free( tmpname );


		// [!] : "nonnon~1" => "nonnon_win"

		WIN32_FIND_DATA f; ZeroMemory( &f, sizeof( WIN32_FIND_DATA ) );

		FindClose( FindFirstFile( shortname, &f ) );
		n_string_path_free( shortname );

		n_posix_char *retname = n_string_path_make_new( longname, f.cFileName );

		n_string_path_free( longname );
		longname = retname;

//n_posix_debug( arg_ret );


		i++;
		if ( i >= count ) { break; }
	}


	n_string_path_copy( longname, arg_ret );

	if ( arg_ret == NULL ) { cch = n_posix_strlen( longname ); }


	n_string_path_free( longname );


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_string_path_copy( arg_path, arg_ret );

	if ( arg_ret == NULL ) { cch = n_string_path_cch( arg_path ); }


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return cch;
}

n_posix_char*
n_string_path_short2long_new( const n_posix_char *path )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_short2long_cch( path, NULL );
	n_posix_char *ret = n_string_path_new_fast( cch );


	n_string_path_short2long( path, ret, NULL );


	return ret;
}

#define n_string_path_folder_change_fast( dir ) n_posix_chdir( dir );

n_posix_bool
n_string_path_folder_change( const n_posix_char *path )
{

	if ( n_string_is_empty( path ) ) { return n_posix_true; }


	n_posix_char *dir;
	if ( n_posix_stat_is_file( path ) )
	{
		dir = n_string_path_upperfolder_new( path );
		n_string_path_drivename_slash_add( dir );
	} else {
		dir = n_string_path_slash_new( path );
	}

	int ret = n_posix_chdir( dir );

	n_string_path_free( dir );


	return ( ret != 0 );
}

n_type_int
n_string_path_folder_current_cch( n_posix_bool *is_shortname )
{

	// [x] : at least PATH_MAX is needed

	n_type_int cch = 1024;// PATH_MAX;
	n_type_int ret = cch;
	n_posix_loop
	{

		n_posix_char *dir = n_string_path_new( cch );

		void *ptr = n_posix_getcwd( dir, (int) cch );
		      ret = cch + n_string_path_short2long_cch( dir, is_shortname ); 

		n_string_path_free( dir );

		if ( ptr != NULL )
		{
			break;
		} else {
			cch++;
		}

	}


	return ret;
}

void
n_string_path_folder_current( n_posix_char *ret, n_type_int cch, n_posix_bool is_shortname )
{

	if ( ret == NULL ) { return; }


	n_posix_getcwd( ret, (int) cch );

	if ( is_shortname )
	{
		n_string_path_short2long( ret, ret, NULL );
	}


	return;
}

n_posix_char*
n_string_path_folder_current_new( void )
{

	n_posix_bool  is_shortname = n_posix_false;
	n_type_int    cch = n_string_path_folder_current_cch( &is_shortname );
	n_posix_char *ret = n_string_path_new_fast( cch ); n_string_path_folder_current( ret, cch, is_shortname );

	return ret;
}

#define n_string_path_ext_get_cch( path ) n_string_path_ext_get( path, NULL )

n_type_int
n_string_path_ext_get( const n_posix_char *path, n_posix_char *ret )
{

	if ( n_string_is_empty( path ) ) { return 0; }


	// [!] : not supported

	if ( path == ret ) { return 0; }


	// [!] : "." is DBCS safe


	n_posix_bool found = n_posix_false;

	n_type_int pos = 0;


	n_type_int i = n_posix_strlen( path );
	n_posix_loop
	{ 

		if ( path[ i ] == N_STRING_CHAR_DOT )
		{

			pos = i;

			found = n_posix_true;

			break;

		} else
		if ( n_string_path_slash_check( path, i ) )
		{
			break;
		}


		if ( i == 0 ) { break; }
		i--;

	}


	n_type_int cch = 0;

	if ( found )
	{
		if ( ret == NULL )
		{
			cch = n_posix_strlen( &path[ pos ] );
		} else {
			// [x] : don't use n_string_path_copy() or n_string_copy()
			cch = n_posix_sprintf_literal( ret, "%s", &path[ pos ] );
			n_string_path_terminate( ret, cch );
		}
	}


	return cch;
}

n_posix_char*
n_string_path_ext_get_new( const n_posix_char *path )
{

	n_type_int    cch = n_string_path_ext_get_cch( path );
	n_posix_char *ret = n_string_path_new_fast( cch ); n_string_path_ext_get( path, ret );

	return ret;
}

#define n_string_path_ext_is_same_literal( f, t ) n_string_path_ext_is_same_internal( n_posix_literal( f ), t, n_posix_true )
#define n_string_path_ext_is_same(         f, t ) n_string_path_ext_is_same_internal( f, t, n_posix_false )

// internal
n_posix_bool
n_string_path_ext_is_same_internal( const n_posix_char *path1, const n_posix_char *path2, n_posix_bool is_literal )
{

	if ( n_string_is_empty( path1 ) ) { return n_posix_false; }
	if ( n_string_is_empty( path2 ) ) { return n_posix_false; }


	n_posix_bool ret;

	if ( is_literal )
	{

		n_posix_char *ext2 = n_string_path_ext_get_new( path2 );

		ret = n_string_is_same( path1, ext2 );

		n_string_path_free( ext2 );

	} else {

		n_posix_char *ext1 = n_string_path_ext_get_new( path1 );
		n_posix_char *ext2 = n_string_path_ext_get_new( path2 );
//n_posix_debug_literal( "1:%s\n2:%s", ext1, ext2 );

		ret = n_string_is_same( ext1, ext2 );

		n_string_path_free( ext1 );
		n_string_path_free( ext2 );

	}


	return ret;
}

#define N_STRING_PATH_EXT_SET_ADD 0
#define N_STRING_PATH_EXT_SET_DEL 1

#define n_string_path_ext_add( e, p ) n_string_path_ext_set_internal(    e, p, N_STRING_PATH_EXT_SET_ADD, n_posix_false )
#define n_string_path_ext_mod( e, p ) n_string_path_ext_set_internal(    e, p, N_STRING_PATH_EXT_SET_DEL, n_posix_false )
#define n_string_path_ext_del(    p ) n_string_path_ext_set_internal( NULL, p, N_STRING_PATH_EXT_SET_DEL, n_posix_false )

#define n_string_path_ext_add_literal( a,b ) n_string_path_ext_set_internal( n_posix_literal( a ), b, N_STRING_PATH_EXT_SET_ADD, n_posix_true )
#define n_string_path_ext_mod_literal( a,b ) n_string_path_ext_set_internal( n_posix_literal( a ), b, N_STRING_PATH_EXT_SET_DEL, n_posix_true )

// internal
void
n_string_path_ext_set_internal( const n_posix_char *ext, n_posix_char *path, int mode, n_posix_bool is_literal )
{

	// [Mechanism] : "ext"
	//
	//	empty : remove extension
	//	other : add/replace extension


	// [!] : avoid making dot-file

	if ( n_string_is_empty( path ) ) { return; }


	// [!] : not supported

	if ( ( ext != NULL )&&( ext[ 0 ] != N_STRING_CHAR_DOT ) ) { return; }


	n_type_int cch = n_posix_strlen( path );

	if ( mode == N_STRING_PATH_EXT_SET_DEL )
	{

		n_posix_char *str_ext = n_string_path_ext_get_new( path );

		cch -= n_posix_strlen( str_ext );

		n_string_path_free( str_ext );

	}


	if ( n_string_is_empty( ext ) )
	{

		// [!] : n_string_path_ext_del()

		n_string_path_terminate( path, cch );

	} else {

		if ( is_literal )
		{
			n_string_copy     ( ext, &path[ cch ] );
			n_string_path_terminate( path, n_posix_strlen( path ) );
//n_posix_debug_literal( " %s ", path );
		} else {
			n_string_path_copy( ext, &path[ cch ] );
		}

	}


	return;
}

n_posix_char*
n_string_path_ext_mod_new( const n_posix_char *path, const n_posix_char *ext )
{

	// [!] : you need to n_string_path_free() a returned variable

	// [!] : for a new name with a replacing extension
	//
	//	"C:\\name.bmp" to "C:\\name.png"

	n_type_int    cch = n_posix_strlen( path ) + n_posix_strlen( ext );
	n_posix_char *str = n_string_path_new_fast( cch );


	n_string_copy( path, str );
	n_string_path_ext_mod( ext, str );


	return str;
}

#define n_string_path_tmpname_cch() ( ( 4 + 2 + 2 ) + 1 + ( 2 + 2 + 2 ) )

void
n_string_path_tmpname( n_posix_char *str )
{

	// [Mechanism]
	//
	//	this like format is used in some locales
	//	big-to-small order is simply sortable


	int year, month, day, hour, minute, second;


	n_time_today( &year, &month, &day, &hour, &minute, &second );


	n_posix_sprintf_literal
	(
		str,
		"%04d%02d%02d_%02d%02d%02d",
		year, month, day,
		hour, minute, second
	);


	return;
}

#define n_string_path_tmpname_new_literal( ext ) n_string_path_tmpname_new_internal( n_posix_literal( ext ), n_posix_true  )
#define n_string_path_tmpname_new(         ext ) n_string_path_tmpname_new_internal(                  ext  , n_posix_false )

n_posix_char*
n_string_path_tmpname_new_internal( const n_posix_char *ext, n_posix_bool is_literal )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_tmpname_cch() + n_posix_strlen( ext );
	n_posix_char *str = n_string_path_new_fast( cch );


	n_string_path_tmpname( str );
	n_string_path_ext_set_internal( ext, str, N_STRING_PATH_EXT_SET_ADD, is_literal );


	return str;
}

n_posix_char*
n_string_path_cmdline2multipath( const n_posix_char *cmdline )
{

	// [!] : "C:\windows C:\nonnon" will be "C:\windows\0C:\nonnon\0\0"


	if ( n_string_is_empty( cmdline ) ) { return n_string_path_carboncopy( N_STRING_EMPTY ); }


	n_posix_char *quote = (n_posix_char*) N_STRING_EMPTY;
	n_posix_char *delim = (n_posix_char*) N_STRING_EMPTY;
	if ( n_string_search_simple( cmdline, N_STRING_DQUOTE ) )
	{
		quote = (n_posix_char*) N_STRING_DQUOTE;
		delim = (n_posix_char*) N_STRING_SPACE;
	} else
	if ( n_string_search_simple( cmdline, N_STRING_SPACE ) )
	{

		n_type_int colon = 0;

		n_type_int i = 0;
		n_posix_loop
		{
			if ( cmdline[ i ] == N_STRING_CHAR_NUL ) { break; }

			if ( cmdline[ i ] == N_STRING_CHAR_COLON )
			{
				colon++;
				if ( colon >= 2 ) { delim = (n_posix_char*) N_STRING_SPACE; break; }
			}

			i++;
		}

	}


	n_type_int    cch = n_posix_strlen( cmdline );
	n_posix_char *str = n_string_path_carboncopy( cmdline );
	n_posix_char *cmd = n_string_path_carboncopy( cmdline );

	n_type_int count = n_string_parameter_count( str, delim, quote );
//n_posix_debug_literal( " %d ", count );
	if ( count == 0 )
	{
		n_string_path_free( str );
		n_string_path_free( cmd );

		return n_string_path_carboncopy( N_STRING_EMPTY );
	}

	n_type_int i = 0;
	n_type_int j = 0;
	n_posix_loop
	{

		if ( i >= count ) { break; }

		n_posix_char *ret = n_string_new( cch );
		if ( ret != NULL )
		{
			n_string_parameter( str, delim, quote, i, ret );
//n_posix_debug_literal( "Ret : %s" , ret );

			j += n_posix_sprintf_literal( &cmd[ j ], "%s", ret );
			cmd[ j ] = N_STRING_CHAR_NUL; j++;
		}
		n_string_path_free( ret );

		i++;
	}

	cmd[ j ] = N_STRING_CHAR_NUL;

	n_string_path_free( str );


	return cmd;
}

#define n_string_path_commandline_option_literal( o, p, m ) n_string_path_commandline_option( n_posix_literal( o ), p, m )

n_posix_bool
n_string_path_commandline_option( const n_posix_char *option, n_posix_char *path, n_posix_bool match_only )
{

	// [Mechanism]
	//
	//	ret = n_posix_false : no touch
	//	ret = n_posix_true  : "option" will be removed


	n_posix_bool ret = n_posix_false;


//n_posix_debug_literal( "%s : %s", option, path );
//n_string_replace( path,path, N_STRING_SPACE, n_posix_literal( "_" ) );

	if (
		(
			( match_only != n_posix_false )
			&&
			( n_string_match( path, option ) )
		)
		||
		(
			( match_only == n_posix_false )
			&&
			( n_string_match( path, option ) )
			&&
			(
				( path[ n_posix_strlen( option ) ] == N_STRING_CHAR_NUL   )
				||
				( path[ n_posix_strlen( option ) ] == N_STRING_CHAR_SPACE )
			)
		)
	)
	{

		ret = n_posix_true;

		n_posix_char *tmp = n_string_path_carboncopy( path );

		n_type_int space = 0;

		n_type_int i = 0;
		n_posix_loop
		{
			if ( tmp[ n_posix_strlen( option ) + i ] == N_STRING_CHAR_NUL   )
			{
				break;
			} else
			if ( tmp[ n_posix_strlen( option ) + i ] != N_STRING_CHAR_SPACE )
			{
				break;
			} else {
				space++;
			}

			i++;
		}

		n_string_path_copy( &tmp[ n_posix_strlen( option ) + space ], path );

		n_string_path_free( tmp );

	}


	return ret;
}

n_posix_bool
n_string_path_rename( const n_posix_char *f, const n_posix_char *t )
{

	int ret = n_posix_rename( f, t );


	return ( ret == -1 );
}

#define n_string_path_relative_cch( str ) n_string_path_relative( str, NULL )

n_type_int
n_string_path_relative( const n_posix_char *str, n_posix_char *ret )
{

	if ( n_string_is_empty( str ) ) { return 0; }


	// [!] : nothing to do : already has "./" or ".\\"

	if (
		( str[ 0 ] == N_STRING_CHAR_DOT )
		&&
		( n_string_path_slash_check( str, 1 ) )
	)
	{
		return 0;
	}


	if ( n_string_path_is_drivename( str ) )
	{
		return 0;
	}


	n_type_int cch = 0;

	if ( ret == NULL )
	{
		// [!] : 1 == n_posix_strlen( N_STRING_DOT )
		// [!] : 1 == n_posix_strlen( N_POSIX_SLASH )

		cch = 1 + 1 + n_posix_strlen( str );
	} else {
		cch = n_posix_sprintf_literal( ret, "%s%s%s", N_STRING_DOT, N_POSIX_SLASH, str );
	}


	return cch;
}

n_posix_char*
n_string_path_relative_new( const n_posix_char *name )
{

	// [!] : you need to n_string_path_free() a returned variable

	n_type_int    cch = n_string_path_relative_cch( name );
	n_posix_char *str = n_string_path_new_fast( cch );


	n_string_path_relative( name, str );


	return str;
}


#endif // _H_NONNON_NEUTRAL_STRING_PATH

