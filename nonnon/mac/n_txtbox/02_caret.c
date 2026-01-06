// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




typedef struct {

	n_type_int x;
	n_type_int y;

} n_point_cch;

typedef struct {

	CGPoint     pxl;
	n_point_cch cch;

} n_caret;

n_caret
NonnonMakeCaret( CGFloat pxl_x, CGFloat pxl_y, n_type_int cch_x, n_type_int cch_y )
{

	n_caret caret = { pxl_x, pxl_y, cch_x, cch_y };

	return caret;
}

void
n_caret_debug_cch( n_caret fr, n_caret to )
{
NSLog( @"CCH : Fr %lld %lld : To %lld %lld", fr.cch.x, fr.cch.y, to.cch.x, to.cch.y );

	return;
}

n_caret
n_txtbox_caret_detect_pixel2caret
(
	n_txt      *txt,
	n_type_int  focus,
	NSRect      rect,
	NSFont     *font,
	CGSize      font_size,
	NSPoint     local_point
)
{

	n_posix_char *line = n_txt_get( txt, focus );


	n_type_int index = 0;
	CGFloat    sx    = 0;
	n_type_int tab   = 0;
	n_type_int cch   = 0;
	n_posix_loop
	{//break;
		if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

		CGSize     char_size;
		n_type_int char_index;
		NSRect     char_rect = rect;

		n_posix_char *character;
		character = n_mac_txtbox_character( font, font_size, (u8*) line, index, &char_size, &char_index, &tab );

		CGFloat half_sx = char_size.width / 2;

		sx += half_sx;
		char_rect.origin.x = rect.origin.x + sx;
		if ( char_rect.origin.x > local_point.x ) { sx -= half_sx; break; }

		index += char_index;
		cch    = index;

		sx += half_sx;
		char_rect.origin.x = rect.origin.x + sx;
		if ( char_rect.origin.x > local_point.x ) { break; }

	}


	return NonnonMakeCaret( sx, focus * font_size.height, cch, focus );
}

n_caret
n_txtbox_caret_detect_cch2pixel
(
	n_txt      *txt,
	n_type_int  focus,
	NSFont     *font,
	CGSize      font_size,
	n_type_int  caret_cch
)
{
//NSLog( @"%lld", caret_cch );

	n_posix_char *line = n_txt_get( txt, focus );


	n_type_int index = 0;
	CGFloat    sx    = 0;
	n_type_int tab   = 0;
	n_posix_loop
	{//break;
		if ( line[ index ] == N_STRING_CHAR_NUL ) { break; }

		CGSize     char_size;
		n_type_int char_index;

		n_mac_txtbox_character( font, font_size, (u8*) line, index, &char_size, &char_index, &tab );

		if ( index >= caret_cch ) { break; }

		sx    += char_size.width;
		index += char_index;

	}

//NSLog( @"%f", sx );
	return NonnonMakeCaret( sx, focus * font_size.height, caret_cch, focus );
}

#define N_MAC_TXTBOX_CARET_MOVE_L ( 1 )
#define N_MAC_TXTBOX_CARET_MOVE_R ( 2 )

n_type_int
n_mac_txtbox_caret_move( n_posix_char *str_arg, n_type_int cch, BOOL accent_onoff, int movement )
{

	if ( cch < 0 ) { return cch; }

	u8 *str = (u8*) str_arg;

	if ( movement == N_MAC_TXTBOX_CARET_MOVE_L )
	{

		if ( cch == 0 ) { return cch; }

		n_posix_loop
		{

			cch--;
			if ( cch <= 0 ) { break; }

			if ( str[ cch ] >= 0xf0 )
			{
				break;
			} else
			if ( str[ cch ] >= 0xe0 )
			{
				break;
			} else
			if ( str[ cch ] >= 0xc0 )
			{
				break;
			} else
			if ( str[ cch ] <= 127 )
			{
				break;
			}

		}

		if (
			( accent_onoff )
			&&
			( n_mac_txtbox_is_accentmark( (void*) &str[ cch ], 4 ) )
		)
		{
			n_posix_loop
			{

				cch--;
				if ( cch <= 0 ) { break; }

				if ( str[ cch ] >= 0xf0 )
				{
					break;
				} else
				if ( str[ cch ] >= 0xe0 )
				{
					break;
				} else
				if ( str[ cch ] >= 0xc0 )
				{
					break;
				} else
				if ( str[ cch ] <= 127 )
				{
					break;
				}

			}
		}

	} else
	if ( movement == N_MAC_TXTBOX_CARET_MOVE_R )
	{

		n_posix_loop
		{

			if ( str[ cch ] == 0x00 ) { break; }
			cch++;

			if ( str[ cch ] >= 0xf0 )
			{
				break;
			} else
			if ( str[ cch ] >= 0xe0 )
			{
				break;
			} else
			if ( str[ cch ] >= 0xc0 )
			{
				break;
			} else
			if ( str[ cch ] <= 127 )
			{
				break;
			}

		}

		if (
			( accent_onoff )
			&&
			( n_mac_txtbox_is_accentmark( (void*) &str[ cch ], 4 ) )
		)
		{
			n_posix_loop
			{

				if ( str[ cch ] == 0x00 ) { break; }
				cch++;

				if ( str[ cch ] >= 0xf0 )
				{
					break;
				} else
				if ( str[ cch ] >= 0xe0 )
				{
					break;
				} else
				if ( str[ cch ] >= 0xc0 )
				{
					break;
				} else
				if ( str[ cch ] <= 127 )
				{
					break;
				}

			}
		}

	}


	return cch;
}

n_caret
n_mac_txtbox_caret_tail
(
	n_txt      *txt,
	n_type_int  focus,
	NSFont     *font,
	CGSize      font_size,
	BOOL       *is_tail
)
{
//NSLog( @"%f", n_focus );


	n_posix_char *line = n_txt_get( txt, focus );
	n_type_int    cch  = n_posix_strlen( line );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		txt,
		focus,
		font,
		font_size,
		cch
	);

	if ( is_tail != NULL ) { (*is_tail) = TRUE; }


	return caret;
}
