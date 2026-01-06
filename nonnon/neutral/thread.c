// Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Needed] : Link : -mthreads
//
//	if you don't use this, it seems no problem
//	but POSIX function misbehaves
//
//	Win2000/XP : folder count will be zero


// [!] : currently, process divider pattern only
//
//	don't use the worker pattern
//	a thread will hang up at closing from a main thread sometimes
//
//	WaitForSingleObject() may have some bug
//	crash when not using INFINITE, but this causes hang-up




#ifndef _H_NONNON_NEUTRAL_THREAD
#define _H_NONNON_NEUTRAL_THREAD




#include "./posix.c"


#ifndef N_POSIX_PLATFORM_MAC

#include <process.h>

#endif // #ifndef N_POSIX_PLATFORM_MAC




static u32          n_thread_core_count   = 1;
static n_posix_bool n_thread_is_available = n_posix_false;




#ifdef N_POSIX_PLATFORM_WINDOWS




#include "../win32/sysinfo/version.c"




n_posix_bool
n_thread_onoff( void )
{

	static n_posix_bool ret = n_posix_false;
//return ret;

	static n_posix_bool is_first = n_posix_true;

	if ( is_first )
	{
		is_first = n_posix_false;

		n_thread_core_count   = n_posix_cpu_count();
		n_thread_is_available = n_sysinfo_version_2000_or_later();

		ret = ( ( n_thread_is_available )&&( n_thread_core_count > 1 ) );
	}


	return ret;
}




#define n_thread               HANDLE

#define n_thread_argument      void*
#define n_thread_return        unsigned __stdcall
#define n_thread_return_worker void     __cdecl


#ifdef _MSC_VER

#define n_thread_func _beginthreadex_proc_type

#else  // #ifdef _MSC_VER

#define n_thread_func void*

#endif // #ifdef _MSC_VER




n_thread
n_thread_init( n_thread_func func, void *data )
{
	return (n_thread) _beginthreadex( 0, 0, func, data, 0, NULL );
}

void
n_thread_wait( n_thread thread )
{

	if ( thread == NULL ) { return; }

	WaitForSingleObject( thread, INFINITE );

	return;
}

void
n_thread_exit( n_thread thread )
{

	if ( thread == NULL ) { return; }

	CloseHandle( thread );

	return;
}




#else  // #ifdef N_POSIX_PLATFORM_WINDOWS




#include <pthread.h>




n_posix_bool
n_thread_onoff( void )
{

	static n_posix_bool ret = n_posix_false;


	static n_posix_bool is_first = n_posix_true;

	if ( is_first )
	{
		is_first = n_posix_false;

#ifdef N_POSIX_PLATFORM_MAC

		n_thread_core_count   = n_posix_cpu_count();
		n_thread_is_available = n_posix_true;

#else  // #ifdef N_POSIX_PLATFORM_MAC

		n_thread_core_count   = n_posix_cpu_count();
		n_thread_is_available = n_posix_false;

#endif // #ifdef N_POSIX_PLATFORM_MAC

		ret = ( ( n_thread_is_available )&&( n_thread_core_count > 1 ) );
	}


	return ret;
}




#define n_thread          pthread_t

#define n_thread_argument void*
#define n_thread_return   void*




n_thread
n_thread_init( void *func, void *data )
{

	n_thread thread;


	pthread_create( &thread, NULL, func, data );


	return thread;
}

void
n_thread_wait( n_thread thread )
{

	if ( thread == NULL ) { return; }

	pthread_join( thread, NULL );


	return;
}

void
n_thread_exit( n_thread thread )
{
	return;
}




#endif // #ifdef N_POSIX_PLATFORM_WINDOWS



/*
void
n_thread_sync( n_thread thread, n_posix_bool *onoff )
{

	// [ Mechanism ]
	//
	//	implement "onoff" checker in loop of a thread
	//	set "onoff" n_posix_true
	//	call this


	if ( thread == NULL ) { return; }
	if ( onoff  == NULL ) { return; }


	// [x] : hang-up when wait forever, crash when using timeout


	u32 timeout = n_posix_tickcount() + 1000;
	n_posix_loop
	{
		if ( (*onoff) == n_posix_false ) { break; } else { n_posix_sleep( 1 ); }

		// [x] : hang-up here

		if ( timeout < n_posix_tickcount() )
		{
			timeout = n_posix_tickcount() + 1000;

			if ( thread == NULL ) { break; }

			DWORD dw;

			dw = 0;
			if ( 0 == GetExitCodeThread( thread, &dw ) ) { break; }
			if ( dw != STILL_ACTIVE ) { break; }

			(*onoff) = n_posix_true;
			dw = ResumeThread( thread );
			if ( dw == (DWORD) -1 ) { break; }
		}

	}


	return;
}
*/



/*
// [x] : this cannot prevent hang-up

void
n_thread_sync_event_main( n_thread thread, const n_posix_char *str_namespace )
{

	// [ Mechanism ] : call this in a main thread


	if ( thread == NULL ) { return; }


	HANDLE h_event = CreateEvent( NULL, TRUE, FALSE, str_namespace );

	SetEvent( h_event );
	WaitForSingleObject( h_event, INFINITE );

	CloseHandle( h_event );


	return;
}

n_posix_bool
n_thread_sync_event_sub_is_closed( n_thread thread, const n_posix_char *str_namespace, u32 wait_msec )
{

	// [ Mechanism ] : call this in a made thread


	HANDLE h_event = OpenEvent( EVENT_ALL_ACCESS, n_posix_false, str_namespace );

	n_posix_bool ret = ( WaitForSingleObject( h_event, wait_msec ) == WAIT_OBJECT_0 );

	CloseHandle( h_event );


	return ret;
}
*/




#endif // _H_NONNON_NEUTRAL_THREAD


