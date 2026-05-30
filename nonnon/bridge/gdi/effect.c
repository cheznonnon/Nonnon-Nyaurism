// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifndef _H_NONNON_WIN32_GDI_EFFECT
#define _H_NONNON_WIN32_GDI_EFFECT




void
n_gdi_bmp_alpha_enhancer( const n_gdi *gdi, n_bmp *bmp )
{
//return;

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : font smoothing is not beautiful when background color is dark

	u32 ahsl = n_bmp_argb2ahsl( gdi->base_color_bg );
	if ( 64 <= n_bmp_l( ahsl ) ) { return; }


	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];
		if ( color != n_bmp_black )
		{
			N_BMP_PTR( bmp )[ i ] = n_bmp_blend_pixel( color, n_bmp_white, 0.33 );
		}

		i++;
		if ( i >= c ) { break; }
	}


	return;
}




void
n_gdi_effect_fogmaker( n_bmp *bmp, n_type_real effect_count, BOOL is_icon, BOOL smooth_onoff, BOOL fog_onoff )
{

	if ( n_bmp_error( bmp ) ) { return; }

//NSLog( @"%f %d %d %d", effect_count, is_icon, smooth_onoff, fog_onoff );


	// [!] : combined : Clear Alpha + Monochrome + Fog


	// [!] : default behavior is too blurry

	n_type_real coeff = 1.0;

	if ( fog_onoff )
	{
		coeff = (n_type_real) ( 1.0 / n_posix_max_n_type_real( 1, effect_count ) ) * n_posix_max_n_type_real( 1, effect_count * 0.10 );
	} else
	if ( smooth_onoff )
	{
		coeff = (n_type_real) ( 1.0 / n_posix_max_n_type_real( 1, effect_count ) ) * n_posix_max_n_type_real( 1, effect_count * 0.50 );
	}


	n_type_int c = N_BMP_SX( bmp ) * N_BMP_SY( bmp );
	n_type_int i = 0;
	n_posix_loop
	{

		u32 color = N_BMP_PTR( bmp )[ i ];
		u32 p_clr = color;

		if ( N_BMP_ALPHA_CHANNEL_INVISIBLE == n_bmp_a( color ) )
		{
			color = n_bmp_black;
		}

		if ( is_icon )
		{
			if ( color != n_bmp_black )
			{
				color = n_bmp_white;
			}
		}

		if ( ( color != n_bmp_black )&&( coeff != 1.0 ) )
		{
			int a = n_bmp_clamp_channel( (int) ( (n_type_real) n_bmp_r( color ) * coeff ) );
			color = n_bmp_argb( a, a, a, a );
		}


		if ( p_clr != color ) { N_BMP_PTR( bmp )[ i ] = color; }


		i++;
		if ( i >= c ) { break; }
	}


	return;
}




// internal
void
n_gdi_bmp_effect_go( n_bmp *fog, n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx radius, const u32 map_color[] )
{
//return;

	if ( radius <= 0 ) { return; }


	n_type_gfx fx_x = -radius;
	n_type_gfx fx_y = -radius;
	n_posix_loop
	{//break;

		n_type_gfx m_x = ( fx_x + radius ) / radius;
		n_type_gfx m_y = ( fx_y + radius ) / radius;

		u32 color = map_color[ m_x + ( 3 * m_y ) ];

		n_bmp_rasterizer( fog, bmp, x + fx_x, y + fx_y, color );

		fx_x++;
		if ( fx_x > radius )
		{
			fx_x = -radius;

			fx_y++;
			if ( fx_y > radius ) { break; }
		}
	}


	return;
}




n_type_gfx
n_gdi_effect_size_sum_icon( const n_gdi *gdi )
{

	n_type_gfx ret = 0;

	int i = 0;
	n_posix_loop
	{
		if ( gdi->icon_effect_style[ i ] == N_GDI_EFFECT_NONE ) { break; }

		ret += gdi->icon_effect_param[ i ];

		i++;
		if ( i >= N_GDI_EFFECT_MAX ) { break; }
	}


	return ret * 2;
}

n_type_gfx
n_gdi_effect_size_sum_text( const n_gdi *gdi )
{

	n_type_gfx ret = 0;

	int i = 0;
	n_posix_loop
	{
		if ( gdi->text_effect_style[ i ] == N_GDI_EFFECT_NONE ) { break; }

		ret += gdi->text_effect_param[ i ];

		i++;
		if ( i >= N_GDI_EFFECT_MAX ) { break; }
	}


	return ret * 2;
}




#define N_GDI_EFFECT_ICON 0
#define N_GDI_EFFECT_TEXT 1

#define n_gdi_bmp_effect_icon( gdi, bmp, obj ) n_gdi_bmp_effect( gdi, bmp, obj, N_GDI_EFFECT_ICON )
#define n_gdi_bmp_effect_text( gdi, bmp, obj ) n_gdi_bmp_effect( gdi, bmp, obj, N_GDI_EFFECT_TEXT )

void
n_gdi_bmp_effect( const n_gdi *gdi, n_bmp *bmp, n_bmp *obj, int mode )
{

	// [!] : TEXT : "obj" will be broken


	if ( gdi == NULL ) { return; }

	if ( n_bmp_error( bmp ) ) { return; }
	if ( n_bmp_error( obj ) ) { return; }


	int        style;
	n_type_gfx x,y;

	if ( mode == N_GDI_EFFECT_ICON )
	{
		style = gdi->icon_style;
		x     = gdi->icon_x;
		y     = gdi->icon_y;
	} else {
		style = gdi->text_style;
		x     = gdi->text_x;
		y     = gdi->text_y;
	}


	n_type_gfx sx = N_BMP_SX( obj );
	n_type_gfx sy = N_BMP_SY( obj );

	if ( mode == N_GDI_EFFECT_TEXT )
	{

		if ( style & N_GDI_TEXT_SMOOTH )
		{
#ifdef N_POSIX_PLATFORM_WINDOWS

			//n_bmp_smoothshrink( obj, n_gdi_smoothness );
			//n_gdi_bmp_alpha_enhancer( gdi, obj );

			int i = 0;
			n_posix_loop
			{
				n_bmp_flush_antialias( obj, 1.0 );

				i++;
				if ( i >= n_gdi_smoothness ) { break; }
			}

			n_type_real ratio = 1.0 / n_gdi_smoothness;
			n_bmp_resampler( obj, ratio, ratio );

//n_posix_char str[ 100 ]; n_posix_snprintf_literal( str, 100, "%08x.bmp", n_posix_tickcount() );
//n_bmp_save( obj, str );

			sx = N_BMP_SX( obj );
			sy = N_BMP_SY( obj );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
		}

		if (
			( gdi->frame_style == N_GDI_FRAME_LUNA  )
			||
			( gdi->frame_style == N_GDI_FRAME_ROUND )
		)
		{

			// [!] : clipping

			if ( x < 0 )
			{

				n_type_gfx mx = gdi->frame_round;
				n_type_gfx fx = abs( x );
				n_type_gfx tx = ( fx + gdi->sx ) - mx;

				n_bmp_box( obj, fx,0,mx*1,sy, n_bmp_white_invisible );
				n_bmp_box( obj, tx,0,mx*2,sy, n_bmp_white_invisible );

			}

		}

	}


	// [!] : this is shared by TEXT and ICON

	u32 clr_main;
	u32 clr_grad;

	if ( mode == N_GDI_EFFECT_ICON )
	{
		clr_main = 0;
		clr_grad = 0;
	} else {
		clr_main = gdi->text_color_main;
		clr_grad = gdi->text_color_gradient;
	}


	// [!] : Pop Maker

	BOOL is_icon      = ( mode == N_GDI_EFFECT_ICON );
	BOOL smooth_onoff = ( ( style & N_GDI_ICON_SMOOTH )||( style & N_GDI_TEXT_SMOOTH ) );

	n_type_gfx sum;
	if ( mode == N_GDI_EFFECT_ICON )
	{
		sum = gdi->effect_size_sum_icon;
	} else {
		sum = gdi->effect_size_sum_text;
	}
//NSLog( @"%d", sum );

	if ( mode == N_GDI_EFFECT_ICON )
	{
		//
	} else {
		n_bmp_resizer( obj, sx + sum, sy + sum, n_bmp_black, N_BMP_RESIZER_CENTER );
	}

	n_bmp fog_base; n_bmp_carboncopy( obj, &fog_base );

	int effect_style[ N_GDI_EFFECT_MAX ];
	u32 effect_color[ N_GDI_EFFECT_MAX ];
	int effect_param[ N_GDI_EFFECT_MAX ];

	if ( mode == N_GDI_EFFECT_ICON )
	{
		n_memory_copy( gdi->icon_effect_style, effect_style, sizeof( int ) * N_GDI_EFFECT_MAX );
		n_memory_copy( gdi->icon_effect_color, effect_color, sizeof( u32 ) * N_GDI_EFFECT_MAX );
		n_memory_copy( gdi->icon_effect_param, effect_param, sizeof( int ) * N_GDI_EFFECT_MAX );
	} else {
		n_memory_copy( gdi->text_effect_style, effect_style, sizeof( int ) * N_GDI_EFFECT_MAX );
		n_memory_copy( gdi->text_effect_color, effect_color, sizeof( u32 ) * N_GDI_EFFECT_MAX );
		n_memory_copy( gdi->text_effect_param, effect_param, sizeof( int ) * N_GDI_EFFECT_MAX );
	}

	int p = sum / 2;
	int i = N_GDI_EFFECT_MAX - 1;
	n_posix_loop
	{//break;

		if ( i < 0 ) { break; }

		if ( effect_style[ i ] == N_GDI_EFFECT_NONE ) { i--; continue; }


		int style = effect_style[ i ];
		u32 color = effect_color[ i ];
		int param = p;
//NSLog( @"%d %d", style, param );

		BOOL fog_onoff = ( ( style & N_GDI_EFFECT_OUTLINE_FOG )||( style & N_GDI_EFFECT_SHADOW_FOG ) );

		n_bmp fog; n_bmp_carboncopy( &fog_base, &fog );
		n_gdi_effect_fogmaker( &fog, 9, is_icon, smooth_onoff, fog_onoff );

		if ( ( style & N_GDI_EFFECT_OUTLINE )||( style & N_GDI_EFFECT_OUTLINE_FOG ) )
		{
//NSLog( @"OUTLINE" );
//if ( style & N_GDI_EFFECT_OUTLINE_FOG ) { NSLog( @"OUTLINE_FOG" ); }

			const u32 ol = color;

			const u32 color_map[] = {
				ol, ol, ol,
				ol, ol, ol,
				ol, ol, ol,
			};

			n_gdi_bmp_effect_go( &fog, bmp, x,y, param, color_map );

		} else
		if ( style & N_GDI_EFFECT_SINK )
		{
//NSLog( @"SINK" );

			const u32 tl = n_bmp_blend_pixel( n_bmp_black, color, 0.5 );
			const u32 br = n_bmp_blend_pixel( n_bmp_white, color, 0.5 );

			const u32 color_map[] = {
				tl, tl, tl,
				tl, tl, br,
				tl, br, br,
			};

			n_gdi_bmp_effect_go( &fog, bmp, x,y, param, color_map );

		} else
		if ( ( style & N_GDI_EFFECT_SHADOW )||( style & N_GDI_EFFECT_SHADOW_FOG ) )
		{
//NSLog( @"SHADOW" );

			const u32 tr = n_bmp_white_invisible;
			const u32 sh = color;

			const u32 color_map[] = {
				tr, tr, tr,
				tr, tr, sh,
				tr, sh, sh,
			};

			n_gdi_bmp_effect_go( &fog, bmp, x,y, param, color_map );

		}

		n_bmp_free_fast( &fog );

//break;

		p -= effect_param[ i ];

		i--;

	}

	n_bmp_free_fast( &fog_base );


	if ( mode == N_GDI_EFFECT_ICON )
	{

		if ( style & N_GDI_ICON_FASTMODE )
		{
			n_bmp_fastcopy( obj, bmp, 0,0,sx,sy, x,y );
		} else {
			if ( style & N_GDI_ICON_NOFINALIZE )
			{
				n_bmp_transcopy_no_finalize( obj, bmp, 0,0,sx,sy, x,y );
			} else {
				n_bmp_transcopy            ( obj, bmp, 0,0,sx,sy, x,y );
			}
		}

//n_posix_char *name = n_string_path_name_new( gdi->icon );
//n_posix_char str[ 1024 ]; n_posix_snprintf_literal( str, 1024, "./_empty/%s.bmp", name );
//n_bmp_save( obj, str );
//n_string_path_free( name );

	} else {

#ifdef N_POSIX_PLATFORM_WINDOWS
		if ( style & N_GDI_TEXT_SYS_SMOOTH )
		{
			//n_bmp_transcopy( obj, bmp, 0,0,sx,sy, x,y );

			extern void n_gdi_text_draw_line_direct( const n_gdi *gdi, n_bmp *bmp, n_posix_char *str );
			n_gdi_text_draw_line_direct( gdi, bmp, gdi->text );

			extern void n_gdi_bmp_alpha_visible( n_bmp*, u32 );
			n_gdi_bmp_alpha_visible( bmp, gdi->base_color_bg );
		} else
#endif
		if ( style & N_GDI_TEXT_GRADIENT )
		{
			n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy );
			n_bmp_flush_gradient( &b, clr_main,clr_grad, N_BMP_GRADIENT_VERTICAL );
			n_bmp_rasterizer_main( obj, bmp, x,y, &b, 0 );
			n_bmp_free_fast( &b );
		} else {
			n_bmp_rasterizer( obj, bmp, x,y, clr_main );
		}

	}


	return;
}


#endif // _H_NONNON_WIN32_GDI_EFFECT

