// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@implementation NonnonTxtbox (NonnonTxtboxDragAndDrop)


-(BOOL) n_txtbox_open:(NSString*)nsstring
{

	n_posix_char *str = n_mac_nsstring2str( nsstring );

	BOOL ret = n_txt_load_utf8( n_txt_data, str );
	if ( ret ) { n_string_free( str ); return FALSE; }


	if (
		( n_txt_data->unicode == N_TXT_UNICODE_NIL )
		&&
		( n_txt_data->newline != N_TXT_NEWLINE_BINARY )
	)
	{
		n_mac_window_dialog_ok( "Your file seems to be Non-Unicode." );
	}


	n_txt_decoration_init( n_txt_data, n_txt_deco );

//NSLog( @"Unicode %d : Newline %d", n_txt->unicode, n_txt->newline );

	n_path = nsstring;

	[self NonnonTxtboxReset];
	[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

	n_edited = FALSE;
	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
	{
		[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
	}

	[self NonnonTxtboxRedraw];


	n_string_free( str );


	[self.delegate NonnonTxtbox_delegate_dropped];


	return YES;
}




-(NSDragOperation) draggingEntered:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingEntered" );

	// [!] : call when hovered

	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX ) { return NSDragOperationNone; }
	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX ) { return NSDragOperationNone; }
	if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE ) { return NSDragOperationNone; }

	return NSDragOperationCopy;
}

-(void) draggingExited:( id <NSDraggingInfo> )sender
{
//NSLog( @"draggingExited" );
}

-(BOOL) prepareForDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"prepareForDragOperation" );

	return YES;
}

-(BOOL) performDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"performDragOperation" );

	return YES;
}

-(void) concludeDragOperation:( id <NSDraggingInfo> )sender
{
//NSLog( @"concludeDragOperation" );

	//[self setStringValue:@""];

	NSPasteboard *pasteboard = [sender draggingPasteboard];
	NSURL        *url        = [NSURL URLFromPasteboard:pasteboard];
	NSString     *nsstring   = [url path];

	[self n_txtbox_open:nsstring];

	drag_timer_queue = FALSE;

	n_mac_foreground();

}

-(NSDragOperation) draggingUpdated:( id <NSDraggingInfo> ) sender
{
//NSLog( @"draggingUpdated" );

	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX ) { return NSDragOperationNone; }
	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX ) { return NSDragOperationNone; }
	if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE ) { return NSDragOperationNone; }

	return NSDragOperationCopy;
}


@end

