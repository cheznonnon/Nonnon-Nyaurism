// curico
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : alpha format
//
//	.BMP  : 0 == full transparent : Vista or later : DWM only uses
//	.ICO  : 0 == full transparent
//	.PNG  : 0 == full transparent
//
//	.ICO  : all pixels have zero for alpha value
//	"not transparent for backward compatibility"




#ifndef _H_NONNON_NEUTRAL_CURICO
#define _H_NONNON_NEUTRAL_CURICO




#include "./bmp/_codec.c"
#include "./dir.c"
#include "./png.c"
#include "./string_path.c"




// public


#define N_CURICO_TYPE_ICO 1
#define N_CURICO_TYPE_CUR 2


#define N_CURICO_SIZE_DIR     6
#define N_CURICO_SIZE_ENTRY  16
#define N_CURICO_SIZE_HEADER sizeof( n_curico )


#define N_CURICO_FAVICON_OFF 0
#define N_CURICO_FAVICON_ON  1


#define N_CURICO_LOADED_PNG  ( -2 )


#define N_CURICO_EXT_ICO n_posix_literal( ".ico\0\0" )
#define N_CURICO_EXT_CUR n_posix_literal( ".cur\0\0" )
#define N_CURICO_EXT_PNG n_posix_literal( ".png\0\0" )
#define N_CURICO_EXT_MLT n_posix_literal( ".multi\0\0" )




#pragma pack( push, 1 )

typedef struct {

	// ICONDIR

	u16 zero1;
	u16 type;
	u16 count;


	// ICONDIRENTRY

	// Win2000 may ignore those 4 params
	u8  sx;
	u8  sy;
	u8  colors;
	u8  zero2;

	u16 hotspotx;
	u16 hotspoty;
	u32 bmpsize;
	u32 offset;

} n_curico;

#pragma pack( pop )




static int n_curico_favicon_onoff = N_CURICO_FAVICON_OFF;




#define n_curico_zero(  f    ) n_memory_zero( f,    sizeof( n_curico ) )
#define n_curico_alias( f, t ) n_memory_copy( f, t, sizeof( n_curico ) )

#define n_curico_alpha_reverse_encode( b ) n_curico_alpha_reverse( b, n_posix_true  )
#define n_curico_alpha_reverse_decode( b ) n_curico_alpha_reverse( b, n_posix_false )

void
n_curico_alpha_reverse( n_bmp *bmp, n_posix_bool is_encode )
{

	if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 0 )
	{

		if ( n_bmp_alpha_is_used( bmp ) )
		{
			n_bmp_alpha_reverse( bmp );
		}

	} else {

		if ( ( is_encode )&&( n_bmp_alpha_is_visible( bmp ) ) )
		{
			n_bmp_alpha_reverse( bmp );
		}


	}


	return;
}

void
n_curico_mask_encode( n_bmp *bmp, u8 *maskptr )
{

	if (     bmp == NULL ) { return; }
	if ( maskptr == NULL ) { return; }


	if ( NULL == N_BMP_PTR  ( bmp ) ) { return; }
	if ( 32   != N_BMP_DEPTH( bmp ) ) { return; }


	const u32 curicoblack = n_bmp_rgb( 1,1,1 );
	const s32 bmpsx       = N_BMP_SX( bmp );
	const s32 bmpsy       = N_BMP_SY( bmp );


	{

		// [!] : currently memory clear is needed

		n_type_int byte = n_bmp_size( bmpsx, bmpsy, 1 );

		n_memory_zero( maskptr, byte );

	}


	n_type_gfx masklinebyte = n_bmp_linebyte( bmpsx, 1 );
	int        bit          = 7;


	s32 x = 0;
	s32 y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );

		if (
			( color == n_bmp_trans )
			||
			( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( color ) )
		)
		{
			n_type_gfx pos = ( x / 8 ) + ( masklinebyte * ( bmpsy - 1 - y ) );
			maskptr[ pos ] += ( 1 << bit );
		}

		if ( color == curicoblack ) 
		{
			n_bmp_ptr_set_fast( bmp, x,y, n_bmp_trans );
		}

		if ( bit != 0 ) { bit--; } else { bit = 7; }


		x++;
		if ( x >= bmpsx )
		{

			bit = 7;

			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	n_curico_alpha_reverse_encode( bmp );


	return;
}

void
n_curico_mask_decode( n_bmp *bmp, u8 *maskptr )
{

	if (     bmp == NULL ) { return; }
	if ( maskptr == NULL ) { return; }


	if ( NULL == N_BMP_PTR  ( bmp ) ) { return; }
	if ( 32   != N_BMP_DEPTH( bmp ) ) { return; }


	const u32 curicoblack = n_bmp_rgb( 1,1,1 );
	const s32 bmpsx       = N_BMP_SX( bmp );
	const s32 bmpsy       = N_BMP_SY( bmp );


	n_curico_alpha_reverse_decode( bmp );


	n_type_gfx masklinebyte = n_bmp_linebyte( bmpsx, 1 );


	int bit  = 7;
	int byte = 0;


	s32 x = 0;
	s32 y = 0;
	n_posix_loop
	{

		if ( bit == 7 )
		{
			n_type_gfx pos = ( x / 8 ) + ( masklinebyte * ( bmpsy - 1 - y ) );
			byte = maskptr[ pos ];
		}


		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );


		// [!] : for some buggy files

		int   sum = n_bmp_r( color ) + n_bmp_g( color ) + n_bmp_b( color );
		int alpha = n_bmp_a( color );

		if ( ( alpha == N_BMP_ALPHA_CHANNEL_VISIBLE )&&( sum <= 3 ) )
		{
			n_bmp_ptr_set_fast( bmp, x,y, curicoblack );
		}


		if (
			( ( byte >> bit ) & 1 )
			&&
			( alpha != N_BMP_ALPHA_CHANNEL_INVISIBLE )
		)
		{
			n_bmp_ptr_set_fast( bmp, x,y, n_bmp_trans );
		}

		if ( bit != 0 ) { bit--; } else { bit = 7; }


		x++;
		if ( x >= bmpsx )
		{

			bit = 7;

			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	return;
}

void
n_curico_mask_restore( n_bmp *bmp, n_bmp *msk )
{

	if ( n_bmp_error( bmp ) ) { return; }
	if ( n_bmp_error( msk ) ) { return; }


	const s32 bmpsx = N_BMP_SX( bmp );
	const s32 bmpsy = N_BMP_SY( bmp );


	s32 x = 0;
	s32 y = 0;
	n_posix_loop
	{

		u32 f,t;
		n_bmp_ptr_get_fast( msk, x,y, &f ); f &= 0x00ffffff;
		n_bmp_ptr_get_fast( bmp, x,y, &t );


		if ( f )
		{

			n_bmp_ptr_set_fast( bmp, x,y, n_bmp_trans );

		} else
		if ( t == n_bmp_black )
		{

			n_bmp_ptr_set_fast( bmp, x,y, n_bmp_rgb( 1,1,1 ) );

		} else
		if ( N_BMP_ALPHA_CHANNEL_VISIBLE != n_bmp_a( t ) )
		{

			// [Patch] : XP : shortcut arrows will be disappeared

			if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 0 )
			{
				t = n_bmp_alpha_replace_pixel( t, 255 - n_bmp_a( t ) );
			}

			if ( t == n_bmp_black ) { t = n_bmp_rgb( 1,1,1 ); }

			n_bmp_ptr_set_fast( bmp, x,y, t );

		}


		x++;
		if ( x >= bmpsx )
		{

			x = 0;

			y++;
			if ( y >= bmpsy ) { break; }
		}
	}


	return;
}

#define n_curico_save_literal( c, b, name ) n_curico_save( c, b, n_posix_literal( name ) )

n_posix_bool
n_curico_save( n_curico *curico, const n_bmp *bmp, const n_posix_char *bmpname )
{

	if ( curico == NULL ) { return n_posix_true; }
	if (    bmp == NULL ) { return n_posix_true; }

	if ( NULL == N_BMP_PTR( bmp ) ) { return n_posix_true; }

	if ( n_string_is_empty( bmpname ) ) { return n_posix_true; }


	if (
		( curico->type != N_CURICO_TYPE_ICO )
		&&
		( curico->type != N_CURICO_TYPE_CUR )
	)
	{
		curico->type = N_CURICO_TYPE_ICO;
	}


	n_posix_char *newname = n_string_path_ext_mod_new( bmpname, N_CURICO_EXT_ICO );

	if ( curico->type == N_CURICO_TYPE_ICO ) 
	{
		//n_string_path_ext_mod( N_CURICO_EXT_ICO, newname );
	} else {
		n_string_path_ext_mod( N_CURICO_EXT_CUR, newname );
	}


	FILE *fp = n_posix_fopen_write( newname );
	n_string_path_free( newname );
	if ( fp == NULL ) { return n_posix_true; }


	// [!] : non-breaking

	n_bmp save;
	n_bmp_carboncopy( bmp, &save );

	const n_type_gfx bmpsx = N_BMP_SX( &save );
	const n_type_gfx bmpsy = N_BMP_SY( &save );


	n_type_gfx  palbyte  = 0;
	n_type_gfx  maskbyte = n_bmp_size( bmpsx, bmpsy, 1 );
	u8         *maskptr  = n_memory_new_closed( maskbyte );

	n_curico_mask_encode( &save, maskptr );

	if ( n_curico_favicon_onoff == N_CURICO_FAVICON_OFF )
	{

		n_bmp_full2pal( &save );
		palbyte = N_BMP_PAL( &save ) * sizeof( u32 );

		// [x] : not available : maybe out of specification
		//n_bmp_rle8_encode( &save );

		if ( 4 >= N_BMP_DEPTH( &save ) )
		{
			curico->colors = (u8) N_BMP_PAL( &save );
		}

	}


	if (
		( curico->type != N_CURICO_TYPE_ICO )
		&&
		( curico->type != N_CURICO_TYPE_CUR )
	)
	{
		curico->type = N_CURICO_TYPE_ICO;
	}

	curico->count = 1;

	if ( 256 > bmpsx ) { curico->sx = (u8) bmpsx; }
	if ( 256 > bmpsy ) { curico->sy = (u8) bmpsy; }


	// [!] : need to be [ (.ico/.cur size) - (header:22byte) ]

	curico->offset  = N_CURICO_SIZE_HEADER;
	curico->bmpsize = N_BMP_SIZE_INFOH + (u32) palbyte + (u32) N_BMP_SIZE( &save ) + (u32) maskbyte;


	// [Needed] : a cheap trick!

	N_BMP_SY( &save ) *= 2;


	n_posix_fwrite(  curico,                 N_CURICO_SIZE_HEADER, 1, fp );
	n_posix_fwrite( &N_BMP_INFOH  ( &save ), N_BMP_SIZE_INFOH,     1, fp );
	n_posix_fwrite(  N_BMP_PTR_PAL( &save ), palbyte,              1, fp );
	n_posix_fwrite(  N_BMP_PTR    ( &save ), N_BMP_SIZE( &save ),  1, fp );
	n_posix_fwrite(  maskptr,                maskbyte,             1, fp );


	// Cleanup

	n_bmp_free( &save );
	n_memory_free_closed( maskptr );


	n_posix_fclose( fp );


	return n_posix_false;
}

n_posix_bool
n_curico_bmp2curico( const n_posix_char *bmpname, u16 type, u16 hotspotx, u16 hotspoty )
{

	n_posix_bool ret;

	n_curico curico;
	n_bmp    bmp;


	n_bmp_zero( &bmp );

	ret = n_bmp_load( &bmp, bmpname );

	if ( ret ) { return ret; }


	n_curico_zero( &curico );

	curico.type     = type;
	curico.hotspotx = hotspotx;
	curico.hotspoty = hotspoty;

	ret = n_curico_save( &curico,&bmp, bmpname );

	n_bmp_free( &bmp );


	return ret;
}

n_posix_bool
n_curico_load( n_curico *curico, n_bmp *bmp, const n_posix_char *fname )
{

	if ( curico == NULL ) { return n_posix_true; }
	if (    bmp == NULL ) { return n_posix_true; }


	n_type_int file_byte = n_posix_stat_size( fname );
	if ( file_byte <= ( N_CURICO_SIZE_HEADER + N_BMP_SIZE_INFOH ) ) { return n_posix_true; }
	if ( n_bmp_size_is_overflow( file_byte ) ) { return n_posix_true; }


	// [!] : non-breaking : previous n_curico, n_bmp

	n_curico check_curico;
	n_bmp    check_bmp;


	FILE *fp = n_posix_fopen_read( fname );
	if ( fp == NULL ) { return n_posix_true; }


	n_posix_fread( &check_curico, N_CURICO_SIZE_HEADER, 1, fp );

	if (
		( file_byte != ( N_CURICO_SIZE_HEADER + check_curico.bmpsize ) )
		||
		( check_curico.zero1  !=  0 )
		||
		( check_curico.count  !=  1 )
		||
		( check_curico.offset != N_CURICO_SIZE_HEADER )
	)
	{

		n_posix_fclose( fp );

		return n_posix_true;
	}


	n_bmp_zero( &check_bmp );

	n_posix_fread( &N_BMP_INFOH( &check_bmp ), N_BMP_SIZE_INFOH, 1, fp );


	// [!] : sanitize
	//
	//	+ hangup when PNG icon
	//
	//	PNG : 0x89 50 4e 47 : . P N G
	//	BMP : 0x28 00 00 00 : BITMAPINFOHEADER.biSize (40Byte)
	//
	//	+ unknown depth cannot be decoded

//n_posix_debug_literal( "%08x", N_BMP_CBSIZE( &check_bmp ) );

	if (
		( N_BMP_SIZE_INFOH != N_BMP_CBSIZE( &check_bmp ) )
		||
		( 0 == N_BMP_DEPTH( &check_bmp ) )
	)
	{

		n_posix_fclose( fp );

		return n_posix_true;
	}


	// [Needed] : a cheap trick!

	N_BMP_SY( &check_bmp ) /= 2;


	n_type_gfx data_byte = n_bmp_size( N_BMP_SX( &check_bmp ), N_BMP_SY( &check_bmp ), N_BMP_DEPTH( &check_bmp ) );
	n_type_gfx mask_byte = n_bmp_size( N_BMP_SX( &check_bmp ), N_BMP_SY( &check_bmp ), 1 );
	n_type_gfx read_byte = 0;

	if ( 0 == N_BMP_PAL( &check_bmp ) )
	{
		u32 known_byte = N_CURICO_SIZE_HEADER + N_BMP_SIZE_INFOH + (u32) data_byte + (u32) mask_byte;
		N_BMP_PAL( &check_bmp ) = ( (u32) file_byte - known_byte ) / sizeof( u32 );
	}

	read_byte = data_byte + ( N_BMP_PAL( &check_bmp ) * sizeof( u32 ) );
	data_byte = N_BMP_SIZE_HEADER + read_byte;


	u8 *data_ptr = n_memory_new_closed( data_byte );
	u8 *mask_ptr = n_memory_new_closed( mask_byte );


	n_memory_copy( &N_BMP_FILEH( &check_bmp ),  data_ptr,                     N_BMP_SIZE_FILEH );
	n_memory_copy( &N_BMP_INFOH( &check_bmp ), &data_ptr[ N_BMP_SIZE_FILEH ], N_BMP_SIZE_INFOH );

	n_posix_fread( &data_ptr[ N_BMP_SIZE_HEADER ], read_byte, 1, fp );
	n_posix_fread(  mask_ptr,                      mask_byte, 1, fp );


	n_posix_fclose( fp );


	n_posix_bool ret = n_bmp_load_onmemory( &check_bmp, data_ptr, data_byte );
	if ( ret == n_posix_false )
	{

		n_bmp_replace( &check_bmp, bmp );

		n_curico_alias( &check_curico, curico );


		n_curico_mask_decode( bmp, mask_ptr );

	}


	n_memory_free_closed( mask_ptr );
	n_memory_free_closed( data_ptr );


//n_posix_debug_literal( "%d", ret );
	return ret;
}

n_posix_bool
n_curico_curico2bmp( const n_posix_char *fname )
{

	n_curico curico;
	n_bmp    bmp;


	n_posix_bool ret = n_curico_load( &curico,&bmp, fname );
	if ( ret ) { return ret; }

	n_bmp_save( &bmp, fname );

	n_bmp_free( &bmp );


	return n_posix_false;
}

n_posix_bool
n_curico_multi_pack( const n_posix_char *folder )
{

	// [Mechanism]
	//
	//	a folder name will be a icon/cursor name
	//	the first valid item determines icon or cursor


	if ( folder == NULL ) { return n_posix_true; }


	n_dir d; n_dir_zero( &d );
	if (
		( n_dir_load( &d, folder ) )
		||
		( 0 == n_dir_all( &d ) )
		||
		( d.file == 0 )
	)
	{

		n_dir_free( &d );

		return n_posix_true;
	}

	n_dir_sort( &d );


	n_curico *c = n_memory_new_closed( d.file * sizeof( n_curico ) );
	n_bmp    *b = n_memory_new_closed( d.file * sizeof( n_bmp    ) );
	n_bmp    *m = n_memory_new_closed( d.file * sizeof( n_bmp    ) );


	n_type_gfx i, validcount;


	i = validcount = 0;
	n_posix_loop
	{

		n_posix_char *str = n_string_path_make_new( folder, n_dir_name( &d, i ) );
//n_posix_debug( str );

		n_curico *curico = &c[ validcount ];
		n_bmp    *bmp    = &b[ validcount ];
		n_bmp    *mask   = &m[ validcount ];

		n_curico_zero( curico );
		n_bmp_zero   ( bmp    );
		n_bmp_zero   ( mask   );

		if (
			( n_posix_false == n_curico_load( curico, bmp, str ) )
			&&
			( curico->type == c[ 0 ].type )
		)
		{
//n_posix_debug_literal( "%d %d", curico->type, c[ 0 ].type );

			// Mask

			s32 bmpsx = N_BMP_SX( bmp );
			s32 bmpsy = N_BMP_SY( bmp );

			N_BMP_SIZE( mask ) = n_bmp_size( bmpsx, bmpsy, 1 );
			N_BMP_PTR ( mask ) = n_memory_new( N_BMP_SIZE( mask ) );

			n_curico_mask_encode( bmp, (void*) N_BMP_PTR( mask ) );

			n_bmp_full2pal( bmp );
			if ( 4 >= N_BMP_DEPTH( bmp ) )
			{
				curico->colors = (u8) N_BMP_PAL( bmp );
			}

			curico->bmpsize  = N_BMP_SIZE_INFOH;
			curico->bmpsize += N_BMP_PAL ( bmp ) * 4;
			curico->bmpsize += (u32) N_BMP_SIZE( bmp );
			curico->bmpsize += (u32) N_BMP_SIZE( mask );


			// Bitmap

			n_type_gfx byte_pal = N_BMP_PAL ( bmp ) * 4;
			n_type_gfx byte_bmp = N_BMP_SIZE( bmp );
			n_type_gfx byte     = byte_pal + byte_bmp;

			u8 *ptr = n_memory_new( byte );

			n_memory_copy( N_BMP_PTR_PAL( bmp ), &ptr[        0 ], byte_pal );
			n_memory_copy( N_BMP_PTR    ( bmp ), &ptr[ byte_pal ], byte_bmp );


			n_memory_free( N_BMP_PTR( bmp ) );

			N_BMP_PTR ( bmp ) = (void*) ptr;
			N_BMP_SIZE( bmp ) = byte;


			validcount++;

		} else
		if ( n_string_path_ext_is_same( N_CURICO_EXT_PNG, str ) )
		{

			// [!] : n_dir_sort() : CUR => ICO => PNG

			FILE *fp = n_posix_fopen_read( str );
			if ( fp != NULL )
			{

				// [!] : when PNG only

				if ( c[ 0 ].type == 0 ) { c[ 0 ].type = N_CURICO_TYPE_ICO; }

				// [Problem] : Is PNG Cursor possible?

				curico->type    = c[ 0 ].type;
				curico->count   = 1;
				curico->bmpsize = (u32) n_posix_stat_size( str );
				curico->offset  = N_CURICO_SIZE_HEADER;


				N_BMP_PTR ( bmp ) = n_memory_new( curico->bmpsize );
				N_BMP_SIZE( bmp ) = curico->bmpsize;

				n_posix_fread( N_BMP_PTR ( bmp ), curico->bmpsize, 1, fp );

				n_posix_fclose( fp );


				validcount++;

			}

		}


		n_string_path_free( str );


		i++; 
		if ( i >= n_dir_all( &d ) ) { break; }
	}


	n_dir_free( &d );


	if ( validcount == 0 )
	{
//n_posix_debug_literal( "validcount = 0" );

		n_memory_free_closed( c );
		n_memory_free_closed( b );
		n_memory_free_closed( m );

		return n_posix_true;
	}


	// [Mechanism] : <Header + Entries + Data>
	//
	//	Header  : h :  6 byte
	//	Entries : c : 16 byte
	//	Bitmap  : b
	//	Mask    : m
	//
	//	4 icons like this : hccccbmbmbmbm


	n_posix_char *newname = n_string_path_new( n_posix_strlen( folder ) + n_posix_strlen( N_CURICO_EXT_MLT ) + n_posix_strlen( N_CURICO_EXT_ICO ) );
	n_string_copy( folder, newname );

	if ( validcount >= 2 )
	{
		n_string_path_ext_add( N_CURICO_EXT_MLT, newname );
	}

	if ( c[ 0 ].type == N_CURICO_TYPE_ICO )
	{
		n_string_path_ext_add( N_CURICO_EXT_ICO, newname );
	} else {
		n_string_path_ext_add( N_CURICO_EXT_CUR, newname );
	}


	FILE *fp = n_posix_fopen_write( newname );
	n_string_path_free( newname );


	c[ 0 ].count = (u16) validcount;
	n_posix_fwrite( &c[ 0 ], N_CURICO_SIZE_DIR, 1, fp );


	u32 offset = N_CURICO_SIZE_DIR + ( N_CURICO_SIZE_ENTRY * validcount );


	i = 0;
	n_posix_loop
	{//break;

		// [!] : write ICONDIRENTRY first


		c[ i ].offset = offset;

		n_posix_fwrite( &c[ i ].sx, N_CURICO_SIZE_ENTRY, 1, fp );

		if ( NULL != N_BMP_PTR( &m[ i ] ) )
		{
			offset += N_BMP_SIZE_INFOH + N_BMP_SIZE( &b[ i ] ) + N_BMP_SIZE( &m[ i ] );
		} else {
			offset += N_BMP_SIZE( &b[ i ] );
		}


		i++; 
		if ( i >= validcount ) { break; }
	}

	i = 0;
	n_posix_loop
	{

		if ( NULL != N_BMP_PTR( &m[ i ] ) )
		{

			// a cheap trick!

			N_BMP_SY( &b[ i ] ) *= 2;

			n_posix_fwrite( &N_BMP_INFOH( &b[ i ] ), N_BMP_SIZE_INFOH,      1, fp );
			n_posix_fwrite(  N_BMP_PTR  ( &b[ i ] ), N_BMP_SIZE( &b[ i ] ), 1, fp );
			n_posix_fwrite(  N_BMP_PTR  ( &m[ i ] ), N_BMP_SIZE( &m[ i ] ), 1, fp );

		} else {

			n_posix_fwrite(  N_BMP_PTR  ( &b[ i ] ), N_BMP_SIZE( &b[ i ] ), 1, fp );

		}


		n_bmp_free( &b[ i ] );
		n_bmp_free( &m[ i ] );


		i++; 
		if ( i >= validcount ) { break; }
	}

	n_posix_fclose( fp );


	n_memory_free_closed( c );
	n_memory_free_closed( b );
	n_memory_free_closed( m );


	return n_posix_false;
}

n_posix_bool
n_curico_multi_unpack( const n_posix_char *fname )
{

	n_type_int fsize = n_posix_stat_size( fname );
	if ( fsize < ( N_CURICO_SIZE_HEADER + N_BMP_SIZE_INFOH ) ) { return n_posix_true; }
	if ( n_bmp_size_is_overflow( fsize ) ) { return n_posix_true; }


	FILE *fp = n_posix_fopen_read( fname );
	if ( fp == NULL ) { return n_posix_true; }

	u8 *p = n_memory_new_closed( fsize );
	n_posix_fread( p, fsize, 1, fp );

	n_posix_fclose( fp );


	n_curico  c;
	n_memory_copy( p, &c, N_CURICO_SIZE_DIR );

//n_posix_debug_literal( "%d-%d-%d", c.zero1, c.type, c.count );


	if (
		( c.zero1 != 0 )
		||
		(
			( c.type != N_CURICO_TYPE_ICO )
			&&
			( c.type != N_CURICO_TYPE_CUR )
		)
		||
		( c.count == 0 )
	)
	{

		n_memory_free_closed( p );

		return n_posix_true;
	}


	// [!] : PNG icon support
	//
	//	PNG : 0x89 50 4e 47 : . P N G
	//	BMP : 0x28 00 00 00 : BITMAPINFOHEADER.biSize (40Byte)

	// [!] : an .ico file has only one .png file

	if (
		( c.count == 1 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 0 ] != 0x89 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 1 ] != 0x50 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 2 ] != 0x4e )
		&&
		( p[ N_CURICO_SIZE_HEADER + 3 ] != 0x47 )
	)
	{

		n_memory_free_closed( p );

		return n_posix_true;
	}


	u16 count = c.count;
	  c.count = 1;


	n_posix_char *ext = n_string_path_ext_get_new( fname );
	n_posix_char *dir = n_string_path_new( n_posix_strlen( fname ) + n_posix_strlen( N_CURICO_EXT_MLT ) + n_posix_strlen( N_CURICO_EXT_ICO ) );
	n_string_copy( fname, dir );

	if (
		( n_string_path_ext_is_same( N_CURICO_EXT_ICO, dir ) )
		||
		( n_string_path_ext_is_same( N_CURICO_EXT_CUR, dir ) )
	)
	{
		n_string_path_ext_del( dir );
	}

	if ( n_string_path_ext_is_same( N_CURICO_EXT_MLT, dir ) )
	{
		n_string_path_ext_del( dir );
	}

#ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_mkdir( dir );

#else  // #ifdef N_POSIX_PLATFORM_WINDOWS

	n_posix_mkdir( dir, 0777 );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS

//n_posix_debug( dir );

	n_type_int digit;
	{
		n_posix_char str_digit[ 100 ];
		n_type_int   cch_digit = n_posix_sprintf_literal( str_digit, "%d", count );

		digit = n_posix_max_n_type_int( 3, cch_digit );
	}

	n_type_int i = 0;
	n_posix_loop
	{
	
		n_posix_char *str = n_string_path_new( n_posix_strlen( dir ) + n_posix_strlen( N_POSIX_SLASH ) + digit + n_posix_strlen( ext ) );

		n_type_int offset = N_CURICO_SIZE_DIR + ( i * N_CURICO_SIZE_ENTRY );
		if ( offset < fsize )
		{

			n_memory_copy( &p[ offset ], &c.sx, N_CURICO_SIZE_ENTRY );

			offset   = c.offset;
			c.offset = N_CURICO_SIZE_HEADER;

			int digit_int = (int) digit;

			n_posix_sprintf_literal( str, "%s%s%0*lld%s", dir, N_POSIX_SLASH, digit_int, i, ext );
//n_posix_debug( str );

		} else {

			offset = 0;

		}


		if (
			( offset != 0 )
			&&
			( ( offset + c.bmpsize ) <= fsize )
		)
		{

			if ( p[ offset ] == 0x89 )
			{
				n_string_path_ext_mod( N_CURICO_EXT_PNG, str );
			}

			fp = n_posix_fopen_write( str );
			if ( fp != NULL )
			{

				if ( p[ offset ] != 0x89 )
				{
					n_posix_fwrite( &c, N_CURICO_SIZE_HEADER, 1, fp );
				}

				n_posix_fwrite( &p[ offset ], c.bmpsize, 1, fp );


				n_posix_fclose( fp );

			}

		}


		n_string_path_free( str );


		i++;
		if ( i >= count ) { break; }
	}


	n_string_path_free( ext );
	n_string_path_free( dir );


	n_memory_free_closed( p );


	return n_posix_false;
}

n_posix_bool
n_curico_bmp_load_onmemory( u8 *p, n_type_gfx cb, n_bmp *bmp, n_bmp *msk, n_type_gfx *loaded_bpp, n_type_gfx *loaded_size )
{

	n_posix_bool ret = n_posix_true;

	if ( p == NULL ) { return ret; }

	if ( n_bmp_safemode )
	{
		if ( bmp == NULL ) { return ret; }
		//if ( msk == NULL ) { return ret; } /* acceptable */
	}


	if ( cb < 1 ) { return ret; }

	if ( p[ 0 ] != N_BMP_SIZE_INFOH )
	{
//n_posix_debug_literal( " PNG Icon " );

		if ( n_posix_false )//( n_sysinfo_version_vista_or_later() )
		{

			// [!] : WinXP or earlier : not supported

			//DWORD byte = SizeofResource( hmod, hrsrc );
			//UINT    lr = LR_DEFAULTCOLOR | LR_DEFAULTSIZE;
			//HICON hico = CreateIconFromResourceEx( p, byte, n_posix_true, 0x00030000, sx,sy, lr );

			//n_win_icon_hicon2bmp( hico, sx,sy, bmp, msk );

		} else {

			n_png png; n_png_zero( &png );
			ret = n_png_load_onmemory( &png, p, cb );
			if ( ret == n_posix_false )
			{
				ret = n_png_uncompress( &png, bmp );
			}

			if ( ret == n_posix_false )
			{
				if ( loaded_bpp  != NULL ) { (*loaded_bpp ) = N_CURICO_LOADED_PNG; }
				if ( loaded_size != NULL ) { (*loaded_size) = N_BMP_SX( bmp );     }
			}

			n_png_free( &png );

		}

	} else {

		if ( cb < N_BMP_SIZE_INFOH ) { return ret; }


		n_bmp check; n_bmp_zero( &check );


		n_memory_copy( p, &N_BMP_INFOH( &check ), N_BMP_SIZE_INFOH );

		// [!] : trick!

		N_BMP_SY( &check ) /= 2;

		// [!] : index color mode : palette zero is special

		if ( ( 8 >= N_BMP_DEPTH( &check ) )&&( 0 == N_BMP_PAL( &check ) ) )
		{
			if ( 1 == N_BMP_DEPTH( &check ) ) { N_BMP_PAL( &check ) =   2; } else
			if ( 4 == N_BMP_DEPTH( &check ) ) { N_BMP_PAL( &check ) =  16; } else
			if ( 8 == N_BMP_DEPTH( &check ) ) { N_BMP_PAL( &check ) = 256; }
		}

		n_type_gfx bpp  = N_BMP_DEPTH( &check );
		n_type_gfx size = N_BMP_SX   ( &check );


		n_type_gfx offset   = N_BMP_SIZE_INFOH;
		n_type_gfx pal_byte = N_BMP_PAL( &check ) * sizeof( u32 );
		n_type_gfx ptr_byte = n_bmp_size( N_BMP_SX( &check ), N_BMP_SY( &check ), N_BMP_DEPTH( &check ) );

		if ( cb < ( offset + pal_byte + ptr_byte ) ) { return ret; }


		if ( 0 != N_BMP_PAL( &check ) )
		{
			N_BMP_PTR_PAL( &check ) = n_memory_new( pal_byte );
			n_memory_copy( &p[ offset ], N_BMP_PTR_PAL( &check ), pal_byte );
//n_posix_debug_literal( " %d ", pal_byte );

			offset += pal_byte;
		}


		N_BMP_PTR( &check ) = n_memory_new( ptr_byte );
		n_memory_copy( &p[ offset ], N_BMP_PTR( &check ), ptr_byte );

		if ( 0 != N_BMP_PAL( &check ) )
		{

			// [!] : a few percent faster

			n_type_gfx i = 0;
			n_type_gfx b = 0;
			n_posix_loop
			{

				u32 color = N_BMP_PTR_PAL( &check )[ i ];

				if ( N_BMP_ALPHA_CHANNEL_VISIBLE == n_bmp_a( color ) ) { break; }

				i++;

				b += sizeof( u32 );
				if ( b >= pal_byte ) { break; }
			}

			if ( b == pal_byte )
			{
				n_type_gfx i = 0;
				n_type_gfx b = 0;
				n_posix_loop
				{

					u32 color = N_BMP_PTR_PAL( &check )[ i ];

					color = n_bmp_alpha_visible_pixel( color );

					N_BMP_PTR_PAL( &check )[ i ] = color;

					i++;

					b += sizeof( u32 );
					if ( b >= pal_byte ) { break; }
				}
			}

			ret = n_bmp_pal2full( &check );

		} else
		if ( 24 == N_BMP_DEPTH( &check ) )
		{

			ret = n_bmp_32bit( &check );

			if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 )
			{
				if ( n_bmp_alpha_is_zero( &check ) ) { n_bmp_alpha_reverse( &check ); }
			}

		} else {

			ret = n_posix_false;

			if ( N_BMP_ALPHA_CHANNEL_VISIBLE == 255 )
			{
				if ( n_bmp_alpha_is_zero( &check ) ) { n_bmp_alpha_reverse( &check ); }
			}

		}

		// [Needed] : n_bmp_precalc() is needed
		n_bmp_precalc( &check );


		offset += ptr_byte;


		n_curico_mask_decode( &check, &p[ offset ] );

		if ( ( ret == n_posix_false )&&( msk != NULL ) )
		{
			n_bmp_free( msk );
//n_bmp_new( msk, N_BMP_SX( &check ), N_BMP_SY( &check ) );

			N_BMP_RLE  ( msk ) = 0;
			N_BMP_SX   ( msk ) = N_BMP_SX( &check );
			N_BMP_SY   ( msk ) = N_BMP_SY( &check );
			N_BMP_DEPTH( msk ) = 1;
			N_BMP_PAL  ( msk ) = 2;

			n_type_gfx msk_pal_byte = N_BMP_PAL( msk ) * sizeof( u32 );

			N_BMP_PTR_PAL( msk ) = n_memory_new( msk_pal_byte );
			N_BMP_PTR_PAL( msk )[ 0 ] = n_bmp_black;
			N_BMP_PTR_PAL( msk )[ 1 ] = n_bmp_white;

			n_type_gfx msk_byte = n_bmp_size( N_BMP_SX( msk ), N_BMP_SY( msk ), 1 );

			N_BMP_PTR( msk ) = n_memory_new( msk_byte );
			n_memory_copy( &p[ offset ], N_BMP_PTR( msk ), msk_byte );

//n_bmp_new( msk, N_BMP_SX( &check ), N_BMP_SY( &check ) );

			n_bmp_pal2full( msk );

		}


		if ( ret == n_posix_false )
		{
			if ( loaded_bpp  != NULL ) { (*loaded_bpp ) = bpp;  }
			if ( loaded_size != NULL ) { (*loaded_size) = size; }

			n_bmp_replace( &check, bmp );
		}

	} // if ( ii->icHeader.biSize != N_BMP_SIZE_INFOH )


	return ret;
}

n_type_gfx*
n_curico_seek( n_curico *curico, n_type_gfx count, n_type_gfx size, n_type_gfx *ret_found_count )
{

	n_type_gfx  found_cb    = count * sizeof( n_type_gfx );
	n_type_gfx *found       = n_memory_new_closed( found_cb );// n_memory_zero( found, found_cb );
	n_type_gfx  found_count = 0;


	n_type_gfx i = 0;
	n_posix_loop
	{

		s32 icon_size = curico[ i ].sx;
		if ( icon_size == 0 ) { icon_size = 256; }

		if ( icon_size == size ) { found[ found_count ] = i; found_count++; }


		i++;
		if ( i >= count ) { break; }
	}
//n_posix_debug_literal( " %s : %d ", fname, found_count );


	if ( found_count == 0 )
	{

		int nearest = -1;

		n_type_gfx i = 0;
		n_posix_loop
		{

			s32 icon_size = curico[ i ].sx;
			if ( icon_size == 0 ) { icon_size = 256; }

			if ( ( icon_size >= nearest )&&( icon_size <= size ) )
			{
				nearest = icon_size;
			}


			i++;
			if ( i >= count ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			s32 icon_size = curico[ i ].sx;
			if ( icon_size == 0 ) { icon_size = 256; }

			if ( icon_size == nearest )
			{
				found[ found_count ] = i; found_count++;
			}


			i++;
			if ( i >= count ) { break; }
		}

//n_posix_debug_literal( " Nearest : %s : %d ", fname, found_count );
	}


	if ( found_count == 0 )
	{

		n_type_gfx minimum = 256;

		n_type_gfx i = 0;
		n_posix_loop
		{

			s32 icon_size = curico[ i ].sx;
			if ( icon_size == 0 ) { icon_size = 256; }

			if ( icon_size < minimum ) { minimum = icon_size; }


			i++;
			if ( i >= count ) { break; }
		}

		i = 0;
		n_posix_loop
		{

			s32 icon_size = curico[ i ].sx;
			if ( icon_size == 0 ) { icon_size = 256; }

			if ( icon_size == minimum )
			{
				found[ found_count ] = i; found_count++;
			}


			i++;
			if ( i >= count ) { break; }
		}

//n_posix_debug_literal( " Minimum : %s : %d ", fname, found_count );
	}


	if ( ret_found_count != NULL ) { (*ret_found_count) = found_count; }


	return found;
}

n_posix_bool
n_curico_extract( const n_posix_char *fname, n_bmp *bmp, n_type_gfx size, n_type_gfx arg_bpp, n_type_gfx *loaded_bpp, n_type_gfx *loaded_size )
{
//return n_posix_true;
/*
n_posix_bool debug = n_posix_false;
n_posix_char *_name = n_string_path_name_new( fname );
if ( n_string_is_same_literal( "neko.multi.ico", _name ) )
{
	debug = n_posix_true;
}
n_string_path_free( _name );
*/
//if ( debug ) { n_posix_debug_literal( " 1 " ); }


	if ( size <= 0 ) { size = 256; }//{ return n_posix_true; }


	n_curico c; n_curico_zero( &c );

	if ( n_posix_false == n_curico_load( &c, bmp, fname ) )
	{
//n_posix_debug_literal( " n_curico_load() : %s ", fname );
//n_bmp_save_literal( bmp, "ret.bmp" );

		if ( loaded_bpp  != NULL ) { (*loaded_bpp ) = N_BMP_DEPTH( bmp ); }
		if ( loaded_size != NULL ) { (*loaded_size) = N_BMP_SX   ( bmp ); }

		return n_posix_false;
	}
//return n_posix_true;


	n_type_int fsize = n_posix_stat_size( fname );
	if ( fsize < ( N_CURICO_SIZE_HEADER + N_BMP_SIZE_INFOH ) ) { return n_posix_true; }
	if ( n_bmp_size_is_overflow( fsize ) ) { return n_posix_true; }


	FILE *fp = n_posix_fopen_read( fname );
	if ( fp == NULL ) { return n_posix_true; }


	// [!] : sniffer

	u16 zero; n_posix_fread( &zero, sizeof( u16 ), 1, fp );
	if ( zero != 0 )
	{
		n_posix_fclose( fp );
		return n_posix_true;
	}

	u16 type; n_posix_fread( &type, sizeof( u16 ), 1, fp );
	if ( ( type != N_CURICO_TYPE_ICO )&&( type != N_CURICO_TYPE_CUR ) )
	{
		n_posix_fclose( fp );
		return n_posix_true;
	}

	u16 count; n_posix_fread( &count, sizeof( u16 ), 1, fp );
	if ( count == 0 )
	{
		n_posix_fclose( fp );
		return n_posix_true;
	}

/*
	n_posix_fread( &c, N_CURICO_SIZE_DIR, 1, fp );

	if (
//(1)||
		( c.zero1 != 0 )
		||
		(
			( c.type != N_CURICO_TYPE_ICO )
			&&
			( c.type != N_CURICO_TYPE_CUR )
		)
		||
		( c.count == 0 )
	)
	{
//n_posix_debug_literal( " Error : %s ", fname );

		n_posix_fclose( fp );

		return n_posix_true;
	}
*/

	n_posix_fseek( fp, 0, SEEK_SET );

	u8 *p = n_memory_new_closed( fsize );
	n_posix_fread( p, fsize, 1, fp );

	n_posix_fclose( fp );


	// [!] : PNG icon support
	//
	//	PNG : 0x89 50 4e 47 : . P N G
	//	BMP : 0x28 00 00 00 : BITMAPINFOHEADER.biSize (40Byte)
//return n_posix_true;

	// [!] : an .ico file has only one .png file

//n_posix_debug_literal( " PNG Icon : %02x ", p[ N_CURICO_SIZE_HEADER + 0 ] );

	if (
		( count == 1 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 0 ] == 0x89 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 1 ] == 0x50 )
		&&
		( p[ N_CURICO_SIZE_HEADER + 2 ] == 0x4e )
		&&
		( p[ N_CURICO_SIZE_HEADER + 3 ] == 0x47 )
	)
	{
//n_posix_debug_literal( " PNG Icon " );

		n_posix_bool ret = n_posix_true;


		n_png png; n_png_zero( &png );

		ret = n_png_load_onmemory( &png, &p[ N_CURICO_SIZE_HEADER ], fsize - N_CURICO_SIZE_HEADER );
		if ( ret == n_posix_false )
		{
			ret = n_png_uncompress( &png, bmp );
		}

		if ( ret == n_posix_false )
		{
			if ( loaded_bpp  != NULL ) { (*loaded_bpp ) = N_CURICO_LOADED_PNG; }
			if ( loaded_size != NULL ) { (*loaded_size) = N_BMP_SX( bmp );     }
		}

		n_png_free( &png );


		n_memory_free_closed( p );

		return ret;

	}
//return n_posix_true;


	//n_memory_copy( p, &c, N_CURICO_SIZE_DIR );

	c.zero1 = zero;
	c.type  = type;
	c.count = count;

//n_posix_debug_literal( "%s : %d-%d-%d", fname, c.zero1, c.type, c.count );


	n_type_gfx  curico_cb    = c.count * sizeof( n_curico );
	n_curico   *curico       = n_memory_new_closed( curico_cb );// n_memory_zero( curico, curico_cb );
	n_type_gfx  curico_count = 0;

	n_type_gfx i = 0;
	n_posix_loop
	{

		n_memory_copy( p, &curico[ i ], N_CURICO_SIZE_DIR );

		u32 offset = N_CURICO_SIZE_DIR + ( i * N_CURICO_SIZE_ENTRY );
		if ( offset < fsize )
		{
			n_memory_copy( &p[ offset ], &curico[ i ].sx, N_CURICO_SIZE_ENTRY );
			curico_count++;
		}


		i++;
		if ( i >= c.count ) { break; }
	}


	if ( curico_count == 0 )
	{
//n_posix_debug_literal( " Error : %s ", fname );

		n_memory_free_closed( curico );

		return n_posix_true;
	}


	n_type_gfx  found_index = -1;
	n_type_gfx  found_count = -1;
	n_type_gfx *found       = n_curico_seek( curico, curico_count, size, &found_count );

	if ( found_count == 1 )
	{

		found_index = found[ 0 ];

	} else
	if ( found_count > 1 )
	{

		n_type_gfx maximum_bpp   = -1;
		n_type_gfx maximum_index = -1;

		n_type_gfx target_bpp    = -1;
		n_type_gfx target_index  = -1;

		n_type_gfx i = 0;
		n_posix_loop
		{

			n_type_gfx index = found[ i ];

			n_type_gfx bpp;

			u8 n = p[ curico[ index ].offset ];
			if ( n == 40 )
			{
//n_posix_debug_literal( " BMP : %d ", n );
				BITMAPINFOHEADER *bi = (BITMAPINFOHEADER*) &p[ curico[ index ].offset ];
				bpp = bi->biBitCount;
			} else {
//n_posix_debug_literal( " PNG : %d ", n );
				bpp = 32;
			}

			if ( maximum_bpp < bpp )
			{
				maximum_bpp   = bpp;
				maximum_index = i;
			}

			if ( arg_bpp == bpp )
			{
				target_bpp   = bpp;
				target_index = i;
			}

			i++;
			if ( i >= found_count ) { break; }
		}

		if ( target_bpp != -1 )
		{
			found_index =  target_index;
		} else {
			found_index = maximum_index;
		}

	}


	n_posix_bool ret = n_posix_true;
	if ( found_index != -1 )
	{
		ret = n_curico_bmp_load_onmemory( &p[ curico[ found_index ].offset ], curico[ found_index ].bmpsize, bmp, NULL, loaded_bpp, loaded_size );
	}


	n_memory_free_closed( found  );
	n_memory_free_closed( curico );
	n_memory_free_closed( p      );


//if ( ret ) { n_posix_debug_literal( " Error : %s ", fname ); }
	return ret;
}


#endif // _H_NONNON_NEUTRAL_CURICO

