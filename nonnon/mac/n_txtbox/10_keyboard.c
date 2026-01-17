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
		( caret_fr.cch.x == 0 )&&( caret_fr.cch.y == 0 )
		&&
		( caret_to.cch.x == 0 )&&( caret_to.cch.y == 0 )

	)
	{
		ret = TRUE;
	}

	return ret;
}

- (void) NonnonTxtboxKeyboardMoveDetect:(void*) zero ud:(n_type_int)ud lr:(n_type_int)lr oneline:(BOOL)oneline_override
{
//NSLog( @"NonnonTxtboxKeyboardMoveDetect %f", n_focus );


	// [!] : unselect
/*
	if ( ( caret_fr.pxl.x != caret_to.pxl.x )&&( caret_fr.cch.y != caret_to.cch.y ) )
	{
//NSLog( @"!" );
		caret_fr = caret_to;
	}
*/

//NSLog( @"%d", shift_selection_is_tail );


	n_focus += ud;
	if ( n_focus < 0 )
	{
		if ( n_focus <= -1 ) { caret_to.cch.x = 0; }
		n_focus = 0;
	}
	if ( n_focus >= n_txt_data->sy ) { n_focus = n_txt_data->sy - 1; }


	if ( ud )
	{
		NSRect r = NSMakeRect( padding, offset_y, [self frame].size.width - ( offset_x * 2 ), font_size.height );

		caret_to = n_txtbox_caret_detect_pixel2caret
		(
			n_txt_data,
			n_focus,
			r,
			font,
			font_size,
			NSMakePoint( padding + caret_to.pxl.x, caret_to.cch.y + font_size.height )
		);
//NSLog( @"%0.2f", caret_to.pxl.x );

		// [!] : Win32 compatible behavior 
		if ( caret_to.pxl.x == 0 ) { caret_to.cch.x = 0; }
	}


	if ( lr != 0 )
	{

		BOOL oneline = oneline_override;
		if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( self.n_listbox_edit_onoff ) { oneline = TRUE; } else { return; }
		}

		n_posix_char *s = n_txt_get( n_txt_data, n_focus );
		if ( lr < 0 )
		{
			if ( caret_to.cch.x == 0 )
			{
				if ( oneline )
				{
					//
				} else
				if ( n_focus != 0 )
				{
					n_focus--;
					caret_to.cch.x = caret_to.cch.x = n_posix_strlen( n_txt_get( n_txt_data, n_focus ) );
				}
			} else {
				caret_to.cch.x = caret_to.cch.x = n_mac_txtbox_caret_move( s, caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_L );
			}
		} else {
			if ( caret_to.cch.x == n_posix_strlen( n_txt_get( n_txt_data, n_focus ) ) )
			{
				n_type_int max_sy = n_txt_data->sy - 1;
				if ( oneline )
				{
					//
				} else
				if ( n_focus < max_sy )
				{
					n_focus++;
					caret_to.cch.x = 0;
				}
			} else {
				caret_to.cch.x = n_mac_txtbox_caret_move( s, caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_R );	
			}
		}
	}


	caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
		caret_to.cch.x
	);


	return;
}

- (n_caret) NonnonTxtboxKeyboardShiftSelectionDetect:(void*)zero lr:(n_type_int)lr cch:(n_type_int)cch
{
//NSLog( @"NonnonTxtboxKeyboardShiftSelectionDetect %f", n_focus );

	n_posix_char *s = n_txt_get( n_txt_data, n_focus );
	if ( lr < 0 )
	{
		cch = n_mac_txtbox_caret_move( s, cch, YES, N_MAC_TXTBOX_CARET_MOVE_L );
	} else {
		cch = n_mac_txtbox_caret_move( s, cch, YES, N_MAC_TXTBOX_CARET_MOVE_R );
	}
//NSLog( @"%lld", (*cch) );

	n_caret caret = n_txtbox_caret_detect_cch2pixel
	(
		n_txt_data,
		n_focus,
		font,
		font_size,
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

	is_key_input = FALSE;

}

- (void) keyDown : (NSEvent*) event
{
//NSLog( @"Key Code = %d : Chars %@", event.keyCode, event.characters );


	is_key_input = TRUE;


	caret_blink_force_onoff = TRUE;


#ifdef N_TXTBOX_IME_ENABLE

//NSLog( @"IME On/Off : %@ : %d", ime_nsstr, ime_onoff );

	NSString *ime_nsstr_prv = [ime_nsstr copy];

	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		if ( self.n_listbox_edit_onoff )
		{
			ime = [NSTextInputContext currentInputContext];
			[ime handleEvent:event];
		}
	} else {
		ime = [NSTextInputContext currentInputContext];
		[ime handleEvent:event];
	}

	if ( ime_onoff )
	{
		if ( event.keyCode == N_MAC_KEYCODE_BACKSPACE )
		{
//NSLog( @"%@ %d : %@", ime_nsstr, (int) [ime_nsstr length], ime_nsstr_prv );

			NSUInteger cch = [ime_nsstr length];

			if ( ( cch == 1 )&&( [ime_nsstr compare:ime_nsstr_prv] == 0 ) )
			{
				ime_freed = TRUE;
			}

			if ( ime_freed )
			{
				[ime discardMarkedText];
				ime_nsstr = @"";
				ime_onoff = FALSE;
				ime_freed = FALSE;
				cch       = 0;
			}

			if ( cch == 1 )
			{
				ime_freed = TRUE;
			}

			[self NonnonTxtboxRedraw];

		} else
		if ( ( event.keyCode == N_MAC_KEYCODE_RETURN )||( event.keyCode == N_MAC_KEYCODE_ENTER ) )
		{
//NSLog( @"return 1" );
			if ( ime_is_first == FALSE )
			{
				[self NonnonTxtboxInsert:ime_nsstr];
			}

			[ime discardMarkedText];
			ime_nsstr = @"";
			ime_delay = FALSE;
			ime_onoff = FALSE;

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
	if ( ime_delay )
	{
		if ( ( event.keyCode == N_MAC_KEYCODE_RETURN )||( event.keyCode == N_MAC_KEYCODE_ENTER ) )
		{
//NSLog( @"return 2" );

			[ime discardMarkedText];
			ime_nsstr = @"";
			ime_delay = FALSE;
			ime_onoff = FALSE;

			caret_blink_force_onoff = FALSE;
			n_bmp_fade_init( &caret_blink_fade, n_bmp_black );

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

		if ( event.keyCode == N_MAC_KEYCODE_SPACE )
		{
			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];
		}

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_PAGE_UP :
	{

		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		}

		CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
		CGFloat items_per_canvas = csy / font_size.height;

		scroll -= items_per_canvas;

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_PAGE_DOWN :
	{

		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		}

		CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
		CGFloat items_per_canvas = csy / font_size.height;

		scroll += items_per_canvas;

		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_ARROW_UP :
	{
//NSLog( @"Up" );

		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( n_txt_data->readonly ) { break; }

			if ( self.n_listbox_edit_onoff )
			{
				//
			} else
			if ( n_focus >= 1 )
			{

				if ( event.modifierFlags & NSEventModifierFlagShift )
				{
					if ( delegate_option & N_MAC_TXTBOX_DELEGATE_SWAP )
					{
						[self.delegate NonnonTxtbox_delegate_swap:self is_up:YES];
					}
				} else {
					n_focus--;
					self.n_listbox_edit_onoff = FALSE;
				}

				[self NonnonTxtboxCaretOutOfCanvasUpDownListbox];
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

			n_caret caret = caret_fr;

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:-1 lr:0 oneline:FALSE];

			caret_fr = caret;

		} else {

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:-1 lr:0 oneline:FALSE];

		}

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_DOWN:
	{
//NSLog( @"Down" );

		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
		{
			break;
		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( n_txt_data->readonly ) { break; }

			if ( self.n_listbox_edit_onoff )
			{
				//
			} else
			if ( n_focus <= ( n_txt_data->sy - 1 - 1 ) )
			{
			
				if ( event.modifierFlags & NSEventModifierFlagShift )
				{
					if ( delegate_option & N_MAC_TXTBOX_DELEGATE_SWAP )
					{
						[self.delegate NonnonTxtbox_delegate_swap:self is_up:NO];
					}
				} else {
					n_focus++;
					self.n_listbox_edit_onoff = FALSE;
				}

				[self NonnonTxtboxCaretOutOfCanvasUpDownListbox];
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

			n_caret caret = caret_fr;

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:1 lr:0 oneline:FALSE];

			caret_fr = caret;

		} else {

			[self NonnonTxtboxKeyboardMoveDetect:nil ud:1 lr:0 oneline:FALSE];

		}

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_LEFT:
	{
//NSLog( @"Left" );

		if ( event.modifierFlags & NSEventModifierFlagShift )
		{
			n_caret caret = caret_fr;
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:-1 oneline:TRUE];
			caret_fr = caret;

			shift_selection_is_tail = FALSE;
		} else
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			[self NonnonTxtboxDoubleclickDetect:NO tab_patch:NO];
			caret_to = caret_fr;

			n_posix_char *line = n_txt_get( n_txt_data, n_focus );
			caret_to.cch.x = caret_fr.cch.x = n_mac_txtbox_caret_move( line, caret_to.cch.x, YES, N_MAC_TXTBOX_CARET_MOVE_L );
		} else {
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:-1 oneline:FALSE];
		}

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_ARROW_RIGHT:
	{
//NSLog( @"Right" );
//break;

		if ( event.modifierFlags & NSEventModifierFlagShift )
		{
			n_caret caret = caret_fr;
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:1 oneline:TRUE];
			caret_fr = caret;

			shift_selection_is_tail = TRUE;
		} else
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			[self NonnonTxtboxDoubleclickDetect:NO tab_patch:NO];
			caret_fr = caret_to;
		} else {
			[self NonnonTxtboxKeyboardMoveDetect:nil ud:0 lr:1 oneline:FALSE];
		}

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];
	}
	break;

	case N_MAC_KEYCODE_RETURN:
	case N_MAC_KEYCODE_ENTER:
	{

		if ( n_txt_data->readonly ) { break; }


		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{

			self.n_is_enter_pressed = TRUE;

		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
		{

			self.n_is_enter_pressed = TRUE;

		} else
		if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( n_txt_data->readonly ) { break; }

			if ( self.n_listbox_no_edit ) { break; }

			//[delegate NonnonTxtbox_delegate_listbox_rename:n_focus];

			if ( self.n_listbox_edit_onoff )
			{
				self.n_listbox_edit_onoff = FALSE;
				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
				{
					[delegate NonnonTxtbox_delegate_listbox_edited:n_focus];
				}
			} else {
				self.n_listbox_edit_onoff = TRUE;

				listbox_edit_index = n_focus;

				caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
				(
					n_txt_data,
					n_focus,
					font,
					font_size,
					n_posix_strlen( n_txt_get( n_txt_data, n_focus ) )
				);
			}

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxRedraw];

			break;

		} else
		if ( n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to ) )
		{

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				caret_fr.cch.x
			);

//n_caret_debug_cch( caret_fr ,caret_to );

		} else {
//NSLog( @"divide" );

			n_posix_char *line = n_txt_get( n_txt_data, n_focus );
			n_posix_char *s    = n_string_carboncopy( line );

			n_txt_add( n_txt_data, n_focus + 1, &s[ caret_fr.cch.x ] );
			line[ caret_fr.cch.x ] = N_STRING_CHAR_NUL;

			n_string_free( s );


			n_focus++;
			caret_fr = caret_to = NonnonMakeCaret( 0, n_focus * font_size.height, 0, n_focus );

		}

		[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

		n_edited = TRUE;
		if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
		{
			[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
		}

		self.n_is_enter_pressed = FALSE;

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_BACKSPACE:
	{

		if ( n_txt_data->readonly ) { break; }


		if ( [self NonnonTxtboxKeyboardIsFullStop] )
		{
			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			break;
		}


		[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

		if ( event.modifierFlags & NSEventModifierFlagOption )
		{

			n_type_int count = llabs( caret_fr.cch.y - caret_to.cch.y ) + 1;
			n_type_int min_y =   MIN( caret_fr.cch.y,  caret_to.cch.y );

			n_type_int y = 0;
			n_posix_loop
			{
				if ( y >= count ) { break; }

				n_posix_char *line = n_txt_get( n_txt_data, min_y + y );

				if ( line[ 0 ] == N_STRING_CHAR_TAB )
				{
					n_posix_char *str = n_string_new( n_posix_strlen( line ) );
					n_posix_sprintf_literal( str, "%s", &line[ 1 ] );

					n_txt_mod_fast( n_txt_data, min_y + y, str );
				}

				y++;
			}

			caret_fr.cch.x--;
			caret_to.cch.x--;

			n_type_int cch;
			if ( shift_selection_is_tail )
			{
				cch = caret_to.cch.x;
			} else {
				cch = caret_fr.cch.x;
			}

			n_type_int fr_cch_y = caret_fr.cch.y;
			n_type_int to_cch_y = caret_to.cch.y;

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch
			);

			caret_fr.cch.y = fr_cch_y;
			caret_to.cch.y = to_cch_y;

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}

			[self NonnonTxtboxRedraw];

			break;
		}

//n_caret_debug_cch( caret_fr ,caret_to );// break;

		BOOL grow = ( caret_fr.cch.y < caret_to.cch.y );

		if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
		{
			if ( self.n_listbox_edit_onoff )
			{
				n_posix_char *line = n_txt_get( n_txt_data, n_focus );
				n_type_int     cch = n_mac_txtbox_caret_move( line, caret_fr.cch.x, NO, N_MAC_TXTBOX_CARET_MOVE_L );

				n_posix_sprintf_literal( &line[ cch ], "%s", &line[ caret_fr.cch.x ] );

				if ( grow ) { n_focus = caret_fr.cch.y; }

				caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
				(
					n_txt_data,
					n_focus,
					font,
					font_size,
					cch
				);

				[self NonnonTxtboxCaretOutOfCanvasUpDown];
				[self NonnonTxtboxRedraw];
			}

			break;
		}

		if ( n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to ) )
		{
//NSLog( @"Backspace : deleted : %0.2f %lld %lld", n_focus, caret_fr.cch.y, caret_to.cch.y );

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				caret_fr.cch.x
			);

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxRedraw];

			break;
		}

		if (
			( self.n_mode == N_MAC_TXTBOX_MODE_EDITBOX )
			&&
			( caret_fr.cch.x == 0 )
		)
		{
//NSLog( @"Backspace : zero" );

			if ( n_focus == 0 )
			{
				// [!] : for select all
				caret_fr = caret_to = NonnonMakeCaret( 0,0,0,0 );
				[self NonnonTxtboxRedraw];
				break;
			}

			n_posix_char *line_1 = n_txt_get( n_txt_data, n_focus - 1 );
			n_posix_char *line_2 = n_txt_get( n_txt_data, n_focus     );
			n_type_int     cch_1 = n_posix_strlen( line_1 );
			n_type_int     cch_2 = n_posix_strlen( line_2 );

			n_posix_char  *s     = n_string_new( cch_1 + cch_2 );
			n_posix_sprintf_literal( s, "%s%s", line_1, line_2 );

			n_txt_del( n_txt_data, n_focus );
			n_txt_mod( n_txt_data, n_focus - 1, s );

			n_focus--;

			caret_fr.cch.x = caret_to.cch.x = cch_1;

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				caret_fr.cch.x
			);

			n_string_free( s );

		} else {
//NSLog( @"Backspace : normal" );

			n_posix_char *line = n_txt_get( n_txt_data, n_focus );
			n_type_int     cch = n_mac_txtbox_caret_move( line, caret_fr.cch.x, NO, N_MAC_TXTBOX_CARET_MOVE_L );

			n_posix_sprintf_literal( &line[ cch ], "%s", &line[ caret_fr.cch.x ] );

			if ( grow ) { n_focus = caret_fr.cch.y; }

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch
			);

		}

		n_edited = TRUE;
		if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
		{
			[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
		}

		[self NonnonTxtboxCaretOutOfCanvasUpDown];
		[self NonnonTxtboxRedraw];

	}
	break;

	case N_MAC_KEYCODE_TAB:
	{

		// [x] : you cannot use Ctrl + Tab : system uses

		//if ( event.modifierFlags & NSEventModifierFlagCommand )
		if ( event.modifierFlags & NSEventModifierFlagOption )
		{
			if ( n_txt_data->readonly ) { break; }

			n_type_int count = llabs( caret_fr.cch.y - caret_to.cch.y ) + 1;
			n_type_int min_y =   MIN( caret_fr.cch.y,  caret_to.cch.y );

			n_type_int y = 0;
			n_posix_loop
			{
				if ( y >= count ) { break; }

				n_posix_char *line = n_txt_get( n_txt_data, min_y + y );

				if ( FALSE == n_string_is_empty( line ) )
				{
					n_posix_char *str = n_string_new( n_posix_strlen( line ) + 1 );
					n_posix_sprintf_literal( str, "%s%s", N_STRING_TAB, line );

					n_txt_mod_fast( n_txt_data, min_y + y, str );
				}

				y++;
			}

			caret_fr.cch.x++;
			caret_to.cch.x++;

			n_type_int cch;
			if ( shift_selection_is_tail )
			{
				cch = caret_to.cch.x;
			} else {
				cch = caret_fr.cch.x;
			}

			n_type_int fr_cch_y = caret_fr.cch.y;
			n_type_int to_cch_y = caret_to.cch.y;

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				cch
			);

			caret_fr.cch.y = fr_cch_y;
			caret_to.cch.y = to_cch_y;

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxRedraw];
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}
	
	}
	break;

	case N_MAC_KEYCODE_CUT:
	{

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_txt_data->readonly ) { break; }

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}


			NSString *s = n_mac_txtbox_copy( n_txt_data, caret_fr.cch.x, caret_fr.cch.y, caret_to.cch.x, caret_to.cch.y );

			NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
			[pasteboard clearContents];
			[pasteboard writeObjects:@[ s ]];

			n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to );

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				caret_fr.cch.x
			);

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxRedraw];

		} else {

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];

		}
	}
	break;

	case N_MAC_KEYCODE_COPY: 
	{
		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			NSString *s = n_mac_txtbox_copy( n_txt_data, caret_fr.cch.x, caret_fr.cch.y, caret_to.cch.x, caret_to.cch.y );
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
			if ( n_txt_data->readonly ) { break; }

			[self NonnonTxtboxUndo:N_TXTBOX_UNDO_REGISTER];

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}

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
				( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
				||
				( self.n_mode == N_MAC_TXTBOX_MODE_ONELINE )
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


			n_mac_txtbox_del( n_txt_data, &n_focus, &caret_fr, &caret_to );

			if ( t.sy == 1 )
			{

				n_posix_char *line_f = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );
				n_posix_char *line_m = n_string_carboncopy( n_txt_get(         &t,       0 ) );
				n_posix_char *line_t = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );
				
				line_f[ caret_fr.cch.x ] = N_STRING_CHAR_NUL;
				sprintf( line_t, "%s", &line_t[ caret_fr.cch.x ] );
//NSLog( @"%s", line_f );
//NSLog( @"%s", line_t );

				n_type_int    cch = n_posix_strlen( line_f ) + n_posix_strlen( line_m ) + n_posix_strlen( line_t );
				n_posix_char *str = n_string_new( cch );
				n_posix_sprintf_literal( str, "%s%s%s", line_f, line_m, line_t );

				n_txt_mod( n_txt_data, n_focus, str );

				n_string_free( str );


				caret_fr.cch.x = caret_to.cch.x = caret_fr.cch.x + n_posix_strlen( line_m );

				caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
				(
					n_txt_data,
					n_focus,
					font,
					font_size,
					caret_fr.cch.x
				);


				n_string_free( line_f );
				n_string_free( line_m );
				n_string_free( line_t );

			} else {
//break;
				if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX ) { break; }


				n_posix_char *line_f = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );
				n_posix_char *line_t = n_string_carboncopy( n_txt_get( n_txt_data, n_focus ) );

				line_f[ caret_fr.cch.x ] = N_STRING_CHAR_NUL;
				sprintf( line_t, "%s", &line_t[ caret_fr.cch.x ] );
//NSLog( @"F:%s", line_f );
//NSLog( @"T:%s", line_t );

				{
					n_type_int    cch = n_posix_strlen( line_f ) + n_posix_strlen( n_txt_get( &t, 0 ) );
					n_posix_char *str = n_string_new( cch );

					n_posix_sprintf_literal( str, "%s%s", line_f, n_txt_get( &t, 0 ) );
					n_txt_mod_fast( n_txt_data, n_focus, str );
				}

				n_type_int i = 1;
				n_posix_loop
				{//break;
					if ( i >= t.sy ) { break; }

					n_txt_add( n_txt_data, n_focus + i, n_txt_get( &t, i ) );

					i++;
				}

				{
					i--;

					n_type_int    cch = n_posix_strlen( n_txt_get( &t, i ) ) + n_posix_strlen( line_t );
					n_posix_char *str = n_string_new( cch );

					n_posix_sprintf_literal( str, "%s%s", n_txt_get( &t, i ), line_t );
					n_txt_mod_fast( n_txt_data, n_focus + i, str );
				}


				n_type_int p_focus = caret_fr.cch.y;

				n_focus = n_focus + i;

				caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
				(
					n_txt_data,
					n_focus,
					font,
					font_size,
					n_posix_strlen( n_txt_get( &t, i ) )
				);


				CGFloat csy              = self.frame.size.height - ( offset_y * 2 );
				CGFloat items_per_canvas = csy / font_size.height;
				CGFloat edge             = trunc( scroll + items_per_canvas );
//NSLog( @"Focus %lld : Edge %0.2f : Scroll %0.2f", n_focus, edge, scroll );

				if ( p_focus < scroll )
				{
					scroll = p_focus;
					[self NonnonTxtboxDrawScrollClamp];
					edge = trunc( scroll + items_per_canvas );
				}

				if ( n_focus >= edge )
				{
					scroll = scroll + ( n_focus - edge );
					scroll = ceil( scroll ) + 1;
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

			if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX ) { break; }

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
			if ( n_txt_data->readonly ) { break; }

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
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
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
			//if ( n_txt_save_utf8( n_txt_data, n_mac_nsstring2str( n_path ) ) )
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

		if ( n_txt_data->readonly ) { break; }

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

		// [x] : function keys used by system globally

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			if ( n_txt_data->readonly ) { break; }

			n_posix_char str[ 100 ];

			n_txtbox_day_of_week( str, 100 );

			n_txt_add( n_txt_data, n_focus, str );

			n_edited = TRUE;
			if ( delegate_option & N_MAC_TXTBOX_DELEGATE_EDITED )
			{
				[self.delegate NonnonTxtbox_delegate_edited:self onoff:n_edited];
			}

			caret_fr = caret_to = n_txtbox_caret_detect_cch2pixel
			(
				n_txt_data,
				n_focus,
				font,
				font_size,
				n_posix_strlen( n_txt_get( n_txt_data, n_focus ) )
			);

			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxRedraw];
		} else {
			[self NonnonTxtboxCaretOutOfCanvasUpDown];
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}

	}
	break;

	default :

		if ( event.modifierFlags & NSEventModifierFlagCommand )
		{
			//
		} else {
			[self NonnonTxtboxKeyboardInputMethod:[event characters]];
		}
	
	break;

	} // switch

}


@end

