// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#ifndef _H_NONNON_MAC_IMAGE
#define _H_NONNON_MAC_IMAGE




#import <Cocoa/Cocoa.h>


#include "../neutral/bmp/all.c"
#include "../neutral/png.c"


#include "./_mac.c"




// [Needed] : set at awakeFromNib
//
//	initWithCoder is called before awakeFromNib
//	backingScaleFactor is not set in initWithCoder

static NSWindow *n_mac_image_window = nil;




void
n_bmp_mac_color( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	// [!] : format
	//
	//	ABGR
	//	upside down


	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{
		u32 color;
		n_bmp_ptr_get_fast( bmp, x,y, &color );

		color = n_bmp_argb
		(
			n_bmp_a( color ),
			n_bmp_b( color ),
			n_bmp_g( color ),
			n_bmp_r( color )
		);

		n_bmp_ptr_set_fast( bmp, x,y,  color );

		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{

			x = 0;

			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}
	}


	return;
}

void
n_bmp_mac( n_bmp *bmp )
{

	n_bmp_mac_color( bmp );

	n_bmp_flush_mirror( bmp, N_BMP_MIRROR_UPSIDE_DOWN );


	return;
}




#define n_bmp_rgb_mac( r, g, b ) n_bmp_rgb( b, g, r )

#define n_bmp_argb_mac( a, r, g, b ) n_bmp_argb( a, b, g, r )

#define n_bmp_color_mac( c ) n_bmp_argb( n_bmp_a( c ), n_bmp_b( c ), n_bmp_g( c ), n_bmp_r( c ) )

#define n_mac_nscolor_argb( a, r, g, b ) [NSColor colorWithSRGBRed:(r)/255.0 green:(g)/255.0 blue:(b)/255.0 alpha:(a)/255.0]

#define n_mac_argb2nscolor( c ) n_mac_nscolor_argb( n_bmp_a( c ), n_bmp_r( c ), n_bmp_g( c ), n_bmp_b( c ) )

#define n_mac_nscolor_blend( f,t, ratio ) [f blendedColorWithFraction:ratio ofColor:t]

u32
n_mac_nscolor2argb( NSColor *nscolor )
{

	CIColor *cicolor = [[CIColor alloc] initWithColor:nscolor];

	int a = (int) ( cicolor.alpha * 255.0 );
	int r = (int) ( cicolor.red   * 255.0 );
	int g = (int) ( cicolor.green * 255.0 );
	int b = (int) ( cicolor.blue  * 255.0 );

//NSLog( @"%f %f %f %f", cicolor.alpha, cicolor.red, cicolor.green, cicolor.blue );


	return n_bmp_argb( a,r,g,b );
}




NSPoint
n_mac_image_position_flip( const n_bmp *bmp, NSPoint pt )
{

	pt.y = N_BMP_SY( bmp ) - 1 - pt.y;

	return pt;
}

// internal
NSBitmapImageRep*
n_mac_image_NSBitmapImageRep( const n_bmp *bmp )
{

	NSBitmapImageRep *n_rep = [
		[NSBitmapImageRep alloc]
		initWithBitmapDataPlanes : (void*) &N_BMP_PTR( bmp )
		pixelsWide               : N_BMP_SX( bmp )
		pixelsHigh               : N_BMP_SY( bmp )
		bitsPerSample            : 8
		samplesPerPixel          : 4
		hasAlpha                 : YES
		isPlanar                 : NO
		colorSpaceName           : NSCalibratedRGBColorSpace
		bytesPerRow              : (int) N_BMP_SX( bmp ) * 4
		bitsPerPixel             : 32
	];
//if ( n_rep == nil ) { NSLog( @"NSBitmapImageRep : nil" ); }


	return n_rep;
}

NSImage*
n_mac_image_imagerep2nsimage( NSBitmapImageRep *rep )
{

	NSImage *nsimage = [[NSImage alloc] initWithSize:rep.size];
	[nsimage addRepresentation:rep];

//if ( nsimage == nil ) { NSLog( @"NSImage : nil" ); }


	return nsimage;
}

NSImage*
n_mac_image_nbmp2nsimage( const n_bmp *bmp )
{

	NSBitmapImageRep *rep = n_mac_image_NSBitmapImageRep( bmp );

	return n_mac_image_imagerep2nsimage( rep );
}

#define N_MAC_IMAGE_SAVE_PNG ( 1 )
#define N_MAC_IMAGE_SAVE_JPG ( 2 )

BOOL
n_mac_image_save( const n_bmp *bmp, n_posix_char *path, int mode )
{

	NSBitmapImageRep *n_rep = n_mac_image_NSBitmapImageRep( bmp );


	BOOL ret = NO;
/*
	// [x] : buggy : not-the-same image will be saved

	if ( mode == N_MAC_IMAGE_SAVE_PNG )
	{
		NSDictionary *property = [[NSDictionary alloc] init];

		NSData *data;
		data = [n_rep representationUsingType:NSBitmapImageFileTypePNG properties:property];
		ret = [data writeToFile:n_mac_str2nsstring( path ) atomically: NO];
	} else
*/
	if ( mode == N_MAC_IMAGE_SAVE_JPG )
	{
		NSDictionary *property = [[NSDictionary alloc] init];

		NSData *data;
		data = [n_rep representationUsingType:NSBitmapImageFileTypeJPEG properties:property];
		ret = [data writeToFile:n_mac_str2nsstring( path ) atomically: NO];
	}


	return ( ret == NO );
}

BOOL
n_mac_image_nsimage2nbmp( NSImage *image, n_bmp *bmp )
{

	[image lockFocus];

	NSRect rect = NSMakeRect( 0,0,image.size.width,image.size.height );

	NSGraphicsContext *context = [NSGraphicsContext currentContext];
	CGImageRef         ref     = [image CGImageForProposedRect:&rect context:context hints:nil];
	NSBitmapImageRep  *bmprep  = [[NSBitmapImageRep alloc] initWithCGImage:ref];

	[image unlockFocus];


	// [x] : NSBitmapImageFileTypeBMP : unknown format will be returned

	//NSData *data = [bmprep representationUsingType:NSBitmapImageFileTypeBMP properties:[[NSDictionary alloc] init]];
	//if ( data == nil ) { return TRUE; }


//u32 tick = n_posix_tickcount();

/*
	// [x] : heavy

	n_type_gfx sx = (n_type_gfx) image.size.width;
	n_type_gfx sy = (n_type_gfx) image.size.height;

	n_bmp_new( bmp, sx, sy );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{
		NSColor *c = [bmprep colorAtX:x y:y];
		n_bmp_ptr_set_fast( bmp, x,y, n_mac_nscolor2argb( c ) );

		x++;
		if ( x >= sx )
		{
			x = 0;

			y++;
			if ( y >= sy ) { break; }
		}
	}
*/

	NSData *data = [bmprep representationUsingType:NSBitmapImageFileTypePNG properties:[[NSDictionary alloc] init]];
	if ( data == nil ) { return TRUE; }

	n_png png; n_png_zero( &png );

	BOOL ret = n_png_load_onmemory( &png, (void*) [data bytes], [data length] );
	if ( ret ) { return TRUE; }

	ret = n_png_uncompress( &png, bmp );
	if ( ret ) { n_png_free( &png ); return TRUE; }

	n_png_free( &png );


//NSLog( @"%d", (int) n_posix_tickcount() - tick );


	return FALSE;
}

BOOL
n_mac_image_load( n_posix_char *path, n_bmp *bmp )
{

	NSImage *image = [[NSImage alloc] initWithContentsOfFile:n_mac_str2nsstring( path )];
	if ( image == nil ) { return TRUE; }


	return n_mac_image_nsimage2nbmp( image, bmp );
}

void
n_mac_image_nsimage_draw( NSImage *image, NSRect *rect, BOOL flip, BOOL fast )
{

	NSGraphicsContext *context = [NSGraphicsContext currentContext];
	CGImageRef         ref     = [image CGImageForProposedRect:rect context:context hints:nil];
	CGContextRef       cg_ctx  = [context CGContext];

	if ( flip )
	{
		CGContextTranslateCTM( cg_ctx, 0, image.size.height );
		CGContextScaleCTM( cg_ctx, 1.0, -1.0 );
	}

	// [x] : CatPad : Search Field : delete "x" : not drawn
	if ( fast )
	{
		CGContextSetBlendMode( cg_ctx, kCGBlendModeCopy );
	}

	CGContextDrawImage( cg_ctx, NSRectToCGRect( *rect ), ref );


	return;
}

void
n_mac_image_imagerep_draw( NSBitmapImageRep *rep, NSRect *rect, BOOL flip, BOOL fast )
{

	NSGraphicsContext *context = [NSGraphicsContext currentContext];
	CGImageRef         ref     = [rep CGImage];
	CGContextRef       cg_ctx  = [context CGContext];

	if ( flip )
	{
		CGContextTranslateCTM( cg_ctx, 0, rep.size.height );
		CGContextScaleCTM( cg_ctx, 1.0, -1.0 );
	}

	// [x] : CatPad : Search Field : delete "x" : not drawn
	if ( fast )
	{
		CGContextSetBlendMode( cg_ctx, kCGBlendModeCopy );
	}

	CGContextDrawImage( cg_ctx, NSRectToCGRect( *rect ), ref );


	return;
}

void
n_mac_image_nbmp_direct_draw( const n_bmp *bmp, NSRect *rect, BOOL flip )
{

//static u32 tick_prv = 0;

/*
	// [!] : slowest

	NSDrawBitmap
	(
		NSMakeRect( 0,0, N_BMP_SX( bmp ),N_BMP_SY( bmp ) ),
		N_BMP_SX( bmp ),
		N_BMP_SY( bmp ),
		8,
		4,
		0,
		(int) N_BMP_SX( bmp ) * 4,
		NO,
		YES,
		NSCalibratedRGBColorSpace,
		(void*) &N_BMP_PTR( bmp )
	);
*/

	NSImage *img = n_mac_image_nbmp2nsimage( bmp );


	// [!] : slower than CGContextDrawImage()

	//[img drawInRect:*rect];


	// [!] : fastest

	n_mac_image_nsimage_draw( img, rect, flip, NO );


//u32 tick_cur = n_posix_tickcount();

//NSLog( @"%d", (int) tick_cur - tick_prv );

//tick_prv = tick_cur;


	return;
}

void
n_mac_image_nbmp_direct_draw_fast( const n_bmp *bmp, NSRect *rect, BOOL flip )
{

	NSImage *img = n_mac_image_nbmp2nsimage( bmp );

	n_mac_image_nsimage_draw( img, rect, flip, YES );


	return;
}

void
n_mac_image_imagerep_alias_fast( NSBitmapImageRep *rep, n_bmp *bmp )
{

	// [!] : use when width and height are the same

	// [!] : don't n_bmp_free()

	N_BMP_PTR( bmp ) = (void*) [rep bitmapData];


	return;
}

void
n_mac_image_imagerep_alias( NSBitmapImageRep *rep, n_bmp *bmp )
{

	// [!] : don't n_bmp_free()

	n_type_gfx sx = (n_type_gfx) [rep pixelsWide];
	n_type_gfx sy = (n_type_gfx) [rep pixelsHigh];

	n_bmp_header( bmp, sx,sy );

	n_mac_image_imagerep_alias_fast( rep, bmp );


	return;
}

void
n_mac_image_imagerep_sync( NSBitmapImageRep *rep, n_bmp *bmp )
{

	n_bmp tmp; n_mac_image_imagerep_alias( rep, &tmp );

	n_bmp_flush_fastcopy( bmp, &tmp );


	return;
}

void
n_mac_image_nbmp_direct_draw_faster( NSBitmapImageRep *rep, NSRect *rect, BOOL flip )
{

	//NSImage *img = n_mac_image_imagerep2nsimage( rep );

	//n_mac_image_nsimage_draw( img, rect, flip, YES );

	n_mac_image_imagerep_draw( rep, rect, flip, YES );


	return;
}




CGSize
n_mac_image_text_pixelsize( NSString *text, NSFont *font )
{

	// [x] : buggy : this module will report insufficient size
	//
	//	make size larger than reported size
	//
	//	"Bounding Rect" means "minimum rectangle"
	//	use nsfont.maximumAdvancement.width for width
	//	use nsfont.ascender, fabs( .descender ), .capHeight for height
	//
	//	HiDPI : multiply with [_window backingScaleFactor]
	//	else small size will be reported

	if ( font == nil ) { NSLog( @"font is nil" ); return NSMakeSize( 0, 0 ); }

	CGRect frame = [text
		 boundingRectWithSize:CGSizeMake( CGFLOAT_MAX, CGFLOAT_MAX )
		 options:NSStringDrawingUsesLineFragmentOrigin
		 attributes:@{ NSFontAttributeName:font }
		 context:nil
	];

	CGSize size = CGSizeMake( frame.size.width, frame.size.height );


	return size;
}

void
n_mac_image_text_native
(
	NSImage  *image,
	NSString *text,
	NSFont   *font,
	NSColor  *color,
	CGFloat   ox,
	CGFloat   oy
)
{

	// [!] : this module is only working under drawRect


	CGFloat isx = image.size.width;
	CGFloat isy = image.size.height;

	CGSize size = n_mac_image_text_pixelsize( text, font );

	NSRect rect = NSMakeRect
	(
		( ( isx - size.width  ) / 2 ) + ox,
		( ( isy - size.height ) / 2 ) + oy,
		size.width,
		size.height
	);


	NSMutableDictionary *attr = [NSMutableDictionary dictionary];

	[attr setObject:font  forKey:NSFontAttributeName           ];
	[attr setObject:color forKey:NSForegroundColorAttributeName];


	[image lockFocus];

	[text drawInRect:rect withAttributes:attr];

	[image unlockFocus];


	return;
}

void
n_mac_image_text_effect
(
	NSImage  *image,
	NSString *text,
	NSFont   *font,
	NSColor  *color,
	CGFloat   ox,
	CGFloat   oy,
	CGFloat   parameter_effect
)
{

	n_type_gfx prm = parameter_effect;

	n_type_gfx x = -prm;
	n_type_gfx y = -prm;
	n_posix_loop
	{

		n_mac_image_text_native( image, text, font, color, ox + x,oy + y );

		x++;
		if ( x >= ( prm + 1 ) )
		{
			x = -prm;
			y++;
			if ( y >= ( prm + 1 ) ) { break; }
		}
	}


	return;
}

#define N_MAC_IMAGE_TEXT_NONE    ( 0 << 0 )
#define N_MAC_IMAGE_TEXT_CONTOUR ( 1 << 0 )
#define N_MAC_IMAGE_TEXT_SINK    ( 1 << 1 )
#define N_MAC_IMAGE_TEXT_POP     ( 1 << 2 )

void
n_mac_image_text_direct_draw
(
	NSImage  *image,
	NSString *text,
	NSFont   *font,
	u32       color_text,
	u32       color_effect,
	CGFloat   parameter_effect,
	CGFloat   ox,
	CGFloat   oy,
	int       mode
)
{

	// [!] : this module is only working under drawRect


	if ( mode & N_MAC_IMAGE_TEXT_CONTOUR )
	{

		NSColor *c = n_mac_argb2nscolor( color_effect );

		n_mac_image_text_effect( image, text, font, c, ox, oy, parameter_effect );

	} else
	if ( mode & N_MAC_IMAGE_TEXT_SINK )
	{

		NSColor *tl = n_mac_nscolor_argb( 255,  1,  1,  1 );
		NSColor *dr = n_mac_nscolor_argb( 255,255,255,255 );

		n_mac_image_text_native( image, text, font, tl, ox + 1,oy - 1 );
		n_mac_image_text_native( image, text, font, dr, ox - 1,oy + 1 );

	} else
	if ( mode & N_MAC_IMAGE_TEXT_POP )
	{

		// [x] : too much heavy : useless

		NSColor *c1 = n_mac_nscolor_argb( 255,0,0,0 );
		NSColor *c2 = n_mac_argb2nscolor( color_effect );

		n_mac_image_text_effect( image, text, font, c1, ox, oy, parameter_effect * 3 );
		n_mac_image_text_effect( image, text, font, c2, ox, oy, parameter_effect * 1 );

	}// else


	NSColor *c = n_mac_argb2nscolor( color_text );

	n_mac_image_text_native( image, text, font, c, ox,oy );


	return;
}




void
n_mac_image_rc_load_bmp( NSString *name, n_bmp *bmp )
{

	NSBundle *main = [NSBundle mainBundle];
	NSString *path = [main pathForResource:name ofType:@"bmp"];

	n_posix_char *str = n_mac_nsstring2str( path );

	BOOL ret = n_bmp_load( bmp, str );
	if ( ret == FALSE )
	{
		// [!] : do when needed manually
		//n_bmp_mac( bmp );
	}

	n_string_free( str );


	return;
}




void
n_mac_image_bmp_gray( n_bmp *bmp )
{

	if ( n_bmp_error( bmp ) ) { return; }


	u32 bg = n_mac_nscolor2argb( [NSColor windowBackgroundColor] );

	n_type_gfx x = 0;
	n_type_gfx y = 0;
	n_posix_loop
	{

		u32 color; n_bmp_ptr_get_fast( bmp, x,y, &color );
		if ( 0 != n_bmp_a( color ) )
		{
			color = n_bmp_blend_pixel( bg, color, 0.5 );
			n_bmp_ptr_set_fast( bmp, x,y, color );
		}

		x++;
		if ( x >= N_BMP_SX( bmp ) )
		{
			x = 0;
			y++;
			if ( y >= N_BMP_SY( bmp ) ) { break; }
		}
	}


	n_bmp_flush_grayscale( bmp );


	return;
}




void
n_mac_image_debug_save( n_bmp *bmp_save )
{

	if ( n_bmp_error( bmp_save ) ) 
	{
NSLog( @"n_mac_debug_save() : Error" );
		return;
	}


	static int i = 0;

	NSArray      *paths   = NSSearchPathForDirectoriesInDomains( NSDesktopDirectory, NSUserDomainMask, YES );
	NSString     *desktop = [paths objectAtIndex:0];
	n_posix_char  tmpname[ 100 ]; n_string_path_tmpname( tmpname );
	NSString     *name    = [NSString stringWithFormat:@"%@/%s_%d.png", desktop, tmpname, i ];
//NSLog( @"%@", name );

	i++;


	n_bmp bmp; n_bmp_carboncopy( bmp_save, &bmp );


	n_bmp_mac_color( &bmp );


	n_posix_char *str = n_mac_nsstring2str( name );

	n_png png = n_png_template;

	n_png_compress( &png, &bmp );
	n_png_save( &png, str );

	n_png_free( &png );

	n_memory_free( str );


	n_bmp_free( &bmp );


	return;
}




#endif // _H_NONNON_MAC_IMAGE


