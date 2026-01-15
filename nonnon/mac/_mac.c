// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC
#define _H_NONNON_MAC




#import <Cocoa/Cocoa.h>




#include "../neutral/posix.c"
#include "../neutral/string_path.c"
#include "../neutral/txt.c"

#include "../bridge/helper.c"




// [!] : event.keyCode

#define N_MAC_KEYCODE_ARROW_UP    126
#define N_MAC_KEYCODE_ARROW_DOWN  125
#define N_MAC_KEYCODE_ARROW_LEFT  123
#define N_MAC_KEYCODE_ARROW_RIGHT 124

#define N_MAC_KEYCODE_RETURN       36
#define N_MAC_KEYCODE_ENTER        76 // [!] : japanese calls this "Ten-key"

#define N_MAC_KEYCODE_TAB          48

#define N_MAC_KEYCODE_SPACE        49

#define N_MAC_KEYCODE_BACKSPACE    51

#define N_MAC_KEYCODE_CUT          7 // 'x'
#define N_MAC_KEYCODE_COPY         8 // 'c'
#define N_MAC_KEYCODE_PASTE        9 // 'v'
#define N_MAC_KEYCODE_SELECT_ALL   0 // 'a'
#define N_MAC_KEYCODE_SAVE         1 // 's'
#define N_MAC_KEYCODE_UNDO         6 // 'z'
#define N_MAC_KEYCODE_FIND         3 // 'f'

#define N_MAC_KEYCODE_MENU       110

#define N_MAC_KEYCODE_ESCAPE      53

#define N_MAC_KEYCODE_HOME       115
#define N_MAC_KEYCODE_END        119
#define N_MAC_KEYCODE_INSERT     114
#define N_MAC_KEYCODE_DELETE     117
#define N_MAC_KEYCODE_PRTSCN     105
#define N_MAC_KEYCODE_PAGE_UP    116
#define N_MAC_KEYCODE_PAGE_DOWN  121

#define N_MAC_KEYCODE_F1         122
#define N_MAC_KEYCODE_F2         120
#define N_MAC_KEYCODE_F3          99
#define N_MAC_KEYCODE_F4         118
#define N_MAC_KEYCODE_F5          96
#define N_MAC_KEYCODE_F6          97
#define N_MAC_KEYCODE_F7          98
#define N_MAC_KEYCODE_F8         100
#define N_MAC_KEYCODE_F9         101
#define N_MAC_KEYCODE_F10        109
//#define N_MAC_KEYCODE_F11      handled as special
#define N_MAC_KEYCODE_F12        111

#define N_MAC_KEYCODE_NUMBER_1    18
#define N_MAC_KEYCODE_NUMBER_2    19
#define N_MAC_KEYCODE_NUMBER_3    20
#define N_MAC_KEYCODE_NUMBER_4    21
#define N_MAC_KEYCODE_NUMBER_5    23
#define N_MAC_KEYCODE_NUMBER_6    22
#define N_MAC_KEYCODE_NUMBER_7    26
#define N_MAC_KEYCODE_NUMBER_8    28
#define N_MAC_KEYCODE_NUMBER_9    25
#define N_MAC_KEYCODE_NUMBER_0    29


BOOL
n_mac_keystate_get( uint16_t keycode )
{
	// [!] : this is useful as GetAsyncKeyState() in Win32

	return CGEventSourceKeyState( kCGEventSourceStateCombinedSessionState, keycode );
}




// [!] : shared

@protocol NonnonDragAndDrop_delegate

- (void) NonnonDragAndDrop_dropped : (NSString*) nsstr;

@end




void
n_mac_debug_count( void )
{

	static int i = 0;

	NSLog( @"%d", i );

	i++;


	return;
}




NSString*
n_mac_str2nsstring( n_posix_char *str )
{
	if ( str == NULL ) { return [[NSString alloc] init]; }

	return [NSString stringWithUTF8String:str];
}

char*
n_mac_nsstring2str( NSString *nsstring )
{
	char *ret = n_string_path_carboncopy( (void*) [nsstring UTF8String] );

	return ret;
}




void
n_mac_rect_expand_size( NSRect rect, n_type_gfx *x, n_type_gfx *y, n_type_gfx *sx, n_type_gfx *sy )
{

	if (  x != NULL ) { (* x) = rect.origin.x   ; }
	if (  y != NULL ) { (* y) = rect.origin.y   ; }
	if ( sx != NULL ) { (*sx) = rect.size.width ; }
	if ( sy != NULL ) { (*sy) = rect.size.height; }


	return;
}

NSRect
n_mac_rect_resize( NSRect rect, n_type_gfx size )
{

	CGFloat  x = rect.origin.x    - size;
	CGFloat  y = rect.origin.y    - size;
	CGFloat sx = rect.size.width  + ( size * 2 );
	CGFloat sy = rect.size.height + ( size * 2 );


	return NSMakeRect( x,y,sx,sy );
}




NSFont*
n_mac_stdfont( void )
{
	return [NSFont systemFontOfSize:14 weight:NSFontWeightRegular];
}



/*
// [x] : usage is unclear

#define n_type_timer CFTimeInterval

#define n_mac_tickcount CACurrentMediaTime

BOOL
n_mac_game_timer( n_type_timer *prv, n_type_timer interval )
{

	// [!] : interval : msec / 1000


	n_type_timer cur = n_mac_tickcount();
	n_type_timer usc = cur - (*prv);


	if ( usc >= interval )
	{
		(*prv) = cur;

		return TRUE;
	}


	return FALSE;
}
*/



NSTimer*
n_mac_timer_init( id self, SEL selector, NSTimeInterval msec )
{
//return nil;

	NSTimer *timer = [NSTimer
		 scheduledTimerWithTimeInterval:msec / 1000
		                         target:self
		                       selector:selector
		                       userInfo:nil
		                        repeats:YES
	];

	timer.tolerance = 1 / 1000;

	return timer;
}

void
n_mac_timer_init_once( id self, SEL selector, NSTimeInterval msec )
{
//return nil;

	NSTimer *timer = [NSTimer
		 scheduledTimerWithTimeInterval:msec / 1000
		                         target:self
		                       selector:selector
		                       userInfo:nil
		                        repeats:NO
	];

	timer.tolerance = 1 / 1000;

	return;
}

void
n_mac_timer_exit( NSTimer *timer )
{

	// [!] : don't call in a timer method : leak happens

	[timer invalidate];

	return;
}




void
n_mac_finder_call( NSString *path )
{
//NSLog( @"%@", path );

	n_posix_char *str = n_mac_nsstring2str( path );

	if ( n_posix_stat_is_file( str ) )
	{
		NSArray *array = [NSArray arrayWithObjects:path, nil];
		[[NSWorkspace sharedWorkspace] activateFileViewerSelectingURLs:array];
	} else {
		// [x] : deprecated
		//[[NSWorkspace sharedWorkspace] openFile:path withApplication:@"Finder"];

		NSURL *url = [NSURL fileURLWithPath:path];
		[[NSWorkspace sharedWorkspace] openURL:url];
	}

	n_string_path_free( str );


	return;
}




BOOL
n_mac_is_darkmode( void )
{
	NSString *interfaceStyle = [NSUserDefaults.standardUserDefaults valueForKey:@"AppleInterfaceStyle"];
	return [interfaceStyle isEqualToString:@"Dark"];
}

BOOL
n_mac_is_sandbox( void )
{

	NSDictionary* env = [[NSProcessInfo processInfo] environment];
	BOOL          ret = (nil != [env objectForKey:@"APP_SANDBOX_CONTAINER_ID"]);
//NSLog( @"Sandbox : %d", ret );

	return ret;
}




void
n_mac_desktop_size( CGFloat *sx, CGFloat *sy )
{

	NSRect rect = [[NSScreen mainScreen] visibleFrame];

	if ( sx != NULL ) {  (*sx) = NSWidth ( rect ); }
	if ( sy != NULL ) {  (*sy) = NSHeight( rect ); }


	return;
}




void
n_mac_topmost( NSWindow *window, BOOL onoff )
{

	if ( onoff )
	{
		[window setLevel:NSFloatingWindowLevel];
	} else {
		[window setLevel:NSNormalWindowLevel];
	}

	return;
}

void
n_mac_foreground( void )
{
	[NSApp activateIgnoringOtherApps:YES];
	[NSApp activateIgnoringOtherApps: NO];

	return;
}




NSString*
n_mac_settings_read( NSString *nsstr_key )
{

	CFStringRef key = (__bridge CFStringRef) nsstr_key;
	CFStringRef ret = CFPreferencesCopyAppValue( key, kCFPreferencesCurrentApplication );

	return (__bridge NSString*) ret;
}

void
n_mac_settings_write( NSString *nsstr_key, NSString *nsstr_val )
{

	CFStringRef key = (__bridge CFStringRef) nsstr_key;
	CFStringRef val = (__bridge CFStringRef) nsstr_val;

	CFPreferencesSetAppValue( key, val, kCFPreferencesCurrentApplication );

	return;
}




NSString*
n_mac_alias_resolve( NSString *path )
{

	// [Needed] : Sandbox OFF

	NSError *error = nil;
	NSURL   *ret   = [NSURL URLByResolvingAliasFileAtURL:[NSURL fileURLWithPath:path]
						     options:NSURLBookmarkResolutionWithoutUI
						       error:&error
	];

//NSLog( @"%@", [ret path] );


	return [ret path];
}




NSString*
n_mac_webloc_resolve( NSString *path )
{

	// [Needed] : Sandbox OFF

/*
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>URL</key>
	<string>https://cheznonnon.blogspot.com/</string>
</dict>
</plist>
 */

	NSURL *url = [NSURL fileURLWithPath:path];


	NSError *error = nil;


	NSUInteger options = NSXMLNodePreserveWhitespace | NSXMLDocumentTidyXML;

	NSXMLDocument *xml = [[NSXMLDocument alloc]
		initWithContentsOfURL:url
			      options:options
		                error:&error
	];

	if ( error ) { return @""; }


	NSArray *array = [xml nodesForXPath:@".//plist/dict/string" error:&error];

	if ( error ) { return @""; }

	NSXMLNode *node = [array objectAtIndex:0];
	NSString  *str  = [node stringValue];
//NSLog( @"%@", str );


	return str;
}

void
n_mac_webloc_save( const n_posix_char *fname, const n_posix_char *url )
{

	// [Needed] : Sandbox OFF

/*
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>URL</key>
	<string>https://cheznonnon.blogspot.com/</string>
</dict>
</plist>
 */
 
	n_txt txt; n_txt_zero( &txt ); n_txt_new( &txt );

	n_posix_char *str = n_string_new( n_posix_strlen( url ) * 2 );
	n_posix_sprintf_literal( str, "<string>%s</string>", url );

	n_txt_add( &txt, 0, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
	n_txt_add( &txt, 1, "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">" );
	n_txt_add( &txt, 2, "<plist version=\"1.0\">" );
	n_txt_add( &txt, 3, "<dict>" );
	n_txt_add( &txt, 4, "<key>URL</key>" );
	n_txt_add( &txt, 5, str );
	n_txt_add( &txt, 6, "</dict>" );
	n_txt_add( &txt, 7, "</plist>" );

	n_txt_save( &txt, fname );


	return;
}




BOOL
n_mac_doubleclick_detect( int *phase, u32 *msec )
{

	BOOL onoff = FALSE;

	if ( (*phase) == 0 )
	{
		(*phase) = 1;
		(*msec )  = n_posix_tickcount();
	} else
	if ( (*phase) == 1 )
	{
		if ( n_posix_false == n_game_timer_once( msec, 500 ) )
		{
			onoff = TRUE;
			(*phase) = 0;
		} else {
			(*msec ) = n_posix_tickcount();
		}
	}


	return onoff;
}




NSString*
n_mac_fork_rename( NSString *path )
{
/*
	n_posix_char *str = n_mac_nsstring2str( paint.filename );
	n_posix_char *dir = n_string_path_upperfolder_new( str );
	n_posix_char *ext = n_string_path_ext_get_new( str );
	n_posix_char *nam = n_string_path_tmpname_new( ext );
	n_posix_char *tmp = n_string_path_make_new( dir, nam );

//NSLog( @"str %@", paint.filename );
//NSLog( @"str %s", str );
//NSLog( @"dir %s", dir );
//NSLog( @"ext %s", ext );
//NSLog( @"nam %s", nam );
//NSLog( @"tmp %s", tmp );

	NSString *ret = n_mac_str2nsstring( tmp );

	n_string_free( str );
	n_string_free( dir );
	n_string_free( ext );
	n_string_free( nam );
	n_string_free( tmp );

	return ret;
*/

	n_posix_char tmp[ 100 ]; n_string_path_tmpname( tmp );

	NSString *dir = [path stringByDeletingLastPathComponent];
	NSString *ext = [path pathExtension];
	NSString *nam = n_mac_str2nsstring( tmp );
//NSLog( @"%@ : %@ : %@", dir, ext, nam );

	nam = [nam stringByAppendingPathExtension:ext];


	return [dir stringByAppendingPathComponent:nam];
}


#endif // _H_NONNON_MAC


