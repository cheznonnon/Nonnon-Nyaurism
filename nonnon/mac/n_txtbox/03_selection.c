// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




BOOL
n_mac_txtbox_is_selected
(
	NSPoint cur,
	CGFloat fr_pxl_x,
	CGFloat fr_cch_y,
	CGFloat to_pxl_x,
	CGFloat to_cch_y
)
{
//NSLog( @"%0.2f %0.2f %0.2f", cur.x, fr_pxl_x, to_pxl_x );

	BOOL ret = FALSE;

	if ( fr_cch_y == to_cch_y )
	{
		if ( fr_pxl_x == to_pxl_x )
		{
			//
		} else
		if ( fr_pxl_x < to_pxl_x )
		{
			if ( cur.y != fr_cch_y )
			{
				ret = FALSE;
			} else
			if ( ( cur.x >= fr_pxl_x )&&( cur.x < to_pxl_x ) )
			{
				ret = TRUE;
			}
		} else {
			if ( cur.y != fr_cch_y )
			{
				ret = FALSE;
			} else
			if ( ( cur.x >= to_pxl_x )&&( cur.x < fr_pxl_x ) )
			{
				ret = TRUE;
			}
		}
	} else
	if ( fr_cch_y < to_cch_y )
	{
		if ( ( 2 <= fabs( fr_cch_y - to_cch_y ) ) )
		{
			if ( ( cur.y > fr_cch_y )&&( cur.y < to_cch_y ) )
			{
				ret = TRUE;
			}
		}

		if ( ( cur.y == fr_cch_y )&&( cur.x >= fr_pxl_x ) )
		{
			ret = TRUE;
		} else
		if ( ( cur.y == to_cch_y )&&( cur.x <  to_pxl_x ) )
		{
			ret = TRUE;
		}
	} else
	if ( fr_cch_y > to_cch_y )
	{
		if ( ( 2 <= fabs( fr_cch_y - to_cch_y ) ) )
		{
			if ( ( cur.y > to_cch_y )&&( cur.y < fr_cch_y ) )
			{
				ret = TRUE;
			}
		}

		if ( ( cur.y == fr_cch_y )&&( cur.x <  fr_pxl_x ) )
		{
			ret = TRUE;
		} else
		if ( ( cur.y == to_cch_y )&&( cur.x >= to_pxl_x ) )
		{
			ret = TRUE;
		}
	}


	return ret;
}




static n_posix_char *n_mac_txtbox_selection_table_ascii[] = {

	":", ";",
	",", ".",
	"(", ")",
	"[", "]",
	"<", ">",
	"\'", "\"",
	"=",
	"/",
	"|",
	NULL

};

static n_posix_char *n_mac_txtbox_selection_table_cjk[] = {

	"　",
	"：",
	"；",
	"、",
	"。",
	"（", "）",
	"「", "」",
	"＜", "＞",
	"’", "”",
	"＝",
	"・",
	"｜",
	NULL

};

// internal
BOOL
n_mac_txtbox_selection_is_stop_char_single_utf8_search( n_posix_char *f, n_posix_char *t )
{

	if ( ( f[ 0 ] == N_STRING_CHAR_NUL )&&( t[ 0 ] == N_STRING_CHAR_NUL ) ) { return TRUE; }

	if ( f[ 0 ] == N_STRING_CHAR_NUL ) { return FALSE; }
	if ( t[ 0 ] == N_STRING_CHAR_NUL ) { return FALSE; }


	if ( f[ 0 ] != t[ 0 ] ) { return FALSE; }

	if ( ( f[ 1 ] == N_STRING_CHAR_NUL )&&( t[ 1 ] == N_STRING_CHAR_NUL ) ) { return TRUE; }

	if ( f[ 1 ] == N_STRING_CHAR_NUL ) { return FALSE; }
	if ( t[ 1 ] == N_STRING_CHAR_NUL ) { return  TRUE; }


	if ( f[ 1 ] != t[ 1 ] ) { return FALSE; }

	if ( ( f[ 2 ] == N_STRING_CHAR_NUL )&&( t[ 2 ] == N_STRING_CHAR_NUL ) ) { return TRUE; }

	if ( f[ 2 ] == N_STRING_CHAR_NUL ) { return FALSE; }
	if ( t[ 2 ] == N_STRING_CHAR_NUL ) { return  TRUE; }


	if ( f[ 2 ] != t[ 2 ] ) { return FALSE; }

	if ( ( f[ 3 ] == N_STRING_CHAR_NUL )&&( t[ 3 ] == N_STRING_CHAR_NUL ) ) { return TRUE; }

	if ( f[ 3 ] == N_STRING_CHAR_NUL ) { return FALSE; }
	if ( t[ 3 ] == N_STRING_CHAR_NUL ) { return  TRUE; }


	return FALSE;
}

BOOL
n_mac_txtbox_selection_is_stop_char_single_utf8( n_posix_char *starting_char, n_posix_char *current_char, n_posix_char *compare_char )
{
	if ( n_mac_txtbox_selection_is_stop_char_single_utf8_search( starting_char, compare_char ) )
	{
		return ( FALSE == n_mac_txtbox_selection_is_stop_char_single_utf8_search( current_char, compare_char ) );
	} else {
		return ( FALSE != n_mac_txtbox_selection_is_stop_char_single_utf8_search( current_char, compare_char ) );
	}
}

BOOL
n_mac_txtbox_selection_is_stop_char_utf8_ascii( n_posix_char *starting_char, n_posix_char *current_char )
{

	BOOL ret = FALSE;

	int i = 0;
	n_posix_loop
	{
		n_posix_char *str = n_mac_txtbox_selection_table_ascii[ i ];
		if ( str == NULL ) { break; }

		if ( n_mac_txtbox_selection_is_stop_char_single_utf8( starting_char, current_char, str ) )
		{
			ret = TRUE;
			break;
		}

		i++;
	}


	return ret;
}

BOOL
n_mac_txtbox_selection_is_stop_char_utf8_cjk( n_posix_char *starting_char, n_posix_char *current_char )
{

	BOOL ret = FALSE;

	int i = 0;
	n_posix_loop
	{
		n_posix_char *str = n_mac_txtbox_selection_table_cjk[ i ];
		if ( str == NULL ) { break; }

		if ( n_mac_txtbox_selection_is_stop_char_single_utf8( starting_char, current_char, str ) )
		{
			ret = TRUE;
			break;
		}

		i++;
	}


	return ret;
}

BOOL
n_mac_txtbox_selection_is_stop_char_utf8( n_posix_char *starting_char, n_posix_char *current_char )
{
	if (
		( n_mac_txtbox_selection_is_stop_char_utf8_ascii( starting_char, current_char ) )
		||
		( n_mac_txtbox_selection_is_stop_char_utf8_cjk  ( starting_char, current_char ) )
	)
	{
		return TRUE;
	}

	return FALSE;
}

