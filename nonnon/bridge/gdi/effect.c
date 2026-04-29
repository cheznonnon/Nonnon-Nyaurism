// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifndef _H_NONNON_WIN32_GDI_EFFECT
#define _H_NONNON_WIN32_GDI_EFFECT




void
n_gdi_effect_fogmaker( n_bmp *bmp, n_type_real effect_count, BOOL is_icon, BOOL smooth_onoff, BOOL fog_onoff )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : Clear Alpha + Monochrome + Fog


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
			color = n_bmp_trans;
		}

		if ( is_icon )
		{
			if ( FALSE == n_bmp_is_trans( bmp, color ) )
			{
				color = n_bmp_white;
			}
		}

		if ( ( color != n_bmp_trans )&&( coeff != 1.0 ) )
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

#define N_GDI_EFFECT_ICON 0
#define N_GDI_EFFECT_TEXT 1

#define n_gdi_bmp_effect_icon( gdi, bmp, obj ) n_gdi_bmp_effect( gdi, bmp, obj, N_GDI_EFFECT_ICON )
#define n_gdi_bmp_effect_text( gdi, bmp, obj ) n_gdi_bmp_effect( gdi, bmp, obj, N_GDI_EFFECT_TEXT )

// internal
void
n_gdi_bmp_effect_text_go
(
	const n_gdi *gdi, n_bmp *bmp, n_bmp *obj, int mode,
	int   style,
	n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy,
	n_type_gfx o,
	const u32 *color,
	int        matrix
)
{

	if ( o <= 0 ) { return; }


	n_type_gfx fx_size      = ( o + 1 + o );
	BOOL       is_icon      = ( mode == N_GDI_EFFECT_ICON );
	BOOL       smooth_onoff = ( style & N_GDI_ICON_SMOOTH );
	BOOL       fog_onoff    = ( ( style & N_GDI_ICON_SHADOW_FOG )||( style & N_GDI_ICON_CONTOUR_FOG ) );
//n_posix_debug_literal( " %d %d ", ( style & N_GDI_ICON_SHADOW_FOG ), ( style & N_GDI_ICON_CONTOUR_FOG ) );

	n_bmp fog; n_bmp_carboncopy( obj, &fog );

	n_bmp_resizer( &fog, o + sx + o, o + sy + o, n_bmp_trans, N_BMP_RESIZER_CENTER );

	n_gdi_effect_fogmaker( &fog, fx_size * fx_size, is_icon, smooth_onoff, fog_onoff );

//if ( gdi->debug_id == 1 ) { n_bmp_save_literal( &fog, "fog.bmp" ); }


	n_type_real ratio_x = (n_type_real) matrix / fx_size;
	n_type_real ratio_y = (n_type_real) matrix / fx_size;

	n_type_gfx fx_x = -o;
	n_type_gfx fx_y = -o;
	n_posix_loop
	{//break;

		n_type_gfx c_x = (n_type_gfx) trunc( (n_type_real) ( o + fx_x ) * ratio_x );
		n_type_gfx c_y = (n_type_gfx) trunc( (n_type_real) ( o + fx_y ) * ratio_y );

		u32 clr = color[ c_x + ( matrix * c_y ) ];

		n_bmp_rasterizer( &fog, bmp, x + fx_x - o, y + fx_y - o, clr );

		fx_x++;
		if ( fx_x > o )
		{

			fx_x = -o;

			fx_y++;
			if ( fx_y > o ) { break; }
		}
	}


	n_bmp_free_fast( &fog );


	return;
}

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
		if ( FALSE == n_bmp_is_trans( bmp, color ) )
		{
			N_BMP_PTR( bmp )[ i ] = n_bmp_blend_pixel( color, n_bmp_white, 0.33 );
		}

		i++;
		if ( i >= c ) { break; }
	}


	return;
}

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

			n_bmp_smoothshrink( obj, n_gdi_smoothness );
			n_gdi_bmp_alpha_enhancer( gdi, obj );

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

				n_bmp_box( obj, fx,0,mx*1,sy, n_bmp_trans );
				n_bmp_box( obj, tx,0,mx*2,sy, n_bmp_trans );

			}

		}

	}


	// [!] : this is shared by TEXT and ICON

	u32 clr_main;
	u32 clr_grad;
	u32 clr_shadow;
	u32 clr_contour;
	u32 clr_sink_tl;
	u32 clr_sink_br;

	if ( mode == N_GDI_EFFECT_ICON )
	{

		clr_main    = 0;
		clr_grad    = 0;
		clr_shadow  = gdi->icon_color_shadow;
		clr_contour = gdi->icon_color_contour;
		clr_sink_tl = gdi->icon_color_sink_tl;
		clr_sink_br = gdi->icon_color_sink_br;

	} else {

		clr_main    = gdi->text_color_main;
		clr_grad    = gdi->text_color_gradient;
		clr_shadow  = gdi->text_color_shadow;
		clr_contour = gdi->text_color_contour;
		clr_sink_tl = gdi->text_color_sink_tl;
		clr_sink_br = gdi->text_color_sink_br;

	}

	if ( clr_main    == n_bmp_trans ) { clr_main    = n_bmp_rgb( 1,1,1 ); }
	if ( clr_grad    == n_bmp_trans ) { clr_grad    = n_bmp_rgb( 1,1,1 ); }
	if ( clr_shadow  == n_bmp_trans ) { clr_shadow  = n_bmp_rgb( 1,1,1 ); }
	if ( clr_contour == n_bmp_trans ) { clr_contour = n_bmp_rgb( 1,1,1 ); }
	if ( clr_sink_tl == n_bmp_trans ) { clr_sink_tl = n_bmp_rgb( 1,1,1 ); }
	if ( clr_sink_br == n_bmp_trans ) { clr_sink_br = n_bmp_rgb( 1,1,1 ); }


	n_type_gfx o1 = 0;
	n_type_gfx o2 = 0;
	if ( mode == N_GDI_EFFECT_ICON )
	{
		o1 = gdi->icon_fxsize1;
		o2 = gdi->icon_fxsize2;
	} else {
		o1 = gdi->text_fxsize1;
		o2 = gdi->text_fxsize2;
	}


	if ( ( style & N_GDI_ICON_SHADOW )||( style & N_GDI_ICON_SHADOW_FOG ) )
	{

		o1 += o2;

		const u32 tr = n_bmp_trans;
		const u32 sh = clr_shadow;

		const u32 color[ 9 ] = {
			tr, tr, tr,
			tr, tr, tr,
			tr, tr, sh,
		};

		n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o1, color, 3 );

	}


	if ( gdi->text_style & N_GDI_TEXT_POP )
	{

		// [!] : under construction : this is not the last code

		{
			const u32 tr = n_bmp_trans;
			const u32 cn = clr_contour;

			const u32 color[ 9 ] = {
				cn, cn, cn,
				cn, tr, cn,
				cn, cn, cn,
			};

			n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o2*3, color, 3 );
		}

		{
			const u32 tr = n_bmp_trans;
			const u32 cn = n_bmp_white;

			const u32 color[ 9 ] = {
				cn, cn, cn,
				cn, tr, cn,
				cn, cn, cn,
			};

			n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o2*2, color, 3 );
		}

		{
			const u32 tr = n_bmp_trans;
			const u32 cn = clr_contour;

			const u32 color[ 9 ] = {
				cn, cn, cn,
				cn, tr, cn,
				cn, cn, cn,
			};

			n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o2*1, color, 3 );
		}

	} else

	if ( ( style & N_GDI_ICON_CONTOUR )||( style & N_GDI_ICON_CONTOUR_FOG ) )
	{

		const u32 tr = n_bmp_trans;
		const u32 cn = clr_contour;

		const u32 color[ 9 ] = {
			cn, cn, cn,
			cn, tr, cn,
			cn, cn, cn,
		};

		n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o2, color, 3 );

	} else
	if ( style & N_GDI_ICON_SINK )
	{

		const u32 tr = n_bmp_trans;
		const u32 tl = clr_sink_tl;
		const u32 br = clr_sink_br;

		const u32 color[ 9 ] = {
			tl, tl, tl,
			tl, tr, br,
			tl, br, br,
		};

		n_gdi_bmp_effect_text_go( gdi, bmp, obj, mode, style, x,y,sx,sy, o2, color, 3 );

	}// else


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

