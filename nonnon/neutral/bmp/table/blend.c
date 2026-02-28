// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : currently broken




#ifndef _H_NONNON_NEUTRAL_BMP_TABLE_BLEND
#define _H_NONNON_NEUTRAL_BMP_TABLE_BLEND




static BOOL  n_bmp_table_blend_onoff = FALSE;
static n_bmp n_bmp_table_blend_bmp;




void
n_bmp_table_blend_init( void )
{

	if ( n_bmp_table_blend_onoff != FALSE ) { return; }


	n_bmp_zero( &n_bmp_table_blend_bmp );
	n_bmp_new_fast( &n_bmp_table_blend_bmp, 256, 100 );

	n_bmp_flush( &n_bmp_table_blend_bmp, 0 );


	n_bmp_table_blend_onoff = TRUE;


	return;
}

void
n_bmp_table_blend_exit( void )
{

	if ( n_bmp_table_blend_onoff == FALSE ) { return; }


	n_bmp_free( &n_bmp_table_blend_bmp );
	n_bmp_zero( &n_bmp_table_blend_bmp );


	n_bmp_table_blend_onoff = FALSE;


	return;
}




int
n_bmp_table_blend_calc( int f, int t, n_type_real blend )
{

	n_type_real d = ( f - t ) * blend;

	if ( f > t )
	{
		return f - (int) n_posix_max_n_type_real(  1.0, d );
	} else {
		return f - (int) n_posix_min_n_type_real( -1.0, d );
	}
}

n_posix_inline int
n_bmp_table_blend_channel( int f, int t, n_type_real blend )
{

	// [!] : shortcut

	if ( f == t ) { return t; }

	if ( blend <= 0 ) { return f; }
	if ( blend >= 1 ) { return t; }


	if ( n_bmp_table_blend_onoff == FALSE )
	{
		atexit( n_bmp_table_blend_exit );
		n_bmp_table_blend_init();
	}


	int tx = abs( f - t );
	int ty = blend * 100;

	int y = 0;
	n_posix_loop
	{break;

		if ( y == ty )
		{

			u32 c; n_bmp_ptr_get( &n_bmp_table_blend_bmp, tx,ty, &c );

			if ( c == 0 )
			{
				c = n_bmp_table_blend_calc( f,t, blend );
				n_bmp_ptr_set( &n_bmp_table_blend_bmp, tx,ty, c );
			}

			return c;
		}

		y++;
		if ( y >= 100 ) { break; }
	}


	return n_bmp_table_blend_calc( f,t, blend );
}


#endif // _H_NONNON_NEUTRAL_BMP_TABLE_BLEND

