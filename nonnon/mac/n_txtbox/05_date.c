// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




void
n_txtbox_date( n_posix_char *str, n_type_int cch )
{

	// [Mechanism]
	//
	//	this like format is used in some locales
	//	big-to-small order is simply sortable


	int year, month, day, hour, minute, second;


	n_time_today( &year, &month, &day, &hour, &minute, &second );


	n_posix_snprintf_literal
	(
		str,
		cch + 1,
		"%04d/%02d/%02d %02d:%02d:%02d",
		year, month, day,
		hour, minute, second
	);


	return;
}

void
n_txtbox_day_of_week( n_posix_char *str, n_type_int cch )
{

	NSString *nsstr = [[NSLocale preferredLanguages] objectAtIndex:0];
//NSLog( @"%@", nsstr );


	// [!] : "ja-JP" to "ja_JP"

	n_posix_char *s = n_mac_nsstring2str( nsstr );

	if ( 3 <= n_posix_strlen( s ) )
	{
		s[ 2 ] = '_';
	} else {
		s[ 0 ] = '\0';
	}

	setlocale( LC_ALL, s );

	n_string_free( s );


	time_t     rawtime; time( &rawtime );
	struct tm *timeinfo = localtime( &rawtime );

	n_posix_strftime_literal( str, cch, "%A", timeinfo );

//n_posix_debug_literal( "%s", str );


	return;

}

