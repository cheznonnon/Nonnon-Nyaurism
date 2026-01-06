// Nonnon MIDI
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [Mechanism]
//
//	Standard MIDI File (SMF) Format
//	BIG Endian formatted
//
//	compile from a script file
//	based on Music Macro Language (MML)


// [x] : Win2000 or earlier : Logging
//
//	a : fprintf() crashes when NULL
//	b : output .MID will be broken when using this macro below
//
//	#define n_midi_log( midi, ... ) n_posix_fprintf_literal( ( midi )->log_fp, ##__VA_ARGS__ )


// [x] : Mac cannot play .MID files
//
//	a special app is needed to play
//	search AppStore




#ifndef _H_NONNON_NEUTRAL_MIDI
#define _H_NONNON_NEUTRAL_MIDI




#include "./endian.c"
#include "./bmp/_codec.c"
#include "./bmp/_error.c"
#include "./string_path.c"
#include "./txt.c"




#define N_MIDI_NAME_MAX         256
#define N_MIDI_CHANNEL_MAX       16
#define N_MIDI_PARAM_MAX        128
#define N_MIDI_PROGRAM_NOT_USED N_MIDI_PARAM_MAX
#define N_MIDI_MAP_MAX          ( N_MIDI_CHANNEL_MAX * 2 )
#define N_MIDI_DATASIZE_OFFSET   18
#define N_MIDI_PANPOT_CENTER     64
#define N_MIDI_VOLUME_DEFAULT   100
#define N_MIDI_RESOLUTION_MIN    24
#define N_MIDI_TIMEBASE         ( 24 * 2 )
#define N_MIDI_RHYTHMCHANNEL      9
#define N_MIDI_CCH              ( 1024 )
#define N_MIDI_EXT              n_posix_literal( ".mid\0\0" )




typedef struct {

	n_posix_char author[ N_MIDI_NAME_MAX ];
	n_posix_char  title[ N_MIDI_NAME_MAX ];
	u8           volume[ N_MIDI_CHANNEL_MAX ];
	u8           panpot[ N_MIDI_CHANNEL_MAX ];
	u8            sound[ N_MIDI_CHANNEL_MAX ];
	int            step[ N_MIDI_CHANNEL_MAX ];
	u32          *note;
	int           note_count;
	int           repeat;


	// [!] : for logging

	n_posix_bool  log_onoff;
	n_posix_char  log_name[ N_MIDI_CCH ];
	n_txt         log;
	int           zero_pause;

} n_midi;




#define n_midi_log( h, f, ... ) n_midi_logging( h, n_posix_literal( f ), ##__VA_ARGS__ )

void
n_midi_logging( const n_midi *midi, const n_posix_char *format, ... )
{

	if ( midi->log_onoff == n_posix_false ) { return; }


	n_posix_char str[ N_MIDI_CCH ];


	va_list vl;


	va_start( vl, format );

	n_posix_vsprintf( str, format, vl );

	va_end( vl );


	n_txt_add( &midi->log, midi->log.sy, str );


	return;
}

#define n_midi_zero( midi ) n_memory_zero( midi, sizeof( n_midi ) )

n_posix_bool
n_midi_error( const n_midi *midi )
{

	if ( midi == NULL ) { return n_posix_true; }


	return n_posix_false;
}

u32
n_midi_note( u8 delta, u8 event, u8 channel, u8 p1, u8 p2 )
{

	u32 ret;


	ret  = ( (u32) delta           ) << 24;
	ret += ( (u32) event + channel ) << 16;
	ret += ( (u32) p1              ) <<  8;
	ret += ( (u32) p2              );


	return ret;
}

u32
n_midi_note_on( u32 note )
{

	// zero-clear "delta"

	return ( note & 0x00ffffff );
}

u32
n_midi_note_off( u32 note )
{

	// zero-clear "p2"

	return ( note & 0xffffff00 );
}

u8
n_midi_note_delta_get( u32 note )
{
	return (note & 0xff000000) >> 24;
}

u8
n_midi_note_event_get( u32 note )
{
	return (u8) ( ( note & 0x00f00000 ) >> 16 );
}

u8
n_midi_note_channel_get( u32 note )
{
	return (u8) ( ( note & 0x000f0000 ) >> 16 );
}

u8
n_midi_note_p1_get( u32 note )
{
	return ( note & 0x0000ff00 ) >>  8;
}

u8
n_midi_note_p2_get( u32 note )
{
	return ( note & 0x000000ff );
}

u32
n_midi_note_delta_set( u32 note, u8 delta )
{
	return ( note & 0x00ffffff ) | ( (u32) delta << 24 );
}

int
n_midi_token2note( n_posix_char *token, int p1 )
{

	if ( n_string_is_empty( token ) ) { return 0; }


	int i;
	int note = ( p1 + 1 ) * 12;


	i = 0;
	n_posix_loop
	{

		if ( token[ i ] == n_posix_literal( 'r' ) ) { note  =  0; break; } else
		if ( token[ i ] == n_posix_literal( 'c' ) ) { note +=  0; } else
		if ( token[ i ] == n_posix_literal( 'd' ) ) { note +=  2; } else
		if ( token[ i ] == n_posix_literal( 'e' ) ) { note +=  4; } else
		if ( token[ i ] == n_posix_literal( 'f' ) ) { note +=  5; } else
		if ( token[ i ] == n_posix_literal( 'g' ) ) { note +=  7; } else
		if ( token[ i ] == n_posix_literal( 'a' ) ) { note +=  9; } else
		if ( token[ i ] == n_posix_literal( 'b' ) ) { note += 11; } else
		if ( token[ i ] == n_posix_literal( '+' ) ) { note +=  1; } else
		if ( token[ i ] == n_posix_literal( '-' ) ) { note -=  1; }// else


		i++;
		if ( token[ i ] == N_STRING_CHAR_NUL ) { break; }
	}


	return note;
}

void
n_midi_token( const n_posix_char *token, n_posix_char *p1, n_posix_char *p2 )
{

	if ( p1 != NULL ) { n_string_truncate( p1 ); }
	if ( p2 != NULL ) { n_string_truncate( p2 ); }

	if ( n_string_is_empty( token ) ) { return; }


	int i;


	i = 0;
	n_posix_loop
	{

		if ( token[ i ] == n_posix_literal( '(' ) ) { break; }

		i++;
		if ( token[ i ] == N_STRING_CHAR_NUL ) { return; }
	}

	if ( p1 != NULL )
	{
		n_string_copy( &token[ i + 1 ], p1 );
		n_string_commentout( p1, p1, N_STRING_COMMA );
		n_string_remove_blank( p1, p1 );
		n_string_roundbracket_del( p1, p1 );
	}


	n_posix_loop
	{

		if ( token[ i ] == N_STRING_CHAR_COMMA ) { break; }

		i++;
		if ( token[ i ] == N_STRING_CHAR_NUL ) { return; }
	}


	if ( p2 != NULL )
	{
		n_string_copy( &token[ i + 1 ], p2 );
		n_string_remove_blank( p2, p2 );
		n_string_roundbracket_del( p2, p2 );
	}


	return;
}

#define n_midi_write_MThd(     ) n_endian_big( 0x4d546864,      4 ) // "MThd"
#define n_midi_write_six(      ) n_endian_big( 0x00000006,      4 )
#define n_midi_write_format(   ) n_endian_big( 0x00000001,      2 )
#define n_midi_write_track( )    n_endian_big( 0x00000001,      2 )
#define n_midi_write_timebase( ) n_endian_big( N_MIDI_TIMEBASE, 2 )
#define n_midi_write_MTrk(     ) n_endian_big( 0x4d54726b,      4 ) // "MTrk"
#define n_midi_write_eot(      ) n_endian_big( 0x60ff2f00,      4 )

#define n_midi_write_author( v ) n_endian_big( n_midi_note( 0, 0xff, 0,   2,   v ) , 4 )
#define n_midi_write_title(  v ) n_endian_big( n_midi_note( 0, 0xff, 0,   3,   v ) , 4 )
#define n_midi_write_volume( v ) n_endian_big( n_midi_note( 0, 0xb0, i,   7,   v ) , 4 )
#define n_midi_write_panpot( v ) n_endian_big( n_midi_note( 0, 0xb0, i,  10,   v ) , 4 )
#define n_midi_write_sound(  v ) n_endian_big( n_midi_note( 0, 0xc0, i,   v,   0 ) , 4 )

n_posix_bool
n_midi_write( const n_midi *midi, const n_posix_char *name )
{

	if ( n_midi_error( midi ) ) { return n_posix_true; }


	n_posix_bool debug_onoff = n_posix_false;//n_posix_true;


if ( debug_onoff ) { n_posix_debug_literal( " n_posix_fopen_write() " ); }

	FILE *fp = n_posix_fopen_write( name );
	if ( fp == NULL ) { return n_posix_true; }


	// [Mechanism]
	//
	//	header       : 14 byte
	//	track chunk  :  8 byte
	//	data         :  n byte
	//	end of track :  4 byte
	//
	// 	trk_1 : data size will be written later


if ( debug_onoff ) { n_posix_debug_literal( " Headers " ); }

	u32 hdr_0 = n_midi_write_MThd();
	u32 hdr_1 = n_midi_write_six();
	u16 hdr_2 = (u16) n_midi_write_format();
	u16 hdr_3 = (u16) n_midi_write_track();
	u16 hdr_4 = (u16) n_midi_write_timebase();
	u32 trk_0 = n_midi_write_MTrk();
	u32 trk_1 = 0;

	n_posix_fwrite( &hdr_0, 4, 1, fp );
	n_posix_fwrite( &hdr_1, 4, 1, fp );
	n_posix_fwrite( &hdr_2, 2, 1, fp );
	n_posix_fwrite( &hdr_3, 2, 1, fp );
	n_posix_fwrite( &hdr_4, 2, 1, fp );
	n_posix_fwrite( &trk_0, 4, 1, fp );
	n_posix_fwrite( &trk_1, 4, 1, fp );


	// [!] : data length

	u32 data_byte = 0;


	// Author
if ( debug_onoff ) { n_posix_debug_literal( " Author " ); }

	if ( n_posix_false == n_string_is_empty( midi->author ) )
	{

#ifdef UNICODE
		char *ansi = n_posix_unicode2ansi( midi->author );
#else // #ifdef UNICODE
		char *ansi = n_string_carboncopy( midi->author );
#endif // #ifdef UNICODE

		size_t byte  = strlen( ansi );
		u32    tmp32 = n_midi_write_author( (u8) byte );

		n_posix_fwrite( &tmp32,    4, 1, fp );
		n_posix_fwrite(   ansi, byte, 1, fp );

		n_memory_free( ansi );


		data_byte += 4 + (u32) byte;

	}


	// Title
if ( debug_onoff ) { n_posix_debug_literal( " Title " ); }

	if ( n_posix_false == n_string_is_empty( midi->title ) )
	{

#ifdef UNICODE
		char *ansi = n_posix_unicode2ansi( midi->title );
#else // #ifdef UNICODE
		char *ansi = n_string_carboncopy( midi->title );
#endif // #ifdef UNICODE

		size_t byte  = strlen( ansi );
		u32    tmp32 = n_midi_write_title( (u8) byte );

		n_posix_fwrite( &tmp32,    4, 1, fp );
		n_posix_fwrite(   ansi, byte, 1, fp );

		n_memory_free( ansi );


		data_byte += 4 + (u32) byte;

	}


	int i;


	// Meta Events
if ( debug_onoff ) { n_posix_debug_literal( " Meta Events " ); }

	i = 0;
	n_posix_loop
	{//break;

		if ( midi->sound[ i ] != N_MIDI_PROGRAM_NOT_USED )
		{

			u32 v = n_midi_write_volume( midi->volume[ i ] );
			u32 p = n_midi_write_panpot( midi->panpot[ i ] );
			u32 s = n_midi_write_sound ( midi->sound [ i ] );

			n_posix_fwrite( &v, 4, 1, fp );
			n_posix_fwrite( &p, 4, 1, fp );
			n_posix_fwrite( &s, 3, 1, fp );

			data_byte += ( 4 + 4 + 3 );

		}


		i++;
		if ( i >= N_MIDI_CHANNEL_MAX ) { break; }
	}


	// Notes
if ( debug_onoff ) { n_posix_debug_literal( " Notes " ); }

	{

		u32 byte = midi->note_count * sizeof( u32 );

		n_posix_fwrite( midi->note, byte, 1, fp );

		data_byte += byte;

	}

	// [Patch] : all note off

	i = 0;
	n_posix_loop
	{

		if ( midi->sound[ i ] != N_MIDI_PROGRAM_NOT_USED )
		{

			u8 tmp8;
			if ( i == 0 ) { tmp8 = 4; } else { tmp8 = 0; }

			u32 tmp32 = n_endian_big( n_midi_note( tmp8, 0x90, i, 0, 0 ), 4 );
			n_posix_fwrite( &tmp32, sizeof( u32 ), 1, fp );

			data_byte += sizeof( u32 );

		}


		i++;
		if ( i >= N_MIDI_CHANNEL_MAX ) { break; }
	}


	// End of Track : 4 byte
if ( debug_onoff ) { n_posix_debug_literal( " End of Track " ); }

	u32 eot = n_midi_write_eot();
	n_posix_fwrite( &eot, 4, 1, fp );

	data_byte += 4;


	// Data Size

n_midi_log( midi, "%s", N_STRING_EMPTY );
n_midi_log( midi, "File Size : %d Byte ( 0x%08x )", 22 + data_byte, 22 + data_byte );
n_midi_log( midi, "Data Size : %d Byte ( 0x%08x )",      data_byte,      data_byte );
n_midi_log( midi, "%s", N_STRING_EMPTY );

	n_posix_fseek( fp, N_MIDI_DATASIZE_OFFSET, SEEK_SET );

	data_byte = n_endian_big( data_byte, 4 );
	n_posix_fwrite( &data_byte, 4, 1, fp );

	n_posix_fclose( fp );


if ( debug_onoff ) { n_posix_debug_literal( " Done! " ); }
	return n_posix_false;
}

// internal
n_posix_bool
n_midi_txt( n_midi *midi, n_txt *txt )
{

	// [!] : "txt" will be touched


	if ( n_midi_error( midi ) ) { return n_posix_true; }

	if ( n_txt_error( txt ) ) { return n_posix_true; }


	{ // Phase 1 : Sanitizing


	u32 i,ii;
	n_posix_bool multiline_comment = n_posix_false;


	i = 0;
	n_posix_loop
	{//break;

		// [!] : remove comment-out

		n_posix_char *l = n_txt_get( txt, i );

		if ( n_string_is_same_literal( "/*", l ) )
		{
			multiline_comment = n_posix_true;
		} else
		if ( n_string_is_same_literal( "*/", l ) )
		{
			multiline_comment = n_posix_false;
		}

		if ( multiline_comment )
		{
			n_string_truncate( l );
		} else {
			n_string_commentout( l,l,      N_STRING_SEMICOLON );
			n_string_commentout( l,l, n_posix_literal( "//" ) );
		}

		i++;
		if ( i >= txt->sy ) { break; }
	}

	i = 0;
	n_posix_loop
	{//break;

		// [!] : one token by one line

		n_posix_char *l = n_txt_get( txt, i );


		ii = 0;
		n_posix_loop
		{

			if ( n_string_is_empty( l ) ) { break; }


			if ( l[ ii ] == n_posix_literal( ')' ) )
			{

				n_txt_add( txt, i + 1, &l[ ii + 1 ] );
				n_string_terminate( l, ii + 1 );

				break;
			}


			ii++;
			if ( l[ ii ] == N_STRING_CHAR_NUL ) { break; }
		}


		i++;
		if ( i >= txt->sy ) { break; }
	}

	i = 0;
	n_posix_loop
	{//break;

		// [!] : token need to start from supported characters

		n_posix_char *l = n_txt_get( txt, i );


		ii = 0;
		n_posix_loop
		{

			if ( n_string_is_empty( l ) ) { break; }


			if (
				( n_string_is_alphabet( l, ii ) )
				||
				( l[ ii ] == n_posix_literal( '!' ) )
			)
			{

				n_string_copy( &l[ ii ], l );

				break;
			}


			ii++;
			if ( l[ ii ] == N_STRING_CHAR_NUL ) { break; }
		}


		i++;
		if ( i >= txt->sy ) { break; }
	}

	i = 0;
	n_posix_loop
	{//break;

		// [!] : remove blank and empty line

		n_posix_char *l = n_txt_get( txt, i );

		n_string_remove_blank( l, l );


		if ( n_string_is_empty( l ) ) { n_txt_del( txt, i ); } else { i++; }

		if ( i >= txt->sy ) { break; }
	}

//n_txt_save_literal( txt, "result.txt" );
//return 0;


	} // Phase 1 : Sanitizing


	{ // Phase 2 : Tokenization


	// [!] : enough size
	//
	//	x2 for note-on/note-off
	//	x2 for syncopation support

	u32 byte = (u32) ( txt->sy * 2 * 2 ) * sizeof( u32 );

	midi->note = n_memory_new( byte );
	n_memory_zero( midi->note, byte );


	n_posix_char prm_1[ N_MIDI_CCH ];
	n_posix_char prm_2[ N_MIDI_CCH ];
	int          channel     = 0;
	int          syncopation = 0;
	int          tweak_pitch = 0;
	int          tweak_tempo = 0;


	u32 i = 0;
	n_posix_loop
	{

		// [ Mechanism ]
		//
		//	Token     : "c( 4, 10 )"
		//	Parameter : "c( 4, 10 )" => prm_1 = 4 : prm_2 = 10

		n_posix_char *token = n_txt_get( txt, i );

		n_string_truncate( prm_1 );
		n_string_truncate( prm_2 );

		n_midi_token( token, prm_1, prm_2 );

		int p0 = n_posix_atoi( token );
		int p1 = n_posix_atoi( prm_1 );
		int p2 = n_posix_atoi( prm_2 );

n_midi_log( midi, "Token #%04d : %s : %s : %s", i, token, prm_1, prm_2 );

		if ( token[ 0 ] == n_posix_literal( '!' ) )
		{

			if ( n_string_is_same_literal( "Pitch", prm_1 ) )
			{
				tweak_pitch = p2;
			} else
			if ( n_string_is_same_literal( "Tempo", prm_1 ) )
			{
				tweak_tempo = p2 * -1;
			}// else

		} else
		if ( token[ 0 ] == n_posix_literal( 'A' ) )
		{

			if (
				( n_string_is_empty( midi->author ) )
				&&
				( N_MIDI_NAME_MAX > n_posix_strlen( prm_1 ) )
			)
			{
				n_string_copy( prm_1, midi->author );
//n_posix_debug( midi->author );
			}

		} else
		if ( token[ 0 ] == n_posix_literal( 'T' ) )
		{

			if (
				( n_string_is_empty( midi->title ) )
				&&
				( N_MIDI_NAME_MAX > n_posix_strlen( prm_1 ) )
			)
			{
				n_string_copy( prm_1, midi->title );
			}

		} else
		if ( token[ 0 ] == n_posix_literal( 'C' ) )
		{ 

			// [!] : Timekeeper Channel : 16th channel is reserved

			p1 = n_posix_min( p1, N_MIDI_CHANNEL_MAX - 1 - 1 );
			p2 = n_posix_min( p2, N_MIDI_PARAM_MAX   - 1     );

			channel = p1;

			if ( midi->sound[ channel ] == N_MIDI_PROGRAM_NOT_USED )
			{
				midi->sound[ channel ] = p2;
			}

		} else
		if ( token[ 0 ] == n_posix_literal( 'P' ) )
		{

			midi->panpot[ channel ] = n_posix_min( p1, N_MIDI_PARAM_MAX - 1 );

		} else
		if ( token[ 0 ] == n_posix_literal( 'R' ) )
		{

			midi->repeat = n_posix_min( p1, N_MIDI_PARAM_MAX - 1 );

		} else
		if ( token[ 0 ] == n_posix_literal( 'S' ) )
		{

			syncopation = n_posix_min( p1, N_MIDI_PARAM_MAX - 1 );

		} else
		if ( token[ 0 ] == n_posix_literal( 'V' ) )
		{

			midi->volume[ channel ] = n_posix_min( p1, N_MIDI_PARAM_MAX - 1 );

		} else
		if (
			(
				( token[ 0 ] >= n_posix_literal( 'a' ) )
				&&
				( token[ 0 ] <= n_posix_literal( 'g' ) )
			)
			||
			( token[ 0 ] == n_posix_literal( 'r' ) )
			||
			( p0 )
		)
		{

			// [ Mechanism ] : note[] shares note-on and note-off
			//
			//	         | delta | volume |
			//	note-on  |   x   |   o    |
			//	note-off |   o   |   x    |


			u32 n = 0;

			n_posix_bool is_single_param = ( p2 == 0 );

			p1 = n_posix_minmax( 0, N_MIDI_PARAM_MAX - 1, p1               );
			p2 = n_posix_minmax( 0, N_MIDI_PARAM_MAX - 1, p2 + tweak_tempo );

			// [!] : r( 4 ) and r( 3,4 ) are handled as the same
			if ( is_single_param ) { p2 = n_posix_minmax( 0, N_MIDI_PARAM_MAX - 1, p1 + tweak_tempo ); }


			if ( token[ 0 ] == n_posix_literal( 'r' ) )
			{

				n = n_midi_note( p2, 0x90, channel, 0, 0 );

				midi->step[ channel ] += p2;

if ( p2 == 0 ) { midi->zero_pause++; }

			} else {

				// [!] : syncopation support

				if (
					( syncopation != 0 )
					&&
					( p2 > syncopation )
				)
				{

					p2 -= syncopation;


					n = n_midi_note( syncopation, 0x90, channel, 0, 0 );

					midi->step[ channel ] += syncopation;


					midi->note[ midi->note_count ] = n;

n_midi_log
(
	midi,
	"#%04d : Channel %3d : Param %3d, %3d : Note %08lx : Syncopation",
	midi->note_count,
	channel, p1, syncopation,
	n
);

					midi->note_count++;

				}


				// [ Mechanism ]
				//
				//	rhythm     = sound number  : directly set
				//	non-rhythm = pitch of note : need to calculate

				if ( channel != N_MIDI_RHYTHMCHANNEL )
				{
					if ( p0 )
					{
						p1 = p0;
					} else {
						p1 = n_midi_token2note( token, p1 );
					}
					p1 = n_posix_minmax( 0, N_MIDI_PARAM_MAX - 1, p1 + tweak_pitch );
				}


				n = n_midi_note( p2, 0x90, channel, p1, midi->volume[ channel ] );

				midi->step[ channel ] += p2;

			}


			midi->note[ midi->note_count ] = n;

n_midi_log
(
	midi,
	"#%04d : Channel %3d : Param %3d, %3d : Note %08lx",
	midi->note_count,
	channel, p1, p2,
	n
);

			midi->note_count++;

		}


		i++;
		if ( i >= txt->sy ) { break; }

	} // n_posix_loop


	} // Phase 2 : Tokenization


	return n_posix_false;
}

// internal
n_posix_bool
n_midi_bmp( n_midi *midi, const n_bmp *bmp )
{

	// [Mechanism]
	//
	//	X         | Red      | Green  | Blue   |
	//	----------------------------------------
	//	0         | Channel# | Sound# | Panpot |
	//	1 or more | -------- | Note#  | Volume |
	//
	//	clamp to 127 when value is over 127
	//
	//	C4 (of piano) is 60 
	//	panpot center is 64
	//	volume zero means pause (note-off)


	if ( n_midi_error( midi ) ) { return n_posix_true; }

	if ( n_bmp_error( bmp ) ) { return n_posix_true; }


	const n_type_gfx sx = N_BMP_SX( bmp );
	const n_type_gfx sy = N_BMP_SY( bmp );


	// [!] : enough size
	//
	//	x2 for note-on/note-off

	u32 byte = ( ( sx * sy ) * 2 ) * sizeof( u32 );

	midi->note = n_memory_new( byte );
	n_memory_zero( midi->note, byte );


	// [!] : tempo base

	const int unit_length = 1;


	u32 color;
	int r,g,b;

	int channel = 0;


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{//break;

		n_bmp_ptr_get( bmp, x,y, &color );

		r = n_posix_min( n_bmp_r( color ), N_MIDI_CHANNEL_MAX - 1 );
		g = n_posix_min( n_bmp_g( color ), N_MIDI_PARAM_MAX   - 1 );
		b = n_posix_min( n_bmp_b( color ), N_MIDI_PARAM_MAX   - 1 );

		if ( x == 0 )
		{

			channel                 = r;
			midi->sound [ channel ] = g;
			midi->panpot[ channel ] = b;

		} else {

			static u32 p = 0xffffffff;

			int note   = g;
			int volume = b;
			u32 n      = n_midi_note( unit_length, 0x90, channel, note, volume );
			int l      = n_midi_note_delta_get( p );


			// [!] : this means "tie" or "slur"

			if (
				( channel != N_MIDI_RHYTHMCHANNEL )
				&&
				( ( l + unit_length ) < N_MIDI_PARAM_MAX )
				&&
				( channel == n_midi_note_channel_get( p ) )
				&&
				(    note == n_midi_note_p1_get( p ) )
				&&
				(  volume == n_midi_note_p2_get( p ) )
			)
			{

				n = n_midi_note( l + unit_length, 0x90, channel, note, volume );
				midi->note[ midi->note_count - 1 ] = n;

			} else {

				midi->note[ midi->note_count ] = n;
				midi->note_count++;

			}

			p = n;

			midi->step[ channel ] += unit_length;

		}

		x++;
		if ( x >= sx )
		{

			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}


	return n_posix_false;
}

void
n_midi_encode( const n_posix_char *abspath, n_posix_bool log_onoff )
{

	n_posix_char *dir = n_string_path_folder_current_new();
	n_string_path_folder_change( abspath );


	// Phase 0 : Is the MIDI file writable

	n_posix_char *newname = n_string_path_ext_mod_new( abspath, N_MIDI_EXT );

	{

		FILE *fp = n_posix_fopen_write( newname );
		if ( fp == NULL )
		{
			n_string_path_free( newname );

			n_string_path_folder_change( dir );
			n_string_path_free( dir );

			return;
		}
		n_posix_fclose( fp );

	}


	// Phase 1 : Loading

	n_bmp bmp; n_bmp_zero( &bmp );
	n_txt txt; n_txt_zero( &txt );

	n_posix_bool is_bmp = n_posix_false;

	if (
		( n_bmp_load( &bmp, abspath ) )
		&&
		( n_txt_load( &txt, abspath ) )
	)
	{
		n_string_path_free( newname );

		n_string_path_folder_change( dir );
		n_string_path_free( dir );

		return;
	}

	if ( NULL != N_BMP_PTR( &bmp ) ) { is_bmp = n_posix_true; }


	n_midi midi; n_midi_zero( &midi );

midi.log_onoff = log_onoff;


	// [!] : Log File

	if ( midi.log_onoff )
	{

		n_string_copy( abspath, midi.log_name );
		n_string_path_ext_mod_literal( ".log.txt\0\0", midi.log_name );

		n_txt_new( &midi.log );

	}


	n_memory_padding_u8( midi.volume, N_MIDI_VOLUME_DEFAULT,   N_MIDI_CHANNEL_MAX );
	n_memory_padding_u8( midi.panpot, N_MIDI_PANPOT_CENTER,    N_MIDI_CHANNEL_MAX );
	n_memory_padding_u8( midi.sound,  N_MIDI_PROGRAM_NOT_USED, N_MIDI_CHANNEL_MAX );


#ifdef UNICODE

#endif // #ifdef UNICODE

n_midi_log( &midi, "DATA : %s", abspath );
n_midi_log( &midi, "MIDI : %s", newname );
n_midi_log( &midi, "%s", N_STRING_EMPTY );

	if ( is_bmp )
	{
		n_midi_bmp( &midi, &bmp );
	} else {
		n_midi_txt( &midi, &txt );
	}

	midi.repeat = n_posix_max( 1, midi.repeat );

n_midi_log( &midi, "%s", N_STRING_EMPTY );
n_midi_log( &midi, "Author : %s", midi.author     );
n_midi_log( &midi, "Title  : %s", midi.title      );
n_midi_log( &midi, "Repeat : %d", midi.repeat     );
n_midi_log( &midi, "Note   : %d", midi.note_count );
n_midi_log( &midi, "%s", N_STRING_EMPTY );

	n_bmp_free( &bmp );
	n_txt_free( &txt );


	if ( midi.note_count == 0 )
	{

		n_memory_free( midi.note );

		if ( midi.log_onoff )
		{
			n_txt_save( &midi.log, midi.log_name );
			n_txt_free( &midi.log );
		}

		n_string_path_free( newname );

		n_string_path_folder_change( dir );
		n_string_path_free( dir );

		return;
	}


	// Phase 2 : Mapping


	// [ Mechanism ]
	//
	//	o == ON
	//	x == OFF
	//	r == pause
	//
	//	[ Ch. #0 ] [ ox ox ox ] => [ 12 34 56 ]
	//	[ Ch. #1 ] [ o- -x rr ] => [ 7- -8 rr ]
	//	[ Output ] [ 17 2- 3- 48 56 ]


	int max_step  = 0;
	int map_count = 0;

	{

		int i = 0;
		n_posix_loop
		{

			if ( max_step < midi.step[ i ] ) { max_step = midi.step[ i ]; }


			i++;
			if ( i >= N_MIDI_CHANNEL_MAX ) { break; }
		}

n_midi_log( &midi, "Max Step   : %d", max_step );

		map_count = ( max_step * N_MIDI_MAP_MAX ) * midi.repeat;

	}


	u32 *map = NULL;

	if ( map_count == 0 )
	{

		n_memory_free( midi.note );

		if ( midi.log_onoff )
		{
			n_txt_save( &midi.log, midi.log_name );
			n_txt_free( &midi.log );
		}

		n_string_path_free( newname );

		n_string_path_folder_change( dir );
		n_string_path_free( dir );

		return;

	} else {

		int byte = map_count * sizeof( u32 );

		map = n_memory_new_closed( byte );
		n_memory_zero( map, byte );

	}


	{

		// [!] : reuse

		n_memory_padding_int( midi.step, 0, N_MIDI_CHANNEL_MAX );

		int r = 0;
		int i = 0;
		n_posix_loop
		{//break;

			u8  ch = n_midi_note_channel_get( midi.note[ i ] );
			u32 m;

			// [!] : note on  : [ 16 - 31 ]

			m = ( midi.step[ ch ] * ( N_MIDI_MAP_MAX - 1 ) ) + ch;
			m = m + N_MIDI_CHANNEL_MAX;

			map[ m ] = n_midi_note_on( midi.note[ i ] );


			// [!] : midi.step[ ch ] will be changed

			midi.step[ ch ] += n_midi_note_delta_get( midi.note[ i ] );


			// [!] : note off : [  0 - 15 ]

			m = ( midi.step[ ch ] * ( N_MIDI_MAP_MAX - 1 ) ) + ch;

			map[ m ] = n_midi_note_off( midi.note[ i ] );


			i++;
			if ( i >= midi.note_count )
			{

				i = 0;

				r++;
				if ( r >= midi.repeat ) { break; }

				n_memory_padding_int( midi.step, max_step * r, N_MIDI_CHANNEL_MAX );

			}

		}

	}


	// Phase 3 : Timekeeper Channel : 16th channel is reserved
	//
	//	delta time can be minimum length
	//	this means timebase-free

	int resolution = N_MIDI_TIMEBASE / N_MIDI_RESOLUTION_MIN;
	int timekeeper = 0;

	{

		// [!] : getting heavy if making too many events

		int ch = 15;

		int i = 0;
		n_posix_loop
		{//break;

			map[ i + ch ] = n_midi_note( 0, 0x80, ch, 0, 0 );

			timekeeper++;

			i += N_MIDI_MAP_MAX * N_MIDI_RESOLUTION_MIN;
			if ( i >= map_count ) { break; }
		}

	}


	// Phase 4 : Time Delta Synchronization
	//
	//	[Condition]
	//	already mapped with time delta
	//
	// 	[Mechanism]
	//	the first note on a step : time from previous note-on/off
	//	the rest notes are zero  : it means "at the same time"

	{

		n_posix_bool not_found = n_posix_true;

		int last = 0;
		int unit = 0;

		int i = midi.note_count = 0;
		n_posix_loop
		{//break;

			int index = unit + i;

			if ( map[ index ] != 0 )
			{

				midi.note_count++;

				if ( not_found )
				{

					not_found = n_posix_false;

					int delta = ( unit - last ) / N_MIDI_MAP_MAX;

					map[ index ] = n_midi_note_delta_set( map[ index ], delta * resolution );

					last = unit;

				} else {

					map[ index ] = n_midi_note_delta_set( map[ index ], 0 );

				}

			}


			i++;
			if ( i >= N_MIDI_MAP_MAX )
			{

				i = 0;

				not_found = n_posix_true;

				unit += N_MIDI_MAP_MAX;
				if ( unit >= map_count ) { break; }
			}
		}

	}


	// Phase 5 : Write back to midi.note

n_midi_log( &midi, "%s", N_STRING_EMPTY );
n_midi_log( &midi, "Note Count : %3d", midi.note_count );
n_midi_log( &midi, "Timekeeper : %3d", timekeeper );
n_midi_log( &midi, "Zero Pause : %3d", midi.zero_pause );
n_midi_log( &midi, "%s", N_STRING_EMPTY );

	n_memory_free( midi.note );

	midi.note = n_memory_new( sizeof( u32 ) * midi.note_count );
	n_memory_zero( midi.note, sizeof( u32 ) * midi.note_count );

	{

		int  i = 0;
		int ii = 0;
		n_posix_loop
		{//break;

			if ( map[ ii ] != 0 )
			{

				midi.note[ i ] = n_endian_big( map[ ii ], 4 );

n_midi_log( &midi, "Note  %4d : %08lx", i, midi.note[ i ] );

				i++;
				if ( i >= midi.note_count ) { break; }

			}


			ii++;
			if ( ii >= map_count ) { break; }
		}

	}

	n_memory_free_closed( map );


	// Phase 6 : Done!

	n_midi_write( &midi, newname );


	// Phase 7 : Cleanup

	n_memory_free( midi.note );

	if ( midi.log_onoff )
	{
		n_txt_save( &midi.log, midi.log_name );
		n_txt_free( &midi.log );
	}

	n_string_path_free( newname );


	n_string_path_folder_change( dir );
	n_string_path_free( dir );


	return;
}


#endif // _H_NONNON_NEUTRAL_MIDI

