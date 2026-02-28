// OrangeCat
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_DIR
#define _H_NONNON_NEUTRAL_DIR




#include "./string_path.c"
#include "./vector.c"




#define N_DIR_COUNT_MAX ULONG_MAX




typedef struct {

	// path : same as getcwd()
	// name : original name for displaying
	// low  : lower case name for comparison
	// ext  : extension cache for performance

	n_vector path;
	n_vector name;
	n_vector low;
	n_vector ext;

	n_type_int dir;
	n_type_int file;

} n_dir;




#define n_dir_all( d ) ( ( d )->file + ( d )->dir )

#define n_dir_path( d, i ) (n_posix_char*) ( d )->path.line[ i ]
#define n_dir_name( d, i ) (n_posix_char*) ( d )->name.line[ i ]
#define n_dir_low(  d, i ) (n_posix_char*) ( d )->low.line [ i ]
#define n_dir_ext(  d, i ) (n_posix_char*) ( d )->ext.line [ i ]

// internal
BOOL
n_dir_is_lower( const n_posix_char *str, n_type_int *cch )
{

	if ( str == NULL ) { return FALSE; }


	BOOL ret = TRUE;


	n_type_int i = 0;
	n_posix_loop
	{

		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }

		if (
			( n_string_is_alphabet( str, i ) )
			&&
			( FALSE == n_string_is_lower( str, i ) )
		)
		{
			ret = FALSE;
		}

		i++;

	}


	if ( cch != NULL ) { (*cch) = i; }


	return ret;
}

// internal
n_posix_inline const n_posix_char*
n_dir_extension_ptr_alias( const n_posix_char *str, n_type_int *cch )
{

	// [Mechanism]
	//
	//	a return value is alias to an input string
	//	this is not flexible but faster


	if ( cch != NULL ) { (*cch) = 0; }


	if ( str == NULL ) { return N_STRING_EMPTY; }


	n_type_int i = n_posix_strlen( str ); 
	n_type_int f = i;
	n_posix_loop
	{ 

		if ( str[ i ] == N_STRING_CHAR_DOT )
		{

			if ( cch != NULL ) { (*cch) = f - i; }

			return &str[ i ];

		} else
		if ( n_string_path_slash_check( str, i ) )
		{
			break;
		}


		if ( i == 0 ) { break; }
		i--;

	}


	return N_STRING_EMPTY;
}

#define n_dir_zero(  f    ) n_memory_zero( f,    sizeof( n_dir ) )
#define n_dir_alias( f, t ) n_memory_copy( f, t, sizeof( n_dir ) )

BOOL
n_dir_error( n_dir *d )
{

	if ( d == NULL ) { return TRUE; }

	if (
		( n_vector_error( &d->path ) )
		||
		( n_vector_error( &d->name ) )
		||
		( n_vector_error( &d->low  ) )
		||
		( n_vector_error( &d->ext  ) )
	)
	{
//n_posix_debug_literal( "Error : Data" );

		return TRUE;
	}

	if (
		( d->path.sy != d->name.sy )
		||
		( d->name.sy != d->low .sy )
		||
		( d->low .sy != d->ext .sy )
	)
	{
//n_posix_debug_literal( "Error : Integrity : %d %d %d", d->name.sy, d->low.sy, d->ext.sy );

		return TRUE;
	}


	return FALSE;
}

void
n_dir_new( n_dir *d )
{

	if ( d == NULL ) { return; }


	n_vector_new( &d->path );
	n_vector_new( &d->name );
	n_vector_new( &d->low  );
	n_vector_new( &d->ext  );

	d->dir = d->file = 0;


	return;
}

void
n_dir_free( n_dir *d )
{

	if ( d == NULL ) { return; }


	n_vector_free( &d->path );
	n_vector_free( &d->name );
	n_vector_free( &d->low  );
	n_vector_free( &d->ext  );

	n_dir_zero( d );


	return;
}

n_type_int
n_dir_add( n_dir *d, const n_posix_char *folder, const n_posix_char *name, const n_posix_char *abspath )
{

	if (       d == NULL ) { return 0; }
	if (  folder == NULL ) { return 0; }
	if (    name == NULL ) { return 0; }
	if ( abspath == NULL ) { return 0; }


	// [x] : Win32 : some API makes error dialog with removable drives
	//
	//	posix : stat()
	//	win32 : FindFirstFile()


	n_type_int i = d->dir + d->file;
	if ( i > N_DIR_COUNT_MAX ) { return -1; }

	if (

#ifdef N_POSIX_PLATFORM_WINDOWS
		( 1 == n_string_path_split_count( abspath ) )
		||
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

		( n_posix_stat_is_dir( abspath ) )

	)
	{
		if ( i > d->dir ) { i = d->dir; }
		d->dir++;
	} else
	if ( n_posix_stat_is_file( abspath ) )
	{
		if ( i > ( d->dir + d->file ) ) { i = d->dir + d->file; }
		d->file++;
	}


	if ( n_dir_all( d ) == 1 )
	{

		n_posix_char *s = n_string_carboncopy( name );

		if ( s == NULL )
		{
			s = n_memory_new( 1 * sizeof( n_posix_char ) );
			s[ 0 ] = N_STRING_CHAR_NUL;
		} else {
			n_string_lower( s, s );
		}

		n_vector_mod     ( &d->path, i, folder );
		n_vector_mod     ( &d->name, i, name   );
		n_vector_mod_fast( &d->low,  i, s      );
		n_vector_mod     ( &d->ext,  i, n_dir_extension_ptr_alias( s, NULL ) );

		//n_memory_free( s );

		return i;
	}


	n_type_int cch = 0;

	n_vector_add( &d->path, i, folder );

	if ( n_dir_is_lower( name, &cch ) )
	{

		n_vector_add_fast_cch( &d->name, i, name, cch );
		n_vector_add_fast_cch( &d->low,  i, name, cch );

		      n_type_int    ext_cch;
		const n_posix_char *ext_str = n_dir_extension_ptr_alias( name, &ext_cch );
		n_vector_add_fast_cch( &d->ext, i, ext_str, ext_cch );

	} else {

		n_vector_add_fast_cch( &d->name, i, name, cch );


		n_posix_char *s = n_string_new( cch );

		n_string_copy( name, s );
		n_string_lower( s, s );

		n_vector_add_fast_ptr( &d->low, i, s );


		      n_type_int    ext_cch;
		const n_posix_char *ext_str = n_dir_extension_ptr_alias( s, &ext_cch );
		n_vector_add_fast_cch( &d->ext, i, ext_str, ext_cch );

		//n_memory_free( s );

	}

//n_posix_debug_literal( "%s", n_dir_name( d, i ) );


	return i;
}

#ifdef N_POSIX_PLATFORM_WINDOWS
n_type_int
n_dir_add_fast( n_dir *d, const n_posix_char *folder, const n_posix_char *name, DWORD attrib, BOOL is_drive )
{

	if (      d == NULL ) { return 0; }
	if ( folder == NULL ) { return 0; }
	if (   name == NULL ) { return 0; }


	// [x] : Win32 : some API makes error dialog with removable drives
	//
	//	posix : stat()
	//	win32 : FindFirstFile()


	n_type_int i = d->dir + d->file;
	if ( i > N_DIR_COUNT_MAX ) { return -1; }

	if (
		( is_drive )
		||
		( FILE_ATTRIBUTE_DIRECTORY & attrib )
	)
	{
		if ( d->dir  >= ( UINT_MAX / 2 ) ) { return 0; }
		if ( i > d->dir ) { i = d->dir; }
		d->dir++;
	} else {
		if ( d->file >= ( UINT_MAX / 2 ) ) { return 0; }
		if ( i > ( d->dir + d->file ) ) { i = d->dir + d->file; }
		d->file++;
	}


	if ( n_dir_all( d ) == 1 )
	{

		n_posix_char *s = n_string_carboncopy( name );

		if ( s == NULL )
		{
			s = n_memory_new( 1 * sizeof( n_posix_char ) );
			s[ 0 ] = N_STRING_CHAR_NUL;
		} else {
			n_string_lower( s, s );
		}

		n_vector_mod     ( &d->path, i, folder );
		n_vector_mod     ( &d->name, i, name   );
		n_vector_mod_fast( &d->low,  i, s      );
		n_vector_mod     ( &d->ext,  i, n_dir_extension_ptr_alias( s, NULL ) );

		//n_memory_free( s );

		return i;
	}


	n_type_int cch = 0;

	n_vector_add( &d->path, i, folder );

	if ( n_dir_is_lower( name, &cch ) )
	{

		n_vector_add_fast_cch( &d->name, i, name, cch );
		n_vector_add_fast_cch( &d->low,  i, name, cch );

		      n_type_int    ext_cch;
		const n_posix_char *ext_str = n_dir_extension_ptr_alias( name, &ext_cch );
		n_vector_add_fast_cch( &d->ext, i, ext_str, ext_cch );

	} else {

		n_vector_add_fast_cch( &d->name, i, name, cch );


		n_posix_char *s = n_string_new( cch );

		n_string_copy( name, s );
		n_string_lower( s, s );

		n_vector_add_fast_ptr( &d->low, i, s );


		      n_type_int    ext_cch;
		const n_posix_char *ext_str = n_dir_extension_ptr_alias( s, &ext_cch );
		n_vector_add_fast_cch( &d->ext, i, ext_str, ext_cch );

		//n_memory_free( s );

	}

//n_posix_debug_literal( "%s", n_dir_name( d, i ) );


	return i;
}
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

void
n_dir_mod( n_dir *d, n_type_int i, const n_posix_char *dir, const n_posix_char *str )
{

	if (   d == NULL ) { return; }
	if ( str == NULL ) { return; }

	if ( i >= n_dir_all( d ) ) { return; }


	n_vector_mod( &d->path, i, dir );
	n_vector_mod( &d->name, i, str );
	n_vector_mod( &d->low,  i, str );
	n_vector_mod( &d->ext,  i, n_dir_extension_ptr_alias( str, NULL ) );


	{

		n_posix_char *low = n_dir_low( d, i );
		n_posix_char *ext = n_dir_ext( d, i );


		n_string_lower( low, low );
		n_string_lower( ext, ext );

	}


	return;
}

void
n_dir_del( n_dir *d, n_type_int i )
{

	if ( d == NULL ) { return; }

	if ( i >= n_dir_all( d ) ) { return; }


	if ( i < d->dir )
	{
		d->dir --;
	} else {
		d->file--;
	}

	n_vector_del( &d->path, i );
	n_vector_del( &d->name, i );
	n_vector_del( &d->low,  i );
	n_vector_del( &d->ext,  i );


	return;
}

// internal
BOOL
n_dir_load_internal( n_dir *d, const n_posix_char *folder, BOOL recurse_onoff )
{

	if ( d == NULL ) { return TRUE; }


	BOOL ret = FALSE;


	if ( folder != NULL )
	{

		n_posix_DIR *dp = n_posix_opendir_nodot( folder );
		if ( dp == NULL ) { return TRUE; }


#ifdef N_POSIX_PLATFORM_WINDOWS

		n_posix_char *curdir = n_string_path_folder_current_new();
		n_string_path_folder_change_fast( folder );


		n_posix_char *path = n_string_path_folder_current_new();


		n_posix_loop
		{//break;

			n_posix_dirent *dirent = n_posix_readdir( dp );
			if ( dirent == NULL ) { break; }

			n_type_int check = n_dir_add_fast( d, path, dirent->d_name, dirent->dwFileAttributes, FALSE );
//n_posix_debug_literal( "%s\n%s", folder, dirent->d_name );
			if ( check == -1 )
			{
				ret = TRUE;
				break;
			}

			if ( recurse_onoff )
			{
				if ( FILE_ATTRIBUTE_DIRECTORY & dirent->dwFileAttributes )
				{
					n_dir_load_internal( d, dirent->d_name, recurse_onoff );
				}
			}

		}


		n_string_path_free( path );


		n_string_path_folder_change_fast( curdir );
		n_string_path_free( curdir );


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


		// [!] : 1 == n_posix_strlen( N_POSIX_SLASH )

		n_posix_loop
		{//break;

			n_posix_dirent *dirent = n_posix_readdir( dp );
			if ( dirent == NULL ) { break; }

			if ( dirent->d_name[ 0 ] == n_posix_literal( '.' ) ) { continue; }

			n_posix_char *item = n_string_path_make_new( folder, dirent->d_name );

			n_type_int check = n_dir_add( d, folder, dirent->d_name, item );
//n_posix_debug_literal( "%s\n%s", folder, dirent->d_name );
			if ( check == -1 )
			{
				ret = TRUE;
				break;
			}

			if ( recurse_onoff )
			{
				if ( n_posix_stat_is_dir( item ) )
				{
					n_dir_load_internal( d, item, recurse_onoff );
				}
			}

			n_string_path_free( item );

		}


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


		n_posix_closedir( dp );


//n_posix_debug_literal( "%d %d", d->dir, d->file );

	} else {

#ifdef N_POSIX_PLATFORM_WINDOWS

		n_posix_char drivename[ 100 ] = n_posix_literal( "A:\\" );

		n_type_int i = 0;
		n_posix_loop
		{

			if ( DRIVE_NO_ROOT_DIR != GetDriveType( drivename ) )
			{
				n_dir_add_fast( d, N_STRING_EMPTY, drivename, 0, TRUE );
			}

			drivename[ 0 ]++;

			i++;
			if ( i >= 26 ) { break; }
		}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

	}


	return ret;
}

#define n_dir_load(           d, folder ) n_dir_load_main( d, folder, FALSE )
#define n_dir_load_recursive( d, folder ) n_dir_load_main( d, folder, TRUE  )

BOOL
n_dir_load_main( n_dir *d, const n_posix_char *folder, BOOL recurse_onoff )
{

	if ( d == NULL ) { return TRUE; }


	if ( folder != NULL )
	{
		// [!] : n_dir.path : "folder" needs to be an absolute path

		if ( FALSE == n_string_path_is_abspath( folder ) ) { return TRUE; }
	}


	n_dir_free( d );
	n_dir_new ( d );


	BOOL ret = n_dir_load_internal( d, folder, recurse_onoff );
	if ( ret )
	{
		n_dir_free( d );
	}


	return ret;
}

#define N_DIR_SWAP_SIMPLE 0
#define N_DIR_SWAP_EXT    1
#define N_DIR_SWAP_ALL    2
#define N_DIR_SWAP_PATH   3
#define N_DIR_SWAP_SIZE   4
#define N_DIR_SWAP_ATIME  5
#define N_DIR_SWAP_CTIME  6
#define N_DIR_SWAP_MTIME  7

// internal
BOOL
n_dir_swap( n_dir *d, n_type_int upper, n_type_int lower, int option )
{

	if ( n_dir_error( d ) ) { return FALSE; }


	if ( upper >= d->name.sy ) { return FALSE; }
	if ( lower >= d->name.sy ) { return FALSE; }


	if ( ( upper < d->dir )&&( lower >= d->dir ) )  { return FALSE; }


	n_posix_char *path_u = n_dir_path( d, upper );
	n_posix_char *path_l = n_dir_path( d, lower );

	n_posix_char *name_u = n_dir_name( d, upper );
	n_posix_char *name_l = n_dir_name( d, lower );


	n_posix_char *u = NULL;
	n_posix_char *l = NULL;


	if ( option == N_DIR_SWAP_SIZE )
	{

		n_posix_char *str_u = n_string_path_make_new( path_u, name_u );
		n_posix_char *str_l = n_string_path_make_new( path_l, name_l );

		n_type_int size_u = n_posix_stat_size( str_u );
		n_type_int size_l = n_posix_stat_size( str_l );

		n_string_path_free( str_u );
		n_string_path_free( str_l );

		if ( size_u < size_l )
		{

			return FALSE;

		} else
		if ( size_u == size_l )
		{

			// [!] : Name

			u = n_dir_low( d, upper );
			l = n_dir_low( d, lower );

			if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

		}

	} else
	if ( option == N_DIR_SWAP_ATIME )
	{

		n_posix_char *str_u = n_string_path_make_new( path_u, name_u );
		n_posix_char *str_l = n_string_path_make_new( path_l, name_l );

		time_t atime_u = n_posix_stat_atime( str_u );
		time_t atime_l = n_posix_stat_atime( str_l );

		n_string_path_free( str_u );
		n_string_path_free( str_l );

		if ( atime_u < atime_l )
		{

			return FALSE;

		} else
		if ( atime_u == atime_l )
		{

			// [!] : Name

			u = n_dir_low( d, upper );
			l = n_dir_low( d, lower );

			if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

		}

	} else
	if ( option == N_DIR_SWAP_CTIME )
	{

		n_posix_char *str_u = n_string_path_make_new( path_u, name_u );
		n_posix_char *str_l = n_string_path_make_new( path_l, name_l );

		if ( n_posix_stat_ctime( str_u ) < n_posix_stat_ctime( str_l ) )
		{
			n_string_path_free( str_u );
			n_string_path_free( str_l );

			return FALSE;
		}

		time_t ctime_u = n_posix_stat_ctime( str_u );
		time_t ctime_l = n_posix_stat_ctime( str_l );

		n_string_path_free( str_u );
		n_string_path_free( str_l );

		if ( ctime_u < ctime_l )
		{

			return FALSE;

		} else
		if ( ctime_u == ctime_l )
		{

			// [!] : Name

			u = n_dir_low( d, upper );
			l = n_dir_low( d, lower );

			if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

		}

	} else
	if ( option == N_DIR_SWAP_MTIME )
	{

		n_posix_char *str_u = n_string_path_make_new( path_u, name_u );
		n_posix_char *str_l = n_string_path_make_new( path_l, name_l );

		time_t mtime_u = n_posix_stat_mtime( str_u );
		time_t mtime_l = n_posix_stat_mtime( str_l );

		n_string_path_free( str_u );
		n_string_path_free( str_l );

		if ( mtime_u < mtime_l )
		{

			return FALSE;

		} else
		if ( mtime_u == mtime_l )
		{

			// [!] : Name

			u = n_dir_low( d, upper );
			l = n_dir_low( d, lower );

			if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

		}

	} else
	if ( option == N_DIR_SWAP_PATH )
	{

		// [!] : Path

		n_type_int u_count = n_string_path_split_count( n_dir_path( d, upper ) );
		n_type_int l_count = n_string_path_split_count( n_dir_path( d, lower ) );
		n_type_int   count = n_posix_min_n_type_int( u_count, l_count );

		if ( u_count > l_count ) { return FALSE; }

		n_type_int i = 0;
		while( u_count == l_count )
		{

			n_posix_char *str_u = n_string_path_split_new( u, i );
			n_posix_char *str_l = n_string_path_split_new( l, i );

			n_string_path_split( u, str_u, i );
			n_string_path_split( l, str_l, i );

			int compare = n_string_compare( str_u, str_l );

			n_string_path_free( str_u );
			n_string_path_free( str_l );

			if ( 0 > compare ) { return FALSE; } else if ( 0 < compare ) { break; }


			// [!] : Name

			n_posix_char *u_low = n_dir_low( d, upper );
			n_posix_char *l_low = n_dir_low( d, lower );

			if ( 0 >= n_string_compare_strict( u_low, l_low ) ) { return FALSE; } else { break; }


			i++;
			if ( i >= count ) { break; }
		}

	} else
	if ( option == N_DIR_SWAP_EXT )
	{

		// [!] : Extension : file only

		if ( upper < d->dir ) { return FALSE; }


		// [!] : Extension : same name

		u = n_dir_ext( d, upper );
		l = n_dir_ext( d, lower );

		if ( n_string_is_same_strict( u, l ) ) { return FALSE; }


		if ( 0 >= n_string_compare( u, l ) ) { return FALSE; }

	} else
	if ( option == N_DIR_SWAP_ALL )
	{

		// [!] : Extension : file only

		if ( upper >= d->dir )
		{

			// [!] : Extension : different name

			u = n_dir_ext( d, upper );
			l = n_dir_ext( d, lower );

			if ( FALSE == n_string_is_same_strict( u, l ) ) { return FALSE; }

		}


		// [!] : Name

		u = n_dir_low( d, upper );
		l = n_dir_low( d, lower );

		if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

	} else {

		// [!] : Name

		u = n_dir_low( d, upper );
		l = n_dir_low( d, lower );

		if ( 0 >= n_string_compare_strict( u, l ) ) { return FALSE; }

	}

//n_posix_debug_literal( "%s : %s", u, l );return FALSE;


	n_vector_swap( &d->path, upper, lower );
	n_vector_swap( &d->name, upper, lower );
	n_vector_swap( &d->low,  upper, lower );
	n_vector_swap( &d->ext,  upper, lower );


	return TRUE;
}

#define n_dir_sort( d ) n_dir_sort_internal( d, N_DIR_SWAP_EXT ); n_dir_sort_internal( d, N_DIR_SWAP_ALL )

#define n_dir_sort_simple( d ) n_dir_sort_internal( d, N_DIR_SWAP_SIMPLE )
#define n_dir_sort_path(   d ) n_dir_sort_internal( d, N_DIR_SWAP_PATH   )
#define n_dir_sort_size(   d ) n_dir_sort_internal( d, N_DIR_SWAP_SIZE   )
#define n_dir_sort_atime(  d ) n_dir_sort_internal( d, N_DIR_SWAP_ATIME  )
#define n_dir_sort_ctime(  d ) n_dir_sort_internal( d, N_DIR_SWAP_CTIME  )
#define n_dir_sort_mtime(  d ) n_dir_sort_internal( d, N_DIR_SWAP_MTIME  )

void
n_dir_sort_internal( n_dir *d, int option )
{

	if ( n_dir_error( d ) ) { return; }


	n_type_int i  = 0;
	n_type_int ii = i + 1;
	n_posix_loop
	{

		n_dir_swap( d, i, ii, option );


		ii++;
		if ( ii >= d->name.sy )
		{

			i++;
			if ( i >= d->name.sy ) { break; }

			ii = i + 1;

		}
	}


	return;
}

void
n_dir_carboncopy( const n_dir *dir_old, n_dir *dir_new )
{

	n_vector_carboncopy( &dir_old->path, &dir_new->path, N_STRING_CRLF );
	n_vector_carboncopy( &dir_old->name, &dir_new->name, N_STRING_CRLF );
	n_vector_carboncopy( &dir_old->ext,  &dir_new->ext,  N_STRING_CRLF );
	n_vector_carboncopy( &dir_old->low,  &dir_new->low,  N_STRING_CRLF );

	dir_new->dir  = dir_old->dir;
	dir_new->file = dir_old->file;


	return;
}

void
n_dir_debug_iotest( const n_posix_char *cmdline )
{

#ifdef _H_NONNON_WIN32_WIN


	n_win_exedir2curdir();


	n_dir d; n_dir_zero( &d );
	if ( n_dir_load( &d, cmdline ) )
	{

		n_posix_debug_literal( "n_dir_load()" );

		return;
	}


	{

		u32 prv = n_posix_tickcount();

		n_dir_sort( &d );

		u32 cur = n_posix_tickcount();

		n_posix_debug_literal( "%d msec.", (int) ( cur - prv ) );

	}


	n_vector_save_literal( &d.path, "path.txt", N_STRING_CRLF );
	n_vector_save_literal( &d.name, "name.txt", N_STRING_CRLF );
	n_vector_save_literal( &d.low,   "low.txt", N_STRING_CRLF );
	n_vector_save_literal( &d.ext,   "ext.txt", N_STRING_CRLF );


	n_dir_free( &d );


#endif // #ifdef _H_NONNON_WIN32_WIN


	return;
}


#endif // _H_NONNON_NEUTRAL_DIR

