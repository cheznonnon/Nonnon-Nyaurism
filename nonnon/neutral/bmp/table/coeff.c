// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : don't use n_memory_new()/free()
//
//	N_MEMORY_DEBUG will not work by atexit()




#ifndef _H_NONNON_NEUTRAL_BMP_TABLE_COEFF
#define _H_NONNON_NEUTRAL_BMP_TABLE_COEFF




static n_posix_bool n_bmp_table_coeff_onoff = n_posix_false;


static n_type_real *n_bmp_table_coeff = NULL;




void
n_bmp_table_coeff_init( void )
{

	if ( n_bmp_table_coeff_onoff != n_posix_false ) { return; }


	n_bmp_table_coeff = (n_type_real*) malloc( 256 * sizeof( n_type_real ) );


	int i = 0;
	n_posix_loop
	{
		n_bmp_table_coeff[ i ] = -1;

		i++;
		if ( i >= 256 ) { break; }
	}


	n_bmp_table_coeff_onoff = n_posix_true;


	return;
}

void
n_bmp_table_coeff_exit( void )
{

	if ( n_bmp_table_coeff_onoff == n_posix_false ) { return; }


	free( n_bmp_table_coeff ); n_bmp_table_coeff = NULL;


	n_bmp_table_coeff_onoff = n_posix_false;


	return;
}




n_posix_inline n_type_real
n_bmp_table_coeff_channel( n_type_real n )
{

	if ( n_bmp_table_coeff_onoff == n_posix_false )
	{
		atexit( n_bmp_table_coeff_exit );
		n_bmp_table_coeff_init();
	}


	int elem = (int) n;

	n_type_real v = n_bmp_table_coeff[ elem ];
	if ( v == -1 )
	{
		n_bmp_table_coeff[ elem ] = v = n * n_bmp_coeff_channel;
	}


	return v;
}


#endif // _H_NONNON_NEUTRAL_BMP_TABLE_COEFF

