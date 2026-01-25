// Nonnon Posix
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : the reasons why this file is needed
//
//	a : tchar.h is Windows only
//	b : POSIX APIs are flaky with crash
//	c : bridging between POSIX-compliant OSs
//
//	GLIBC doesn't support almost of Unicode APIs


// [!] : C99 only
//
//	"inline" is used for performance
//	on-the-fly variable declaration is used for readability


// [!] : inclusion order
//
//	1 : #define UNICODE
//	2 : #define N_POSIX_TIMEGETTIME
//	3 : #include "Nonnon Base Layer"
//	4 : #include <other headers>
//
//	you need to link "-lwinmm" when you include <windows.h> before the base layer
//	N_POSIX_TIMEGETTIME also needs "-lwinmm"




#ifndef _H_NONNON_NEUTRAL_POSIX
#define _H_NONNON_NEUTRAL_POSIX




#ifdef _MSC_VER


#pragma comment( lib, "winmm" )


#define M_PI 3.14159265358979323846


// [!] : use newer MS original POSIX functions
#define _CRT_SECURE_NO_WARNINGS

// [!] : for vswprintf() : parameter is different from vsprintf()
#define _CRT_NON_CONFORMING_SWPRINTFS

// [!] : suppress : HANDLE to another type
#pragma warning( disable : 4311 )

// [!] : suppress : another type to HANDLE
#pragma warning( disable : 4312 )

// [!] : suppress : "don't use GetVersionEx()"
#pragma warning( disable : 4996 )


#endif // #ifdef _MSC_VER




// [!] : GCC : <direct.h> is not exist
// [!] : errno.h : buggy : but n_posix_access() uses this

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <sys/stat.h>
#include <sys/types.h>




#ifdef __APPLE__


#define N_POSIX_PLATFORM_MAC

#include <dirent.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/time.h>
#include <utime.h>


#else // #ifndef __APPLE__


#include <io.h>
#include <sys/utime.h>


#endif // #ifdef __APPLE__




#ifdef __MINGW_H


#define N_POSIX_PLATFORM_MINGW


// [!] : MinGW only

//#include <dirent.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/time.h>


#endif // #ifdef __MINGW_H



#ifdef  __MINGW32__


#define N_POSIX_PLATFORM_MINGW64


// [x] : suppress stupid sprintf() warning

//#pragma GCC diagnostic ignored "-Wrestrict"


#endif // #ifdef  __MINGW32__




// [!] : __MINGW_H  : good old MinGW
// [!] :__MINGW32__ : MinGW64 : compatibility hell

#if defined( N_POSIX_PLATFORM_MINGW ) || defined( N_POSIX_PLATFORM_MINGW64 ) || defined( _MSC_VER )

#undef  _WIN32_IE
#undef  _WIN32_WINDOWS
#undef  _WIN32_WINNT
#undef  _WINVER

#define _WIN32_IE      0xffff
#define _WIN32_WINDOWS 0xffff
#define _WIN32_WINNT   0xffff
#define _WINVER        0xffff


#define WIN32_LEAN_AND_MEAN
#define COBJMACROS

#include <objbase.h>
#include <windows.h>


#define N_POSIX_PLATFORM_WINDOWS
//#error  "Windows"


#endif




#if defined( N_POSIX_PLATFORM_WINDOWS ) && defined( N_POSIX_TIMEGETTIME )

#include <mmsystem.h>
//#error   "timeGetTime()"

#endif // #if defined( N_POSIX_PLATFORM_WINDOWS ) && defined( N_POSIX_TIMEGETTIME )




#ifdef N_POSIX_PLATFORM_MAC

#define n_posix_inline

#else  // #ifdef N_POSIX_PLATFORM_MAC

// [x] : MinGW64 : incompatible

#define n_posix_inline

#endif // #ifdef N_POSIX_PLATFORM_MAC




#include "./memory.c"
#include "./unicode.c"




#ifdef UNICODE


// [!] : wchar.h hasn't some symbols like _wopendir()

// [!] : Win2000 or earlier : _wtof() is not exist

// [x] : WinNT4.0 : swscanf() crashes


#define	n_posix_literal(q) L##q
#define n_posix_char       wchar_t

//#define n_posix_fopen      _wfopen
#define n_posix_fgets      fgetws
#define n_posix_printf     wprintf
#define n_posix_sprintf    swprintf
//#define n_posix_snprintf    swnprintf
#define n_posix_vsprintf   vswprintf
//#define n_posix_sscanf     swscanf
#define n_posix_rename     _wrename

#define n_posix_atoi       _wtoi
//#define n_posix_atof       _wtof
#define n_posix_system     _wsystem

#define n_posix_ctime      _wctime

#define n_posix_strcat     wcscat
#define n_posix_strcmp     wcscmp
//#define n_posix_strlen     wcslen

#define n_posix_chmod      _wchmod
#define n_posix_unlink     _wunlink

#define n_posix_chdir      _wchdir
#define n_posix_getcwd     _wgetcwd
#define n_posix_mkdir      _wmkdir
#define n_posix_rmdir      _wrmdir

//#define n_posix_structstat struct _stat
//#define n_posix_stat       _wstat


#ifndef N_POSIX_PLATFORM_WINDOWS

#define n_posix_DIR        _WDIR
#define n_posix_dirent     struct _wdirent
#define n_posix_opendir    _wopendir
#define n_posix_closedir   _wclosedir
#define n_posix_readdir    _wreaddir

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS


#define n_posix_utimbuf    struct _utimbuf
#define n_posix_utime      _wutime

///#define n_posix_strftime   wcsftime


#define n_posix_access     _waccess


#else // #ifdef UNICODE


#define	n_posix_literal(q) q
#define n_posix_char       char

//#define n_posix_fopen      fopen
#define n_posix_fgets      fgets
#define n_posix_printf     printf
#define n_posix_sprintf    sprintf
//#define n_posix_snprintf    snprintf
#define n_posix_vsprintf   vsprintf
//#define n_posix_sscanf     sscanf
#define n_posix_rename     rename

#define n_posix_atoi       atoi
//#define n_posix_atof       atof
#define n_posix_system     system

#define n_posix_ctime      ctime

#define n_posix_strcat     strcat
#define n_posix_strcmp     strcmp
//#define n_posix_strlen     strlen

#define n_posix_chmod      chmod
#define n_posix_unlink     unlink

#define n_posix_chdir      chdir
#define n_posix_getcwd     getcwd
#define n_posix_mkdir      mkdir
#define n_posix_rmdir      rmdir

//#define n_posix_structstat struct stat
//#define n_posix_stat       stat


#ifndef N_POSIX_PLATFORM_WINDOWS

#define n_posix_DIR        DIR
#define n_posix_dirent     struct dirent
#define n_posix_opendir    opendir
#define n_posix_closedir   closedir
#define n_posix_readdir    readdir

#endif // #ifndef N_POSIX_PLATFORM_WINDOWS


#define n_posix_utimbuf    struct utimbuf
#define n_posix_utime      utime

///#define n_posix_strftime   strftime


#define n_posix_access     _access


#endif // #ifdef UNICODE




#ifdef N_POSIX_PLATFORM_WINDOWS

#define N_POSIX_CHAR_SLASH         n_posix_literal( '\\' )
#define N_POSIX_SLASH              n_posix_literal( "\\" )

#else // #ifdef N_POSIX_PLATFORM_WINDOWS

#define N_POSIX_CHAR_SLASH         n_posix_literal( '/' )
#define N_POSIX_SLASH              n_posix_literal( "/" )
#define N_POSIX_PERMISSION_DEFAULT ( S_IRWXU | ( S_IRGRP | S_IXGRP ) | ( S_IROTH | S_IXOTH ) )

// [!] : GCC : ENOFILE is not exist

//#define ENOFILE ENOENT

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#ifdef N_POSIX_PLATFORM_MINGW

#include "../win32/sysinfo/version.c"

#endif




// [!] : for() is 1% faster than while()

#define n_posix_loop for(;;)
//#define n_posix_loop while( 1 )




// [ Mechanism ]
//
//	false is zero, true is non-zero
//	so you cannot use if ( var == true )
//
//	[ accurate code ]
//
//	if ( var ) { /* true */ }
//	if ( var != false ) { /*  true */ }
//	if ( var == false ) { /* false */ }


#define n_posix_bool  BOOL
#define n_posix_false FALSE
#define n_posix_true  TRUE




// [!] : this is simple but problematic

//#define n_posix_min( a, b ) ( ( (a) < (b) ) ? (a) : (b) )
//#define n_posix_max( a, b ) ( ( (a) > (b) ) ? (a) : (b) )


/*
// [!] : this is GCC only

#define n_posix_min( a, b )			\
	(					\
		{				\
			typeof( a ) _a = (a);	\
			typeof( b ) _b = (b);	\
			_a < _b ? _a : _b;	\
		}				\
	)
#define n_posix_max( a, b )			\
	(					\
		{				\
			typeof( a ) _a = (a);	\
			typeof( b ) _b = (b);	\
			_a > _b ? _a : _b;	\
		}				\
	)
*/

n_posix_inline int    n_posix_min       ( int    a, int    b ) { return a < b ? a : b; }
n_posix_inline s32    n_posix_min_s32   ( s32    a, s32    b ) { return a < b ? a : b; }
n_posix_inline s64    n_posix_min_s64   ( s64    a, s64    b ) { return a < b ? a : b; }
//n_posix_inline double n_posix_min_double( double a, double b ) { return a < b ? a : b; }

n_posix_inline int    n_posix_max       ( int    a, int    b ) { return a > b ? a : b; }
n_posix_inline s32    n_posix_max_s32   ( s32    a, s32    b ) { return a > b ? a : b; }
n_posix_inline s64    n_posix_max_s64   ( s64    a, s64    b ) { return a > b ? a : b; }
//n_posix_inline double n_posix_max_double( double a, double b ) { return a > b ? a : b; }

#define n_posix_minmax(        minim, maxim, n ) n_posix_max(        minim, n_posix_min(        maxim, n ) )
#define n_posix_minmax_s32(    minim, maxim, n ) n_posix_max_s32(    minim, n_posix_min_s32(    maxim, n ) )
//#define n_posix_minmax_double( minim, maxim, n ) n_posix_max_double( minim, n_posix_min_double( maxim, n ) )


n_posix_inline n_type_int n_posix_min_n_type_int( n_type_int a, n_type_int b ) { return a < b ? a : b; }
n_posix_inline n_type_int n_posix_max_n_type_int( n_type_int a, n_type_int b ) { return a > b ? a : b; }

#define n_posix_minmax_n_type_int( minim, maxim, n ) n_posix_max_n_type_int( minim, n_posix_min_n_type_int( maxim, n ) )

n_posix_inline n_type_int
n_posix_abs_n_type_int( n_type_int v )
{

	if ( v < 0 ) { v *= -1; }

	return v;
}

n_posix_inline n_type_gfx
n_posix_abs_n_type_gfx( n_type_gfx v )
{

	if ( v < 0 ) { v *= -1; }

	return v;
}


n_posix_inline n_type_gfx n_posix_min_n_type_gfx( n_type_gfx a, n_type_gfx b ) { return a < b ? a : b; }
n_posix_inline n_type_gfx n_posix_max_n_type_gfx( n_type_gfx a, n_type_gfx b ) { return a > b ? a : b; }

#define n_posix_minmax_n_type_gfx( minim, maxim, n ) n_posix_max_n_type_gfx( minim, n_posix_min_n_type_gfx( maxim, n ) )


n_posix_inline n_type_real n_posix_min_n_type_real( n_type_real a, n_type_real b ) { return a < b ? a : b; }
n_posix_inline n_type_real n_posix_max_n_type_real( n_type_real a, n_type_real b ) { return a > b ? a : b; }

#define n_posix_minmax_n_type_real( minim, maxim, n ) n_posix_max_n_type_real( minim, n_posix_min_n_type_real( maxim, n ) )




#define n_posix_printf_literal( a, ... ) n_posix_printf( n_posix_literal( a ), ##__VA_ARGS__ )

#define n_posix_sprintf_literal(  s, a, ... ) n_posix_sprintf( s, n_posix_literal( a ), ##__VA_ARGS__ )

//#define n_posix_sscanf_literal(   s, a, ... ) n_posix_sprintf(  s, n_posix_literal( a ), ##__VA_ARGS__ )




#define n_posix_strlen_literal( a ) n_posix_strlen( n_posix_literal( a ) ) 

n_type_int
n_posix_strlen( const n_posix_char *str )
{

	// [x] : strlen() and wcslen() support size_t only

/*
	// [Needed] : VC++ 2017

#ifdef UNICODE

	if ( ( str != NULL )&&( str[ 0 ] != n_posix_literal( '\0' ) ) ) { return wcslen( str ); }

#else  // #ifdef UNICODE

	if ( ( str != NULL )&&( str[ 0 ] != n_posix_literal( '\0' ) ) ) { return strlen( str ); }

#endif // #ifdef UNICODE

	return 0;
*/

	if ( str == NULL ) { return 0; }

	n_type_int i = 0;
	n_posix_loop
	{
		if ( str[ i ] ==  n_posix_literal( '\0' ) ) { break; }
		i++;
	}


	return i;
}




u32
n_posix_tickcount( void )
{

	u32 ret;


#ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_TIMEGETTIME

/*
	// [x] : run on Win95 but slow

	LARGE_INTEGER li_frq; QueryPerformanceFrequency( &li_frq );
	LARGE_INTEGER li_now; QueryPerformanceCounter  ( &li_now );

	ret = (u32) ( ( 1000LL * li_now.QuadPart ) / li_frq.QuadPart );
*/

	ret = timeGetTime();

#else  // #ifdef N_POSIX_TIMEGETTIME

	ret = GetTickCount();

#endif // #ifdef N_POSIX_TIMEGETTIME


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MAC

/*
	// [x] : gettimeofday() and clock_gettime() : unstable and useless

	struct timespec ts;

	clock_gettime( CLOCK_MONOTONIC, &ts );

	ret = (u32) ts.tv_nsec / 1000 / 1000;
*/

	ret = (u32) trunc( CACurrentMediaTime() * 1000 );


#else  // #infdef N_POSIX_PLATFORM_MAC


	struct timeval tv;

	gettimeofday( &tv, NULL );

	ret = tv.tv_usec / 1000;


#endif // #infdef N_POSIX_PLATFORM_MAC


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

wchar_t*
n_posix_ansi2unicode( const char *str )
{

	// [!] : you need to n_memory_free() a returned variable


	if ( str == NULL ) { str = ""; }

	n_type_int  byte = strlen( str ) * sizeof( char );
	wchar_t    *wstr = (wchar_t*) n_unicode_alloccopy( &byte, str );


	//n_unicode_codec_char2wchar( wstr, byte );
	//n_unicode_bom_remove( wstr, byte );


	n_unicode_codec_char2wchar_no_bom( wstr, byte );


	return wstr;
}

char*
n_posix_unicode2ansi( const wchar_t *wstr )
{

	// [!] : you need to n_memory_free() a returned variable


	if ( wstr == NULL ) { wstr = L""; }

	n_type_int  byte = wcslen( wstr ) * sizeof( wchar_t );
	char       *ansi = (char*) n_unicode_alloccopy( &byte, wstr );


	//n_unicode_bom_restore_utf16_le( ansi, byte );
	//n_unicode_codec_wchar2char( ansi, byte );


	n_unicode_codec_wchar2char_no_bom( ansi, byte );


	return ansi;
}

#define n_posix_debug_literal( a, ... ) n_posix_debug( n_posix_literal( a ), ##__VA_ARGS__ )

void
n_posix_debug( const n_posix_char *format, ... )
{

	n_posix_char str[ 1024 ];


	va_list vl;


	va_start( vl, format );

	n_posix_vsprintf( str, format, vl );

	va_end( vl );


#ifdef N_POSIX_PLATFORM_WINDOWS


	MessageBox( NULL, str, n_posix_literal( "DEBUG" ), MB_TOPMOST | MB_OK );


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_printf_literal( "%s", str );


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

#ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef UNICODE

#ifdef _WIN64
#define n_posix_stat_main         _wstat64
#define n_posix_structstat        struct __stat64
#define n_posix_structstat_size_t __int64
#define n_posix_structstat_time_t __time64_t
#else  // #ifdef _WIN64
#define n_posix_stat_main         _wstat
#define n_posix_structstat        struct _stat
#define n_posix_structstat_size_t size_t
#define n_posix_structstat_time_t time_t
#endif // #ifdef _WIN64

#else  // #ifdef UNICODE

#define n_posix_stat_main         stat
#define n_posix_structstat        struct stat
#define n_posix_structstat_size_t size_t
#define n_posix_structstat_time_t time_t

#endif // #ifdef UNICODE


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


#define n_posix_stat_main         stat
#define n_posix_structstat        struct stat
#define n_posix_structstat_size_t off_t
#define n_posix_structstat_time_t time_t


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

int
n_posix_stat_vc_patch( const n_posix_char *name, n_posix_structstat *st, int ret )
{

#ifdef _MSC_VER

	if ( ret )
	{

		WIN32_FIND_DATA f; ZeroMemory( &f, sizeof( WIN32_FIND_DATA ) );

		FindClose( FindFirstFile( name, &f ) );
//n_posix_debug_literal( "%s\n%s", name, f.cFileName );

		if ( f.cFileName[ 0 ] != n_posix_literal( '\0' ) )
		{
			ret = 0;

#ifdef _WIN64
			st->st_size = (n_posix_structstat_size_t) f.nFileSizeLow;
			st->st_size = st->st_size | ( (n_posix_structstat_size_t) f.nFileSizeHigh << 32 );
#else  // #ifdef _WIN64
			if ( f.nFileSizeHigh == 0 )
			{
				st->st_size = f.nFileSizeLow;
			} else {
				st->st_size = 0;
			}
#endif // #ifdef _WIN64
		}

	}

	if ( ret == 0 )
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

			DWORD dw = GetFileAttributes( name );
//n_posix_debug_literal( "%s : %x : %d", name_new, dw, GetLastError() );

			// [Patch] : "C:\\hyberfil.sys" and others are set 0xffffffff
			//
			//	not exist        : 0xffffffff
			//	hidden or locked : 0xffffffff

			ret = 0;

			if ( dw == 0 )
			{
				ret = -1;
			} else
			if ( dw == 0xffffffff )
			{
				st->st_mode |= S_IFREG;
			} else
			if ( dw & FILE_ATTRIBUTE_DIRECTORY )
			{
				st->st_mode |= S_IFDIR;
			} else {
				st->st_mode |= S_IFREG;
			}

		}

		CloseHandle( h );

	}

#endif // #ifdef _MSC_VER


	return ret;
}

static n_posix_bool n_posix_stat_safemode = n_posix_false;

int
n_posix_stat( const n_posix_char *name_arg, n_posix_structstat *st )
{

	if ( n_posix_stat_safemode )
	{
		if ( st != NULL ) { n_memory_zero( st, sizeof( n_posix_structstat ) ); }
	}


	if ( name_arg == NULL ) { return 1; }

	if ( name_arg[ 0 ] == n_posix_literal( '\0' ) ) { return 1; }


	int ret = 0;


#ifdef N_POSIX_PLATFORM_WINDOWS


	// [!] : Windows : drives : a name needs to be like "C:\", "C:" is not acceptable

	n_posix_char *name = NULL;

	if (
		( ( name_arg[ 1 ] != n_posix_literal( '\0' ) )&&( name_arg[ 1 ] == n_posix_literal( ':' ) ) )
		&&
		( name_arg[ 2 ] == n_posix_literal( '\0' ) )
	)
	{
		name = (n_posix_char*) n_memory_new( 4 * sizeof( n_posix_char ) );
		n_posix_sprintf_literal( name, "%s%s", name_arg, N_POSIX_SLASH );
	}


	// [!] : VC++ 2017 : locked or hidden files
	//
	//	  ret will be -1
	//	errno will be  2

	if ( name == NULL )
	{
		ret = n_posix_stat_main( name_arg, st );
	} else {
		ret = n_posix_stat_main( name    , st );
	}


	if ( name == NULL )
	{
		ret = n_posix_stat_vc_patch( name_arg, st, ret );
	} else {
		ret = n_posix_stat_vc_patch( name    , st, ret );
	}


	if ( name != NULL ) { n_memory_free( name ); }


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef UNICODE

	// [!] : Linux : not support unicode api

	ret = 1;

#else  // #ifdef UNICODE

	ret = lstat( name_arg, st );

#endif // #ifdef UNICODE


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

#define N_POSIX_STAT_TYPE_NOTEXIST  0
#define N_POSIX_STAT_TYPE_FILE      1
#define N_POSIX_STAT_TYPE_DIRECTORY 2

#define n_posix_stat_is_exist( f ) (                                n_posix_stat_type( f ) )
#define n_posix_stat_is_file(  f ) ( N_POSIX_STAT_TYPE_FILE      == n_posix_stat_type( f ) )
#define n_posix_stat_is_dir(   f ) ( N_POSIX_STAT_TYPE_DIRECTORY == n_posix_stat_type( f ) )

#ifndef S_ISDIR

#define S_ISDIR( mode )  ( ( (mode) & S_IFMT ) == S_IFDIR )

#endif // #ifndef S_ISDIR

int
n_posix_stat_type( const n_posix_char *path )
{

	if ( ( path == NULL )||( path[ 0 ] == n_posix_literal( '\0' ) ) )
	{
		return N_POSIX_STAT_TYPE_NOTEXIST;
	}

#ifdef N_POSIX_PLATFORM_WINDOWS


	// [Patch] : "C:\\hyberfil.sys" and others are set 0xffffffff
	//
	//	not exist        : 0xffffffff
	//	hidden or locked : 0xffffffff

	DWORD dw = GetFileAttributes( path );
	DWORD er = GetLastError();
//n_posix_debug_literal( "%x : %d", dw, er );

	int ret;

	if ( dw == 0xffffffff )
	{
		if ( er == ERROR_SHARING_VIOLATION )
		{
			ret = N_POSIX_STAT_TYPE_FILE;
		} else {
			ret = N_POSIX_STAT_TYPE_NOTEXIST;
		}
	} else
	if ( dw & FILE_ATTRIBUTE_DIRECTORY )
	{
		ret = N_POSIX_STAT_TYPE_DIRECTORY;
	} else {
		ret = N_POSIX_STAT_TYPE_FILE;
	}


	return ret;


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_structstat st;

	if ( 0 != n_posix_stat( path, &st ) )
	{
		return N_POSIX_STAT_TYPE_NOTEXIST;
	}

	if ( S_ISDIR( st.st_mode ) )
	{
		return N_POSIX_STAT_TYPE_DIRECTORY;
	}

	return N_POSIX_STAT_TYPE_FILE;


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
}

n_posix_bool
n_posix_stat_size_is_overflowed( const n_posix_char *name )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	WIN32_FIND_DATA f; FindClose( FindFirstFile( name, &f ) );

	if ( f.cFileName[ 0 ] != n_posix_literal( '\0' ) )
	{
#ifdef _WIN64
		//
#else  // #ifdef _WIN64
		if ( f.nFileSizeHigh != 0 )
		{
			return n_posix_true;
		}
#endif // #ifdef _WIN64
	}
#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return n_posix_false;
}

n_posix_structstat_size_t
n_posix_stat_size( const n_posix_char *name )
{

	// [Needed] : Win2000 : crash prevention

	if ( name == NULL ) { return 0; }
	if ( name[ 0 ] == n_posix_literal( '\0' ) ) { return 0; }


#ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_structstat_size_t ret = 0;


	WIN32_FIND_DATA f; FindClose( FindFirstFile( name, &f ) );

	if ( f.cFileName[ 0 ] != n_posix_literal( '\0' ) )
	{
#ifdef _WIN64
		ret = (n_posix_structstat_size_t) f.nFileSizeLow;
		ret = ret | ( (n_posix_structstat_size_t) f.nFileSizeHigh << 32 );
#else  // #ifdef _WIN64
		if ( f.nFileSizeHigh == 0 )
		{
			ret = f.nFileSizeLow;
		}
#endif // #ifdef _WIN64
	}


	return ret;

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_structstat st;

	n_posix_stat( name, &st );

	return st.st_size;

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
}

n_posix_structstat_time_t
n_posix_stat_atime( const n_posix_char *name )
{

	n_posix_structstat st;


	n_posix_stat( name, &st );


	return st.st_atime;
}

n_posix_structstat_time_t
n_posix_stat_ctime( const n_posix_char *name )
{

	n_posix_structstat st;


	n_posix_stat( name, &st );


	return st.st_ctime;
}

n_posix_structstat_time_t
n_posix_stat_mtime( const n_posix_char *name )
{

	n_posix_structstat st;


	n_posix_stat( name, &st );


	return st.st_mtime;
}




#ifdef N_POSIX_PLATFORM_WINDOWS

typedef struct {

	DWORD    dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD    nFileSizeHigh;
	DWORD    nFileSizeLow;
#ifdef _WIN32_WCE
	DWORD    dwOID; 
#else
	DWORD    dwReserved0;
	DWORD    dwReserved1;
#endif
	CHAR     d_name[ MAX_PATH ];
#ifndef _WIN32_WCE
	CHAR     cAlternateFileName[ 14 ];
#endif

} n_posix_dirent_A;

typedef struct {

	DWORD    dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD    nFileSizeHigh;
	DWORD    nFileSizeLow;
#ifdef _WIN32_WCE
	DWORD    dwOID; 
#else
	DWORD    dwReserved0;
	DWORD    dwReserved1;
#endif
	WCHAR    d_name[ MAX_PATH ];
#ifndef _WIN32_WCE
	WCHAR    cAlternateFileName[ 14 ];
#endif

} n_posix_dirent_W;

#ifdef UNICODE

#define n_posix_dirent n_posix_dirent_W

#else  // #ifdef UNICODE

#define n_posix_dirent n_posix_dirent_A

#endif // #ifdef UNICODE


typedef struct {

	HANDLE         handle;
	n_posix_dirent dirent;
	n_posix_bool   is_first;

} n_posix_DIR;

n_posix_DIR*
n_posix_opendir( const n_posix_char *folder_name )
{

	if ( n_posix_false == n_posix_stat_is_dir( folder_name ) ) { return NULL; }


	n_posix_DIR *dir = (n_posix_DIR*) n_memory_new( sizeof( n_posix_DIR ) );

	// [!] : 4 == n_posix_strlen_literal( "\\*.*" );

	n_type_int cch = n_posix_strlen( folder_name ) + 4 + 1;
	n_type_int cb  = cch * sizeof( n_posix_char );

	n_posix_char *name = (n_posix_char*) n_memory_new( cb );

	if (
		( folder_name[ 1 ] == n_posix_literal( ':'  ) )
		&&
		( folder_name[ 2 ] == n_posix_literal( '\\' ) )
		&&
		( folder_name[ 3 ] == n_posix_literal( '\0' ) )
	)
	{
		n_posix_sprintf_literal( name, "%s*.*",   folder_name );
	} else {
		n_posix_sprintf_literal( name, "%s\\*.*", folder_name );
	}
//n_posix_debug_literal( "%s", name );


	dir->handle = FindFirstFile( name, (WIN32_FIND_DATA*) &dir->dirent );


	dir->is_first = n_posix_true;


	n_memory_free( name );


	return dir;
}

void
n_posix_closedir( n_posix_DIR *dir )
{

	if ( dir == NULL ) { return; }


	FindClose( dir->handle );

	n_memory_free( dir );


	return;
}

n_posix_dirent*
n_posix_readdir( n_posix_DIR *dir )
{

	if ( dir == NULL ) { return NULL; }


	if ( dir->is_first )
	{

		dir->is_first = n_posix_false;

	} else {

		n_posix_bool ret = FindNextFile( dir->handle, (WIN32_FIND_DATA*) &dir->dirent );
		if ( ret == n_posix_false ) { return NULL; }
		//if ( ERROR_NO_MORE_FILES == GetLastError() ) { return NULL; }

	}


	return &dir->dirent;
}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

n_posix_bool
n_posix_is_drivename( const n_posix_char *path )
{

	if ( path == NULL ) { return n_posix_false; }

	if ( path[ 0 ] == n_posix_literal( '\0' ) ) { return n_posix_false; }

#ifdef N_POSIX_PLATFORM_WINDOWS

	// [!] : don't use n_posix_strlen() for performance

	if (
		(
			( path[ 0 ] != n_posix_literal( '\0' ) )
			&&
			(
				( ( path[ 0 ] >= n_posix_literal( 'A' ) )&&( path[ 0 ] <= n_posix_literal( 'Z' ) ) )
				||
				( ( path[ 0 ] >= n_posix_literal( 'a' ) )&&( path[ 0 ] <= n_posix_literal( 'z' ) ) )
			)
		)
		&&
		(
			( path[ 1 ] != n_posix_literal( '\0' ) )
			&&
			( path[ 1 ] == n_posix_literal( ':' ) )
		)
		&&
		(
			( path[ 2 ] == n_posix_literal( '\0' ) )
			||
			(
				( ( path[ 2 ] == n_posix_literal( '\\' ) )||( path[ 2 ] == n_posix_literal( '/' ) ) )
				&&
				( path[ 3 ] == n_posix_literal( '\0' ) )
			)
		)
	)
	{
		return n_posix_true;
	}

#endif


	return n_posix_false;
}

n_posix_DIR*
n_posix_opendir_nodot( const n_posix_char *folder_name )
{

	n_posix_DIR *dir = n_posix_opendir( folder_name );
	if ( dir != NULL )
	{

		// [!] : skip "." and ".."

		if ( n_posix_is_drivename( folder_name ) )
		{
			//
		} else {
			n_posix_readdir( dir );
			n_posix_readdir( dir );
		}

	}


	return dir;
}
/*
n_posix_dirent*
n_posix_readdir_nodot( n_posix_DIR *dir )
{

	// [!] : n_posix_opendir_nodot() is 2x faster


	n_posix_dirent *dirent;


	n_posix_loop
	{

		dirent = n_posix_readdir( dir );
		if ( dirent == NULL ) { break; }

		if (
			( 0 != n_posix_strcmp( dirent->d_name, n_posix_literal( "."  ) ) )
			&&
			( 0 != n_posix_strcmp( dirent->d_name, n_posix_literal( ".." ) ) )
		)
		{
			break;
		}

	}


	return dirent;
}
*/
n_type_int
n_posix_folder_count( const n_posix_char *path )
{

	n_posix_DIR *dp = n_posix_opendir_nodot( path );
	if ( dp == NULL ) { return 0; }


	n_type_int ret = 0;
	n_posix_loop
	{

		n_posix_dirent *dirent = n_posix_readdir( dp );
		if ( dirent == NULL ) { break; }

		ret++;

	}

	n_posix_closedir( dp );


	return ret;
}

n_posix_bool
n_posix_access_is_denied( const n_posix_char *path )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	DWORD dw = GetFileAttributes( path );
	if ( dw & FILE_ATTRIBUTE_REPARSE_POINT )
	{
		return n_posix_true;
	}


	// [Needed] : VC++

	dw = 0;
	GetFileSecurity( path, OWNER_SECURITY_INFORMATION, 0, 0, &dw );

	// [!] : Win9x : ERROR_CALL_NOT_IMPLEMENTED will be returned

	dw = GetLastError();
	if (
		( dw != ERROR_CALL_NOT_IMPLEMENTED )
		&&
		( dw != ERROR_INSUFFICIENT_BUFFER  )
	)
	{
		return n_posix_true;
	}

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MAC

	// [!] : stub

#else  // #ifdef N_POSIX_PLATFORM_MAC

	n_posix_access( path, 0 );

	if ( ( errno == EACCES )&&( 0 == n_posix_folder_count( path ) ) )
	{
		return n_posix_true;
	}

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return n_posix_false;
}




#define n_posix_fopen_read(   f ) n_posix_fopen( f, n_posix_literal( "rb" ) )
#define n_posix_fopen_write(  f ) n_posix_fopen( f, n_posix_literal( "wb" ) )
#define n_posix_fopen_append( f ) n_posix_fopen( f, n_posix_literal( "ab" ) )

FILE*
n_posix_fopen( const n_posix_char *name, const n_posix_char *mode )
{

	// [!] : fail-safe
	//
	//	a : "name" is NULL
	//	b : "name" includes invalid characters like "*"
	//	c : "name" is a folder
	//
	//	"not exist" is acceptable

	if ( n_posix_stat_is_dir( name ) ) { return NULL; }


#ifdef N_POSIX_PLATFORM_WINDOWS

	// [!] : for prevent integer overflow

	if ( 0 == n_posix_strcmp( mode, n_posix_literal( "rb" ) ) )
	{

		s64 size = -1;

		HANDLE h = CreateFile
		(
			name,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		if ( h != INVALID_HANDLE_VALUE )
		{

			// [!] : Win95 hasn't GetFileSizeEx()
	
			//LARGE_INTEGER li;
			//GetFileSizeEx( h, &li );
			//size = li.QuadPart

			DWORD dw_hi = 0;
			DWORD dw_lo = GetFileSize( h, &dw_hi );

			size = ( (s64) dw_hi << 32 ) + ( (s64) dw_lo );

		}

		CloseHandle( h );

//n_posix_debug_literal( "%lld", size );

		// [!] : ptr[ INT_MAX - 1 ] : reserved for '\0' 

		if ( ( size == -1 )||( size >= INT_MAX ) ) { return NULL; }

	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

 
#ifdef UNICODE

	FILE *fp = _wfopen( name, mode );

#else  // #ifdef UNICODE

	FILE *fp = fopen( name, mode );

#endif // #ifdef UNICODE


	return fp;
}

n_type_int 
n_posix_fread( void *ptr, n_type_int byte, n_type_int unit, FILE *fp )
{

	if ( fp == NULL ) { return 0; }


	// [!] : GCC : compile error will occur


	n_type_int ret_byte = fread( ptr, byte, unit, fp );


	//if ( ret_byte > ( byte * unit ) ) { n_posix_debug_literal( "!" ); }


	return ret_byte;
}

n_type_int 
n_posix_fwrite( const void *ptr, n_type_int byte, n_type_int unit, FILE *fp )
{

	if ( fp  == NULL ) { return 0; }
	if ( ptr == NULL ) { return 0; }


	return fwrite( ptr, byte, unit, fp );
}

int
n_posix_fclose( FILE *fp )
{

	if ( fp == NULL ) { return EOF; }


	return fclose( fp );
}

#define n_posix_fseek fseek

void
n_posix_sleep( u32 millisec )
{

#ifdef N_POSIX_PLATFORM_WINDOWS


	Sleep( millisec );


#else // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MAC

	usleep( millisec * 1000 );

#else  // #ifdef N_POSIX_PLATFORM_MAC

	struct timespec ts;

	ts.tv_sec  = 0;
	ts.tv_nsec = millisec * 1000 * 1000;

	nanosleep( &ts, NULL );

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

#define n_posix_dump_literal( f, p, s ) n_posix_dump( n_posix_literal( f ), p, s )

void
n_posix_dump( n_posix_char *filename, void *ptr, n_type_int size )
{

	FILE *fp = n_posix_fopen_write( filename );
	if ( fp == NULL ) { return; }

	n_posix_fwrite( ptr, size, 1, fp );

	n_posix_fclose( fp );


	return;
}

u32
n_posix_cpu_count( void )
{

	u32 ret = 1;


#ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MINGW

	HMODULE hmod = LoadLibrary( n_posix_literal( "kernel32.dll" ) );
	if ( hmod == NULL ) { return ret; }


	// [!] : Hyper-Threading : 5% faster but bad performance sometimes

	FARPROC func = NULL;//GetProcAddress( hmod, "GetLogicalProcessorInformation" );

	if ( func != NULL )
	{
		typedef enum
		{

			n_RelationProcessorCore,
			n_RelationNumaNode,
			n_RelationCache,
			n_RelationProcessorPackage,
			n_RelationGroup,
			n_RelationAll

		} n_LOGICAL_PROCESSOR_RELATIONSHIP;

		typedef enum
		{

			n_CacheUnified,
			n_CacheInstruction,
			n_CacheData,
			n_CacheTrace

		} n_PROCESSOR_CACHE_TYPE;

		typedef struct
		{

			BYTE                   Level;
			BYTE                   Associativity;
			WORD                   LineSize;
			DWORD                  Size;
			n_PROCESSOR_CACHE_TYPE Type;

		} n_CACHE_DESCRIPTOR;

		typedef struct
		{

			ULONG_PTR                        ProcessorMask;
			n_LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
			union
			{
				struct
				{
					BYTE Flags;
				} ProcessorCore;

				struct
				{
					DWORD NodeNumber;
				} NumaNode;

				n_CACHE_DESCRIPTOR Cache;
				ULONGLONG          Reserved[ 2 ];

			} DUMMYUNIONNAME;

		} n_SYSTEM_LOGICAL_PROCESSOR_INFORMATION;

		n_SYSTEM_LOGICAL_PROCESSOR_INFORMATION *p = NULL;
		DWORD                                   b = 0;

		n_posix_loop
		{
			DWORD ret = (DWORD) (*func)( p, &b );
			if ( ret ) { break; }

			n_memory_free( p );
			p = (n_SYSTEM_LOGICAL_PROCESSOR_INFORMATION*) n_memory_new( b );
		}

		ret = 0;

		DWORD c = b / sizeof( n_SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
		DWORD i = 0;
		n_posix_loop
		{
			if ( p[ i ].Relationship == n_RelationProcessorCore )
			{
				ret++;
			}

			i++;
			if ( i >= c ) { break; }
		}

		n_memory_free( p );

		if ( ret == 0 ) { ret = 1; }

	} else {

		// [!] : Hyper-Threading(HT) logical cores will be included

		func = GetProcAddress( hmod, "GetSystemInfo" );

		if ( func != NULL )
		{
			SYSTEM_INFO system_info; memset( &system_info, 0, sizeof( SYSTEM_INFO ) );
			(*func)( &system_info );
			ret = system_info.dwNumberOfProcessors;
		}

	}


	FreeLibrary( hmod );


#else  // #ifdef N_POSIX_PLATFORM_MINGW

	SYSTEM_INFO system_info; memset( &system_info, 0, sizeof( SYSTEM_INFO ) );

	GetSystemInfo( &system_info );

	ret = system_info.dwNumberOfProcessors;

#endif // #ifdef N_POSIX_PLATFORM_MINGW


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


#ifdef N_POSIX_PLATFORM_MAC

	ret = (u32) [[NSProcessInfo processInfo] processorCount];
//NSLog( @"%d", ret );

#endif // #ifdef N_POSIX_PLATFORM_MAC


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

#ifdef N_POSIX_PLATFORM_MINGW

#include "../win32/sysinfo/version.c"

#define n_posix_thread_is_available() n_sysinfo_version_2000_or_later()

#else

#define n_posix_thread_is_available() TRUE

#endif




#define n_posix_strftime_literal( a,b,c,d ) n_posix_strftime( a, b, n_posix_literal( c ), d )

s64
n_posix_strftime( n_posix_char *str, n_type_int cch, const n_posix_char *format, const struct tm *timeinfo )
{

#ifdef UNICODE

	return wcsftime( str, cch, format, timeinfo );

#else  // #ifdef UNICODE

	return strftime( str, cch, format, timeinfo );

#endif // #ifdef UNICODE

}

u32
n_posix_crc32( u8 *data, u32 len )
{

	u32 crc = 0xffffffffL;
	u32 table[ 256 ];
	u32 i, ii, c;


 	i = ii = c = 0;
	n_posix_loop
	{

		if ( c & 1 )
		{
			c = 0xedb88320L ^ ( c >> 1 );
		} else {
			c = c >> 1;
		}


		ii++;
		if ( ii >= 8 )
		{

			table[ i ] = c;

			i++;
			if ( i >= 256 ) { break; }

			c  = i;
			ii = 0;
		}
	}


 	i = 0;
	n_posix_loop
	{

		crc = table[ ( crc ^ data[ i ] ) & 0xff ] ^ ( crc >> 8 );

		i++;
		if ( i >= len ) { break; }
	}


	return crc ^ 0xffffffffL;
}

int
n_posix_math_gcd( int a, int b )
{

	// Greatest Common Divisor

	while( b != 0 )
	{
		int temp = b;
		       b = a % b;
		       a = temp;
	}

	return a;
}

int
n_posix_math_lcm( int a, int b )
{

	// Least Common Multiple

	if ( ( a == 0 )||( b == 0 ) ) { return 0; }

	return abs( a / n_posix_math_gcd( a, b ) * b );
}


#endif // _H_NONNON_NEUTRAL_POSIX

