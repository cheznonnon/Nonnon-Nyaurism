// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@implementation NonnonTxtbox (NonnonTxtboxKeyboard)


- (BOOL)acceptsFirstResponder
{
//NSLog(@"acceptsFirstResponder");
	return YES;
}

- (BOOL)becomeFirstResponder
{
//NSLog(@"becomeFirstResponder");

	n_txtbox_first_responder = self;

        return YES;
}




- (BOOL) NonnonTxtboxKeyboardIsFullStop
{

	BOOL ret = FALSE;

	if (
		( txtbox->caret_fr.cch.x == 0 )&&( txtbox->caret_fr.cch.y == 0 )
		&&
		( txtbox->caret_to.cch.x == 0 )&&( txtbox->caret_to.cch.y == 0 )

	)
	{
		ret = TRUE;
	}

	return ret;
}

- (void) NonnonTxtboxKeyboardMoveDetect:(void*) zero ud:(n_type_int)ud lr:(n_type_int)lr oneline:(BOOL)oneline_override
{
//NSLog( @"NonnonTxtboxKeyboardMoveDetect %f", txtbox->focus );


	// [!] : unselect
/*
	if ( ( caret_fr.pxl.x != caret_to.pxl.x )&&( caret_fr.cch.y != caret_to.cch.y ) )
	{
//NSLog( @"!" );
		caret_fr = caret_to;
	}
*/

//NSLog( @"%d", shift_selection_is_tail );


	txtbox->focus += ud;
	if ( txtbox->focus < 0 )
	{
		if ( txtbox->focus <= -1 ) { txtbox->caret_to.cch.x = 0; }
		txtbox->focus = 0;
	}
	if ( txtbox->focus >= txtbox->txt_data->sy ) { txtbox->focus = txtbox->txt_data->sy - 1; }


	if ( ud )
	{
		NSRect r = NSMakeRect( txtbox->padding, txtbox->offset_y, [self frame].size.width - ( txtbox->offset_x * 2 ), txtbox->font_size.height );

		txtbox->caret_to = n_txtbox_caret_detect_pixel2caret
		(
			txtbox->txt_data,
			txtbox->focus,
			r,
			txtbox->font,
			txtbox->font_size,
			NSMakePoint( txtbox->padding + txtbox->caret_to.pxl.x, txtbox->caret_to.cch.y + txtbox->font_size.height )
		);
//NSLog( @"%0.2f", caret_to.pxl.x );

		// [!] : Win32 compatible behavior 
		if ( txtbox->caret_to.pxl.x == 0 ) { txtbox->caret_to.cch.x = 0; }
	}


	if ( lr != 0 )
	{

		BOOL oneline = oneline_override;
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( self.txtbox->listbox_edit_onoff ) { oneline = TRUE; } else { return; }
		}

		n_posix_char *s = n_txt_get( txtbox->txt_data, txtbox->focus );
		if ( lr < 0 )
		{
			if ( txtbox->caret_to.cch.x == 0 )
			{
				if ( oneline )
				{
					//
				} else
				if ( txtbox->focus != 0 )
				{
					txtbox->focus--;
					txtbox->caret_to.cch.x = txtbox->caret_to.cch.x = n_posix_strlen( n_txt_get( txtbox->txt_data, txtbox->focus ) );
				}
			} else {
				txtbox->caret_to.cch.x = n_mac_txtbox_caret_move( s, txtbox->caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_L );
			}
		} else {
			if ( txtbox->caret_to.cch.x == n_posix_strlen( n_txt_get( txtbox->txt_data, txtbox->focus ) ) )
			{
				n_type_int max_sy = txtbox->txt_data->sy - 1;
				if ( oneline )
				{
					//
				} else
				if ( txtbox->focus < max_sy )
				{
					txtbox->focus++;
					txtbox->caret_to.cch.x = 0;
				}
			} else {
				txtbox->caret_to.cch.x = n_mac_txtbox_caret_move( s, txtbox->caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_R );	
			}
		}
	}


	txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
	(
		txtbox->txt_data,
		txtbox->focus,
		txtbox->font,
		txtbox->font_size,
		txtbox->caret_to.cch.x
	);


	[self NonnonTxtboxCaretOutOfCanvasUpDown];


	return;
}

- (n_caret) NonnonTxtboxKeyboardShiftSelectionDetect:(void*)zero lr:(n_type_int)lr cch:(n_type_int)cch
{
//NSLog( @"NonnonTxtboxKeyboardShiftSelectionDetect %f", txtbox->focus );

	n_posix_char *s = n_txt_get( txtbox->txt_data, txtbox->focus );
	if ( lr < 0 )
	{
		cch = n_mac_txtbox_caret_move( s, cch, YES, N_MAC_TXTBOX_CARET_MOVE_L );
	} else {
		cch = n_mac_txtbox_caret_move( s, cch, YES, N_MAC_TXTBOX_CARET_MOVE_R );
	}
//NSLog( @"%lld", (*cch) );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		txtbox->txt_data,
		txtbox->focus,
		txtbox->font,
		txtbox->font_size,
		cch
	);


	return caret;
}




- (void) flagsChanged : (NSEvent*) theEvent
{

	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_SHIFT )
	{
		[self.delegate NonnonTxtbox_delegate_shift:theEvent];
	}

}

- (void) keyUp : (NSEvent*) event
{

	txtbox->is_key_input = FALSE;

}

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d : Chars %@", event.keyCode, event.characters );


	txtbox->is_key_input = TRUE;


	txtbox->caret_blink_force_onoff = TRUE;


#ifdef N_TXTBOX_IME_ENABLE

//NSLog( @"IME On/Off : %@ : %d", ime_nsstr, ime_onoff );

	NSString *ime_nsstr_prv = [txtbox->ime_nsstr copy];

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		if ( self.txtbox->listbox_edit_onoff )
		{
			txtbox->ime = [NSTextInputContext currentInputContext];
			[txtbox->ime handleEvent:event];
		}
	} else {
		txtbox->ime = [NSTextInputContext currentInputContext];
		[txtbox->ime handleEvent:event];
	}

	if ( txtbox->ime_onoff )
	{
		if ( event.keyCode == N_MAC_KEYCODE_BACKSPACE )
		{
//NSLog( @"%@ %d : %@", ime_nsstr, (int) [ime_nsstr length], ime_nsstr_prv );

			NSUInteger cch = [txtbox->ime_nsstr length];

			if ( ( cch == 1 )&&( [txtbox->ime_nsstr compare:ime_nsstr_prv] == 0 ) )
			{
				txtbox->ime_freed = TRUE;
			}

			if ( txtbox->ime_freed )
			{
				[txtbox->ime discardMarkedText];
				txtbox->ime_nsstr = @"";
				txtbox->ime_onoff = FALSE;
				txtbox->ime_freed = FALSE;
				cch               = 0;
			}

			if ( cch == 1 )
			{
				txtbox->ime_freed = TRUE;
			}

			[self NonnonTxtboxRedraw];

		} else
		if ( ( event.keyCode == N_MAC_KEYCODE_RETURN )||( event.keyCode == N_MAC_KEYCODE_ENTER ) )
		{
//NSLog( @"return 1" );
			if ( txtbox->ime_is_first == FALSE )
			{
				[self NonnonTxtboxInsert:txtbox->ime_nsstr];
			}

			[txtbox->ime discardMarkedText];
			txtbox->ime_nsstr = @"";
			txtbox->ime_delay = FALSE;
			txtbox->ime_onoff = FALSE;

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];
			[self NonnonTxtboxRedraw];
		} else {
			if ( event.keyCode == N_MAC_KEYCODE_SPACE )
			{
				[self NonnonTxtboxRedraw];
			} else
			if ( event.keyCode == N_MAC_KEYCODE_ARROW_LEFT )
			{
				//
			} else
			if ( event.keyCode == N_MAC_KEYCODE_ARROW_RIGHT )
			{
				//
			} else {
				[self NonnonTxtboxRedraw];
			}
		}

		return;
	} else
	if ( txtbox->ime_delay )
	{
		if ( ( event.keyCode == N_MAC_KEYCODE_RETURN )||( event.keyCode == N_MAC_KEYCODE_ENTER ) )
		{
//NSLog( @"return 2" );

			[txtbox->ime discardMarkedText];
			txtbox->ime_nsstr = @"";
			txtbox->ime_delay = FALSE;
			txtbox->ime_onoff = FALSE;

			txtbox->caret_blink_force_onoff = FALSE;
			n_bmp_fade_init( &txtbox->caret_blink_fade, n_bmp_black );

			[self NonnonTxtboxRedraw];

			return;
		}
	}

#endif


	switch( event.keyCode ) {

	case N_MAC_KEYCODE_HOME:
	case N_MAC_KEYCODE_END:
	case N_MAC_KEYCODE_INSERT:
	case N_MAC_KEYCODE_DELETE:
	case N_MAC_KEYCODE_PRTSCN:
	case N_MAC_KEYCODE_F1:
	case N_MAC_KEYCODE_F2:
	//case N_MAC_KEYCODE_F3: // see below
	case N_MAC_KEYCODE_F4:
	//case N_MAC_KEYCODE_F5: // see below
	case N_MAC_KEYCODE_F6:
	case N_MAC_KEYCODE_F7:
	case N_MAC_KEYCODE_F8:
	case N_MAC_KEYCODE_F9:
	case N_MAC_KEYCODE_F10:
	case N_MAC_KEYCODE_F12:
	case N_MAC_KEYCODE_SPACE:
	{

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_PAGE_UP :
	{

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		}

		CGFloat csy              = self.frame.size.height - ( txtbox->offset_y * 2 );
		CGFloat items_per_canvas = csy / txtbox->font_size.height;

		txtbox->scr.unit_pos -= items_per_canvas;

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_PAGE_DOWN :
	{

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		}

		CGFloat csy              = self.frame.size.height - ( txtbox->offset_y * 2 );
		CGFloat items_per_canvas = csy / txtbox->font_size.height;

		txtbox->scr.unit_pos += items_per_canvas;

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_ARROW_UP :
	{
//NSLog( @"Up" );

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			if ( self.txtbox->listbox_edit_onoff )
			{
				//
			} else
			if ( txtbox->focus >= 1 )
			{

				if ( event.modifierFlags & NSEventModifierFlagShift )
				{
					if ( delegate_option & N_MAC_TXTBOX_DELEGATE_SWAP )
					{
						[self.delegate NonnonTxtbox_delegate_swap:self is_up:YES];
					}
				} else {
					txtbox->focus--;
					self.txtbox->listbox_edit_onoff = FALSE;
					[self NonnonTxtboxCaretOutOfCanvasUpDown];
				}

				[self NonnonTxtboxRedraw];

				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_LISTBOX_MOVED )
				{
					[delegate keyDown:event];
				}
			}

			break;
		}


		if ( event.modifierFlags & NSEventModifierFlagShift )
		{

			n_caret caret = txtbox->caret_fr;

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:-1 lr:0 oneline:FALSE];

			txtbox->caret_fr = caret;

		} else {

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:-1 lr:0 oneline:FALSE];

		}

		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_DOWN:
	{
//NSLog( @"Down" );

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			if ( self.txtbox->listbox_edit_onoff )
			{
				//
			} else
			if ( txtbox->focus <= ( txtbox->txt_data->sy - 1 - 1 ) )
			{
			
				if ( event.modifierFlags & NSEventModifierFlagShift )
				{
					if ( delegate_option & N_MAC_TXTBOX_DELEGATE_SWAP )
					{
						[self.delegate NonnonTxtbox_delegate_swap:self is_up:NO];
					}
				} else {
					txtbox->focus++;
					self.txtbox->listbox_edit_onoff = FALSE;
					[self NonnonTxtboxCaretOutOfCanvasUpDown];
				}

				[self NonnonTxtboxRedraw];

				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_LISTBOX_MOVED )
				{
					[delegate keyDown:event];
				}
			}

			break;
		}


		if ( event.modifierFlags & NSEventModifierFlagShift )
		{

			n_caret caret = txtbox->caret_fr;

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:1 lr:0 oneline:FALSE];

			txtbox->caret_fr = caret;

		} else {

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:1 lr:0 oneline:FALSE];

		}

		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_LEFT:
	{
//NSLog( @"Left" );

		if ( event.modifierFlags & NSEventModifierFlagShift )
		{
			n_caret caret = txtbox->caret_fr;
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:-1 oneline:TRUE];
			txtbox->caret_fr = caret;

			txtbox->shift_selection_is_tail = FALSE;
		} else
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			[self NonnonTxtboxDoubleclickDetect:NO tab_patch:NO];
			txtbox->caret_to = txtbox->caret_fr;

			n_posix_char *line = n_txt_get( txtbox->txt_data, txtbox->focus );
			txtbox->caret_to.cch.x = txtbox->caret_fr.cch.x = n_mac_txtbox_caret_move( line, txtbox->caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_L );
		} else {
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:-1 oneline:FALSE];
		}

		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:
	{
//NSLog( @"Right" );
//break;

		if ( event.modifierFlags & NSEventModifierFlagShift )
		{
			n_caret caret = txtbox->caret_fr;
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:1 oneline:TRUE];
			txtbox->caret_fr = caret;

			txtbox->shift_selection_is_tail = TRUE;
		} else
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			[self NonnonTxtboxDoubleclickDetect:NO tab_patch:NO];
			txtbox->caret_fr = txtbox->caret_to;
		} else {
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:1 oneline:FALSE];
		}

		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_RETURN:
	case N_MAC_KEYCODE_ENTER:
	{

		if ( txtbox->txt_data->readonly ) { break; }


		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{

			self.txtbox->is_enter_pressed = TRUE;
			[self NonnonTxtboxEditedNotifyForced:TRUE];
			self.txtbox->is_enter_pressed = FALSE;

		} else
		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			//
		} else
		if ( txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
//NSLog( @"1" );
			if ( txtbox->listbox_no_edit ) { break; }

			if ( txtbox->listbox_edit_onoff )
			{
//NSLog( @"2" );
				txtbox->listbox_edit_onoff = FALSE;
			} else {
//NSLog( @"3" );
				txtbox->listbox_edit_onoff = TRUE;

				txtbox->listbox_edit_index = txtbox->focus;

				txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
				(
					txtbox->txt_data,
					txtbox->focus,
					txtbox->font,
					txtbox->font_size,
					n_posix_strlen( n_txt_get( txtbox->txt_data, txtbox->focus ) )
				);
			}

		} else
		if ( n_mac_txtbox_del( txtbox->txt_data, &txtbox->focus, &txtbox->caret_fr, &txtbox->caret_to ) )
		{

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				txtbox->caret_fr.cch.x
			);

//n_caret_debug_cch( caret_fr ,caret_to );

		} else {
//NSLog( @"divide" );

			n_posix_char *line = n_txt_get( txtbox->txt_data, txtbox->focus );
			n_posix_char *s    = n_string_carboncopy( line );

			n_txt_add( txtbox->txt_data, txtbox->focus + 1, &s[ txtbox->caret_fr.cch.x ] );
			line[ txtbox->caret_fr.cch.x ] = N_STRING_CHAR_NUL;

			n_string_free( s );


			txtbox->focus++;
			txtbox->caret_fr = txtbox->caret_to = NonnonMakeCaret( 0, txtbox->focus * txtbox->font_size.height, 0, txtbox->focus );

			[self NonnonTxtboxCaretOutOfCanvasUpDown];


			self.txtbox->is_enter_pressed = TRUE;
			[self NonnonTxtboxEditedNotify:TRUE];
			self.txtbox->is_enter_pressed = FALSE;

		}

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_BACKSPACE:
	{

		if ( txtbox->txt_data->readonly ) { break; }


		if ( [self NonnonTxtboxKeyboardIsFullStop] )
		{
			break;
		}


		[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];


//n_caret_debug_cch( caret_fr ,caret_to );// break;

		BOOL grow = ( txtbox->caret_fr.cch.y < txtbox->caret_to.cch.y );

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( self.txtbox->listbox_edit_onoff )
			{
				n_posix_char *line = n_txt_get( txtbox->txt_data, txtbox->focus );
				n_type_int     cch = n_mac_txtbox_caret_move( line, txtbox->caret_fr.cch.x, NO, N_MAC_TXTBOX_CARET_MOVE_L );

				n_posix_snprintf_literal( &line[ cch ], n_posix_strlen( line ) - cch + 1, "%s", &line[ txtbox->caret_fr.cch.x ] );

				if ( grow ) { txtbox->focus = txtbox->caret_fr.cch.y; }

				txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
				(
					txtbox->txt_data,
					txtbox->focus,
					txtbox->font,
					txtbox->font_size,
					cch
				);

				[self NonnonTxtboxRedraw];
			}

			break;
		}

		if ( n_mac_txtbox_del( txtbox->txt_data, &txtbox->focus, &txtbox->caret_fr, &txtbox->caret_to ) )
		{
//NSLog( @"Backspace : deleted : %0.2f %lld %lld", txtbox->focus, caret_fr.cch.y, caret_to.cch.y );

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				txtbox->caret_fr.cch.x
			);

			[self NonnonTxtboxEditedNotify:TRUE];

			[self NonnonTxtboxRedraw];

			break;
		}

		if (
			( self.txtbox->mode == N_MAC_TXTBOX_MODE_EDITBOX )
			&&
			( txtbox->caret_fr.cch.x == 0 )
		)
		{
//NSLog( @"Backspace : zero" );

			if ( txtbox->focus == 0 )
			{
				// [!] : for select all
				txtbox->caret_fr = txtbox->caret_to = NonnonMakeCaret( 0,0,0,0 );
				[self NonnonTxtboxRedraw];
				break;
			}

			n_posix_char *line_1 = n_txt_get( txtbox->txt_data, txtbox->focus - 1 );
			n_posix_char *line_2 = n_txt_get( txtbox->txt_data, txtbox->focus     );
			n_type_int     cch_1 = n_posix_strlen( line_1 );
			n_type_int     cch_2 = n_posix_strlen( line_2 );

			n_posix_char  *s     = n_string_new( cch_1 + cch_2 );
			n_posix_snprintf_literal( s, cch_1 + cch_2 + 1, "%s%s", line_1, line_2 );

			n_txt_del( txtbox->txt_data, txtbox->focus );
			n_txt_mod( txtbox->txt_data, txtbox->focus - 1, s );

			txtbox->focus--;

			txtbox->caret_fr.cch.x = txtbox->caret_to.cch.x = cch_1;

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				txtbox->caret_fr.cch.x
			);

			n_string_free( s );

			[self NonnonTxtboxCaretOutOfCanvasUpDown];

		} else {
//NSLog( @"Backspace : normal" );

			n_posix_char *line = n_txt_get( txtbox->txt_data, txtbox->focus );
			n_type_int     cch = n_mac_txtbox_caret_move( line, txtbox->caret_fr.cch.x, NO, N_MAC_TXTBOX_CARET_MOVE_L );

			n_posix_snprintf_literal( &line[ cch ], n_posix_strlen( line ) - cch + 1, "%s", &line[ txtbox->caret_fr.cch.x ] );

			if ( grow ) { txtbox->focus = txtbox->caret_fr.cch.y; }

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				cch
			);

		}

		[self NonnonTxtboxEditedNotify:TRUE];

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_TAB:
	{

		if ( txtbox->txt_data->readonly ) { break; }


		// [!] : same as Xcode

		BOOL selected = ( 1 <= llabs( txtbox->caret_fr.cch.y - txtbox->caret_to.cch.y ) );

		[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];


		// [x] : you cannot use Cmd + Tab : system uses

		//if ( event.modifierFlags & NSEventModifierFlagCommand )
		if ( event.modifierFlags & NSEventModifierFlagShift )
		{
			// [!] : multi-line tab remover

			if ( selected == FALSE )
			{
				[self NonnonTxtboxKeyboardInputMethod:[event characters]];
				break;
			}

			n_type_int count = llabs( txtbox->caret_fr.cch.y - txtbox->caret_to.cch.y ) + 1;
			n_type_int min_y = MIN( txtbox->caret_fr.cch.y, txtbox->caret_to.cch.y );

			n_type_int y = 0;
			n_posix_loop
			{
				if ( y >= count ) { break; }

				n_posix_char *line = n_txt_get( txtbox->txt_data, min_y + y );

				if ( line[ 0 ] == N_STRING_CHAR_TAB )
				{
					n_type_int    cch = n_posix_strlen( line ) + 1;
					n_posix_char *str = n_string_new( cch );
					n_posix_snprintf_literal( str, cch + 1, "%s", &line[ 1 ] );

					n_txt_mod_fast( txtbox->txt_data, min_y + y, str );
				}

				y++;
			}


//NSLog( @"%lld %lld", caret_fr.cch.y, caret_to.cch.y );
			if ( txtbox->caret_fr.cch.y < txtbox->caret_to.cch.y )
			{
				txtbox->caret_fr.cch.x = 0;
				txtbox->caret_fr.pxl.x = 0;

				txtbox->caret_to.cch.x = txtbox->txt_data->sx;
				txtbox->caret_to.pxl.x = txtbox->font_size.width * txtbox->caret_to.cch.x;
			} else
			if ( txtbox->caret_fr.cch.y > txtbox->caret_to.cch.y )
			{
				txtbox->caret_to.cch.x = 0;
				txtbox->caret_to.pxl.x = 0;

				txtbox->caret_fr.cch.x = txtbox->txt_data->sx;
				txtbox->caret_fr.pxl.x = txtbox->font_size.width * txtbox->caret_fr.cch.x;

			}

			[self NonnonTxtboxEditedNotify:TRUE];

			[self NonnonTxtboxRedraw];
		} else {
			// [!] : multi-line tab adder

			if ( selected == FALSE )
			{
				[self NonnonTxtboxKeyboardInputMethod:[event characters]];
				break;
			}

			n_type_int count = llabs( txtbox->caret_fr.cch.y - txtbox->caret_to.cch.y ) + 1;
			n_type_int min_y =   MIN( txtbox->caret_fr.cch.y,  txtbox->caret_to.cch.y );

			n_type_int y = 0;
			n_posix_loop
			{
				if ( y >= count ) { break; }

				n_posix_char *line = n_txt_get( txtbox->txt_data, min_y + y );

				if ( FALSE == n_string_is_empty( line ) )
				{
					n_type_int    cch = n_posix_strlen( line ) + 1;
					n_posix_char *str = n_string_new( cch );
					n_posix_snprintf_literal( str, cch + 1, "%s%s", N_STRING_TAB, line );

					n_txt_mod_fast( txtbox->txt_data, min_y + y, str );
				}

				y++;
			}


//NSLog( @"%lld %lld", caret_fr.cch.y, caret_to.cch.y );
			if ( txtbox->caret_fr.cch.y < txtbox->caret_to.cch.y )
			{
				txtbox->caret_fr.cch.x = 0;
				txtbox->caret_fr.pxl.x = 0;

				txtbox->caret_to.cch.x = txtbox->txt_data->sx;
				txtbox->caret_to.pxl.x = txtbox->font_size.width * txtbox->caret_to.cch.x;
			} else
			if ( txtbox->caret_fr.cch.y > txtbox->caret_to.cch.y )
			{
				txtbox->caret_to.cch.x = 0;
				txtbox->caret_to.pxl.x = 0;

				txtbox->caret_fr.cch.x = txtbox->txt_data->sx;
				txtbox->caret_fr.pxl.x = txtbox->font_size.width * txtbox->caret_fr.cch.x;

			}

			[self NonnonTxtboxEditedNotify:TRUE];

			[self NonnonTxtboxRedraw];
		}

	}
	break;

	case N_MAC_KEYCODE_CUT:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

			[self NonnonTxtboxEditedNotify:TRUE];


			NSString *s = n_mac_txtbox_copy( txtbox->txt_data, txtbox->caret_fr.cch.x, txtbox->caret_fr.cch.y, txtbox->caret_to.cch.x, txtbox->caret_to.cch.y );

			NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
			[pasteboard clearContents];
			[pasteboard writeObjects:@[ s ]];

			n_mac_txtbox_del( txtbox->txt_data, &txtbox->focus, &txtbox->caret_fr, &txtbox->caret_to );

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				txtbox->caret_fr.cch.x
			);

			[self NonnonTxtboxRedraw];

		} else {

			[self NonnonTxtboxKeyboardInputMethod:[event characters]];

		}
	}
	break;

	case N_MAC_KEYCODE_COPY: 
	{
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			NSString *s = n_mac_txtbox_copy( txtbox->txt_data, txtbox->caret_fr.cch.x, txtbox->caret_fr.cch.y, txtbox->caret_to.cch.x, txtbox->caret_to.cch.y );
//NSLog( @"%@", s );
//n_caret_debug_cch( caret_fr, caret_to );

			if ( s == nil ) { break; }

			NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
			[pasteboard clearContents];
			[pasteboard writeObjects:@[ s ]];
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}
	}
	break;

	case N_MAC_KEYCODE_PASTE:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

			[self NonnonTxtboxEditedNotify:TRUE];

			NSPasteboard *p = [NSPasteboard generalPasteboard];
			NSString     *s = [p stringForType:NSPasteboardTypeString];
			n_posix_char *c = n_mac_nsstring2str( s );
//NSLog( @"%@", s );
//NSLog( @"%s", c );
/*
int i = 0;
n_posix_loop
{
	if ( c[ i ] == 0 ) { break; }
	
	if ( c[ i ] == N_STRING_CHAR_LF ) { c [i] = 'Z'; }
	
	i++;
}
*/

			n_type_int cch = n_posix_strlen( c );


			// [Patch] : vector_load() may have a bug
			BOOL patch = FALSE;
			if ( ( cch > 0 )&&( c[ cch - 1 ] == N_STRING_CHAR_LF ) )
			{
//NSLog( @"patched" );
				patch = TRUE;
			}

			n_txt t; n_txt_zero( &t ); n_txt_load_utf8_onmemory( &t, c, cch );
//NSLog( @"%lld", t.sy );

			if (
				( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
				||
				( self.txtbox->mode == N_MAC_TXTBOX_MODE_ONELINE )
			)
			{
				if ( t.sy != 1 ) { n_txt_free( &t ); break; }

				n_posix_char *s = n_txt_get( &t, 0 );
				n_string_replace( s, s, N_STRING_LF, N_STRING_NUL );
			} else {
				if ( patch )
				{
					n_txt_add( &t, t.sy, N_STRING_EMPTY );
				}
			}


			n_mac_txtbox_del( txtbox->txt_data, &txtbox->focus, &txtbox->caret_fr, &txtbox->caret_to );

			if ( t.sy == 1 )
			{

				n_posix_char *line_f = n_string_carboncopy( n_txt_get( txtbox->txt_data, txtbox->focus ) );
				n_posix_char *line_m = n_string_carboncopy( n_txt_get(               &t,             0 ) );
				n_posix_char *line_t = n_string_carboncopy( n_txt_get( txtbox->txt_data, txtbox->focus ) );
				
				line_f[ txtbox->caret_fr.cch.x ] = N_STRING_CHAR_NUL;
				n_posix_snprintf( line_t, n_posix_strlen( line_t ) + 1, "%s", &line_t[ txtbox->caret_fr.cch.x ] );
//NSLog( @"%s", line_f );
//NSLog( @"%s", line_t );

				n_type_int    cch = n_posix_strlen( line_f ) + n_posix_strlen( line_m ) + n_posix_strlen( line_t );
				n_posix_char *str = n_string_new( cch );
				n_posix_snprintf_literal( str, cch + 1, "%s%s%s", line_f, line_m, line_t );

				n_txt_mod( txtbox->txt_data, txtbox->focus, str );

				n_string_free( str );


				txtbox->caret_fr.cch.x = txtbox->caret_to.cch.x = txtbox->caret_fr.cch.x + n_posix_strlen( line_m );

				txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
				(
					txtbox->txt_data,
					txtbox->focus,
					txtbox->font,
					txtbox->font_size,
					txtbox->caret_fr.cch.x
				);


				n_string_free( line_f );
				n_string_free( line_m );
				n_string_free( line_t );

			} else {
//break;
				if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX ) { break; }


				n_posix_char *line_f = n_string_carboncopy( n_txt_get( txtbox->txt_data, txtbox->focus ) );
				n_posix_char *line_t = n_string_carboncopy( n_txt_get( txtbox->txt_data, txtbox->focus ) );

				line_f[ txtbox->caret_fr.cch.x ] = N_STRING_CHAR_NUL;
				n_posix_snprintf( line_t, n_posix_strlen( line_t ) + 1, "%s", &line_t[ txtbox->caret_fr.cch.x ] );
//NSLog( @"F:%s", line_f );
//NSLog( @"T:%s", line_t );

				{
					n_type_int    cch = n_posix_strlen( line_f ) + n_posix_strlen( n_txt_get( &t, 0 ) );
					n_posix_char *str = n_string_new( cch );

					n_posix_snprintf_literal( str, cch + 1, "%s%s", line_f, n_txt_get( &t, 0 ) );
					n_txt_mod_fast( txtbox->txt_data, txtbox->focus, str );
				}

				n_type_int i = 1;
				n_posix_loop
				{//break;
					if ( i >= t.sy ) { break; }

					n_txt_add( txtbox->txt_data, txtbox->focus + i, n_txt_get( &t, i ) );

					i++;
				}

				{
					i--;

					n_type_int    cch = n_posix_strlen( n_txt_get( &t, i ) ) + n_posix_strlen( line_t );
					n_posix_char *str = n_string_new( cch );

					n_posix_snprintf_literal( str, cch + 1, "%s%s", n_txt_get( &t, i ), line_t );
					n_txt_mod_fast( txtbox->txt_data, txtbox->focus + i, str );
				}


				n_type_int p_focus = txtbox->caret_fr.cch.y;

				txtbox->focus = txtbox->focus + i;

				txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
				(
					txtbox->txt_data,
					txtbox->focus,
					txtbox->font,
					txtbox->font_size,
					n_posix_strlen( n_txt_get( &t, i ) )
				);


				CGFloat csy              = self.frame.size.height - ( txtbox->offset_y * 2 );
				CGFloat items_per_canvas = csy / txtbox->font_size.height;
				CGFloat edge             = trunc( txtbox->scr.unit_pos + items_per_canvas );
//NSLog( @"Focus %lld : Edge %0.2f : Scroll %0.2f", txtbox->focus, edge, scroll );

				if ( p_focus < txtbox->scr.unit_pos )
				{
					txtbox->scr.unit_pos = p_focus;
					[self NonnonTxtboxDrawScrollClamp];
					edge = trunc( txtbox->scr.unit_pos + items_per_canvas );
				}

				if ( txtbox->focus >= edge )
				{
					txtbox->scr.unit_pos = txtbox->scr.unit_pos + ( txtbox->focus - edge );
					txtbox->scr.unit_pos = ceil( txtbox->scr.unit_pos ) + 1;
				}


				n_string_free( line_f );
				n_string_free( line_t );

			}

			n_txt_free( &t );


			[self NonnonTxtboxRedraw];

		} else {

			[self NonnonTxtboxKeyboardInputMethod:[event characters]];

		}

	}
	break;

	case N_MAC_KEYCODE_SELECT_ALL:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX ) { break; }

			[self NonnonTxtboxSelectAll:TRUE];
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	case N_MAC_KEYCODE_UNDO:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_RESTORE];

			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_UNDO )
			{
				[self.delegate NonnonTxtbox_delegate_undo:event];
			}

			[self NonnonTxtboxRedraw];
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	case N_MAC_KEYCODE_FIND:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_FIND )
			{
				[self.delegate NonnonTxtbox_delegate_find:self];
			}
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	case N_MAC_KEYCODE_SAVE:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			//if ( n_txt_save_utf8( txtbox->txt_data, n_mac_nsstring2str( txtbox->path ) ) )
			{
//NSLog( @"n_txt_save_utf8() failed : %s", n_mac_nsstring2str( path ) );
			}
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	case N_MAC_KEYCODE_F3:
	{

		if ( delegate_option & N_MAC_TXTBOX_DELEGATE_F3 )
		{
			BOOL is_left = YES;
			if ( event.modifierFlags & NSEventModifierFlagCommand )
			{
				is_left = NO;
			}
			[self.delegate NonnonTxtbox_delegate_F3:self is_left:is_left];
		}

	}
	break;

	case N_MAC_KEYCODE_F5:
	{

		// [x] : Command + function keys used by system globally

		if ( txtbox->txt_data->readonly ) { break; }

		n_posix_char str[ 100 ];

		if ( event.modifierFlags & NSEventModifierFlagOption )
		{
			n_txtbox_day_of_week( str, 100 );
		} else {
			n_txtbox_date( str, 100 );
		}

		[self NonnonTxtboxInsert:n_mac_str2nsstring( str )];

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_NUMBER_6:
	{

		[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

		// [x] : function keys used by system globally

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( txtbox->txt_data->readonly ) { break; }

			n_posix_char str[ 100 ];

			n_txtbox_day_of_week( str, 100 );

			n_txt_add( txtbox->txt_data, txtbox->focus, str );

			[self NonnonTxtboxEditedNotify:TRUE];

			txtbox->caret_fr = txtbox->caret_to = n_txtbox_caret_detect_cch2pixel
			(
				txtbox->txt_data,
				txtbox->focus,
				txtbox->font,
				txtbox->font_size,
				n_posix_strlen( n_txt_get( txtbox->txt_data, txtbox->focus ) )
			);

			[self NonnonTxtboxRedraw];
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	default :
//NSLog( @"pressed" );

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			//
		} else {
//NSLog( @"%s", n_txt_get( txtbox->txt_data, txtbox->focus ) );

			// [!] : IME ON : see insertText@n_txtbox.c
#ifndef N_TXTBOX_IME_ENABLE
			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
#endif
		}
	
	break;

	} // switch

}


@end

