// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : don't use n_memory_new()/free()
//
//	N_MEMORY_DEBUG will not work by atexit()


// [!] : memo
//
//	maximum 40% faster when well aligned
//
//	to white, step is 0.1 , gamma_0_1 to 0_9
//	to black, step is vary, gamma_1_1 to 1_9, a little confused




#ifndef _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA
#define _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA




static n_posix_bool n_bmp_table_gamma_onoff = n_posix_false;


static n_type_real *n_bmp_table_gamma_0_1 = NULL;
static n_type_real *n_bmp_table_gamma_0_2 = NULL;
static n_type_real *n_bmp_table_gamma_0_3 = NULL;
static n_type_real *n_bmp_table_gamma_0_4 = NULL;
static n_type_real *n_bmp_table_gamma_0_5 = NULL;
static n_type_real *n_bmp_table_gamma_0_6 = NULL;
static n_type_real *n_bmp_table_gamma_0_7 = NULL;
static n_type_real *n_bmp_table_gamma_0_8 = NULL;
static n_type_real *n_bmp_table_gamma_0_9 = NULL;
static n_type_real *n_bmp_table_gamma_1_1 = NULL;
static n_type_real *n_bmp_table_gamma_1_2 = NULL;
static n_type_real *n_bmp_table_gamma_1_3 = NULL;
static n_type_real *n_bmp_table_gamma_1_4 = NULL;
static n_type_real *n_bmp_table_gamma_1_5 = NULL;
static n_type_real *n_bmp_table_gamma_1_6 = NULL;
static n_type_real *n_bmp_table_gamma_1_7 = NULL;
static n_type_real *n_bmp_table_gamma_1_8 = NULL;
static n_type_real *n_bmp_table_gamma_1_9 = NULL;




n_type_real
n_bmp_table_gamma_calc( n_type_real n, n_type_real gamma )
{
	return 255.0 * pow( n_bmp_table_coeff_channel( n ), gamma );
}

void
n_bmp_table_gamma_init( void )
{

	if ( n_bmp_table_gamma_onoff != n_posix_false ) { return; }


	n_bmp_table_gamma_0_1 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_2 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_3 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_4 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_5 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_6 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_7 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_8 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_0_9 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_1 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_2 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_3 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_4 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_5 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_6 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_7 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_8 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );
	n_bmp_table_gamma_1_9 = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );


	int i = 0;
	n_posix_loop
	{
		n_bmp_table_gamma_0_1[ i ] = -1;
		n_bmp_table_gamma_0_2[ i ] = -1;
		n_bmp_table_gamma_0_3[ i ] = -1;
		n_bmp_table_gamma_0_4[ i ] = -1;
		n_bmp_table_gamma_0_5[ i ] = -1;
		n_bmp_table_gamma_0_6[ i ] = -1;
		n_bmp_table_gamma_0_7[ i ] = -1;
		n_bmp_table_gamma_0_8[ i ] = -1;
		n_bmp_table_gamma_0_9[ i ] = -1;
		n_bmp_table_gamma_1_1[ i ] = -1;
		n_bmp_table_gamma_1_2[ i ] = -1;
		n_bmp_table_gamma_1_3[ i ] = -1;
		n_bmp_table_gamma_1_4[ i ] = -1;
		n_bmp_table_gamma_1_5[ i ] = -1;
		n_bmp_table_gamma_1_6[ i ] = -1;
		n_bmp_table_gamma_1_7[ i ] = -1;
		n_bmp_table_gamma_1_8[ i ] = -1;
		n_bmp_table_gamma_1_9[ i ] = -1;

		i++;
		if ( i >= 256 ) { break; }
	}


	n_bmp_table_gamma_onoff = n_posix_true;


	return;
}

void
n_bmp_table_gamma_exit( void )
{

	if ( n_bmp_table_gamma_onoff == n_posix_false ) { return; }


	free( n_bmp_table_gamma_0_1 ); n_bmp_table_gamma_0_1 = NULL;
	free( n_bmp_table_gamma_0_2 ); n_bmp_table_gamma_0_2 = NULL;
	free( n_bmp_table_gamma_0_3 ); n_bmp_table_gamma_0_3 = NULL;
	free( n_bmp_table_gamma_0_4 ); n_bmp_table_gamma_0_4 = NULL;
	free( n_bmp_table_gamma_0_5 ); n_bmp_table_gamma_0_5 = NULL;
	free( n_bmp_table_gamma_0_6 ); n_bmp_table_gamma_0_6 = NULL;
	free( n_bmp_table_gamma_0_7 ); n_bmp_table_gamma_0_7 = NULL;
	free( n_bmp_table_gamma_0_8 ); n_bmp_table_gamma_0_8 = NULL;
	free( n_bmp_table_gamma_0_9 ); n_bmp_table_gamma_0_9 = NULL;
	free( n_bmp_table_gamma_1_1 ); n_bmp_table_gamma_1_1 = NULL;
	free( n_bmp_table_gamma_1_2 ); n_bmp_table_gamma_1_2 = NULL;
	free( n_bmp_table_gamma_1_3 ); n_bmp_table_gamma_1_3 = NULL;
	free( n_bmp_table_gamma_1_4 ); n_bmp_table_gamma_1_4 = NULL;
	free( n_bmp_table_gamma_1_5 ); n_bmp_table_gamma_1_5 = NULL;
	free( n_bmp_table_gamma_1_6 ); n_bmp_table_gamma_1_6 = NULL;
	free( n_bmp_table_gamma_1_7 ); n_bmp_table_gamma_1_7 = NULL;
	free( n_bmp_table_gamma_1_8 ); n_bmp_table_gamma_1_8 = NULL;
	free( n_bmp_table_gamma_1_9 ); n_bmp_table_gamma_1_9 = NULL;


	n_bmp_table_gamma_onoff = n_posix_false;


	return;
}




#define N_EPSILON 2.2204460492503131e-16

n_posix_bool
n_bmp_table_gamma_compare( n_type_real d, n_type_real base )
{
	return ( fabs( d - base ) < N_EPSILON );
}

n_type_real
n_bmp_table_gamma_channel( n_type_real n, n_type_real gamma )
{

	if ( n_bmp_table_gamma_onoff == n_posix_false )
	{
		atexit( n_bmp_table_gamma_exit );
		n_bmp_table_gamma_init();
	}


	if ( gamma > 2.0 ) { goto n_bmp_table_gamma_channel_1_5; }
	if ( gamma > 1.0 ) { goto n_bmp_table_gamma_channel_1_0; }
	if ( gamma > 0.5 ) { goto n_bmp_table_gamma_channel_0_5; }


	if ( n_bmp_table_gamma_compare( gamma, 0.1 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_1[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_1[ elem ] = v = n_bmp_table_gamma_calc( n, 0.10 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.2 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_2[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_2[ elem ] = v = n_bmp_table_gamma_calc( n, 0.20 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.3 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_3[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_3[ elem ] = v = n_bmp_table_gamma_calc( n, 0.30 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.4 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_4[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_4[ elem ] = v = n_bmp_table_gamma_calc( n, 0.40 );
		}

		return v;
	}

	n_bmp_table_gamma_channel_0_5 :

	if ( n_bmp_table_gamma_compare( gamma, 0.5 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_5[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_5[ elem ] = v = n_bmp_table_gamma_calc( n, 0.50 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.6 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_6[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_6[ elem ] = v = n_bmp_table_gamma_calc( n, 0.60 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.7 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_7[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_7[ elem ] = v = n_bmp_table_gamma_calc( n, 0.70 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.8 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_8[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_8[ elem ] = v = n_bmp_table_gamma_calc( n, 0.80 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 0.9 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_0_9[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_0_9[ elem ] = v = n_bmp_table_gamma_calc( n, 0.90 );
		}

		return v;
	}

	n_bmp_table_gamma_channel_1_0 :

	if ( n_bmp_table_gamma_compare( gamma, 1.1111111111111112 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_1[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_1[ elem ] = v = n_bmp_table_gamma_calc( n, 1.1111111111111112 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 1.25 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_2[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_2[ elem ] = v = n_bmp_table_gamma_calc( n, 1.25 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 1.4285714285714286 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_3[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_3[ elem ] = v = n_bmp_table_gamma_calc( n, 1.4285714285714286 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 1.6666666666666667 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_4[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_4[ elem ] = v = n_bmp_table_gamma_calc( n, 1.6666666666666667 );
		}

		return v;
	}

	n_bmp_table_gamma_channel_1_5 :

	if ( n_bmp_table_gamma_compare( gamma, 2.0 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_5[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_5[ elem ] = v = n_bmp_table_gamma_calc( n, 2.0 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 2.5 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_6[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_6[ elem ] = v = n_bmp_table_gamma_calc( n, 2.5 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 3.3333333333333335 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_7[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_7[ elem ] = v = n_bmp_table_gamma_calc( n, 3.3333333333333335 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 5.0 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_8[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_8[ elem ] = v = n_bmp_table_gamma_calc( n, 5.0 );
		}

		return v;
	}

	if ( n_bmp_table_gamma_compare( gamma, 10.0 ) )
	{
		int elem = (int) n;

		n_type_real v = n_bmp_table_gamma_1_9[ elem ];
		if ( v == -1 )
		{
			n_bmp_table_gamma_1_9[ elem ] = v = n_bmp_table_gamma_calc( n, 10.0 );
		}

		return v;
	}
//if ( n_posix_false != ( 0x8000 & GetAsyncKeyState( VK_F2 ) ) ) { n_posix_debug_literal( "%0.16f", gamma ); }


	return n_bmp_table_gamma_calc( n, gamma );
}


#endif // _H_NONNON_NEUTRAL_BMP_TABLE_GAMMA

