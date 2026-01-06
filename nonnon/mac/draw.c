// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC_DRAW
#define _H_NONNON_MAC_DRAW




#import <Cocoa/Cocoa.h>

#include "image.c"




void
n_mac_draw_box( NSColor *color, NSRect rect )
{

	[color set];
	//[NSBezierPath fillRect:rect];

	NSRectFill( rect );


	return;
}

void
n_mac_draw_box_invert( NSColor *color, NSRect rect )
{
//rect.size.width = 10;

	// [x] : no change at all
	//NSRectFillUsingOperation( rect, NSCompositingOperationXOR );

	n_mac_draw_box( color, rect );


	// [x] : DeepSeek AI : something wrong
/*
	CGContextRef context = [[NSGraphicsContext currentContext] CGContext];

	CGContextSaveGState( context );

	CGContextSetBlendMode( context, kCGBlendModeDifference );
	CGContextSetFillColorWithColor( context, color.CGColor );
	CGContextFillRect( context, rect );

	CGContextRestoreGState( context );
*/

	return;
}

void
n_mac_draw_frame( NSColor *color, NSRect rect )
{

	[color set];

	NSFrameRect( rect );


	return;
}

#define N_MAC_DRAW_ROUNDRECT_NONE        ( 0x0000 )
#define N_MAC_DRAW_ROUNDRECT_ALL         ( 0xffff )
#define N_MAC_DRAW_ROUNDRECT_TOPLEFT     ( 1 << 0 )
#define N_MAC_DRAW_ROUNDRECT_TOPRIGHT    ( 1 << 1 )
#define N_MAC_DRAW_ROUNDRECT_BOTTOMLEFT  ( 1 << 2 )
#define N_MAC_DRAW_ROUNDRECT_BOTTOMRIGHT ( 1 << 3 )
#define N_MAC_DRAW_ROUNDRECT_LEFT        ( N_MAC_DRAW_ROUNDRECT_TOPLEFT  | N_MAC_DRAW_ROUNDRECT_BOTTOMLEFT  )
#define N_MAC_DRAW_ROUNDRECT_RIGHT       ( N_MAC_DRAW_ROUNDRECT_TOPRIGHT | N_MAC_DRAW_ROUNDRECT_BOTTOMRIGHT )

#define n_mac_draw_roundrect( c, r, radius ) n_mac_draw_roundrect_partial( c, r, radius, N_MAC_DRAW_ROUNDRECT_ALL )

void
n_mac_draw_roundrect_partial( NSColor *color, NSRect rect, CGFloat radius, int mode )
{

	NSGraphicsContext *nscontext = [NSGraphicsContext currentContext];
	CGContextRef         context = [nscontext CGContext];

	[nscontext saveGraphicsState];


	CGContextMoveToPoint( context, rect.origin.x, rect.origin.y + radius );


	// [!] : top-left

	CGFloat radius_tl = 0;
	if ( mode & N_MAC_DRAW_ROUNDRECT_TOPLEFT ) { radius_tl = radius; }

	CGContextAddArcToPoint
	(
		context,
		rect.origin.x,
		rect.origin.y,
		rect.origin.x + radius_tl,
		rect.origin.y,
		radius_tl
	);



	// [!] : top-right

	CGFloat radius_tr = 0;
	if ( mode & N_MAC_DRAW_ROUNDRECT_TOPRIGHT ) { radius_tr = radius; }

	CGContextAddArcToPoint
	(
		context,
		rect.origin.x + rect.size.width,
		rect.origin.y,
		rect.origin.x + rect.size.width,
		rect.origin.y + radius_tr,
		radius_tr
	);


	// [!] : bottom-right

	CGFloat radius_br = 0;
	if ( mode & N_MAC_DRAW_ROUNDRECT_BOTTOMRIGHT ) { radius_br = radius; }

	CGContextAddArcToPoint
	(
		context,
		rect.origin.x + rect.size.width,
		rect.origin.y + rect.size.height,
		rect.origin.x + rect.size.width - radius_br,
		rect.origin.y + rect.size.height,
		radius_br
	);


	// [!] : bottom-left

	CGFloat radius_bl = 0;
	if ( mode & N_MAC_DRAW_ROUNDRECT_BOTTOMLEFT ) { radius_bl = radius; }

	CGContextAddArcToPoint
	(
		context,
		rect.origin.x,
		rect.origin.y + rect.size.height,
		rect.origin.x,
		rect.origin.y,
		radius_bl
	);


	CGContextClosePath( context );
 
 
 	[color set];
	CGContextFillPath( context );


	[nscontext restoreGraphicsState];


	return;
}

void
n_mac_draw_circle( NSColor *color, NSRect rect )
{

	NSGraphicsContext *nscontext = [NSGraphicsContext currentContext];
	CGContextRef         context = [nscontext CGContext];

	[nscontext saveGraphicsState];

	CGFloat radius_x = rect.size.width;
	CGFloat radius_y = rect.size.height;

	NSBezierPath *path = [NSBezierPath bezierPathWithRoundedRect:rect xRadius:radius_x yRadius:radius_y];
	[path addClip];

	[color set];
	CGContextFillRect( context, NSRectToCGRect( rect ) );

	[nscontext restoreGraphicsState];


	return;
}

void
n_mac_draw_gradient( NSColor *color_left, NSColor *color_right, NSRect rect )
{

	NSGradient *g = [[NSGradient alloc] initWithStartingColor:color_left endingColor:color_right];

	[g drawInRect:rect angle:1.0];


	return;
}




#endif // _H_NONNON_MAC_DRAW


