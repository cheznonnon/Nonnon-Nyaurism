// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : you need to register GCControllerDidConnectNotification
//
//	then you need to call n_mac_gamepad_init() from n_DidConnectNotification method

// [!] : and more, there is GCControllerDidDisconnectNotification

/*
	[[NSNotificationCenter defaultCenter]
		addObserver: self
		   selector: @selector( n_DidConnectNotification: )
		       name: GCControllerDidConnectNotification
		     object: nil
	];
*/




#ifndef _H_NONNON_MAC_GAMEPAD
#define _H_NONNON_MAC_GAMEPAD




#import <GameController/GameController.h>




//static BOOL n_mac_gamepad_log_onoff = TRUE;




GCExtendedGamepad*
n_mac_gamepad_init( void )
{

	GCExtendedGamepad *ret = nil;


	NSArray *a = [GCController controllers];
	if ( a == NULL )
	{
//if ( n_mac_gamepad_log_onoff ) { NSLog( @"NSArray is NULL" ); }
	} else
	if ( a.count > 0 )
	{
		GCController *c = a[ 0 ];
		if ( c != NULL )
		{
//if ( n_mac_gamepad_log_onoff ) { NSLog( @"Gamepad : exist" ); }
			ret = c.extendedGamepad;
			if ( ret != nil )
			{
//if ( n_mac_gamepad_log_onoff ) { NSLog( @"extendedGamepad != nil" ); }
				//
			}
		} else {
//if ( n_mac_gamepad_log_onoff ) { NSLog( @"Gamepad : not exist" ); }
		}
	} else {
//if ( n_mac_gamepad_log_onoff ) { NSLog( @"GCController : count = %lu", a.count ); }
	}


	return ret;
}



/*

// [!] : sample code

BOOL
n_nn2_keystate_get( n_nn2 *p, int id )
{

	BOOL ret = n_mac_keystate_get( id );

	if ( id == N_MAC_KEYCODE_ARROW_DOWN )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.down.pressed;
			ret |= p->gamepad.leftThumbstick.down.pressed;
		}
	} else
	if ( id == N_MAC_KEYCODE_ARROW_LEFT )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.left.pressed;
			ret |= p->gamepad.leftThumbstick.left.pressed;
		}
	} else
	if ( id == N_MAC_KEYCODE_ARROW_RIGHT )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.dpad.right.pressed;
			ret |= p->gamepad.leftThumbstick.right.pressed;
		}
	} else
	if ( id == N_MAC_KEYCODE_NN2_DASH )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.buttonX.pressed;
			ret |= p->gamepad.buttonY.pressed;
		}
	} else
	if ( id == N_MAC_KEYCODE_NN2_JUMP )
	{
		if ( p->gamepad != nil )
		{
			ret |= p->gamepad.buttonA.pressed;
			ret |= p->gamepad.buttonB.pressed;
		}
	}// else


	return ret;
}
*/



#endif // _H_NONNON_MAC_GAMEPAD
