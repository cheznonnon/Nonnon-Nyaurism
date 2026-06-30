// LINE MASHER 2
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#include "../../../neutral/bmp/all.c"


#include "../../../neutral/bmp/ui/animated_counter.c"
#include "../../../neutral/bmp/ui/transition.c"


#include "../../../bridge/chara.c"
#include "../../../bridge/gdi.c"




#ifdef N_POSIX_PLATFORM_MAC


#include "../../../mac/_mac.c"
#include "../../../mac/image.c"
#include "../../../mac/sound.c"
#include "../../../mac/window.c"

#include "../../../mac/gamepad.c"


#else


#include "../../../win64/game_engine.c"

#include "../../../neutral/wav/all.c"
#include "../../../win64/resource.c"
#include "../../../bridge/sound.c"

#include "../../../win64/XInput.c"


#define n_bmp_rgb_mac  n_bmp_rgb
#define n_bmp_argb_mac n_bmp_argb


#define N_MAC_KEYCODE_SPACE       VK_SPACE
#define N_MAC_KEYCODE_ARROW_LEFT  VK_LEFT
#define N_MAC_KEYCODE_ARROW_RIGHT VK_RIGHT


#endif




static n_type_real n_lm2_chara_draw_blend = 0.0;

void
n_lm2_chara_draw( n_game_chara *c )
{

	n_game_chara_prv( c );


	n_type_gfx fx  = c->srcx;
	n_type_gfx fy  = c->srcy;
	n_type_gfx fsx = c->sx;
	n_type_gfx fsy = c->sy;
	n_type_gfx tx  = c->x;
	n_type_gfx ty  = c->y;

	if ( c->mx ) { fx += c->mx; fsx -= c->mx * 2; tx += c->mx; }
	if ( c->my ) { fy += c->my; fsy -= c->my * 2; ty += c->my; }

	n_bmp_blendcopy( c->chara, c->main, fx,fy,fsx,fsy, tx,ty, n_lm2_chara_draw_blend );


	return;
}

n_posix_inline void
n_lm2_chara_erase( const n_game_chara *c )
{

	n_type_gfx fx  = c->px;
	n_type_gfx fy  = c->py;
	n_type_gfx fsx = c->sx;
	n_type_gfx fsy = c->sy;
	n_type_gfx tx  = fx;
	n_type_gfx ty  = fy;

	if ( c->mx ) { fx += c->mx; fsx -= c->mx * 2; tx += c->mx; }
	if ( c->my ) { fy += c->my; fsy -= c->my * 2; ty += c->my; }

	if ( c->bg == NULL )
	{
		// [!] : for title screen
	} else {
		n_bmp_fastcopy( c->bg, c->main, fx,fy,fsx,fsy, tx,ty );
	}


	return;
}





// Constants

#define N_LM2_CHARA_INDEX    ( 0 )
#define N_LM2_MSG_1_INDEX    ( 1 )
#define N_LM2_MSG_2_INDEX    ( 2 )
#define N_LM2_BMPBG_INDEX    ( 3 )
#define N_LM2_STARS_INDEX    ( 4 )
#define N_LM2_BMP_ALL        ( 5 )

#define N_LM2_START_INDEX    ( 0 )
#define N_LM2_BWOPP_INDEX    ( 1 )
#define N_LM2_SHOOT_INDEX    ( 2 )
#define N_LM2_CHIPP_INDEX    ( 3 )
#define N_LM2_WOOOW_INDEX    ( 4 )
#define N_LM2_PHOOT_INDEX    ( 5 )
#define N_LM2_SND_ALL        ( 6 )

#define N_LM2_CHARA_FIRE_MIN (    3 )
#define N_LM2_CHARA_FIRE_MAX (  255 )
#define N_LM2_CHARA_FIRE_RND ( 1500 )
#define N_LM2_VIRUS_FIRE_RND ( 2500 )
#define N_LM2_VIRUS_SX       (   18 )
#define N_LM2_VIRUS_SY       (    3 )
#define N_LM2_VIRUS_MAX      ( N_LM2_VIRUS_SX * N_LM2_VIRUS_SY )


#define N_LM2_FIRE_OFF            0
#define N_LM2_FIRE_ON_SLOW        1
#define N_LM2_FIRE_ON_FAST        2
#define N_LM2_FIRE_ON_TRICKY_SLOW 3
#define N_LM2_FIRE_ON_TRICKY_FAST 4

#define N_LM2_FIRE_ON        N_LM2_FIRE_ON_SLOW
#define N_LM2_FIRE_ON_DOUBLE N_LM2_FIRE_ON_TRICKY_SLOW

#define N_LM2_VIRUS_STOP     0
#define N_LM2_VIRUS_FIRE     1
#define N_LM2_VIRUS_DEAD     2


#define N_LM2_STARS_MAX       ( 50 )

#define N_LM2_STARS_SLOW      (  0 )
#define N_LM2_STARS_FAST      (  1 )
#define N_LM2_STARS_RAND      (  5 )


#define N_LM2_POWER_NONE     ( 0 )
#define N_LM2_POWER_LASER_1  ( 1 )
#define N_LM2_POWER_LASER_2  ( 2 )
#define N_LM2_POWER_GATLING  ( 3 )
#define N_LM2_POWER_STARDUST ( 4 )
#define N_LM2_POWER_SHIELD   ( 5 )

#define n_lm2_power_laser_1(  p ) ( ( (p)->level >=   2 )&&( (p)->power < N_LM2_POWER_LASER_1  ) )
#define n_lm2_power_laser_2(  p ) ( ( (p)->level >=   5 )&&( (p)->power < N_LM2_POWER_LASER_2  ) )
#define n_lm2_power_gatling(  p ) ( ( (p)->level >=  10 )&&( (p)->power < N_LM2_POWER_GATLING  ) )
#define n_lm2_power_stardust( p ) ( ( (p)->level >=  25 )&&( (p)->power < N_LM2_POWER_STARDUST ) )
#define n_lm2_power_shield(   p ) ( ( (p)->level >= 100 )&&( (p)->power < N_LM2_POWER_SHIELD   ) )

#define N_LM2_FIRE_LEVEL_1 (  200 )
#define N_LM2_FIRE_LEVEL_2 (  500 )
#define N_LM2_FIRE_LEVEL_3 ( 1000 )


#define N_LM2_TRANSITION_NONE     ( 0 )
#define N_LM2_TRANSITION_FADE_IN  ( 1 )
#define N_LM2_TRANSITION_FADE_OUT ( 2 )


#define N_LM2_INPUT_L    ( 1 << 0 )
#define N_LM2_INPUT_R    ( 1 << 1 )
#define N_LM2_INPUT_FIRE ( 1 << 2 )
#define N_LM2_INPUT_F1   ( 1 << 3 )
#define N_LM2_INPUT_F2   ( 1 << 4 )
#define N_LM2_INPUT_F3   ( 1 << 5 )


#define N_LM2_SCORE_MAX ( 99999 )




typedef struct {

#ifdef N_POSIX_PLATFORM_MAC
	NonnonGame  *self;
	n_bmp        canvas_base;
	n_bmp        canvas_alias;

	GCExtendedGamepad *gamepad;
#else
	HWND         hwnd;

	n_XInput     xinput;
#endif

	BOOL         refresh;
	n_bmp       *canvas;
	u32          color;

	int          input;
	BOOL         input_lock;
	BOOL         input_any_keys_onoff;

	BOOL         title_screen_onoff;
	BOOL         title_screen_stop_movement;
	n_bmp        title_screen;
	n_bmp        title_screen_scanning_line;
	n_bmp        title_screen_press_any_keys;
	n_type_real  title_screen_press_any_keys_blend;
	BOOL         title_screen_press_any_keys_updown;

	BOOL         title_screen_transition_onoff;
	n_bmp        title_screen_transition_bmp_old;
	n_bmp        title_screen_transition_bmp_new;
	n_type_real  title_screen_transition_percent;

	n_type_gfx   zoom;
	n_type_gfx   csx,csy;
	n_type_gfx   unit;
	n_type_gfx   min_x, mid_x, max_x;
	n_type_gfx   msg1_x, msg2_x;
	n_type_gfx   hit_offset_x,hit_offset_y;

	n_type_gfx   star_size;

	n_type_gfx   step_chara;
	n_type_gfx   step_chara_fire;
	n_type_gfx   step_virus_slow;
	n_type_gfx   step_virus_fast;

	n_gdi        gdi;
	n_bmp        bmp[ N_LM2_BMP_ALL ];

	n_game_chara item;
	n_game_chara chara;
	n_game_chara virus     [ N_LM2_VIRUS_MAX      ];
	n_game_chara chara_fire[ N_LM2_CHARA_FIRE_MAX ];
	n_game_chara virus_fire[ N_LM2_VIRUS_MAX      ];
	n_type_real  gatling   [ N_LM2_CHARA_FIRE_MAX ];

	n_game_chara stars     [ N_LM2_STARS_MAX      ];
	u32          stars_tmr [ N_LM2_STARS_MAX      ];

	int          chara_fire_min;
	int          chara_fire_max;
	int          chara_fire_rnd;
	int          chara_fire_cur;
	u32          chara_fire_tmr;

	int          virus_sx;
	int          virus_sy;
	int          virus_max;
	int          virus_fire_rnd;
	int          virus_fire_tmr;
	int          virus_fire_level;

	BOOL         reset;
	u32          reset_timer;
	int          hiscore, score, score_counter;
	int          hiscore_prev, score_prev;
	int          level, power;

	BOOL         delayed_margin_onoff;

	n_type_real  transition_phase;

	BOOL         debug_invincible;

	n_bmp_ui_animated_counter animated_counter_hiscr;
	n_bmp_ui_animated_counter animated_counter_score;

} n_lm2;




void
n_lm2_settings( n_lm2 *p, BOOL is_read )
{

#ifdef N_POSIX_PLATFORM_MAC

	if ( is_read )
	{
		NSString     *nsstr = n_mac_settings_read( @"highscore" );
		n_posix_char *str   = n_mac_nsstring2str( nsstr );

		p->hiscore = n_posix_atoi( str );
//NSLog( @"%d", p->hiscore );
	
		n_memory_free( str );
	} else {
		n_posix_char s[ 100 ];
		n_posix_snprintf_literal( s, 100, "%d", p->hiscore );

		n_mac_settings_write( @"highscore", n_mac_str2nsstring( s ) );

	}

#else

	n_ini ini; n_ini_zero( &ini ); n_ini_load( &ini, n_posix_literal( "nonnon.ini" ) );

	if ( is_read )
	{
		n_posix_char str[ 100 ];

		n_ini_value_str_literal( &ini, "[LINE MASHER 2]", "highscore", "0", str, 100 );
		p->hiscore = n_posix_atoi( str );
//NSLog( @"%d", p->hiscore );
	} else {
		n_ini_section_add_literal( &ini, "[LINE MASHER 2]" );

		n_posix_char str[ 100 ];
		n_posix_snprintf_literal( str, 100, "%d", p->hiscore );

		n_ini_key_add_int_literal( &ini, "[LINE MASHER 2]", "highscore", p->hiscore );

		n_ini_save( &ini, n_posix_literal( "nonnon.ini" ) );

	}

	n_ini_free( &ini );

#endif


	return;
}




#ifdef N_POSIX_PLATFORM_MAC


//#define N_LM2_SOUND_SIMPLE


// [!] : this is important to play

#ifdef N_LM2_SOUND_SIMPLE

// [x] : high value causes slow down
#define N_LM2_SND_SIMUL ( 1 )

static AVAudioPlayer *n_lm2_snd_start = NULL;
static AVAudioPlayer *n_lm2_snd_shoot[ N_LM2_SND_SIMUL ];
static AVAudioPlayer *n_lm2_snd_phoot[ N_LM2_SND_SIMUL ];
static AVAudioPlayer *n_lm2_snd_bwopp = NULL;
static AVAudioPlayer *n_lm2_snd_chipp[ N_LM2_SND_SIMUL ];
static AVAudioPlayer *n_lm2_snd_wooow = NULL;

void
n_lm2_sound( n_lm2 *p, int index )
{

	static int i = 0;


	if ( index == N_LM2_START_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_start );
	} else
	if ( index == N_LM2_SHOOT_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_shoot[ i ] );
	} else
	if ( index == N_LM2_PHOOT_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_phoot[ i ] );
	} else
	if ( index == N_LM2_BWOPP_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_bwopp );
	} else
	if ( index == N_LM2_CHIPP_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_chipp[ i ] );
	} else
	if ( index == N_LM2_WOOOW_INDEX )
	{
		n_mac_sound_play( n_lm2_snd_wooow );
	} else { return; }


	i++;
	if ( i >= N_LM2_SND_SIMUL ) { i = 0; }


	return;
}

void
n_lm2_sound_init( n_lm2 *p )
{

	n_lm2_snd_start = n_mac_sound_init( @"rc/start", @"wav" );
	n_lm2_snd_bwopp = n_mac_sound_init( @"rc/bwopp", @"wav" );
	n_lm2_snd_wooow = n_mac_sound_init( @"rc/woow" , @"wav" );

	int i = 0;
	n_posix_loop
	{
		n_lm2_snd_shoot[ i ] = n_mac_sound_init( @"rc/shoot", @"wav" );
		n_lm2_snd_phoot[ i ] = n_mac_sound_init( @"rc/phoot", @"wav" );
		n_lm2_snd_chipp[ i ] = n_mac_sound_init( @"rc/click", @"wav" );

		i++;
		if ( i >= N_LM2_SND_SIMUL ) { break; }
	}


	return;
}

#else  // #ifdef N_LM2_SOUND_SIMPLE

static n_mac_sound2 n_lm2_snd_start;
static n_mac_sound2 n_lm2_snd_shoot;
static n_mac_sound2 n_lm2_snd_phoot;
static n_mac_sound2 n_lm2_snd_bwopp;
static n_mac_sound2 n_lm2_snd_chipp;
static n_mac_sound2 n_lm2_snd_wooow;

void
n_lm2_sound( n_lm2 *p, int index )
{

	if ( index == N_LM2_START_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_start );
	} else
	if ( index == N_LM2_SHOOT_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_shoot );
	} else
	if ( index == N_LM2_PHOOT_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_phoot );
	} else
	if ( index == N_LM2_BWOPP_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_bwopp );
	} else
	if ( index == N_LM2_CHIPP_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_chipp );
	} else
	if ( index == N_LM2_WOOOW_INDEX )
	{
		n_mac_sound2_play( &n_lm2_snd_wooow );
	} else { return; }


	return;
}

void
n_lm2_sound_init( n_lm2 *p )
{

	n_mac_sound2_init( &n_lm2_snd_start, @"rc/start", @"wav" );
	n_mac_sound2_init( &n_lm2_snd_bwopp, @"rc/bwopp", @"wav" );
	n_mac_sound2_init( &n_lm2_snd_wooow, @"rc/woow" , @"wav" );
	n_mac_sound2_init( &n_lm2_snd_shoot, @"rc/shoot", @"wav" );
	n_mac_sound2_init( &n_lm2_snd_phoot, @"rc/phoot", @"wav" );
	n_mac_sound2_init( &n_lm2_snd_chipp, @"rc/click", @"wav" );


	return;
}

#endif // #ifdef N_LM2_SOUND_SIMPLE


#else


static n_game_sound n_lm2_snd_start;
static n_game_sound n_lm2_snd_shoot;
static n_game_sound n_lm2_snd_phoot;
static n_game_sound n_lm2_snd_bwopp;
static n_game_sound n_lm2_snd_chipp;
static n_game_sound n_lm2_snd_wooow;

void
n_lm2_sound_init( n_lm2 *p )
{

	n_game_sound_init_literal( &n_lm2_snd_start, p->hwnd, "N_LM2_WAV_4" );
	n_game_sound_init_literal( &n_lm2_snd_shoot, p->hwnd, "N_LM2_WAV_3" );
	n_game_sound_init_literal( &n_lm2_snd_phoot, p->hwnd, "N_LM2_WAV_2" );
	n_game_sound_init_literal( &n_lm2_snd_chipp, p->hwnd, "N_LM2_WAV_1" );
	n_game_sound_init_literal( &n_lm2_snd_bwopp, p->hwnd, "N_LM2_WAV_0" );
	n_game_sound_init_literal( &n_lm2_snd_wooow, p->hwnd, "N_LM2_WAV_5" );


	return;
}

void
n_lm2_sound_exit( n_lm2 *p )
{

	n_game_sound_exit( &n_lm2_snd_start );
	n_game_sound_exit( &n_lm2_snd_shoot );
	n_game_sound_exit( &n_lm2_snd_phoot );
	n_game_sound_exit( &n_lm2_snd_chipp );
	n_game_sound_exit( &n_lm2_snd_bwopp );
	n_game_sound_exit( &n_lm2_snd_wooow );


	return;
}

void
n_lm2_sound( n_lm2 *p, int index )
{

	if ( index == N_LM2_START_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_start );
	} else
	if ( index == N_LM2_SHOOT_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_shoot );
	} else
	if ( index == N_LM2_PHOOT_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_phoot );
	} else
	if ( index == N_LM2_BWOPP_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_bwopp );
	} else
	if ( index == N_LM2_CHIPP_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_chipp );
	} else
	if ( index == N_LM2_WOOOW_INDEX )
	{
		n_game_sound_loop( &n_lm2_snd_wooow );
	} else { return; }


	return;
}


#endif




// Component

#include "./effect.c"




#define n_lm2_zero( p ) n_memory_zero( p, sizeof( n_lm2 ) )

#define n_lm2_vibrate_off( p ) n_lm2_vibrate( p, FALSE )
#define n_lm2_vibrate_on(  p ) n_lm2_vibrate( p, TRUE  )

void
n_lm2_vibrate( n_lm2 *p, BOOL is_on )
{

#ifdef N_POSIX_PLATFORM_WINDOWS

	int v;
	if ( is_on )
	{
		v = N_XINPUT_VIBRATE_MAX;
	} else {
		v = 0;
	}

	n_XInput_vibrate( &p->xinput, v, v );

#endif

	return;
}

void
n_lm2_gradient_refresh( n_lm2 *p, u32 upper, u32 lower )
{
//u32 tick = n_posix_tickcount();


	// [!] : 2 msec @ 1500MHz

	n_bmp_new_fast( &p->bmp[ N_LM2_BMPBG_INDEX ], p->csx, p->csy );

	n_bmp_flush( &p->bmp[ N_LM2_BMPBG_INDEX ], upper );


	// [!] : Performance : CPU @ 1500MHz
	//
	//	1/1 : 200 msec
	//	1/2 :  70 msec
	//	1/3 :  50 msec

	int option = N_BMP_GRADIENT_RECTANGLE | N_BMP_GRADIENT_VERTICAL;

	n_type_gfx sy = p->csy / 3;
	n_bmp_gradient
	(
		&p->bmp[ N_LM2_BMPBG_INDEX ],
		0, p->csy - sy, p->csx, sy,
		upper, lower, option
	);


//n_game_hwndprintf_literal( "%d", (int) n_posix_tickcount() - tick );
	return;
}

void
n_lm2_digitbitmap( n_lm2 *p )
{
//return;

	// Hiscore

	if ( p->hiscore_prev != p->hiscore )
	{

		static u32 timer = 0;
		if ( n_bmp_ui_timer( &timer, 100 ) )
		{
			n_bmp_ui_animated_counter_loop_count( &p->animated_counter_hiscr, p->hiscore );
		}

		p->hiscore_prev = p->hiscore;

	}

	{

		n_type_gfx x = p->unit + N_BMP_SX( &p->bmp[ N_LM2_MSG_1_INDEX ] );
		n_type_gfx y = 0;

		p->animated_counter_hiscr.blend = n_lm2_chara_draw_blend;

		n_bmp_ui_animated_counter_loop_draw( &p->animated_counter_hiscr, p->canvas, x,y );

	}

	{

		n_bmp      *b = &p->bmp[ N_LM2_MSG_1_INDEX ];
		n_type_gfx  x = p->msg1_x;
		n_type_gfx  y = 0;
		n_type_gfx sx = N_BMP_SX( b );
		n_type_gfx sy = N_BMP_SY( b );

		n_bmp_blendcopy( b, p->canvas, 0,0,sx,sy, x,y, n_lm2_chara_draw_blend );

	}


	// Score
//return;

	if ( p->score_prev != p->score )
	{

		n_bmp_ui_animated_counter_loop_count( &p->animated_counter_score, p->score );

		p->score_prev = p->score;

	}

	n_type_gfx digit_sx = N_BMP_SX( &p->animated_counter_score.digit[ 0 ] );

	{

		n_type_gfx x = p->csx - p->unit - ( digit_sx * p->animated_counter_score.figure );
		n_type_gfx y = 0;

		p->animated_counter_score.blend = n_lm2_chara_draw_blend;

		n_bmp_ui_animated_counter_loop_draw( &p->animated_counter_score, p->canvas, x,y );

	}

	{

		n_bmp      *b = &p->bmp[ N_LM2_MSG_2_INDEX ];
		n_type_gfx sx = N_BMP_SX( b );
		n_type_gfx sy = N_BMP_SY( b );
		n_type_gfx  x = p->csx - p->unit - ( digit_sx * p->animated_counter_score.figure ) + p->animated_counter_score.offset - sx + digit_sx;
		n_type_gfx  y = 0;

		n_bmp_blendcopy( b, p->canvas, 0,0,sx,sy, x,y, n_lm2_chara_draw_blend );

	}


	// [!] : avoid first time

	p->animated_counter_hiscr.msec = 333;
	p->animated_counter_score.msec = 333;


	return;
}

void
n_lm2_reset( n_lm2 *p )
{

	n_bmp *bmp_bg    = &p->bmp[ N_LM2_BMPBG_INDEX ];
	n_bmp *bmp_chara = &p->bmp[ N_LM2_CHARA_INDEX ];

	if ( p->title_screen_onoff ) { bmp_bg = NULL; }


	n_type_gfx u = p->unit;


	n_game_chara_zero( &p->chara );
	n_game_chara_zero( &p->item  );

	n_game_chara_bmp( &p->chara, p->canvas, bmp_chara, bmp_bg, p->color );
	n_game_chara_bmp( &p->item,  p->canvas, bmp_chara, bmp_bg, p->color );

	n_game_chara_src( &p->chara, u * 0,u * 0, u,u, 0,0 );
	n_game_chara_src( &p->item,  u * 0,u * 3, u,u, p->zoom * 3,p->zoom * 3 );

	n_game_chara_pos( &p->chara, p->mid_x, p->csy - p->unit );
	n_game_chara_pos( &p->item,  p->mid_x,        - p->unit );

	n_game_chara_prv( &p->chara );
	n_game_chara_prv( &p->item  );


	n_game_chara_bulk_zero( p->chara_fire, p->chara_fire_max );
	n_game_chara_bulk_zero( p->virus_fire, p->virus_max      );
	n_game_chara_bulk_zero( p->virus,      p->virus_max      );

	n_game_chara_bulk_bmp( p->chara_fire, p->chara_fire_max, p->canvas, bmp_chara, bmp_bg, p->color );
	n_game_chara_bulk_bmp( p->virus     , p->virus_max,      p->canvas, bmp_chara, bmp_bg, p->color );
	n_game_chara_bulk_bmp( p->virus_fire, p->virus_max,      p->canvas, bmp_chara, bmp_bg, p->color );

	n_game_chara_bulk_src( p->chara_fire, p->chara_fire_max, u * 0,u * 2, u,u, p->zoom * 6,p->zoom * 2 );
	n_game_chara_bulk_src( p->virus     , p->virus_max,      u * 0,u * 1, u,u, p->zoom * 1,p->zoom * 1 );
	n_game_chara_bulk_src( p->virus_fire, p->virus_max,      u * 3,u * 1, u,u, p->zoom * 3,p->zoom * 3 );


	int i = 0;
	n_posix_loop
	{

		int x = ( i % p->virus_sx ) * u;
		int y = ( i / p->virus_sx ) * u;

		n_game_chara_pos( &p->virus[ i ], u + x, u + y );
		n_game_chara_prv( &p->virus[ i ] );

		n_game_chara_pos( &p->virus_fire[ i ], -u, -u );
		n_game_chara_prv( &p->virus_fire[ i ] );


		i++;
		if ( i >= p->virus_max ) { break; }
	}


	p->hiscore_prev   = -1;
	p->score_prev     = -1;
	p->score          = 0;
	p->score_counter  = 0;
	p->level          = p->score / 100;
	p->power          = N_LM2_POWER_NONE;


	// [!] : reset offset
	n_bmp_ui_animated_counter_loop_count( &p->animated_counter_score, p->score );


	p->chara_fire_cur = p->chara_fire_min;
	p->chara_fire_tmr = p->chara_fire_rnd / p->chara_fire_cur;
	p->virus_fire_tmr = p->virus_fire_rnd / p->chara_fire_cur;


	p->virus_fire_level = N_LM2_FIRE_ON_SLOW;


	n_lm2_effect_init( p, bmp_bg );


	p->delayed_margin_onoff = FALSE;


	// [!] : Moving Stars

	static BOOL stars_init = FALSE;

	if ( stars_init == FALSE )
	{

		stars_init = TRUE;


		if ( p->zoom == 1 )
		{
			p->star_size = p->zoom;
		} else {
			p->star_size = p->zoom * 2;
		}

		n_bmp_new( &p->bmp[ N_LM2_STARS_INDEX ], p->star_size, p->star_size );

		if ( p->star_size > 2 )
		{
			n_bmp_flush_hoop( &p->bmp[ N_LM2_STARS_INDEX ], p->star_size, n_bmp_white );
		} else {
			n_bmp_flush( &p->bmp[ N_LM2_STARS_INDEX ], n_bmp_white );
		}
//n_bmp_save_literal( &p->bmp[ N_LM2_STARS_INDEX ], "ret.bmp" );


		n_game_chara_bulk_zero( p->stars, N_LM2_STARS_MAX );

		n_game_chara_bulk_bmp( p->stars, N_LM2_STARS_MAX, p->canvas, &p->bmp[ N_LM2_STARS_INDEX ], bmp_bg, p->color );
		n_game_chara_bulk_src( p->stars, N_LM2_STARS_MAX, 0,0, p->star_size,p->star_size, 0,0 );
//n_bmp_save_literal( p->stars[ 0 ].chara, "ret2.bmp" );

		int i = 0;
		n_posix_loop
		{//break;

			n_type_gfx x = n_random_range( N_BMP_SX( p->canvas ) );
			n_type_gfx y = n_random_range( N_BMP_SY( p->canvas ) );

			n_game_chara_pos( &p->stars[ i ], x, y );
			n_game_chara_prv( &p->stars[ i ] );


			if ( 0 == n_random_range( N_LM2_STARS_RAND ) )
			{
				p->stars[ i ].data = N_LM2_STARS_FAST;
			} else {
				p->stars[ i ].data = N_LM2_STARS_SLOW;
			}


			i++;
			if ( i >= N_LM2_STARS_MAX ) { break; }
		}

	} else {

		n_game_chara_bulk_bmp( p->stars, N_LM2_STARS_MAX, p->canvas, &p->bmp[ N_LM2_STARS_INDEX ], bmp_bg, p->color );

	}


	return;
}

void
n_lm2_levelup( n_lm2 *p )
{

	p->chara_fire_cur = n_posix_min( p->chara_fire_max, p->chara_fire_cur + 1 );
	p->chara_fire_tmr = p->chara_fire_rnd / p->chara_fire_cur;
	p->virus_fire_tmr = p->virus_fire_rnd / p->chara_fire_cur;


	p->level      = p->score / 100;
	p->item.data  = FALSE;
	p->item.data |= n_lm2_power_laser_1 ( p );
	p->item.data |= n_lm2_power_laser_2 ( p );
	p->item.data |= n_lm2_power_gatling ( p );
	p->item.data |= n_lm2_power_stardust( p );
	p->item.data |= n_lm2_power_shield  ( p );


	if ( p->score >= N_LM2_FIRE_LEVEL_3 )
	{
		p->virus_fire_level = N_LM2_FIRE_ON_TRICKY_FAST;
	} else
	if ( p->score >= N_LM2_FIRE_LEVEL_2 )
	{
		p->virus_fire_level = N_LM2_FIRE_ON_TRICKY_SLOW;
	} else
	if ( p->score >= N_LM2_FIRE_LEVEL_1 )
	{
		p->virus_fire_level = N_LM2_FIRE_ON_FAST;
	} else {
		p->virus_fire_level = N_LM2_FIRE_ON_SLOW;
	}


	return;
}

BOOL
n_lm2_keystate_get( n_lm2 *p, int vk )
{
#ifdef N_POSIX_PLATFORM_MAC

	BOOL ret = n_mac_keystate_get( vk );

	if ( vk == N_MAC_KEYCODE_ARROW_LEFT )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.left.pressed;
			ret |= p->gamepad.leftThumbstick.left.pressed;
		}
	} else
	if ( vk == N_MAC_KEYCODE_ARROW_RIGHT )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.right.pressed;
			ret |= p->gamepad.leftThumbstick.right.pressed;
		}
	} else
	if ( vk == N_MAC_KEYCODE_SPACE )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.buttonA.pressed;
			ret |= p->gamepad.buttonB.pressed;
			ret |= p->gamepad.buttonX.pressed;
			ret |= p->gamepad.buttonY.pressed;
		}
	}// else

#else

	BOOL ret = n_win64_is_input( vk );


	// [!] : Win11 : XInput based controllers are supported by joyGetPos()

	// [x] : buggy : joyGetPos(), joyGetPosEx() : overall sluggishness after unplugged

	// [x] : Bluetooth Nintendo Pro Controller : maybe not supported (though Settings support)

	static BOOL unplugged = FALSE;

	if ( n_XInput_is_enabled( &p->xinput ) )
	{
		if ( n_XInput_is_input( &p->xinput, vk ) ) { ret |= TRUE; }
	} else
	if ( unplugged == FALSE )
	{
		JOYINFOEX info; n_memory_zero( &info, sizeof( JOYINFOEX ) );

		info.dwSize  = sizeof( JOYINFOEX );
		info.dwFlags = JOY_RETURNX | JOY_RETURNBUTTONS;

		BOOL mm = joyGetPosEx( 0, &info );
		if ( mm ) { unplugged = TRUE; }

//n_log( "%x %d", info.dwXpos, info.dwButtons );

		if ( vk == VK_LEFT )
		{
			if ( ( mm == FALSE )&&( info.dwXpos == 0x0000 ) ) { ret |= TRUE; }
		} else
		if ( vk == VK_RIGHT )
		{
			if ( ( mm == FALSE )&&( info.dwXpos == 0xffff ) ) { ret |= TRUE; }
		} else
		if ( vk == VK_SPACE )
		{
			if ( ( mm == FALSE )&&( info.dwButtons != 0 )&&( info.dwButtons - 256 ) )  { ret |= TRUE; }
		}

	}

#endif


	return ret;
}

void
n_lm2_input( n_lm2 *p )
{

	// Debug Center

	if ( p->input & N_LM2_INPUT_F1 )
	{
		p->score          = p->score + 100;
		p->score_counter  = p->score % 1000;
		p->chara_fire_cur = p->chara_fire_min + ( p->score / 100 );
		p->chara_fire_cur = n_posix_min( p->chara_fire_max, p->chara_fire_cur );
	}

	if ( p->input & N_LM2_INPUT_F2 )
	{
		p->item.data = TRUE;
	}

	if ( p->input & N_LM2_INPUT_F3 )
	{
		p->debug_invincible = TRUE;
	}


	// Player

	if ( n_lm2_keystate_get( p, N_MAC_KEYCODE_ARROW_RIGHT ) )
	{
		p->chara.x = n_posix_min_n_type_gfx( p->max_x, p->chara.x + p->step_chara );
	}

	if ( n_lm2_keystate_get( p, N_MAC_KEYCODE_ARROW_LEFT ) )
	{
		p->chara.x = n_posix_max( p->min_x, p->chara.x - p->step_chara );
	}


//n_game_hwndprintf_literal( "%d", n_game_input_loop( &p->input, VK_SPACE ) );

	BOOL fire = FALSE;


	static u32 single_timer = 0;
	static u32  rapid_timer = 0;
	static int  rapid       = 1;
	static int  rapid_step  = 4;

	if ( n_lm2_keystate_get( p, N_MAC_KEYCODE_SPACE ) )
	{

		if ( n_bmp_ui_timer( &single_timer, p->chara_fire_tmr / rapid ) )
		{
			fire  = TRUE;
			rapid = n_posix_max( 1, rapid - rapid_step );
		}

		rapid_timer = n_posix_tickcount() + 100;

	} else {

		if ( rapid_timer > n_posix_tickcount() )
		{
			rapid += rapid_step;
			rapid_timer = n_posix_tickcount();
		}

	}

	if ( fire )
	{

		int i = 0;
		n_posix_loop
		{

			n_game_chara *cf = &p->chara_fire[ i ];

			if ( cf->data == N_LM2_FIRE_OFF )
			{

				n_lm2_sound( p, N_LM2_SHOOT_INDEX );

				cf->data = N_LM2_FIRE_ON;
				n_game_chara_pos( cf, p->chara.x, p->chara.y - p->unit );

				break; 
			}

			i++;
			if ( i >= N_LM2_CHARA_FIRE_MAX ) { break; }
		}

	}


	return;
}

void
n_lm2_scanline_init( n_lm2 *p )
{

	n_bmp *b = &p->title_screen_scanning_line;

	n_bmp_new_fast( b, 2,2 );
	n_bmp_flush( b, n_bmp_black_invisible );

	u32 color_stripe = n_bmp_argb( 48,128,128,128 );

	n_bmp_ptr_set_fast( b, 0,1, color_stripe );
	n_bmp_ptr_set_fast( b, 1,1, color_stripe );
	n_bmp_scaler_big( b, 2 );

	n_bmp_resizer( b, p->csx, p->csy, n_bmp_black_invisible, N_BMP_RESIZER_TILE );


	return;
}

void
n_lm2_init( n_lm2 *p )
{

	// Global

	n_bmp_safemode = FALSE;

	//n_bmp_flip_onoff = TRUE;

	n_bmp_transparent_onoff_default = FALSE;


	// Size
	
#ifdef N_POSIX_PLATFORM_MAC
	CGFloat desktop_sx; n_mac_desktop_size( &desktop_sx, NULL );
#else
	n_type_gfx desktop_sx; n_win64_desktop_size( p->hwnd, &desktop_sx, NULL );
#endif

	p->zoom = n_posix_max_n_type_gfx( 1, desktop_sx / 512 );


	p->csx             = 320 * p->zoom;
	p->csy             = 240 * p->zoom;
	p->unit            =  16 * p->zoom;
	p->hit_offset_x    =   3 * p->zoom;
	p->hit_offset_y    =   6 * p->zoom;

	p->min_x           = 0;
	p->mid_x           = ( p->csx - p->unit ) / 2;
	p->max_x           = p->csx - p->unit;
	p->msg1_x          = p->unit;
	p->msg2_x          = p->csx - p->unit - p->unit;

	p->step_chara      = (n_type_gfx) ( (n_type_real) 1.5 * p->zoom );
	p->step_chara_fire = p->step_chara * 2;
	p->step_virus_slow = p->step_chara / 2;
	p->step_virus_fast = p->step_chara / 1;

	p->virus_sx        = N_LM2_VIRUS_SX;
	p->virus_sy        = N_LM2_VIRUS_SY;
	p->virus_max       = N_LM2_VIRUS_MAX;
	p->chara_fire_min  = N_LM2_CHARA_FIRE_MIN;
	p->chara_fire_max  = N_LM2_CHARA_FIRE_MAX;
	p->chara_fire_rnd  = N_LM2_CHARA_FIRE_RND;
	p->virus_fire_rnd  = N_LM2_VIRUS_FIRE_RND;


	// System

	p->color = n_bmp_black;


	p->reset       = TRUE;
	p->reset_timer = 0;

	n_lm2_chara_draw_blend = 1.0;
	p->transition_phase    = N_LM2_TRANSITION_FADE_IN;


	n_lm2_settings( p, TRUE );


#ifdef N_POSIX_PLATFORM_WINDOWS

	n_XInput_init( &p->xinput, 0 );

	n_XInput_vk2udlr_default( &p->xinput );

	n_XInput_vk2button( &p->xinput, VK_SPACE, 0 );
	n_XInput_vk2button( &p->xinput, VK_SPACE, 1 );
	n_XInput_vk2button( &p->xinput, VK_SPACE, 2 );
	n_XInput_vk2button( &p->xinput, VK_SPACE, 3 );
	n_XInput_vk2button( &p->xinput, VK_SPACE, 4 );
	n_XInput_vk2button( &p->xinput, VK_SPACE, 5 );

#endif



	// Resources

	n_memory_zero( p->bmp, sizeof( n_bmp ) * N_LM2_BMP_ALL );

#ifdef N_POSIX_PLATFORM_MAC
	n_mac_image_rc_load_bmp( @"rc/chara"  , &p->bmp[ 0 ] ); n_bmp_mac_color( &p->bmp[ 0 ] );
	n_mac_image_rc_load_bmp( @"rc/hiscore", &p->bmp[ 1 ] ); n_bmp_mac_color( &p->bmp[ 1 ] );
	n_mac_image_rc_load_bmp( @"rc/msg"    , &p->bmp[ 2 ] ); n_bmp_mac_color( &p->bmp[ 2 ] );
#else
	n_win64_resource_load_nbmp( n_posix_literal( "N_LM2_BMP_0" ), &p->bmp[ 0 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_LM2_BMP_1" ), &p->bmp[ 1 ] );
	n_win64_resource_load_nbmp( n_posix_literal( "N_LM2_BMP_2" ), &p->bmp[ 2 ] );
#endif

	n_bmp_scaler_big( &p->bmp[ 0 ], p->zoom );
	n_bmp_scaler_big( &p->bmp[ 1 ], p->zoom );
	n_bmp_scaler_big( &p->bmp[ 2 ], p->zoom );


	n_gdi gdi; n_gdi_zero( &gdi );

	gdi.sy                 = p->unit;
	gdi.sx                 = 0;
	gdi.style              = 0;//N_GDI_SMOOTH;

	gdi.base_color_bg      = p->color;
	gdi.base_color_fg      = 0;
	gdi.base_style         = N_GDI_BASE_SOLID;

	gdi.frame_style        = N_GDI_FRAME_NOFRAME;

	gdi.icon               = n_posix_literal( "" );
	gdi.icon_index         = 0;
	gdi.icon_style         = N_GDI_ICON_DEFAULT;

	gdi.text               = NULL;
	gdi.text_font          = n_posix_literal( "Trebuchet MS" );
	gdi.text_size          = (n_type_real) p->unit * 0.8;
	gdi.text_color_main    = n_bmp_rgb_mac( 255,255,255 );
	gdi.text_style         = N_GDI_TEXT_MAC_NO_CROP;

	gdi.text_effect_style[ 0 ] = N_GDI_EFFECT_OUTLINE;
	gdi.text_effect_color[ 0 ] = n_bmp_rgb_mac(   0,200,255 );
	gdi.text_effect_param[ 0 ] = 1;

	p->gdi = gdi;


	{

		n_gdi gdi; n_gdi_zero( &gdi );

		gdi.sx                 = p->csx;
		gdi.sy                 = p->csy;
		gdi.style              = N_GDI_SMOOTH;

		gdi.base_color_bg      = n_bmp_argb_mac(  64, 0,  0,  0 );

		gdi.text               = n_posix_literal( "L I N E  M A S H E R  2" );
		gdi.text_font          = n_posix_literal( "Trebuchet MS" );
		gdi.text_size          = p->csx / n_posix_strlen( gdi.text ) * 2;
		gdi.text_color_main    = n_bmp_argb_mac( 255, 0,  0,  0 );

		gdi.text_effect_style[ 0 ] = N_GDI_EFFECT_OUTLINE_FOG;
		gdi.text_effect_color[ 0 ] = n_bmp_argb_mac( 255, 0,100,150 );
		gdi.text_effect_param[ 0 ] = 8;


		n_gdi_bmp( &gdi, &p->title_screen );

	}

	n_lm2_scanline_init( p );
	n_bmp_flush_transcopy( &p->title_screen_scanning_line, &p->title_screen );

	{
	

		n_gdi gdi; n_gdi_zero( &gdi );

		//gdi.base_color_bg      = n_bmp_white;

		gdi.text               = n_posix_literal( "Press any keys" );
		gdi.text_font          = n_posix_literal( "Trebuchet MS" );
		gdi.text_size          = p->csx / 40;
		gdi.text_color_main    = n_bmp_white;
//NSLog( @"%d", p->csx / 40 );

		n_gdi_bmp( &gdi, &p->title_screen_press_any_keys );

	}


	{
		n_gdi g = gdi;

		g.base_color_bg = n_bmp_black_invisible;

		n_bmp_ui_animated_counter_init( &p->animated_counter_hiscr, &g, 0 );
		n_bmp_ui_animated_counter_init( &p->animated_counter_score, &g, 0 );

		p->animated_counter_hiscr.figure = n_bmp_ui_animated_counter_figure_count( N_LM2_SCORE_MAX );
	}

//p->hiscore = 123456;
//p->animated_counter_hiscr.figure = n_bmp_ui_animated_counter_figure_count( p->hiscore );
//NSLog( @"%d", p->animated_counter_hiscr.figure );


	n_lm2_sound_init( p );


	n_lm2_gradient_refresh( p, p->color, n_bmp_rgb_mac( 0,100,150 ) );


	p->title_screen_onoff = TRUE;


	p->input_lock = TRUE;


	return;
}

void
n_lm2_loop( n_lm2 *p )
{
//NSLog( @"%d", p->input ); return;
//return;

	// Opening

	if (
		( p->title_screen_onoff )
		&&
		( p->title_screen_stop_movement == FALSE )
		&&
		( p->input_any_keys_onoff )
	)
	{
		n_lm2_sound( p, N_LM2_START_INDEX );

		n_bmp_new( &p->title_screen_transition_bmp_old, p->csx, p->csy );
		n_bmp_new( &p->title_screen_transition_bmp_new, p->csx, p->csy );

		n_bmp_flush_fastcopy( p->canvas, &p->title_screen_transition_bmp_old );

		p->title_screen_onoff = FALSE;

		n_lm2_reset( p );
		n_bmp_flush_fastcopy( &p->bmp[ N_LM2_BMPBG_INDEX ], p->canvas );

		p->title_screen_stop_movement = TRUE;
		n_lm2_loop ( p );
		p->title_screen_stop_movement = FALSE;

		n_bmp_flush_fastcopy( p->canvas, &p->title_screen_transition_bmp_new );

		p->title_screen_transition_onoff = TRUE;

		n_posix_sleep( 200 );
	}

	if ( p->title_screen_transition_onoff )
	{

		BOOL ret = n_bmp_ui_transition
		(
			 p->canvas,
			&p->title_screen_transition_bmp_old,
			&p->title_screen_transition_bmp_new,
			250 * 2,
			&p->title_screen_transition_percent,
			N_BMP_UI_TRANSITION_FADE
		);
		if ( ret )
		{
			p->title_screen_transition_onoff = FALSE;
			p->input_lock = FALSE;
		}

		p->refresh = TRUE;
		return;
	}


	// Control

/*
	if ( n_win_is_input( VK_CONTROL ) )
	{
		p->transition_phase = N_LM2_TRANSITION_FADE_OUT;
	}
*/

	if ( ( p->reset )&&( n_bmp_ui_timer( &p->reset_timer, 1000 ) ) )
	{
		p->reset = FALSE;

		n_lm2_vibrate_off( p );

		n_lm2_reset( p );

		if ( p->title_screen_onoff )
		{
			n_bmp_flush_transcopy( &p->title_screen              , p->canvas );
			n_bmp_flush_transcopy( &p->title_screen_scanning_line, p->canvas );
		} else {
			n_bmp_flush_fastcopy( &p->bmp[ N_LM2_BMPBG_INDEX ], p->canvas );
		}

		return;
	}


	// [Mechanism]
	//
	//	[Flow]
	//	Erase => Input => Draw
	//
	//	[Rendering Order]
	//	score => virus => virus_fire => effect => chara => chara_fire => item

	int i;


	// Erase : erase with previous position

	n_game_chara_bulk_erase( p->stars, N_LM2_STARS_MAX );


	if ( p->title_screen_onoff )
	{
		//
	} else {
		//n_bmp_box( &game.bmp, 0,0, p->csx,p->unit, game.color );
		n_bmp_fastcopy( &p->bmp[ N_LM2_BMPBG_INDEX ], p->canvas, 0,0, p->csx,p->unit, 0,0 );
	}


	n_game_chara_bulk_erase( p->virus     , p->virus_max );
	n_game_chara_bulk_erase( p->virus_fire, p->virus_max );


	n_lm2_effect_loop_erase( p );


	n_lm2_chara_erase( &p->chara );


	i = 0;
	n_posix_loop
	{//break;

		n_game_chara *cf = &p->chara_fire[ i ];

		if ( cf->data != N_LM2_FIRE_OFF )
		{

			n_game_chara_erase( cf );

			cf->y -= p->step_chara_fire;
			if ( cf->y < -p->unit ) { cf->data = N_LM2_FIRE_OFF; }

			if ( cf->data == N_LM2_FIRE_ON_DOUBLE ) { cf->data = N_LM2_FIRE_OFF; }

			if ( p->power >= N_LM2_POWER_GATLING )
			{

				n_type_real o = sin( ( 2 * M_PI ) * p->gatling[ i ] );
				n_type_real a = p->unit;

				if ( p->power >= N_LM2_POWER_STARDUST ) { a *= 1 + n_random_range( 2 ); }

				cf->x = p->chara.x + (n_type_gfx) ( a * o );

				p->gatling[ i ] += (n_type_real) 0.003 * p->level;

			}

		}

		i++;
		if ( i >= p->chara_fire_max ) { break; }
	}

	if ( p->delayed_margin_onoff )
	{

		p->delayed_margin_onoff = FALSE;

		int i = 0;
		n_posix_loop
		{

			n_game_chara *cf = &p->chara_fire[ i ];

			cf->srcx = p->unit * n_posix_min( 4, p->power );

			if ( p->power == N_LM2_POWER_LASER_1 )
			{
				cf->mx = p->zoom * 6;
				cf->my = p->zoom * 2;
			} else
			if ( p->power == N_LM2_POWER_LASER_2 )
			{
				cf->mx = p->zoom * 6;
				cf->my = p->zoom * 2;
			} else
			if ( p->power == N_LM2_POWER_GATLING )
			{
				cf->mx = p->zoom * 6;
				cf->my = p->zoom * 4;
			} else
			if ( p->power == N_LM2_POWER_STARDUST )
			{
				cf->mx = p->zoom * 3;
				cf->my = p->zoom * 3;
			}// else

			i++;
			if ( i >= p->chara_fire_max ) { break; }
		}

	}


	if ( p->item.data )
	{
		n_game_chara_erase( &p->item );
	}


	// Input

	if ( p->input_lock == FALSE )
	{
		n_lm2_input( p );
	}


	// Draw

	i = 0;
	n_posix_loop
	{//break;

		if ( p->title_screen_stop_movement ) { break; }

		if ( p->stars[ i ].data == N_LM2_STARS_FAST )
		{
			p->stars[ i ].y += p->zoom * 2;
		} else {
			p->stars[ i ].y += p->zoom * 1;
		}

		if ( p->stars[ i ].y >= ( p->csy + N_BMP_SY( p->stars[ i ].chara ) ) )
		{
			n_type_gfx x = n_random_range( N_BMP_SX( p->canvas ) );
			n_type_gfx y = N_BMP_SY( p->stars[ i ].chara );

			n_game_chara_pos( &p->stars[ i ], x, -y );
			n_game_chara_prv( &p->stars[ i ] );

			if ( 0 == n_random_range( N_LM2_STARS_RAND ) )
			{
				p->stars[ i ].data = N_LM2_STARS_FAST;
			} else {
				p->stars[ i ].data = N_LM2_STARS_SLOW;
			}
		}


		i++;
		if ( i >= N_LM2_STARS_MAX ) { break; }
	}

	n_game_chara_bulk_draw( p->stars, N_LM2_STARS_MAX );

	i = 0;
	n_posix_loop
	{//break;

		n_game_chara *c  = &p->chara;
		n_game_chara *v  = &p->virus     [ i ];
		n_game_chara *vf = &p->virus_fire[ i ];

		if ( v->data == N_LM2_VIRUS_STOP )
		{

			if ( 0 == n_random_range( p->virus_fire_tmr ) )
			{
				if ( p->title_screen_onoff == FALSE )
				{
					v->data = N_LM2_VIRUS_FIRE;
				}
			}

			v->srcx = p->unit * 0;
			n_lm2_chara_draw( v );

		} else
		if ( v->data == N_LM2_VIRUS_FIRE )
		{

			if ( vf->data == N_LM2_FIRE_OFF )
			{
				vf->data = N_LM2_FIRE_ON + n_random_range( p->virus_fire_level );
				n_game_chara_pos( vf, v->x, v->y );
			} else
			if ( vf->data == N_LM2_FIRE_ON_SLOW )
			{
				v ->srcx = p->unit * 1;
				vf->srcx = p->unit * 3;
				vf->mx   = p->zoom * 4;
				vf->my   = p->zoom * 4;
				vf->y += p->step_virus_slow;
			} else
			if ( vf->data == N_LM2_FIRE_ON_FAST )
			{
				v ->srcx = p->unit * 1;
				vf->srcx = p->unit * 3;
				vf->mx   = p->zoom * 4;
				vf->my   = p->zoom * 4;
				vf->y += p->step_virus_fast;
			} else
			if ( vf->data == N_LM2_FIRE_ON_TRICKY_SLOW )
			{ 

				v->srcx = p->unit * 1;

				if ( vf->x > c->x )
				{
					vf->x -= p->step_virus_slow;
				} else {
					vf->x += p->step_virus_slow;
				}

				vf->y += p->step_virus_slow;

				vf->srcx = ( p->unit * 3 ) - ( p->unit * ( vf->x & 1 ) );
				if ( vf->srcx == ( p->unit * 3 ) )
				{
					vf->mx   = p->zoom * 3;
					vf->my   = p->zoom * 3;
				} else {
					vf->mx   = p->zoom * 4;
					vf->my   = p->zoom * 4;
				}

			} else
			if ( vf->data == N_LM2_FIRE_ON_TRICKY_FAST )
			{ 

				v->srcx = p->unit * 1;

				if ( vf->x > c->x )
				{
					vf->x -= p->step_virus_slow;
				} else {
					vf->x += p->step_virus_slow;
				}

				vf->y += p->step_virus_fast;

				vf->srcx = ( p->unit * 3 ) - ( p->unit * ( vf->x & 1 ) );
				if ( vf->srcx == ( p->unit * 3 ) )
				{
					vf->mx   = p->zoom * 3;
					vf->my   = p->zoom * 3;
				} else {
					vf->mx   = p->zoom * 4;
					vf->my   = p->zoom * 4;
				}

			}


			if ( vf->y > p->csy )
			{

				v ->data = N_LM2_VIRUS_STOP;
				vf->data = N_LM2_FIRE_OFF;

			} else
			if (
				( p->debug_invincible == FALSE )
				&&
				( vf->y >= c->y )
				&&
				( n_game_chara_is_hit_offset( vf, c, p->hit_offset_x,p->hit_offset_y, 0,0 ) )
				&&
				( p->transition_phase == N_LM2_TRANSITION_NONE )
			)
			{

				// GAME OVER

				if ( p->power >= N_LM2_POWER_SHIELD )
				{

					n_lm2_sound( p, N_LM2_PHOOT_INDEX );

				} else {

					n_lm2_sound( p, N_LM2_BWOPP_INDEX );

					n_lm2_vibrate_on( p );

					p->transition_phase = N_LM2_TRANSITION_FADE_OUT;

					n_lm2_settings( p, FALSE );

				}

				n_lm2_effect_go( c );

			}


			n_lm2_chara_draw( v  );
			n_lm2_chara_draw( vf );

		} else
		if ( 0 == n_random_range( p->virus_fire_tmr ) )
		{

			// Revive

			v->data = N_LM2_VIRUS_STOP;

		}


		int ii = 0;
		while( v->data != N_LM2_VIRUS_DEAD )
		{//break;

			n_game_chara *cf = NULL;
			n_posix_loop
			{

				cf = &p->chara_fire[ ii ];
				ii++;

				if ( cf->data != N_LM2_FIRE_OFF ) { break; }
				if ( ii >= p->chara_fire_max ) { break; }
			}
			if ( ii >= p->chara_fire_max ) { break; }


			if ( n_game_chara_is_hit( cf, v ) )
			{

				n_lm2_sound( p, N_LM2_CHIPP_INDEX );

				n_lm2_effect_go( v );


				// Double Shot : one shot two viruses

				// [!] : laser is default behavior

				if ( p->power == N_LM2_POWER_NONE )
				{
					if ( cf->data == N_LM2_FIRE_ON )
					{
						cf->data = N_LM2_FIRE_ON_DOUBLE;
					} else {
						cf->data = N_LM2_FIRE_OFF;
					}
				}


				v ->data = N_LM2_VIRUS_DEAD;
				vf->data = N_LM2_FIRE_OFF;


				p->score++;
				if ( p->score >= N_LM2_SCORE_MAX ) 
				{
					p->score = N_LM2_SCORE_MAX;
				} else {
					p->score_counter++;
					if ( p->score_counter >= 100 )
					{
						p->score_counter = 0;
						n_lm2_levelup( p );
					}
				}

				if ( p->hiscore < p->score ) { p->hiscore = p->score; }

			} else
			if (
				( p->power >= N_LM2_POWER_LASER_2 )
				&&
				( vf->data != N_LM2_FIRE_OFF )
				&&
				( n_game_chara_is_hit( cf, vf ) )
			)
			{

				// Virus Fire Eraser

				if ( p->power < N_LM2_POWER_GATLING ) { cf->data = N_LM2_FIRE_OFF; }
				v ->data = N_LM2_VIRUS_STOP;
				vf->data = N_LM2_FIRE_OFF;

			}

		}


		i++;
		if ( i >= p->virus_max ) { break; }
	}


	n_lm2_effect_loop_draw( p );


	if (
		( p->transition_phase == N_LM2_TRANSITION_NONE )
		||
		( p->transition_phase == N_LM2_TRANSITION_FADE_IN )
	)
	{
		n_lm2_chara_draw( &p->chara );
	}


	i = 0;
	n_posix_loop
	{

		n_game_chara *cf = &p->chara_fire[ i ];

		if ( cf->data != N_LM2_FIRE_OFF )
		{
			n_lm2_chara_draw( cf );
		}

		i++;
		if ( i >= p->chara_fire_max ) { break; }
	}

	if ( p->item.data )
	{

		p->item.y += p->step_virus_slow;

		if ( p->item.y > p->csy )
		{
			p->item.data = FALSE;
			p->item.y    = -p->unit;
		}

		if ( n_game_chara_is_hit_offset( &p->item, &p->chara, p->hit_offset_x,p->hit_offset_y, 0,0 ) )
		{

			p->item.data = FALSE;
			p->item.y    = -p->unit;

			n_game_chara_erase( &p->item  );
			n_lm2_chara_draw  ( &p->chara );


			p->power++;
			p-> item.srcx = p->unit * n_posix_min( 4, p->power     );
			p->chara.srcx = p->unit * n_posix_min( 4, p->power - 1 );

			p->delayed_margin_onoff = TRUE;

			n_lm2_sound( p, N_LM2_WOOOW_INDEX );

		} else {

			n_lm2_chara_draw( &p->item );

		}

	}


	// [!] : this position is important for hiscore

	n_lm2_digitbitmap( p );


	if ( p->title_screen_onoff )
	{

		n_bmp_flush_transcopy( &p->title_screen, p->canvas );

		n_type_gfx sx = N_BMP_SX( &p->title_screen_press_any_keys );
		n_type_gfx sy = N_BMP_SY( &p->title_screen_press_any_keys );
		n_type_gfx  x = ( p->csx - sx ) / 2;
		n_type_gfx  y = ( p->csy - sy ) / 2; y += ( y / 2 );

		n_type_real step = 0.01;

		if ( p->title_screen_press_any_keys_updown )
		{
			p->title_screen_press_any_keys_blend -= step;
			if ( p->title_screen_press_any_keys_blend <= 0.0 )
			{
				p->title_screen_press_any_keys_updown = FALSE;
			}
		} else {
			p->title_screen_press_any_keys_blend += step;
			if ( p->title_screen_press_any_keys_blend >= 1.0 )
			{
				p->title_screen_press_any_keys_updown = TRUE;
			}
		}

		n_type_real blend = p->title_screen_press_any_keys_blend;
//NSLog( @"%f", blend );
		n_bmp_blendcopy( &p->title_screen_press_any_keys, p->canvas, 0,0,sx,sy, x,y, blend );

#ifdef N_POSIX_PLATFORM_WINDOWS
		p->input_any_keys_onoff = n_lm2_keystate_get( p, VK_SPACE );
#endif
	}


	p->refresh = TRUE;
	return;
}

void
n_lm2_exit( n_lm2 *p )
{

	int i = 0;
	n_posix_loop
	{

		n_bmp_free( &p->bmp[ i ] );

		i++;
		if ( i >= N_LM2_BMP_ALL ) { break; }
	}


#ifdef N_POSIX_PLATFORM_WINDOWS

	n_lm2_sound_exit( p );

	n_XInput_exit( &p->xinput );

#endif


	n_lm2_effect_exit();


	n_lm2_zero( p );


	return;
}


