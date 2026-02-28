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
		txtbox->caret_to = txtbox->caret_fr;
	} else {
		txtbox->caret_fr = txtbox->caret_to;
	}


	if ( ( query[ 0 ] == n_posix_literal( '#' ) )&&( n_string_is_digit( query, 1 ) ) )
	{

		n_type_int y = n_posix_atoi( &query[ 1 ] ) - 1;

		txtbox->focus = y;

		txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
		(
			txtbox->txt_data,
			y,
			txtbox->font,
			txtbox->font_size,
			0
		);

		[self NonnonTxtboxCaretOutOfCanvasUpDownSearch];

		[self NonnonTxtboxRedraw];

	} else {
//NSLog( @"%ld", labs( caret_fr.cch.y - caret_to.cch.y ) );

		if ( 0 != labs( txtbox->caret_fr.cch.y - txtbox->caret_to.cch.y ) ) { return; }

		n_type_int caret_f = MIN( txtbox->caret_fr.cch.x, txtbox->caret_to.cch.x );
		n_type_int caret_t = MAX( txtbox->caret_fr.cch.x, txtbox->caret_to.cch.x );

		n_type_int y = txtbox->caret_fr.cch.y;

		if ( is_reverse )
		{
			n_posix_char *line = n_txt_get( txtbox->txt_data, y );
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
						y = txtbox->txt_data->sy - 1;
						break_onoff = TRUE;
					}

					line = n_txt_get( txtbox->txt_data, y );
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

				n_string_search( n_txt_get( txtbox->txt_data, y ), query, &caret_f, &caret_t );
				if ( caret_f != caret_t ) { break; }

				caret_f = caret_t = 0;

				y++;
				if ( y >= txtbox->txt_data->sy ) { y = 0; break; }
			}
		}

		txtbox->focus = y;

		txtbox->caret_fr = n_txtbox_caret_detect_cch2pixel
		(
			txtbox->txt_data,
			txtbox->focus,
			txtbox->font,
			txtbox->font_size,
			caret_f
		);

		txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
		(
			txtbox->txt_data,
			txtbox->focus,
			txtbox->font,
			txtbox->font_size,
			caret_t
		);

		[self NonnonTxtboxCaretOutOfCanvasUpDownSearch];

		txtbox->shift_selection_is_tail = TRUE;

		[self NonnonTxtboxRedraw];

	}


	return;
}

- (void) NonnonTxtboxSearchMarker:(n_posix_char*) query
{
//NSLog( @"NonnonTxtboxSearchMarker" );

	// [!] : n_txtbox.c has NonnonTxtboxSearchMarkerReset : to resolve dependency problem

	[self NonnonTxtboxSearchMarkerReset:NO];

	txtbox->search_marker_count = txtbox->txt_data->sy;
	txtbox->search_marker       = n_memory_new( sizeof( BOOL ) * txtbox->search_marker_count );


	n_type_int f = 0;
	n_type_int t = 0;

	n_type_int y = 0;
	n_posix_loop
	{//break;

		n_string_search( n_txt_get( txtbox->txt_data, y ), query, &f, &t );
		if ( f != t )
		{
			txtbox->search_marker[ y ] = TRUE;
		} else {
			txtbox->search_marker[ y ] = FALSE;
		}

		f = t = 0;

		y++;
		if ( y >= txtbox->txt_data->sy ) { break; }
	}

	[self NonnonTxtboxRedraw];


	return;
}


@end

