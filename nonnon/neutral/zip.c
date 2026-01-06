// Nonnon Zip
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : Mac Version
//
//	file names of alias will be conflict
//	+ test and test.zip then test.zip is not extracted


// [Mechanism] : when using DLL : currently not needed
//
//	http://gnuwin32.sourceforge.net/packages/zlib.htm
//
//	[ How to Get ]
//
//	1 : click "Binaries"        to get "zlib-1.2.3-bin.zip"
//	2 : click "Developer files" to get "zlib123-dll.zip"
//	3 : done!
//
//	[ How to Install ]
//
//	1 : merge "include" and "lib" folders into "MinGW/include" "MinGW/lib" folders
//	2 : put "bin/zlib1.dll" into a "MinGW/bin" folder
//	3 : done!
//
//	[ How to include ]
//
//	#include <zlib.h>
//
//	[ How to Link ]
//
//	-lzdll




#ifndef _H_NONNON_NEUTRAL_ZIP
#define _H_NONNON_NEUTRAL_ZIP




#ifdef N_POSIX_PLATFORM_WINDOWS

//#include <zlib.h>
#include "../floss/zlib.c"

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

#include <zlib.h>

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#include "./string_path.c"
#include "./vector.c"




#define N_ZIP_EXT          n_posix_literal( ".zip\0\0" )

#define N_ZIP_BYTE_MINIMAL ( 100 )

#define N_ZIP_PK           0x4b50
#define N_ZIP_ITEM         0x0403
#define N_ZIP_TOC_ITEM     0x0201
#define N_ZIP_TOC          0x0605
//#define N_ZIP_VFAT       0x0014


#pragma pack( push, 2 )

typedef struct {

	// [Local file header]

	u16 pk;
	u16 type;

	u16 version;
	u16 mode;
	u16 method;
	u16 dos_mtime;
	u16 dos_mdate;
	u32 crc;
	u32 byte;
	u32 byte_original;
	u16 byte_name;
	u16 byte_extra;

} n_zip_item;

typedef struct {

	// [Central directory structure]

	u16 pk;
	u16 type;

	u16 filesystem;
	u16 version;
	u16 mode;
	u16 method;
	u16 dos_mtime;
	u16 dos_mdate;
	u32 crc;
	u32 byte;
	u32 byte_original;
	u16 byte_name;
	u16 byte_extra;
	u16 byte_comment;
	u16 disk;
	u16 attrib_internal;
	u32 attrib_external;
	u32 offset;

} n_zip_toc_item;

typedef struct {

	// [End of central directory record]

	u16 pk;
	u16 type;

	u16 disk_count;
	u16 disk_offset;
	u16 item_total_count;
	u16 item_count;
	u32 item_byte;
	u32 item_offset;
	u16 byte_comment;

} n_zip_toc;

#pragma pack( pop )




// internal
void
n_zip_mkdir( const n_posix_char *folder )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_mkdir( folder );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_mkdir( folder, N_POSIX_PERMISSION_DEFAULT );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

// internal
n_posix_bool
n_zip_encode_file( const n_posix_char *name, n_type_int name_cch, n_vector *v_item, n_vector *v_name )
{

	n_posix_structstat st;

	if ( 0 != n_posix_stat( name, &st ) ) { return n_posix_true; }


	n_zip_toc_item *item = n_memory_new( sizeof( n_zip_toc_item ) );
	n_memory_zero( item, sizeof( n_zip_toc_item ) );


	item->pk         = N_ZIP_PK;
	item->type       = N_ZIP_TOC_ITEM;
	//item->filesystem = N_ZIP_VFAT;
	item->version    = 10;

	// [x] : Mac Version : folders must be zero byte

	if ( S_ISDIR( st.st_mode ) )
	{
		item->byte = item->byte_original = 0;
	} else {
		item->byte = item->byte_original = (u32) n_posix_stat_size( name );
	}


	// [!] : 1 == n_posix_strlen( N_STRING_SLASH )

	n_type_int    cch = name_cch;
	n_posix_char *rel = n_string_path_new( cch + 1 );
	n_string_copy( name, rel );
	n_string_replace( rel, rel, N_STRING_BSLASH, N_STRING_SLASH );

	if ( S_ISDIR( st.st_mode ) )
	{
		cch += n_posix_sprintf_literal( &rel[ cch ], "%s", N_STRING_SLASH );
	}

	item->byte_name = (u16) cch;


#ifdef N_POSIX_PLATFORM_WINDOWS


	// [!] : CreateFile() : fail when folders

	{

		WIN32_FIND_DATA fd;
		FindClose( FindFirstFile( name, &fd ) );

		FILETIME write = fd.ftLastWriteTime;

		FileTimeToLocalFileTime( &write, &write );
		FileTimeToDosDateTime( &write, &item->dos_mdate, &item->dos_mtime );

		item->attrib_external = fd.dwFileAttributes;

	}


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_char str[ 100 ];

#ifdef _WIN64
	n_posix_sprintf_literal( str, "%llu", (u64) item );
#else  // #ifdef _WIN64
#ifdef N_POSIX_PLATFORM_WINDOWS
	n_posix_sprintf_literal( str, "%lu",  (u32) item );
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
	n_posix_sprintf_literal( str, "%llu", (u64) item );
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
//n_posix_debug_literal( "%s\n%lu", str, (u32) item );
#endif // #ifdef _WIN64

	n_vector_set( v_item, v_item->sy, str );
	n_vector_set( v_name, v_name->sy, rel );


	n_string_path_free( rel );


	return n_posix_false;
}

// internal
n_posix_bool
n_zip_encode_dir( const n_posix_char *name, n_type_int name_cch, n_vector *v_item, n_vector *v_name )
{
//return n_posix_false;

	if ( n_posix_false == n_posix_stat_is_dir( name ) ) { return n_posix_true; }
//n_posix_debug( reldir );


	n_posix_DIR *dp = n_posix_opendir_nodot( name );
	if ( dp == NULL ) { return n_posix_true; }


	n_posix_loop
	{

		n_posix_dirent *dirent = n_posix_readdir( dp );
		if ( dirent == NULL ) { break; }


		// [!] : 1 == n_posix_strlen( N_STRING_SLASH )

		n_type_int    cch = name_cch + 1 + n_posix_strlen( dirent->d_name );
		n_posix_char *str = n_string_path_new( cch );

		n_string_path_make( name, dirent->d_name, str );
//n_posix_debug_literal( "%s", str );
//NSLog( @"%s", str );

		n_zip_encode_file( str, cch, v_item, v_name );

#ifdef N_POSIX_PLATFORM_WINDOWS
		if ( FILE_ATTRIBUTE_DIRECTORY & dirent->dwFileAttributes )
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
		if ( n_posix_stat_is_dir( str ) )
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
		{
			n_zip_encode_dir( str, cch, v_item, v_name );
		}


		n_string_path_free( str );

	}

	n_posix_closedir( dp );


	return n_posix_false;
}

n_posix_bool
n_zip_encode( const n_posix_char *name )
{

	if ( n_posix_false == n_posix_stat_is_exist( name ) ) { return n_posix_true; }


	n_posix_char *dir = n_string_path_folder_current_new();
	n_posix_char *rel = n_string_path_name_new( name );
	n_posix_char *zip = n_string_path_tmpname_new( N_ZIP_EXT );
	n_posix_char *nam = n_string_path_carboncopy( name );


	n_string_path_folder_change( nam );
	if ( n_posix_stat_is_dir( nam ) ) { n_posix_chdir( N_STRING_DOTDOT ); }


	// [x] : Buggy : WinVista or later : a file while encoding can be unlocked by Explorer

	FILE *fp = n_posix_fopen_write( zip );
	if ( fp == NULL )
	{

		n_string_path_folder_change( dir );

		n_string_path_free( dir );
		n_string_path_free( rel );
		n_string_path_free( zip );
		n_string_path_free( nam );

		return n_posix_true;
	}


	n_vector v_item; n_vector_zero( &v_item ); n_vector_new( &v_item );
	n_vector v_name; n_vector_zero( &v_name ); n_vector_new( &v_name );


	{
		n_type_int cch = n_posix_strlen( rel );

		n_zip_encode_file( rel, cch, &v_item, &v_name );
		n_zip_encode_dir ( rel, cch, &v_item, &v_name );
	}


	// [!] : empty entry will be made if folder structure is changed
	//
	//	target file/folder is locked/removed


	n_type_int i;
	n_type_int byte;

	i = byte = 0;
	n_posix_loop
	{

//n_posix_debug( v_name.line[ i ] );

#ifdef UNICODE

		char *filename = n_posix_unicode2ansi( v_name.line[ i ] );

#else  // #ifdef UNICODE

		char *filename = n_string_carboncopy ( v_name.line[ i ] );

#endif // #ifdef UNICODE

//MessageBoxA( NULL, filename, "DEBUG", 0 );


#ifdef _WIN64
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#else  // #ifdef _WIN64
#ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_posix_atoi( v_item.line[ i ] );
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
#endif // #ifdef _WIN64


		toc_item->offset = (u32) byte;


		n_zip_item item;

		item.pk            = N_ZIP_PK;
		item.type          = N_ZIP_ITEM;
		item.version       = toc_item->version;
		item.mode          = toc_item->mode;
		item.method        = toc_item->method;
		item.dos_mtime     = toc_item->dos_mtime;
		item.dos_mdate     = toc_item->dos_mdate;
		item.crc           = toc_item->crc;
		item.byte          = toc_item->byte;
		item.byte_original = toc_item->byte_original;
		item.byte_name     = toc_item->byte_name = (u16) strlen( filename );
		item.byte_extra    = toc_item->byte_extra;


		// [!] : "ptr" will have non-NULL value when "item.byte" is zero

		u8 *ptr = NULL;

		n_posix_bool is_file = n_posix_stat_is_file( v_name.line[ i ] );
		if ( ( is_file )&&( item.byte == 0 ) )
		{

			// [Needed] : Windows ZIP Folder needs this

		} else
		if ( is_file )
		{

			// [!] : don't use compress() : need to remove zlib header

			u32  fbyte = item.byte;
			u32  tbyte = item.byte * 2;
			u8  *fdata = n_memory_new_closed( fbyte );
			u8  *tdata = n_memory_new_closed( tbyte );


			// [!] : zero-clear is needed

			FILE *fp_i = n_posix_fopen_read( v_name.line[ i ] );
			n_posix_fread( fdata, fbyte, 1, fp_i );


			// [!] : use original data

			item.crc = toc_item->crc = n_posix_crc32( fdata, fbyte );

//n_posix_debug_literal( "%d : %x", item.byte, item.crc );

			n_memory_zero( tdata, tbyte );


			// [!] : no significant change

			z_stream z;
			n_memory_zero( &z, sizeof( z_stream ) );
			deflateInit2( &z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY );
			//deflateInit2( &z, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY );


			z.total_in  = 0;
			z.total_out = 0;
			z.avail_in  = fbyte;
			z.avail_out = tbyte;
			z.next_in   = fdata;
			z.next_out  = tdata;

			deflate( &z, Z_FULL_FLUSH );
			deflate( &z, Z_FINISH );


			item.byte = toc_item->byte = (u32) z.total_out;


			deflateEnd( &z );


			item.method = toc_item->method = 8;


			n_posix_fclose( fp_i );


			n_memory_free_closed( fdata );
			ptr = tdata;

		}


		n_posix_fwrite( &item, sizeof( n_zip_item ), 1, fp );
		byte += sizeof( n_zip_item );


		n_posix_fwrite( filename, item.byte_name, 1, fp );
		byte += toc_item->byte_name;


		n_posix_fwrite( ptr, item.byte, 1, fp );
		byte += item.byte;


		n_memory_free_closed( ptr );


		n_memory_free( filename );


		i++;
		if ( i >= v_item.sy ) { break; }
	}


	n_zip_toc toc = { N_ZIP_PK, N_ZIP_TOC, 0,0,(u16) v_name.sy, (u16) v_name.sy,0, (u32) byte,0 };


	i = 0;
	n_posix_loop
	{

#ifdef _WIN64
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#else  // #ifdef _WIN64
#ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_posix_atoi( v_item.line[ i ] );
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
#endif // #ifdef _WIN64


		n_posix_fwrite( toc_item, sizeof( n_zip_toc_item ), 1, fp );


#ifdef UNICODE

//n_posix_debug_literal( "%s", v_name.line[ i ] );
		char *s = n_posix_unicode2ansi( v_name.line[ i ] );
//MessageBoxA( NULL, s, "DEBUG", 0 );
		n_posix_fwrite( s, toc_item->byte_name, 1, fp );
		n_memory_free( s );

#else // #ifdef UNICODE

		n_posix_fwrite( v_name.line[ i ], toc_item->byte_name, 1, fp );

#endif // #ifdef UNICODE


		toc.item_byte += sizeof( n_zip_toc_item ) + toc_item->byte_name;


		i++;
		if ( i >= v_item.sy ) { break; }
	}


	n_posix_fwrite( &toc, sizeof( n_zip_toc ), 1, fp );



	i = 0;
	n_posix_loop
	{

#ifdef _WIN64
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#else  // #ifdef _WIN64
#ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_posix_atoi( v_item.line[ i ] );
#else  // #ifdef N_POSIX_PLATFORM_WINDOWS
		n_zip_toc_item *toc_item = (void*) n_string_str2u64( v_item.line[ i ] );
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
#endif // #ifdef _WIN64


		n_memory_free( toc_item );


		i++;
		if ( i >= v_item.sy ) { break; }
	}

	n_vector_free( &v_item );
	n_vector_free( &v_name );



	n_posix_fclose( fp );


	{

		n_posix_char *newname = n_string_path_ext_mod_new( rel, N_ZIP_EXT );

		n_posix_unlink( newname );
		n_string_path_rename( zip, newname );

		n_string_path_free( newname );

	}


	n_string_path_folder_change( dir );


	n_string_path_free( dir );
	n_string_path_free( rel );
	n_string_path_free( zip );
	n_string_path_free( nam );


	return n_posix_false;
}

void
n_zip_autofolder( const n_posix_char *relname )
{

	// [x] : Windows ZIP Folder hasn't folder entry as item


	n_type_int i = 0;
	n_posix_loop
	{

		if ( relname[ i ] == N_STRING_CHAR_NUL )
		{

			break;

		}
		if ( relname[ i ] == N_STRING_CHAR_SLASH )
		{

			n_posix_char *str = n_string_carboncopy( relname );

			n_string_terminate( str, i );
//NSLog( @"%s", str );
			if ( n_posix_false == n_posix_stat_is_exist( str ) )
			{
				n_zip_mkdir( str );
			}

			n_string_free( str );

		}
		
		i++;

	}


	return;
}

n_posix_bool
n_zip_decode( const n_posix_char *name )
{

	n_type_int byte = n_posix_stat_size( name );
	if ( byte < N_ZIP_BYTE_MINIMAL ) { return n_posix_true; }
	if ( byte > LONG_MAX ) { return n_posix_true; }


	FILE *fp = n_posix_fopen_read( name );
	if ( fp == NULL ) { return n_posix_true; }


	// Phase 1 : is ZIP
//NSLog( @"Phase 1" );

	u16 pk = 0;
	n_posix_fread( &pk, sizeof( u16 ), 1, fp );

	if ( pk != N_ZIP_PK ) { n_posix_fclose( fp ); return n_posix_true; }


	// Phase 2 : search main table-of-contents
//NSLog( @"Phase 2" );

	n_zip_toc      toc;
	n_zip_toc_item toc_item;
	n_zip_item     item;

	u32 i = (u32) ( byte - sizeof( n_zip_toc ) );
	n_posix_loop
	{

		n_posix_fseek( fp, i, SEEK_SET );
		n_posix_fread( &toc, sizeof( n_zip_toc ), 1, fp );

//n_posix_debug_literal( "%04x : %04x", toc.pk, toc.type );
		if (
			( toc.pk   == N_ZIP_PK  )
			&&
			( toc.type == N_ZIP_TOC )
		)
		{
			break;
		}

		if ( i == 0 ) { break; }
		i--;

	}


//n_posix_debug_literal( "%d %d", ( toc.item_offset + toc.item_byte ), i );

	if (
		( toc.disk_count != 0 )
		||
		( toc.disk_offset != 0 )
		||
		( toc.item_total_count == 0 )
		||
		( toc.item_count == 0 )
		||
		( toc.item_total_count != toc.item_count )
		||
		( toc.item_byte == 0 )
		||
		( ( toc.item_offset + toc.item_byte ) > i )
	)
	{
		n_posix_fclose( fp );

		return n_posix_true;
	}


	// Phase 3 : get an item
//NSLog( @"Phase 3" );

	// [!] : 2 == n_posix_strlen_literal( "./" )
	// [!] : 1 == n_posix_strlen( N_STRING_SLASH )

	const n_type_int name_cch = n_posix_strlen( name ) + 2 + 1;


	n_posix_bool ret = n_posix_false;

	u32 offset_toc = 0;


	n_posix_loop
	{

		n_posix_fseek( fp, (n_type_int) toc.item_offset + offset_toc, SEEK_SET );
		n_posix_fread( &toc_item, sizeof( n_zip_toc_item ), 1, fp );

		if (
			( toc_item.pk != N_ZIP_PK  )
			||
			( toc_item.type != N_ZIP_TOC_ITEM )
			||
			( ( toc_item.offset + sizeof( n_zip_item ) ) > toc.item_offset )
		)
		{

			ret = n_posix_true;

			break;
		}


		n_posix_fseek( fp, toc_item.offset, SEEK_SET );
		n_posix_fread( &item, sizeof( n_zip_item ), 1, fp );

		// [x] : buggy encoders
		//
		//	item.method = 0, item.mode = 2 is possible

//n_posix_debug_literal( "Method %d\nMode %d", item.method, item.mode );

		if (
			( item.pk != N_ZIP_PK  )
			||
			( item.type != N_ZIP_ITEM )
			||
			( ( item.method != 0 )&&( item.method != 8 ) )
			||
			(
				( item.method == 0 )
				&&
				(
					( item.byte != item.byte_original )
					//||
					//( item.mode != 0 )
				)
			)
			||
			(
				( item.method == 8 )
				&&
				( ( item.mode & (~0x0006) ) != 0 )
			)
			||
			( item.byte_name == 0 )
		)
		{

			ret = n_posix_true;

			break;
		}


		n_posix_char *filename  = NULL;
		n_posix_bool  is_folder = n_posix_false;


		{ // file name maker


		// Not Supported : some variants use UTF-8 for a file name


		char *s = n_memory_new_closed( ( 2 + item.byte_name ) + 1 );

		s[ 0 ] = '.';
		s[ 1 ] = '/';

		n_posix_fseek( fp, toc_item.offset + sizeof( n_zip_item ), SEEK_SET );
		n_posix_fread( &s[ 2 ], item.byte_name, 1, fp );

		if ( s[ ( 2 + item.byte_name - 1 ) ] == '/' ) { is_folder = n_posix_true; }

		s[ ( 2 + item.byte_name ) ] = '\0';

//n_posix_debug_literal( "%d", item.byte_name );
//MessageBoxA( NULL, s, "DEBUG", 0 );

#ifdef UNICODE

		filename = n_posix_ansi2unicode( s );

		n_memory_free_closed( s );

#else // #ifndef UNICODE

		filename = s;

#endif // #ifndef UNICODE


		} // file name maker


		// [!] : sanitize for security : phase 1
		//
		//	"./../../../" => "./__/../__/" => "./__/__/__/"

		if ( n_string_search_simple_literal( filename, ".." ) )
		{
			n_string_replace( filename, filename, n_posix_literal( "/../" ) , n_posix_literal( "/__/" ) );
			n_string_replace( filename, filename, n_posix_literal( "/../" ) , n_posix_literal( "/__/" ) );
		}

		// [!] : sanitize for security : phase 2
		//
		//	"./folder/<?>/" => "./folder/___/"

		n_string_safepath( filename, filename );

//n_posix_debug_literal( "%s", filename );


		// [Mechanism] : the first item
		//
		//	Nonnon ZIP         : "./filename/"
		//	Windows ZIP Folder : "./filename/file.txt"
		//	Other Possibility  : "./file.txt"
		//	Other Possibility  : "./bin/"

		if (
			( toc.item_total_count > 1 )
			&&
			( offset_toc == 0 )
		)
		{

			n_posix_char *str = n_string_path_carboncopy( name ); n_string_path_ext_del( str );
			n_posix_char *f   = n_string_path_new( name_cch );

			f[ 0 ] = '.';
			f[ 1 ] = '/';
			n_string_path_name( str, &f[ 2 ] );

			n_string_path_drivename_slash_add( f );

//n_posix_debug_literal( "%s\n%s", f, filename );

			if ( n_posix_false == n_string_match( filename, f ) )
			{
				n_zip_mkdir( str );
				n_string_path_folder_change( str );
			}

			n_string_path_free( str );
			n_string_path_free( f   );

		}


		u32 data_offset = toc_item.offset + sizeof( n_zip_item ) + item.byte_name + item.byte_extra;

		n_type_int compare_f = (n_type_int) ( data_offset + item.byte );
		n_type_int compare_t = (n_type_int) ( byte - sizeof( n_zip_toc ) );

		if ( compare_f > compare_t ) { ret = n_posix_true; break; }

		if ( item.method == 0 )
		{

			if ( is_folder )
			{

				n_zip_mkdir( filename );

			} else {

				u8 *ptr = n_memory_new_closed( item.byte );


				n_posix_fseek( fp, data_offset, SEEK_SET );
				n_posix_fread( ptr, item.byte, 1, fp );


				FILE *fp_o = n_posix_fopen_write( filename );
				if ( fp_o == NULL )
				{
					n_zip_autofolder( filename );
					fp_o = n_posix_fopen_write( filename );
				}
				if ( fp_o != NULL )
				{
					n_posix_fwrite( ptr, item.byte, 1, fp_o );
				}
				n_posix_fclose( fp_o );


				n_memory_free_closed( ptr );

			}

		} else
		if ( item.method == 8 )
		{

			// [!] : don't use uncompress() : need to remove zlib header

			u32  fbyte = item.byte;
			u32  tbyte = item.byte_original;
			u8  *fdata = n_memory_new_closed( fbyte );
			u8  *tdata = n_memory_new_closed( tbyte );


			n_posix_fseek( fp, data_offset, SEEK_SET );
			n_posix_fread( fdata, fbyte, 1, fp );


			z_stream z;
			n_memory_zero( &z, sizeof( z_stream ) );
			inflateInit2( &z, -MAX_WBITS );


			z.total_in  = 0;
			z.total_out = 0;
			z.avail_in  = fbyte;
			z.avail_out = tbyte;
			z.next_in   = fdata;
			z.next_out  = tdata;

			inflate( &z, Z_FULL_FLUSH );


			z.total_in = z.total_out = 0;
			z.avail_in = z.avail_out = 0;
			z.next_in  = z.next_out  = NULL;

			inflate( &z, Z_FINISH );


			inflateEnd( &z );


			FILE *fp_o = n_posix_fopen_write( filename );
			if ( fp_o == NULL )
			{
				n_zip_autofolder( filename );
				fp_o = n_posix_fopen_write( filename );
			}
			if ( fp_o != NULL )
			{
				n_posix_fwrite( tdata, tbyte, 1, fp_o );
			}
			n_posix_fclose( fp_o );


			n_memory_free_closed( fdata );
			n_memory_free_closed( tdata );

		}


#ifdef N_POSIX_PLATFORM_WINDOWS


		if ( is_folder == n_posix_false )
		{

			n_type_int i = 0;
			n_posix_loop
			{

				HANDLE h = CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
				if ( h != INVALID_HANDLE_VALUE )
				{

					FILETIME write;
					DosDateTimeToFileTime( toc_item.dos_mdate, toc_item.dos_mtime, &write );
					LocalFileTimeToFileTime( &write, &write );

					SetFileTime( h, NULL, NULL, &write );

					CloseHandle( h );

					break;

				}


				// [!] : spell a magic

				n_posix_sleep( 10 );

				i++;
				if ( i >= 100 ) { break; }
			}

		}


		SetFileAttributes( filename, toc_item.attrib_external );


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


		n_memory_free( filename );


		offset_toc += sizeof( n_zip_toc_item ) + toc_item.byte_name + toc_item.byte_extra + toc_item.byte_comment;
		if ( offset_toc >= toc.item_byte ) { break; }

	} // n_posix_loop


	n_posix_fclose( fp );


	return ret;
}

n_posix_bool
n_zip_main( const n_posix_char *abspath )
{

	n_posix_bool ret = n_posix_false;


	n_posix_char *dir = n_string_path_folder_current_new();
	n_posix_char *pth = n_string_path_carboncopy( abspath );


	n_string_path_folder_change( pth );


	if ( n_string_path_ext_is_same( N_ZIP_EXT, pth ) )
	{
//NSLog( @"Decode" );

		ret = n_zip_decode( pth );

//NSLog( @"%d", ret );

	} else {
//NSLog( @"Encode" );

//NSLog(                @"%ld", sizeof( n_zip_item ) ); // Mac     : 30 byte
//n_posix_debug_literal( "%ld", sizeof( n_zip_item ) ); // Windows : 30 byte

//NSLog(                @"%ld", sizeof( n_zip_toc_item ) ); // Mac     : 46 byte
//n_posix_debug_literal( "%ld", sizeof( n_zip_toc_item ) ); // Windows : 46 byte

//NSLog(                @"%ld", sizeof( n_zip_toc ) ); // Mac     : 22 byte
//n_posix_debug_literal( "%ld", sizeof( n_zip_toc ) ); // Windows : -- byte

		ret = n_zip_encode( pth );

	}


	n_string_path_folder_change( dir );


	n_string_path_free( dir );
	n_string_path_free( pth );


	return ret;
}


#endif // _H_NONNON_NEUTRAL_ZIP

