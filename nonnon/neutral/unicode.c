// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : don't use posix.c in this file


// [!] : iconv support
//
//	header : #include <iconv.h>
//	link   : -liconv
//	dll    : libiconv2.dll




#ifndef _H_NONNON_NEUTRAL_UNICODE
#define _H_NONNON_NEUTRAL_UNICODE




#include <stdlib.h>
#include <string.h>




#define n_unicode_loop for(;;)




#ifndef _LIBICONV_H


#ifdef N_POSIX_PLATFORM_MAC

// [!] : stub

#else  // #ifdef N_POSIX_PLATFORM_MAC

#include <windows.h>

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifndef _LIBICONV_H


#include "./memory.c"




#ifdef _WINDOWS_H

#define n_unicode_bool  BOOL
#define n_unicode_false FALSE
#define n_unicode_true  TRUE

#else  // #ifdef _WINDOWS_H

#define n_unicode_bool  int
#define n_unicode_false 0
#define n_unicode_true  1

#endif // #ifdef _WINDOWS_H




#define n_unicode_alloccopy( byte, stream ) n_unicode_alloccopy_with_bom( byte, stream, NULL ) 

const u8 n_unicode_bom_utf8    [ 4 ] = { 0xef, 0xbb, 0xbf, 0x00 };
const u8 n_unicode_bom_utf16_le[ 4 ] = { 0xff, 0xfe, 0x00, 0x00 };
const u8 n_unicode_bom_utf16_be[ 4 ] = { 0xfe, 0xff, 0x00, 0x00 };

// internal
void*
n_unicode_alloccopy_with_bom( n_type_int *byte, const void *stream, const u8 *bom )
{

	// [!] : you need to n_memory_free() a returned variable

	// [Mechanism]
	//
	//	a : always handled as byte stream
	//	b : stream accepts the both Unicode and Non-Unicode
	//	c : no error handling available like if ( byte < strlen( stream ) )

	// [Mechanism] : inaccurate but enough size
	//
	//	[ to Unicode ]
	//	Non-Unicode : ( BOM(1) + strlen() + NUL(1) ) * sizeof( wchar_t )
	//	UTF-8       : 1 byte encode will inflate
	//	UTF-8       : 2 byte encode will be the same
	//	UTF-8       : 3 byte encode will deflate
	//
	//	[ Unicode to Non-Unicode ]
	//	Non-DBCS : always deflate
	//	DBCS     : same or deflate
	//
	//	[ Unicode to UTF-8 ]
	//	1 byte encode will deflate
	//	2 byte encode will be the same
	//	3 byte encode will inflate


	if ( byte == NULL ) { return NULL; }


	// [!] : maximum size as possible
	//
	//	[ BOM(0/2/3/4) + String + NUL(1/2/4) ]

	n_type_int  b = 4 + ( (*byte) * 4 ) + 4;
	void       *s = n_memory_new( b * 4 );


	if ( stream == NULL )
	{

		n_memory_zero( s, b );

	} else {

		u8 *p = (u8*) s;

		n_type_int offset = 0;

		if ( bom != NULL )
		{

			p[ 0 ] = bom[ 0 ];
			p[ 1 ] = bom[ 1 ];
			p[ 2 ] = bom[ 2 ];
			p[ 3 ] = bom[ 3 ];

			if ( bom[ 2 ] == 0x00 )
			{
				n_memory_copy( stream, &p[ 2 ], (*byte) );
				offset = 2 + (*byte);
			} else {
				n_memory_copy( stream, &p[ 3 ], (*byte) );
				offset = 3 + (*byte);
			}

		} else {

			n_memory_copy( stream, s, (*byte) );
			offset = (*byte);

		}

		p[ offset + 0 ] = 0;
		p[ offset + 1 ] = 0;
		p[ offset + 2 ] = 0;
		p[ offset + 3 ] = 0;

	}


	(*byte) = b;


	return s;
}

#define n_unicode_bom_is_utf8(     s, b ) n_unicode_bom_detect( s, b, n_unicode_bom_utf8     )
#define n_unicode_bom_is_utf16_le( s, b ) n_unicode_bom_detect( s, b, n_unicode_bom_utf16_le )
#define n_unicode_bom_is_utf16_be( s, b ) n_unicode_bom_detect( s, b, n_unicode_bom_utf16_be )

// internal
n_unicode_bool
n_unicode_bom_detect( const void *stream, n_type_int byte, const u8 *bom )
{

	// [!] : no length check for "bom"


	if ( stream == NULL ) { return n_unicode_false; }
	if ( bom    == NULL ) { return n_unicode_false; }


	const u8 *p = (u8*) stream;

	if ( n_memory_is_same( n_unicode_bom_utf8,     bom, 3 ) )
	{
		if ( ( byte >= 3 )&&( n_memory_is_same( p, bom, 3 ) ) ) { return n_unicode_true; }
	} else
	if ( n_memory_is_same( n_unicode_bom_utf16_le, bom, 2 ) )
	{
		if ( ( byte >= 2 )&&( n_memory_is_same( p, bom, 2 ) ) ) { return n_unicode_true; }
	} else
	if ( n_memory_is_same( n_unicode_bom_utf16_be, bom, 2 ) )
	{
		if ( ( byte >= 2 )&&( n_memory_is_same( p, bom, 2 ) ) ) { return n_unicode_true; }
	}// else


	return n_unicode_false;
}

#define n_unicode_bom_restore_utf8(       s, b ) n_unicode_bom_restore(   s, b, n_unicode_bom_utf8     )
#define n_unicode_bom_restore_utf16_le(   s, b ) n_unicode_bom_restore(   s, b, n_unicode_bom_utf16_le )
#define n_unicode_bom_restore_utf16_be(   s, b ) n_unicode_bom_restore(   s, b, n_unicode_bom_utf16_be )

#define n_unicode_bom_overwrite_utf8(     s, b ) n_unicode_bom_overwrite( s, b, n_unicode_bom_utf8     )
#define n_unicode_bom_overwrite_utf16_le( s, b ) n_unicode_bom_overwrite( s, b, n_unicode_bom_utf16_le )
#define n_unicode_bom_overwrite_utf16_be( s, b ) n_unicode_bom_overwrite( s, b, n_unicode_bom_utf16_be )

#define n_unicode_bom_overwrite( s, b, bom ) n_unicode_bom_set( s, b, bom, n_unicode_false )
#define n_unicode_bom_restore(   s, b, bom ) n_unicode_bom_set( s, b, bom, n_unicode_true  )

// internal
void
n_unicode_bom_set( void *stream, n_type_int byte, const u8 *bom, n_unicode_bool restore )
{

	// [!] : no length check for "bom"
	// [!] : no adding NUL


	// [!] : use n_unicode_alloccopy() to re-allocate


	if ( stream == NULL ) { return; }
	if ( bom    == NULL ) { return; }
	if ( byte   <     4 ) { return; }


	u8 *p = (u8*) stream;

	if ( n_memory_is_same( n_unicode_bom_utf8, bom, 3 ) )
	{

		if ( restore ) { memmove( &p[ 3 ], &p[ 0 ], byte - 3 ); }

		n_memory_copy( bom, p, 3 );

	} else
	if (
		( n_memory_is_same( n_unicode_bom_utf16_le, bom, 2 ) )
		||
		( n_memory_is_same( n_unicode_bom_utf16_be, bom, 2 ) )
	)
	{

		if ( restore ) { memmove( &p[ 2 ], &p[ 0 ], byte - 2 ); }

		n_memory_copy( bom, p, 2 );

	}// else


	return;
}

void
n_unicode_bom_remove( void *stream, n_type_int byte )
{

	u8 *p = (u8*) stream;

	if ( n_unicode_bom_is_utf8( stream, byte ) )
	{

		n_memory_copy( &p[ 3 ], &p[ 0 ], byte - 3 );

		p[ byte - 3 ] = '\0';

	} else
	if (
		( n_unicode_bom_is_utf16_le( stream, byte ) )
		||
		( n_unicode_bom_is_utf16_be( stream, byte ) )
	)
	{

		n_memory_copy( &p[ 2 ], &p[ 0 ], byte - 2 );

		p[ byte - 2 ] = '\0';
		p[ byte - 3 ] = '\0';

	}// else


	return;
}

void
n_unicode_endianness( void *stream, n_type_int byte )
{

	if ( stream == NULL ) { return; }
	if (   byte <=    1 ) { return; }


	u8 *p = (u8*) stream;

	u8 swap;

	n_type_int i = 0;
	n_unicode_loop
	{

		swap       = p[ i     ];
		p[ i     ] = p[ i + 1 ];
		p[ i + 1 ] = swap;


		i += 2;
		if ( i >= byte ) { break; }
	}


	return;
}

n_posix_inline n_unicode_bool
n_unicode_surrogatepair_is_hi( u16 c )
{

#ifdef UNICODE

	return ( ( c >= 0xd800 )&&( c <= 0xdbff ) );

#else  // #ifdef UNICODE

	return n_unicode_false;

#endif // #ifdef UNICODE

}

n_posix_inline n_unicode_bool
n_unicode_surrogatepair_is_lo( u16 c )
{

#ifdef UNICODE

	return ( ( c >= 0xdc00 )&&( c <= 0xdfff ) );

#else  // #ifdef UNICODE

	return n_unicode_false;

#endif // #ifdef UNICODE

}

#define N_UNICODE_UTF8_ENCODE 0
#define N_UNICODE_UTF8_DECODE 1

#define n_unicode_utf8_encode( p, byte ) n_unicode_utf8( p, byte, N_UNICODE_UTF8_ENCODE, n_unicode_true )
#define n_unicode_utf8_decode( p, byte ) n_unicode_utf8( p, byte, N_UNICODE_UTF8_DECODE, n_unicode_true )

#define n_unicode_utf8_encode_no_bom( p, byte ) n_unicode_utf8( p, byte, N_UNICODE_UTF8_ENCODE, n_unicode_false )
#define n_unicode_utf8_decode_no_bom( p, byte ) n_unicode_utf8( p, byte, N_UNICODE_UTF8_DECODE, n_unicode_false )

void
n_unicode_utf8( void *stream, n_type_int byte, int mode, n_unicode_bool bom_needed )
{

	if ( mode == N_UNICODE_UTF8_ENCODE )
	{

		// UTF-16 Little Endian to UTF-8

		u16 *utf16 = (u16*) stream;
		u8  *utf8  = (u8*) n_memory_new_closed( byte );

		n_memory_zero( utf8, byte );


		// BOM

		n_type_int bom_utf16_offset = 0;
		n_type_int bom_utf_8_offset = 0;
		if ( bom_needed )
		{
			bom_utf_8_offset = 3;
			n_unicode_bom_overwrite_utf8( utf8, byte );

			bom_utf16_offset = 1;
		}


		n_type_int i  = bom_utf16_offset;
		n_type_int ii = bom_utf_8_offset;
		n_unicode_loop
		{

			if ( n_unicode_surrogatepair_is_hi( utf16[ i ] ) )
			{

				// Surrogate Pair

				u32 utf32 = 0x10000;
				utf32 |= ( utf16[ i + 0 ] & 0x03ff ) << 10;
				utf32 |= ( utf16[ i + 1 ] & 0x03ff );

				utf8[ ii + 0 ] = (u8) ( 0xf0 | ( ( utf32 >> 17 )          ) ); // 3bit
				utf8[ ii + 1 ] = (u8) ( 0x80 | ( ( utf32 >> 12 ) & 0x003f ) ); // 5bit
				utf8[ ii + 2 ] = (u8) ( 0x80 | ( ( utf32 >>  6 ) & 0x003f ) ); // 6bit
				utf8[ ii + 3 ] = (u8) ( 0x80 | ( ( utf32 >>  0 ) & 0x003f ) ); // 6bit

				i  += 2;
				ii += 4;

			} else

			if ( utf16[ i ] >= 0x0800 )
			{

				// UTF-16 : 16bit
				// UTF-8  : 4bit + 6bit + 6bit

				utf8[ ii + 0 ] = 0xe0 | ( ( utf16[ i ] >> 12 )          );
				utf8[ ii + 1 ] = 0x80 | ( ( utf16[ i ] >>  6 ) & 0x003f );
				utf8[ ii + 2 ] = 0x80 | ( ( utf16[ i ] >>  0 ) & 0x003f );


				i++;

				ii += 3;

			} else

			if ( utf16[ i ] >= 0x0080 )
			{

				// UTF-16 : 11bit
				// UTF-8  : 5bit + 6bit

				utf8[ ii + 0 ] = 0xc0 | ( ( utf16[ i ] >>  6 )          );
				utf8[ ii + 1 ] = 0x80 | ( ( utf16[ i ] >>  0 ) & 0x003f );


				i++;

				ii += 2;

			} else

			if ( utf16[ i ] != 0x0000 )
			{

				utf8[ ii ] = (u8) utf16[ i ];


				i++;

				ii += 1;

			}


			if ( utf16[ i ] == L'\0' ) { break; }

//if ( ii > byte ) { MessageBoxA( NULL, "Buffer is insufficient", "", 0 ); free( utf8 ); return; }
		}

		n_memory_copy( utf8, utf16, byte );

		n_memory_free_closed( utf8 );

	} else {

		// UTF-8 to UTF-16 Little Endian

		// [!] : a hiragana letter which pronounced as /a/
		//
		//	utf-8  : e3 81 82 : 11000011 10000001 10000010
		//	utf-16 : 30 42    :     0011   000001   000010


		u8  *utf8  = (u8 *) stream;
		u16 *utf16 = (u16*) n_memory_new_closed( byte );

		n_memory_zero( utf16, byte );


		n_type_int utf_8_bom = 0;

		if ( bom_needed )
		{
			if ( n_unicode_bom_is_utf8( utf8, byte ) ) { utf_8_bom = 3; }
		}


		n_type_int utf16_bom = 0;

		if ( bom_needed )
		{
			n_unicode_bom_overwrite_utf16_le( utf16, byte );
			utf16_bom = 1;
		}


		n_unicode_bool nonascii = n_unicode_false;
		n_unicode_bool error    = n_unicode_false;


		n_type_int i  = utf_8_bom;
		n_type_int ii = utf16_bom;
		n_unicode_loop
		{

			if ( utf8[ i ] >= 0xfe )
			{

				// [!] : not utf-8 by specification

				error = n_unicode_true;

				break;

			} else

			if ( utf8[ i ] >= 0xf0 )
			{

				// Surrogate Pair

				nonascii = n_unicode_true;


				if ( utf8[ i + 1 ] == '\0' ) { error = n_unicode_true; break; }
				if ( utf8[ i + 2 ] == '\0' ) { error = n_unicode_true; break; }
				if ( utf8[ i + 3 ] == '\0' ) { error = n_unicode_true; break; }


				u32 utf32 = 0;
				utf32 |= ( utf8[ i + 0 ] & ~0xf0 ) << 17; // 3bit
				utf32 |= ( utf8[ i + 1 ] & ~0x80 ) << 12; // 5bit
				utf32 |= ( utf8[ i + 2 ] & ~0x80 ) <<  6; // 6bit
				utf32 |= ( utf8[ i + 3 ] & ~0x80 ) <<  0; // 6bit

				utf32 -= 0x10000;

				utf16[ ii + 0 ] |= 0xd800 + ( utf32 >>    10 );
				utf16[ ii + 1 ] |= 0xdc00 + ( utf32  & 0x3ff );


				ii += 2;
				 i += 4;

			} else

			if ( utf8[ i ] >= 0xe0 )
			{

				nonascii = n_unicode_true;


				if ( utf8[ i + 1 ] == '\0' ) { error = n_unicode_true; break; }
				if ( utf8[ i + 2 ] == '\0' ) { error = n_unicode_true; break; }


				utf16[ ii ] |= ( utf8[ i + 0 ] & ~0xe0 ) << 12; // 4bit
				utf16[ ii ] |= ( utf8[ i + 1 ] & ~0x80 ) <<  6; // 6bit
				utf16[ ii ] |= ( utf8[ i + 2 ] & ~0x80 )      ; // 6bit


				ii++;

				i += 3;

			} else

			if ( utf8[ i ] >= 0xc0 )
			{

				nonascii = n_unicode_true;


				if ( utf8[ i + 1 ] == '\0' ) { error = n_unicode_true; break; }


				utf16[ ii ] |= ( utf8[ i + 0 ] & ~0xc0 ) << 6; // 5bit
				utf16[ ii ] |= ( utf8[ i + 1 ] & ~0x80 )     ; // 6bit


				ii++;

				i += 2;

			} else

			if ( utf8[ i ] >= 0x80 )
			{

				error = n_unicode_true;

				break;

			} else {

				utf16[ ii ] = utf8[ i ];


				ii++;

				i += 1;

			}


			if ( utf8[ i ] == '\0' ) { break; }
		}


		// [!] : ascii-only / non-utf-8 / broken
		// [!] : except for empty file

		if (
			( error )
			||
			(
				( bom_needed )
				&&
				( utf_8_bom == 0 )
				&&
				( nonascii == n_unicode_false )
			)
		)
		{

			// [!] : placeholder for memory freeing

		} else {

			n_memory_copy( utf16, utf8, byte );

		}

//MessageBoxA( NULL, utf8, "DEBUG", 0 );
//MessageBoxW( NULL, (wchar_t*) &utf16[ 1 ], L"DEBUG", 0 );


		n_memory_free_closed( utf16 );

	}


	return;
}

#define N_UNICODE_CODEC_CHAR2WCHAR 0
#define N_UNICODE_CODEC_WCHAR2CHAR 1

#define n_unicode_codec_char2wchar( f, s ) n_unicode_codec( f, s, N_UNICODE_CODEC_CHAR2WCHAR, n_unicode_true )
#define n_unicode_codec_wchar2char( f, s ) n_unicode_codec( f, s, N_UNICODE_CODEC_WCHAR2CHAR, n_unicode_true )

#define n_unicode_codec_char2wchar_no_bom( f, s ) n_unicode_codec( f, s, N_UNICODE_CODEC_CHAR2WCHAR, n_unicode_false )
#define n_unicode_codec_wchar2char_no_bom( f, s ) n_unicode_codec( f, s, N_UNICODE_CODEC_WCHAR2CHAR, n_unicode_false )

n_unicode_bool
n_unicode_codec( void *stream, n_type_int byte, int mode, n_unicode_bool bom_needed )
{

	n_unicode_bool lost = n_unicode_true;


	if ( mode == N_UNICODE_CODEC_CHAR2WCHAR )
	{


		u8 *f = (u8*) stream;
		u8 *t = (u8*) n_memory_new_closed( byte );

		n_memory_zero( t, byte );


		u8  *target;
		int  offset;

		if ( bom_needed )
		{
			n_unicode_bom_overwrite_utf16_le( t, byte );
			target = &t[ 2 ];
			offset = 2;
		} else {
			target = t;
			offset = 0;
		}


#ifdef _LIBICONV_H


		{

			// [!] : skip Byte Order Mark (BOM)

			char *i = (void*) f;
			char *o = (void*) target;

			size_t byte_i = byte;
			size_t byte_o = byte;


			iconv_t cd = iconv_open( "UCS-2LE", "" );

			iconv( cd, (void*) &i, &byte_i, &o, &byte_o );

			iconv_close( cd );


			lost = n_unicode_false;

		}


#else  // #ifdef _LIBICONV_H


#ifdef N_POSIX_PLATFORM_MAC

	// [!] : stub

#else  // #ifdef N_POSIX_PLATFORM_MAC

		MultiByteToWideChar
		(
			CP_ACP,
			0,
			(char*) f,
			-1,
			(wchar_t*) target,
			(int) byte - offset
		);

		lost = n_unicode_false;

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifdef _LIBICONV_H

		n_memory_copy( t, f, byte );
		n_memory_free_closed( t );

	} else {

		// [!] : *f need to be Unicode Little Endian

		u8 *f = (u8*) stream;
		u8 *t = (u8*) n_memory_new_closed( byte );

		n_memory_zero( t, byte );


		// [!] : skip Byte Order Mark (BOM)

		u8 *from;

		if ( bom_needed )
		{
			from = &f[ 2 ];
		} else {
			from = f;
		}


#ifdef _LIBICONV_H


		{

			iconv_t cd;

			char *i = (void*) from;
			char *o = (void*) t;

			size_t byte_i = byte;
			size_t byte_o = byte;


			cd = iconv_open( "", "UCS-2LE" );

			lost = (int) iconv( cd, (void*) &i, &byte_i, &o, &byte_o );

			iconv_close( cd );

		}


#else // #ifdef _LIBICONV_H


#ifdef N_POSIX_PLATFORM_MAC

	// [!] : stub

#else  // #ifdef N_POSIX_PLATFORM_MAC

		// [!] : CP_MACCP : doesn't mean UNICODE Big Endian

		WideCharToMultiByte
		(
			CP_ACP,
			0,
			(wchar_t*) from,
			-1,
			(char*) t,
			(int) byte,
			NULL,
			&lost
		);

//n_posix_debug_literal( "%ld", GetLastError() );


		// [MSDN] : Win95 : do round-trip

		// [!] : Win98 or later only
		//
		//	<winnls.h>
		//	#define WC_NO_BEST_FIT_CHARS 1024

		if ( lost == n_unicode_false )
		{

			// [!] : handle as byte stream

			u8 *check = (u8*) n_memory_new_closed( byte );


			// [!] : copy BOM

			u8 *check_2;

			if ( bom_needed )
			{
				check[ 0 ] = f[ 0 ];
				check[ 1 ] = f[ 1 ];

				check_2 = &check[ 2 ];
			} else {
				check_2 = check;
			}


			MultiByteToWideChar
			(
				CP_ACP,
				0,
				(char*) t,
				-1,
				(wchar_t*) check_2,
				(int) byte
			);


			wchar_t *wf = (wchar_t*) f;
			wchar_t *wt = (wchar_t*) check;

			n_type_int i = 0;
			n_unicode_loop
			{

				if ( wf[ i ] != wt[ i ] )
				{

					lost = n_unicode_true;

					break;
				}

				i++;
				if ( ( wf[ i ] == 0 )||( wt[ i ] == 0 ) ) { break; }
			}


			n_memory_free_closed( check );

		}

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifdef _LIBICONV_H

		n_memory_copy( t, f, byte );
		n_memory_free_closed( t );

	}


	return lost;
}


#endif // _H_NONNON_NEUTRAL_UNICODE

