// Nonnon Game
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_RANDOM
#define _H_NONNON_NEUTRAL_RANDOM




#include "./posix.c"




// [!] : for scalability

#define n_srand( seed ) srand( seed )
#define n_rand(       ) rand()




void
n_random_shuffle( void )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	n_srand( (int) n_posix_tickcount() );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_srand( (unsigned int) time( NULL ) );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	return;
}

u32
n_random_range( u32 n )
{

	if ( n == 0 ) { return 0; }

	return ( n_rand() % n );
}

#define n_random_range_hq n_random_range_high_quality

u32
n_random_range_high_quality( u32 n )
{

	// [!] : 3x slower : slow but precise


	double d;


	d = (double) n_rand() / RAND_MAX;
	d = (double) d * n;


	// [!] : clamp is needed

	if ( d >= n ) { d = n - 1; }


	return (u32) d;
}


#endif // _H_NONNON_NEUTRAL_RANDOM

