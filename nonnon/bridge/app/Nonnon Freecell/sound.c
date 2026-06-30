// Nonon Freecell
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




#ifdef N_POSIX_PLATFORM_MAC


#define N_FREECELL_SOUND_CLICK   ( 1 )
#define N_FREECELL_SOUND_FANFARE ( 2 )


void
n_freecell_sound_init( n_freecell *p, HWND hwnd )
{
	return;
}

void
n_freecell_sound_exit( n_freecell *p )
{
	return;
}

// [!] : global variables : this is important to play

static n_mac_sound2 n_freecell_snd_click;
static n_mac_sound2 n_freecell_snd_fanfare;

void
n_freecell_sound_play( n_freecell *p, int id )
{

	if ( id == N_FREECELL_SOUND_CLICK )
	{
		n_mac_sound2_play( &n_freecell_snd_click );
	} else
	if ( id == N_FREECELL_SOUND_FANFARE )
	{
		n_mac_sound2_stop( &n_freecell_snd_fanfare );
		n_mac_sound2_play( &n_freecell_snd_fanfare );
	}

	return;
}


#else


void
n_freecell_sound_init( n_freecell *p, HWND hwnd )
{

	n_game_sound_init_literal( &p->sound[ 0 ], hwnd, "N_FREECELL_WAV_0" );
	n_game_sound_init_literal( &p->sound[ 1 ], hwnd, "N_FREECELL_WAV_1" );

	return;
}

void
n_freecell_sound_exit( n_freecell *p )
{

	n_game_sound_exit( &p->sound[ 0 ] );
	n_game_sound_exit( &p->sound[ 1 ] );

	return;
}

#define N_FREECELL_SOUND_CLICK   1
#define N_FREECELL_SOUND_FANFARE 2

void
n_freecell_sound_play( n_freecell *p, int id )
{
//return;

	if ( id == N_FREECELL_SOUND_CLICK )
	{
		n_game_sound_loop( &p->sound[ 0 ] );
	} else
	if ( id == N_FREECELL_SOUND_FANFARE )
	{
		n_game_sound_loop( &p->sound[ 1 ] );
	}


	return;
}


#endif

