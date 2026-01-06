// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@implementation NonnonTxtbox (NonnonTxtboxSearch)


- (void) NonnonTxtboxSearch:(n_posix_char*) query is_reverse:(BOOL)is_reverse
{

	// [!] : unselect

	if ( is_reverse )
	{
		caret_to = caret_fr;
	} else {
		caret_fr = caret_to;
	}


	if ( ( query[ 0 ] == n_posix_literal( '#' ) )&&( n_string_is_digit( query, 1 ) ) )
	{

		n_type_int y = n_posix_atoi( &query[ 1 ] ) - 1;

		n_focus = y;

		caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
		(
			n_txt_data,
			y,
			font,
			font_size,
			0
		);

		[self NonnonTxtboxCaretOutOfCanvasUpDownSearch];

		[self NonnonTxtboxRedraw];

	} else {
//NSLog( @"%ld", labs( caret_fr.cch.y - caret_to.cch.y ) );

		if ( 0 != labs( caret_fr.cch.y - caret_to.cch.y ) ) { return; }

		n_type_int caret_f = MIN( caret_fr.cch.x, caret_to.cch.x );
		n_type_int caret_t = MAX( caret_fr.cch.x, caret_to.cch.x );

		n_type_int y = caret_fr.cch.y;

		if ( is_reverse )
		{
			n_posix_char *line = n_txt_get( n_txt_data, y );
			n_type_int    lcch = n_posix_strlen( line );

			n_posix_loop
			{//break;
				caret_f--;
				caret_t = caret_f;
//break;
				if ( caret_f < 0 )
				{
					BOOL break_onoff = FALSE;

					y--;
					if ( y < 0 )
					{
						y = n_txt_data->sy - 1;
						break_onoff = TRUE;
					}

					line = n_txt_get( n_txt_data, y );
					lcch = n_posix_strlen( line );

					caret_f = caret_t = lcch;

					if ( break_onoff ) { break; }
				}

				if ( n_string_match( &line[ caret_f ], query ) )
				{
					caret_t = caret_f + n_posix_strlen( query );
					break;
				}
			}
		} else {
			n_posix_loop
			{//break;

				n_string_search( n_txt_get( n_txt_data, y ), query, &caret_f, &caret_t );
				if ( caret_f != caret_t ) { break; }

				caret_f = caret_t = 0;

				y++;
				if ( y >= n_txt_data->sy ) { y = 0; break; }
			}
		}

		n_focus = y;

		caret_fr = n_txtbox_caret_detect_cch2pixel
		(
			n_txt_data,
			n_focus,
			font,
			font_size,
			caret_f
		);

		caret_to = n_txtbox_caret_detect_cch2pixel
		(
			n_txt_data,
			n_focus,
			font,
			font_size,
			caret_t
		);

		[self NonnonTxtboxCaretOutOfCanvasUpDownSearch];

		shift_selection_is_tail = TRUE;

		[self NonnonTxtboxRedraw];

	}


	return;
}

- (void) NonnonTxtboxSearchMarker:(n_posix_char*) query
{
//NSLog( @"NonnonTxtboxSearchMarker" );

	// [!] : n_txtbox.c has NonnonTxtboxSearchMarkerReset : to resolve dependency problem

	[self NonnonTxtboxSearchMarkerReset:NO];

	n_search_marker_count = n_txt_data->sy;
	n_search_marker       = n_memory_new( sizeof( BOOL ) * n_search_marker_count );


	n_type_int f = 0;
	n_type_int t = 0;

	n_type_int y = 0;
	n_posix_loop
	{//break;

		n_string_search( n_txt_get( n_txt_data, y ), query, &f, &t );
		if ( f != t )
		{
			n_search_marker[ y ] = TRUE;
		} else {
			n_search_marker[ y ] = FALSE;
		}

		f = t = 0;

		y++;
		if ( y >= n_txt_data->sy ) { break; }
	}

	[self NonnonTxtboxRedraw];


	return;
}


@end

