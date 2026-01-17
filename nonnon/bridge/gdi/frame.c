// Nonnon Win32
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifndef _H_NONNON_WIN32_GDI_FRAME
#define _H_NONNON_WIN32_GDI_FRAME



#include "../../neutral/bmp/ui/rectframe.c"




#define n_gdi_frame_roundframe(      b, x,y,sx,sy, r,sz, fg,bg, blend       ) n_gdi_frame_roundframe_main( b, x,y,sx,sy, r,sz, fg,bg, blend, FALSE,    0 )
#define n_gdi_frame_roundframe_mask( b, x,y,sx,sy, r,sz, fg,bg, blend, mask ) n_gdi_frame_roundframe_main( b, x,y,sx,sy, r,sz, fg,bg, blend, TRUE , mask )

// internal
void
n_gdi_frame_roundframe_main
(
	n_bmp        *bmp,
	n_type_gfx    x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy,
	n_type_gfx    round_size,
	n_type_gfx    frame_size,
	u32           color_fg,
	u32           color_bg,
	n_type_real   blend,
	BOOL          cornermask_onoff,
	u32           cornermask_color
)
{

	//if ( n_bmp_error( bmp ) ) { return; }

	if ( sx == -1 ) { sx = N_BMP_SX( bmp ); }
	if ( sy == -1 ) { sy = N_BMP_SY( bmp ); }


	u32 color_base;

	if (0)//( n_win_darkmode_onoff )
	{
		color_base = n_bmp_black_invisible;
	} else {
		color_base = n_bmp_white_invisible;
	}


	if ( cornermask_onoff )
	{
		//
	} else {
		cornermask_color = color_base;
	}


	// [Needed] : more space for antialias

	n_type_gfx o  = 1;
	n_type_gfx oo = o + o;

	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx + oo, sy + oo ); n_bmp_flush( &b, color_base );

	{

		n_type_gfx r = round_size;
		n_type_gfx m = 0;

		n_bmp_roundrect( &b, o + m, o + m, sx - ( m*2 ), sy - ( m*2 ), color_fg, r );

		if ( frame_size )
		{
			m += frame_size;
			n_bmp_roundrect( &b, o + m, o + m, sx - ( m*2 ), sy - ( m*2 ), color_base, r );
		}

	}

	n_bmp_blendcopy( &b, bmp, o,o,sx,sy, x,y, blend );

//static int z = 0; if ( z == 0 ) { z++; n_bmp_save_literal( &b, "b.bmp" ); }

	n_bmp_free( &b );


	n_bmp_cornermask( bmp, round_size, 0, cornermask_color );


	return;
}

// internal
void
n_gdi_frame_roundframe_luna_edge( n_bmp *bmp, n_type_gfx x, n_type_gfx y, u32 color )
{

	u32 clr;

	n_bmp_ptr_get( bmp, x,y, &clr );

	clr = n_bmp_blend_pixel( color, clr, 0.75 );

	n_bmp_ptr_set( bmp, x,y,  clr );


	return;
}

void
n_gdi_frame_roundframe_luna
(
	n_gdi      *gdi,
	n_bmp      *bmp,
	n_type_gfx  sx,
	n_type_gfx  sy,
	n_type_gfx  offset,
	u32         clr_fg,
	u32         clr_bg,
	u32         clr_luna,
	u32         clr_mrgn,
	int         scaling
)
{

	n_type_gfx o  = 1;
	n_type_gfx oo = o * 2;

	n_type_gfx r  = gdi->frame_round;
	n_type_gfx sz = gdi->frame_size;


	// [!] : button shadow and highlight

	if ( offset == 0 )
	{
//n_bmp_box( bmp, 0,0,sx,sy, n_bmp_rgb( 255,0,0 ) );

		n_bmp_cornermask( bmp, r, 1, gdi->frame_corner_color );

		n_type_real d = 0.66;

		u32 clr_m = clr_mrgn;
		u32 clr_1 = n_bmp_blend_pixel( clr_m, n_bmp_black, d );
		u32 clr_2 = n_bmp_blend_pixel( clr_m, n_bmp_white, d );

		n_gdi_frame_roundframe_main( bmp, -1+o,-1+o,sx+2-oo,sy+2-oo, r,sz, clr_1, clr_m, 0.0, FALSE, 0 );
		n_gdi_frame_roundframe_main( bmp,  0+o, 0+o,sx+1-oo,sy+1-oo, r,sz, clr_2, clr_m, 0.0, FALSE, 0 );

	}


	n_type_gfx fx  =  0;
	n_type_gfx fy  =  0;
	n_type_gfx fsx = sx;
	n_type_gfx fsy = sy;

	fx  += offset;
	fy  += offset;
	fsx -= offset * 2;
	fsy -= offset * 2;


	// [Patch] : no better solution

	if ( offset != 0 )
	{
		if ( scaling == 1 )
		{

			//

		} else {

			n_gdi_frame_roundframe( bmp, 0,0,sx,sy, r, sz+1, clr_luna, clr_bg, 0.0 );

		}
	}


	if ( offset == 0 )
	{

		n_gdi_frame_roundframe_main( bmp, fx+o,fy+o,fsx-oo,fsy-oo, r, sz, clr_fg, clr_bg, 0.0, FALSE, 0 );

	} else {

		n_bmp gradient; n_bmp_zero( &gradient ); n_bmp_1st_fast( &gradient, sx, sy );

		u32 clr_gray = n_bmp_blend_pixel( n_bmp_black, n_bmp_white, 0.66 );

		n_bmp_flush_gradient( &gradient, clr_gray, n_bmp_white, N_BMP_GRADIENT_VERTICAL );
		n_bmp_roundrect( &gradient, fx+(sz*1)+o,fy+(sz*1)+o,fsx-(sz*2)-oo,fsy-(sz*2)-oo, n_bmp_black_invisible, r );
		n_bmp_cornermask( &gradient, r, sz+o, gdi->frame_corner_color );

		n_bmp_rasterizer( &gradient, bmp, 0,0, clr_fg, FALSE );

		n_bmp_free( &gradient );

	}


	// [Patch] : no better solution

	if ( scaling == 1 )
	{

		if ( offset == 0 )
		{
			offset = 2;

			fx  += offset;
			fy  += offset;
			fsx -= offset * 2;
			fsy -= offset * 2;
		}


		u32 clr1 = clr_luna;
		u32 clr2 = clr1; 

//clr1 = n_bmp_rgb( 255,0,0 );
//clr2 = n_bmp_rgb( 0,255,0 );

		n_gdi_frame_roundframe_luna_edge( bmp, fx  + 1, fy  + 0, clr1 );
		n_gdi_frame_roundframe_luna_edge( bmp, fx  + 0, fy  + 1, clr2 );

		n_gdi_frame_roundframe_luna_edge( bmp, fsx - 0, fy  + 0, clr1 );
		n_gdi_frame_roundframe_luna_edge( bmp, fsx + 1, fy  + 1, clr2 );

		n_gdi_frame_roundframe_luna_edge( bmp, fx  + 1, fsy + 1, clr1 );
		n_gdi_frame_roundframe_luna_edge( bmp, fx  + 0, fsy - 0, clr2 );

		n_gdi_frame_roundframe_luna_edge( bmp, fsx + 1, fsy - 0, clr1 );
		n_gdi_frame_roundframe_luna_edge( bmp, fsx - 0, fsy + 1, clr2 );

	} else {

		//

	}


	return;
}

void
n_gdi_frame_lineframe( n_gdi *gdi, n_bmp *bmp, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy, u32 fg )
{

	if ( n_bmp_error( bmp ) ) { return; }


	if ( sx <= gdi->scale ) { return; }
	if ( sy <= gdi->scale ) { return; }


	const n_type_gfx unit = gdi->scale;
	const n_type_gfx u    = unit * 1;
	const n_type_gfx uu   = unit * 2;

	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx, sy ); n_bmp_flush( &b, fg );

	n_bmp_box( &b, u, u, sx - uu, sy - uu, n_bmp_white_invisible );

	n_bmp_transcopy( &b, bmp, 0,0,sx,sy, x,y );


	n_bmp_free_fast( &b );


	return;
}

void
n_gdi_frame_bmp_cornermask_aqua( n_bmp *bmp, n_type_gfx round_size, n_type_gfx frame_size, u32 bg )
{

	if ( n_bmp_error( bmp ) ) { return; }

	if ( round_size == 0 ) { return; }


	const n_type_gfx sx = N_BMP_SX( bmp );
	const n_type_gfx sy = N_BMP_SY( bmp );


	n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy ); n_bmp_flush( &b, n_bmp_white );


	n_type_gfx tx  = frame_size;
	n_type_gfx ty  = frame_size;
	n_type_gfx tsx = sx - ( frame_size * 2 );
	n_type_gfx tsy = sy - ( frame_size * 2 );

	n_bmp_roundrect( &b, tx,ty,tsx,tsy, n_bmp_black, round_size );
//n_bmp_save_literal( &b, "b.bmp" );

	BOOL p_trans = b.transparent_onoff; b.transparent_onoff = FALSE;
	n_bmp_flush_antialias( &b, 1.0 );
	b.transparent_onoff = p_trans;

//n_bmp_save_literal( &b, "c.bmp" );

//n_bmp_rasterizer( &b, bmp, 0,0, bg, FALSE );


	n_bmp b_f; n_bmp_zero( &b_f ); n_bmp_carboncopy( bmp, &b_f );
	n_bmp b_t; n_bmp_zero( &b_t ); n_bmp_carboncopy( bmp, &b_t );

	u32        sh = n_bmp_black_invisible;
	n_type_gfx hf = sy / 2;

	n_bmp_rasterizer( &b, &b_f, 0,0, bg, FALSE );
	n_bmp_rasterizer( &b, &b_t, 0,0, sh, FALSE );

	//n_bmp_flush_antialias( &b_f, 1.0 );
	//n_bmp_flush_antialias( &b_t, 1.0 );

	n_bmp_fastcopy( &b_f, bmp, 0, 0,sx,hf, 0, 0 );
	n_bmp_fastcopy( &b_t, bmp, 0,hf,sx,sy, 0,hf );

	n_bmp_free_fast( &b_f );
	n_bmp_free_fast( &b_t );


	n_bmp_free_fast( &b );


	return;
}

void
n_gdi_frame_draw( n_gdi *gdi, n_bmp *bmp )
{

	// [Mechanism] : N_GDI_FRAME_*
	//
	// 	NOFRAME  : noframe
	//	SIMPLE   : WS_BORDER
	//	FLAT     : BS_FLAT
	//	BUTTON   : BS_PUSHBUTTON, WS_THICKFRAME, WS_EX_DLGMODALFRAME
	//	SINK     : SS_SUNKEN
	//	PUSH     : WS_EX_CLIENTEDGE
	//	ETCH     : SS_ETCHEDFRAME
	//	TRANS    : no frame with offset


	if ( gdi == NULL ) { return; }
	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : nothing to do

	if ( gdi->frame_style == N_GDI_FRAME_NOFRAME ) { return; }


	// Color Scheme

	u32 trans   = n_bmp_trans;
	u32 black   = n_bmp_rgb(   1,  1,  1 );
	u32 darker  = n_bmp_rgb(  50, 50, 50 );
	u32 dark    = n_bmp_rgb( 111,111,111 );
	u32 light   = n_bmp_rgb( 222,222,222 );
	u32 lighter = n_bmp_rgb( 255,255,255 );
	u32 round   = n_bmp_white_invisible;

#ifdef N_POSIX_PLATFORM_WINDOWS

	if ( gdi->style & N_GDI_SYSTEMCOLOR )
	{

		darker  = n_gdi_systemcolor( COLOR_3DDKSHADOW );
		dark    = n_gdi_systemcolor( COLOR_3DSHADOW   );
		light   = n_gdi_systemcolor( COLOR_3DLIGHT    );
		lighter = n_gdi_systemcolor( COLOR_3DHILIGHT  );

		if ( darker  == n_bmp_trans ) { darker  = black; }
		if ( dark    == n_bmp_trans ) { dark    = black; }
		if ( light   == n_bmp_trans ) { light   = black; }
		if ( lighter == n_bmp_trans ) { lighter = black; }
	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	if ( gdi->frame_style == N_GDI_FRAME_SIMPLE )
	{

		u32 color[ 8 ] = { black,black,black,black, trans,trans,trans,trans };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_FLAT )
	{

		u32 color[ 8 ] = { darker,darker,darker,darker, lighter,lighter,lighter,lighter };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_BUTTON )
	{

		u32 color[ 8 ] = { lighter,darker,lighter,darker, light,dark,light,dark };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_SCROLLARROW )
	{

		u32 color[ 8 ] = { light,darker,light,darker, lighter,dark,lighter,dark };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_PUSH )
	{

		u32 color[ 8 ] = { dark,lighter,dark,lighter, darker,light,darker,light };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_SINK )
	{

		u32 color[ 8 ] = { dark,lighter,dark,lighter, trans,trans,trans,trans };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_ETCH )
	{

		u32 color[ 8 ] = { dark,lighter,dark,lighter, lighter,dark,lighter,dark };
		n_bmp_ui_rectframe( bmp, gdi->scale, color );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_LUNA )
	{

		n_gdi_frame_roundframe_luna( gdi, bmp, gdi->sx,gdi->sy, 0, dark, lighter, dark, n_bmp_white_invisible, 1 );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_ROUND )
	{

		u32 bg = n_bmp_white_invisible;

		n_gdi_frame_roundframe( bmp, 0,0,-1,-1, gdi->frame_round, gdi->frame_size, bg, round, 0.5 );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_RPG )
	{

		u32 bg = lighter;

		n_gdi_frame_roundframe( bmp, 0,0,-1,-1, gdi->frame_round, gdi->frame_size, bg, round, 0.1 );

	} else
	if ( gdi->frame_style == N_GDI_FRAME_FLUENT_CHK )
	{
#ifdef N_POSIX_PLATFORM_WINDOWS

		// [!] : currently hard-coded

		u32 base = n_win_darkmode_systemcolor_colorref2argb_ui( COLOR_BTNFACE );

		u32 accnent;
		if ( gdi->frame_is_hot )
		{
			accnent = n_win_fluent_ui_accent_color();
		} else {
			accnent = dark;
		}

		if ( n_string_is_empty( gdi->text ) )
		{
			n_gdi_frame_roundframe_mask( bmp, 0,0,-1,-1, gdi->frame_round, gdi->frame_size, accnent, round, 0.0, base );
		} else {
			n_bmp_box( bmp, 0,0,gdi->sx,gdi->sy, base );
			n_bmp_roundrect( bmp, 0,0,gdi->sx,gdi->sy, accnent, gdi->frame_round );
		}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
	} else
	if ( gdi->frame_style == N_GDI_FRAME_FLUENT_BTN )
	{
#ifdef N_POSIX_PLATFORM_WINDOWS

		n_type_gfx sx = gdi->sx;
		n_type_gfx sy = gdi->sy;

		u32 outer = gdi->base_color_fg;
		u32 inner = gdi->base_color_bg;

		n_bmp b; n_bmp_zero( &b ); n_bmp_1st_fast( &b, sx,sy );
		n_bmp_flush( &b, n_bmp_white_invisible );

		b.transparent_onoff = FALSE;

		n_bmp_ui_roundframe( &b, 0,0,sx,sy, gdi->frame_round, gdi->frame_size, outer, inner );

		n_bmp_transcopy( &b, bmp, 0,0,sx,sy, 0,0 );

		n_bmp_free( &b );

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS
	} else
	if ( gdi->frame_style == N_GDI_FRAME_AQUA )
	{

		n_type_gfx sx = gdi->sx;
		n_type_gfx sy = gdi->sy - 3;

		u32 fg = gdi->base_color_bg;
		u32 mg = gdi->base_color_fg;
		u32 bg = n_bmp_white_invisible;


		n_bmp b;       n_bmp_zero( &b       ); n_bmp_1st_fast( &b,       sx,sy );
		n_bmp bmp_tmp; n_bmp_zero( &bmp_tmp ); n_bmp_1st_fast( &bmp_tmp, sx,sy );


		// [!] : Gradient background

		if ( gdi->base_style == N_GDI_BASE_DEFAULT )
		{

			{

				u32 color_fg = n_bmp_blend_pixel( fg,       black, 0.25 );
				u32 color_bg = n_bmp_blend_pixel( fg, n_bmp_white, 0.75 );

				n_bmp_flush_gradient( &b, color_fg, color_bg, N_BMP_GRADIENT_VERTICAL );

			}

			{

				u32 color_fg = fg;
				u32 color_bg = mg;

				int option = N_BMP_GRADIENT_CIRCLE | N_BMP_GRADIENT_CENTERING;
				n_bmp_flush_gradient( &bmp_tmp, color_fg, color_bg, option );
				n_bmp_flush_blendcopy( &bmp_tmp, &b, 0.5 );

			}

		} else {

			n_bmp_flush_fastcopy( bmp, &b );

		}


		// [!] : Corner mask

		n_gdi_frame_bmp_cornermask_aqua( &b, gdi->frame_round, gdi->frame_size, gdi->frame_corner_color );


		// [!] : Highlight

		n_type_gfx sugar_m  = gdi->frame_size * 2;
		n_type_gfx sugar_mm =         sugar_m * 2;
		n_type_gfx half     = sy / 2;
		n_type_gfx sugar_sx = sx - ( half * 2 ) + sugar_mm;
		n_type_gfx sugar_sy = half;
		n_type_gfx sugar_x  = half - sugar_m;
		n_type_gfx sugar_y  = 4;

		// [Patch] : currently inaccurate
		if ( gdi->sx <= gdi->sy )
		{
			sugar_sx = sx - sugar_mm - ( half / 2 );
			sugar_sy = sy - sugar_mm - half;
			sugar_x  = sugar_m + ( half / 4 );
			sugar_y  = sugar_m + ( half / 4 );
		}

		n_bmp_new_fast( &bmp_tmp, sugar_sx,sugar_sy );
		n_bmp_flush_gradient( &bmp_tmp, n_bmp_white_invisible, n_bmp_white, N_BMP_GRADIENT_VERTICAL );
		n_bmp_flush_mirror( &bmp_tmp, N_BMP_MIRROR_UPSIDE_DOWN );
		n_bmp_cornermask( &bmp_tmp, gdi->frame_round, 0, gdi->frame_corner_color );
		n_bmp_blendcopy( &bmp_tmp, &b, 0,0,sugar_sx,sugar_sy, sugar_x,sugar_y, 0.25 );


		// [!] : Back buffer

		n_bmp_new_fast( bmp, gdi->sx,gdi->sy );
		n_bmp_flush( bmp, bg );


		// [!] : Write back : Drop shadow

		n_type_gfx shadow_m    = 1;
		n_type_gfx shadow_mm   = shadow_m * 2;
		n_type_gfx shadow_sx   = gdi->sx + shadow_mm;
		n_type_gfx shadow_sy   = gdi->sy;
		n_type_gfx shadow_size = 3;

		n_bmp shadow; n_bmp_zero( &shadow ); n_bmp_1st_fast( &shadow, shadow_sx,shadow_sy );
		n_bmp_flush( &shadow, bg );

		{
			u32        clr = n_bmp_blend_pixel( bg, black, 1.00 );
			n_type_gfx sz  = shadow_size;
			n_bmp_roundrect_ratio( &shadow, sz,sz,shadow_sx-(sz*2),shadow_sy-(sz*2), clr, 100 );
		}

		n_type_gfx i = 0;
		n_posix_loop
		{
			n_bmp_flush_antialias( &shadow, 1.0 );

			i++;
			if ( i >= shadow_size ) { break; }
		}

//n_bmp_save_literal( &shadow, "ret.bmp" );

		n_bmp_blendcopy( &shadow, bmp, 0,0,shadow_sx,shadow_sy, 0-shadow_m,0+2, 0.5 );

		n_bmp_free( &shadow );


		// [!] : Write back : Frame

		//n_bmp_alpha_visible( bmp );

		n_bmp_transcopy( &b, bmp, 0,0,sx,sy, 0,0 );


		// [!] : Cleanup

		n_bmp_free( &bmp_tmp );

	}


	return;
}

void
n_gdi_frame_draw_highlight( n_gdi *gdi, n_bmp *bmp )
{

	const n_type_gfx sx = gdi->sx;
	const n_type_gfx sy = gdi->sy / 2;

	const n_type_real blend = 0.8;

	const u32 round = n_bmp_white_invisible;


	if ( gdi->frame_style == N_GDI_FRAME_LUNA  )
	{
		n_gdi_frame_roundframe( bmp, 0,0,sx,sy, gdi->frame_round, 0, n_bmp_white, round, blend );
	} else
	if ( gdi->frame_style == N_GDI_FRAME_ROUND )
	{
		n_gdi_frame_roundframe( bmp, 0,0,sx,sy, gdi->frame_round, 0, n_bmp_white, round, blend );
	} else
	if ( gdi->frame_style == N_GDI_FRAME_RPG   )
	{
		n_gdi_frame_roundframe( bmp, 0,0,sx,sy, gdi->frame_round, 0, n_bmp_white, round, blend );
	} else
	//
	{
		n_bmp_mixer( bmp, 0,0,sx,sy, n_bmp_white, 1.0 - blend );
	}


	return;
}


#endif // _H_NONNON_WIN32_GDI_FRAME

