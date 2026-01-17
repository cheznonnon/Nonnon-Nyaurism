// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Template File


// [!] : Usage
//
//	1 : replace your NSView (Custom View) to NonnonGame
//	2 : IB : right pane : "Custom Class", "Class", set "NonnonGame"
//	3 : modify behavior


// [!] : trouble shooter
//
//	[ drawRect is not called at redraw ]
//
//		a : call [_n_game display];
//		b : see your @property and connect(DnD) to UI on the Xib canvas
//		c : layer may cause
//		d : Xcode integrity bug : remake a project from scratch




#ifndef _H_NONNON_MAC_NONNON_GAME
#define _H_NONNON_MAC_NONNON_GAME




#import <Cocoa/Cocoa.h>


#include "_mac.c"
#include "image.c"




#define N_MAC_GAME_SX 512
#define N_MAC_GAME_SY 512




@interface NonnonGame : NSView
@end


@interface NonnonGame ()
@end


@implementation NonnonGame {

	n_bmp canvas;
	BOOL  refresh;

}


- (instancetype)initWithCoder:(NSCoder *)coder
{
//NSLog( @"initWithCoder" );

	self = [super initWithCoder:coder];
	if ( self )
	{
		n_bmp_zero( &canvas );
		n_bmp_new( &canvas, N_MAC_GAME_SX, N_MAC_GAME_SY );

		refresh = TRUE;

		n_mac_timer_init( self, @selector( n_timer_method ), 1 );
	}


	return self;
}


- (BOOL) isFlipped
{
	return YES;
}


- (void) n_timer_method
{

//static u32 tick_prv = 0;

	static u32 timer = 0;
	if ( n_game_timer( &timer, 12 ) )
	{
		n_bmp_box
		(
			&canvas,
			n_game_random( N_BMP_SX( &canvas ) ),
			n_game_random( N_BMP_SY( &canvas ) ),
			12, 12,
			n_game_randomcolor()
		);

		refresh = TRUE;
	}


	if ( refresh )
	{
//n_mac_debug_count();
		refresh = FALSE;

		[self display];
	}

//u32 tick_cur = n_posix_tickcount();

//NSLog( @"%d", (int) tick_cur - tick_prv );

//tick_prv = tick_cur;

}




- (void)drawRect:(NSRect)rect
{
//NSLog( @"drawRect" );

	n_mac_image_nbmp_direct_draw( &canvas, &rect, FALSE );

}


- (void)mouseDown:(NSEvent*)theEvent
{
NSLog( @"mouseDown" );

	n_bmp_flush( &canvas, n_game_randomcolor() );

}




- (BOOL)acceptsFirstResponder
{
//NSLog(@"acceptsFirstResponder");
	return YES;
}

- (BOOL)becomeFirstResponder
{
//NSLog(@"becomeFirstResponder");
        return YES;
}

- (void)keyDown:(NSEvent *)event
{
NSLog( @"%d", event.keyCode );
}


@end




#endif // _H_NONNON_MAC_NONNON_GAME

