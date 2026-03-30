// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#define N_TXTBOX_INDICATOR_SEARCH n_mac_nscolor_argb( 255,255,  0,128 )
#define N_TXTBOX_INDICATOR_CARET  n_mac_nscolor_argb( 255,  0,200,255 )




void
n_mac_image_alpha_grow( n_bmp *bmp, u32 color_replace, CGFloat d )
{

	if ( n_bmp_error( bmp ) ) { return; }


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		u32 color; n_bmp_ptr_get_fast( bmp, x, y, &color );

		int a = n_posix_min( 255, trunc( (CGFloat) n_bmp_a( color ) * d ) );
		int r = n_bmp_r( color_replace );
		int g = n_bmp_g( color_replace );
		int b = n_bmp_b( color_replace );

		color = n_bmp_argb( a,r,g,b );

		n_bmp_ptr_set_fast( bmp, x, y, color );

		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{
			x = 0;
			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}
	}


	return;
}




NSColor*
n_txtbox_thin_highlight( n_txt *txt, n_txt *txt_deco, n_type_int i, NSColor *f, NSColor *t )
{

	NSColor *ret = f;

	if ( ( txt_deco != NULL )&&( txt->sy == txt_deco->sy ) )
	{
		n_posix_char *deco = n_txt_get( txt_deco, i );
		if ( ( 2 <= n_posix_strlen( deco ) )&&( deco[ 1 ] == '!' ) )
		{
			ret = n_mac_nscolor_blend( f, t, 0.2 );
		}
	}


	return ret;
}




// internal
n_posix_char*
n_mac_txtbox_path_ellipsis( n_posix_char *path, NSFont *font, CGFloat width_limit )
{

	// [!] : internal :  security : omitted


	if ( path[ 0 ] != '/' )
	{
//NSLog( @"1 : %c", path[ 0 ] );
		return n_string_path_carboncopy( path );
	}


	n_type_int c = n_string_path_split_count( path );
	if ( c <= 1 )
	{
//NSLog( @"2 : %lld", c );
		return n_string_path_carboncopy( path );
	}


	n_posix_char **line = n_memory_new( sizeof(n_posix_char*) * c );


	n_type_int i = 0;
	n_posix_loop
	{
		line[ i ] = n_string_path_split_new( path, i );

		i++;
		if ( i >= c ) { break; }
	}

/*
	i = 0;
	n_posix_loop
	{break;
		NSLog( @"%s", line[ i ] );

		i++;
		if ( i >= c ) { break; }
	}
*/

	n_posix_char *ret = NULL;


	i = 0;
	n_posix_loop
	{

		n_type_int   j = 0;
		n_type_int cch = 0;
		n_posix_loop
		{
			cch += n_posix_strlen( line[ j ] );

			j++;
			if ( j >= c ) { break; }

			cch += 1;
		}

		n_string_free( ret );
		ret = n_string_path_new( cch );

		n_type_int start_cch = cch;

		j = cch = 0;
		n_posix_loop
		{
			cch += n_posix_snprintf_literal( &ret[ cch ], start_cch - cch + 1, "%s", line[ j ] );

			j++;
			if ( j >= c ) { break; }

			cch += n_posix_snprintf_literal( &ret[ cch ], start_cch - cch + 1, "/" );
		}
//NSLog( @"%s", s ); //break;


		CGSize size = n_mac_image_text_pixelsize( n_mac_str2nsstring( ret ), font );
		if ( size.width < width_limit ) { break; }
//NSLog( @"%f %f", size.width, width_limit );

		n_memory_free( line[ 1 + i ] );
		line[ 1 + i ] = n_string_path_new( 3 ); n_posix_snprintf_literal( line[ 1 + i ], 3 + 1, "..." );


		i++;
		if ( i >= ( c - 1 ) ) { break; } // [!] : the last component will always be visible
	}


	// [!] : clean-up

	i = 0;
	n_posix_loop
	{
		n_string_path_free( line[ i ] );

		i++;
		if ( i >= c ) { break; }
	}

	n_string_path_free( line );


	return ret;
}




@implementation NonnonTxtbox (NonnonTxtboxDraw)


- (BOOL) isFlipped
{
	return YES;
}




- (void) NonnonTxtboxDrawScrollClamp
{

	if ( txtbox->scrollbar_used )
	{
//NSLog( @"%f", txtbox->scroll );

//NSLog( @"%f %f", txtbox->scr.pixel_pos, txtbox->scr.pixel_max - txtbox->scr.pixel_thumb );
		if ( txtbox->scr.pixel_pos > txtbox->scr.pixel_max )
		{
			txtbox->scr.pixel_pos = txtbox->scr.pixel_max;
			txtbox->scr. unit_pos = ( txtbox->scr.pixel_pos  / txtbox->scr.pixel_max ) * txtbox->scr.unit_max;
		}

		if ( txtbox->scr.unit_pos < 0 )
		{
			txtbox->scr.unit_pos = txtbox->scr.pixel_pos = 0;
		}

	} else {

		txtbox->scr.unit_pos = txtbox->scr.pixel_pos = 0;

	}


}

-(void) NonnonTxtboxScrollMetrics
{

	if ( txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE ) { return; }
	if ( txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX ) { return; }


	const CGFloat scroll_unit_max = (CGFloat) txtbox->txt_data->sy;

	const CGFloat scroll_pixel_sx = 12;
	const CGFloat scroll_pixel_sy = (CGFloat) self.frame.size.height - ( txtbox->offset_y * 2 );

	const CGFloat corner_size     = scroll_pixel_sx;

	txtbox->scr.pixel_shaft = scroll_pixel_sy - corner_size;
	txtbox->scr.pixel_minim = scroll_pixel_sx;


	// [!] : the first mode

	txtbox->scr.unit_step = 1.0;
	txtbox->scr.unit_page = (CGFloat) txtbox->scr.pixel_shaft / txtbox->font_size.height;
	txtbox->scr.unit_max  = scroll_unit_max;

//NSLog( @"%f %f", txtbox->scr.unit_page, txtbox->scr.unit_max ); return;

	if ( ceil( txtbox->scr.unit_page ) < txtbox->scr.unit_max )
	{
		txtbox->scrollbar_used = TRUE;
	} else {
		txtbox->scrollbar_used = FALSE;

		[self NonnonTxtboxDrawScrollClamp];

		return;
	}

	// [!] : the second mode

	txtbox->scr.pixel_max  = txtbox->scr.pixel_shaft;
	txtbox->scr.pixel_step = ( txtbox->scr.unit_step / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;
	txtbox->scr.pixel_page = ( txtbox->scr.unit_page / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;
	txtbox->scr.pixel_pos  = ( txtbox->scr.unit_pos  / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;

	txtbox->scr.pixel_thumb = txtbox->scr.pixel_page;

//NSLog( @"%f %f %f", txtbox->scr.pixel_max, txtbox->scr.pixel_step, txtbox->scr.pixel_page ); return;

	// [!] : the third mode

	txtbox->scr.pixel_thumb = n_posix_max_n_type_real( txtbox->scr.pixel_thumb, txtbox->scr.pixel_minim );
	txtbox->scr.unit_max    = scroll_unit_max - txtbox->scr.unit_page;

	// [Patch] : you can avoid almost troubles
	if ( txtbox->scrollbar_patch ) { txtbox->scr.unit_max++; }

	txtbox->scr.pixel_max  = txtbox->scr.pixel_shaft - txtbox->scr.pixel_thumb;
	txtbox->scr.pixel_step = ( txtbox->scr.unit_step / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;
	txtbox->scr.pixel_page = ( txtbox->scr.unit_page / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;
	txtbox->scr.pixel_pos  = ( txtbox->scr.unit_pos  / txtbox->scr.unit_max ) * txtbox->scr.pixel_max;

//NSLog( @"%0.2f %0.2f", txtbox->scr.pixel_thumb, txtbox->scr.pixel_max );


	[self NonnonTxtboxDrawScrollClamp];

}




- (void) NonnonTxtboxCaretDraw:(void*)zero rect:(NSRect)rect focus:(n_type_int)focus
{
//return;

	double d;

	if (
		( [self window].keyWindow == FALSE )
		||
		( n_txtbox_first_responder != self )
	)
	{
		d = 0.1;
	} else
	if ( txtbox->caret_blink_force_onoff )
	{
		d = 1.0;
	} else {
		if ( txtbox->caret_blink_onoff )
		{
//NSLog( @"On %d", caret_blink_fade.percent );
			d = txtbox->caret_blink_fade.percent * 0.01;
		} else {
//NSLog( @"Off %d", caret_blink_fade.percent );
			d = txtbox->caret_blink_fade.percent * 0.01;
			d = 1.0 - d;
		}
	}

//NSLog( @"Percent %f", d );


	NSColor *color_bg;

	if ( focus & 1 )
	{
		color_bg = txtbox->nscolor_stripe;
	} else {
		color_bg = txtbox->nscolor_back;
	}


	NSColor *color_caret;

	if ( txtbox->is_darkmode )
	{
		color_caret = [NSColor whiteColor];
	} else {
		color_caret = [NSColor blackColor];
	}


	NSColor *color = n_mac_nscolor_blend( color_bg, color_caret, d );

	n_mac_draw_box( color, rect );

}

- (void) NonnonTxtboxDrawFindIconMake:(void*)zero color:(u32)color_replace offset:(n_type_gfx)o
{
//return;

	n_bmp bmp; n_bmp_carboncopy( &txtbox->find_icon, &bmp );


	int replace_r = n_bmp_r( color_replace );
	int replace_g = n_bmp_g( color_replace );
	int replace_b = n_bmp_b( color_replace );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;
		u32 c; n_bmp_ptr_get_fast( &bmp, x,y, &c );

		int a = n_bmp_a( c );
		if ( a != 0 )
		{
			int r = replace_r;
			int g = replace_g;
			int b = replace_b;

			c = n_bmp_argb( a,r,g,b );

			n_bmp_ptr_set_fast( &bmp, x,y, c );
		}

		x++;
		if ( x >= N_BMP_SX( &bmp ) )
		{
			x = 0;
			y++;
			if ( y >= N_BMP_SY( &bmp ) ) { break; }
		}
	}



	CGFloat sz = self.frame.size.height - 6;
/*
	int i = 0;
	n_posix_loop
	{
		n_bmp_flush_antialias( &bmp, 1.0 );

		i++;
		if ( sz > ( N_BMP_SY( &bmp ) / i ) ) { break; }
	}

	n_mac_image_alpha_grow( &bmp, color_replace, 3 );
*/

	CGFloat c = ( self.frame.size.height - sz ) / 2;
	CGFloat p = 0;

	if ( txtbox->find_icon_is_pressed )
	{
		p = 1;
	}

	txtbox->find_icon_rect = NSMakeRect( o+p,c+p, sz,sz );


//n_bmp_save( &bmp, "/Users/nonnon/Desktop/ret.bmp" );
	txtbox->find_icon_cache_nsimage = n_mac_image_nbmp2nsimage( &bmp );


	// [!] : find_icon_cache_nsimage seems to use this pointer
	//n_bmp_free_fast( &bmp );


	return;
}

- (void) NonnonTxtboxDrawLine_Phase0:(n_type_int)i rect:(NSRect)rect
{
//return;

	NSColor *nscolor;
	if ( txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		nscolor = txtbox->nscolor_back;
	} else
	if ( i & 1 )
	{
		nscolor = txtbox->nscolor_stripe;
	} else {
		nscolor = txtbox->nscolor_back;
	}

	nscolor = n_txtbox_thin_highlight( txtbox->txt_data, txtbox->txt_deco, i, nscolor, txtbox->nscolor_accent );

	if ( txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		n_mac_draw_box( nscolor, self.frame );
	} else {
		n_mac_draw_box( nscolor, rect       );
	}

}

- (void) NonnonTxtboxDrawLine_Phase1:(n_type_int)i rect:(NSRect)rect
{
//return;

	n_posix_char *line = n_txt_get( txtbox->txt_data, i );

	n_type_int index = 0;
	CGFloat    sx    = 0;
	n_type_int tab   = 0;
	CGSize     char_size;
	n_type_int char_index;
	n_posix_loop
	{//break;
		if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

		n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );

		index += char_index;
		sx    += char_size.width;
	}


	// [!] : End-of-Line Marker

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		//
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
	{
		//
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		//
	} else {
		NSRect char_rect = rect;
		char_rect.origin.x += sx;

		char_rect = NSMakeRect(
			char_rect.origin.x    + txtbox->crlf_ox,
			char_rect.origin.y    + txtbox->crlf_oy,
			char_rect.size.width  - txtbox->crlf_ox,
			char_rect.size.height - txtbox->crlf_oy
		);

		[txtbox->crlf_nsstring drawInRect:char_rect withAttributes:txtbox->attr_crlf];
	}


#ifdef N_TXTBOX_IME_ENABLE

	if ( ( txtbox->ime_onoff )&&( i == txtbox->focus ) )
	{

		n_posix_char *line = n_mac_nsstring2str( txtbox->ime_nsstr );

		n_type_int index  = 0;
		CGFloat    sx     = txtbox->ime_caret_fr.pxl.x;
		n_type_int tab    = 0;
		n_type_int glyph  = 0;
		n_type_int ime_sx = sx;
		CGSize     char_size;
		n_type_int char_index;
		n_posix_loop
		{//break;
			if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

			n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );

			index += char_index;
			sx    += char_size.width;
			glyph += 1;
			if ( glyph == txtbox->ime_caret_offset ) { ime_sx = sx; }
		}

		n_string_free( line );


		// [!] : Fake Caret : IME version

		{
			n_type_gfx oy = (n_type_gfx) txtbox->scr.unit_pos * txtbox->font_size.height;
			n_type_gfx  x = (n_type_gfx) txtbox->padding + (n_type_gfx) ime_sx;
			n_type_gfx  y = (n_type_gfx) txtbox->offset_y + txtbox->ime_caret_fr.pxl.y - oy - txtbox->caret_centered_offset;
			n_type_gfx sx = (n_type_gfx) N_MAC_TXTBOX_CARET_SIZE;
			n_type_gfx sy = (n_type_gfx) txtbox->font_size.height;

			txtbox->ime_caret_rect = NSMakeRect( x,y,sx,sy );
		}

	}

#endif

}

- (void) NonnonTxtboxDrawLine_Phase2:(n_type_int)i rect:(NSRect)rect csx:(n_type_gfx)csx csy:(n_type_gfx)csy
{
//return;

	n_posix_char *line = n_txt_get( txtbox->txt_data, i );


	// [!] : don't use edit : currently not supported

	n_posix_char *line_ellipsis = NULL;
	if ( txtbox->path_ellipsis_onoff )
	{
		n_type_int cch = n_posix_strlen( line );
		if ( ( cch >= txtbox->path_ellipsis_offset )&&( line[ txtbox->path_ellipsis_offset ] == '/' ) )
		{
			line_ellipsis = n_mac_txtbox_path_ellipsis( &line[ txtbox->path_ellipsis_offset ], txtbox->font, (CGFloat) csx * 0.8 );
//NSLog( @"%s", line_ellipsis );
			if ( txtbox->path_ellipsis_offset != 0 )
			{
				n_type_int    c = cch * 10;
				n_posix_char *l = n_string_alloccopy( c, line );
				n_posix_snprintf_literal( &l[ txtbox->path_ellipsis_offset ], c + 1, "%s", line_ellipsis );
				n_string_path_free( line_ellipsis );
				line_ellipsis = l;
			}

			line = line_ellipsis;
		}
	}


	if ( ( txtbox->txt_deco != NULL )&&( txtbox->txt_data->sy == txtbox->txt_deco->sy ) )
	{
		n_posix_char *deco = n_txt_get( txtbox->txt_deco, i );

		// [x] : italic : no better way

		if ( ( self.txtbox->listbox_edit_onoff )&&( i == txtbox->focus ) )
		{
			NSNumber *n = [NSNumber numberWithDouble:0.0];
			[txtbox->attr setObject:n forKey:NSObliquenessAttributeName ];

			NSNumber *u = [NSNumber numberWithInt:NSUnderlineStyleNone];
			[txtbox->attr setObject:u forKey:NSUnderlineStyleAttributeName ];
		} else
		if ( deco[ 0 ] == ' ' )
		{
			NSNumber *n = [NSNumber numberWithDouble:0.2];
			[txtbox->attr setObject:n forKey:NSObliquenessAttributeName ];

			NSNumber *u = [NSNumber numberWithInt:NSUnderlineStyleNone];
			[txtbox->attr setObject:u forKey:NSUnderlineStyleAttributeName ];
		} else
		if ( deco[ 0 ] == 'B' )
		{
			NSNumber *n = [NSNumber numberWithDouble:0.0];
			[txtbox->attr setObject:n forKey:NSObliquenessAttributeName ];

			NSNumber *u = [NSNumber numberWithInt:NSUnderlineStyleNone];
			[txtbox->attr setObject:u forKey:NSUnderlineStyleAttributeName ];
		} else
		if ( deco[ 0 ] == 'u' )
		{
			NSNumber *n = [NSNumber numberWithDouble:0.2];
			[txtbox->attr setObject:n forKey:NSObliquenessAttributeName ];

			NSNumber *u = [NSNumber numberWithInt:NSUnderlineStyleSingle];
			[txtbox->attr setObject:u forKey:NSUnderlineStyleAttributeName ];
		} else
		if ( deco[ 0 ] == 'U' )
		{
			NSNumber *n = [NSNumber numberWithDouble:0.0];
			[txtbox->attr setObject:n forKey:NSObliquenessAttributeName ];

			NSNumber *u = [NSNumber numberWithInt:NSUnderlineStyleSingle];
			[txtbox->attr setObject:u forKey:NSUnderlineStyleAttributeName ];
		}
	}


	BOOL     gradient_onoff  = FALSE;
	NSRect   gradient_rect   = NSMakeRect( 0,0,0,0 );
	CGFloat  gradient_cut    = (CGFloat) csx - csy - csy;
	BOOL     gradient_go     = FALSE;
	BOOL     roundrect_onoff = FALSE;

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		roundrect_onoff = TRUE;

		n_type_int index = 0;
		CGFloat    sx    = 0;
		n_type_int tab   = 0;
		CGSize     char_size;
		n_type_int char_index;
		n_posix_loop
		{//break;
			if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

			NSRect char_rect = rect;

			n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );

			char_rect.origin.x   += sx;
			char_rect.origin.x   -= 1;
			char_rect.size.width  = char_size.width + 1;


			index += char_index;
			sx    += char_size.width;

//NSLog( @"%f %f", ceil( sx ), (CGFloat) csx - csy - csy );
			if ( ceil( sx ) > gradient_cut )
			{
				gradient_onoff = TRUE;

				gradient_rect = char_rect;
				gradient_rect.origin.x   = gradient_cut - csy;
				gradient_rect.size.width = csx - gradient_rect.origin.x;

				roundrect_onoff = FALSE;

				break;
			}
		}
	}


	int round_mode = N_MAC_DRAW_ROUNDRECT_LEFT;

	{
		n_type_int index = 0;
		CGFloat    sx    = 0;
		n_type_int tab   = 0;
		CGSize     char_size;
		n_type_int char_index;
		n_posix_loop
		{//break;
			if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

			NSRect char_rect = rect;

			n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );

			char_rect.origin.x   += sx;
			char_rect.origin.x   -= 1;
			char_rect.size.width  = char_size.width + 1;

			NSPoint pt = NSMakePoint( sx, i );

			sx += char_size.width;

			BOOL selected = n_mac_txtbox_is_selected( pt, txtbox->caret_fr.pxl.x, txtbox->caret_fr.cch.y, txtbox->caret_to.pxl.x, txtbox->caret_to.cch.y );
			if ( selected )
			{
				BOOL focus = ( n_txtbox_first_responder == self );

				NSColor *nscolor;
				if ( focus )
				{
					nscolor = txtbox->nscolor_accent;
				} else {
					nscolor = txtbox->nscolor_nofocus;
				}

				if ( txtbox->delete_circle_fade_pressed.stop == FALSE )
				{
					NSColor *f = nscolor;
					NSColor *t = [NSColor whiteColor];

					nscolor = n_mac_nscolor_blend( f, t, txtbox->delete_circle_fade_pressed.percent * 0.01 );
				}

				if ( roundrect_onoff )
				{
					const n_type_gfx round_param = 3;
					n_mac_draw_roundrect_partial( nscolor, char_rect, round_param, round_mode );

					if (
						( line[ index + 1 ] != N_STRING_CHAR_NUL )
						&&
						( line[ index + 2 ] == N_STRING_CHAR_NUL )
					)
					{
						round_mode = N_MAC_DRAW_ROUNDRECT_RIGHT;
					} else {
						round_mode = N_MAC_DRAW_ROUNDRECT_NONE;
					}
				} else {
					n_mac_draw_box( nscolor, char_rect );
				}
			}

			if ( ( ( gradient_onoff ) )&&( ceil( sx ) > gradient_rect.origin.x ) )
			{
				gradient_go = TRUE;
			}

			index += char_index;

		}
	}


	n_type_int index = 0;
	CGFloat    sx    = 0;
	n_type_int tab   = 0;
	CGSize     char_size;
	n_type_int char_index;
	n_posix_loop
	{//break;
		if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

		NSRect char_rect = rect;

		n_posix_char *character;
		character = n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );
//NSLog( @"#%lld : %ld", i, strlen( character ) );

		char_rect.origin.x   += sx;
		char_rect.origin.x   -= 1;
		char_rect.size.width  = char_size.width + 1;


		NSPoint pt = NSMakePoint( sx, i );

		BOOL selected = n_mac_txtbox_is_selected( pt, txtbox->caret_fr.pxl.x, txtbox->caret_fr.cch.y, txtbox->caret_to.pxl.x, txtbox->caret_to.cch.y );
		if ( ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )&&( self.txtbox->listbox_edit_onoff == FALSE ) )
		{
			if ( i == self.txtbox->focus )
			{
				selected = TRUE;
			}
		}


		if ( line[ index ] == N_STRING_CHAR_TAB )
		{

			NSRect rect_local = char_rect; rect_local.size.width = 0.5;

			n_mac_draw_box( txtbox->nscolor_crlf, rect_local );

		} else
		//
		{

			NSColor *nscolor;
			if ( selected )
			{
				nscolor = txtbox->nscolor_text_highlight;
			} else {
				nscolor = txtbox->nscolor_text_normal;
			}

			if ( txtbox->delete_circle_fade_pressed.stop == FALSE )
			{
				NSColor *f = nscolor;
				NSColor *t = [NSColor whiteColor];

				nscolor = n_mac_nscolor_blend( f, t, txtbox->delete_circle_fade_pressed.percent * 0.01 );
			}

			[txtbox->attr setObject:nscolor forKey:NSForegroundColorAttributeName];

			NSString *text = n_mac_str2nsstring( character );
			char_rect = [self NonnonTxtboxDrawTextAdjust:text rect:char_rect];

			// [!] : for "g" or "y" : maybe a little inaccurate
			char_rect.size.height *= 2;//txtbox->font.ascender;
			char_rect.size.width  *= 2;

			[text drawInRect:char_rect withAttributes:txtbox->attr];
//NSLog( @"%@", text );

		}

//NSLog( @"%lld", char_index );
		index += char_index;
		sx    += char_size.width;
	}


#ifdef N_TXTBOX_IME_ENABLE

	if ( ( txtbox->ime_onoff )&&( i == txtbox->focus ) )
	{

		n_posix_char *line = n_mac_nsstring2str( txtbox->ime_nsstr );

		n_type_int range_f = txtbox->ime_focus.location;
		n_type_int range_t = txtbox->ime_focus.location + txtbox->ime_focus.length;
		//if ( range_f == range_t ) { range_f = 0; }
//NSLog( @"%lld %lld", range_f, range_t );

		n_type_int index  = 0;
		CGFloat    sx     = txtbox->ime_caret_fr.pxl.x;
		n_type_int tab    = 0;
		n_type_int glyph  = 0;
		n_type_int ime_sx = sx;
		CGSize     char_size;
		n_type_int char_index;
		n_posix_loop
		{//break;
			if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

			NSRect char_rect = rect;

			n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );

			char_rect.origin.x   += sx;
			char_rect.origin.x   -= 1;
			char_rect.size.width  = char_size.width + 1;


			// [!] : underline based

			if ( ( glyph >= range_f )&&( glyph < range_t ) )
			{
				if ( txtbox->underline_fx == -1 ) { txtbox->underline_fx = char_rect.origin.x; }
				txtbox->underline_tx = char_rect.origin.x + char_rect.size.width;
			}

			[txtbox->attr_ime setObject:txtbox->nscolor_text_normal forKey:NSForegroundColorAttributeName];


			if ( i & 1 )
			{
				n_mac_draw_box( txtbox->nscolor_stripe, char_rect );
			} else {
				n_mac_draw_box( txtbox->nscolor_back  , char_rect );
			}


			index += char_index;
			sx    += char_size.width;
			glyph += 1;
			if ( glyph == txtbox->ime_caret_offset ) { ime_sx = sx; }
		}

		n_string_free( line );


		// [!] : Underline : draw later

		txtbox->underline_rect = NSMakeRect(
			txtbox->padding + txtbox->ime_caret_fr.pxl.x,
			rect.origin.y + rect.size.height - 2,
			sx - txtbox->ime_caret_fr.pxl.x,
			2
		);

	}

#endif

	n_string_free( line_ellipsis );


	if ( gradient_go )
	{
//n_mac_draw_box( nscolor_back, gradient_rect );
		n_mac_draw_gradient( n_mac_nscolor_argb( 0,0,0,0 ), txtbox->nscolor_back, gradient_rect );
	}

}

- (void) NonnonTxtboxDrawLine_Phase3:(n_type_int)i rect:(NSRect)rect
{
//return;

	n_posix_char *line = n_mac_nsstring2str( txtbox->ime_nsstr );

	n_type_int index  = 0;
	CGFloat    sx     = txtbox->ime_caret_fr.pxl.x;
	n_type_int tab    = 0;
	n_type_int glyph  = 0;
	n_type_int ime_sx = sx;
	CGSize     char_size;
	n_type_int char_index;
	n_posix_loop
	{//break;

		if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

		NSRect char_rect = rect;

		n_posix_char *character;
		character = n_mac_txtbox_character( txtbox->font, txtbox->font_size, line, index, &char_size, &char_index, &tab );
//NSLog( @"#%lld : %ld", i, strlen( character ) );

		char_rect.origin.x   += sx;
		char_rect.origin.x   -= 1;
		char_rect.size.width  = char_size.width + 1;


		[txtbox->attr_ime setObject:txtbox->nscolor_text_normal forKey:NSForegroundColorAttributeName];


		NSString *text = n_mac_str2nsstring( character );
		char_rect = [self NonnonTxtboxDrawTextAdjust:text rect:char_rect];

		// [!] : for "g" or "y" : maybe a little inaccurate
		char_rect.size.height *= 2;//txtbox->font.ascender;
		char_rect.size.width  *= 2;

		[text drawInRect:char_rect withAttributes:txtbox->attr_ime];

		index += char_index;
		sx    += char_size.width;
		glyph += 1;
		if ( glyph == txtbox->ime_caret_offset ) { ime_sx = sx; }
	}

	n_string_free( line );

}

-(void) NonnonTxtboxDrawLineNumber:(n_type_int)index rect:(NSRect)rect semi_indicator:(BOOL)semi_indicator
{
//return;

	if ( txtbox->option_linenumber == N_MAC_TXTBOX_DRAW_LINENUMBER_NONE ) { return; }


	n_type_int offset = txtbox->scr.unit_pos;

	n_type_int focus_f = (n_type_int) MIN( txtbox->caret_fr.cch.y, txtbox->caret_to.cch.y );
	n_type_int focus_t = (n_type_int) MAX( txtbox->caret_fr.cch.y, txtbox->caret_to.cch.y );


	n_type_int cch_y = index + offset;

	if ( txtbox->option_linenumber & N_MAC_TXTBOX_DRAW_LINENUMBER_ZEROBASED_INDEX )
	{
		//
	} else {
		cch_y++;
	}


	NSColor *nscolor_main = n_txtbox_thin_highlight( txtbox->txt_data, txtbox->txt_deco, index + txtbox->scr.unit_pos, txtbox->nscolor_back, txtbox->nscolor_accent );
	NSColor *nscolor_stripe;
	if ( ( index + offset ) & 1 )
	{
		nscolor_stripe = n_mac_nscolor_blend( txtbox->nscolor_text, nscolor_main, 0.90 );
	} else {
		nscolor_stripe = n_mac_nscolor_blend( txtbox->nscolor_text, nscolor_main, 0.95 );
	}

	n_mac_draw_box( nscolor_stripe, rect );


	if ( ( index + offset ) < txtbox->txt_data->sy )
	{

		NSColor *nscolor_txt;
		if ( ( ( index + offset ) >= focus_f )&&( ( index + offset ) <= focus_t ) )
		{
			// [!] : indicator

			n_type_gfx size = 2;

			NSRect r = rect; r.size.width = size;

			n_mac_draw_box( txtbox->nscolor_accent, r );

			nscolor_txt = n_mac_nscolor_blend( txtbox->nscolor_text, txtbox->nscolor_back, 0.33 );
		} else {
			nscolor_txt = n_mac_nscolor_blend( txtbox->nscolor_text, txtbox->nscolor_back, 0.66 );
		}

		if ( semi_indicator )
		{
			n_type_gfx size = 2;

			NSRect r = rect;
			r.origin.x += r.size.width - size;
			r.size.width = size;

			NSColor *color = n_mac_nscolor_blend( txtbox->nscolor_text, txtbox->nscolor_back, 0.66 );

			n_mac_draw_box( color, r );
		}


		BOOL over_ten_thousand = FALSE;
		if ( cch_y >= 10000 ) { over_ten_thousand = TRUE; }

		if ( cch_y >= 10000 ) { cch_y = cch_y % 10000; }

		NSString *nsstr;

		// [Patch] : not working accurately

		if ( cch_y < 1000 )
		{
			if ( over_ten_thousand )
			{
				nsstr = [[NSString alloc] initWithFormat:@" %04d ", (int) cch_y];
			} else {
				nsstr = [[NSString alloc] initWithFormat:@" % 4d ", (int) cch_y];
			}
		} else {
			nsstr = [[NSString alloc] initWithFormat:@" %d ", (int) cch_y];
		}


		NSMutableParagraphStyle *align = [[NSMutableParagraphStyle alloc] init];
		align.alignment = NSTextAlignmentRight;

		NSMutableDictionary *attr = [NSMutableDictionary dictionary];
		[attr setObject:txtbox->linenumber_font forKey:NSFontAttributeName           ];
		[attr setObject:nscolor_txt             forKey:NSForegroundColorAttributeName];
		[attr setObject:align                   forKey:NSParagraphStyleAttributeName ];

		rect.size.width -= txtbox->margin * 2;

		rect.origin.x -= txtbox->linenumber_single.width   / 2;
		rect.origin.y -= txtbox->linenumber_font.descender / 2;

		[nsstr drawInRect:rect withAttributes:attr];

	}


	return;
}

- (NSRect) NonnonTxtboxDrawTextAdjust:(NSString*) text rect:(NSRect) rect
{

	CGSize size = n_mac_image_text_pixelsize( text, txtbox->font );

	CGFloat cx = ( rect.size.width  - size.width  ) / 2;
	CGFloat cy = ( rect.size.height - size.height ) / 2;

	NSRect ret = NSMakeRect(
		rect.origin.x    + cx,
		rect.origin.y    + cy,
		rect.size.width  - cx,
		rect.size.height - cy
	);

	return ret;
}

- (BOOL) isOpaque {
//NSLog( @" isOpaque " );
	return NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
//NSLog( @"drawRect" );

	// [x] : partial redraw : it seems to be unimplementable
/*
if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
{
NSRect r = dirtyRect;
NSLog( @"%0.0f %0.0f %0.0f %0.0f", r.origin.x, r.origin.y, r.size.width, r.size.height );
}
*/

//NSRectClip( NSMakeRect( 100,100,200,200 ) ); // [!] : it seems to be working


	// [x] : Sonoma Xcode 15 : rect has window size at the first run

	dirtyRect = [super bounds]; // [!] : this variable is not used below


	//[[NSColor clearColor] set];
	//NSRectFill( dirtyRect );
//return;


	BOOL is_partial_redraw = FALSE;
/*
if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
{
	redraw_fy = 100;
	redraw_ty = redraw_fy + 10;
}
*/
	if ( ( txtbox->redraw_fy == -1 )||( txtbox->redraw_ty == -1 ) )
	{//return;
		txtbox->redraw_fy = 0;
		txtbox->redraw_ty = txtbox->txt_data->sy;
	} else {
		is_partial_redraw = TRUE;
	}
/*
if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
{
NSLog( @"%lld %lld", redraw_fy, redraw_ty );
}
*/

	BOOL delete_circle_onoff = FALSE;
	if ( txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( 0 != n_posix_strlen( n_txt_get( txtbox->txt_data, 0 ) ) )
		{
			delete_circle_onoff = TRUE;
		}
	}


	// [!] : Metrics

	[self NonnonTxtboxScrollMetrics];

	n_type_gfx csx = (n_type_gfx) self.frame.size.width;
	n_type_gfx csy = (n_type_gfx) self.frame.size.height;


	txtbox->caret_centered_offset = 0;

	if (
		( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		||
		( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )

	)
	{
		txtbox->caret_centered_offset = ( ( txtbox->font_size.height + ( txtbox->offset_y * 2 ) ) - csy ) / 2;
	} else {
		CGFloat minim = txtbox->font_size.height + ( txtbox->offset_y * 2 );
		if ( ( csx < minim )||( csy < minim ) ) { return; }
	}

	if ( txtbox->caret_fr.cch.y >= txtbox->txt_data->sy ) { txtbox->caret_fr.cch.y = txtbox->txt_data->sy - 1; }


	CGFloat findbox_border_blend = 0.0;


	n_type_int i = txtbox->scr.unit_pos;

	NSRect _rect_main = NSMakeRect( txtbox->padding, txtbox->offset_y - txtbox->caret_centered_offset, csx - ( txtbox->offset_x * 2 ), txtbox->font_size.height );
	if ( is_partial_redraw )
	{
		_rect_main.origin.y -= txtbox->font_size.height * i;
		_rect_main.origin.y += txtbox->font_size.height * txtbox->redraw_fy;
	}

	// [!] : max_sy : plus one line

	const NSRect  rect_main = _rect_main;
	const CGFloat max_base  = csy - ( txtbox->offset_y * 2 );
	const CGFloat max_sy    = max_base + txtbox->font_size.height;


	// [!] : Metrics 2 : Colors


	// [x] : don't move : old color is used when darkmode state is changed

	if ( txtbox->is_darkmode )
	{
		txtbox->nscolor_stripe = n_mac_nscolor_blend( txtbox->nscolor_back, txtbox->nscolor_text, 0.10 );
	} else {
		txtbox->nscolor_stripe = n_mac_nscolor_blend( txtbox->nscolor_back, txtbox->nscolor_text, 0.05 );
	}

	txtbox->nscolor_frame = n_mac_nscolor_blend( txtbox->nscolor_back, txtbox->nscolor_text, 0.25 );
	txtbox->nscolor_crlf  = txtbox->nscolor_frame;//n_mac_nscolor_blend( txtbox->nscolor_back, txtbox->nscolor_text, 0.25 );

	txtbox->attr_crlf = [NSMutableDictionary dictionary];
	[txtbox->attr_crlf setObject:txtbox->linenumber_font forKey:NSFontAttributeName           ];
	[txtbox->attr_crlf setObject:txtbox->nscolor_crlf    forKey:NSForegroundColorAttributeName];


	// [!] : Find Icon

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		// [!] : for reducing CPU usage

		BOOL focus_cur = ( n_txtbox_first_responder == self );
//NSLog( @"Prev %d : Cur %d", find_icon_focus_prev, focus_cur );


		BOOL fade_init = FALSE;

		static int fade_type = 0;

		if ( ( focus_cur )&&( txtbox->find_icon_focus_prev == FALSE ) ) 
		{
//NSLog( @"Up" );
			fade_init = TRUE;
			fade_type = 1;
		} else
		if ( ( focus_cur == FALSE )&&( txtbox->find_icon_focus_prev ) ) 
		{
//NSLog( @"Down" );
			fade_init = TRUE;
			fade_type = 2;
		}

		txtbox->find_icon_focus_prev = focus_cur;

		if ( fade_init )
		{
//NSLog( @"Init" );
			fade_init = FALSE;

			n_bmp_fade_init( &txtbox->find_icon_fade, n_bmp_black );
			n_bmp_fade_go  ( &txtbox->find_icon_fade, n_bmp_white );
		}


		BOOL blend_onoff = FALSE;

		CGFloat d = 1.0;
		if ( txtbox->find_icon_fade.stop == FALSE )
		{
			blend_onoff = TRUE;

			d = txtbox->find_icon_fade.percent * 0.01;
			if ( fade_type == 2 ) { d = 1.0 - d; }
		}

		u32 color_bg = n_mac_nscolor2argb( txtbox->nscolor_back );
		u32 color_fg = n_mac_nscolor2argb( txtbox->nscolor_text );

		u32 color_nrml = n_bmp_blend_pixel( color_fg, color_bg, 0.25 );
		u32 color_main;

		if ( blend_onoff )
		{
			color_main = n_bmp_blend_pixel( color_nrml, n_bmp_white, d );
		} else
		if ( focus_cur )
		{
			color_main = n_bmp_white;
		} else {
			color_main = color_nrml;
		}


		// [!] : for reducing CPU usage

		static u32  prv_color = 0;
		static BOOL prv_press = FALSE;

		if ( ( prv_color == 0 )||( prv_color != color_main ) )
		{
			[self NonnonTxtboxDrawFindIconMake:NULL color:color_main offset:txtbox->offset_x];
		} else
		if ( prv_press != txtbox->find_icon_is_pressed )
		{
			[self NonnonTxtboxDrawFindIconMake:NULL color:color_main offset:txtbox->offset_x];
		}
//NSLog( @"%x %x", prv_color, color_main );
//NSLog( @"%d %d", prv_press, find_icon_is_pressed );
//NSLog( @"%d", find_icon_fade.percent );

		prv_color = color_main;
		prv_press = txtbox->find_icon_is_pressed;


		NSColor *nscolor_bg;// = nscolor_back;
		NSColor *nscolor_fg = [NSColor controlAccentColor];

		if ( focus_cur )
		{
			nscolor_bg = txtbox->nscolor_back;
		} else {
			nscolor_bg = txtbox->nscolor_stripe;
		}

		if ( blend_onoff )
		{
			NSColor *nscolor = n_mac_nscolor_blend( nscolor_bg, nscolor_fg, d );
			n_mac_draw_circle( nscolor, txtbox->find_icon_rect );

			findbox_border_blend = d;
		} else
		if ( focus_cur )
		{
			n_mac_draw_circle( nscolor_fg, txtbox->find_icon_rect );

			findbox_border_blend = 1.0;
		} else {
			//n_mac_draw_circle( nscolor_bg, txtbox->find_icon_rect );

			findbox_border_blend = 0.0;
		}

		NSRect rect_local = n_mac_rect_resize( txtbox->find_icon_rect, -2 );
		[txtbox->find_icon_cache_nsimage drawInRect:rect_local];
	}


	// [!] : n_txt rendering engine

//NSLog( @"%f %f", caret_fr.pxl.x, caret_to.pxl.x );

	NSRect listbox_rect = NSMakeRect( 0,0,0,0 );

#ifdef N_TXTBOX_IME_ENABLE

	txtbox->underline_rect = NSMakeRect( -1,-1,-1,-1 );
	txtbox->underline_fx   = -1;
	txtbox->underline_tx   = -1;

#endif

	{ // [!] : Phase #0 : Background : before text rendering : for some fonts like "g"

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			//
		} else {
			//n_mac_draw_box( nscolor_back, self.frame );
//n_mac_draw_box( n_mac_nscolor_argb( 255,0,200,255 ), self.frame );

			//NSColor *nscolor = n_mac_nscolor_argb( 255,255,200,0 );
			NSColor *nscolor = txtbox->nscolor_back;

			NSRect rect_1 = NSMakeRect( 0,0,csx,txtbox->offset_y );
			NSRect rect_2 = NSMakeRect( 0,0,txtbox->offset_x,csy );

			n_mac_draw_box( nscolor, rect_1 );
			n_mac_draw_box( nscolor, rect_2 );

			NSRect rect_g = NSMakeRect( txtbox->offset_x + txtbox->linenumber_size.width, txtbox->offset_y, txtbox->margin, csy );
			n_mac_draw_box( nscolor, rect_g );
		}
//n_mac_draw_box( n_mac_nscolor_argb( 255,0,200,255 ), self.frame );


		NSRect rect_local = rect_main;

		if ( is_partial_redraw )
		{
			i = txtbox->redraw_fy;
			n_posix_loop
			{//break;

				if ( ( txtbox->redraw_fy <= i )&&( i < txtbox->redraw_ty ) )
				{
					[self NonnonTxtboxDrawLine_Phase0:i rect:rect_local];
				}

				rect_local.origin.y += txtbox->font_size.height;
				if ( rect_local.origin.y > max_sy ) { break; }

				i++;
				if ( i >= txtbox->redraw_ty ) { break; }
			}
		} else {
			i = txtbox->scr.unit_pos;
			n_posix_loop
			{//break;

				[self NonnonTxtboxDrawLine_Phase0:i rect:rect_local];

				rect_local.origin.y += txtbox->font_size.height;
				if ( rect_local.origin.y > max_sy ) { break; }

				i++;
			}
		}

	} // [!] : Background


	{ // [!] : Phase #1 : EOL marker : and calculation for caret

		NSRect rect_local = rect_main;

		i = txtbox->scr.unit_pos; if ( is_partial_redraw ) { i = txtbox->redraw_fy; }
//NSLog( @"%f %lld", scroll, n_mac_listbox_txt.sy );
		n_posix_loop
		{//break;

			if ( ( txtbox->redraw_fy <= i )&&( i < txtbox->redraw_ty )&&( i < txtbox->txt_data->sy ) )
			{
				[self NonnonTxtboxDrawLine_Phase1:i rect:rect_local];
			}

			rect_local.origin.y += txtbox->font_size.height;
			if ( rect_local.origin.y > max_sy ) { break; }

			i++;
			if ( i >= txtbox->redraw_ty ) { break; }
		}

	} // [!] : Phase #1


	// [!] : Fake Caret #1 : IME is later applied

//NSLog( @"%d %d", ime_onoff, ime_delay );

	if (
		( txtbox->ime_onoff )
		||
		( ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )&&( self.txtbox->listbox_edit_onoff == FALSE ) )
		||
		( txtbox->delete_circle_fade_pressed_phase )
	)
	{
		//
	} else
	//if ( [self NonnonTxtboxCaretIsOnScreen] )
	{
		if ( ( delete_circle_onoff )&&( txtbox->caret_pt.x > ( csx - csy + txtbox->offset_x ) ) )
		{
			//
		} else {
			[self NonnonTxtboxCaretCalculate];

			NSRect rect_local = NSMakeRect( txtbox->caret_pt.x, txtbox->caret_pt.y - txtbox->caret_centered_offset, N_MAC_TXTBOX_CARET_SIZE, txtbox->font_size.height );
			[self NonnonTxtboxCaretDraw:nil rect:rect_local focus:txtbox->focus];
		}
	}


	{ // [!] : Phase #2 : Main

		NSRect rect_local = rect_main;

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{

			i = txtbox->scr.unit_pos; if ( is_partial_redraw ) { i = txtbox->redraw_fy; }
//NSLog( @"%f %lld", scroll, n_mac_listbox_txt.sy );
			n_posix_loop
			{//break;

				if ( i == self.txtbox->focus )
				{
					if ( self.txtbox->listbox_edit_onoff )
					{
						listbox_rect = rect_local;
						listbox_rect.size.width = csx - txtbox->padding - txtbox->offset_x;
					} else {
						if ( txtbox->txt_data->readonly )
						{
							n_mac_draw_box( txtbox->nscolor_nofocus, rect_local );
						} else {
							n_mac_draw_box( txtbox->nscolor_accent , rect_local );
						}
					}
				}

				if ( ( txtbox->redraw_fy <= i )&&( i < txtbox->redraw_ty )&&( i < txtbox->txt_data->sy ) )
				{
					[self NonnonTxtboxDrawLine_Phase2:i rect:rect_local csx:csx csy:csy];
				}

				if ( i == self.txtbox->focus )
				{
					if ( self.txtbox->listbox_edit_onoff )
					{
						NSRect rect_local = listbox_rect;

						rect_local.origin.x--;
						rect_local.origin.y--;
						//rect_local.size.width += 2;
						rect_local.size.height += 2;

						n_mac_draw_frame( txtbox->nscolor_accent, rect_local );
					}
				}

				rect_local.origin.y += txtbox->font_size.height;
				if ( rect_local.origin.y > max_sy ) { break; }

				i++;
				if ( i >= txtbox->redraw_ty ) { break; }
			}

		} else {

			i = txtbox->scr.unit_pos; if ( is_partial_redraw ) { i = txtbox->redraw_fy; }
//NSLog( @"%f %lld", scroll, n_mac_listbox_txt.sy );
			n_posix_loop
			{//break;

				if ( ( txtbox->redraw_fy <= i )&&( i < txtbox->redraw_ty )&&( i < txtbox->txt_data->sy ) )
				{
					[self NonnonTxtboxDrawLine_Phase2:i rect:rect_local csx:csx csy:csy];
				}

				rect_local.origin.y += txtbox->font_size.height;
				if ( rect_local.origin.y > max_sy ) { break; }

				i++;
				if ( i >= txtbox->redraw_ty ) { break; }
			}

		}

	} // [!] : Phase #2


	// [!] : Fake Caret #2 : IME

	if ( txtbox->ime_onoff )
	{
		[self NonnonTxtboxCaretDraw:nil rect:txtbox->ime_caret_rect focus:txtbox->focus];
	}


	{ // [!] : Phase #3 : IME

#ifdef N_TXTBOX_IME_ENABLE

		NSRect rect_local = rect_main;

		i = txtbox->scr.unit_pos; if ( is_partial_redraw ) { i = txtbox->redraw_fy; }
//NSLog( @"%f %lld", scroll, n_mac_listbox_txt.sy );
		n_posix_loop
		{//break;

			if ( ( txtbox->ime_onoff )&&( i == txtbox->focus ) )
			{
				[self NonnonTxtboxDrawLine_Phase3:i rect:rect_local];
			}

			rect_local.origin.y += txtbox->font_size.height;
			if ( rect_local.origin.y > max_sy ) { break; }

			i++;
			if ( i >= txtbox->redraw_ty ) { break; }
		}

	} // [!] : Phase #3

#endif


	// [!] : Underline

	if ( txtbox->underline_rect.origin.x != -1 )
	{

		n_bmp_flip_onoff = TRUE;

		n_type_gfx  x = txtbox->underline_rect.origin.x;
		n_type_gfx  y = txtbox->underline_rect.origin.y;
		n_type_gfx sx = txtbox->underline_rect.size.width;
		n_type_gfx sy = 4;//underline_rect.size.height;

		n_type_gfx circle = sy;
		n_type_gfx step   = circle * 1.5;

		n_type_gfx xx = 0;
		n_posix_loop
		{//break;

			NSRect rct = NSMakeRect( x + xx,y,circle,circle );

			if (
				( txtbox->underline_fx != -1 )
				&&
				( ( ( x + xx ) >= txtbox->underline_fx )&&( ( x + xx ) <= txtbox->underline_tx ) )
			)
			{
				n_mac_draw_circle( txtbox->nscolor_ime_main, rct );
			} else {
				n_mac_draw_circle( txtbox->nscolor_ime_rest, rct );
			}

			xx += step;
			if ( xx >= sx ) { break; }
		}

		n_bmp_flip_onoff = FALSE;
	}


	// [!] : Line Number

	if (
		( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
		||
		( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	)
	{
//{static int i = 0;NSLog( @"%d", i );i++;}

		n_bmp_flip_onoff = TRUE;

		n_type_int     i = 0;
		n_type_gfx pxl_y = 0;
		n_posix_loop
		{

			BOOL semi_indicator = FALSE;

			if ( ( txtbox->txt_deco != NULL )&&( txtbox->txt_data->sy == txtbox->txt_deco->sy ) )
			{
				semi_indicator = TRUE;

				n_posix_char *deco = n_txt_get( txtbox->txt_deco, txtbox->scr.unit_pos + i );

				if ( ( self.txtbox->listbox_edit_onoff )&&( i == txtbox->focus ) )
				{
					//
				} else
				if ( deco[ 0 ] == ' ' )
				{
					semi_indicator = FALSE;
				} else
				if ( deco[ 0 ] == 'B' )
				{
					//
				} else
				if ( deco[ 0 ] == 'u' )
				{
					semi_indicator = FALSE;
				} else
				if ( deco[ 0 ] == 'U' )
				{
					//
				}
			}

			[self NonnonTxtboxDrawLineNumber:i
				rect:NSMakeRect(
					txtbox->offset_x,
					txtbox->offset_y + pxl_y,
					txtbox->linenumber_size.width,
					txtbox->font_size.height
				)
				semi_indicator:semi_indicator
			];

			i++;

			pxl_y += txtbox->font_size.height;
			if ( pxl_y > max_sy ) { break; }

		}

		n_bmp_flip_onoff = FALSE;
	}


	// [!] : Delete Circle

	if ( delete_circle_onoff )
	{
		NSColor *color_bg;

		if ( txtbox->delete_circle_fade_pressed.stop == FALSE )
		{
			NSColor *f = [NSColor  grayColor];
			NSColor *t = [NSColor whiteColor];

			color_bg = n_mac_nscolor_blend( f, t, txtbox->delete_circle_fade_pressed.percent * 0.01 );
		} else
		if ( txtbox->delete_circle_is_pressed )
		{
			color_bg = [NSColor blackColor];

			txtbox->delete_circle_fade_hovered.stop =  TRUE;
			txtbox->delete_circle_is_hovered        = FALSE;
		} else
		if ( txtbox->delete_circle_fade_hovered.stop == FALSE )
		{
//NSLog( @"%x : %d", delete_circle_fade.color_fg, delete_circle_fade.percent );

			NSColor *f;
			if ( n_txtbox_first_responder == self )
			{
				f = txtbox->nscolor_accent;
			} else {
				f = [NSColor grayColor];
			}

			NSColor *t = n_mac_nscolor_argb( 255, 255, 0, 128 );

			if ( txtbox->delete_circle_fade_hovered.color_to == n_bmp_white )
			{
				color_bg = n_mac_nscolor_blend( f, t, txtbox->delete_circle_fade_hovered.percent * 0.01 );
			} else {
				color_bg = n_mac_nscolor_blend( t, f, txtbox->delete_circle_fade_hovered.percent * 0.01 );
			}
		} else
		if ( txtbox->delete_circle_is_hovered )
		{
			color_bg = n_mac_nscolor_argb( 255, 255, 0, 128 );
		} else
		if ( n_txtbox_first_responder == self )
		{
			color_bg = txtbox->nscolor_accent;
		} else {
			color_bg = [NSColor grayColor];
		}

		CGFloat sz = csy - ( txtbox->offset_y * 2 );
		txtbox->delete_circle_rect = NSMakeRect( csx - sz - txtbox->offset_x, txtbox->offset_y, sz, sz );
		n_mac_draw_circle( color_bg, txtbox->delete_circle_rect );


		n_gdi gdi; n_gdi_zero( &gdi );

		gdi.sx                  = 15;
		gdi.sy                  = 15;

		gdi.base_color_bg       = n_bmp_white_invisible;
		gdi.base_color_fg       = n_bmp_white_invisible;
		gdi.text                = n_posix_literal( "×" );
		gdi.text_color_main     = n_bmp_white;
		gdi.text_size           = csy;

		n_bmp bmp1; n_bmp_zero( &bmp1 );
		n_gdi_bmp( &gdi, &bmp1 );

		n_bmp bmp2; n_bmp_zero( &bmp2 ); n_bmp_new_fast( &bmp2, gdi.sx, gdi.sy );
		n_bmp_flush( &bmp2, gdi.base_color_bg );
		n_bmp_transcopy( &bmp1, &bmp2, 0,0,gdi.sx,gdi.sy, 0,-1 );

//n_bmp_save( &bmp2, "/Users/nonnon/Desktop/ret.bmp" );

		NSRect rect_local = n_mac_rect_resize( txtbox->delete_circle_rect, -2 );
//NSLog( @"%f %f", rect.origin.y, rect.size.height );

		n_mac_image_nbmp_direct_draw( &bmp2, &rect_local, NO );

		n_bmp_free_fast( &bmp1 );
		n_bmp_free_fast( &bmp2 );
	}


	// [!] : Margin : before scrollbar

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		//
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
	{
		//
	} else {
		// [!] : right-side margin

		NSRect rect_padding = NSMakeRect( csx - txtbox->offset_x, 0, txtbox->offset_x, csy );
		n_mac_draw_box( txtbox->nscolor_back, rect_padding );


		// [!] : bottom-side margin

		{
			CGFloat o = txtbox->offset_y;
			if ( txtbox->border_separator_only ) { o -= 2; }

			rect_padding = NSMakeRect( 0, csy - o, csx, o );
			n_mac_draw_box( txtbox->nscolor_back, rect_padding );
		}
	}


	// [!] : Scrollbar

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		//
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
	{
		//
	} else {

		// [!] : Fake Scroller : also see NonnonTxtboxScrollMetrics

//NSLog( @"%f %lld", txtbox->scr.unit_page, txtbox->txt_data->sy );
		if ( txtbox->scrollbar_used )
		{

			// [!] : for hit test

			const CGFloat pos   = txtbox->scr.pixel_pos;

			const CGFloat scrsx = 12;

			const CGFloat off_x = txtbox->offset_x;
			const CGFloat off_y = txtbox->offset_y;

			const CGFloat scr_x = csx - off_x - scrsx;
			const CGFloat scr_y = off_y + pos;

			txtbox->scrollbar_shaft_rect = NSMakeRect( scr_x, off_y, scrsx, txtbox->scr.pixel_shaft );
			txtbox->scrollbar_thumb_rect = NSMakeRect( scr_x, scr_y, scrsx, txtbox->scr.pixel_thumb );


			const CGFloat thickness = 1;
			const CGFloat indicator = txtbox->offset_y + ( txtbox->focus / ( (CGFloat) txtbox->txt_data->sy - 1 ) * txtbox->scr.pixel_shaft );


			u32 color = n_mac_nscolor2argb( txtbox->nscolor_text );

			n_bmp_flip_onoff = TRUE;

			u32 color_shaft = n_bmp_alpha_replace_pixel( color, 16 );
//color_shaft = n_bmp_rgb(255,0,128);

			{
				NSColor *clr = n_mac_argb2nscolor( color_shaft );
				n_mac_draw_roundrect( clr, txtbox->scrollbar_shaft_rect, scrsx / 2 );
			}

			if ( txtbox->search_marker != NULL )
			{
				if ( txtbox->search_marker_count != txtbox->txt_data->sy )
				{
					[self NonnonTxtboxSearchMarkerReset:NO];
				} else {
					n_type_int i = 0;
					n_posix_loop
					{
						if ( txtbox->search_marker[ i ] )
						{
							NSRect rect_local = NSMakeRect( scr_x, i * txtbox->scr.pixel_step, scrsx, 1 );
							n_mac_draw_box( N_TXTBOX_INDICATOR_SEARCH, rect_local );
						}

						i++;
						if ( i >= txtbox->txt_data->sy ) { break; }
					}
				}
			}


			int hot    =  64;
			int normal =  96;
			int press  = 128;

			u32 color_thumb;
			if ( txtbox->scrollbar_fade_captured_onoff )
			{
//NSLog( @"scrollbar_fade_captured_onoff" );

				u32 f = n_bmp_alpha_replace_pixel( color, press  );
				u32 t = n_bmp_alpha_replace_pixel( color, hot    );
				if ( txtbox->scrollbar_thumb_is_captured )
				{
//NSLog( @"thumb_is_captured" );
					color_thumb = n_bmp_blend_pixel( t, f, (CGFloat) txtbox->scrollbar_fade.percent * 0.01 );
				} else {
//NSLog( @"else" );
					if ( txtbox->scrollbar_thumb_is_hovered )
					{
						t = n_bmp_alpha_replace_pixel( color, hot    );
					} else {
						t = n_bmp_alpha_replace_pixel( color, normal );
					}
					color_thumb = n_bmp_blend_pixel( f, t, (CGFloat) txtbox->scrollbar_fade.percent * 0.01 );
				}
//NSLog( @"%d", scrollbar_fade.percent );
			} else
			if ( txtbox->scrollbar_fade_hovered_onoff )
			{
//NSLog( @"scrollbar_fade_hovered_onoff" );

				u32 f = n_bmp_alpha_replace_pixel( color, hot    );
				u32 t = n_bmp_alpha_replace_pixel( color, normal );
				if ( txtbox->scrollbar_thumb_is_captured )
				{
					color_thumb = n_bmp_alpha_replace_pixel( color, press  );
				} else
				if ( txtbox->scrollbar_thumb_is_hovered )
				{
					color_thumb = n_bmp_blend_pixel( t, f, (CGFloat) txtbox->scrollbar_fade.percent * 0.01 );
				} else {
					color_thumb = n_bmp_blend_pixel( f, t, (CGFloat) txtbox->scrollbar_fade.percent * 0.01 );
				}
			} else {
//NSLog( @"else" );

				if ( txtbox->scrollbar_thumb_is_captured )
				{
					color_thumb = n_bmp_alpha_replace_pixel( color, press  );
				} else
				if ( txtbox->scrollbar_thumb_is_hovered )
				{
					color_thumb = n_bmp_alpha_replace_pixel( color, hot    );
				} else {
					color_thumb = n_bmp_alpha_replace_pixel( color, normal );
				}
			}

			{
				NSColor *clr = n_mac_argb2nscolor( color_thumb );
				n_mac_draw_roundrect( clr, txtbox->scrollbar_thumb_rect, scrsx / 2 );
			}


			// [!] : caret indicator on a shaft

			if ( txtbox->focus >= 0 )
			{
				NSRect rect_local = NSMakeRect( scr_x, indicator, scrsx, thickness );
				n_mac_draw_box( N_TXTBOX_INDICATOR_CARET, rect_local );
			}


			n_bmp_flip_onoff = FALSE;
		}

	}
	

	// [!] : Frame

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
	{
		if ( self.txtbox->border_separator_only )
		{
			NSRect rect_separator = NSMakeRect( 0, 0, csx, 2 );
			n_mac_draw_box( txtbox->nscolor_frame, rect_separator );
		} else {
			[self.layer setBorderWidth:2];

			CGFloat r = 10;

			[self.layer setBorderColor    :txtbox->nscolor_frame.CGColor];
			[self.layer setBackgroundColor:txtbox->nscolor_back .CGColor];

			[self.layer setCornerRadius:r];
		}
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
	{
		[self.layer setBorderWidth:2];

		CGFloat r = csy / 4;

		[self.layer setBorderColor    :txtbox->nscolor_frame.CGColor];
		[self.layer setBackgroundColor:txtbox->nscolor_back .CGColor];

		[self.layer setCornerRadius:r];
	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		[self.layer setBorderWidth:2];

		CGFloat r = csy / 2;

		NSColor *nscolor_bg = txtbox->nscolor_frame;
		NSColor *nscolor_fg = [NSColor controlAccentColor];
		NSColor *nscolor    = n_mac_nscolor_blend( nscolor_bg, nscolor_fg, findbox_border_blend );

		if ( n_txtbox_first_responder == self )
		{
			[self.layer setBorderColor    :nscolor               .CGColor];
			[self.layer setBackgroundColor:txtbox->nscolor_back  .CGColor];
		} else {
			[self.layer setBorderColor    :nscolor               .CGColor];
			[self.layer setBackgroundColor:txtbox->nscolor_stripe.CGColor];
		}
		[self.layer setCornerRadius:r];
	} else {
		[self.layer setBorderWidth:2];

		CGFloat r = 10;

		[self.layer setBorderColor    :txtbox->nscolor_frame.CGColor];
		[self.layer setBackgroundColor:txtbox->nscolor_back .CGColor];

		[self.layer setCornerRadius:r];
	}


	// [!] : Clean-up
//if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX ) { NSLog( @"%lld %lld", redraw_fy, redraw_ty ); }

	txtbox->redraw_fy = -1;
	txtbox->redraw_ty = -1;

/*
	{
		NSColor *clr = n_mac_nscolor_argb( 255, 0,200,255 );
		NSRect   rct = NSMakeRect( 100,100,200,200 );
		n_mac_draw_roundrect( clr, rct, 10 );
	}
*/
/*
	{
		NSColor *clr = n_mac_nscolor_argb( 255, 0,200,255 );
		NSRect   rct = NSMakeRect( 100,100,200,200 );
		n_mac_draw_circle( clr, rct );
	}
*/

}


@end

