// Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : don't use "posix.c" in this file


// Debug Facility : don't use as main release
//
//	"#define N_MEMORY_DEBUG" to turn ON





#ifndef _H_NONNON_NEUTRAL_MEMORY
#define _H_NONNON_NEUTRAL_MEMORY




#include "./type.c"




#ifdef N_POSIX_PLATFORM_MAC

// [!] : Copilot : "use memcpy() simply"
#define n_memcpy_fast memcpy

/*
void
n_memcpy_fast( void *dst, const void *src, size_t count )
{

	// [!] : a little faster : I've found this on StackOverflow

	u8 *dst8 = (u8*) dst;
	u8 *src8 = (u8*) src;

	if ( count & 1 )
	{
		dst8[ 0 ] = src8[ 0 ];

		dst8 += 1;
		src8 += 1;
	}

	// [!] : this code only improves performance
	//
	//	u32 or u64 are not faster, a little slower

	count /= 2;

	while( count-- )
	{
		dst8[ 0 ] = src8[ 0 ];
		dst8[ 1 ] = src8[ 1 ];

		dst8 += 2;
		src8 += 2;
	}


	return;
}
*/

#else  //#ifdef N_POSIX_PLATFORM_MAC


// [!] : Copilot : "use memcpy() simply"
#define n_memcpy_fast memcpy

//#include "../floss/memcpy_fast.c"


#endif //#ifdef N_POSIX_PLATFORM_MAC




#include <stdlib.h>
#include <string.h>




#define n_memory_loop for(;;)




// [!] : use when logic is safely closed
//
//	a : allocated memory will be overwritten immediately
//	b : allocated memory will be freed in narrow scope

#define n_memory_new_closed  n_memory_new
#define n_memory_free_closed n_memory_free




// [!] : malloc() limits size_t
//
//	currently, do exit() when gets over limit

#define N_MEMORY_COUNT_MAX ULONG_MAX




#ifdef N_MEMORY_DEBUG


#ifdef N_POSIX_PLATFORM_WINDOWS

#include "../win32/win/mutex.c"

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_MEMORY_DEBUG




#ifdef N_MEMORY_DEBUG


static n_type_int n_memory_refcount = 0;


#else // #ifdef N_MEMORY_DEBUG


#define n_memory_refcount 0


#endif // #ifdef N_MEMORY_DEBUG




void
n_memory_dialog( char *str )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	MessageBoxA( NULL, str, "INFO", MB_TOPMOST | MB_OK );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MAC

	NSLog( @"%s", str );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	printf( "%s", str );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

	return;
}




typedef void n_memory_limit_callback( void );

static n_memory_limit_callback *n_memory_limit_callback_func = NULL;

void
n_memory_limit( int error_number, long long byte )
{

#ifdef DEBUG

	if ( n_memory_limit_callback_func != NULL )
	{
		(*n_memory_limit_callback_func)();
	}

	char str[ 100 ];
	snprintf( str, 100, "Memory limit is over : %d byte  No. %lld", error_number, byte );

	n_memory_dialog( str );

#else

	if ( n_memory_limit_callback_func != NULL )
	{
		(*n_memory_limit_callback_func)();
	} else {
		char str[ 100 ];
		snprintf( str, 100, "Memory limit is over : %d byte  No. %lld", error_number, byte );

		n_memory_dialog( str );
	}

#endif

	exit( 0 );


	return;
}




void
n_memory_debug_refcount( void )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

#ifdef N_MEMORY_DEBUG

	char str[ 100 ];

	snprintf( str, 100, "%d", (int) n_memory_refcount );

	MessageBoxA( NULL, str, "DEBUG", MB_TOPMOST | MB_OK );

#endif // #ifdef N_MEMORY_DEBUG

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

	return;
}

void
n_memory_debug_on( void )
{

	// [x] : this module is flaky : call n_memory_debug_refcount() directly

#ifdef N_MEMORY_DEBUG

	atexit( n_memory_debug_refcount );

#endif // #ifdef N_MEMORY_DEBUG

	return;
}

void*
n_memory_new( n_type_int byte )
{

	// [!] : free() equivalent behavior

	if ( byte == 0 )
	{
		return NULL;
	}


	// [!] : not supported by POSIX

	if ( byte > N_MEMORY_COUNT_MAX )
	{
		n_memory_limit( 1, byte );
	}


	if ( byte < 0 )
	{
		n_memory_limit( 2, byte );
	}


	void *ptr = malloc( byte );


	// [!] : currently, not supported

	// [!] : 3GB when 32-bit

	if ( ptr == NULL )
	{
		n_memory_limit( 3, byte );
	}

	//memset( ptr, 0, byte );


#ifdef N_MEMORY_DEBUG

	// [!] : scrambling

	memset( ptr, rand(), byte );


#ifdef N_POSIX_PLATFORM_WINDOWS

	HANDLE hmutex = n_win_mutex_init_and_wait_literal( NULL, "n_memory_new()" );

	n_memory_refcount++;

	hmutex = n_win_mutex_exit( hmutex );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_memory_refcount++;

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_MEMORY_DEBUG


	return ptr;
}

void
n_memory_free( void *ptr )
{

	if ( ptr == NULL ) { return; }


	free( ptr );


#ifdef N_MEMORY_DEBUG


#ifdef N_POSIX_PLATFORM_WINDOWS

	HANDLE hmutex = n_win_mutex_init_and_wait_literal( NULL, "n_memory_free()" );

	if ( n_memory_refcount >= 1 ) { n_memory_refcount--; }

	hmutex = n_win_mutex_exit( hmutex );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	if ( n_memory_refcount >= 1 ) { n_memory_refcount--; }

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_MEMORY_DEBUG


	return;
}

void*
n_memory_resize( void* ptr, n_type_int byte )
{

	// [!] : free() equivalent behavior

	if ( byte == 0 )
	{

		n_memory_free( ptr );

		return NULL;
	}


	// [!] : not supported by POSIX

	if ( byte > N_MEMORY_COUNT_MAX )
	{
		n_memory_limit( 4, byte );
	}


	if ( byte < 0 )
	{
		n_memory_limit( 5, byte );
	}


	void *ret = realloc( ptr, byte );


	// [!] : currently, not supported

	// [!] : 3GB when 32-bit

	if ( ret == NULL )
	{
		n_memory_limit( 6, byte );
	}


#ifdef N_MEMORY_DEBUG


#ifdef N_POSIX_PLATFORM_WINDOWS

	HANDLE hmutex = n_win_mutex_init_and_wait_literal( NULL, "n_memory_resize()" );

	if ( ptr == NULL ) { n_memory_refcount++; }

	hmutex = n_win_mutex_exit( hmutex );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	if ( ptr == NULL ) { n_memory_refcount++; }

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


#endif // #ifdef N_MEMORY_DEBUG


	return ret;
}




#define n_memory_zero( ptr, byte ) n_memory_padding( ptr, 0, byte )

void
n_memory_padding( void *ptr, char c, n_type_int byte )
{

	if ( ptr  == NULL ) { return; }
	if ( byte <=    0 ) { return; }


	memset( ptr, c, byte );


	return;
}

void
n_memory_padding_u8( void *ptr, u8 c, n_type_int byte )
{

	if ( ptr  == NULL ) { return; }
	if ( byte <=    0 ) { return; }


	memset( ptr, c, byte );


	return;
}

void
n_memory_padding_unsigned( void *ptr, unsigned char c, n_type_int byte )
{

	if ( ptr  == NULL ) { return; }
	if ( byte <=    0 ) { return; }


	memset( ptr, c, byte );


	return;
}

void
n_memory_padding_int( int *ptr, int c, n_type_int count )
{

	// [!] : count = byte / sizeof( int )


	if ( ptr == NULL ) { return; }


	n_type_int i = 0;
	n_memory_loop
	{

		ptr[ i ] = c;

		i++;
		if ( i >= count ) { break; }
	}


	return;
}

int
n_memory_is_same( const void *a, const void *b, n_type_int byte )
{

	if ( a    == NULL ) { return 0; }
	if ( b    == NULL ) { return 0; }
	if ( byte ==    0 ) { return 0; }


	return ( 0 == memcmp( a, b, byte ) );
}

void
n_memory_copy_slow( const void *f, void *t, n_type_int byte )
{

	if ( f    == NULL ) { return; }
	if ( t    == NULL ) { return; }
	if ( byte ==    0 ) { return; }


	memmove( t, f, byte );


	return;
}

void
n_memory_copy( const void *f, void *t, n_type_int byte )
{

	if ( f    == NULL ) { return; }
	if ( t    == NULL ) { return; }
	if ( byte ==    0 ) { return; }


	n_memcpy_fast( t, f, byte );


	return;
}




void*
n_memory_element_add( void *p, n_type_int align, n_type_int count, n_type_int index )
{

	if ( p == NULL ) { return NULL; }

	if ( align == 0 ) { return NULL; }
	if ( count == 0 ) { return NULL; }

	// [!] : index == count is acceptable

	if ( index > count ) { return NULL; }


	count++;

	n_type_int  all  = align * count;
	n_type_int  prev = align * index;
	n_type_int  next = prev  + align;
	char       *ptr  = (char*) n_memory_resize( p, all );

	if ( next != all )
	{
		n_memory_copy_slow( &ptr[ prev ], &ptr[ next ], all - next );
	}

	n_memory_padding( &ptr[ prev ], 0, align );


	return ptr;
}

void*
n_memory_element_del( void *p, n_type_int align, n_type_int count, n_type_int index )
{

	if ( p == NULL ) { return NULL; }

	if ( align <= 0 ) { return NULL; }
	if ( count <= 0 ) { return NULL; }

	if ( index >= count ) { return NULL; }


	n_type_int  all  = align * count;
	n_type_int  prev = align * index;
	n_type_int  next = prev  + align;
	char       *ptr  = (char*) p;

	if ( prev != all )
	{
		n_memory_copy_slow( &ptr[ next ], &ptr[ prev ], all - next );
	}


	// [!] : malloc() compatible behavior

	p = n_memory_resize( p, all - align );
	if ( p == NULL ) { p = malloc( 0 ); }


	return p;
}

void
n_memory_element_swap( void *p, n_type_int align, n_type_int index_1, n_type_int index_2 )
{

	if ( p == NULL ) { return; }

	if ( align == 0 ) { return; }

	if ( index_1 == index_2 ) { return; }


	index_1 *= align;
	index_2 *= align;

	char *ptr = (char*) p;

/*
	char *elm = (char*) n_memory_new_closed( align );

	n_memory_copy_slow( &ptr[ index_1 ],             elm, align );
	n_memory_copy_slow( &ptr[ index_2 ], &ptr[ index_1 ], align );
	n_memory_copy_slow(             elm, &ptr[ index_2 ], align );

	n_memory_free_closed( elm );
*/

	// [!] : Thx : Copilot

	// Use a stack buffer for small element sizes to avoid malloc/free overhead.
	if ( align <= 1024 )
	{
		char elm_stack[ 1024 ];
		n_memory_copy_slow( &ptr[ index_1 ],       elm_stack, align );
		n_memory_copy_slow( &ptr[ index_2 ], &ptr[ index_1 ], align );
		n_memory_copy_slow( elm_stack      , &ptr[ index_2 ], align );
	}
	else
	{
		char *elm = (char*) n_memory_new_closed( align );
		if ( elm == NULL ) { return; }

		n_memory_copy_slow( &ptr[ index_1 ],             elm, align );
		n_memory_copy_slow( &ptr[ index_2 ], &ptr[ index_1 ], align );
		n_memory_copy_slow(             elm, &ptr[ index_2 ], align );

		n_memory_free_closed( elm );
	}

	return;
}


#endif // _H_NONNON_NEUTRAL_MEMORY

