// Nonnon Nyaurism for Mac
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Thx : DeepSeek AI




#import <AudioToolbox/AudioToolbox.h>




//#include "../../nonnon/neutral/wav/all.c"




typedef struct {

	AudioQueueRef               queue;
	AudioQueueBufferRef         buffer;
	AudioStreamBasicDescription format;

} n_mac_sound_AudioQueue;


static n_mac_sound_AudioQueue n_mac_sound_AudioQueue_instance;




void
n_mac_sound_AudioQueue_callback
(
	void                *inUserData,
	AudioQueueRef        inAQ,
	AudioQueueBufferRef  inBuffer
)
{

	// [!] : this function cannot be omitted

	// [!] : this function will be called after playback
//NSLog( @"callback" );


	return;
}

void
n_mac_sound_AudioQueue_init( n_wav *wav, u32 x, u32 sx )
{

	n_mac_sound_AudioQueue *player = &n_mac_sound_AudioQueue_instance;


	if ( x >= N_WAV_COUNT( wav ) ) { return; }

	if ( sx > ( N_WAV_COUNT( wav ) - x ) ) { sx = N_WAV_COUNT( wav ) - x; }
//NSLog( @"%d", N_WAV_COUNT( wav ) - sx );


	UInt32 ch   = 2;
	UInt32 byte = sx * ch * sizeof( float );


	player->format.mSampleRate       = 44100.0;
	player->format.mFormatID         = kAudioFormatLinearPCM;
	player->format.mFormatFlags      = kAudioFormatFlagIsFloat | kAudioFormatFlagIsPacked;
	player->format.mBytesPerPacket   = ch * sizeof( float );
	player->format.mFramesPerPacket  = 1;
	player->format.mBytesPerFrame    = ch * sizeof( float );
	player->format.mChannelsPerFrame = ch;
	player->format.mBitsPerChannel   = sizeof( float ) * 8;

	AudioQueueNewOutput
	(
		&player->format,
		n_mac_sound_AudioQueue_callback,
		&player,
		NULL,
		NULL,
		0,
		&player->queue
	);

	AudioQueueAllocateBuffer( player->queue, byte, &player->buffer );


	float *audioData = (float*) player->buffer->mAudioData;

	if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
	{
		u32 i = 0;
		u32 j = 0;
		n_posix_loop
		{

			n_type_real l,r; n_wav_sample_get( wav, x + i, &l, &r );

			if ( N_WAV_FORMAT_PCM == N_WAV_FORMAT( wav ) )
			{
				l = l / SHRT_MAX;
				r = r / SHRT_MAX;
			}

			audioData[ j + 0 ] = l;
			audioData[ j + 1 ] = r;

			i++; j += ch;
			if ( i >= sx ) { break; }
		}
	} else {
		memcpy( audioData, N_WAV_PTR( wav ), byte );
	}


	player->buffer->mAudioDataByteSize = byte;


	AudioQueueEnqueueBuffer( player->queue, player->buffer, 0, NULL );


	return;
}

void
n_mac_sound_AudioQueue_play( void )
{

	// [!] : this includes resume after pause

	n_mac_sound_AudioQueue *player = &n_mac_sound_AudioQueue_instance;

	AudioQueueStart( player->queue, NULL );

	return;
}

void
n_mac_sound_AudioQueue_pause( void )
{

	n_mac_sound_AudioQueue *player = &n_mac_sound_AudioQueue_instance;

	AudioQueuePause( player->queue );

	return;
}

void
n_mac_sound_AudioQueue_stop( void )
{

	n_mac_sound_AudioQueue *player = &n_mac_sound_AudioQueue_instance;

	AudioQueueStop( player->queue, YES );

	return;
}


