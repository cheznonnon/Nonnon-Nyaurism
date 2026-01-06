// Nonnon Wave
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// From : http://en.wikipedia.org/wiki/Piano_key_frequencies




#ifndef _H_NONNON_NEUTRAL_WAV_PIANO
#define _H_NONNON_NEUTRAL_WAV_PIANO




// 88 = [ 3 + ( 12 * 7 ) + 1 ]

#define N_WAV_PIANO_MAX ( 88 )

n_type_real n_wav_piano[ N_WAV_PIANO_MAX + 1 ] = {

	27.5000,	// A
	29.1352,
	30.8677,	// B

	32.7032,	// C
	34.6478,
	36.7081,	// D
	38.8909,
	41.2034,	// E

	43.6535,	// F
	46.2493,
	48.9994,	// G
	51.9131,
	55.0000,	// A
	58.2705,
	61.7354,	// B

	65.4064,	// C : Deep C
	69.2957,
	73.4162,	// D
	77.7817,
	82.4069,	// E

	87.3071,	// F
	92.4986,
	97.9989,	// G
	103.826,
	110.000,	// A
	116.541,
	123.471,	// B

	130.813,	// C : Low C
	138.591,
	146.832,	// D
	155.563,
	164.814,	// E

	174.614,	// F
	184.997,
	195.998,	// G
	207.652,
	220.000,	// A
	233.082,
	246.942,	// B

	261.626,	// C : Middle C
	277.183,
	293.665,	// D
	311.127,
	329.628,	// E

	349.228,	// F
	369.994,
	391.995,	// G
	415.305,
	440.000,	// A
	466.164,
	493.883,	// B

	523.251,	// C : Tenor C
	554.365,
	587.330,	// D
	622.254,
	659.255,	// E

	698.456,	// F
	739.989,
	783.991,	// G
	830.609,
	880.000,	// A
	932.328,
	987.767,	// B

	1046.50,	// C : Soprano C
	1108.73,
	1174.66,	// D
	1244.51,
	1318.51,	// E

	1396.91,	// F
	1479.98,
	1567.98,	// G
	1661.22,
	1760.00,	// A
	1864.66,
	1975.53,	// B

	2093.00,	// C : Double High C
	2217.46,
	2349.32,	// D
	2489.02,
	2637.02,	// E

	2793.83,	// F
	2959.96,
	3135.96,	// G
	3322.44,
	3520.00,	// A
	3729.31,
	3951.07,	// B

	4186.01,	// C : Eighth octave

	0,

};


#define N_WAV_PIANO_DEEP       ( 3 + ( 12 * 1 ) )
#define N_WAV_PIANO_LOW        ( 3 + ( 12 * 2 ) )
#define N_WAV_PIANO_MIDDLE     ( 3 + ( 12 * 3 ) )
#define N_WAV_PIANO_TENOR      ( 3 + ( 12 * 4 ) )
#define N_WAV_PIANO_SOPRANO    ( 3 + ( 12 * 5 ) )
#define N_WAV_PIANO_DOUBLEHIGH ( 3 + ( 12 * 6 ) )




#endif // _H_NONNON_NEUTRAL_WAV_PIANO

