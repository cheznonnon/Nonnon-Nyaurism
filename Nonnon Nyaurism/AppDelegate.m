//
//  AppDelegate.m
//  Nonnon Nyaurism
//
//  Created by のんのん２ on 2025/12/19.
//




#import "AppDelegate.h"




#include "extern.h"




#define N_NYAURISM_FFT_RESOLUTION pow( 2, 14 ) // [!] : higher is better but heavy


#define N_NYAURISM_PLAYBACK_STOP    ( 0 )
#define N_NYAURISM_PLAYBACK_PLAYING ( 1 )
#define N_NYAURISM_PLAYBACK_PAUSE   ( 2 )




// [!] : plotter.m uses these data and n_nyaurism_slider_redraw()

typedef struct {

	AppDelegate *AppDelegate;

	NSString    *fname;

	n_bmp        bmp;
	n_wav        wav;

	n_wav        wav_undo;
	n_wav        wav_base;
	n_wav        wav_clip;

	BOOL         mix_onoff;

	NSView      *plotter;
	int          plotter_selection_channel;

	NSSlider    *slider_body_l;
	NSSlider    *slider_body_r;

	NSTextField *slider_label_l;
	NSTextField *slider_label_r;

	n_type_real  slider_value_l;
	n_type_real  slider_value_r;

	n_bmp        seekbar_bmp;
	float        seekbar_float_norm;

	int          playback_status;
	u32          playback_timeout;
	float        playback_msec_x;
	float        playback_msec_sx;

	BOOL         selection_command_onoff;

	BOOL         selection_shift_onoff;
	n_type_gfx   selection_shift_pixel;

	BOOL         selection_reselect_onoff;
	n_type_gfx   selection_reselect_pixel;

	BOOL         selection_reverse;
	BOOL         selection_drag_onoff;
	BOOL         selection_select_all;

	n_type_gfx   selection_step;
	n_type_gfx   selection_size_pixel;
	n_type_gfx   selection_from_pixel;
	n_type_gfx   selection_loop_pixel;

} n_nyaurism_struct;

#define n_nyaurism_zero( p ) n_memory_zero( p, sizeof( n_nyaurism_struct ) )


static n_nyaurism_struct  n_nyaurism_instance;
static n_nyaurism_struct *n_nyaurism = &n_nyaurism_instance;




#define n_nyaurism_now_playing() ( n_nyaurism->seekbar_float_norm != -1 )




void
n_nyaurism_slider_redraw( n_wav *wav )
{

	extern void n_nyaurism_plotter_selection_pixel2sample( n_wav *wav, n_type_gfx *ret_x, n_type_gfx *ret_sx );
	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( wav, &x, &sx );


	if ( sx == 0 ) { sx = N_WAV_COUNT( wav ); }

//x = 0; sx = N_WAV_COUNT( wav );


	n_type_real hi_l, hi_r;
	n_wav_peak_value( wav, x, sx, &hi_l, &hi_r );
//NSLog( @"%f %f", hi_l, hi_r );

	n_nyaurism->slider_value_l = hi_l / n_wav_sample_amp( wav );
	n_nyaurism->slider_value_r = hi_r / n_wav_sample_amp( wav );
//NSLog( @"%f %f %f %f", hi_l, hi_r, n_slider_value_l_global, n_slider_value_r_global );

	[n_nyaurism->slider_body_l setIntValue:n_nyaurism->slider_value_l * 100];
	[n_nyaurism->slider_body_r setIntValue:n_nyaurism->slider_value_r * 100];


	n_nyaurism->slider_label_l.stringValue = [NSString stringWithFormat:@"%0.0f%%", n_nyaurism->slider_value_l * 100];;
	n_nyaurism->slider_label_r.stringValue = [NSString stringWithFormat:@"%0.0f%%", n_nyaurism->slider_value_r * 100];;


	if ( n_nyaurism->plotter_selection_channel == 1 )
	{
		[n_nyaurism->AppDelegate NonnonNyaurismSliderUIOnOff:nil L:FALSE R:TRUE];
	} else
	if ( n_nyaurism->plotter_selection_channel == 2 )
	{
		[n_nyaurism->AppDelegate NonnonNyaurismSliderUIOnOff:nil L:TRUE R:FALSE];
	} else {
		[n_nyaurism->AppDelegate NonnonNyaurismSliderUIOnOff:nil L:TRUE R:TRUE];
	}


	return;
}

void
n_nyaurism_backup( n_wav *f, n_wav *t )
{

	n_wav_free( t );
	n_wav_carboncopy( f, t );

	return;
}




#include "AudioQueue.c"
#include "stub.c"


#import "plotter.m"




@interface NonnonNSView : NSView

@end

@implementation NonnonNSView

- init
{
	self = [super init];
	if ( self )
	{
	}

	return self;
}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	if ( n_nyaurism->plotter != nil )
	{
		n_nyaurism_plotter_selection_off();
		[n_nyaurism->plotter display];
	}

	[super mouseDown:theEvent];

}

@end




BOOL
n_nyaurism_wav_load( n_wav *wav, n_posix_char *path, BOOL *rename_needed )
{

	if ( FALSE == n_wav_load( wav, path ) ) { return FALSE; }
//return TRUE;


	n_posix_char  tmp_name[ 100 ]; n_string_path_tmpname( tmp_name );

	NSString     *tmp_dir   = NSTemporaryDirectory();
	NSString     *tmp_file  = [NSString stringWithFormat:@"%@/%s.wav", tmp_dir, tmp_name];

	n_posix_char *tmp_path  = n_mac_nsstring2str( tmp_file );
	NSString     *tmp_nsstr = n_mac_str2nsstring( tmp_path );

	n_filer_copy( path, tmp_path );


	AVAudioPCMBuffer *buffer = n_mac_sound_load( tmp_nsstr );
	n_mac_sound_save( buffer, tmp_nsstr );


	BOOL ret = n_wav_load( wav, tmp_path );

	n_filer_remove( tmp_path );


	n_string_free ( tmp_path );


	if ( ret == FALSE )
	{
		if ( rename_needed != NULL ) { *rename_needed = TRUE; }
	}


	return ret;
}




@interface CustomSliderCell : NSSliderCell
@end

@implementation CustomSliderCell

- (BOOL)startTrackingAt:(NSPoint)startPoint inView:(NSView *)controlView {
//NSLog(@"startTrackingAt");

	n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_undo );
	n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );

	return [super startTrackingAt:startPoint inView:controlView];
}

- (void)stopTracking:(NSPoint)lastPoint at:(NSPoint)stopPoint inView:(NSView *)controlView mouseIsUp:(BOOL)flag {
//NSLog(@"stopTracking (mouseIsUp: %@)", flag ? @"YES" : @"NO");

	[super stopTracking:lastPoint at:stopPoint inView:controlView mouseIsUp:flag];

}

@end




@interface AppDelegate ()

@property (strong) IBOutlet NonnonStub *n_window_main_stub;
@property (strong) IBOutlet NonnonStub *n_window_eq_stub;

@property (strong) IBOutlet NSWindow *window;

@property (weak) IBOutlet NonnonPlotter *n_plotter;

@property (weak) IBOutlet NonnonButton *n_button_zero;
@property (weak) IBOutlet NonnonButton *n_button_mix;
@property (weak) IBOutlet NonnonButton *n_button_stop;
@property (weak) IBOutlet NonnonButton *n_button_play;
@property (weak) IBOutlet NonnonButton *n_button_eq;
@property (weak) IBOutlet NonnonButton *n_button_save;
@property (weak) IBOutlet NonnonButton *n_button_resizer;

@property (weak) IBOutlet NSMenu     *n_popup_menu;
@property (weak) IBOutlet NSMenuItem *n_popup_menu_cut;
@property (weak) IBOutlet NSMenuItem *n_popup_menu_copy;
@property (weak) IBOutlet NSMenuItem *n_popup_menu_paste;
@property (weak) IBOutlet NSMenuItem *n_popup_menu_overwrite;

@property (weak) IBOutlet NSWindow   *n_save_window;
@property (weak) IBOutlet NSComboBox *n_save_combo_channel;
@property (weak) IBOutlet NSComboBox *n_save_combo_bit;
@property (weak) IBOutlet NSComboBox *n_save_combo_rate;

@property (weak) IBOutlet NSTextField *n_slider_label_base_l;
@property (weak) IBOutlet NSTextField *n_slider_label_base_r;
@property (weak) IBOutlet NSTextField *n_slider_label_l;
@property (weak) IBOutlet NSTextField *n_slider_label_r;
@property (weak) IBOutlet NSSlider    *n_slider_body_l;
@property (weak) IBOutlet NSSlider    *n_slider_body_r;

@property (weak) IBOutlet NSWindow    *n_resizer_window;
@property (weak) IBOutlet NSTextField *n_resizer_label;

@property (weak) IBOutlet NSWindow *n_eq_window;
@property (weak) IBOutlet NSSlider *n_eq_0;
@property (weak) IBOutlet NSSlider *n_eq_1;
@property (weak) IBOutlet NSSlider *n_eq_2;
@property (weak) IBOutlet NSSlider *n_eq_3;
@property (weak) IBOutlet NSSlider *n_eq_4;
@property (weak) IBOutlet NSSlider *n_eq_5;
@property (weak) IBOutlet NSSlider *n_eq_6;
@property (weak) IBOutlet NSSlider *n_eq_7;
@property (weak) IBOutlet NSSlider *n_eq_8;
@property (weak) IBOutlet NSSlider *n_eq_9;

@property (weak) IBOutlet NSTextField *n_eq_label_0;
@property (weak) IBOutlet NSTextField *n_eq_label_1;
@property (weak) IBOutlet NSTextField *n_eq_label_2;
@property (weak) IBOutlet NSTextField *n_eq_label_3;
@property (weak) IBOutlet NSTextField *n_eq_label_4;
@property (weak) IBOutlet NSTextField *n_eq_label_5;
@property (weak) IBOutlet NSTextField *n_eq_label_6;
@property (weak) IBOutlet NSTextField *n_eq_label_7;
@property (weak) IBOutlet NSTextField *n_eq_label_8;
@property (weak) IBOutlet NSTextField *n_eq_label_9;

@property (weak) IBOutlet NSButton *n_eq_button_apply;
@property (weak) IBOutlet NSButton *n_eq_button_undo;

@end




@implementation AppDelegate {

	BOOL popup_menu_shift_onoff;

}




- (void) NonnonIconSet:(NSString*) rc_name button:(NonnonButton*)button
{

	n_bmp bmp_icon; n_bmp_zero( &bmp_icon );

	n_mac_image_rc_load_bmp( rc_name, &bmp_icon );
//NSLog( @"%d", n_bmp_error( &bmp_icon ) );

	n_mac_button_system_themed( &bmp_icon );

	[button n_icon_free];
	[button n_icon_set:&bmp_icon];
	[button display];

}

- (void) NonnonIconGDI:(n_posix_char*)text button:(NonnonButton*)button
{

	n_bmp bmp; n_bmp_zero( &bmp );

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.sx                  = 40;
	gdi.sy                  = 40;

	gdi.text                = text;
	gdi.text_font           = n_posix_literal( "Trebuchet MS" );
	gdi.text_size           = 14;
	gdi.text_style          = N_GDI_TEXT_SMOOTH;
	gdi.text_color_main     = n_bmp_rgb( 255,255,255 );

	u32 color_outline = n_mac_nscolor2argb( [NSColor controlAccentColor] );
	if ( n_mac_is_darkmode() )
	{
		color_outline = n_bmp_blend_pixel( color_outline, n_bmp_black, 0.25 );
	} else {
		color_outline = n_bmp_blend_pixel( color_outline, n_bmp_black, 0.25 );
	}

	gdi.text_effect_style[ 0 ] = N_GDI_EFFECT_OUTLINE;
	gdi.text_effect_color[ 0 ] = color_outline;
	gdi.text_effect_param[ 0 ] = 1;

	n_bmp_free( &bmp );
	n_gdi_bmp( &gdi, &bmp );

	[button n_icon_set:&bmp];
	[button display];

	n_bmp_free( &bmp );

}




-(void)NonnonNyaurismTitle
{
	NSString *title = [NSString stringWithFormat:@"%@ - Nyaurism", n_nyaurism->fname];
	[_window setTitle:title];
}

-(NSString*)NonnonNyaurismSliderLabel:(n_type_real)value
{
	return [NSString stringWithFormat:@"%0.0f%%", value * 100];
}

-(NSString*)NonnonNyaurismMsecNSString
{
	int msec = N_WAV_MSEC( &n_nyaurism->wav );

	return [NSString stringWithFormat:@"%d", msec];
}




- (void)awakeFromNib
{

	n_gdi_scale_factor = _window.backingScaleFactor;


	n_mac_image_window = _window;
	n_gdi_scale_factor = n_mac_image_window.backingScaleFactor;


	// [!] : instance

	n_nyaurism_zero( n_nyaurism );

	n_nyaurism->AppDelegate = self;

	n_nyaurism->plotter = _n_plotter;

	n_nyaurism->slider_body_l = _n_slider_body_l;
	n_nyaurism->slider_body_r = _n_slider_body_r;

	n_nyaurism->slider_label_l = _n_slider_label_l;
	n_nyaurism->slider_label_r = _n_slider_label_r;

	n_nyaurism->seekbar_float_norm = -1;


	// [Needed] : DnD Support

	_n_window_main_stub = [[NonnonStub alloc] init];

	[_n_window_main_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_window contentView] addSubview:_n_window_main_stub];

	_n_window_main_stub.delegate = self;


	_n_window_eq_stub = [[NonnonStub alloc] init];

	[_n_window_eq_stub setFrame:[[NSScreen mainScreen] frame]];
	[[_n_eq_window contentView] addSubview:_n_window_eq_stub];

	_n_window_eq_stub.delegate = self;


	{
		NSArray      *paths   = NSSearchPathForDirectoriesInDomains( NSDesktopDirectory, NSUserDomainMask, YES );
		NSString     *desktop = [paths objectAtIndex:0];
		n_posix_char  tmpname[ 100 ]; n_string_path_tmpname( tmpname );

		n_nyaurism->fname = [NSString stringWithFormat:@"%@/%s.wav", desktop, tmpname];
		//[self NonnonNyaurismTitle];
		[_window setTitle:@"Nonnon Nyaurism"];
	}


	n_wav_zero( &n_nyaurism->wav ); n_wav_new_by_sample( &n_nyaurism->wav, 44100 * 5 );
	n_wav_martian( &n_nyaurism->wav, 0.25, 0.25 );

	n_wav_carboncopy( &n_nyaurism->wav, &n_nyaurism->wav_undo );
	n_wav_carboncopy( &n_nyaurism->wav, &n_nyaurism->wav_base );

	[_n_plotter display];


	{
		[_n_button_zero n_enable:TRUE];
		[_n_button_zero n_border:TRUE];
		[_n_button_zero n_nswindow_set:_window];
		[_n_button_zero n_direct_click:TRUE];

		[self NonnonIconSet:@"zero" button:_n_button_zero];

		_n_button_zero.delegate = self;
	}

	{
		[_n_button_mix n_enable:TRUE];
		[_n_button_mix n_border:TRUE];
		[_n_button_mix n_nswindow_set:_window];
		[_n_button_mix n_direct_click:TRUE];

		//[self NonnonIconSet:@"mix" button:_n_button_mix];
		[self NonnonIconGDI:"Mix" button:_n_button_mix];

		_n_button_mix.delegate = self;
	}

	{
		[_n_button_stop n_enable:TRUE];
		[_n_button_stop n_border:TRUE];
		[_n_button_stop n_nswindow_set:_window];
		[_n_button_stop n_direct_click:TRUE];

		[self NonnonIconSet:@"stop" button:_n_button_stop];

		_n_button_stop.delegate = self;
	}

	{
		[_n_button_play n_enable:TRUE];
		[_n_button_play n_border:TRUE];
		[_n_button_play n_nswindow_set:_window];
		[_n_button_play n_direct_click:TRUE];

		[self NonnonIconSet:@"play" button:_n_button_play];

		_n_button_play.delegate = self;
	}

	{
		[_n_button_eq n_enable:TRUE];
		[_n_button_eq n_border:TRUE];
		[_n_button_eq n_nswindow_set:_window];
		[_n_button_eq n_direct_click:TRUE];

		[self NonnonIconSet:@"eq" button:_n_button_eq];

		_n_button_eq.delegate = self;
	}

	{
		[_n_button_resizer n_enable:TRUE];
		[_n_button_resizer n_border:TRUE];
		[_n_button_resizer n_nswindow_set:_window];
		[_n_button_resizer n_direct_click:TRUE];

		[self NonnonIconSet:@"resizer" button:_n_button_resizer];

		_n_button_resizer.delegate = self;
	}

	{
		[_n_button_save n_enable:TRUE];
		[_n_button_save n_border:TRUE];
		[_n_button_save n_nswindow_set:_window];
		[_n_button_save n_direct_click:TRUE];

		[self NonnonIconSet:@"save" button:_n_button_save];

		_n_button_save.delegate = self;
	}


	_n_plotter.n_popup_menu = _n_popup_menu;

	_n_popup_menu.delegate = self;

	n_nyaurism_slider_redraw( &n_nyaurism->wav );


	[_n_resizer_label setDelegate:self];


	[[NSNotificationCenter defaultCenter]
	      addObserver: self
		 selector: @selector( windowWillClose: )
		     name: NSWindowWillCloseNotification
		   object: nil
	];

	// [!] : accent color
	[[NSDistributedNotificationCenter defaultCenter]
		addObserver: self
		   selector: @selector( accentColorChanged: )
		       name: @"AppleColorPreferencesChangedNotification"
		     object: nil
	];

	// [!] : dark mode
	[[NSDistributedNotificationCenter defaultCenter]
		addObserver: self
		   selector: @selector( darkModeChanged: )
		       name: @"AppleInterfaceThemeChangedNotification"
		     object: nil
	];

	n_mac_timer_init( self, @selector( n_timer_method_playback ), 10 );

}




- (void) windowWillClose:(NSNotification *)notification
{
//NSLog( @"closed" );

	NSWindow *window = notification.object;
	if ( window == self.window )
	{
		[NSApp terminate:self];
	}
}




- (BOOL)application:(NSApplication *)sender
           openFile:(NSString *)filename
{
//return NO;

	NSString *path = n_mac_alias_resolve( filename );

	[self NonnonDragAndDrop_dropped:path];

	n_mac_window_centering( _window );

	n_mac_window_show( _window );


	return YES;
}

- (void)NonnonDragAndDrop_dropped:(NSString*)nsstr
{
//NSLog( @"%@", nsstr );


	n_nyaurism->fname = nsstr;


	n_posix_char *str = n_mac_nsstring2str( n_nyaurism->fname );


	n_wav wav_local; n_wav_zero( &wav_local );

	BOOL rename_needed = FALSE;

	if ( n_nyaurism_wav_load( &wav_local, str, &rename_needed ) )
	{
//NSLog( @"n_wav : load error" );

		n_mac_window_dialog_ok( "Not Supported" );

	} else {
//n_mac_sound_AudioQueue_play( &wav );

		if ( n_nyaurism->mix_onoff )
		{
			n_wav_free( &n_nyaurism->wav_clip );
			n_wav_alias( &wav_local, &n_nyaurism->wav_clip );

			//n_nyaurism_tooltip_calc( N_WAV_COUNT( &wav_local ) );

			n_nyaurism_plotter_overwrite( &n_nyaurism->wav );

			n_nyaurism_slider_redraw( &n_nyaurism->wav );

			[_n_plotter display];
		} else {
			if ( rename_needed )
			{
				n_nyaurism->fname = [n_nyaurism->fname stringByDeletingPathExtension];
				n_nyaurism->fname = [n_nyaurism->fname stringByAppendingPathExtension:@"wav"];
//NSLog( @"%@", n_nyaurism->fname );
			}

			n_wav_free( &n_nyaurism->wav );
			n_wav_alias( &wav_local, &n_nyaurism->wav );

			[self NonnonNyaurismTitle];

			n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_undo );
			n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );

			n_nyaurism_plotter_selection_off();

			n_nyaurism_slider_redraw( &n_nyaurism->wav );

			[_n_plotter display];
		}
	}


	n_string_free( str );

}




- (void) accentColorChanged:(NSNotification *)notification
{
//NSLog( @"accentColorChanged" );

	// [x] : Sonoma : buggy : old value is set

	n_mac_timer_init_once( self, @selector( n_timer_method_color ), 200 );

}

- (void) n_timer_method_color
{
//NSLog( @"n_timer_method_color" );

	[self NonnonIconSet:@"zero"    button:_n_button_zero   ];
	//[self NonnonIconSet:@"mix"     button:_n_button_mix    ];
	[self NonnonIconGDI:"Mix"      button:_n_button_mix    ];
	[self NonnonIconSet:@"stop"    button:_n_button_stop   ];
	[self NonnonIconSet:@"play"    button:_n_button_play   ];
	[self NonnonIconSet:@"eq"      button:_n_button_eq     ];
	[self NonnonIconSet:@"resizer" button:_n_button_resizer];
	[self NonnonIconSet:@"save"    button:_n_button_save   ];

}

- (void) darkModeChanged:(NSNotification *)notification
{
//NSLog( @"darkModeChanged" );

	[self n_timer_method_color];

}




- (void) NonnonNyaurismSliderUIOnOff:(void*)stub L:(BOOL)onoff_l R:(BOOL)onoff_r
{

	CGFloat blend_l;
	if ( onoff_l )
	{
		blend_l = 1.0;
	} else {
		blend_l = 0.2;
	}

	[_n_slider_body_l       setEnabled:onoff_l];
	[_n_slider_label_base_l setAlphaValue:blend_l];
	[_n_slider_label_l      setAlphaValue:blend_l];


	CGFloat blend_r;
	if ( onoff_r )
	{
		blend_r = 1.0;
	} else {
		blend_r = 0.2;
	}

	[_n_slider_body_r       setEnabled:onoff_r];
	[_n_slider_label_base_r setAlphaValue:blend_r];
	[_n_slider_label_r      setAlphaValue:blend_r];

}

- (void) NonnonNyaurismPlaybackUIOnOff:(BOOL)onoff
{

	[_n_button_zero    n_enable:onoff]; [_n_button_zero    display];
	[_n_button_mix     n_enable:onoff]; 
	[_n_button_resizer n_enable:onoff]; [_n_button_resizer display];
	[_n_button_save    n_enable:onoff]; [_n_button_save    display];
	[_n_button_eq      n_enable:onoff]; [_n_button_eq      display];

	[_n_button_mix n_fake:n_nyaurism->mix_onoff];
	[_n_button_mix display];


	CGFloat blend;
	if ( onoff )
	{
		blend = 1.0;
	} else {
		blend = 0.2;
	}

	[_n_slider_body_l       setEnabled:onoff];
	[_n_slider_label_base_l setAlphaValue:blend];
	[_n_slider_label_l      setAlphaValue:blend];

	[_n_slider_body_r       setEnabled:onoff];
	[_n_slider_label_base_r setAlphaValue:blend];
	[_n_slider_label_r      setAlphaValue:blend];

}

- (void) NonnonNyaurismPlaybackReset:(BOOL)force_stop
{
//return;

	if ( force_stop ) { n_mac_sound_AudioQueue_stop(); }

	[self NonnonIconSet:@"play" button:_n_button_play];
	n_nyaurism->playback_status = N_NYAURISM_PLAYBACK_STOP;

	n_bmp_flush( &n_nyaurism->seekbar_bmp, n_bmp_black_invisible );
	n_nyaurism->seekbar_float_norm = -1;

	[self NonnonNyaurismPlaybackUIOnOff:TRUE];

	n_mac_window_all_controls_onoff( [_n_eq_window contentView], YES );

	[_n_plotter display];

}

- (void) n_timer_method_playback
{

	if ( n_nyaurism->playback_status == N_NYAURISM_PLAYBACK_PLAYING )
	{
		if ( n_nyaurism->playback_timeout < n_posix_tickcount() )
		{
			[self NonnonNyaurismPlaybackReset:NO];
		} else {
			float  x;
			 x = (float) n_nyaurism->playback_msec_x;
			 x = (float)  x / N_WAV_MSEC( &n_nyaurism->wav );

			float sx;
			sx = (float) n_nyaurism->playback_msec_sx;
			sx = (float) sx / N_WAV_MSEC( &n_nyaurism->wav );

			float rest;
			rest = n_nyaurism->playback_timeout - n_posix_tickcount();
			rest = (float) rest / n_nyaurism->playback_msec_sx;
			rest = 1.0 - rest;
//NSLog( @"%f", rest );

			n_nyaurism->seekbar_float_norm = rest + x;
//NSLog( @"%f %f", n_nyaurism_seekbar_float_norm, x + sx );

			if ( n_nyaurism->seekbar_float_norm > 1.0 )
			{
				[self NonnonNyaurismPlaybackReset:NO];
			} else
			if ( n_nyaurism->seekbar_float_norm > ( x + sx ) )
			{
				[self NonnonNyaurismPlaybackReset:YES];
			} else {
				[_n_plotter display];
			}
		}
	}

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");
}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog(@"mouseUp");

	if ( [_n_button_zero n_is_pressed] )
	{
//NSLog( @"_n_button_zero" );

		n_nyaurism_plotter_mute( &n_nyaurism->wav );

		[_n_plotter display];
	} else
	if ( [_n_button_mix n_is_pressed] )
	{
//NSLog( @"_n_button_mix" );

		if ( n_nyaurism->mix_onoff )
		{
			n_nyaurism->mix_onoff = FALSE;
		} else {
			n_nyaurism->mix_onoff =  TRUE;
		}

		[_n_button_mix n_fake:n_nyaurism->mix_onoff];
	} else
	if ( [_n_button_stop n_is_pressed] )
	{
//NSLog( @"_n_button_stop" );

		[self NonnonNyaurismPlaybackReset:YES];

	} else
	if ( [_n_button_play n_is_pressed] )
	{
//NSLog( @"_n_button_play" );

		if ( n_nyaurism->playback_status == N_NYAURISM_PLAYBACK_STOP )
		{
			n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( &n_nyaurism->wav, &x, &sx );
			if ( sx == 0 ) { sx = N_WAV_COUNT( &n_nyaurism->wav ); }

			const float sample_per_msec = (float) N_WAV_RATE( &n_nyaurism->wav ) / 1000;

			n_nyaurism->playback_msec_x  = (float)  x / sample_per_msec;
			n_nyaurism->playback_msec_sx = (float) sx / sample_per_msec;
//NSLog( @"playback msec : %f %f", n_nyaurism_playback_msec_x, n_nyaurism_playback_msec_sx );

			[self NonnonNyaurismPlaybackUIOnOff:FALSE];

			n_mac_window_all_controls_onoff( [_n_eq_window contentView], NO );

			[self NonnonIconSet:@"pause" button:_n_button_play];
	
			n_mac_sound_AudioQueue_init( &n_nyaurism->wav, x, sx );
			n_mac_sound_AudioQueue_play();

			n_nyaurism->playback_timeout = n_posix_tickcount() + n_nyaurism->playback_msec_sx;

			n_nyaurism->playback_status = N_NYAURISM_PLAYBACK_PLAYING;
		} else {
			if ( n_nyaurism->playback_status == N_NYAURISM_PLAYBACK_PLAYING )
			{
				n_nyaurism->playback_status = N_NYAURISM_PLAYBACK_PAUSE;

				[self NonnonIconSet:@"play" button:_n_button_play];
				n_mac_sound_AudioQueue_pause();

				n_nyaurism->playback_timeout -= n_posix_tickcount();
			} else
			if ( n_nyaurism->playback_status == N_NYAURISM_PLAYBACK_PAUSE )
			{
				n_nyaurism->playback_status = N_NYAURISM_PLAYBACK_PLAYING;

				[self NonnonIconSet:@"pause" button:_n_button_play];
				n_mac_sound_AudioQueue_play();

				n_nyaurism->playback_timeout += n_posix_tickcount();
			}
		}
	} else
	if ( [_n_button_eq n_is_pressed] )
	{
//NSLog( @"_n_button_eq" );

		n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );

		NSRect parent = [_window      frame];
		NSRect child  = [_n_eq_window frame];
		child.origin.x = parent.origin.x + parent.size.width + 24;

		[_n_eq_window setFrameOrigin:child.origin];

		[_n_eq_window makeKeyWindow];
		n_mac_window_show( _n_eq_window );
	} else
	if ( [_n_button_save n_is_pressed] )
	{
//NSLog( @"_n_button_save" );
//NSLog( @"%d %d %d", n_nyaurism->wav.channel, n_nyaurism->wav.bit, n_nyaurism->wav.rate );

		if ( n_nyaurism->wav.channel == 1 )
		{
			[_n_save_combo_channel selectItemAtIndex:0];
		} else
		if ( n_nyaurism->wav.channel == 2 )
		{
			[_n_save_combo_channel selectItemAtIndex:1];
		}

		if ( n_nyaurism->wav.bit == 8 )
		{
			[_n_save_combo_bit selectItemAtIndex:0];
		} else
		if ( n_nyaurism->wav.bit == 16 )
		{
			[_n_save_combo_bit selectItemAtIndex:1];
		} else
		if ( n_nyaurism->wav.bit == 32 )
		{
			[_n_save_combo_bit selectItemAtIndex:2];
		}

		if ( n_nyaurism->wav.rate == 11 )
		{
			[_n_save_combo_rate selectItemAtIndex:0];
		} else
		if ( n_nyaurism->wav.rate == 22 )
		{
			[_n_save_combo_rate selectItemAtIndex:1];
		} else
		if ( n_nyaurism->wav.rate == 44 )
		{
			[_n_save_combo_rate selectItemAtIndex:2];
		}

		n_mac_window_all_controls_onoff( [_n_eq_window contentView], NO );

		[_window beginSheet:_n_save_window completionHandler:^(NSModalResponse returnCode)
		{
			//
		}
		];

	} else
	if ( [_n_button_resizer n_is_pressed] )
	{
//NSLog( @"_n_button_resizer" );

		_n_resizer_label.stringValue = [self NonnonNyaurismMsecNSString];

		n_mac_window_all_controls_onoff( [_n_eq_window contentView], NO );

		[_window beginSheet:_n_resizer_window completionHandler:^(NSModalResponse returnCode)
		{
			//
		}
		];

	}

}




-(void)menuWillOpen:(NSMenu*)menu
{
//NSLog( @"menuWillOpen" );

	// [Needed] : menu.delegate = self;

	NSUInteger flags = [NSEvent modifierFlags];

	if ( flags & NSEventModifierFlagShift )
	{
		popup_menu_shift_onoff = TRUE;
	}

}

-(void)menuDidClose:(NSMenu*)menu
{
//NSLog( @"menuDidClose" );

	// [Needed] : menu.delegate = self;

	NSUInteger flags = [NSEvent modifierFlags];

	if ( flags & NSEventModifierFlagShift )
	{
		//
	} else
	if ( popup_menu_shift_onoff )
	{
		popup_menu_shift_onoff = FALSE;

		n_nyaurism->selection_shift_onoff = FALSE;
		[_n_plotter display];
	}

}

-(BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
//NSLog( @"validateMenuItem" );

	// [x] : deprecated warning: set NSMenuItemValidation at AppDelegate.h

	if ( n_nyaurism->selection_shift_onoff )
	{
//NSLog( @"%@ : %ld", menuItem.title, menuItem.tag );

		// [!] : you can set tag by right-pane of XIB

		if ( menuItem.tag != 0 )
		{
			return NO;
		}
	} else
	if ( n_nyaurism->plotter_selection_channel != 0 )
	{
		// [!] : except for "Unselect"

		if ( menuItem.tag == 1 )
		{
			return NO;
		}
	}


	return YES;
}
/*
-(void)menuNeedsUpdate:(NSMenu*)menu
{
NSLog( @"menuNeedsUpdate" );

	// [x] : not working : set NSMenuDelegate at AppDelegate.h

}
*/
- (IBAction)n_plotter_menu_undo:(id)sender {

	n_nyaurism_plotter_undo( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_select_all:(id)sender {

	n_nyaurism_plotter_select_all( &n_nyaurism->wav, YES );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_unselect:(id)sender {

	n_nyaurism_plotter_selection_off();

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_cut:(id)sender {

	n_nyaurism_plotter_cut( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_copy:(id)sender {

	n_nyaurism_plotter_copy( &n_nyaurism->wav, &n_nyaurism->wav_clip );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_paste:(id)sender {

	n_nyaurism_plotter_paste( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_overwrite:(id)sender {

	n_nyaurism_plotter_overwrite( &n_nyaurism->wav );

	[_n_plotter display];
}

- (IBAction)n_plotter_menu_mute:(id)sender {
//NSLog( @"n_plotter_menu_mute" );

	n_nyaurism_plotter_mute( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_fade_in:(id)sender {

	n_nyaurism_plotter_fade_in( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_fade_out:(id)sender {

	n_nyaurism_plotter_fade_out( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_monaural:(id)sender {

	n_nyaurism_plotter_monoaural( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_L2R:(id)sender {

	n_nyaurism_plotter_L2R( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_R2L:(id)sender {

	n_nyaurism_plotter_R2L( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_smoother:(id)sender {

	n_nyaurism_plotter_smoother( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_overdrive:(id)sender {

	n_nyaurism_plotter_overdrive( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_tremolo:(id)sender {

	n_nyaurism_plotter_tremolo( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_whitenoise:(id)sender {

	n_nyaurism_plotter_whitenoise( &n_nyaurism->wav );

	[_n_plotter display];

}

- (IBAction)n_plotter_menu_pinknoise:(id)sender {

	n_nyaurism_plotter_pinknoise( &n_nyaurism->wav );

	[_n_plotter display];

}




- (IBAction)n_slider_l_method:(id)sender
{
//NSLog( @"%d", (int) [sender integerValue] );


	n_nyaurism->slider_value_l = (n_type_real) [sender integerValue] / 100.0;

	_n_slider_label_l.stringValue = [self NonnonNyaurismSliderLabel:n_nyaurism->slider_value_l];


	n_nyaurism_backup( &n_nyaurism->wav_base, &n_nyaurism->wav );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( &n_nyaurism->wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( &n_nyaurism->wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

//NSLog( @"%f %f", n_slider_value_l_global, n_slider_value_r_global );
	n_wav_normalize_partial( &n_nyaurism->wav, x, sx, n_nyaurism->slider_value_l, n_nyaurism->slider_value_r );


	[_n_plotter display];

}

- (IBAction)n_slider_r_method:(id)sender
{
//NSLog( @"%d", (int) [sender integerValue] );


	n_nyaurism->slider_value_r = (n_type_real) [sender integerValue] / 100.0;

	_n_slider_label_r.stringValue = [self NonnonNyaurismSliderLabel:n_nyaurism->slider_value_r];


	n_nyaurism_backup( &n_nyaurism->wav_base, &n_nyaurism->wav );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( &n_nyaurism->wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( &n_nyaurism->wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

	n_wav_normalize_partial( &n_nyaurism->wav, x, sx, n_nyaurism->slider_value_l, n_nyaurism->slider_value_r );


	[_n_plotter display];

}




- (IBAction)n_save_button_cancel:(id)sender {

	n_mac_window_all_controls_onoff( [_n_eq_window contentView], YES );

	[_window endSheet:_n_save_window];

}

- (IBAction)n_save_button_go:(id)sender {

	n_mac_window_all_controls_onoff( [_n_eq_window contentView], YES );

	[_window endSheet:_n_save_window];


	// [!] : non-breaking

	n_wav save; n_wav_carboncopy( &n_nyaurism->wav, &save );

	int c = (int) [_n_save_combo_channel indexOfSelectedItem];
	int b = (int) [_n_save_combo_bit     indexOfSelectedItem];
	int r = (int) [_n_save_combo_rate    indexOfSelectedItem];

	save.channel = 1 + c;

	if ( b == 0 )
	{
		save.format = N_WAV_FORMAT_PCM;
		save.bit = 8;
	} else
	if ( b == 1 )
	{
		save.format = N_WAV_FORMAT_PCM;
		save.bit = 16;
	} else
	if ( b == 2 )
	{
		save.format = N_WAV_FORMAT_FLOAT;
		save.bit = 32;
	}

	if ( r == 0 )
	{
		save.rate = 11;
	} else
	if ( r == 1 )
	{
		save.rate = 22;
	} else
	if ( r == 2 )
	{
		save.rate = 44;
	}
//n_wav_save_literal( &save, "./result.wav" );

	n_posix_char *str = n_mac_nsstring2str( n_nyaurism->fname );

	if ( n_posix_stat_is_exist( str ) )
	{
		//if ( n_mac_window_dialog_yesno( "Overwrite?" ) )
		{
			n_wav_save( &save, str );
		}
	} else {
		if ( n_wav_save( &save, str ) )
		{
//NSLog( @"Save Error" );
			NSString *name = [n_nyaurism->fname lastPathComponent];

			NSSavePanel *panel = [NSSavePanel savePanel];

			[panel setNameFieldStringValue:name];

			NSArray *desktop = [[NSFileManager defaultManager]
				URLsForDirectory:NSDesktopDirectory
				       inDomains:NSUserDomainMask
			];
			if ( desktop.count > 0 ) { [panel setDirectoryURL:desktop[ 0 ]]; }

			[panel beginSheetModalForWindow:_window completionHandler:^(NSInteger result)
			{
				if ( result == NSModalResponseOK )
				{
					NSString     *nsstring = [[panel URL] path];
					n_posix_char *str      = n_mac_nsstring2str( nsstring );

					n_wav_save( &save, str );

					n_string_free( str );
				}
			}];
		}
	}

	n_string_free( str );


	n_wav_free( &save );

}




- (IBAction)n_resizer_button_cancel:(id)sender {

	n_mac_window_all_controls_onoff( [_n_eq_window contentView], YES );

	[_window endSheet:_n_resizer_window];

}

- (IBAction)n_resizer_button_go:(id)sender {

	n_mac_window_all_controls_onoff( [_n_eq_window contentView], YES );

	[_window endSheet:_n_resizer_window];

	n_wav_resizer( &n_nyaurism->wav, _n_resizer_label.doubleValue, N_WAV_RESIZER_NORMAL );

	[_n_plotter display];

}

- (void)controlTextDidChange:(NSNotification *)obj {

	// [!] : Thx : DeepSeek AI

	NSTextField    *textField      = [obj object];
	NSString       *originalString = [textField stringValue];

	if ( [originalString length] == 0 )
	{
		_n_resizer_label.stringValue = [self NonnonNyaurismMsecNSString];
		return;
	}

	NSCharacterSet *nonDigits      = [[NSCharacterSet decimalDigitCharacterSet] invertedSet];
	NSString       *filteredString = [[originalString componentsSeparatedByCharactersInSet:nonDigits] componentsJoinedByString:@""];

	if ( ![originalString isEqualToString:filteredString] )
	{
		[textField setStringValue:filteredString];
	}

}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor {

	// [!] : Thx : DeepSeek AI

	NSString *text = [fieldEditor string];

	if ( [text length] == 0 )
	{
		_n_resizer_label.stringValue = [self NonnonNyaurismMsecNSString];
		return NO;
	}

	NSCharacterSet *nonDigits = [[NSCharacterSet decimalDigitCharacterSet] invertedSet];
	NSRange         range     = [text rangeOfCharacterFromSet:nonDigits];

	if ( range.location != NSNotFound )
	{
		NSBeep();
		return NO;
	}


	return YES;
}




- (IBAction)n_eq_button_undo:(id)sender {

	n_nyaurism_plotter_undo( &n_nyaurism->wav );

	[_n_plotter display];

}

- (void) NonnonNyaurismEqualizerUIOnOff:(BOOL)onoff
{

	[_n_button_zero    n_enable:onoff]; [_n_button_zero    display];
	[_n_button_mix     n_enable:onoff]; [_n_button_mix     display];
	[_n_button_stop    n_enable:onoff]; [_n_button_stop    display];
	[_n_button_play    n_enable:onoff]; [_n_button_play    display];
	[_n_button_resizer n_enable:onoff]; [_n_button_resizer display];
	[_n_button_save    n_enable:onoff]; [_n_button_save    display];
	[_n_button_eq      n_enable:onoff]; [_n_button_eq      display];

	[_n_slider_body_l setEnabled:onoff];
	[_n_slider_body_r setEnabled:onoff];

	CGFloat blend;
	if ( onoff )
	{
		blend = 1.0;
	} else {
		blend = 0.2;
	}

	[_n_slider_label_base_l setAlphaValue:blend];
	[_n_slider_label_base_r setAlphaValue:blend];

	[_n_slider_label_l setAlphaValue:blend];
	[_n_slider_label_r setAlphaValue:blend];


	[_n_eq_button_undo  setEnabled:onoff];
	[_n_eq_button_apply setEnabled:onoff];

}

- (void) n_timer_eq_button_apply
{

	int num_bands = 10;

	double gains_db[] = {

		[_n_eq_0 doubleValue],	// 20-50Hz
		[_n_eq_1 doubleValue],	// 50-125Hz
		[_n_eq_2 doubleValue],	// 125-315Hz
		[_n_eq_3 doubleValue],	// 315-800Hz
		[_n_eq_4 doubleValue],	// 800-2000Hz
		[_n_eq_5 doubleValue],	// 2000-5000Hz
		[_n_eq_6 doubleValue],	// 5000-12500Hz
		[_n_eq_7 doubleValue],	// 12500-20000Hz
		[_n_eq_8 doubleValue],	// high freq
		[_n_eq_9 doubleValue]	// highest freq

	};
//NSLog( @"%f", gains_db[ 0 ] );


	n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_undo );
	n_nyaurism_backup( &n_nyaurism->wav, &n_nyaurism->wav_base );


	n_type_gfx x,sx; n_nyaurism_plotter_selection_pixel2sample( &n_nyaurism->wav, &x, &sx );
	if ( sx == 0 ) { sx = N_WAV_COUNT( &n_nyaurism->wav ); }
	if ( n_nyaurism_plotter_selection_line_only() ) { sx = 0; }

//NSLog( @"%d %d", x, sx );


	if ( ( x == 0 )&&( sx == N_WAV_COUNT( &n_nyaurism->wav ) )&&( n_nyaurism->mix_onoff == FALSE ) )
	{
		n_fft_equalizer_apply( &n_nyaurism->wav, gains_db, num_bands, N_NYAURISM_FFT_RESOLUTION );
	} else {
		n_wav tmp; n_wav_carboncopy( &n_nyaurism->wav, &tmp );

		n_fft_equalizer_apply( &tmp, gains_db, num_bands, N_NYAURISM_FFT_RESOLUTION );


		n_type_real l = 1.0; if ( n_nyaurism->plotter_selection_channel == 1 ) { l = -1; }
		n_type_real r = 1.0; if ( n_nyaurism->plotter_selection_channel == 2 ) { r = -1; }

		if ( n_nyaurism->mix_onoff )
		{
			n_wav_copy( &tmp, &n_nyaurism->wav, x, sx, x, l, r, N_WAV_COPY_ADD );
		} else {
			n_wav_copy( &tmp, &n_nyaurism->wav, x, sx, x, l, r, N_WAV_COPY_SET );
		}


		n_wav_free( &tmp );
	}

	n_nyaurism_slider_redraw( &n_nyaurism->wav );


	[_n_plotter display];


	[self NonnonNyaurismEqualizerUIOnOff:TRUE];

}

- (IBAction)n_eq_button_apply:(NSButton *)sender {

	[self NonnonNyaurismEqualizerUIOnOff:FALSE];

	n_mac_timer_init_once( self, @selector( n_timer_eq_button_apply ), 200 );

}

- (IBAction)n_eq_button_reset:(NSButton *)sender {

	[_n_eq_0 setIntValue:0]; _n_eq_label_0.stringValue = @"0";
	[_n_eq_1 setIntValue:0]; _n_eq_label_1.stringValue = @"0";
	[_n_eq_2 setIntValue:0]; _n_eq_label_2.stringValue = @"0";
	[_n_eq_3 setIntValue:0]; _n_eq_label_3.stringValue = @"0";
	[_n_eq_4 setIntValue:0]; _n_eq_label_4.stringValue = @"0";
	[_n_eq_5 setIntValue:0]; _n_eq_label_5.stringValue = @"0";
	[_n_eq_6 setIntValue:0]; _n_eq_label_6.stringValue = @"0";
	[_n_eq_7 setIntValue:0]; _n_eq_label_7.stringValue = @"0";
	[_n_eq_8 setIntValue:0]; _n_eq_label_8.stringValue = @"0";
	[_n_eq_9 setIntValue:0]; _n_eq_label_9.stringValue = @"0";

}

- (void)n_eq_button_analyze_value_set:(NSSlider*) slider label:(NSTextField*) label value:(int)value
{
	[slider setIntValue:value];
	label.stringValue = [NSString stringWithFormat:@"%d", value];
}

- (IBAction)n_eq_button_analyze:(id)sender {

	int    histogram_count = 10;
	double histogram[ histogram_count ];

	n_fft_histogram_main( &n_nyaurism->wav, histogram, histogram_count, N_NYAURISM_FFT_RESOLUTION, NO );

	for( int i = 0; i < histogram_count; i++ )
	{
		histogram[ i ]  = n_wav_sample_clamp_normalized( histogram[ i ] );
		histogram[ i ] -=  1.0;
		histogram[ i ] *= 10;
	}


	[self n_eq_button_analyze_value_set:_n_eq_0 label:_n_eq_label_0 value:(int) histogram[ 0 ]];
	[self n_eq_button_analyze_value_set:_n_eq_1 label:_n_eq_label_1 value:(int) histogram[ 1 ]];
	[self n_eq_button_analyze_value_set:_n_eq_2 label:_n_eq_label_2 value:(int) histogram[ 2 ]];
	[self n_eq_button_analyze_value_set:_n_eq_3 label:_n_eq_label_3 value:(int) histogram[ 3 ]];
	[self n_eq_button_analyze_value_set:_n_eq_4 label:_n_eq_label_4 value:(int) histogram[ 4 ]];
	[self n_eq_button_analyze_value_set:_n_eq_5 label:_n_eq_label_5 value:(int) histogram[ 5 ]];
	[self n_eq_button_analyze_value_set:_n_eq_6 label:_n_eq_label_6 value:(int) histogram[ 6 ]];
	[self n_eq_button_analyze_value_set:_n_eq_7 label:_n_eq_label_7 value:(int) histogram[ 7 ]];
	[self n_eq_button_analyze_value_set:_n_eq_8 label:_n_eq_label_8 value:(int) histogram[ 8 ]];
	[self n_eq_button_analyze_value_set:_n_eq_9 label:_n_eq_label_9 value:(int) histogram[ 9 ]];

}

- (IBAction)n_eq_slider_shared:(NSSlider *)sender {

	NSTextField *target = nil;

	if ( sender == _n_eq_0 ) { target = _n_eq_label_0; } else
	if ( sender == _n_eq_1 ) { target = _n_eq_label_1; } else
	if ( sender == _n_eq_2 ) { target = _n_eq_label_2; } else
	if ( sender == _n_eq_3 ) { target = _n_eq_label_3; } else
	if ( sender == _n_eq_4 ) { target = _n_eq_label_4; } else
	if ( sender == _n_eq_5 ) { target = _n_eq_label_5; } else
	if ( sender == _n_eq_6 ) { target = _n_eq_label_6; } else
	if ( sender == _n_eq_7 ) { target = _n_eq_label_7; } else
	if ( sender == _n_eq_8 ) { target = _n_eq_label_8; } else
	if ( sender == _n_eq_9 ) { target = _n_eq_label_9; }

	if ( target != nil )
	{
		target.stringValue = [NSString stringWithFormat:@"%d", sender.intValue];
	}

}




- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
	return YES;
}




- (IBAction)n_tools_menu_readme:(id)sender {

	NSString *helpFilePath = [[NSBundle mainBundle] pathForResource:@"nonnon_nyaurism" ofType:@"html"];
	NSURL    *helpFileURL  = [NSURL fileURLWithPath:helpFilePath];

	[[NSWorkspace sharedWorkspace] openURL:helpFileURL];

}


@end
