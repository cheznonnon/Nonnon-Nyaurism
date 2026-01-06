// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




NSString*
n_mac_txtbox_copy( n_txt *txt, n_type_int fr_x, n_type_int fr_y, n_type_int to_x, n_type_int to_y )
{

	NSString *nsstr;

	if ( fr_y == to_y )
	{
//NSLog( @"same" );
		n_type_int min_x = MIN( fr_x, to_x );
		n_type_int max_x = MAX( fr_x, to_x );
//NSLog( @"%lld %lld", min_x, max_x );

		n_posix_char *line = n_txt_get( txt, fr_y );
//NSLog( @"Line : NSString : %@", n_mac_str2nsstring( line ) );


		n_type_int    cch = n_posix_strlen( line );
		n_posix_char *str = n_string_new( cch );

		n_posix_sprintf_literal( str, "%s", line );
		str[ max_x ] = N_STRING_CHAR_NUL;
//NSLog( @"Line Max : NSString : %@", n_mac_str2nsstring( str ) );

		n_posix_sprintf_literal( str, "%s", &str[ min_x ] );
//NSLog( @"Line Min : NSString : %@", n_mac_str2nsstring( str ) );


		nsstr = n_mac_str2nsstring( str );


		n_string_free( str );

	} else {

		n_type_int x1;
		n_type_int x2;
		n_type_int y1;
		n_type_int y2;

		if ( fr_y < to_y )
		{
			x1 = fr_x;
			y1 = fr_y;
			x2 = to_x;
			y2 = to_y;
		} else {
			x1 = to_x;
			y1 = to_y;
			x2 = fr_x;
			y2 = fr_y;
		}

		n_posix_char *line = n_txt_get( txt, y1 );

		n_type_int i;


		// [!] : detect cch

		n_type_int cch = n_posix_strlen( &line[ x1 ] );

		i = 0;
		n_posix_loop
		{
			cch += 1;

			i++;
			if ( ( y1 + i ) >= y2 ) { break; }

			cch += n_posix_strlen( n_txt_get( txt, y1 + i ) );
		}

		{
			n_posix_char *last = n_txt_get( txt, y1 + i );
			if ( last[ 0 ] == N_STRING_CHAR_NUL )
			{
				cch += 1;
			} else {
				cch += n_posix_strlen( n_txt_get( txt, y1 + i ) );
				//cch++;
			}
		}


		// [!] : copy

		n_posix_char *str = n_string_new( cch );

		n_type_int index = n_posix_sprintf_literal( str, "%s", &line[ x1 ] );

		i = 0;
		n_posix_loop
		{
			index += n_posix_sprintf_literal( &str[ index ], "%c", N_STRING_CHAR_LF );

			i++;
			if ( ( y1 + i ) >= y2 ) { break; }

			index += n_posix_sprintf_literal( &str[ index ], "%s", n_txt_get( txt, y1 + i ) );
		}

		{
			n_posix_char *last = n_txt_get( txt, y1 + i );
			if ( last[ 0 ] == N_STRING_CHAR_NUL )
			{
				//n_posix_sprintf_literal( &str[ index ], "%c", N_STRING_CHAR_LF );
			} else {
				n_posix_sprintf_literal( &str[ index ], "%s", n_txt_get( txt, y1 + i ) );
				str[ index + x2 ] = N_STRING_CHAR_NUL;
				//n_posix_strcat( str, N_STRING_LF );
			}
		}
		

		nsstr = n_mac_str2nsstring( str );


		n_string_free( str );

	}


	return nsstr;
}

// internal
BOOL
n_mac_txtbox_del_internal( n_txt *txt, n_type_int fr_x, n_type_int fr_y, n_type_int to_x, n_type_int to_y )
{

	// [!] : this module has historical reason

	BOOL ret = FALSE;

	if ( ( fr_x == to_x )&&( fr_y == to_y ) )
	{
		//
	} else
	if ( fr_y == to_y )
	{

		n_posix_char *line = n_txt_get( txt, fr_y );

		if ( fr_x < to_x )
		{
			n_posix_sprintf_literal( &line[ fr_x ], "%s", &line[ to_x ] );
		} else {
			n_posix_sprintf_literal( &line[ to_x ], "%s", &line[ fr_x ] );
		}

		ret = TRUE;

	} else
	if ( fr_y < to_y )
	{

		n_type_int to_cch = n_posix_strlen( n_txt_get( txt, txt->sy - 1 ) );
		if ( ( fr_y == 0 )&&( to_y == ( txt->sy - 1 ) )&&( fr_x == 0 )&&( to_x == to_cch ) )
		{
			ret = TRUE;
			n_txt_utf8_new( txt );

			return ret;
		}


		n_posix_char *line_f = n_txt_get( txt, fr_y );
		n_posix_char *line_t = n_txt_get( txt, to_y );

		line_f[ fr_x ] = N_STRING_CHAR_NUL;
		n_posix_sprintf_literal( line_t, "%s", &line_t[ to_x ] );
//NSLog( @"F : %s", line_f );
//NSLog( @"T : %s", line_t );

		n_posix_char *str = n_string_new( txt->byte );
		n_posix_sprintf_literal( str, "%s%s", line_f, line_t );

		n_type_int i = 0;
		n_posix_loop
		{//break;

			n_txt_del( txt, fr_y );

			i++;
			if ( i > ( to_y - fr_y ) ) { break; }
		}

		n_txt_add( txt, fr_y, str );
		n_string_free( str );

		ret = TRUE;

	} else
	if ( fr_y > to_y )
	{

		n_posix_char *line_f = n_txt_get( txt, to_y );
		n_posix_char *line_t = n_txt_get( txt, fr_y );

		line_f[ to_x ] = N_STRING_CHAR_NUL;
		n_posix_sprintf_literal( line_t, "%s", &line_t[ fr_x ] );
//NSLog( @"F : %s", line_f );
//NSLog( @"T : %s", line_t );

		n_posix_char *str = n_string_new( txt->byte );
		n_posix_sprintf_literal( str, "%s%s", line_f, line_t );

		n_type_int i = 0;
		n_posix_loop
		{//break;

			n_txt_del( txt, to_y );

			i++;
			if ( i > ( fr_y - to_y ) ) { break; }
		}

		n_txt_add( txt, to_y, str );
		n_string_free( str );

		ret = TRUE;

	}


	return ret;
}

BOOL
n_mac_txtbox_del( n_txt *txt, n_type_int *focus, n_caret *fr, n_caret *to )
{

	BOOL ret = n_mac_txtbox_del_internal( txt, fr->cch.x, fr->cch.y, to->cch.x, to->cch.y );

	if ( fr->cch.y == to->cch.y )
	{
		if ( fr->cch.x == to->cch.x )
		{
			//
		} else
		if ( fr->cch.x < to->cch.x )
		{
			(*to) = (*fr);
		} else {
			(*fr) = (*to);
		}
	} else
	if ( fr->cch.y <= to->cch.y )
	{
		(*focus) = fr->cch.y;
		(*to) = (*fr);
	} else {
		(*focus) = to->cch.y;
		(*fr) = (*to);
	}


	return ret;
}

