// Nonnon TAR
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_TAR
#define _H_NONNON_NEUTRAL_TAR




#include "./filer.c"
#include "./string_path.c"
#include "./unicode.c"




#define N_TAR_EXT n_posix_literal( ".tar\0\0" )




// internal
void
n_tar_name_check( n_posix_char *name )
{

	if ( n_string_is_empty( name ) ) { return; }


	const n_posix_char replace = n_posix_literal( '_' );

	if ( name[ 1 ] == N_STRING_CHAR_COLON ) { name[ 1 ] = replace; }

	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_string_path_slash_check( name, i ) ) { name[ i ] = N_STRING_CHAR_SLASH; }

		if (
			( name[ i + 0 ] == N_STRING_CHAR_DOT )
			&&
			( name[ i + 1 ] == N_STRING_CHAR_DOT )
		)
		{
			name[ i + 0 ] = replace;
			name[ i + 1 ] = replace;
		}


		i++;
		if ( name[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return;
}

// internal
n_posix_bool
n_tar_archive_single( const n_posix_char *relname, n_posix_bool is_first, n_posix_bool cleanup )
{

	static n_posix_char *first = NULL;

	if ( cleanup )
	{
		n_string_path_free( first );
		return n_posix_false;
	}


	if ( n_posix_false == n_posix_stat_is_exist( relname ) ) { return n_posix_true; }

	if ( 100 <= n_posix_strlen( relname ) ) { return n_posix_true; }


	FILE *fp_i, *fp_o;

	char       tar[ 512 ];
	n_type_int i, checksum;


	if ( is_first )
	{

		first = n_string_path_ext_mod_new( relname, N_TAR_EXT );
//n_posix_debug_literal( "%s", first );

		fp_o = n_posix_fopen_write ( first );

	} else {

		fp_o = n_posix_fopen_append( first );

	}

	if ( fp_o == NULL ) { return n_posix_true; }



	n_type_int fsize = n_posix_stat_size ( relname );
	time_t     mtime = n_posix_stat_mtime( relname );


	n_memory_zero( tar, 512 );


	{

		n_posix_char *str = n_string_carboncopy( relname ); n_tar_name_check( str );

#ifdef UNICODE
		sprintf( &tar[   0 ], "%ls",  str );
#else
		sprintf( &tar[   0 ], "%s",   str );
#endif

		n_string_free( str );

	}


	sprintf( &tar[ 100 ], "%6d ", 777 );
	sprintf( &tar[ 108 ], "%6d ",   0 );
	sprintf( &tar[ 116 ], "%6d ",   0 );

	sprintf( &tar[ 124 ], "%11lo ", (u32) fsize );
	sprintf( &tar[ 136 ], "%11lo ", (u32) mtime );

#ifdef UNICODE
	sprintf( &tar[ 148 ], "%8lc", N_STRING_CHAR_SPACE );
#else
	sprintf( &tar[ 148 ], "%8c",  N_STRING_CHAR_SPACE );
#endif


	if ( n_posix_stat_is_dir( relname ) )
	{
		sprintf( &tar[ 156 ], "5" );
	} else {
		sprintf( &tar[ 156 ], "0" );
	}


	i = checksum = 0;
	n_posix_loop
	{

		checksum += tar[ i ];

		i++;
		if ( i >= ( 156 + 1 ) ) { break; }
	}
	sprintf( &tar[ 148 ], "%6o ", (unsigned int) checksum );


	n_posix_fwrite( &tar, 512, 1, fp_o );


	// [!] : file only

	if ( fsize != 0 )
	{

		fp_i = n_posix_fopen_read( relname );
		if ( fp_i == NULL ) { n_posix_fclose( fp_o ); return n_posix_true; }

		n_filer_copy_file( fp_i, fp_o, fsize );

		n_posix_fclose( fp_i );

	}


	// Padding : alignment = 512byte

	if ( fsize != 0 )
	{

		n_type_int byte_padding = 512 - ( fsize % 512 );

		u8 *ptr = n_memory_new_closed( byte_padding );
		n_memory_zero( ptr, byte_padding );

		n_posix_fwrite( ptr, byte_padding, 1, fp_o );

		n_memory_free_closed( ptr );

	}


	n_posix_fclose( fp_o );


	return n_posix_false;
}

// internal
n_posix_bool
n_tar_archive_dir( const n_posix_char *reldir )
{

	if ( n_posix_false == n_posix_stat_is_dir( reldir ) ) { return n_posix_true; }
//n_posix_debug( reldir );


	n_posix_DIR *dp = n_posix_opendir_nodot( reldir );
	if ( dp == NULL ) { return n_posix_true; }

	n_posix_bool ret = n_posix_false;

	n_posix_loop
	{

		n_posix_dirent *dirent = n_posix_readdir( dp );
		if ( dirent == NULL ) { break; }

//n_posix_debug( reldir );
		n_posix_char *rel = n_string_path_make_new( reldir, dirent->d_name );
//n_posix_debug( rel );

		ret = n_tar_archive_single( rel, n_posix_false, n_posix_false );
		//if ( ret ) { break; }


		if ( n_posix_stat_is_dir( rel ) )
		{
			ret = n_tar_archive_dir( rel );
			//if ( ret ) { break; }
		}

		n_string_path_free( rel );

	}

	n_posix_closedir( dp );


	return ret;
}

// internal
n_posix_bool
n_tar_archive_simple( const n_posix_char *rel )
{

	n_posix_bool ret;


	if ( n_posix_stat_is_file( rel ) )
	{
//n_posix_debug_literal( "FILE" );

		ret = n_tar_archive_single( rel, n_posix_true, n_posix_false );

	} else {
//n_posix_debug_literal( "DIR" );

		n_posix_chdir( N_STRING_DOTDOT );

		ret  = n_tar_archive_single( rel, n_posix_true, n_posix_false );
		ret |= n_tar_archive_dir( rel );

	}


	n_tar_archive_single( NULL, n_posix_false, n_posix_true );


	return ret;
}

// internal
n_posix_bool
n_tar_dearchive( const n_posix_char *relname, n_type_int *offset )
{

	if ( offset == NULL ) { return n_posix_true; }


	// [!] : not exist and not a file

	if ( n_posix_false == n_posix_stat_is_file( relname ) ) { return n_posix_true; }


	if ( n_posix_false == n_string_path_ext_is_same( N_TAR_EXT, relname ) ) { return n_posix_true; }

	n_type_int fsize = n_posix_stat_size( relname );
	if ( (*offset) >= fsize )
	{

		(*offset) = 0;

		return n_posix_false;
	}


	char tar[ 512 ];

	FILE *fp_i, *fp_o;


	fp_i = n_posix_fopen_read( relname );
	if ( fp_i == NULL ) { return n_posix_true; }


	n_posix_fseek( fp_i, (long) (*offset), SEEK_SET );

	n_posix_fread( tar, 512, 1, fp_i );
	(*offset) += 512;


	// [Mechanism]
	//
	//	. / t e s t \0
	//	0 1 2 3 4 5  6

#ifdef UNICODE

	n_posix_char *newname = n_posix_ansi2unicode( tar );
//MessageBoxA( NULL, tar, "DEBUG", 0 );
//n_posix_debug( s );

#else // #ifdef UNICODE

	n_posix_char *newname = n_string_carboncopy ( tar );

#endif // #ifdef UNICODE


	n_tar_name_check( newname );
//n_posix_debug( newname );


	if ( 0 == atoi( &tar[ 156 ] ) )
	{
//n_posix_debug_literal( "File" );

		u32 byte = 0;
		sscanf( &tar[ 124 ], "%11lo", &byte );


		fp_o = n_posix_fopen_write( newname );
		if ( fp_o == NULL ) { n_posix_fclose( fp_i ); return n_posix_true; }

		n_filer_copy_file( fp_i, fp_o, byte );

		n_posix_fclose( fp_o );


		(*offset) += byte;


		// [!] : alignment : 512byte

		if ( byte != 0 )
		{
			byte = 512 - ( byte % 512 );
		}

		(*offset) += byte;


#ifndef N_POSIX_PLATFORM_WINDOWS

		n_posix_chmod( newname, N_POSIX_PERMISSION_DEFAULT );
		//n_posix_chmod( newname, atol( &tar[ 100 ] ) ); // not checked yet

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS

	} else {
//n_posix_debug_literal( "Dir" );

#ifndef N_POSIX_PLATFORM_WINDOWS

		n_posix_mkdir( newname, N_POSIX_PERMISSION_DEFAULT );
		//n_posix_mkdir( newname, atol( &tar[ 100 ] ) ); // not checked yet

#else // #ifndef N_POSIX_PLATFORM_WINDOWS

		n_posix_mkdir( newname );

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS

	}

	n_posix_fclose( fp_i );


	{

		u32 tm = 0;

		sscanf( &tar[ 136 ], "%11lo", &tm );


		// [!] : spell a magic

		n_posix_sleep( 100 );


		n_posix_utimbuf ut;

		ut.actime = ut.modtime = tm;
		n_posix_utime( newname, &ut );

	}

	n_string_path_free( newname );


	return n_posix_false;
}

// internal
n_posix_bool
n_tar_dearchive_simple( const n_posix_char *relname )
{

	n_posix_bool ret = n_posix_false;


	n_type_int offset = 0;
	n_posix_loop
	{

		ret = n_tar_dearchive( relname, &offset );
		if ( ret ) { break; }

		// [!] : dearchiving is succeeded

//n_posix_debug_literal( "%lu", offset );

		if ( offset == 0 ) { break; }
	}


	return ret;
}

n_posix_bool
n_tar_main( n_posix_char *abspath )
{

	// [!] : archive/dearchive integrated function


	n_posix_char *dir = n_string_path_folder_current_new();


	// [!] : 2 == n_posix_strlen_literal( "./" ) )

	n_posix_char *rel = n_string_path_new( n_posix_strlen( abspath ) + 2 );
	n_string_path_name( abspath, rel );
	n_string_path_make( N_STRING_DOT, rel, rel );
//n_posix_debug( rel );


	n_posix_char *pth = n_string_path_carboncopy( abspath );
	n_string_path_folder_change( pth );


	// [!] : need to check ".tar" or not many times

	n_posix_bool ret;

	if ( n_string_path_ext_is_same( N_TAR_EXT, rel ) )
	{
//n_posix_debug( rel );

		ret = n_tar_dearchive_simple( rel );

	} else {

		ret = n_tar_archive_simple( rel );

		if ( ret == n_posix_false )
		{
			n_string_path_ext_mod( N_TAR_EXT, abspath );
		}

	}


	n_string_path_folder_change( dir );


	n_string_path_free( dir );
	n_string_path_free( rel );
	n_string_path_free( pth );


//n_posix_debug_literal( "%d", ret );


	return ret;
}


#endif // _H_NONNON_NEUTRAL_TAR

