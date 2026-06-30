// hunyapiyo3
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#include "../../../neutral/posix.c"




#ifdef N_POSIX_PLATFORM_MAC


#include "../../../mac/_mac.c"
#include "../../../mac/image.c"
#include "../../../mac/sound.c"
#include "../../../mac/window.c"


#include "../../../neutral/bmp/ui/pie_throbber.c"
#include "../../../neutral/bmp/ui/timer.c"
#include "../../../neutral/bmp/ui/transition.c"

#include "../../../bridge/gdi.c"


#else


#include "../../../win64/game_engine.c"


#include "../../../neutral/bmp/all.c"

#include "../../../neutral/bmp/ui/pie_throbber.c"
#include "../../../neutral/bmp/ui/transition.c"

#include "../../../bridge/gdi.c"

#include "../../../neutral/wav/all.c"
#include "../../../win64/resource.c"
#include "../../../bridge/sound.c"




#define n_bmp_rgb_mac n_bmp_rgb


#endif




#define N_HUNYAPIYO3_GO n_posix_literal( "Go!" )
#define N_HUNYAPIYO3_OK n_posix_literal( "OK" )


#define N_HUNYAPIYO3_FONT_NAME n_posix_literal( "Trebuchet MS" )


#define N_HUNYAPIYO3_COLOR_BG       n_bmp_rgb_mac(   0, 200, 255 )
#define N_HUNYAPIYO3_COLOR_FRAME    n_bmp_rgb_mac(   0, 150, 200 )
#define N_HUNYAPIYO3_COLOR_FRAME_O  n_bmp_rgb_mac( 255, 255,   0 )
#define N_HUNYAPIYO3_COLOR_FRAME_X  n_bmp_rgb_mac(   1,   1,   1 )
#define N_HUNYAPIYO3_COLOR_MIXER_O  n_bmp_rgb_mac( 255, 255, 255 )
#define N_HUNYAPIYO3_COLOR_MIXER_X  n_bmp_rgb_mac(   1,   1,   1 )
#define N_HUNYAPIYO3_COLOR_TEXT     n_bmp_white
#define N_HUNYAPIYO3_COLOR_OUTLINE  n_bmp_rgb_mac(   0, 150, 200 )
#define N_HUNYAPIYO3_COLOR_SHAFT    n_bmp_rgb_mac(   0, 150, 200 )
#define N_HUNYAPIYO3_COLOR_PROGRESS n_bmp_rgb_mac(   0, 200, 255 )


#define N_HUNYAPIYO3_BMP_ALL ( 5 )
#define N_HUNYAPIYO3_WAV_ALL ( 7 )


#define N_HUNYAPIYO3_SOUND_CLICK       ( 0 )
#define N_HUNYAPIYO3_SOUND_TURN        ( 1 )
#define N_HUNYAPIYO3_SOUND_FANFARE     ( 2 )
#define N_HUNYAPIYO3_SOUND_ANSWER_O    ( 3 )
#define N_HUNYAPIYO3_SOUND_ANSWER_X    ( 4 )
#define N_HUNYAPIYO3_SOUND_COUNTDOWN_1 ( 5 )
#define N_HUNYAPIYO3_SOUND_COUNTDOWN_2 ( 6 )
#define N_HUNYAPIYO3_SOUND_COUNTDOWN_3 ( 7 )


#define N_HUNYAPIYO3_RANDOM  ( 4 )
#define N_HUNYAPIYO3_MAP_SX  ( 3 )
#define N_HUNYAPIYO3_MAP_SY  ( 3 )
#define N_HUNYAPIYO3_MAP_ALL ( N_HUNYAPIYO3_MAP_SX * N_HUNYAPIYO3_MAP_SY )


#define N_HUNYAPIYO3_PHASE_CLCK (  0 )
#define N_HUNYAPIYO3_PHASE_STRT (  1 )
#define N_HUNYAPIYO3_PHASE_CD_3 (  2 )
#define N_HUNYAPIYO3_PHASE_CD_2 (  3 )
#define N_HUNYAPIYO3_PHASE_CD_1 (  4 )
#define N_HUNYAPIYO3_PHASE_CD_0 (  5 )
#define N_HUNYAPIYO3_PHASE_INIT (  6 )
#define N_HUNYAPIYO3_PHASE_MAIN (  7 )
#define N_HUNYAPIYO3_PHASE_RSLT (  8 )
#define N_HUNYAPIYO3_PHASE_PERC (  9 )


#define N_HUNYAPIYO3_SHORTTERM_MSEC ( 1000 )
#define N_HUNYAPIYO3_O_X_SOUND_MSEC (  500 )


#define N_HUNYAPIYO3_FADE_MSEC ( 777 )
#define N_HUNYAPIYO3_ANIM_MSEC ( 333 )




typedef struct {

#ifdef N_POSIX_PLATFORM_MAC

	//NonnonGame   *self;
	NSPoint       pt;
	n_bmp         canvas_base;
	n_bmp         canvas_alias; // [!] : alias of NSBitmapImageRep

	NSBitmapImageRep *rep;

#else

	HWND          hwnd;
	POINT         pt;

	n_game_sound  sound[ 10 ];

#endif

	n_bmp        *canvas;

	n_posix_char  font_server_text[ 100 ];
	int           font_server_number;

	BOOL          refresh;
	BOOL          click;
	BOOL          click_menu;
	BOOL          sound_lock_onoff;
	u32           sound_lock;

	n_type_gfx    zoom;
	n_type_gfx    sx, sy;
	n_type_gfx    bar;
	n_type_gfx    unit;
	n_type_gfx    button_press_offset_default;
	n_type_gfx    button_press_offset;
	n_type_gfx    message_font_size;
	n_type_gfx    question_size;

	n_bmp         bmp[ N_HUNYAPIYO3_BMP_ALL ];
	n_bmp         bmp_bg;
	n_bmp         bmp_result;
	n_bmp         bmp_question;
	n_bmp         bmp_font;
	n_bmp         bmp_button;

	int           map[ N_HUNYAPIYO3_MAP_ALL ];
	int           ret[ N_HUNYAPIYO3_MAP_ALL ];
	BOOL          answer_check_onoff;

	int           phase;
	u32           phase_timer;

	u32           result_timer;
	int           result_index;

	int           score;
	BOOL          is_fanfare;

	n_type_real   text_jump_coeff;
	u32           text_jump_timer;

	u32           progress;

	u32           color_bg;
	u32           color_gradient;
	u32           color_frame;
	u32           color_text;
	u32           color_outline;
	u32           color_shaft;
	u32           color_progress;
	u32           color_button_bg;
	u32           color_button_fg;

	BOOL          transition_onoff;
	n_bmp         transition_bmp_old;
	n_bmp         transition_bmp_new;
	n_type_real   transition_percent;
	u32           transition_color_old;

	BOOL          animation_onoff;
	n_bmp         animation_bmp_tmp;
	n_bmp         animation_bmp_old;
	n_bmp         animation_bmp_new;
	n_type_gfx    animation_x;
	n_type_gfx    animation_y;
	n_type_real   animation_percent;
	int           animation_type;

	n_type_real   countdown_text_animation_step;
	n_type_real   countdown_text_animation_size;
	n_type_gfx    countdown_text_animation_min;
	n_type_gfx    countdown_text_animation_max;
	//u32           countdown_text_animation_timer;

	n_type_gfx    redraw_px;
	n_type_gfx    redraw_py;
	n_type_gfx    redraw_psx;
	n_type_gfx    redraw_psy;

	n_type_real   throbber_degree;
	u32           throbber_timer;

} n_hunyapiyo3;

static n_hunyapiyo3 hunyapiyo3;




n_type_gfx
n_game_centering( n_type_gfx a, n_type_gfx b )
{

	// [!] : ( ( a / 2 ) - ( b / 2 ) ) == ( ( a - b ) / 2 )

	return ( a - b ) / 2;
}



BOOL
n_hunyapiyo3_is_darkmode( void )
{
#ifdef N_POSIX_PLATFORM_MAC

	return n_mac_is_darkmode();

#else

	return n_win64_darkmode_is_on();

#endif
}




void
n_hunyapiyo3_gdi_effect( n_hunyapiyo3 *p, n_posix_char *str, n_type_gfx fontsize, BOOL is_countdown )
{

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.base_color_bg   = n_bmp_alpha_replace_pixel( p->color_bg, 0 );
	gdi.base_color_fg   = gdi.base_color_bg;
	gdi.base_style      = N_GDI_BASE_SOLID;

	gdi.frame_style     = N_GDI_FRAME_DEFAULT;

	gdi.text            = str;
	gdi.text_font       = N_HUNYAPIYO3_FONT_NAME;
	gdi.text_size       = fontsize;
	gdi.text_style      = N_GDI_TEXT_SMOOTH | N_GDI_TEXT_MAC_NO_CROP;
	gdi.text_color_main = n_bmp_rgb( 255,255,255 );

	int n = 0;

	gdi.text_effect_style[ n ] = N_GDI_EFFECT_OUTLINE;
	//gdi.text_effect_style[ n ] = N_GDI_EFFECT_SINK;
	gdi.text_effect_color[ n ] = p->color_bg;
	gdi.text_effect_param[ n ] = 2;
/*
	n++;
	gdi.text_effect_style[ n ] = N_GDI_EFFECT_OUTLINE;
	gdi.text_effect_color[ n ] = n_bmp_white;
	gdi.text_effect_param[ n ] = 4;

	n++;
	gdi.text_effect_style[ n ] = N_GDI_EFFECT_OUTLINE;
	gdi.text_effect_color[ n ] = n_bmp_rgb( 111,111,111 );
	gdi.text_effect_param[ n ] = 1;

	n++;
	gdi.text_effect_style[ n ] = N_GDI_EFFECT_SHADOW_FOG;
	gdi.text_effect_color[ n ] = n_bmp_rgb( 111,111,111 );
	gdi.text_effect_param[ n ] = 3;
*/

#ifndef N_POSIX_PLATFORM_MAC

	if ( is_countdown ) { gdi.effect_no_bicubic = TRUE; }

#endif

	n_gdi_bmp( &gdi, &p->bmp_font );

//n_bmp_save( &p->bmp_font, "/Users/nonnon2/Desktop/ret.bmp" );


	return;
}




void
n_hunyapiyo3_gdi_button( n_hunyapiyo3 *p, n_bmp *bmp, n_posix_char *str )
{

	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.sx                  = p->sx;
	gdi.sy                  = p->bar;

	gdi.frame_style         = N_GDI_FRAME_NOFRAME;

	gdi.base_style          = N_GDI_BASE_SOLID;

#ifdef N_POSIX_PLATFORM_MAC

	gdi.base_color_bg       = n_bmp_white_invisible;//p->color_button_bg;
	gdi.base_color_fg       = n_bmp_white_invisible;//p->color_button_bg;

#else

	gdi.base_color_bg       = p->color_button_bg;
	gdi.base_color_fg       = p->color_button_bg;

#endif

	gdi.text                = str;
	gdi.text_font           = N_HUNYAPIYO3_FONT_NAME;
	gdi.text_size           = p->bar / 2;
	gdi.text_style          = N_GDI_TEXT_BOLD | N_GDI_TEXT_SMOOTH;
	gdi.text_color_main     = p->color_button_fg;

	n_gdi_bmp( &gdi, &p->bmp_button );

//n_bmp_save( &p->bmp_font, "/Users/nonnon2/Desktop/ret.bmp" );


	return;
}




void
n_hunyapiyo3_font_server( n_hunyapiyo3 *p )
{

	if ( p->phase == N_HUNYAPIYO3_PHASE_CLCK )
	{

//NSMutableDictionary *attr = [NSMutableDictionary dictionary];
//NSString            *text = @"test";
//[text drawAtPoint:NSMakePoint( 0,0 ) withAttributes:attr];

		const int fontsize = p->unit / 2;


		if ( n_bmp_ui_timer( &p->text_jump_timer, 25 ) ) { p->text_jump_coeff += 0.01; }

		double     d = fabs( sin( 2.0 * M_PI * p->text_jump_coeff ) );
		n_type_gfx y = ( fontsize / 2 ) * d; y += ( fontsize / 4 );


		if ( NULL == N_BMP_PTR( &p->bmp_font ) )
		{
			n_hunyapiyo3_gdi_effect( p, p->font_server_text, fontsize, FALSE );
		}


		n_type_gfx sx = N_BMP_SX( &p->bmp_font );
		n_type_gfx sy = N_BMP_SY( &p->bmp_font );
		n_type_gfx tx = n_game_centering( p->sx, sx );
		n_type_gfx ty = n_game_centering( p->sy, sy ) - y + ( sy / 3 );
		n_type_gfx px = p->redraw_px;
		n_type_gfx py = p->redraw_py;

		n_bmp_fastcopy( &p->bmp_bg, p->canvas, px,py,sx,sy, px,py );

		n_bmp_transcopy( &p->bmp_font, p->canvas, 0,0,sx,sy, tx,ty );

		p->redraw_px = tx;
		p->redraw_py = ty;

	} else
	if (
		( p->phase == N_HUNYAPIYO3_PHASE_CD_3 )
		||
		( p->phase == N_HUNYAPIYO3_PHASE_CD_2 )
		||
		( p->phase == N_HUNYAPIYO3_PHASE_CD_1 )
	)
	{

		n_posix_char str[ 1 + 1 ];
		n_posix_snprintf_literal( str, 1 + 1, "%d", p->font_server_number );


		//if ( n_bmp_ui_timer( &p->countdown_text_animation_timer, 1 ) )
		p->countdown_text_animation_size += p->countdown_text_animation_step;
		if ( p->countdown_text_animation_size > p->countdown_text_animation_max )
		{
			p->countdown_text_animation_size = p->countdown_text_animation_max;
		}

		n_hunyapiyo3_gdi_effect( p, str, p->countdown_text_animation_size, TRUE );
//NSLog( @"%d", p->countdown_text_animation_size );


		n_type_gfx px  = p->redraw_px;
		n_type_gfx py  = p->redraw_py;
		n_type_gfx psx = p->redraw_psx;
		n_type_gfx psy = p->redraw_psy;

		n_bmp_fastcopy( &p->bmp_bg, p->canvas, px,py,psx,psy, px,py );


		n_type_gfx sx = N_BMP_SX( &p->bmp_font );
		n_type_gfx sy = N_BMP_SY( &p->bmp_font );
		n_type_gfx tx = n_game_centering( p->sx, sx );
		n_type_gfx ty = n_game_centering( p->sy, sy );


		n_type_real tick = n_posix_tickcount() - p->throbber_timer;
//NSLog( @"%0.2f : %0.2f", tick, (n_type_real) ( tick / 3000.0 ) );

		p->throbber_degree = ( tick / 3000.0 ) * 360.0;
//NSLog( @"%0.2f", p->throbber_degree );
		if ( p->throbber_degree > 360 ) { p->throbber_degree = 360; }


		u32 bg; if ( n_hunyapiyo3_is_darkmode() ) { bg = n_bmp_black; } else { bg = n_bmp_white; }
		u32 fg = p->color_progress;

		n_type_gfx pie = p->sx / 3 * 2;
//NSLog( @"%d : %d", p->sx, pie );

		n_type_gfx cx = p->sx / 2 - pie / 2;
		n_type_gfx cy = p->sy / 2 - pie / 2;

		n_bmp_ui_pie_throbber( p->canvas, cx, cy, pie, 5, bg, fg, p->throbber_degree );


		n_bmp_transcopy( &p->bmp_font, p->canvas, 0,0,sx,sy, tx,ty );


		p->redraw_px  = cx;
		p->redraw_py  = cy;
		p->redraw_psx = pie;
		p->redraw_psy = pie;

	} else
	if (
		( p->phase == N_HUNYAPIYO3_PHASE_MAIN )
		||
		( p->phase == N_HUNYAPIYO3_PHASE_RSLT )
	)
	{

		if ( NULL == N_BMP_PTR( &p->bmp_button ) )
		{
			n_hunyapiyo3_gdi_button( p, &p->bmp_button, p->font_server_text );
		}


		n_type_gfx sx = N_BMP_SX( &p->bmp_button );
		n_type_gfx sy = N_BMP_SY( &p->bmp_button );
		n_type_gfx tx = n_game_centering( p->sx, sx );
		n_type_gfx ty = p->sy;

		n_bmp_box( p->canvas, 0,ty, sx,sy, p->color_button_bg );
		n_bmp_transcopy( &p->bmp_button, p->canvas, 0,0,sx,sy, tx,ty + p->button_press_offset );

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_PERC )
	{

		const int fontsize = p->unit / 2;


		if ( n_bmp_ui_timer( &p->text_jump_timer, 25 ) ) { p->text_jump_coeff += 0.01; }

		double     d = fabs( sin( 2.0 * M_PI * p->text_jump_coeff ) );
		n_type_gfx y = ( fontsize / 2 ) * d; y += ( fontsize / 4 );


		// [!] : Percent Jumping Text

		if ( NULL == N_BMP_PTR( &p->bmp_font ) )
		{
			n_hunyapiyo3_gdi_effect( p, p->font_server_text, fontsize, FALSE );
		}

		n_type_gfx sx = N_BMP_SX( &p->bmp_font );
		n_type_gfx sy = N_BMP_SY( &p->bmp_font );
		n_type_gfx tx = n_game_centering( p->sx, sx );
		n_type_gfx ty = n_game_centering( p->sy, sy ) - y + ( sy / 3 );

		n_bmp_transcopy( &p->bmp_font, p->canvas, 0,0,sx,sy, tx,ty );

		p->redraw_px = tx;
		p->redraw_py = ty;


		// [!] : Button

		{

			if ( NULL == N_BMP_PTR( &p->bmp_button ) )
			{
				n_hunyapiyo3_gdi_button( p, &p->bmp_button, N_HUNYAPIYO3_OK );
			}

			n_type_gfx sx = N_BMP_SX( &p->bmp_button );
			n_type_gfx sy = N_BMP_SY( &p->bmp_button );
			n_type_gfx tx = n_game_centering( p->sx, sx );
			n_type_gfx ty = p->sy;

			n_bmp_box( p->canvas, 0,ty, sx,sy, p->color_button_bg );
			n_bmp_transcopy( &p->bmp_button, p->canvas, 0,0,sx,sy, tx,ty + p->button_press_offset );

		}

	}


	return;
}

void
n_hunyapiyo3_text_jump( n_hunyapiyo3 *p, n_posix_char *str, n_bmp *bmp )
{
//return;

	n_posix_snprintf_literal( p->font_server_text, 100, "%s", str );

	n_hunyapiyo3_font_server( p );

	p->refresh = TRUE;
	return;
}

void
n_hunyapiyo3_countdown( n_hunyapiyo3 *p, int number )
{

	p->font_server_number = number;

	n_hunyapiyo3_font_server( p );

	p->refresh = TRUE;
	return;
}

BOOL
n_hunyapiyo3_button( n_hunyapiyo3 *p, n_posix_char *str, n_bmp *bmp_target )
{
//NSLog( @"%f %f %d %d", pt.x, pt.y, N_BMP_SY( p->canvas ), p->sy );

	BOOL ret = FALSE;


//NSLog( @"%f %f", p->pt.x, p->pt.y );
	if ( ( p->pt.x == 0 )&&( p->pt.y == 0 ) )
	{
		// [!] : out-of-bound
#ifdef N_POSIX_PLATFORM_MAC
		p->pt = NSMakePoint( -1, -1 );
#else
		p->pt.x = -1;
		p->pt.y = -1;
#endif
	}

	if ( ( p->pt.x == -1 )||( p->pt.y == -1 ) )
	{
		//
	} else
	if (
		( ( p->pt.x >=     0 )&&( p->pt.x <   p->sx            ) )
		&&
		( ( p->pt.y >= p->sy )&&( p->pt.y < ( p->sy + p->bar ) ) )
	)
	{
		if ( p->click )
		{
			ret = TRUE;
		}
	} else {
		p->button_press_offset = 0;
	}


	n_posix_snprintf_literal( p->font_server_text, 100, "%s", str );


	return ret;
}




void
n_hunyapiyo3_metrics( n_hunyapiyo3 *p )
{


#ifdef N_POSIX_PLATFORM_MAC
	CGFloat desktop_sx, desktop_sy;
	n_mac_desktop_size( &desktop_sx, &desktop_sy );
#else
	n_type_gfx desktop_sx, desktop_sy;
	n_win64_desktop_size( p->hwnd, &desktop_sx, &desktop_sy );
#endif

	n_type_gfx desktop_size = n_posix_min_n_type_gfx( desktop_sx, desktop_sy );


	p->unit = (n_type_real) desktop_size * 0.125;
	p->sx   = p->unit * N_HUNYAPIYO3_MAP_SX;
	p->sy   = p->unit * N_HUNYAPIYO3_MAP_SY;

	p->bar = p->unit / 5;

	p->button_press_offset_default = 2;

	p->message_font_size = p->sx /  6;

	p->countdown_text_animation_min  = p->sx / 8;
	p->countdown_text_animation_max  = p->sx / 2;
	p->countdown_text_animation_step = ( p->countdown_text_animation_max - p->countdown_text_animation_min ) / 20;

//n_log( " %d ", p->unit );


#ifdef N_POSIX_PLATFORM_MAC

	NSColor *nscolor_bg;
	NSColor *nscolor_button_bg;
	NSColor *nscolor_button_fg;
	if ( n_hunyapiyo3_is_darkmode() )
	{
		// [x] : hard-code only : new values are not set when switched
		nscolor_bg        = n_mac_nscolor_argb( 255,  1,  1,  1 );
		nscolor_button_bg = n_mac_nscolor_argb( 255, 66, 66, 66 );
		nscolor_button_fg = n_mac_nscolor_argb( 255,222,222,222 );
	} else {
		nscolor_bg        = [NSColor controlBackgroundColor];
		nscolor_button_bg = [NSColor  windowBackgroundColor];
		nscolor_button_fg = [NSColor       controlTextColor];
	}

	p->color_bg       = n_mac_nscolor2argb( [NSColor controlAccentColor] );
	p->color_frame    = n_bmp_blend_pixel( p->color_bg, n_bmp_black, 0.25 );
	p->color_text     = n_bmp_white;
	p->color_gradient = n_mac_nscolor2argb( nscolor_bg );

	p->color_bg       = n_bmp_color_mac( p->color_bg       );
	p->color_frame    = n_bmp_color_mac( p->color_frame    );
	p->color_text     = n_bmp_color_mac( p->color_text     );
	p->color_gradient = n_bmp_color_mac( p->color_gradient );

	p->color_outline  = p->color_frame;
	p->color_shaft    = p->color_frame;
	p->color_progress = p->color_bg;

	p->color_button_bg = n_mac_nscolor2argb( nscolor_button_bg );
	p->color_button_fg = n_mac_nscolor2argb( nscolor_button_fg );

#else

	BOOL darkmode = n_hunyapiyo3_is_darkmode();

	p->color_bg    = n_win64_GetSysColor_nbmp( COLOR_ACTIVECAPTION );
	p->color_frame = n_bmp_blend_pixel( p->color_bg, n_bmp_black, 0.25 );
	p->color_text  = n_bmp_white;

	p->color_button_bg = n_win64_GetSysColor_nbmp( COLOR_BTNFACE );
	p->color_button_fg = n_win64_GetSysColor_nbmp( COLOR_BTNTEXT );

	if ( darkmode )
	{
		p->color_gradient = n_bmp_black;
	} else {
		p->color_gradient = n_bmp_white;
	}

	p->color_outline  = p->color_bg;
	p->color_shaft    = p->color_frame;
	p->color_progress = p->color_bg;

#endif


	return;
}




#ifdef N_POSIX_PLATFORM_MAC

// [!] : this is important to play
AVAudioPlayer *n_hunyapiyo3_snd;

void
n_hunyapiyo3_sound_play( n_hunyapiyo3 *p, int id )
{
//return;

	if ( id == N_HUNYAPIYO3_SOUND_CLICK )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"click", @"wav" );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_TURN )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"sh", @"wav" );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_FANFARE )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"get", @"wav" );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_ANSWER_O )
	{
		n_hunyapiyo3_snd    = n_mac_sound_init( @"answer_o", @"wav" );
		p->sound_lock_onoff = TRUE;
		p->sound_lock       = n_posix_tickcount();
	} else
	if ( id == N_HUNYAPIYO3_SOUND_ANSWER_X )
	{
		n_hunyapiyo3_snd    = n_mac_sound_init( @"answer_x", @"wav" );
		p->sound_lock_onoff = TRUE;
		p->sound_lock       = n_posix_tickcount();
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_1 )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"countdown_1", @"wav" );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_2 )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"countdown_2", @"wav" );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_3 )
	{
		n_hunyapiyo3_snd = n_mac_sound_init( @"countdown_3", @"wav" );
	}

	//n_hunyapiyo3_snd.delegate = p->self;
	n_mac_sound_play( n_hunyapiyo3_snd );


	return;
}


#else  // #ifdef N_POSIX_PLATFORM_MAC

void
n_hunyapiyo3_sound_init( n_hunyapiyo3 *p, HWND hwnd )
{

	n_game_sound_init_literal( &p->sound[ 0 ], hwnd, "N_HUNYAPIYO3_WAV_0" );
	n_game_sound_init_literal( &p->sound[ 1 ], hwnd, "N_HUNYAPIYO3_WAV_1" );
	n_game_sound_init_literal( &p->sound[ 2 ], hwnd, "N_HUNYAPIYO3_WAV_2" );
	n_game_sound_init_literal( &p->sound[ 3 ], hwnd, "N_HUNYAPIYO3_WAV_3" );
	n_game_sound_init_literal( &p->sound[ 4 ], hwnd, "N_HUNYAPIYO3_WAV_4" );
	n_game_sound_init_literal( &p->sound[ 5 ], hwnd, "N_HUNYAPIYO3_WAV_5" );
	n_game_sound_init_literal( &p->sound[ 6 ], hwnd, "N_HUNYAPIYO3_WAV_6" );
	n_game_sound_init_literal( &p->sound[ 7 ], hwnd, "N_HUNYAPIYO3_WAV_7" );


	return;
}

void
n_hunyapiyo3_sound_exit( n_hunyapiyo3 *p )
{

	n_game_sound_exit( &p->sound[ 0 ] );
	n_game_sound_exit( &p->sound[ 1 ] );
	n_game_sound_exit( &p->sound[ 2 ] );
	n_game_sound_exit( &p->sound[ 3 ] );
	n_game_sound_exit( &p->sound[ 4 ] );
	n_game_sound_exit( &p->sound[ 5 ] );
	n_game_sound_exit( &p->sound[ 6 ] );
	n_game_sound_exit( &p->sound[ 7 ] );

	return;
}

void
n_hunyapiyo3_sound_play( n_hunyapiyo3 *p, int id )
{
//return;

	if ( id == N_HUNYAPIYO3_SOUND_CLICK )
	{
		n_game_sound_loop( &p->sound[ 0 ] );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_TURN )
	{
		n_game_sound_loop( &p->sound[ 1 ] );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_FANFARE )
	{
		n_game_sound_loop( &p->sound[ 2 ] );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_ANSWER_O )
	{
		n_game_sound_loop( &p->sound[ 3 ] );
		p->sound_lock_onoff = TRUE;
		p->sound_lock       = n_posix_tickcount();
	} else
	if ( id == N_HUNYAPIYO3_SOUND_ANSWER_X )
	{
		n_game_sound_loop( &p->sound[ 4 ] );
		p->sound_lock_onoff = TRUE;
		p->sound_lock       = n_posix_tickcount();
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_1 )
	{
		n_game_sound_loop( &p->sound[ 5 ] );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_2 )
	{
		n_game_sound_loop( &p->sound[ 6 ] );
	} else
	if ( id == N_HUNYAPIYO3_SOUND_COUNTDOWN_3 )
	{
		n_game_sound_loop( &p->sound[ 7 ] );
	}


	return;
}

#endif // #ifdef N_POSIX_PLATFORM_MAC




void
n_hunyapiyo3_gdi_background( n_hunyapiyo3 *p, n_bmp *bmp )
{

	n_bmp_new( bmp, p->sx, p->sy + p->bar );

//n_bmp_flush( bmp, n_bmp_rgb( 0,200,255 ) ); return;


	u32 bg = p->color_gradient;
	u32 fg = p->color_bg;

	n_bmp_flush_gradient( bmp, bg, fg, N_BMP_GRADIENT_VERTICAL );
//n_bmp_save_literal( bmp, "ret.bmp" );


	return;
}




void
n_hunyapiyo3_frame( n_hunyapiyo3 *p, n_bmp *target, n_type_gfx x, n_type_gfx y, u32 color_frame_outer, u32 color_frame_inner )
{

	n_bmp b; n_bmp_zero( &b ); n_bmp_1st( &b, p->unit, p->unit );

	n_type_gfx u  = 1;
	n_type_gfx uu = u * 2;

	n_bmp_box( &b, 0  ,0  ,p->unit   ,p->unit   , color_frame_outer     ); u *= 2; uu = u * 2;
	n_bmp_box( &b, 0+u,0+u,p->unit-uu,p->unit-uu, color_frame_inner     ); u *= 2; uu = u * 2;
	n_bmp_box( &b, 0+u,0+u,p->unit-uu,p->unit-uu, n_bmp_white_invisible );

	n_bmp_transcopy( &b, target, 0,0,p->unit,p->unit, x, y );

	n_bmp_free( &b );


	return;
}

void
n_hunyapiyo3_draw_single( n_hunyapiyo3 *p, n_bmp *target, int index, n_type_gfx x, n_type_gfx y )
{

	if ( index != -1 )
	{
		n_bmp_transcopy( &p->bmp[ index ], target, 0,0,p->unit,p->unit, x, y );
	} else {
		n_bmp_transcopy( &p->bmp_question, target, 0,0,p->unit,p->unit, x, y );
	}


	return;
}

void
n_hunyapiyo3_reset_panel( n_hunyapiyo3 *p )
{
//return;

	// [!] : use 4 panels always


	const BOOL shuffle = TRUE;


	int neko[ N_HUNYAPIYO3_RANDOM ];

	int i = 0;
	n_posix_loop
	{

		neko[ i ] = i;

		i++;
		if ( i >= N_HUNYAPIYO3_RANDOM ) { break; }
	}

	i = 0;
	while( shuffle )
	{

		//if ( 0 == n_random_range( 2 ) )
		{
			int r = n_random_range( N_HUNYAPIYO3_RANDOM );

			int swap  = neko[ i ];
			neko[ i ] = neko[ r ];
			neko[ r ] = swap;
		}

		i++;
		if ( i >= N_HUNYAPIYO3_RANDOM ) { break; }
	}


	int rest = N_HUNYAPIYO3_MAP_ALL;

	int panel_count[ N_HUNYAPIYO3_RANDOM ];

	i = 0;
	n_posix_loop
	{

		panel_count[ i ] = 1 + n_random_range( rest - ( N_HUNYAPIYO3_RANDOM - i ) );

		rest -= panel_count[ i ];

		i++;
		if ( i >= N_HUNYAPIYO3_RANDOM ) { break; }

	}

	if ( rest != 0 ) { panel_count[ i - 1 ] += rest; }


	    i = 0;
	int j = 0;
	int k = 0;
	n_posix_loop
	{

		p->map[ i ] = neko[ k ];

		i++;
		if ( i >= N_HUNYAPIYO3_MAP_ALL ) { break; }

		j++;
		if ( j >= panel_count[ k ] )
		{
			j = 0;
			k++;
			if ( k >= N_HUNYAPIYO3_RANDOM )
			{
//n_posix_debug_literal( "Never Come" );
				k--;
			}
		}

	}

	i = 0;
	while( shuffle )
	{


		{
			int r = n_random_range( N_HUNYAPIYO3_MAP_ALL );

			int swap    = p->map[ i ];
			p->map[ i ] = p->map[ r ];
			p->map[ r ] = swap;
		}

		i++;
		if ( i >= N_HUNYAPIYO3_MAP_ALL ) { break; }
	}


	return;
}

void
n_hunyapiyo3_reset( n_hunyapiyo3 *p )
{

	p->score = 0;

	p->answer_check_onoff = FALSE;

	n_hunyapiyo3_reset_panel( p );

	int i = 0;
	n_posix_loop
	{

		p->ret[ i ] = -1;

		i++;
		if ( i >= N_HUNYAPIYO3_MAP_ALL ) { break; }
	}

	p->phase = N_HUNYAPIYO3_PHASE_CLCK;

	n_bmp_free( &p->bmp_font );
	n_bmp_free( &p->bmp_button );

	//n_bmp_flush( &game.bmp, p->color_bg );
	n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );


	return;
}

void
n_hunyapiyo3_short_term_memory( n_hunyapiyo3 *p )
{

	//n_bmp_flush( &game.bmp, p->color_bg );
	n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );


	int x = 0;
	int y = 0;
	n_posix_loop
	{

		n_type_gfx pxl_x = p->unit * x;
		n_type_gfx pxl_y = p->unit * y;

		int map = p->map[ x + ( N_HUNYAPIYO3_MAP_SX * y ) ];

		n_hunyapiyo3_draw_single( p, p->canvas, map, pxl_x, pxl_y );

		x++;
		if ( x >= N_HUNYAPIYO3_MAP_SX )
		{
			x = 0;
			y++;
			if ( y >= N_HUNYAPIYO3_MAP_SY ) { break; }
		}
	}


	p->refresh = TRUE;
	return;
}

void
n_hunyapiyo3_progressbar( n_hunyapiyo3 *p, n_type_real percent_coeff )
{

	n_bmp_box( p->canvas, 0,p->sy,p->sx,p->bar, n_bmp_black );

	n_type_gfx bar = ( p->bar / 6 );
	n_type_gfx   x = p->unit / 10;
	n_type_gfx   y = p->sy + n_game_centering( p->bar, bar );
	n_type_gfx  sx = p->sx - ( x * 2 );
	n_type_gfx  sy = bar;

	n_bmp_box( p->canvas, x,y,sx,sy, p->color_shaft );

	sx = (n_type_gfx) ( (n_type_real) sx * percent_coeff );


	n_bmp_box( p->canvas, x,y,sx,sy, p->color_progress );


	p->refresh = TRUE;
	return;
}

void
n_hunyapiyo3_result( n_hunyapiyo3 *p )
{

	n_type_gfx pxl_x = p->unit * ( p->result_index % N_HUNYAPIYO3_MAP_SX );
	n_type_gfx pxl_y = p->unit * ( p->result_index / N_HUNYAPIYO3_MAP_SX );

	if ( p->map[ p->result_index ] == p->ret[ p->result_index ] )
	{
		p->score++;
		n_bmp_mixer( p->canvas, pxl_x,pxl_y,p->unit,p->unit, N_HUNYAPIYO3_COLOR_MIXER_O, 0.25 );
		n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_ANSWER_O );
		n_hunyapiyo3_frame( p, p->canvas, pxl_x,pxl_y, p->color_frame, N_HUNYAPIYO3_COLOR_FRAME_O );
	} else {
		n_bmp_mixer( p->canvas, pxl_x,pxl_y,p->unit,p->unit, N_HUNYAPIYO3_COLOR_MIXER_X, 0.25 );
		n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_ANSWER_X );
		n_hunyapiyo3_frame( p, p->canvas, pxl_x,pxl_y, p->color_frame, N_HUNYAPIYO3_COLOR_FRAME_X );
	}

	p->refresh = TRUE;
	

	p->result_index++;
	if ( p->result_index >= N_HUNYAPIYO3_MAP_ALL )
	{
		n_bmp_flush_fastcopy( p->canvas, &p->bmp_result );

		p->pt.x = p->pt.y = -1;

		if ( p->score == N_HUNYAPIYO3_MAP_ALL ) { p->is_fanfare = TRUE; }

		n_string_truncate( p->font_server_text );

		n_bmp_free( &p->bmp_font );
		n_bmp_free( &p->bmp_button );

		p->phase = N_HUNYAPIYO3_PHASE_PERC;
	}

//n_posix_debug_literal( "%d/%d", p->score, N_HUNYAPIYO3_MAP_ALL );


	return;
}

void
n_hunyapiyo3_main( n_hunyapiyo3 *p )
{
//NSLog( @"!" );

	if ( p->animation_onoff ) { return; }


	//n_bmp_flush( p->canvas, n_bmp_white ); p->refresh = TRUE; return;
	n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );


	BOOL ret = n_hunyapiyo3_button( p, N_HUNYAPIYO3_GO, p->canvas );
	if ( ret )
	{
		p->phase = N_HUNYAPIYO3_PHASE_RSLT;

		p->button_press_offset = 0;

		p->result_timer = n_posix_tickcount();
		p->result_index = 0;

		p->text_jump_coeff = 0.0;

		n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_CLICK );
	}


	int x = 0;
	int y = 0;
	n_posix_loop
	{//break;

		n_type_gfx pxl_x = p->unit * x;
		n_type_gfx pxl_y = p->unit * y;

		int ret_pos = x + ( N_HUNYAPIYO3_MAP_SX * y );

		u32 color_frame = n_bmp_white_invisible;
		if (
			( ( p->pt.x >= pxl_x )&&( p->pt.x < ( pxl_x + p->unit ) ) )
			&&
			( ( p->pt.y >= pxl_y )&&( p->pt.y < ( pxl_y + p->unit ) ) )
		)
		{

			int anim_f = -1;
			int anim_t = -1;

			p->animation_type = N_BMP_UI_TRANSITION_NOTHING;

			color_frame = N_HUNYAPIYO3_COLOR_FRAME_O;
			if ( p->click )
			{
				p->animation_type = N_BMP_UI_TRANSITION_SCROLL_D;

				anim_f = p->ret[ ret_pos ];

				p->ret[ ret_pos ]++;
				if ( p->ret[ ret_pos ] >= N_HUNYAPIYO3_RANDOM )
				{
					p->ret[ ret_pos ] = -1;
				}

				anim_t = p->ret[ ret_pos ];
			} else
			if ( p->click_menu )
			{
				p->animation_type = N_BMP_UI_TRANSITION_SCROLL_U;

				anim_f = p->ret[ ret_pos ];

				p->ret[ ret_pos ]--;
				if ( p->ret[ ret_pos ] < -1 )
				{
					p->ret[ ret_pos ] = N_HUNYAPIYO3_RANDOM - 1;
				}

				anim_t = p->ret[ ret_pos ];
			}

			if ( p->animation_type != N_BMP_UI_TRANSITION_NOTHING )
			{
				p->animation_onoff = TRUE;

				p->animation_x = pxl_x;
				p->animation_y = pxl_y;

				p->animation_percent = 0;

				n_bmp *bmp_f;
				n_bmp *bmp_t;

				if ( anim_f != -1 )
				{
					bmp_f = &p->bmp[ anim_f ];
				} else {
					bmp_f = &p->bmp_question ;
				}

				if ( anim_t != -1 )
				{
					bmp_t = &p->bmp[ anim_t ];
				} else {
					bmp_t = &p->bmp_question ;
				}

				n_bmp_new_fast( &p->animation_bmp_tmp, p->unit, p->unit );

				n_bmp_carboncopy( bmp_f, &p->animation_bmp_old );
				n_bmp_carboncopy( bmp_t, &p->animation_bmp_new );

				n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_TURN );

			}
		}

		n_hunyapiyo3_draw_single( p, p->canvas, p->ret[ ret_pos ], pxl_x, pxl_y );

		n_hunyapiyo3_frame( p, p->canvas, pxl_x, pxl_y, p->color_frame, color_frame );


		x++;
		if ( x >= N_HUNYAPIYO3_MAP_SX )
		{
			x = 0;

			y++;
			if ( y >= N_HUNYAPIYO3_MAP_SY ) { break; }
		}
	}


	n_hunyapiyo3_font_server( p );


	p->refresh = TRUE;
	return;
}

void
n_hunyapiyo3_last( n_hunyapiyo3 *p )
{

	n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );


	BOOL ret = n_hunyapiyo3_button( p, N_HUNYAPIYO3_OK, p->canvas );
	if ( ret )
	{
		p->button_press_offset = 0;

		n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_CLICK );

		n_hunyapiyo3_reset( p );

		p->refresh = TRUE;
		return;
	}


	int x = 0;
	int y = 0;
	n_posix_loop
	{//break;

		n_type_gfx pxl_x = p->unit * x;
		n_type_gfx pxl_y = p->unit * y;

		int pos = x + ( N_HUNYAPIYO3_MAP_SX * y );

		u32 color_frame = n_bmp_white_invisible;

		int index;
		if ( p->answer_check_onoff )
		{
			index = p->map[ pos ];
			n_hunyapiyo3_draw_single( p, p->canvas, index, pxl_x, pxl_y );
		} else {
			index = p->ret[ pos ];
			if ( p->ret[ pos ] == p->map[ pos ] )
			{
				color_frame = N_HUNYAPIYO3_COLOR_FRAME_O;
				n_hunyapiyo3_draw_single( p, p->canvas, index, pxl_x, pxl_y );
				n_bmp_mixer( p->canvas, pxl_x,pxl_y,p->unit,p->unit, N_HUNYAPIYO3_COLOR_MIXER_O, 0.25 );
			} else {
				color_frame = N_HUNYAPIYO3_COLOR_FRAME_X;
				n_hunyapiyo3_draw_single( p, p->canvas, index, pxl_x, pxl_y );
				n_bmp_mixer( p->canvas, pxl_x,pxl_y,p->unit,p->unit, N_HUNYAPIYO3_COLOR_MIXER_X, 0.25 );
			}
		}

		n_hunyapiyo3_frame( p, p->canvas, pxl_x, pxl_y, p->color_frame, color_frame );


		x++;
		if ( x >= N_HUNYAPIYO3_MAP_SX )
		{
			x = 0;

			y++;
			if ( y >= N_HUNYAPIYO3_MAP_SY ) { break; }
		}
	}


	int percent = (int) ( (n_type_real) p->score / N_HUNYAPIYO3_MAP_ALL * 100 );
	n_posix_snprintf_literal( p->font_server_text, 100, "%d%%", percent );


	n_hunyapiyo3_font_server( p );


	p->refresh = TRUE;
	return;
}




#define n_hunyapiyo3_zero( p ) n_memory_zero( p, sizeof( n_hunyapiyo3 ) )

void
n_hunyapiyo3_init( n_hunyapiyo3 *p )
{

	// Global

	n_bmp_safemode = FALSE;

	n_bmp_transparent_onoff_default = FALSE;

	n_random_shuffle();

#ifdef N_POSIX_PLATFORM_MAC

	n_bmp_flip_onoff = TRUE;

#endif


	// Metrics

	n_hunyapiyo3_metrics( p );


	// Resources

#ifdef N_POSIX_PLATFORM_MAC

	n_mac_image_rc_load_bmp( @"rainbow"   , &p->bmp[ 0 ] );
	n_mac_image_rc_load_bmp( @"watchcat"  , &p->bmp[ 1 ] );
	n_mac_image_rc_load_bmp( @"himalayan" , &p->bmp[ 2 ] );
	n_mac_image_rc_load_bmp( @"neko_no_te", &p->bmp[ 3 ] );
	n_mac_image_rc_load_bmp( @"question"  , &p->bmp_question );

#else

	n_win64_resource_load_nbmp( n_posix_literal( "N_RAINBOW"   ), &p->bmp[ 0 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_WATCHCAT"  ), &p->bmp[ 1 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_HIMALAYAN" ), &p->bmp[ 2 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_NEKONOTE"  ), &p->bmp[ 3 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_QUESTION"  ), &p->bmp_question );

#endif

//n_bmp_save_literal( &p->bmp[ 0 ], "ret.bmp" );


#ifdef N_POSIX_PLATFORM_MAC

	int i = 0;
	n_posix_loop
	{

		n_bmp_mac_color( &p->bmp[ i ] );

		n_bmp_resizer( &p->bmp[ i ], p->unit, p->unit, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );

		i++;
		if ( i >= 4 ) { break; }
	}

	n_bmp_resizer( &p->bmp_question, p->unit, p->unit, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );

#else

	n_type_real scale = n_win64_scale_factor_ui( p->hwnd );

	int i = 0;
	n_posix_loop
	{

		if ( scale >= 2.0 )
		{
			n_bmp_scaler_big( &p->bmp[ i ], scale );
		}

		n_bmp_resizer( &p->bmp[ i ], p->unit, p->unit, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );

		i++;
		if ( i >= 4 ) { break; }
	}

	if ( scale >= 2.0 )
	{
		n_bmp_scaler_big( &p->bmp_question, scale );
	}

	n_bmp_resizer( &p->bmp_question, p->unit, p->unit, n_bmp_white_invisible, N_BMP_RESIZER_CENTER );

#endif


	// Init

#ifdef N_POSIX_PLATFORM_MAC

	n_bmp_zero( &p->canvas_base );
	n_bmp_new( &p->canvas_base, p->sx, p->sy + p->bar );

	p->rep = n_mac_image_NSBitmapImageRep( &p->canvas_base );
	n_mac_image_imagerep_alias( p->rep, &p->canvas_alias );
	p->canvas = &p->canvas_alias;

	n_hunyapiyo3_gdi_background( p, &p->bmp_bg );

	n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );

	n_bmp_carboncopy( &p->bmp_bg, &p->bmp_result );

	n_hunyapiyo3_reset( p );

#endif

	return;
}

void
n_hunyapiyo3_exit( n_hunyapiyo3 *p )
{

	int i = 0;
	n_posix_loop
	{
		n_bmp_free_fast( &p->bmp[ i ] );

		i++;
		if ( i >= N_HUNYAPIYO3_BMP_ALL ) { break; }
	}

	n_bmp_free_fast( &p->bmp_bg );
	n_bmp_free_fast( &p->bmp_result );
	n_bmp_free_fast( &p->bmp_question );
	n_bmp_free_fast( &p->bmp_font );
	n_bmp_free_fast( &p->bmp_button );

	n_bmp_free_fast( &p->transition_bmp_old );
	n_bmp_free_fast( &p->transition_bmp_new );

	n_bmp_free_fast( &p->animation_bmp_tmp );
	n_bmp_free_fast( &p->animation_bmp_old );
	n_bmp_free_fast( &p->animation_bmp_new );

#ifndef N_POSIX_PLATFORM_MAC

	n_hunyapiyo3_sound_exit( p );

#endif


	return;
}

void
n_hunyapiyo3_loop( n_hunyapiyo3 *p )
{

	if ( p->transition_onoff )
	{
//NSLog( @"%f", p->transition_percent );

		BOOL ret = n_bmp_ui_transition
		(
			 p->canvas,
			&p->transition_bmp_old,
			&p->transition_bmp_new,
			N_HUNYAPIYO3_FADE_MSEC,
			&p->transition_percent,
			N_BMP_UI_TRANSITION_FADE
		);
		if ( ret )
		{
			p->transition_onoff = FALSE;

			n_bmp_free( &p->transition_bmp_old );
			n_bmp_free( &p->transition_bmp_new );
		}

		p->refresh = TRUE;
		return;
	}

	if ( p->animation_onoff )
	{

		n_bmp_flush( &p->animation_bmp_tmp, n_bmp_white_invisible );

		BOOL ret = n_bmp_ui_transition
		(
			&p->animation_bmp_tmp,
			&p->animation_bmp_old,
			&p->animation_bmp_new,
			 N_HUNYAPIYO3_ANIM_MSEC,
			&p->animation_percent,
			 p->animation_type
		);

		n_bmp_fastcopy
		(
			&p->bmp_bg,
			 p->canvas,
			 p->animation_x,
			 p->animation_y,
			 N_BMP_SX( &p->animation_bmp_tmp ),
			 N_BMP_SY( &p->animation_bmp_tmp ),
			 p->animation_x,
			 p->animation_y
		);

		n_bmp_transcopy
		(
			&p->animation_bmp_tmp,
			 p->canvas,
			 0, 0,
			 N_BMP_SX( &p->animation_bmp_tmp ),
			 N_BMP_SY( &p->animation_bmp_tmp ),
			 p->animation_x,
			 p->animation_y
		);

		n_hunyapiyo3_frame( p, p->canvas, p->animation_x, p->animation_y, p->color_frame, N_HUNYAPIYO3_COLOR_FRAME_O );

		if ( ret )
		{
			p->animation_onoff = FALSE;

			n_bmp_free( &p->animation_bmp_old );
			n_bmp_free( &p->animation_bmp_new );
		}


		p->refresh = TRUE;
		return;
	}




	if ( p->sound_lock_onoff )
	{
		if ( n_bmp_ui_timer( &p->sound_lock, N_HUNYAPIYO3_O_X_SOUND_MSEC ) )
		{
			p->sound_lock_onoff = FALSE;
		} else {
			return;
		}
	}


	if ( p->phase == N_HUNYAPIYO3_PHASE_CLCK )
	{
//NSLog( @"0" );

		n_hunyapiyo3_text_jump( p, n_posix_literal( "Click!" ), p->canvas );

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_STRT )
	{
//NSLog( @"3" );

		p->phase       = N_HUNYAPIYO3_PHASE_CD_3;
		p->phase_timer = n_posix_tickcount();
//NSLog( @"3 : %d", p->phase_timer );

		n_string_truncate( p->font_server_text );
		n_bmp_free( &p->bmp_font );

		p->throbber_degree = 0;
		p->throbber_timer  = n_posix_tickcount();

		n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_COUNTDOWN_3 );

		//n_bmp_flush( &game.bmp, p->color_bg );
		n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );

		p->countdown_text_animation_size = p->countdown_text_animation_min;

		p->refresh = TRUE;

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_CD_3 )
	{
//return;
//static BOOL stop = FALSE;
//if ( stop == FALSE ) { stop = TRUE; } else { return; }

		if ( n_bmp_ui_timer( &p->phase_timer, 1000 ) )
		{
//NSLog( @"2 : %d", p->phase_timer );

			p->phase       = N_HUNYAPIYO3_PHASE_CD_2;
			p->phase_timer = n_posix_tickcount();

			n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_COUNTDOWN_2 );

			//n_bmp_flush( &game.bmp, p->color_bg );
			n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );

			n_bmp_free( &p->bmp_font );
			p->countdown_text_animation_size = p->countdown_text_animation_min;

		} else {

			n_hunyapiyo3_countdown( p, 3 );

		}

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_CD_2 )
	{
//return;

		if ( n_bmp_ui_timer( &p->phase_timer, 1000 ) )
		{
//n_game_hwndprintf_literal( "1" );

			p->phase       = N_HUNYAPIYO3_PHASE_CD_1;
			p->phase_timer = n_posix_tickcount();


			n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_COUNTDOWN_1 );

			//n_bmp_flush( &game.bmp, p->color_bg );
			n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );

			n_bmp_free( &p->bmp_font );
			p->countdown_text_animation_size = p->countdown_text_animation_min;

		} else {

			n_hunyapiyo3_countdown( p, 2 );

		}

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_CD_1 )
	{

		if ( n_bmp_ui_timer( &p->phase_timer, 1000 ) )
		{
//n_game_hwndprintf( N_HUNYAPIYO3_APPNAME );

			p->phase       = N_HUNYAPIYO3_PHASE_CD_0;
			p->phase_timer = n_posix_tickcount();

			//n_bmp_flush( &game.bmp, p->color_bg );
			n_bmp_flush_fastcopy( &p->bmp_bg, p->canvas );

			n_bmp_free( &p->bmp_font );
			p->countdown_text_animation_size = p->countdown_text_animation_min;

		} else {

			n_hunyapiyo3_countdown( p, 1 );

		}

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_CD_0 )
	{

		n_hunyapiyo3_short_term_memory( p );

		p->phase       = N_HUNYAPIYO3_PHASE_INIT;
		p->phase_timer = n_posix_tickcount();
		p->progress    = p->phase_timer;

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_INIT )
	{

		if ( n_bmp_ui_timer( &p->phase_timer, N_HUNYAPIYO3_SHORTTERM_MSEC ) )
		{
			p->phase = N_HUNYAPIYO3_PHASE_MAIN;

//n_bmp_flush( p->canvas, n_bmp_white );
//p->refresh = TRUE;
		} else {
//n_game_hwndprintf_literal( " %f ", (n_type_real) ( n_posix_tickcount() - p->progress ) / N_HUNYAPIYO3_SHORTTERM_MSEC );
			n_hunyapiyo3_progressbar( p, (n_type_real) ( n_posix_tickcount() - p->progress ) / N_HUNYAPIYO3_SHORTTERM_MSEC );
		}

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_MAIN )
	{
//n_bmp_save( p->canvas, "ret.bmp" );

		n_hunyapiyo3_main( p );

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_RSLT )
	{

		n_hunyapiyo3_result( p );

	} else
	if ( p->phase == N_HUNYAPIYO3_PHASE_PERC )
	{

		if ( p->is_fanfare )
		{
			p->is_fanfare = FALSE;
			n_hunyapiyo3_sound_play( p, N_HUNYAPIYO3_SOUND_FANFARE );
		}

		n_hunyapiyo3_last( p );

	} // else


	return;
}

