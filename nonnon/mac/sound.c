// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC_SOUND
#define _H_NONNON_MAC_SOUND




#import <AVFoundation/AVFoundation.h>




#include "../neutral/posix.c"


#include "./window.c"




NSURL*
n_mac_sound_resource_url( NSString *resource_name, NSString *ext )
{

	// [!] : resource_name
	//
	//	[ how to register ]
	//
	//		DnD to left side of Xcode
	//
	//	[ pitfalls ]
	//
	//		resource_name needs no extension
	//		for example : "mew.wav" => "mew" and "wav"


	// [!] : in case of crash
	//
	//	Xcode 16 seems to be broken
	//	I don't know how to fix


	NSBundle *main = [NSBundle mainBundle];
	if ( main == NULL ) { return NULL; }

	NSString *path = [main pathForResource:resource_name ofType:ext];
	if ( path == NULL ) { return NULL; }


	return [NSURL fileURLWithPath:path];
}




#define n_mac_sound_wav_init( wav_name ) n_mac_sound_init( wav_name, @"wav" )

AVAudioPlayer*
n_mac_sound_init( NSString *resource_name, NSString *ext )
{

	NSURL *url = n_mac_sound_resource_url( resource_name, ext );
	if ( url == NULL ) { return NULL; }

	return [[AVAudioPlayer alloc] initWithContentsOfURL:url error:nil];
}

void
n_mac_sound_loop_count( AVAudioPlayer *player, int value )
{

	if ( player == NULL ) { return; }


	// [!] : -1 for value : infinite loop playback

	player.numberOfLoops = value;


	return;
}

void
n_mac_sound_rewind( AVAudioPlayer *player )
{

	if ( player == NULL ) { return; }


	player.currentTime = 0;


	return;
}

#define n_mac_sound_play(  player ) [player  play]
#define n_mac_sound_stop(  player ) [player  stop]
#define n_mac_sound_pause( player ) [player pause]




// [!] : if you want more speed like games, use this

// [!] : this layer cannot play loop playback

// [x] : Monterey : "throwing -10878" is logged : maybe bug
// [x] : Sonoma   : "skipping cycle due to overload"

static AVAudioEngine *n_mac_sound_engine = NULL;

typedef struct {

	AVAudioFile          *file;

	AVAudioPlayerNode    *player;
	AVAudioMixerNode     *mixer;

	AVAudioUnitReverb    *reverb;
	AVAudioUnitDelay     *delay;

} n_mac_sound2;

#define n_mac_sound2_zero( p ) n_memory_zero( p, sizeof( n_mac_sound2 ) )

void
n_mac_sound2_init( n_mac_sound2 *p, NSString *resource_name, NSString *ext )
{

	if ( n_mac_sound_engine == NULL )
	{
		n_mac_sound_engine = [[AVAudioEngine alloc] init];
	}


	if ( p == NULL ) { return; }

	n_mac_sound2_zero( p );


	NSURL *url = n_mac_sound_resource_url( resource_name, ext );
	if ( url == NULL ) { return; }

	AVAudioFile *file = [[AVAudioFile alloc] initForReading:url error:nil];

	p->file = file;


	p->player = [[AVAudioPlayerNode alloc] init];
	p->mixer  = [[AVAudioMixerNode  alloc] init];

	[n_mac_sound_engine attachNode:p->player];
	[n_mac_sound_engine attachNode:p->mixer ];

	[n_mac_sound_engine connect:p->player to:p->mixer                         format:nil];
	[n_mac_sound_engine connect:p->mixer  to:n_mac_sound_engine.mainMixerNode format:nil];


	// [!] : crash : when not connected

	[n_mac_sound_engine startAndReturnError:NULL];
	[n_mac_sound_engine prepare];


	return;
}

n_mac_sound2*
n_mac_sound2_init2( NSString *resource_name, NSString *ext )
{

	// [!] : for compatibility with n_mac_sound_init()

	n_mac_sound2 *ret = n_memory_new( sizeof( n_mac_sound2 ) );

	n_memory_zero( ret, sizeof( n_mac_sound2 ) );


	n_mac_sound2_init( ret, resource_name, ext );


	return ret;
}

void
n_mac_sound2_stop( n_mac_sound2 *p )
{
//NSLog( @"n_mac_sound2_stop" );

	if ( p == NULL ) { return; }

	if ( p->player == NULL ) { return; }


	[p->player stop];


	return;
}

void
n_mac_sound2_play( n_mac_sound2 *p )
{
//NSLog( @"n_mac_sound2_play" );

	if ( p == NULL ) { return; }

	if ( p->player == NULL ) { return; }


	// [x] : LM2    : slowdown
	// [x] : Typing : delay

	//if ( n_mac_sound_engine.running ) { n_mac_sound2_stop( p ); }


	// [Needed] : do every time

	[p->player scheduleFile:p->file
	                 atTime:nil
	      completionHandler:nil
	];


	[p->player play];


	return;
}




void
n_mac_sound2_effect_init( n_mac_sound2 *p, NSString *resource_name, NSString *ext )
{

	if ( n_mac_sound_engine == NULL )
	{
		n_mac_sound_engine = [[AVAudioEngine alloc] init];
	}


	if ( p == NULL ) { return; }

	n_mac_sound2_zero( p );


	NSURL *url = n_mac_sound_resource_url( resource_name, ext );
	if ( url == NULL ) { return; }

	AVAudioFile *file = [[AVAudioFile alloc] initForReading:url error:nil];

	p->file = file;


	p->player = [[AVAudioPlayerNode alloc] init];
	p->mixer  = [[AVAudioMixerNode  alloc] init];
	p->delay  = [[AVAudioUnitDelay  alloc] init];
	p->reverb = [[AVAudioUnitReverb alloc] init];

	[n_mac_sound_engine attachNode:p->player];
	[n_mac_sound_engine attachNode:p->mixer ];
	[n_mac_sound_engine attachNode:p->reverb];
	[n_mac_sound_engine attachNode:p->delay];

	[n_mac_sound_engine connect:p->player to:p->reverb                        format:nil];
	[n_mac_sound_engine connect:p->reverb to:p->delay                         format:nil];
	[n_mac_sound_engine connect:p->delay  to:p->mixer                         format:nil];
	[n_mac_sound_engine connect:p->mixer  to:n_mac_sound_engine.mainMixerNode format:nil];


	[n_mac_sound_engine startAndReturnError:NULL];
	[n_mac_sound_engine prepare];


	return;
}

n_mac_sound2*
n_mac_sound2_effect_init2( NSString *resource_name, NSString *ext )
{

	// [!] : for compatibility with n_mac_sound_init()

	n_mac_sound2 *ret = n_memory_new( sizeof( n_mac_sound2 ) );

	n_memory_zero( ret, sizeof( n_mac_sound2 ) );

	n_mac_sound2_effect_init( ret, resource_name, ext );


	return ret;
}

void
n_mac_sound2_effect_set( n_mac_sound2 *p, float pan, float wetdry, NSTimeInterval time, float feedback )
{

	if ( p == NULL ) { return; }


	[p->mixer setPan:pan]; 

	p->reverb.wetDryMix = wetdry;

	[p->delay setDelayTime:    time];
	[p->delay setFeedback :feedback];


	[n_mac_sound_engine connect:p->player to:p->reverb                        format:nil];
	[n_mac_sound_engine connect:p->reverb to:p->delay                         format:nil];
	[n_mac_sound_engine connect:p->delay  to:p->mixer                         format:nil];
	[n_mac_sound_engine connect:p->mixer  to:n_mac_sound_engine.mainMixerNode format:nil];


	return;
}




typedef struct {

	AVAudioPCMBuffer     *buffer;

	AVAudioPlayerNode    *player;
	AVAudioMixerNode     *mixer;

} n_mac_sound3;

#define n_mac_sound3_zero( p ) n_memory_zero( p, sizeof( n_mac_sound3 ) )

// internal
void
n_mac_sound3_connect( n_mac_sound3 *p )
{

	if ( p == NULL ) { return; }

	if ( p->player == NULL ) { return; }
	if ( p-> mixer == NULL ) { return; }

	[n_mac_sound_engine connect:p->player to:p->mixer                         format:nil];
	[n_mac_sound_engine connect:p->mixer  to:n_mac_sound_engine.mainMixerNode format:nil];


	return;
}

void
n_mac_sound3_init( n_mac_sound3 *p, NSString *resource_name, NSString *ext )
{

	if ( n_mac_sound_engine == NULL )
	{
		n_mac_sound_engine = [[AVAudioEngine alloc] init];
	}


	if ( p == NULL ) { return; }

	n_mac_sound3_zero( p );


	NSURL *url = n_mac_sound_resource_url( resource_name, ext );
	if ( url == NULL ) { return; }


	AVAudioFile *file = [[AVAudioFile alloc] initForReading:url error:nil];

	AVAudioFormat     *audioFormat = file.processingFormat;
	AVAudioFrameCount  length      = (AVAudioFrameCount) file.length;

	p->buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:audioFormat frameCapacity:length];
	[file readIntoBuffer:p->buffer error:nil];


	p->player = [[AVAudioPlayerNode alloc] init];
	p->mixer  = [[AVAudioMixerNode  alloc] init];

	[n_mac_sound_engine attachNode:p->player];
	[n_mac_sound_engine attachNode:p->mixer ];

	n_mac_sound3_connect( p );


	[n_mac_sound_engine startAndReturnError:NULL];
	[n_mac_sound_engine prepare];


	return;
}

void
n_mac_sound3_stop( n_mac_sound3 *p )
{
//NSLog( @"n_mac_sound3_stop" );

	if ( p == NULL ) { return; }

	if ( p->player == NULL ) { return; }


	[p->player stop];


	return;
}

void
n_mac_sound3_play( n_mac_sound3 *p )
{
//NSLog( @"n_mac_sound3_play" );

	if ( p == NULL ) { return; }

	if ( p->player == NULL ) { return; }


	// [Needed] : do every time

	[p->player scheduleBuffer:p->buffer
	                   atTime:nil
	                  options:AVAudioPlayerNodeBufferLoops
	        completionHandler:nil
	];


	[p->player play];


	return;
}




// [!] : use for BGM, infinite loop is implemented

typedef struct {

	AVPlayerItem   *item;
	AVQueuePlayer  *queue;
	AVPlayerLooper *looper;

} n_mac_sound4;

#define n_mac_sound4_zero( p ) n_memory_zero( p, sizeof( n_mac_sound4 ) )

n_mac_sound4*
n_mac_sound4_init( NSString *resource_name, NSString *ext, int64_t fr, int64_t to )
{

	n_mac_sound4 *ret = n_memory_new( sizeof( n_mac_sound4 ) );
	n_mac_sound4_zero( ret );


	NSURL *url = n_mac_sound_resource_url( resource_name, ext );
	if ( url == NULL ) { return  NULL; }


	ret->item   = [AVPlayerItem playerItemWithURL:url];
	ret->queue  = [AVQueuePlayer queuePlayerWithItems:@[ret->item]];

	CMTime f;
	if ( fr == -1 )
	{
		f = kCMTimeZero;
	} else {
		f = CMTimeMake( fr, 1000 );
	}

	CMTime t;
	if ( to == -1 )
	{
		t = kCMTimePositiveInfinity;
	} else {
		t = CMTimeMake( to, 1000 );
	}

	CMTimeRange r = CMTimeRangeMake( f, t );

 	ret->looper = [AVPlayerLooper playerLooperWithPlayer:ret->queue templateItem:ret->item timeRange:r];

	//ret->looper = [AVPlayerLooper playerLooperWithPlayer:ret->queue templateItem:ret->item];


	return ret;
}

void
n_mac_sound4_play( n_mac_sound4 *p )
{

	if ( p == NULL ) { return; }

	if ( p->queue == NULL ) { return; }


	[p->queue play];


	return;
}

void
n_mac_sound4_stop( n_mac_sound4 *p )
{

	if ( p == NULL ) { return; }

	if ( p->queue == NULL ) { return; }


	[p->queue pause];


	return;
}

void
n_mac_sound4_rewind( n_mac_sound4 *p )
{

	if ( p == NULL ) { return; }

	if ( p->queue == NULL ) { return; }


	[p->queue seekToTime:kCMTimeZero];


	return;
}

void
n_mac_sound4_volume( n_mac_sound4 *p, CGFloat zero_one )
{

	if ( p == NULL ) { return; }

	if ( p->queue == NULL ) { return; }


	[p->queue setVolume:zero_one];


	return;
}




AVAudioPCMBuffer*
n_mac_sound_load( NSString *path )
{

	if ( path.length == 0 ) { return NULL; }

	NSURL       *url = [NSURL fileURLWithPath:path];
	AVAudioFile *in  = [[AVAudioFile alloc] initForReading:url error:nil];

	if ( in == NULL ) { return NULL; }

	AVAudioFormat     *format = in.processingFormat;
	AVAudioFrameCount  length = (AVAudioFrameCount) in.length;

	AVAudioPCMBuffer *buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:format frameCapacity:length];
	[in readIntoBuffer:buffer error:nil];


	return buffer;
}

void
n_mac_sound_save( AVAudioPCMBuffer *buffer, NSString *path )
{

	if ( buffer == NULL ) { return; }

	if ( path.length == 0 ) { return; }


	NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithInt   : kAudioFormatLinearPCM ], AVFormatIDKey,
		[NSNumber numberWithFloat :               44100.0 ], AVSampleRateKey,
		[NSNumber numberWithInt   :                  16   ], AVLinearPCMBitDepthKey,
		[NSNumber numberWithInt   :                   2   ], AVNumberOfChannelsKey,
		nil
	];

	NSURL       *url = [NSURL fileURLWithPath:path];
	AVAudioFile *out = [[AVAudioFile alloc] initForWriting:url settings:settings error:nil];

	[out writeFromBuffer:buffer error:NULL];


	return;
}


#endif // _H_NONNON_MAC_SOUND


