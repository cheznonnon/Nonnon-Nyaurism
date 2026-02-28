// Nonnon Paint
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_NEUTRAL_BMP_ERROR
#define _H_NONNON_NEUTRAL_BMP_ERROR




#include "../bmp.c"




n_posix_inline BOOL
n_bmp_error( const n_bmp *bmp )
{

	// [Needed] : Win32 : some concurrency makes timing error

	if ( NULL ==            bmp   ) { return TRUE; }
	if ( NULL == N_BMP_PTR( bmp ) ) { return TRUE; }


	if ( n_bmp_safemode )
	{
		if ( 32 != N_BMP_DEPTH( bmp ) ) { return TRUE; }
		if (  0 >= N_BMP_SX   ( bmp ) ) { return TRUE; }
		if (  0 >= N_BMP_SY   ( bmp ) ) { return TRUE; }
	}


	return FALSE;
}

BOOL
n_bmp_error_clipping
(
	const n_bmp *bmp_f,
	const n_bmp *bmp_t,
	s32 *arg_fx, s32 *arg_fy, s32 *arg_fsx, s32 *arg_fsy,
	s32 *arg_tx, s32 *arg_ty
)
{

	if ( n_bmp_error( bmp_f ) ) { return TRUE; }

	if ( bmp_t != NULL )
	{
		if ( n_bmp_error( bmp_t ) ) { return TRUE; }
	}


	s32 bmp_fsx,bmp_fsy, bmp_tsx,bmp_tsy;
	s32 fx,fy,fsx,fsy, tx,ty;


	bmp_fsx = bmp_fsy = bmp_tsx = bmp_tsy = 0;
	fx = fy = fsx = fsy = tx = ty = 0;


	bmp_fsx = bmp_tsx = N_BMP_SX( bmp_f );
	bmp_fsy = bmp_tsy = N_BMP_SY( bmp_f );

	if ( bmp_t != NULL )
	{
		bmp_tsx = N_BMP_SX( bmp_t );
		bmp_tsy = N_BMP_SY( bmp_t );
	}

	if ( arg_fx  != NULL ) { fx  = *arg_fx;  }
	if ( arg_fy  != NULL ) { fy  = *arg_fy;  }
	if ( arg_fsx != NULL ) { fsx = *arg_fsx; }
	if ( arg_fsy != NULL ) { fsy = *arg_fsy; }
	if ( arg_tx  != NULL ) { tx  = *arg_tx;  }
	if ( arg_ty  != NULL ) { ty  = *arg_ty;  }


	// Phase 1 : out of top-left corner

	if ( fx < 0 ) { fsx -= n_posix_abs_n_type_gfx( fx ); tx += n_posix_abs_n_type_gfx( fx ); fx = 0; }
	if ( fy < 0 ) { fsy -= n_posix_abs_n_type_gfx( fy ); ty += n_posix_abs_n_type_gfx( fy ); fy = 0; }

	if ( bmp_t != NULL )
	{
		if ( tx < 0 ) { fsx -= n_posix_abs_n_type_gfx( tx ); fx += n_posix_abs_n_type_gfx( tx ); tx = 0; }
		if ( ty < 0 ) { fsy -= n_posix_abs_n_type_gfx( ty ); fy += n_posix_abs_n_type_gfx( ty ); ty = 0; }
	}

	if ( fsx <= 0 ) { return TRUE; }
	if ( fsy <= 0 ) { return TRUE; }


	// Phase 2 : out of bottom-right corner

	if ( fx >= bmp_fsx ) { return TRUE; }
	if ( fy >= bmp_fsy ) { return TRUE; }

	if ( bmp_t != NULL )
	{
		if ( tx >= bmp_tsx ) { return TRUE; }
		if ( ty >= bmp_tsy ) { return TRUE; }
	}

	if ( ( fx + fsx ) > bmp_fsx ) { fsx = bmp_fsx - fx; }
	if ( ( fy + fsy ) > bmp_fsy ) { fsy = bmp_fsy - fy; }

	if ( bmp_t != NULL )
	{
		if ( ( tx + fsx ) > bmp_tsx ) { fsx = bmp_tsx - tx; }
		if ( ( ty + fsy ) > bmp_tsy ) { fsy = bmp_tsy - ty; }
	}

	if ( fsx <= 0 ) { return TRUE; }
	if ( fsy <= 0 ) { return TRUE; }


	if ( arg_fx  != NULL ) { *arg_fx  = fx;  }
	if ( arg_fy  != NULL ) { *arg_fy  = fy;  }
	if ( arg_fsx != NULL ) { *arg_fsx = fsx; }
	if ( arg_fsy != NULL ) { *arg_fsy = fsy; }
	if ( arg_tx  != NULL ) { *arg_tx  = tx;  }
	if ( arg_ty  != NULL ) { *arg_ty  = ty;  }


	return FALSE;
}


#endif //_H_NONNON_NEUTRAL_BMP_ERROR

