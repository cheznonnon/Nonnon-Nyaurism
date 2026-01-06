// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_TXT
#define _H_NONNON_NEUTRAL_TXT




#include "./type.c"
#include "./vector.c"




// inherit

#define n_txt_error(     t       ) n_vector_error(     (void*) t       )
#define n_txt_debug(     t       ) n_vector_debug(     (void*) t       )
#define n_txt_add(       t, i, s ) n_vector_add(       (void*) t, i, s )
#define n_txt_mod(       t, i, s ) n_vector_mod(       (void*) t, i, s )
#define n_txt_del(       t, i    ) n_vector_del(       (void*) t, i    )
#define n_txt_get(       t, i    ) n_vector_get(       (void*) t, i    )
#define n_txt_set(       t, i, s ) n_vector_set(       (void*) t, i, s )
#define n_txt_is_empty(  t       ) n_vector_is_empty(  (void*) t       )
#define n_txt_swap(      t, a, b ) n_vector_swap(      (void*) t, a, b )
#define n_txt_sort(      t, a    ) n_vector_sort(      (void*) t, a    )
#define n_txt_sort_up(   t       ) n_vector_sort_up(   (void*) t       )
#define n_txt_sort_down( t       ) n_vector_sort_down( (void*) t       )

#define n_txt_add_literal( t,i,s ) n_vector_add_literal( (void*) t, i, s )
#define n_txt_mod_literal( t,i,s ) n_vector_mod_literal( (void*) t, i, s )
#define n_txt_set_literal( t,i,s ) n_vector_set_literal( (void*) t, i, s )

#define n_txt_add_fast(  t, i, s ) n_vector_add_fast(   (void*) t, i, s )
#define n_txt_mod_fast(  t, i, s ) n_vector_mod_fast(   (void*) t, i, s )




#define N_TXT_NEWLINE_BINARY 0
#define N_TXT_NEWLINE_CR     1
#define N_TXT_NEWLINE_LF     2
#define N_TXT_NEWLINE_CRLF   3

#define N_TXT_UNICODE_NIL    0
#define N_TXT_UNICODE_LIL    1
#define N_TXT_UNICODE_BIG    2
#define N_TXT_UNICODE_UTF    3

#define N_TXT_READONLY_OFF   0
#define N_TXT_READONLY_ON    1


typedef struct {

	// override n_vector

	void       **line;
	n_type_int   sy;

	void        *stream;
	n_type_int   sx;
	n_type_int   byte;


	// [!] : these values mean properties of an original file 

	// N_TXT_NEWLINE_*
	//	BINARY
	//	CR     : \r   : 0x0D   : Mac(old)
	//	LF     : \n   : 0x0A   : Unix, Mac(new)
	//	CRLF   : \r\n : 0x0D0A : Windows : default

	// N_TXT_UNICODE_*
	//	NIL
	//	LIL    : UCS-2 Little Endian : Windows
	//	BIG    : UCS-2 Big    Endian : Mac(old)
	//	UTF    : UTF-8               : Unix, Mac(new)

	int          newline;
	int          unicode;
	n_posix_bool readonly;

} n_txt;




#define n_txt_zero(  f    ) n_memory_zero( f,    sizeof( n_txt ) )
#define n_txt_alias( f, t ) n_memory_copy( f, t, sizeof( n_txt ) )

void
n_txt_stream( n_txt *txt )
{

	n_posix_char *newline = NULL;


	if ( txt->newline == N_TXT_NEWLINE_CR   ) { newline = N_STRING_CR;   } else
	if ( txt->newline == N_TXT_NEWLINE_LF   ) { newline = N_STRING_LF;   } else
	if ( txt->newline == N_TXT_NEWLINE_CRLF ) { newline = N_STRING_CRLF; }


	n_vector_stream( (n_vector*) txt, newline );


	return;
}

void
n_txt_free( n_txt *txt )
{

	if ( n_txt_error( txt ) ) { return; }


	n_vector_free( (n_vector*) txt );

	n_txt_zero( txt );


	return;
}

void
n_txt_new( n_txt *txt )
{

	if ( txt == NULL ) { return; }


	n_txt_free( txt );

	n_vector_new( (n_vector*) txt );

	txt->newline  = N_TXT_NEWLINE_CRLF;
	txt->readonly = N_TXT_READONLY_OFF;


#ifdef UNICODE

	txt->unicode  = N_TXT_UNICODE_LIL;

#else //#ifdef UNICODE

	txt->unicode  = N_TXT_UNICODE_NIL;

#endif // #ifdef UNICODE


	return;
}

// internal
int
n_txt_newline_check( const n_posix_char *stream, n_type_int byte )
{

	int newline = N_TXT_NEWLINE_CRLF;


	if ( stream == NULL ) { return newline; }
	if ( byte   ==    0 ) { return newline; }


	n_type_int cch = byte / sizeof( n_posix_char );


	n_type_int i = 0;
	n_posix_loop
	{

		if ( stream[ i ] == N_STRING_CHAR_NUL )
		{

			// [!] : some files have NUL at end of file

			if ( i < cch )
			{

				newline = N_TXT_NEWLINE_BINARY;

				break;

			}

		} else
		if ( stream[ i ] == N_STRING_CHAR_CR )
		{

			i++;
			if ( stream[ i ] == N_STRING_CHAR_LF )
			{
				newline = N_TXT_NEWLINE_CRLF;
			} else {
				newline = N_TXT_NEWLINE_CR;
			}

			break;

		} else
		if ( stream[ i ] == N_STRING_CHAR_LF )
		{

			newline = N_TXT_NEWLINE_LF;

			break;

		}


		i++;
		if ( i >= cch ) { break; }
	}


	return newline;
}

#define n_txt_load(          v, fname        ) n_txt_load_internal( v, (void*) fname,    0, n_posix_true  )
#define n_txt_load_onmemory( v, stream, byte ) n_txt_load_internal( v,        stream, byte, n_posix_false )

// internal
n_posix_bool
n_txt_load_internal( n_txt *txt, void *stream, n_type_int byte, n_posix_bool is_file )
{

	// [!] : load_onmemory() : don't free "stream" after calling this function


	if ( txt == NULL ) { return n_posix_true; }


	n_type_int bom_offset = 0;


	txt->unicode = N_TXT_UNICODE_NIL;

	if ( is_file )
	{

		FILE *fp = n_posix_fopen_read( stream );
		if ( fp == NULL ) { return n_posix_true; }

		byte = n_posix_stat_size( stream );


		// [!] : remember original spec

		u8 sniffer[ 3 ]; n_posix_fread( sniffer, 3, 1, fp );

		if ( ( byte >= 3 )&&( n_unicode_bom_is_utf8    ( sniffer, 3 ) ) )
		{
			txt->unicode = N_TXT_UNICODE_UTF;
			n_posix_fseek( fp, 3, SEEK_SET );
			byte -= 3;
		} else
		if ( ( byte >= 2 )&&( n_unicode_bom_is_utf16_le( sniffer, 2 ) ) )
		{
			txt->unicode = N_TXT_UNICODE_LIL;
			n_posix_fseek( fp, 2, SEEK_SET );
			byte -= 2;
		} else
		if ( ( byte >= 2 )&&( n_unicode_bom_is_utf16_be( sniffer, 2 ) ) )
		{
			txt->unicode = N_TXT_UNICODE_BIG;
			n_posix_fseek( fp, 2, SEEK_SET );
			byte -= 2;
		} else {
			n_posix_fseek( fp, 0, SEEK_SET );
		}


		stream = n_memory_new( byte );
		n_posix_fread( stream, byte, 1, fp );

		n_posix_fclose( fp );

	} else {

		// [!] : remember original spec

		if ( n_unicode_bom_is_utf8   ( stream, byte ) )
		{
//n_posix_debug_literal( "UTF" );

			txt->unicode = N_TXT_UNICODE_UTF;

			bom_offset = 3;

		} else
		if ( n_unicode_bom_is_utf16_le( stream, byte ) )
		{
//n_posix_debug_literal( "LIL" );

			txt->unicode = N_TXT_UNICODE_LIL;

			bom_offset = 2;

		} else
		if ( n_unicode_bom_is_utf16_be( stream, byte ) )
		{
//n_posix_debug_literal( "BIG" );

			txt->unicode = N_TXT_UNICODE_BIG;

			bom_offset = 2;

		}

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


	// Decoder

	char         *ptr;
	n_posix_bool  lost = n_posix_false;

#ifdef UNICODE


	if ( txt->unicode == N_TXT_UNICODE_NIL )
	{

		// [!] : ANSI only : don't use n_txt_newline_check()

		n_posix_bool binary = n_posix_false;

		u8 *ptr_u8 = (u8*) stream;

		n_type_int i = 0;
		n_posix_loop
		{

			if ( ptr_u8[ i ] == 0 ) { binary = n_posix_true; break; }

			// [!] : some files have NUL at end of file

			i++;
			if ( i >= ( byte - 1 ) ) { break; }
		}


		if ( binary == n_posix_false )
		{

			ptr = n_unicode_alloccopy( &byte, &ptr_u8[ bom_offset ] );
//MessageBoxA( NULL, (char*) ptr, "DEBUG", 0 );


			// [!] : n_posix_false is returned always

			//lost = n_unicode_codec_char2wchar( ptr, byte );
			//n_unicode_bom_remove( ptr, byte );

			lost = n_unicode_codec_char2wchar_no_bom( ptr, byte );
//MessageBoxW( NULL, (wchar_t*) ptr, L"DEBUG", 0 );


			n_memory_free( stream );


			byte   = n_posix_strlen( (void*) ptr ) * sizeof( wchar_t );
			stream = ptr;

		}

	} else {

		u8 *ptr_u8 = (u8*) stream;

		ptr = n_unicode_alloccopy( &byte, &ptr_u8[ bom_offset ] );


		if ( txt->unicode == N_TXT_UNICODE_BIG )
		{

			n_unicode_endianness( ptr, byte );

		} else
		if ( txt->unicode == N_TXT_UNICODE_UTF )
		{

			n_unicode_utf8_decode_no_bom( ptr, byte );

		}


		n_memory_free( stream );

		//n_unicode_bom_remove( ptr, byte );

		byte   = n_posix_strlen( (void*) ptr ) * sizeof( n_posix_char );
		stream = ptr;

	}


#else // #ifdef UNICODE


	if ( txt->unicode != N_TXT_UNICODE_NIL )
	{

		u8 *ptr_u8 = (u8*) stream;

		ptr = n_unicode_alloccopy( &byte, &ptr_u8[ bom_offset ] );


		if ( txt->unicode == N_TXT_UNICODE_BIG )
		{
			n_unicode_endianness( ptr, byte );
		} else
		if ( txt->unicode == N_TXT_UNICODE_UTF )
		{
			n_unicode_utf8_decode_no_bom( ptr, byte );
		}


//MessageBoxW( NULL, (wchar_t*) ptr, L"DEBUG", 0 );

		lost = n_unicode_codec_wchar2char_no_bom( ptr, byte );

//MessageBoxA( NULL, ptr, "DEBUG", 0 );


		n_memory_free( stream );

		byte   = strlen( ptr );
		stream = ptr;

	}


#endif // #ifdef UNICODE


	txt->newline = n_txt_newline_check( stream, byte );

//n_posix_debug( stream );

	n_vector_load_onmemory( (n_vector*) txt, stream, byte );

//n_posix_debug( txt->stream );


	if (
		( lost )
		||
		( txt->newline == N_TXT_NEWLINE_BINARY )
	)
	{
		txt->readonly = N_TXT_READONLY_ON;
	} else {
		txt->readonly = N_TXT_READONLY_OFF;
	}


	return n_posix_false;
}

n_posix_bool
n_txt_save_main( n_txt *txt, const n_posix_char *fname )
{

	if ( n_txt_error( txt ) ) { return n_posix_true; }


	if ( txt->readonly ) { return n_posix_true; }


	// Encoder

	n_posix_bool  is_allocated = n_posix_true;
	char         *ptr;
	n_type_int    byte;

#ifdef UNICODE

	if ( txt->unicode == N_TXT_UNICODE_NIL )
	{

		byte = txt->byte;
		ptr  = n_unicode_alloccopy( &byte, txt->stream );

//MessageBoxW( NULL, (wchar_t*) ptr, L"DEBUG", 0 );

		n_posix_bool lost = n_unicode_codec_wchar2char_no_bom( ptr, byte );
		if ( lost ) { n_memory_free( ptr ); return n_posix_true; }


		byte = strlen( ptr );

	} else {

		byte = txt->byte;
		ptr  = n_unicode_alloccopy_with_bom( &byte, txt->stream, n_unicode_bom_utf16_le );

//n_posix_debug_literal( "%d : %d", byte, txt->byte );


		if ( txt->unicode == N_TXT_UNICODE_LIL )
		{

			byte = 2 * n_posix_strlen( (void*) ptr );

		} else
		if ( txt->unicode == N_TXT_UNICODE_BIG )
		{

			n_unicode_endianness( ptr, byte );

			byte = 2 * n_posix_strlen( (void*) ptr );

		} else
		if ( txt->unicode == N_TXT_UNICODE_UTF )
		{

			n_unicode_utf8_encode( ptr, byte );

			byte = strlen( ptr );

		}

//n_posix_debug_literal( "%d : %d", byte, txt->byte );

	}


#else // #ifdef UNICODE


	if ( txt->unicode == N_TXT_UNICODE_NIL )
	{

		is_allocated = n_posix_false;

		byte = txt->byte;
		ptr  = txt->stream;

//n_posix_debug( ptr );

	} else {

		byte = txt->byte;
		ptr  = n_unicode_alloccopy( &byte, txt->stream );


		// [!] : n_posix_false is returned always

		n_posix_bool lost = n_unicode_codec_char2wchar( ptr, byte );
		if ( lost ) { n_memory_free( ptr ); return n_posix_true; }


		byte = wcslen( (wchar_t*) ptr ) * sizeof( wchar_t );

//MessageBoxW( NULL, ptr_t, L"DEBUG", 0 );

		if ( txt->unicode == N_TXT_UNICODE_BIG )
		{

			n_unicode_endianness( ptr, byte );

		} else
		if ( txt->unicode == N_TXT_UNICODE_UTF )
		{

			n_unicode_utf8_encode( ptr, byte );

			byte = strlen( ptr );

		}

	}


#endif // #ifdef UNICODE


	n_posix_bool ret = n_posix_false;


	FILE *fp = n_posix_fopen_write( fname );
	if ( fp == NULL )
	{
		ret = n_posix_true;
	} else {
		n_posix_fwrite( ptr, byte, 1, fp );
	}
	n_posix_fclose( fp );


	if ( is_allocated ) { n_memory_free( ptr ); }


	return ret;
}

#define n_txt_save_literal( txt, fname ) n_txt_save( txt, n_posix_literal( fname ) )

n_posix_bool
n_txt_save( n_txt *txt, const n_posix_char *fname )
{

	if ( n_txt_error( txt ) ) { return n_posix_true; }


	if ( txt->readonly ) { return n_posix_true; }


	n_txt_stream( txt );


	return n_txt_save_main( txt, fname );
}

void
n_txt_copy( n_txt *p_old, n_txt *p_new )
{

	if ( n_txt_error( p_old ) ) { return; }
	if ( p_new == NULL ) { return; }


	n_txt_new( p_new );


	n_type_int i = 0;
	n_posix_loop
	{//break;

		// [!] : don't use n_txt_set()

		if ( i == 0 )
		{
			n_txt_mod( p_new, i, n_txt_get( p_old, i ) );
		} else {
			n_txt_add( p_new, i, n_txt_get( p_old, i ) );
		}

		i++;
		if ( i >= p_old->sy ) { break; }
	}


	p_new->newline  = p_old->newline;
	p_new->unicode  = p_old->unicode;
	p_new->readonly = p_old->readonly;

	n_txt_stream( p_new );


	return;
}

n_posix_bool
n_txt_is_same( const n_txt *txt_f, const n_txt *txt_t )
{

	n_posix_bool ret = n_posix_false;


	if ( n_txt_error( txt_f ) ) { return ret; }
	if ( n_txt_error( txt_t ) ) { return ret; }


//n_posix_debug_literal( " %d %d ", txt_f->sy, txt_t->sy );
	if ( txt_f->sy != txt_t->sy ) { return ret; }


	n_type_int i = 0;
	n_posix_loop
	{

		if ( n_posix_false == n_string_is_same( n_txt_get( txt_f, i ), n_txt_get( txt_t, i ) ) )
		{
			ret = n_posix_false;
			break;
		}

		i++;
		if ( i >= txt_f->sy ) { ret = n_posix_true; break; }
	}


//n_posix_debug_literal( "%d", ret );
	return ret;
}

#define n_txt_debug_printf_literal( f, ... ) n_txt_debug_printf( n_posix_literal( f ), ##__VA_ARGS__ )

void
n_txt_debug_printf( const n_posix_char *format, ... )
{

	n_posix_char str[ 1024 ];


	va_list vl;


	va_start( vl, format );

	n_posix_vsprintf( str, format, vl );

	va_end( vl );


	n_txt txt; n_txt_zero( &txt ); n_txt_new( &txt );

	n_txt_set( &txt, 0, str );

	n_txt_save_literal( &txt, "debug.txt" );
	n_txt_free( &txt );


	return;
}

void
n_txt_debug_iotest( n_posix_char *fname )
{

	n_txt txt;


	n_txt_zero( &txt );
	n_txt_load( &txt, fname );


	n_txt_stream( &txt );
//n_posix_debug_literal( "%s", txt.stream );


	n_txt_save_literal( &txt, "result.txt" );
	n_txt_free( &txt );


	return;
}


#endif // _H_NONNON_NEUTRAL_TXT

