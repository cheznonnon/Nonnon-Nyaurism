// Nonnon Date And Time Helper Functions
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_TIME
#define _H_NONNON_NEUTRAL_TIME




#include "./posix.c"




#ifdef N_POSIX_PLATFORM_WINDOWS

#define n_time FILETIME

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

#define n_time time_t

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS




#define N_TIME_ACCESS 0
#define N_TIME_CREATE 1
#define N_TIME_MODIFY 2
#define N_TIME_SYSTEM 3




#define n_time_atime( name ) n_time_get( name, N_TIME_ACCESS )
#define n_time_ctime( name ) n_time_get( name, N_TIME_CREATE )
#define n_time_mtime( name ) n_time_get( name, N_TIME_MODIFY )
#define n_time_stime(      ) n_time_get( NULL, N_TIME_SYSTEM )

n_time
n_time_get( const n_posix_char *name, int type )
{

	n_time ret;
	n_memory_zero( &ret, sizeof( n_time ) );


#ifdef N_POSIX_PLATFORM_WINDOWS


	if ( type == N_TIME_SYSTEM )
	{

		SYSTEMTIME systime;
		GetLocalTime( &systime );
		SystemTimeToFileTime( &systime, &ret );

	} else {

		// [x] : time stamp will be different when copying from NTFS to FAT32
		//
		//	shifting by a two seconds
		//	file system's problem


		WIN32_FIND_DATA fd;
		FindClose( FindFirstFile( name, &fd ) );

		WORD w[ 2 ];
		FileTimeToDosDateTime( &fd.ftLastWriteTime, &w[ 0 ], &w[ 1 ] );
		DosDateTimeToFileTime( w[ 0 ], w[ 1 ], &fd.ftLastWriteTime );


		if ( type == N_TIME_ACCESS ) { ret = fd.ftLastAccessTime; } else
		if ( type == N_TIME_CREATE ) { ret = fd.ftCreationTime;   } else
		if ( type == N_TIME_MODIFY ) { ret = fd.ftLastWriteTime;  }

	}

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


	if ( type == N_TIME_ACCESS ) { ret = n_posix_stat_atime( name ); } else
	if ( type == N_TIME_CREATE ) { ret = n_posix_stat_ctime( name ); } else
	if ( type == N_TIME_MODIFY ) { ret = n_posix_stat_mtime( name ); } else
	if ( type == N_TIME_SYSTEM ) { ret =               time( NULL ); }


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

#define n_time_string_atime( name, ret ) n_time_string( name, ret, N_TIME_ACCESS )
#define n_time_string_ctime( name, ret ) n_time_string( name, ret, N_TIME_CREATE )
#define n_time_string_mtime( name, ret ) n_time_string( name, ret, N_TIME_MODIFY )
#define n_time_string_stime(       ret ) n_time_string( NULL, ret, N_TIME_SYSTEM )

void
n_time_string( const n_posix_char *name, n_posix_char *ret, int type )
{

	if ( ret == NULL ) { return; }


	n_time t;

	if ( type == N_TIME_ACCESS ) { t = n_time_atime( name ); } else
	if ( type == N_TIME_CREATE ) { t = n_time_ctime( name ); } else
	if ( type == N_TIME_MODIFY ) { t = n_time_mtime( name ); } else
	if ( type == N_TIME_SYSTEM ) { t = n_time_stime(      ); }


#ifdef N_POSIX_PLATFORM_WINDOWS


	SYSTEMTIME s;
	if ( type != N_TIME_SYSTEM ) { FileTimeToLocalFileTime( &t, &t ); }
	FileTimeToSystemTime( &t, &s );


	n_posix_char *frmt = NULL;//n_posix_literal( "HH':'mm':'ss" );

	n_posix_char date[ 255 ], time[ 255 ];
	GetDateFormat( LOCALE_USER_DEFAULT, DATE_SHORTDATE, &s, NULL, date, 255 );
	GetTimeFormat( LOCALE_USER_DEFAULT,              0, &s, frmt, time, 255 );


	n_posix_sprintf_literal( ret, "%s %s", date, time );


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_posix_sprintf_literal( ret, "%s", n_posix_ctime( &t ) );


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

#define n_time_compare_atime( f, t ) n_time_compare( f, t, N_TIME_ACCESS )
#define n_time_compare_ctime( f, t ) n_time_compare( f, t, N_TIME_CREATE )
#define n_time_compare_mtime( f, t ) n_time_compare( f, t, N_TIME_MODIFY )

int
n_time_compare( const n_posix_char *f, const n_posix_char *t, int type )
{

	// [Mechanism] : Return Value
	//
	//	-1 : f < t
	//	 0 : f = t
	//	+1 : f > t


#ifdef N_POSIX_PLATFORM_WINDOWS


	n_time nt_f;
	n_time nt_t;

	if ( type == N_TIME_ACCESS ) { nt_f = n_time_atime( f ); nt_t = n_time_atime( t ); }
	if ( type == N_TIME_CREATE ) { nt_f = n_time_ctime( f ); nt_t = n_time_ctime( t ); }
	if ( type == N_TIME_MODIFY ) { nt_f = n_time_mtime( f ); nt_t = n_time_mtime( t ); }

	int ret = CompareFileTime( &nt_f, &nt_t );


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


	n_time nt_f = 0;
	n_time nt_t = 0;

	if ( type == N_TIME_ACCESS ) { nt_f = n_time_atime( f ); nt_t = n_time_atime( t ); }
	if ( type == N_TIME_CREATE ) { nt_f = n_time_ctime( f ); nt_t = n_time_ctime( t ); }
	if ( type == N_TIME_MODIFY ) { nt_f = n_time_mtime( f ); nt_t = n_time_mtime( t ); }


#ifdef N_POSIX_PLATFORM_MAC

	// [!] : for exFAT
//NSLog( @"%s : %ld %ld", f, nt_f, nt_t );

	if ( type == N_TIME_MODIFY )
	{
		n_time div = labs( nt_f - nt_t );
		if ( div <= 2 ) { return 0; }
	}

#endif // #ifdef N_POSIX_PLATFORM_MAC


	int ret = 0;

	if ( nt_f < nt_t ) { ret = -1; } else
	if ( nt_f > nt_t ) { ret =  1; }


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return ret;
}

n_posix_bool
n_time_is_leapyear( int year )
{

	if (
		( ( 0 == ( year % 4 ) )&&( 0 != ( year % 100 ) ) )
		||
		( 0 == ( year % 400 ) )
	)
	{
		return n_posix_true;
	}


	return n_posix_false;
}

int
n_time_dayofweek( int year, int month )
{

	// [Mechanism]
	//
	//	Month    | 01 02 03 04 05 06 07 08 09 10 11 12 |
	//	Non-Leap |  3  0  3  2  3  2  3  3  2  3  2  3 |
	//	Leap     |  3  1  3  2  3  2  3  3  2  3  2  3 |
	//
	//	Non-Leap | ( 3 * 7 ) + ( 2 * 4 ) + ( 0 * 1 ) = 29 % 7 = 1
	//	Leap     | ( 3 * 7 ) + ( 2 * 4 ) + ( 1 * 1 ) = 30 % 7 = 2
	//
	//	1899 January 1st is Sunday



	const int year_base = 1899;

	if ( year < year_base ) { return 0; }

	if ( ( month <= 0 )||( month > 12 ) ) { return 0; }


	int week_offset = 0;


	// Phase 1 : Year Offset

	{

		int year_delta = year - year_base;

		int y = 0;
		n_posix_loop
		{

			if ( y >= year_delta ) { break; }

			week_offset += 1 + n_time_is_leapyear( year_base + y );

			y++;

		}
//n_posix_debug_literal( "%d", week_offset % 7 );

	}


	// Phase 2 : Month Offset

	{

		int leap         = n_time_is_leapyear( year );
		int offset[ 12 ] = { 3, leap, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3 };

//n_posix_debug_literal( "%d", n_time_is_leapyear( year ) );

		int m = 1;
		n_posix_loop
		{

			if ( m >= month ) { break; }

			week_offset += offset[ m - 1 ];

			m++;

		}

	}


	return ( week_offset % 7 );
}

int
n_time_lastday( int year, int month )
{

	int lastday = 31;


	if ( month == 2 )
	{

		lastday = 28 + n_time_is_leapyear( year );

	} else
	if (
		( ( month <= 7 )&&( 0 == ( month % 2 ) ) )
		||
		( ( month >= 8 )&&( 0 != ( month % 2 ) ) )
	)
	{
		lastday = 30;
	}


	return lastday;
}

// internal
void
n_time_today( int *year, int *month, int *day, int *hour, int *minute, int *second )
{

	// [Mechanism] : same as daily life
	//
	//	year  : 2010
	//	month : 1 => 12
	//	day   : 1 => 31
	//	hour  : 0 => 23
	//	min   : 0 => 59
	//	sec   : 0 => 59


#ifdef N_POSIX_PLATFORM_WINDOWS


	SYSTEMTIME systime;
	GetLocalTime( &systime );


	if ( year   != NULL ) { *year   = systime.wYear;   }
	if ( month  != NULL ) { *month  = systime.wMonth;  }
	if ( day    != NULL ) { *day    = systime.wDay;    }
	if ( hour   != NULL ) { *hour   = systime.wHour;   }
	if ( minute != NULL ) { *minute = systime.wMinute; }
	if ( second != NULL ) { *second = systime.wSecond; }


#else  // #ifdef N_POSIX_PLATFORM_WINDOWS


	// [!] : localtime() returns NULL when the system timer doesn't function
	//
	//	I have found this problem when a motherboard's battery is empty


	struct tm tm_failsafe = { 0,0,0,1,0, 0,0,0,0 };

	time_t     t  = time( NULL );
	struct tm *tm = localtime( &t );

	if ( tm == NULL ) { tm = &tm_failsafe; }


	tm->tm_year += 1900;
	tm->tm_mon  += 1;

	if ( year   != NULL ) { *year   = tm->tm_year; }
	if ( month  != NULL ) { *month  = tm->tm_mon;  }
	if ( day    != NULL ) { *day    = tm->tm_mday; }
	if ( hour   != NULL ) { *hour   = tm->tm_hour; }
	if ( minute != NULL ) { *minute = tm->tm_min;  }
	if ( second != NULL ) { *second = tm->tm_sec;  }


#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}


#endif // _H_NONNON_NEUTRAL_TIME

