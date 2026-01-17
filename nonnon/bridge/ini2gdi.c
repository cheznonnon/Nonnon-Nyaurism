// Nonnon INI2GDI
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_PROJECT_INI2GDI
#define _H_NONNON_PROJECT_INI2GDI




#include "../neutral/ini.c"
#include "../neutral/string_path.c"

#include "../bridge/gdi.c"




#define N_INI2GDI_CCH ( 1024 )




typedef struct {

	n_posix_char *name;
	u32          value;

} n_ini2gdi_option;




n_ini2gdi_option n_ini2gdi_option_style[] = {

	{ n_posix_literal( "default"     ), N_GDI_DEFAULT     },
	{ n_posix_literal( "gray"        ), N_GDI_GRAY        },
	{ n_posix_literal( "clarity"     ), N_GDI_CLARITY     },
	{ n_posix_literal( "nomargin"    ), N_GDI_NOMARGIN    },
	{ n_posix_literal( "automargin"  ), N_GDI_AUTOMARGIN  },
	{ n_posix_literal( "pressed"     ), N_GDI_PRESSED     },
	{ n_posix_literal( "systemcolor" ), N_GDI_SYSTEMCOLOR },
	{ n_posix_literal( "shadow"      ), N_GDI_SHADOW      },
	{ n_posix_literal( "shadow_fog"  ), N_GDI_SHADOW_FOG  },
	{ n_posix_literal( "contour"     ), N_GDI_CONTOUR     },
	{ n_posix_literal( "contour_fog" ), N_GDI_CONTOUR_FOG },
	{ n_posix_literal( "sink"        ), N_GDI_SINK        },
	{ n_posix_literal( "smooth"      ), N_GDI_SMOOTH      },
	{ n_posix_literal( ""            ), 0                 }

};


n_ini2gdi_option n_ini2gdi_option_layout[] = {

	{ n_posix_literal( "horizontal" ), N_GDI_LAYOUT_HORIZONTAL },
	{ n_posix_literal( "vertical"   ), N_GDI_LAYOUT_VERTICAL   },
	{ n_posix_literal( ""           ), 0                       }

};


n_ini2gdi_option n_ini2gdi_option_align[] = {

	{ n_posix_literal( "center" ), N_GDI_ALIGN_CENTER },
	{ n_posix_literal( "left"   ), N_GDI_ALIGN_LEFT   },
	{ n_posix_literal( "right"  ), N_GDI_ALIGN_RIGHT  },
	{ n_posix_literal( "top"    ), N_GDI_ALIGN_TOP    },
	{ n_posix_literal( "bottom" ), N_GDI_ALIGN_BOTTOM },
	{ n_posix_literal( ""       ), 0                  }

};


n_ini2gdi_option n_ini2gdi_option_base[] = {

	{ n_posix_literal( "solid"         ), N_GDI_BASE_SOLID         },
	{ n_posix_literal( "vertical"      ), N_GDI_BASE_VERTICAL      },
	{ n_posix_literal( "horizontal"    ), N_GDI_BASE_HORIZONTAL    },
	{ n_posix_literal( "rectangle"     ), N_GDI_BASE_RECTANGLE     },
	{ n_posix_literal( "circle"        ), N_GDI_BASE_CIRCLE        },
	{ n_posix_literal( "polkadot"      ), N_GDI_BASE_POLKADOT      },
	{ n_posix_literal( "polkadot_pop"  ), N_GDI_BASE_POLKADOT_POP  },
	{ n_posix_literal( "polkadot_rnd"  ), N_GDI_BASE_POLKADOT_RND  },
	{ n_posix_literal( "polkadot_foam" ), N_GDI_BASE_POLKADOT_FOAM },
	{ n_posix_literal( "rect_tile"     ), N_GDI_BASE_RECT_TILE     },
	{ n_posix_literal( "gingham"       ), N_GDI_BASE_GINGHAM       },
	{ n_posix_literal( "tartan"        ), N_GDI_BASE_TARTAN        },
	{ n_posix_literal( "stripe"        ), N_GDI_BASE_STRIPE        },
	{ n_posix_literal( "checker"       ), N_GDI_BASE_CHECKER       },
	{ n_posix_literal( "luna"          ), N_GDI_BASE_LUNA          },
	{ n_posix_literal( "luna_press"    ), N_GDI_BASE_LUNA_PRESS    },
	{ n_posix_literal( "image"         ), N_GDI_BASE_IMAGE         },
	{ n_posix_literal( "image_tile"    ), N_GDI_BASE_IMAGE_TILE    },
	{ n_posix_literal( ""              ), 0                        }

};


n_ini2gdi_option n_ini2gdi_option_frame[] = {

	{ n_posix_literal( "noframe" ), N_GDI_FRAME_NOFRAME     },
	{ n_posix_literal( "simple"  ), N_GDI_FRAME_SIMPLE      },
	{ n_posix_literal( "flat"    ), N_GDI_FRAME_FLAT        },
	{ n_posix_literal( "button"  ), N_GDI_FRAME_BUTTON      },
	{ n_posix_literal( "scroll"  ), N_GDI_FRAME_SCROLLARROW },
	{ n_posix_literal( "push"    ), N_GDI_FRAME_PUSH        },
	{ n_posix_literal( "sink"    ), N_GDI_FRAME_SINK        },
	{ n_posix_literal( "etch"    ), N_GDI_FRAME_ETCH        },
	{ n_posix_literal( "luna"    ), N_GDI_FRAME_LUNA        },
	{ n_posix_literal( "round"   ), N_GDI_FRAME_ROUND       },
	{ n_posix_literal( "rpg"     ), N_GDI_FRAME_RPG         },
	{ n_posix_literal( "aqua"    ), N_GDI_FRAME_AQUA        },
	{ n_posix_literal( ""        ), 0                       }

};


n_ini2gdi_option n_ini2gdi_option_icon[] = {

	{ n_posix_literal( "default"     ), N_GDI_ICON_DEFAULT     },
	{ n_posix_literal( "shadow"      ), N_GDI_ICON_SHADOW      },
	{ n_posix_literal( "shadow_fog"  ), N_GDI_ICON_SHADOW_FOG  },
	{ n_posix_literal( "contour"     ), N_GDI_ICON_CONTOUR     },
	{ n_posix_literal( "contour_fog" ), N_GDI_ICON_CONTOUR_FOG },
	{ n_posix_literal( "sink"        ), N_GDI_ICON_SINK        },
	{ n_posix_literal( "smooth"      ), N_GDI_ICON_SMOOTH      },
	{ n_posix_literal( "stretch"     ), N_GDI_ICON_STRETCH     },
	{ n_posix_literal( ""            ), 0                      }

};


n_ini2gdi_option n_ini2gdi_option_text[] = {

	{ n_posix_literal( "default"     ), N_GDI_TEXT_DEFAULT     },
	{ n_posix_literal( "bold"        ), N_GDI_TEXT_BOLD        },
	{ n_posix_literal( "italic"      ), N_GDI_TEXT_ITALIC      },
	{ n_posix_literal( "underline"   ), N_GDI_TEXT_UNDERLINE   },
	{ n_posix_literal( "strikeout"   ), N_GDI_TEXT_STRIKEOUT   },
	{ n_posix_literal( "monospace"   ), N_GDI_TEXT_MONOSPACE   },
	{ n_posix_literal( "shadow"      ), N_GDI_TEXT_SHADOW      },
	{ n_posix_literal( "shadow_fog"  ), N_GDI_TEXT_SHADOW_FOG  },
	{ n_posix_literal( "contour"     ), N_GDI_TEXT_CONTOUR     },
	{ n_posix_literal( "contour_fog" ), N_GDI_TEXT_CONTOUR_FOG },
	{ n_posix_literal( "sink"        ), N_GDI_TEXT_SINK        },
	{ n_posix_literal( "smooth"      ), N_GDI_TEXT_SMOOTH      },
	{ n_posix_literal( "clear"       ), N_GDI_TEXT_CLEAR       },
	{ n_posix_literal( "gradient"    ), N_GDI_TEXT_GRADIENT    },
	{ n_posix_literal( ""            ), 0                      }

};




int
n_ini2gdi_str2val( const n_posix_char *str, int index )
{

	if ( n_string_is_empty( str ) ) { return 0; }


	// [Mechanism]
	//
	//	string : "255 255 255"
	//	index  :  [0] [1] [2]


	BOOL onoff = FALSE;


	int count = 0;
	int ret   = 0;


	int i = 0;
	n_posix_loop
	{

		if (
			( n_string_is_digit( str, i ) )
			||
			( str[ i ] == n_posix_literal( '+' ) )
			||
			( str[ i ] == n_posix_literal( '-' ) )
		)
		{

			onoff = TRUE;

			if ( index == count )
			{
				ret = n_posix_atoi( &str[ i ] );
				break;
			}

		} else {

			if ( onoff )
			{
				onoff = FALSE;
				count++;
			}

		}


		i++;
		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	if ( index == -1 ) { ret = 1 + count; }


	return ret;
}

int
n_ini2gdi_str2strval( const n_posix_char *arg, n_posix_char *ret, int ret_len )
{

	// [Mechanism]
	//
	//	"arg"     : "string,12"
	//	"ret"     : "string"
	//	"ret_len" : NUL-included character count
	//	return    : 12


	if ( n_string_is_empty( arg ) )
	{

		n_string_truncate( ret );

		return 0;
	}


	n_string_copy( arg, ret );


	int val;


	int i = 0;
	n_posix_loop
	{

		if ( ret[ i ] == N_STRING_CHAR_COMMA )
		{

			if ( ret != NULL )
			{
				n_string_terminate( ret, i );
			}

			val = n_posix_atoi( &ret[ i + 1 ] );

			break;
		}


		i++;
		if ( ret[ i ] == N_STRING_CHAR_NUL )
		{

			val = n_posix_atoi( ret );

			break;
		}
	}


	return val;
}

void
n_ini2gdi_str2rgb( const n_posix_char *str, u32 *ret1, u32 *ret2, u32 *ret3, u32 *ret4 )
{

	int i = 0;
	n_posix_loop
	{

		int a = n_ini2gdi_str2val( str, i ); i++;
		int r = n_ini2gdi_str2val( str, i ); i++;
		int g = n_ini2gdi_str2val( str, i ); i++;
		int b = n_ini2gdi_str2val( str, i ); i++;

		if ( i == ( 4 * 1 ) ) { if ( ret1 != NULL ) { (*ret1) = n_bmp_argb( a,r,g,b ); } } else
		if ( i == ( 4 * 2 ) ) { if ( ret2 != NULL ) { (*ret2) = n_bmp_argb( a,r,g,b ); } } else
		if ( i == ( 4 * 3 ) ) { if ( ret3 != NULL ) { (*ret3) = n_bmp_argb( a,r,g,b ); } } else
		if ( i == ( 4 * 4 ) ) { if ( ret4 != NULL ) { (*ret4) = n_bmp_argb( a,r,g,b ); } } else
		if ( i >= ( 4 * 5 ) ) { break; }
	}


	return;
}

u32
n_ini2gdi_str2option( const n_ini2gdi_option *o, const n_posix_char *str )
{

	if ( o == NULL ) { return 0; }

	if ( n_string_is_empty( str ) ) { return 0; }


	u32 ret = 0;


	int i = 0;
	n_posix_loop
	{

		int ii = 0;
		n_posix_loop
		{

			if ( n_string_match( &str[ i ], o[ ii ].name ) )
			{

				int cch = (int) n_posix_strlen( o[ ii ].name );

				if (
					( str[ i + cch ] == N_STRING_CHAR_NUL   )
					||
					( str[ i + cch ] == N_STRING_CHAR_SPACE )
				)
				{

					ret |= o[ ii ].value;
					i += cch;

					// [Patch] : prevent buffer overflow
					i--;

					break;
				}

			}

			ii++;
			if ( n_string_is_empty( o[ ii ].name ) ) { break; }
		}


		i++;
		if ( str[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return ret;
}

BOOL
n_ini2gdi_load_single( const n_posix_char *abspath, const n_posix_char *section, n_bmp *b, n_gdi *gdi_ret )
{

	if ( FALSE == n_posix_stat_is_file( abspath ) ) { return TRUE; }

	if ( n_string_is_empty( section ) ) { return TRUE; }


	// Init

	n_ini ini; n_ini_zero( &ini ); n_ini_load( &ini, (void*) abspath );
	n_gdi gdi; n_gdi_zero( &gdi );

	n_posix_char  str[ N_INI2GDI_CCH ];
	n_posix_char base[ N_INI2GDI_CCH ];
	n_posix_char text[ N_INI2GDI_CCH ];
	n_posix_char icon[ N_INI2GDI_CCH ];
	n_posix_char clip[ N_INI2GDI_CCH ];
	n_posix_char font[ N_INI2GDI_CCH ];

	n_posix_char *lval   = NULL;
	n_posix_char *defval = NULL;


	// Global

	lval   = n_posix_literal( "size" );
	defval = n_posix_literal( "0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.sx = n_ini2gdi_str2val( str, 0 );
	gdi.sy = n_ini2gdi_str2val( str, 1 );


	lval   = n_posix_literal( "style" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.style = N_GDI_TEXTLOADER | n_ini2gdi_str2option( n_ini2gdi_option_style, str );


	lval   = n_posix_literal( "layout" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.layout = n_ini2gdi_str2option( n_ini2gdi_option_layout, str );


	lval   = n_posix_literal( "align" );
	defval = n_posix_literal( "center" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.align = n_ini2gdi_str2option( n_ini2gdi_option_align, str );


	lval   = n_posix_literal( "scale" );
	defval = n_posix_literal( "1" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.scale = n_posix_atoi( str );


	// Base

	lval   = n_posix_literal( "base" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );
	n_string_replace( str, str, N_STRING_DQUOTE, N_STRING_EMPTY );

	gdi.base_index = n_ini2gdi_str2strval( str, base, N_INI2GDI_CCH );
	gdi.base       = base;


	lval   = n_posix_literal( "base_color" );
	defval = n_posix_literal( "255   0 200 255, 255 255 255 255" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.base_color_bg, &gdi.base_color_fg, NULL, NULL );


	lval   = n_posix_literal( "base_style" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.base_style = n_ini2gdi_str2option( n_ini2gdi_option_base, str );


	lval   = n_posix_literal( "base_unit" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.base_unit = n_posix_atoi( str );


	lval   = n_posix_literal( "base_margin" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.base_polkadot_margin = n_posix_atoi( str );
//n_posix_debug_literal( "%d", gdi.base_polkadot_margin );


	// Frame

	lval   = n_posix_literal( "frame_style" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.frame_style = n_ini2gdi_str2option( n_ini2gdi_option_frame, str );


	lval   = n_posix_literal( "frame_fxsize" );
	defval = n_posix_literal( "4" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.frame_round = n_ini2gdi_str2val( str, 0 );


	lval   = n_posix_literal( "frame_corner_color" );
	defval = n_posix_literal( "0 255 255 255" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.frame_corner_color, NULL, NULL, NULL );
/*
n_posix_debug_literal
(
	"%d %d %d %d",
	n_bmp_a( gdi.frame_corner_color ),
	n_bmp_r( gdi.frame_corner_color ),
	n_bmp_g( gdi.frame_corner_color ),
	n_bmp_b( gdi.frame_corner_color )
);
*/

	// Icon

	lval   = n_posix_literal( "icon" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );
	n_string_replace( str, str, N_STRING_DQUOTE, N_STRING_EMPTY );

	gdi.icon_index = n_ini2gdi_str2strval( str, icon, N_INI2GDI_CCH );
	gdi.icon       = icon;

#ifdef N_POSIX_PLATFORM_WINDOWS

	if ( n_string_is_same_literal( "%EXENAME%", icon ) )
	{
		n_win_exepath( icon, N_INI2GDI_CCH );
	}

#endif // #ifdef N_POSIX_PLATFORM_WINDOWS


	lval   = n_posix_literal( "icon_color_shadow" );
	defval = n_posix_literal( "255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.icon_color_shadow, NULL, NULL, NULL );


	lval   = n_posix_literal( "icon_color_contour" );
	defval = n_posix_literal( "255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.icon_color_contour, NULL, NULL, NULL );


	lval   = n_posix_literal( "icon_color_sink" );
	defval = n_posix_literal( "255 0 0 0, 255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.icon_color_sink_tl, &gdi.icon_color_sink_br, NULL, NULL );


	lval   = n_posix_literal( "icon_style" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.icon_style = N_GDI_ICON_IMAGELOADER | n_ini2gdi_str2option( n_ini2gdi_option_icon,  str );


	lval   = n_posix_literal( "icon_size" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.icon_sx = n_ini2gdi_str2val( str, 0 );
	gdi.icon_sy = n_ini2gdi_str2val( str, 1 );


	lval   = n_posix_literal( "icon_clip" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );
	n_string_replace( str, clip, N_STRING_DQUOTE, N_STRING_EMPTY );

	gdi.icon_clip   = clip;
	gdi.icon_clip_x = n_ini2gdi_str2val( str, 0 );
	gdi.icon_clip_y = n_ini2gdi_str2val( str, 1 );
/*
if ( FALSE == n_string_is_empty( gdi.icon_clip ) )
{
	n_posix_debug_literal( "%d %d", gdi.icon_clip_x, gdi.icon_clip_y );
}
*/


	// Text

	lval   = n_posix_literal( "text" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, text, N_INI2GDI_CCH );


	lval   = n_posix_literal( "text_font" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.text       = text;
	gdi.text_size  = n_ini2gdi_str2strval( str, font, N_INI2GDI_CCH );
	gdi.text_font  = font;

	if ( gdi.text_size == 0 ) { gdi.text_size = 14; }


	lval   = n_posix_literal( "text_color" );
	defval = n_posix_literal( "255 0 0 0, 255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.text_color_main, &gdi.text_color_gradient, NULL, NULL );


	lval   = n_posix_literal( "text_color_shadow" );
	defval = n_posix_literal( "255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.text_color_shadow, NULL, NULL, NULL );


	lval   = n_posix_literal( "text_color_contour" );
	defval = n_posix_literal( "255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.text_color_contour, NULL, NULL, NULL );


	lval   = n_posix_literal( "text_color_sink" );
	defval = n_posix_literal( "255 0 0 0, 255 0 0 0" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	n_ini2gdi_str2rgb( str, &gdi.text_color_sink_tl, &gdi.text_color_sink_br, NULL, NULL );


	lval   = n_posix_literal( "text_style" );
	defval = n_posix_literal( "" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.text_style = n_ini2gdi_str2option( n_ini2gdi_option_text,  str );


	// Effect

	lval   = n_posix_literal( "icon_fxsize" );
	defval = n_posix_literal( "1 1" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.icon_fxsize1 = n_ini2gdi_str2val( str, 0 );
	gdi.icon_fxsize2 = n_ini2gdi_str2val( str, 1 );


	lval   = n_posix_literal( "text_fxsize" );
	defval = n_posix_literal( "1 1" );

	n_ini_value_str( &ini, section, lval, defval, str, N_INI2GDI_CCH );

	gdi.text_fxsize1 = n_ini2gdi_str2val( str, 0 );
	gdi.text_fxsize2 = n_ini2gdi_str2val( str, 1 );


	// Cleanup

	n_ini_free( &ini );


	// Debug

/*

n_posix_debug_literal
(

	"sy             %d \n"
	"sx             %d \n"
	"style          %d \n"

	"base_color_bg  %x \n"
	"base_color_fg  %x \n"
	"base_style     %d \n"

	"frame_style    %x \n"

	"icon           %s \n"
	"icon_index     %d \n"
	"icon_style     %d \n"
	"icon_fxsize    %d \n"

	"text_font      %s \n"
	"text_size      %d \n"
	"text_style     %d \n"
	"text_fxsize    %d \n"

	"",

	(int) gdi.sx,
	(int) gdi.sy,
	(int) gdi.style,
	(int) gdi.layout,

	(int) gdi.base_color_bg,
	(int) gdi.base_color_fg,
	(int) gdi.base_style,

	(int) gdi.frame_style,

	      gdi.icon,
	(int) gdi.icon_index,
	(int) gdi.icon_style,
	(int) gdi.icon_fxsize,

	      gdi.text_font,
	(int) gdi.text_size,
	(int) gdi.text_style
	(int) gdi.text_fxsize,

);

*/


	{

		// [!] : "pth" : for backward compatibility : n_string_path_*() needs a copy

		n_posix_char *dir = n_string_path_folder_current_new();
		n_posix_char *pth = n_string_path_carboncopy( abspath );

		n_string_path_folder_change( pth );


		n_bmp bmp; n_bmp_zero( &bmp );
		n_gdi_bmp( &gdi, &bmp );

		if ( gdi_ret != NULL ) { n_gdi_alias( &gdi, gdi_ret ); }


		n_string_path_folder_change( dir );

		n_string_path_free( dir );
		n_string_path_free( pth );


		if ( NULL == N_BMP_PTR( &bmp ) )
		{
//n_posix_debug_literal( "n_gdi_bmp()" );

			return TRUE;
		}


		n_bmp_free_fast( b );
		n_bmp_alias( &bmp, b );

	}


	return FALSE;
}

BOOL
n_ini2gdi_load( const n_posix_char *abspath, n_bmp *b )
{

	if ( FALSE == n_posix_stat_is_file( abspath ) ) { return TRUE; }


	const int maxcount = 32;

	typedef struct {

		n_gdi        gdi;
		n_bmp        bmp;
		n_type_gfx   x,y;
		n_type_real  blend;
		int          rotate;
		u32          color_bg;

	} n_ini2gdi_layer;

	n_ini2gdi_layer layer[ 32 ];
	n_memory_zero( layer, sizeof( n_ini2gdi_layer ) * maxcount );


	n_ini ini; n_ini_zero( &ini ); n_ini_load( &ini, (void*) abspath );
	if ( ini.newline == N_TXT_NEWLINE_BINARY )
	{
		n_ini_free( &ini );
		return TRUE;
	}


	const n_posix_char *section = n_posix_literal( "Nonnon INI2GDI Layer" );

	if ( FALSE == n_ini_section_chk( &ini, section ) )
	{
		n_ini_free( &ini );

		return n_ini2gdi_load_single( abspath, n_posix_literal( "Nonnon INI2GDI" ), b, NULL );
	}


	int count =  0;
	int first = -1;
	int i     =  0;
	n_posix_loop
	{

		n_posix_char lval   [ N_INI2GDI_CCH ]; n_posix_sprintf_literal( lval, "%d", i );
		n_posix_char defval [ N_INI2GDI_CCH ]; n_string_zero( defval, N_INI2GDI_CCH );
		n_posix_char label  [ N_INI2GDI_CCH ]; n_string_zero(  label, N_INI2GDI_CCH );
		n_posix_char ret    [ N_INI2GDI_CCH ]; n_string_zero(    ret, N_INI2GDI_CCH );

		n_ini_value_str( &ini, section, lval, defval, label, N_INI2GDI_CCH );

		// [!] : comment-out support : pass through when not exist

		if (
			( FALSE == n_string_is_empty( label ) )
			&&
			( FALSE == n_ini2gdi_load_single( abspath, label, &layer[ i ].bmp, &layer[ i ].gdi ) )
		)
		{

			count++;

			if ( first == -1 ) { first = i; }


			n_posix_sprintf_literal(   lval, "layer" );
			n_posix_sprintf_literal( defval, "0 0 0 0" );

			n_ini_value_str( &ini, label, lval, defval, ret, N_INI2GDI_CCH );

			layer[ i ].x      = n_ini2gdi_str2val( ret, 0 );
			layer[ i ].y      = n_ini2gdi_str2val( ret, 1 );
			layer[ i ].blend  = n_ini2gdi_str2val( ret, 2 );
			layer[ i ].rotate = n_ini2gdi_str2val( ret, 3 );


			n_posix_sprintf_literal(   lval, "base_color" );
			n_posix_sprintf_literal( defval, "0 255 255 255, 0 255 255 255" );

			n_ini_value_str( &ini, label, lval, defval, ret, N_INI2GDI_CCH );

			n_ini2gdi_str2rgb( ret, &layer[ i ].color_bg, NULL, NULL, NULL );
//n_posix_debug_literal( " %x ", layer[ i ].color_bg );

		}

		i++;
		if ( i >= maxcount ) { break; }
	}

	n_ini_free( &ini );


	if ( first == -1 ) { return TRUE; }


	n_bmp *t = &layer[ first ].bmp;

	i = first + 1;
	n_posix_loop
	{

		if ( i >= maxcount ) { break; }


		n_bmp           *f = &layer[ i ].bmp;
		n_type_gfx       x =  layer[ i ].x;
		n_type_gfx       y =  layer[ i ].y;
		n_type_gfx      sx =  N_BMP_SX( f );
		n_type_gfx      sy =  N_BMP_SY( f );
		n_type_real  blend = (n_type_real) layer[ i ].blend / 100;
		n_type_real rotate = (n_type_real) layer[ i ].rotate;
		u32             bg = layer[ i ].color_bg;

		if ( rotate != 0.0 )
		{

			// [!] : centering

			n_type_gfx psx = sx;
			n_type_gfx psy = sy;

			n_bmp_matrix_rotate( f, layer[ i ].rotate, bg, TRUE );

			sx = N_BMP_SX( f );
			sy = N_BMP_SX( f );
			 x = x + ( ( psx - sx ) / 2 );
			 y = y + ( ( psy - sy ) / 2 );

		}

		n_bmp_blendcopy( f,t, 0,0,sx,sy, x,y, blend );


		if ( layer[ i ].gdi.frame_style == N_GDI_FRAME_ROUND )
		{
			n_bmp_cornermask( t, layer[ i ].gdi.frame_round, 0, layer[ i ].gdi.frame_corner_color );
		}


		n_bmp_free( f );


		i++;

	}

	n_bmp_free_fast( b );
	n_bmp_alias( t, b );


	return FALSE;
}


#endif // _H_NONNON_PROJECT_INI2GDI


