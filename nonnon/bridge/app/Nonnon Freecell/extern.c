// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




//#define N_MEMORY_DEBUG


//#define N_FREECELL_DEBUG_FOUNDATION




#include "../../chara.c"


#include "../../../neutral/bmp/ui/transition.c"




#ifdef N_POSIX_PLATFORM_MAC


#include "../../click.c"

#include "cardgenerator_mac.c"


#else


#include "../../../win64/game_engine.c"

#include "../../../win64/resource.c"

#include "../../../win64/ui/window.c"
#include "../../../win64/ui/menu.c"


#include "../../sound.c"


#include "cardgenerator_win64.c"



#define n_bmp_rgb_mac n_bmp_rgb


#endif




// Constants

#define N_FREECELL_NOTHING         ( -1 )

#define N_FREECELL_SUIT_HEARTS     N_CARDGENERATOR_SUIT_HEARTS
#define N_FREECELL_SUIT_DIAMONDS   N_CARDGENERATOR_SUIT_DIAMONDS
#define N_FREECELL_SUIT_SPADES     N_CARDGENERATOR_SUIT_SPADES
#define N_FREECELL_SUIT_CLUBS      N_CARDGENERATOR_SUIT_CLUBS
#define N_FREECELL_SUIT_MAX        N_CARDGENERATOR_SUIT_MAX

#define N_FREECELL_CARD_UNIT       N_CARDGENERATOR_CARD_UNIT
#define N_FREECELL_CARD_ALL        N_CARDGENERATOR_CARD_ALL
#define N_FREECELL_PILE_SX         ( 8 )
#define N_FREECELL_PILE_SY         ( N_FREECELL_CARD_UNIT * 2 )
#define N_FREECELL_PILE_ALL        ( N_FREECELL_PILE_SX * N_FREECELL_PILE_SY )
#define N_FREECELL_TABLEAU         ( N_FREECELL_SUIT_MAX + N_FREECELL_PILE_ALL )

#define N_FREECELL_FADE_MSEC       ( 777 )

#define N_FREECELL_DEAL_PHASE_0    0
#define N_FREECELL_DEAL_PHASE_1    1
#define N_FREECELL_DEAL_PHASE_2    2

#define N_FREECELL_RESZ_PHASE_NONE 0
#define N_FREECELL_RESZ_PHASE_STOP 1

#define N_FREECELL_TIMER_ID_STYLE  ( 1 )

#define N_FREECELL_ANIMATION_STEP     ( 12 )
#define N_FREECELL_ANIMATION_INTERVAL ( 20 )

#define N_FREECELL_SHADOW_COUNT    ( N_FREECELL_SUIT_MAX + 1 )




typedef struct {

#ifdef N_POSIX_PLATFORM_MAC

	NonnonGame       *self;

	NSOperationQueue *queue;
	n_bmp             canvas_base; // [!] : main body
	n_bmp             canvas_alias; // [!] : from NSBitmapImageRep
	NSBitmapImageRep *rep;

	BOOL              on_resize;

#else

	n_game_sound      sound[ 2 ];

#endif

	HWND              hwnd; // [!] : Mac : Stub : see "chara.c"

	n_bmp            *canvas;
	BOOL              refresh;

#ifdef _H_NONNON_WIN32_GAME_CLICK
	n_game_click      doubleclick;
#else
	BOOL              doubleclick;
#endif

	n_type_gfx        sx;
	n_type_gfx        sy;

	BOOL              is_first;

	int               click_phase;
	u32               click_msec;

	n_cardgenerator   cardgen;

	u32               color_bg;
	u32               color_gradient_upper;
	u32               color_gradient_lower;
	u32               color_halo;
	u32               color_halo_focus;
	u32               color_halo_current;

	BOOL              splash_onoff;

	BOOL              is_done;

	n_bmp             bmp_bg;
	n_bmp             bmp_freecell;
	n_bmp             bmp_stock[ N_FREECELL_SUIT_MAX ];

	BOOL              transition_onoff;
	n_bmp             transition_bmp_old;
	n_bmp             transition_bmp_new;
	double            transition_percent;
	int               transition_type;
	int               transition_stage;

	BOOL              animation_onoff;
	int               animation_index_f;
	int               animation_index_t;
	n_type_real       animation_base_x;
	n_type_real       animation_base_y;
	n_type_real       animation_step_x;
	n_type_real       animation_step_y;
	int               animation_server[ N_FREECELL_SUIT_MAX ];

	BOOL              redraw  [ N_FREECELL_PILE_SX  ];
	n_game_chara      freecell[ N_FREECELL_SUIT_MAX ];
	n_game_chara      stock   [ N_FREECELL_SUIT_MAX ];
	n_game_chara      tableau [ N_FREECELL_TABLEAU  ];
	n_game_chara      bottom  [ N_FREECELL_PILE_SX  ];
	n_game_chara      restore [ N_FREECELL_PILE_SY  ];
	int               drag_index;
	int               restore_count;
	int               freespace;

	int               shuffle[ N_FREECELL_CARD_ALL ];

	BOOL              shadow_global_onoff;
	BOOL              shadow_onoff;
	n_bmp            *shadow_bmp;
	n_bmp             shadow_bmp_cache[ N_FREECELL_SHADOW_COUNT ];
	n_type_gfx        shadow_x;
	n_type_gfx        shadow_y;
	n_type_gfx        shadow_sx;
	n_type_gfx        shadow_sy;
	n_type_gfx        shadow_px;
	n_type_gfx        shadow_py;
	n_type_gfx        shadow_psx;
	n_type_gfx        shadow_psy;

	BOOL              debug_onoff;
	int               debug_automove_count;

} n_freecell;


#define n_freecell_zero( p ) n_memory_zero( p, sizeof( n_freecell ) )




#define n_freecell_pos2index( x, y ) ( N_FREECELL_SUIT_MAX + ( x ) + ( N_FREECELL_PILE_SX * ( y ) ) )

#define n_freecell_index_upper( i ) ( ( i ) * N_FREECELL_PILE_SX )




BOOL
n_freecell_is_hovered_offset( n_freecell *p, n_type_gfx x, n_type_gfx y, n_type_gfx sx, n_type_gfx sy )
{

	BOOL ret = FALSE;

#ifdef N_POSIX_PLATFORM_MAC
	NSPoint pt = n_game_chara_cursor_position;
#else
	  POINT pt = n_win64_cursor_position_relative_POINT( p->hwnd );
#endif

	if (
		( ( pt.x > x )&&( pt.x < ( x + sx ) ) )
		&&
		( ( pt.y > y )&&( pt.y < ( y + sy ) ) )
	)
	{
		ret = TRUE;
	}


	return ret;
}


