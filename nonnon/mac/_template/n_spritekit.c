// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [x] : heavy, useless




#ifndef _H_NONNON_MAC_SPRITEKIT
#define _H_NONNON_MAC_SPRITEKIT




#import "SpriteKit/SpriteKit.h"




//typedef void (*n_spritekit_callback)( void );


typedef struct {

	n_bmp   *canvas;
	SKView  *view;
	SKScene *scene;

} n_spritekit;


static n_spritekit n_spritekit_inst = { NULL, NULL, NULL };




void
n_spritekit_test_func( void )
{

	int r = n_random_range( 255 );
	int g = n_random_range( 255 );
	int b = n_random_range( 255 );

	n_bmp_flush( n_spritekit_inst.canvas, n_bmp_rgb_mac( r,g,b ) );


	return;
}




@interface n_SKScene : SKScene
@end


@implementation n_SKScene

-(void)didMoveToView:(SKView*)view
{
//NSLog( @"didMoveToView" );

//NSLog( @"%f", view.bounds.size.width );
}
-(void)update:(NSTimeInterval)currentTime
{
//NSLog( @"update" );

	//n_spritekit_test_func();

	NSImage      *imag = n_mac_image_nbmp2nsimage( n_spritekit_inst.canvas );
	SKTexture    *txtr = [SKTexture textureWithImage:imag];
	SKSpriteNode *node = [SKSpriteNode spriteNodeWithTexture:txtr size:imag.size];

	node.position = CGPointMake( imag.size.width / 2, imag.size.height / 2 );
	//node.xScale   = 2.0;
	node.yScale   = -1.0; // [!] : flip

	[n_spritekit_inst.scene addChild:node];

}

@end




void
n_spritekit_init( NSView *view, n_bmp *bmp )
{

	n_spritekit_inst.canvas = bmp;

	NSRect rect = NSMakeRect( 0,0,N_BMP_SX( bmp ),N_BMP_SY( bmp ) );

	n_spritekit_inst.view = [[SKView alloc] initWithFrame:rect];
	n_spritekit_inst.view.showsFPS = YES;
	[view addSubview:n_spritekit_inst.view];

	n_spritekit_inst.scene = [n_SKScene sceneWithSize:n_spritekit_inst.view.bounds.size];
	[n_spritekit_inst.view presentScene:n_spritekit_inst.scene];
	//n_spritekit_inst.scene.scaleMode = SKSceneScaleModeResizeFill;


	return;
}




#endif // _H_NONNON_MAC_SPRITEKIT


