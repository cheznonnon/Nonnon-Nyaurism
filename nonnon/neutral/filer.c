// Mau
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [ Mechanism ]
//
//	you need to call n_filer_is_locked() before file/folder operation
//	you need to call n_filer_is_same() after copying




#ifndef _H_NONNON_NEUTRAL_FILER
#define _H_NONNON_NEUTRAL_FILER




#include "./dir.c"
#include "./string_path.c"




n_posix_bool
n_filer_is_locked_single( const n_posix_char *name )
{

	n_posix_bool ret = n_posix_false;

/*
	// [x] : don't use this solution : a target file will be truncated

	FILE *fp = n_posix_fopen_write( name );
	if ( fp == NULL ) { ret = n_posix_true; }
	n_posix_fclose( fp );
*/

/*
	// [x] : rename() based solution : hangup in some cases

	if ( n_posix_stat_is_dir( name ) )
	{

		n_posix_char *tmp = n_string_path_tmpname_new( N_STRING_EMPTY );
		n_posix_char *upp = n_string_path_upperfolder_new( name );
		n_posix_char *s_f = n_string_path_carboncopy( name );
		n_posix_char *s_t = n_string_path_make_new( upp, tmp );

		if ( n_posix_false == n_posix_rename( s_f, s_t ) )
		{
			n_posix_rename( s_t, s_f );
		} else {
			ret = n_posix_true;
		}

		n_string_path_free( tmp );
		n_string_path_free( upp );
		n_string_path_free( s_f );
		n_string_path_free( s_t );

	}
*/

#ifdef N_POSIX_PLATFORM_WINDOWS

	if ( n_posix_stat_is_file( name ) )
	{

		HANDLE h = CreateFile
		(
			name,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if ( h == INVALID_HANDLE_VALUE )
		{
			ret = n_posix_true;
		}

		CloseHandle( h );

	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

n_posix_bool
n_filer_is_locked( const n_posix_char *abspath )
{

	n_dir d; n_dir_zero( &d );
	if ( n_dir_load_recursive( &d, abspath ) ) { return n_filer_is_locked_single( abspath ); }


	if ( 0 == n_dir_all( &d ) ) { n_dir_free( &d ); return n_posix_false; }


	// [Needed] : order is random
	n_dir_sort_path( &d );


	n_posix_bool is_locked = n_posix_false;

	n_type_int i = 0;
	n_posix_loop
	{//break;

		if ( i >= n_dir_all( &d ) ) { break; }

		n_posix_char *str = n_string_path_make_new( n_dir_path( &d, i ), n_dir_name( &d, i ) );

		is_locked = n_filer_is_locked_single( str );
		if ( is_locked ) { break; }

		n_string_path_free( str );


		i++;

	}


	n_dir_free( &d );


	return is_locked;
}

#define n_filer_remove( abspath ) n_filer_remove_internal( abspath, n_posix_true )

// internal
n_posix_bool
n_filer_remove_internal( const n_posix_char *abspath, n_posix_bool is_toplevel )
{
//return n_posix_false;

#ifdef N_POSIX_PLATFORM_WINDOWS

	SetFileAttributes( abspath, 0 );

#endif

	int type = n_posix_stat_type( abspath );

	if ( type == N_POSIX_STAT_TYPE_FILE )
	{
//n_posix_debug_literal( "Delete : File : %s", abspath );

		return ( 0 != n_posix_unlink( abspath ) );

	} else
	if ( type == N_POSIX_STAT_TYPE_DIRECTORY )
	{
//n_posix_debug_literal( "Delete : Folder : %s", abspath );

		// [!] : n_posix_rmdir() : errno incompatibility
		//
		// 	Win95   : ret = -1 : errno = EACCES
		// 	Win2000 : ret = -1 : errno = ENOTEMPTY


		n_dir d; n_dir_zero( &d );
		if ( n_dir_load( &d, abspath ) )
		{

			n_dir_free( &d );

			n_posix_rmdir( abspath );

			return n_posix_false;
		}


		// [x] : flaky : n_posix_rmdir() : return -1 when succeded in some cases

		if ( 0 == n_dir_all( &d ) )
		{

			n_dir_free( &d );

			n_posix_rmdir( abspath );

			return n_posix_false;
		}


		// [Needed] : order is random
		n_dir_sort_path( &d );


		// [!] : main

		n_type_int i = 0;
		n_posix_loop
		{

			if ( i >= n_dir_all( &d ) ) { break; }


			n_posix_char *str = n_string_path_make_new( abspath, n_dir_name( &d, i ) );

			n_filer_remove_internal( str, n_posix_false );

			n_string_path_free( str );


			i++;

		}


		n_dir_free( &d );


		// [!] : try to delete self

		n_filer_remove( abspath );

	} else
	if ( type == N_POSIX_STAT_TYPE_NOTEXIST )
	{
		return n_posix_true;
	}


	return n_posix_false;
}

// internal
n_type_int
n_filer_buffersize( n_type_int size )
{

	// [!] : 1MB

	const n_type_int byte_min = ( 1024 * 1024 * 1 );


#ifdef N_POSIX_PLATFORM_WINDOWS


	MEMORYSTATUS memstat; GlobalMemoryStatus( &memstat );

	// [!] : prevent overflow

	// [!] : size_t and DWORD is compatible in 32bit environment

	memstat.dwAvailPhys = n_posix_min_n_type_int( INT_MAX, memstat.dwAvailPhys );

	n_type_int byte_max = n_posix_max_n_type_int( byte_min, memstat.dwAvailPhys );
	n_type_int byte     = n_posix_min_n_type_int( byte_max, size                );


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_type_int byte = byte_min;


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return byte;
}

// internal
void
n_filer_copy_file( FILE *fp_i, FILE *fp_o, n_type_int size )
{

/*
	// [!] : safe but slow

	n_posix_loop
	{
 
		unsigned char c;

		n_posix_fread ( &c, 1, 1, fp_i );

		if ( 0 != feof( fp_i ) ) { break; }

		n_posix_fwrite( &c, 1, 1, fp_o );

	}

	return;
*/


	const n_type_int buffer = n_filer_buffersize( size );


	if ( size <= buffer )
	{

		u8 *ptr = n_memory_new_closed( size );

		n_posix_fread ( ptr, size, 1, fp_i );
		n_posix_fwrite( ptr, size, 1, fp_o );

		n_memory_free_closed( ptr );

	} else {

		u8 *ptr = n_memory_new_closed( buffer );

		n_type_int i = 0;
		n_posix_loop
		{

			n_posix_fread ( ptr, buffer, 1, fp_i );
			n_posix_fwrite( ptr, buffer, 1, fp_o );


			i += buffer;

			// [!] : check the next's next

			if ( size < ( i + buffer ) ) { break; }
		}


		// [!] : output the rest of data

		if ( size != i )
		{
			n_posix_fread ( ptr, (size - i), 1, fp_i );
			n_posix_fwrite( ptr, (size - i), 1, fp_o );
		}


		n_memory_free_closed( ptr );

	}


	return;
}

// internal
n_posix_bool
n_filer_timestamp_internal( const n_posix_char *f, const n_posix_char *t )
{

	if ( f == NULL ) { return n_posix_true; }
	if ( t == NULL ) { return n_posix_true; }


	// [!] : folders have no timestamp

	if ( n_posix_stat_is_dir( f ) ) { return n_posix_true; }
	if ( n_posix_stat_is_dir( t ) ) { return n_posix_true; }


	n_posix_bool ret = n_posix_false;


#ifdef N_POSIX_PLATFORM_WINDOWS

	// [x] : errors
	//
	//	a : when folders
	//	b : locked files : GetLastError() returns ERROR_SHARING_VIOLATION (32) with "from"
	//	c : unknown reasons

/*
	n_posix_utimbuf ut;

	ut.actime  = n_posix_stat_atime( f );
	ut.modtime = n_posix_stat_mtime( f );

	n_posix_utime( t, &ut );
*/
/*
	WIN32_FILE_ATTRIBUTE_DATA wfad; GetFileAttributesEx( f, GetFileExInfoStandard, &wfad );

	HANDLE h = CreateFile( t, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h != INVALID_HANDLE_VALUE )
	{

		FILETIME create = wfad.ftCreationTime;
		FILETIME access = wfad.ftLastAccessTime;
		FILETIME write  = wfad.ftLastWriteTime;

		SetFileTime( h, &create, &access, &write );

		CloseHandle( h );
	}
*/

	HANDLE h_f = CreateFile( f, GENERIC_READ , FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	//DWORD  e_f = GetLastError();
	HANDLE h_t = CreateFile( t, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	//DWORD  e_t = GetLastError();

	if (
		( h_f != INVALID_HANDLE_VALUE )
		&&
		( h_t != INVALID_HANDLE_VALUE )
	)
	{

		FILETIME create, access, write;

		GetFileTime( h_f, &create, &access, &write );
		SetFileTime( h_t, &create, &access, &write );

	} else {

		ret = n_posix_true;

//if ( ( h_f == INVALID_HANDLE_VALUE ) ) { n_posix_debug_literal( "f : %s : %d ", f, (int) e_f ); }
//if ( ( h_t == INVALID_HANDLE_VALUE ) ) { n_posix_debug_literal( "t : %s : %d ", t, (int) e_t ); }

	}


	if ( h_f != INVALID_HANDLE_VALUE ) { CloseHandle( h_f ); }
	if ( h_t != INVALID_HANDLE_VALUE ) { CloseHandle( h_t ); }


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_utimbuf ut;

	ut.actime  = n_posix_stat_atime( f );
	ut.modtime = n_posix_stat_mtime( f );

	n_posix_utime( t, &ut );


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

// internal
n_posix_bool
n_filer_timestamp( const n_posix_char *f, const n_posix_char *t )
{

	n_posix_bool ret = n_posix_false;


	int i = 0;
	n_posix_loop
	{

		ret = n_filer_timestamp_internal( f, t );
		if ( ret == n_posix_false ) { break; }

		// [!] : spell a magic
		n_posix_sleep( 10 );

		i++;
		if ( i >= 100 ) { break; }
	}


	return ret;
}

// internal
void
n_filer_autofolder( const n_posix_char *abspath )
{

	n_type_int count = n_string_path_split_count( abspath );
	if ( count == 0 ) { return; }


	n_posix_char *path = n_string_path_split_new( abspath, 0 );


	n_type_int i = 1;
	n_posix_loop
	{

		n_posix_char *nam = n_string_path_split_new( abspath, i );
		n_posix_char *dir = n_string_path_make_new( path, nam );
//n_posix_debug_literal( "%s\n%s\n%s", abspath, nam, dir );

#ifdef N_POSIX_PLATFORM_WINDOWS
		n_posix_mkdir( dir );
#else // #ifdef N_POSIX_PLATFORM_WINDOWS
		n_posix_mkdir( dir, N_POSIX_PERMISSION_DEFAULT );
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


		n_string_path_free( path );
		path = n_string_path_carboncopy( dir );

		n_string_path_free( nam );


		i++;
		if ( i >= count ) { break; }
	}


	n_string_path_free( path );


	return;
}

n_posix_bool
n_filer_is_same( const n_posix_char *f, const n_posix_char *t )
{

	if ( n_string_is_same( f, t ) ) { return n_posix_true; }


	int type_f = n_posix_stat_type( f );
	int type_t = n_posix_stat_type( t );

	if ( type_f != type_t ) { return n_posix_false; }


	n_posix_bool ret = n_posix_false;

	if ( type_f == N_POSIX_STAT_TYPE_FILE )
	{

		n_type_int f_size = n_posix_stat_size( f );
		n_type_int t_size = n_posix_stat_size( t );

		if ( f_size != t_size ) { return n_posix_false; }


		FILE *f_fp = n_posix_fopen_read( f );
		FILE *t_fp = n_posix_fopen_read( t );

		if ( ( f_fp == NULL )||( t_fp == NULL ) )
		{

			if ( ( f_fp == NULL )&&( t_fp == NULL ) ) { ret = n_posix_true; } else { ret = n_posix_false; }

			n_posix_fclose( f_fp );
			n_posix_fclose( t_fp );

			return ret;
		}


//u32 tick = n_posix_tickcount();

		n_type_int buffer = n_filer_buffersize( f_size );

		if ( buffer == 0 )
		{

			n_posix_fclose( f_fp );
			n_posix_fclose( t_fp );

			return n_posix_true;
		}


		n_type_int i = 0;
		n_posix_loop
		{

			// [Mechanism]
			//
			//	fread() stops file reading at EOF
			//
			//	"int" is 4 times faster than "char"
			//	buffered I/O is 10 times faster than "int"

			u8 *f_p = n_memory_new_closed( buffer );
			u8 *t_p = n_memory_new_closed( buffer );

			if ( ( i + buffer ) > f_size )
			{
				n_memory_zero( f_p, buffer );
				n_memory_zero( t_p, buffer );
			}

			n_posix_fread( f_p, buffer, 1, f_fp );
			n_posix_fread( t_p, buffer, 1, t_fp );

			n_posix_bool is_same = n_memory_is_same( f_p, t_p, buffer );

			n_memory_free_closed( f_p );
			n_memory_free_closed( t_p );

			if ( n_posix_false == is_same ) { ret = n_posix_false; break; }


			i += buffer;
			if ( i >= f_size ) { ret = n_posix_true; break; }
		}

//n_posix_debug_literal( "%d", (int) n_posix_tickcount() - tick );


		n_posix_fclose( f_fp );
		n_posix_fclose( t_fp );

	} else {

		n_dir f_d; n_dir_zero( &f_d );
		n_dir t_d; n_dir_zero( &t_d );

		//n_dir_load_recursive( &f_d, f );
		//n_dir_load_recursive( &t_d, t );

		n_dir_load( &f_d, f );
		n_dir_load( &t_d, t );


		if ( ( 0 == n_dir_all( &f_d ) )&&( 0 == n_dir_all( &t_d ) ) )
		{
//n_posix_debug_literal( "" );
			n_dir_free( &f_d );
			n_dir_free( &t_d );

			return n_posix_true;
		}

		if (
			( f_d.dir  != t_d.dir  )
			||
			( f_d.file != t_d.file )
		)
		{

			n_dir_free( &f_d );
			n_dir_free( &t_d );

			return n_posix_false;
		}


		n_dir_sort_path( &f_d );
		n_dir_sort_path( &t_d );


		n_type_int i = 0;
		n_posix_loop
		{

			if ( i >= n_dir_all( &f_d ) ) { ret = n_posix_true; break; }

			if ( n_string_is_same( n_dir_name( &f_d, i ), n_dir_name( &t_d, i ) ) )
			{

				n_posix_char *f_name = n_string_path_make_new( n_dir_path( &f_d, i ), n_dir_name( &f_d, i ) );
				n_posix_char *t_name = n_string_path_make_new( n_dir_path( &t_d, i ), n_dir_name( &t_d, i ) );

				ret = n_filer_is_same( f_name, t_name );

				n_string_path_free( f_name );
				n_string_path_free( t_name );

				if ( ret == n_posix_false ) { break; }

			} else {

				break;
			}

			i++;

		}

		n_dir_free( &f_d );
		n_dir_free( &t_d );

	}


	return ret;
}

// internal
n_posix_bool
n_filer_copy_single( const n_posix_char *f, const n_posix_char *t, n_posix_bool is_merge )
{

	if ( n_string_is_same( f, t ) ) { return n_posix_true; }


	int type_f = n_posix_stat_type( f );
	int type_t = n_posix_stat_type( t );

	if ( type_f == N_POSIX_STAT_TYPE_NOTEXIST ) { return n_posix_true; }


	if ( type_f == N_POSIX_STAT_TYPE_FILE )
	{

		if ( ( is_merge == n_posix_false )&&( type_t != N_POSIX_STAT_TYPE_NOTEXIST ) ) { return n_posix_true; }


		// [!] : for SSDs or USB drives

		if ( n_posix_false == n_filer_is_same( f, t ) )
		{

			FILE *fp_f = n_posix_fopen_read ( f );
			FILE *fp_t = n_posix_fopen_write( t );

			if ( ( fp_f == NULL )||( fp_t == NULL ) )
			{
				n_posix_fclose( fp_f );
				n_posix_fclose( fp_t );
				return n_posix_true;
			}

			n_filer_copy_file( fp_f, fp_t, n_posix_stat_size( f ) );

			n_posix_fclose( fp_f );
			n_posix_fclose( fp_t );

		}


		n_filer_timestamp( f, t );

	} else
	if ( type_f == N_POSIX_STAT_TYPE_DIRECTORY )
	{

		if ( ( is_merge == n_posix_false )&&( type_t != N_POSIX_STAT_TYPE_NOTEXIST ) ) { return n_posix_true; }

		n_filer_autofolder( t );

	}


#ifdef N_POSIX_PLATFORM_WINDOWS

	SetFileAttributes( t, GetFileAttributes( f ) );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return n_posix_false;
}

#define n_filer_copy(  f, t ) n_filer_copy_internal( f, t, n_posix_false )
#define n_filer_merge( f, t ) n_filer_copy_internal( f, t, n_posix_true  )

// [!] : int percent = (n_type_real) copied / count * 100;

typedef void (*n_filer_copy_callback_type)( n_type_int copied, n_type_int count );

n_filer_copy_callback_type n_filer_copy_callback = NULL;

n_posix_bool
n_filer_copy_internal( const n_posix_char *f, const n_posix_char *t, n_posix_bool is_merge )
{
//n_posix_debug_literal( "%s\n%s", f, t ); return EPERM;


	// [!] : why not use on-the-fly copying?
	//
	//	because infinite looping will happens when a source is a destination's ancestor


	if ( n_string_is_same( f, t ) ) { return n_posix_true; }


	// [!] : lock contents in a source directory

	n_dir d; n_dir_zero( &d );
	if ( n_dir_load_recursive( &d, f ) )
	{

		// [!] : single file/folder

		if ( n_posix_false == n_filer_copy_single( f, t, is_merge )  ) { return n_posix_false; }

		return n_posix_true;
	}


	if ( n_filer_copy_callback != NULL )
	{
		n_filer_copy_callback( 0, n_dir_all( &d ) );
	}


/*
n_dir_sort_path( &d );

n_vector_save_literal( &d.path, "path.txt", N_STRING_CRLF );
n_vector_save_literal( &d.name, "name.txt", N_STRING_CRLF );
n_vector_save_literal( &d.low,   "low.txt", N_STRING_CRLF );
n_vector_save_literal( &d.ext,   "ext.txt", N_STRING_CRLF );

n_dir_free( &d ); return n_posix_true;
*/


	// [!] : make top-level folder

	n_filer_copy_single( f, t, is_merge );


	// [!] : for performance

	n_type_int f_length = n_posix_strlen( f );


	n_type_int i = 0;
	n_posix_loop
	{

		if ( i >= n_dir_all( &d ) ) { break; }

		n_posix_char *f_name;
		n_posix_char *t_name;

		n_posix_char *rel = n_dir_path( &d, i );
		rel = &rel[ f_length ];
//n_posix_debug( rel );

		if ( n_string_is_empty( rel ) )
		{

			f_name = n_string_path_make_new( f, n_dir_name( &d, i ) );
			t_name = n_string_path_make_new( t, n_dir_name( &d, i ) );

		} else {

			f_name = n_string_path_new( n_posix_strlen( f ) + 1 + n_posix_strlen( rel ) + 1 + n_posix_strlen( n_dir_name( &d, i ) ) );
			t_name = n_string_path_new( n_posix_strlen( t ) + 1 + n_posix_strlen( rel ) + 1 + n_posix_strlen( n_dir_name( &d, i ) ) );

			n_string_path_make( f, rel, f_name );
			n_string_path_make( t, rel, t_name );

			n_string_path_make( f_name, n_dir_name( &d, i ), f_name );
			n_string_path_make( t_name, n_dir_name( &d, i ), t_name );

		}
//n_posix_debug_literal( "%s\n%s", f_name, t_name );


		n_filer_copy_single( f_name, t_name, is_merge );


		n_string_path_free( f_name );
		n_string_path_free( t_name );


		if ( n_filer_copy_callback != NULL )
		{
			n_filer_copy_callback( i, n_dir_all( &d ) );
		}


		i++;

	}


	if ( n_filer_copy_callback != NULL )
	{
		n_filer_copy_callback( n_dir_all( &d ), n_dir_all( &d ) );
	}


	n_dir_free( &d );


	return n_posix_false;
}


#endif // _H_NONNON_NEUTRAL_FILER

