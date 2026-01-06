// OrangeCat
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [ Limitation ]
//
//	[ Parameters ]
//
//	In  : no limitation / N_PATH_MAX or larger
//	Out : N_PATH_MAX or larger
//
//	+ limits.h has PATH_MAX symbol
//	+ in C, modules cannot know the allocated memory size of pointer parameters
//	+ so, memory-zero-clear is very important


// [ Implementation ]
//
//	[ Input : Non-Breaking ]
//
//	each parameters can be the same pointer variables
//
//	[ Input : Handle As Unknown Length ]
//
//	an input parameter can be allocated under/over PATH_MAX-1


// [ Mechanism ]
//
//	/folder/folder/file.txt
//      |--0--||--1--||---2---|
//
//	[ C:\WINDOWS\SYSTEM ]
//
//	#0 : C:
//	#1 : \WINDOWS
//	#2 : \SYSTEM




#ifndef _H_NONNON_NEUTRAL_PATH
#define _H_NONNON_NEUTRAL_PATH




#include "./string.c"
#include "./time.c"




#define N_PATH_MAX ( 1024 )




#define n_path_zero( str ) n_string_zero( str, N_PATH_MAX );

n_posix_bool
n_path_slash_check( const n_posix_char *str, s64 index )
{

	if ( n_string_is_empty( str ) ) { return n_posix_false; }

	if ( index < 0 ) { return n_posix_false; }


#ifdef N_POSIX_PLATFORM_WINDOWS

	s64 i = 0;
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
n_path_is_abspath( const n_posix_char *path )
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

	if ( n_path_slash_check( path, 0 ) ) { return n_posix_true; }

#endif


	return n_posix_false;
}

#define n_path_is_drivename n_posix_is_drivename

#define N_PATH_DRIVENAME_SLASH_ADD 0
#define N_PATH_DRIVENAME_SLASH_DEL 1

#define n_path_drivename_slash_add( path ) n_path_drivename_slash_set( path, N_PATH_DRIVENAME_SLASH_ADD )
#define n_path_drivename_slash_del( path ) n_path_drivename_slash_set( path, N_PATH_DRIVENAME_SLASH_DEL )

void
n_path_drivename_slash_set( n_posix_char *path, int mode )
{

	if ( n_string_is_empty( path ) ) { return; }


	if ( n_posix_false == n_path_is_drivename( path ) ) { return; }


	if ( mode == N_PATH_DRIVENAME_SLASH_ADD )
	{
		if ( n_posix_false == n_path_slash_check( path, n_posix_strlen( path ) - 1 ) )
		{
			n_posix_strcat( path, N_POSIX_SLASH );
		}
	} else
	if ( mode == N_PATH_DRIVENAME_SLASH_DEL )
	{
		if ( n_path_slash_check( path, n_posix_strlen( path ) - 1 ) )
		{
			n_string_terminate( path, n_posix_strlen( path ) - 1 );
		}
	}


	return;
}

// internal
int
n_path_nonbreak
(
	const n_posix_char *arg_path,
              n_posix_char *arg_ret,
              n_posix_char *local_path
)
{

	// [!] : Return Value
	//
	//	ret 1 : arg_path is empty
	//	ret 2 : arg_ret  is NULL
	//	ret 3 : path is too long


	// [!] : three parameters can be the same pointer


	if ( n_string_is_empty( arg_path ) )
	{

		if (   arg_ret  != NULL ) { n_string_truncate(   arg_ret  ); }
		if ( local_path != NULL ) { n_string_truncate( local_path ); }

		return 1;
	}


	s64 len = n_posix_strlen( arg_path );
	if ( ( len <= 0 )||( len >= N_PATH_MAX ) )
	{

		if (   arg_ret  != NULL ) { n_string_truncate(   arg_ret  ); }
		if ( local_path != NULL ) { n_string_truncate( local_path ); }

		return 3;
	}


	if ( local_path != NULL )
	{

		n_string_copy( arg_path, local_path );


		// [!] : "C:\" => "C:" if needed

		n_path_drivename_slash_del( local_path );

	}


	if ( arg_ret != NULL )
	{
		n_string_truncate( arg_ret );
	} else {
		return 2;
	}


	return 0;
}

void
n_path_maker_slow
(
	const n_posix_char *arg_dir,
	const n_posix_char *arg_file,
	      n_posix_char *arg_ret
)
{

	// [!] : don't use "arg_ret" with n_path_nonbreak()

	n_posix_char   dir[ N_PATH_MAX ];
	n_posix_char  file[ N_PATH_MAX ];
	n_posix_char dummy[ N_PATH_MAX ];

	{

		int ret;

		ret  = n_path_nonbreak( arg_dir,  dummy, dir  );
		ret += n_path_nonbreak( arg_file, dummy, file );

		if ( arg_ret != NULL ) { n_string_truncate( arg_ret ); }

		if ( ret ) { return; }

	}


	// [!] : "\file" => "file"

	if ( n_path_slash_check( file, 0 ) )
	{
		n_string_copy( &file[ 1 ], file );
	}


	if ( ( n_posix_strlen( dir ) + 1 + n_posix_strlen( file ) ) >= N_PATH_MAX ) { return; }

	n_posix_sprintf_literal( arg_ret, "%s%s%s", dir, N_POSIX_SLASH, file );


	return;
}

void
n_path_maker
(
	const n_posix_char *dir,
	const n_posix_char *file,
	      n_posix_char *ret
)
{
//n_path_maker_slow( dir, file, ret ); return;

	if (
		( file == ret )
		||
		( n_path_is_drivename( dir ) )
		||
		( n_path_slash_check( file, 0 ) )
	)
	{
		n_path_maker_slow( dir, file, ret );
		return;
	}


	if ( ( n_posix_strlen( dir ) + 1 + n_posix_strlen( file ) ) >= N_PATH_MAX ) { return; }

	n_posix_sprintf_literal( ret, "%s%s%s", dir, N_POSIX_SLASH, file );


	return;
}

int
n_path_divider( const n_posix_char *arg_path, n_posix_char *arg_ret, int index )
{

	// [!] : Return Value
	//
	//	ret == 0 : no index
	//	ret >= 1 : index count


	n_posix_char  path[ N_PATH_MAX ];
	n_posix_char dummy[ N_PATH_MAX ];

	{

		int ret;

		if ( arg_ret == NULL )
		{
			ret = n_path_nonbreak( arg_path,   dummy, path );
		} else {
			ret = n_path_nonbreak( arg_path, arg_ret, path );
		}

		if ( ret ) { return 0; }

	}


	// [!] : not a path name

	if ( n_posix_false == n_path_is_abspath( path ) )
	{

		//if ( arg_ret != NULL ) { n_string_copy( path, arg_ret ); }

		return 0;
	}


	int  i = 0;
	int ii = 0;
	int  n = 0;
	n_posix_loop
	{

		if ( ( arg_ret != NULL )&&( index == n ) )
		{

			arg_ret[ ii ] = path[ i ];

			ii++;
			n_string_terminate( arg_ret, ii );

		}


		i++;
		if ( path[ i ] == N_STRING_CHAR_NUL ) { break; }


		if ( n_path_slash_check( path, i ) )
		{

			n++;

			i++;
			if ( path[ i ] == N_STRING_CHAR_NUL ) { break; }
		}
	}


	return ( 1 + n );
}

void
n_path_name( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	// [!] : don't check whether file or dir


	n_posix_char path[ N_PATH_MAX ];

	if ( n_path_nonbreak( arg_path, arg_ret, path ) ) { return; }


	int count = n_path_divider( path, NULL, 0 );
	if ( count == 0 )
	{
		n_string_copy( path, arg_ret );
	} else {
		n_path_divider( path, arg_ret, count - 1 );
	}


	return;
}

void
n_path_upperfolder( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	// [!] : don't check whether file or dir


	n_posix_char path[ N_PATH_MAX ];

	if ( n_path_nonbreak( arg_path, arg_ret, path ) ) { return; }


	n_posix_char name[ N_PATH_MAX ];

	n_path_name( path, name );
//n_posix_debug( path );
//n_posix_debug( name );

	if ( n_posix_false == n_path_is_abspath( name ) )
	{
		n_string_terminate( path, n_posix_strlen( path ) - n_posix_strlen( name ) );
	}


#ifdef N_POSIX_PLATFORM_WINDOWS

	if ( n_path_slash_check( path, n_posix_strlen( path ) - 1 ) )
	{
		n_string_terminate( path, n_posix_strlen( path ) - 1 );
	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_string_copy( path, arg_ret );


	return;
}

void
n_path_relative( n_posix_char *str )
{

	// [!] : this function is cheap by design
	//
	//	"C:\" will be ".\C:\"


	if ( str == NULL ) { return; }


	// [!] : nothing to do : already has "./" or ".\\"

	if (
		( str[ 0 ] == N_STRING_CHAR_DOT )
		&&
		(
			( str[ 0 ] == N_STRING_CHAR_NUL )
			&&
			( n_path_slash_check( str, 1 ) )
		)
	)
	{
		return;
	}


	s64 len = n_posix_strlen( str );
	if ( ( 2 + len ) >= N_PATH_MAX )
	{

		n_string_truncate( str );

		return;

	} else {

		n_posix_char s[ N_PATH_MAX ];

		n_posix_sprintf_literal( s, "%c%s%s", N_STRING_CHAR_DOT, N_POSIX_SLASH, str );

		n_string_copy( s, str );

	}


	return;
}

void
n_path_ext_get( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	n_posix_char path[ N_PATH_MAX ];

	if ( n_path_nonbreak( arg_path, arg_ret, path ) ) { return; }


	// [!] : "." is DBCS safe


	// [!] : like "...ext"
	// [x] : currently not used

	s64 pos  = -1;
	s64 dots =  0;


	s64 i = n_posix_strlen( path );
	n_posix_loop
	{ 

		if ( path[ i ] == N_STRING_CHAR_DOT )
		{

			if ( pos == -1 ) { pos = i; }

			dots++;
break;
			if ( dots >= 2 ) { break; }

		} else
		if ( n_path_slash_check( path, i ) )
		{
			break;
		}


		i--;
		if ( i < 0 ) { break; }
	}


	if ( dots == 1 )
	{
		n_string_copy( &path[ pos ], arg_ret );
	}


	return;
}

#define N_PATH_EXT_SET_ADD 0
#define N_PATH_EXT_SET_DEL 1

#define n_path_ext_add( e, p ) n_path_ext_set(    e, p, N_PATH_EXT_SET_ADD )
#define n_path_ext_mod( e, p ) n_path_ext_set(    e, p, N_PATH_EXT_SET_DEL )
#define n_path_ext_del(    p ) n_path_ext_set( NULL, p, N_PATH_EXT_SET_DEL )

#define n_path_ext_add_literal( a,b ) n_path_ext_add( n_posix_literal( a ), b )
#define n_path_ext_mod_literal( a,b ) n_path_ext_mod( n_posix_literal( a ), b )

// internal
void
n_path_ext_set( const n_posix_char *ext, n_posix_char *path, int mode )
{

	// [Mechanism] : "ext"
	//
	//	empty : remove extension
	//	other : add/replace extension
	//
	//	"txt" and ".txt" are acceptable


	// [!] : avoid making dot-file

	if ( n_string_is_empty( path ) ) { return; }


	s64 len = n_posix_strlen( path );

	if ( mode == N_PATH_EXT_SET_DEL )
	{

		n_posix_char str_ext[ N_PATH_MAX ];
		n_path_ext_get( path, str_ext );

		len -= n_posix_strlen( str_ext );

	}


	if ( n_string_is_empty( ext ) )
	{

		// [!] : n_path_ext_del()

		n_string_terminate( path, len );

	} else
	if ( ext[ 0 ] == N_STRING_CHAR_DOT )
	{

		n_string_copy( ext, &path[ len ] );

	} else {

		if ( ( len + 1 ) >= N_PATH_MAX ) { return; }

		n_posix_sprintf_literal( &path[ len ], "%c%s", N_STRING_CHAR_DOT, ext );

	}


	return;
}

#define n_path_ext_is_same_literal( f, t ) n_path_ext_is_same( n_posix_literal( f ), t )

n_posix_bool
n_path_ext_is_same( const n_posix_char *path1, const n_posix_char *path2 )
{

	n_posix_char ext1[ N_PATH_MAX ];
	n_posix_char ext2[ N_PATH_MAX ];


	n_path_ext_get( path1, ext1 );
	n_path_ext_get( path2, ext2 );


	return n_string_is_same( ext1, ext2 );
}

int
n_path_chdir( const n_posix_char *arg_path )
{

	n_posix_char   dir[ N_PATH_MAX ];
	n_posix_char dummy[ N_PATH_MAX ];

	if ( n_path_nonbreak( arg_path, dummy, dir ) ) { return 1; }


	if ( n_posix_stat_is_file( dir ) ) { n_path_upperfolder( dir, dir ); }
//n_posix_debug( dir );

	n_path_drivename_slash_add( dir );
//n_posix_debug( dir );


	return n_posix_chdir( dir );
}

n_posix_bool
n_path_rename( const n_posix_char *f, const n_posix_char *t )
{

	int ret = n_posix_rename( f, t );


	return ( ret == -1 );
}

void
n_path_short2long( const n_posix_char *arg_path, n_posix_char *arg_ret )
{

	n_posix_char path[ N_PATH_MAX ];

	if ( n_path_nonbreak( arg_path, arg_ret, path ) ) { return; }

//n_posix_debug( path );


#ifdef N_POSIX_PLATFORM_WINDOWS


	// [!] : get a drive name for performance

	n_posix_char drive[ N_PATH_MAX ];

	int count = n_path_divider( path, drive, 0 );

//n_posix_debug_literal( "Count %d : Drive %s", count, drive );

	if ( count == 0 )
	{

		// [!] : not a path : send back

		n_string_copy( path, arg_ret );

		return;

	} else
	if ( count == 1 )
	{

		// [!] : drive name

		n_string_copy( drive, arg_ret );

		return;

	} else
	if ( n_posix_false == n_string_search_simple_literal( path, "~" ) )
	{

		// [!] : not a short name

		n_string_copy( path, arg_ret );

		return;

	}


	// [!] : add drive name

	n_string_copy( drive, arg_ret );


	WIN32_FIND_DATA f;

	n_posix_char str[ N_PATH_MAX ];


	int i = 1;
	n_posix_loop
	{

		n_path_divider( path, str, i );

//n_posix_debug( str );


		n_path_maker( arg_ret, str, str );


		// [!] : nonnon~1 => nonnon_win

		ZeroMemory( &f, sizeof( WIN32_FIND_DATA ) );

		FindClose( FindFirstFile( str, &f ) );

		n_path_maker( arg_ret, f.cFileName, arg_ret );

//n_posix_debug( arg_ret );


		// [!] : path is too long

		if ( n_string_is_empty( arg_ret ) )
		{
			n_string_truncate( arg_ret );
			break;
		}


		i++;
		if ( i >= count ) { break; }
	}


#else // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_string_copy( path, arg_ret );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

void
n_path_arg2cmdline( int argc, n_posix_char *argv[], n_posix_char *ret )
{

	// [!] : MinGW GCC hasn't wmain()
	//
	//	use n_win_commandline() if you want Unicode scalability


	if ( ret == NULL ) { return; } else { n_string_truncate( ret ); }

	if ( argc <= 1 ) { return; }


	s64 cch = 0;
	s64 len = 0;

	s64 i = 1;
	n_posix_loop
	{

		len = n_posix_strlen( argv[ i ] );

		if ( ( cch + len ) >= N_PATH_MAX )
		{
			n_string_truncate( ret );
			return;
		} else {
			cch += n_posix_sprintf_literal( &ret[ cch ], "%s", argv[ i ] );
		}


		i++;
		if ( i >= argc ) { break; }


		len = n_posix_strlen( N_STRING_SPACE );

		if ( ( cch + len ) >= N_PATH_MAX )
		{
			n_string_truncate( ret );
			return;
		} else {
			cch += n_posix_sprintf_literal( &ret[ cch ], "%s", N_STRING_SPACE );
		}

	}

	n_path_short2long( ret, ret );


	return;
}

void*
n_path_getcwd( n_posix_char *arg_ret )
{

	if ( arg_ret == NULL ) { return NULL; }


	void *ret = n_posix_getcwd( arg_ret, N_PATH_MAX - 1 );


	n_path_short2long( arg_ret, arg_ret );


	return ret;
}

void
n_path_tmpname( n_posix_char *str )
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

n_posix_bool
n_path_is_childfolder( const n_posix_char *parent, const n_posix_char *child )
{

	n_posix_char p[ N_PATH_MAX ];
	n_posix_char c[ N_PATH_MAX ];

	n_string_copy( parent, p );
	n_string_copy( child,  c );

	if ( n_posix_stat_is_file( p ) ) { n_path_upperfolder( p, p ); }
	if ( n_posix_stat_is_file( c ) ) { n_path_upperfolder( c, c ); }


	int max_f = n_path_divider( p, NULL, 0 );
	int max_t = n_path_divider( c, NULL, 0 );

	if ( max_f == 0 ) { return n_posix_false; }
	if ( max_t == 0 ) { return n_posix_false; }

	// [!] : Windows : a string which only has drive name

#ifdef N_POSIX_PLATFORM_WINDOWS
	if ( max_f == 1 ) { return n_posix_false; }
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_char str_f[ N_PATH_MAX ];
	n_posix_char str_t[ N_PATH_MAX ];

	int i = 0;
	n_posix_loop
	{

		n_path_divider( p, str_f, i );
		n_path_divider( c, str_t, i );

		if ( n_posix_false == n_string_is_same( str_f, str_t ) ) { break; }

		i++;
		if ( i >= max_f ) { break; }
		if ( i >= max_t ) { break; }
	}


	if ( i == max_f ) { return n_posix_true; }


	return n_posix_false;
}


#endif // _H_NONNON_NEUTRAL_PATH

