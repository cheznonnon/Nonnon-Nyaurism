// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




@implementation NonnonTxtbox (NonnonTxtboxMouse)




- (void) NonnonTxtboxClickEvent:(NSEvent*)theEvent detailed:(BOOL)onoff
{

	static NSInteger prev = 0;


	[self becomeFirstResponder];
	//if ( n_txtbox_focus != self ) { return; }


	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		if ( txtbox->txt_data->readonly ) { return; }

		NSPoint local_point = n_mac_cursor_position_get( self );

		txtbox->focus = [self NonnonTxtboxFocusCalculate:local_point];
		if ( txtbox->focus >= txtbox->txt_data->sy )
		{
			if ( txtbox->listbox_no_selection_onoff )
			{
				[self NonnonTxtboxCaretReset];
				[self NonnonTxtboxRedraw];
				return;
			} else {
				txtbox->focus = txtbox->txt_data->sy - 1;
			}
		}
//NSLog( @"%f", txtbox->focus );

		txtbox->scrollbar_thumb_is_hovered = FALSE;

		if ( ( self.txtbox->listbox_edit_onoff )&&( txtbox->listbox_edit_index == txtbox->focus ) )
		{
			if ( ( [theEvent clickCount] == 3 )&&( prev == [theEvent buttonNumber] ) )
			{
				[self NonnonTxtboxTripleclickDetect];
			} else
			if ( ( [theEvent clickCount] == 2 )&&( prev == [theEvent buttonNumber] ) )
			{
				[self NonnonTxtboxDoubleclickDetect:onoff tab_patch:YES];
			} else
			if ( [theEvent clickCount] == 1 )
			{
				prev = [theEvent buttonNumber];
				txtbox->caret_fr = txtbox->caret_to = [self NonnonTxtboxMouseCursorDetect];
			}

			[self NonnonTxtboxRedraw];
		} else {
			self.txtbox->listbox_edit_onoff = FALSE;

			txtbox->caret_fr = txtbox->caret_to = [self NonnonTxtboxMouseCursorDetect];

			[self NonnonTxtboxCaretOutOfCanvasUpDown];

			[self NonnonTxtboxRedraw];
		}

		if (
			( delegate_option & N_MAC_TXTBOX_DELEGATE_MOUSEDOWN_LEFT )
			&&
			( [theEvent buttonNumber] == 0 )
		)
		{
			[delegate mouseDown:theEvent];
		}

	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
//NSLog( @"!" );

		if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->find_icon_rect ) )
		{
			txtbox->find_icon_is_pressed = TRUE;
		} else
		if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->delete_circle_rect ) )
		{
			txtbox->delete_circle_is_pressed = TRUE;
		} else {
			if ( ( [theEvent clickCount] == 3 )&&( prev == [theEvent buttonNumber] ) )
			{
				[self NonnonTxtboxTripleclickDetect];
			} else
			if ( ( [theEvent clickCount] == 2 )&&( prev == [theEvent buttonNumber] ) )
			{
				[self NonnonTxtboxDoubleclickDetect:onoff tab_patch:YES];
			} else
			if ( [theEvent clickCount] == 1 )
			{
				prev = [theEvent buttonNumber];
				txtbox->caret_fr = txtbox->caret_to = [self NonnonTxtboxMouseCursorDetect];
			}
		}

		txtbox->scrollbar_thumb_is_hovered = FALSE;

		txtbox->caret_blink_force_onoff = TRUE;

	} else {

		if ( ( [theEvent clickCount] == 3 )&&( prev == [theEvent buttonNumber] ) )
		{
			[self NonnonTxtboxTripleclickDetect];
		} else
		if ( ( [theEvent clickCount] == 2 )&&( prev == [theEvent buttonNumber] ) )
		{
			[self NonnonTxtboxDoubleclickDetect:onoff tab_patch:YES];
		} else
		if ( [theEvent clickCount] == 1 )
		{
			prev = [theEvent buttonNumber];
			txtbox->caret_fr = txtbox->caret_to = [self NonnonTxtboxMouseCursorDetect];
		}

		txtbox->scrollbar_thumb_is_hovered = FALSE;

		txtbox->caret_blink_force_onoff = TRUE;


		[self NonnonTxtboxCaretOutOfCanvasUpDown];

		[self NonnonTxtboxRedraw];

	}
}

- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
	return txtbox->direct_click_onoff;
}

- (void) updateTrackingAreas
{
//return;

	// [!] : NSTrackingActiveWhenFirstResponder
	//
	//	this can fix "Detected potentially harmful notification post rate"

	int options =  NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveWhenFirstResponder | NSTrackingActiveAlways;
	NSTrackingArea *trackingArea = [
		[NSTrackingArea alloc]
			initWithRect:[self bounds]
			     options:options
			       owner:self
			    userInfo:nil
	];
	
	[self addTrackingArea:trackingArea];

}

- (void) mouseMoved:(NSEvent *)theEvent
{
//NSLog( @"mouseMoved" );

	// [Needed] : NSTrackingMouseMoved 

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->delete_circle_rect ) )
		{
			if ( txtbox->delete_circle_is_hovered == FALSE )
			{
//NSLog( @"In" );
				txtbox->delete_circle_is_hovered = TRUE;
				n_bmp_fade_always_on( &txtbox->delete_circle_fade_hovered, n_bmp_black, n_bmp_white );
				[self NonnonTxtboxRedraw];
			}
		} else {
			if ( txtbox->delete_circle_is_hovered != FALSE )
			{
//NSLog( @"Out" );
				txtbox->delete_circle_is_hovered = FALSE;
				n_bmp_fade_always_on( &txtbox->delete_circle_fade_hovered, n_bmp_white, n_bmp_black );
				[self NonnonTxtboxRedraw];
			}
		}

	}

}

- (void) mouseExited:(NSEvent *)theEvent
{
//NSLog( @"mouseExited" );

	// [Needed] : NSTrackingMouseEnteredAndExited

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( txtbox->delete_circle_is_hovered != FALSE )
		{
//NSLog( @"Out" );
			txtbox->delete_circle_is_hovered = FALSE;
			n_bmp_fade_always_on( &txtbox->delete_circle_fade_hovered, n_bmp_white, n_bmp_black );
			[self NonnonTxtboxRedraw];
		}
	}

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp" );

	BOOL is_dragged = txtbox->drag_timer_queue;

	txtbox->drag_timer_queue = FALSE;


	if ( txtbox->scrollbar_thumb_is_captured )
	{
		txtbox->scrollbar_thumb_is_captured = FALSE;

		txtbox->scrollbar_fade_captured_onoff = TRUE;
		n_bmp_fade_always_on( &txtbox->scrollbar_fade, n_bmp_black, n_bmp_white );
/*
		// [!] : snapping feature : not working accurately
		if ( txtbox->scr.pixel_thumb > 32 )
		{
			[self NonnonTxtboxScrollMetrics];
			txtbox->scr.pixel_pos = trunc( txtbox->scr.pixel_pos / txtbox->scr.pixel_step ) * txtbox->scr.pixel_step;
			txtbox->scr. unit_pos = ( txtbox->scr.pixel_pos  / txtbox->scr.pixel_max ) * txtbox->scr.unit_max;
		}
*/
		[self NonnonTxtboxRedraw];
	}


	if ( txtbox->scrollbar_thumb_is_hovered ) { return; }
	if ( txtbox->scrollbar_shaft_is_hovered ) { return; }

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		//

	} else {

		if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			if ( txtbox->find_icon_is_pressed )
			{
				if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->find_icon_rect ) )
				{
					if ( delegate_option & N_MAC_TXTBOX_DELEGATE_F3 )
					{
						BOOL is_left = YES;
						if ( theEvent.modifierFlags & NSEventModifierFlagShift )
						{
							is_left = NO;
						}
						[self.delegate NonnonTxtbox_delegate_F3:self is_left:is_left];
					}
				}

				[self NonnonTxtboxRedraw];

				txtbox->find_icon_is_pressed = FALSE;
			} else
			if ( txtbox->delete_circle_is_pressed )
			{
				txtbox->delete_circle_is_pressed = FALSE;

				if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->delete_circle_rect ) )
				{
					txtbox->delete_circle_fade_pressed_phase = 1;
				}
			}
		}

	}

	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_MOUSEUP_LEFT )
	{
		if ( is_dragged )
		{
			//
		} else {
			[delegate mouseUp:theEvent];
		}
	}

}

- (void) mouseDown:(NSEvent*) theEvent
{
//NSLog(@"mouseDown");

	txtbox->pt_cur                     = [NSEvent mouseLocation];
	txtbox->scrollbar_thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, txtbox->scrollbar_thumb_rect );
	txtbox->scrollbar_shaft_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, txtbox->scrollbar_shaft_rect );
//if ( thumb_is_hovered ) { NSLog( @"!" ); }

	txtbox->scrollbar_thumb_offset = txtbox->scrollbar_thumb_rect.origin.y - txtbox->pt_cur.y;
//NSLog( @"%f : %f %f", thumb_offset, scroller_rect_thumb.origin.y, pt_cur.y );

	if ( txtbox->scrollbar_thumb_is_hovered )
	{

		if ( txtbox->scrollbar_thumb_is_captured == FALSE )
		{
			txtbox->scrollbar_thumb_is_captured = TRUE;

			txtbox->scrollbar_fade_captured_onoff = TRUE;
			n_bmp_fade_always_on( &txtbox->scrollbar_fade, n_bmp_black, n_bmp_white );
		}

	} else
	if ( txtbox->scrollbar_shaft_is_hovered )
	{

		if ( txtbox->pt_cur.y < txtbox->scrollbar_thumb_rect.origin.y )
		{
//NSLog( @"upper" );
			txtbox->scr.unit_pos -= txtbox->scr.unit_page;
		} else {
//NSLog( @"lower" );
			txtbox->scr.unit_pos += txtbox->scr.unit_page;
		}

		[self NonnonTxtboxRedraw];

	} else {

		BOOL detailed = theEvent.modifierFlags & NSEventModifierFlagShift;
		[self NonnonTxtboxClickEvent:theEvent detailed:detailed];

	}

}

- (void) mouseDragged:(NSEvent*) theEvent
{
//NSLog( @"mouseDragged" );
//return;

	txtbox->pt_prv = txtbox->pt_cur;
	txtbox->pt_cur = [NSEvent mouseLocation];

	if ( txtbox->scrollbar_thumb_is_captured )
	{
//NSLog( @"scrollbar_thumb_is_captured" );

		CGFloat dy = txtbox->pt_prv.y - txtbox->pt_cur.y;
//NSLog( @"%f %f", dy, [theEvent deltaY] );

//scroll -= dy / font_size.height;

		txtbox->scr.unit_pos += dy / txtbox->scr.pixel_step;
//NSLog( @"%f", txtbox->scr.unit_pos );

		[self NonnonTxtboxRedraw];

	} else
	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		if ( txtbox->scrollbar_thumb_is_hovered ) { return; }
		if ( txtbox->scrollbar_shaft_is_hovered ) { return; }

		[self NonnonTxtboxRedraw];

	} else {

		if ( txtbox->scrollbar_thumb_is_hovered ) { return; }
		if ( txtbox->scrollbar_shaft_is_hovered ) { return; }

		if ( txtbox->find_icon_is_pressed ) { return; }


//NSLog( @"%ld", [theEvent clickCount] ); return;
		if ( [theEvent clickCount] != 1 ) { return; }


		txtbox->caret_to = [self NonnonTxtboxMouseCursorDetect];
//n_caret_debug_cch( caret_fr, caret_to );

		if ( txtbox->caret_fr.cch.x < txtbox->caret_to.cch.x )
		{
			txtbox->shift_selection_is_tail = TRUE;
		} else {
			txtbox->shift_selection_is_tail = FALSE;
		}


		txtbox->drag_timer_queue = TRUE;


		// [x] : partially redraw : impossible

		[self NonnonTxtboxRedraw];

	}

}




- (void)scrollWheel:(NSEvent *)theEvent
{
//NSLog( @"%lu : %0.2f %0.2f", [theEvent phase], [theEvent deltaY], [theEvent scrollingDeltaY] );
//return;

	//if ( thumb_is_hovered ) { return; }
	//if ( shaft_is_hovered ) { return; }


	txtbox->smooth_wheel_delta = [theEvent deltaY];
	if ( txtbox->smooth_wheel_delta == 0 ) { return; }


	if ( [theEvent scrollingDeltaY] == 0.0 ) { txtbox->smooth_wheel_is_mos = TRUE; }


	BOOL is_trackpad;
	if (
		( [theEvent momentumPhase] != NSEventPhaseNone )
		||
		( [theEvent         phase] != NSEventPhaseNone )
	)
	{
//NSLog( @"Trackpad" );
		is_trackpad = TRUE;
	} else {
//NSLog( @"Mouse" );
		is_trackpad = FALSE;
	}
//NSLog( @"%lu %d", [theEvent momentumPhase], [theEvent phase] );


	if ( is_trackpad )
	{

		txtbox->smooth_wheel_delta = [theEvent scrollingDeltaY];

		CGFloat step;

		step = fabs( txtbox->smooth_wheel_delta );
		step = MIN( step, 2.0 );

		if ( txtbox->smooth_wheel_delta < 0 )
		{
			txtbox->scr.unit_pos += step;
		} else {
			txtbox->scr.unit_pos -= step;
		}

		[self NonnonTxtboxDrawScrollClamp];

		[self NonnonTxtboxRedraw];

	} else
	if ( txtbox->smooth_wheel_is_mos )
	{
//NSLog( @"Mos" );

		CGFloat step;

		step = fabs( txtbox->smooth_wheel_delta );
		step = MIN( step, 2.0 );

		if ( txtbox->smooth_wheel_delta < 0 )
		{
			txtbox->scr.unit_pos += step;
		} else {
			txtbox->scr.unit_pos -= step;
		}

		[self NonnonTxtboxDrawScrollClamp];

		[self NonnonTxtboxRedraw];

	} else {

		txtbox->smooth_wheel_delta = [theEvent scrollingDeltaY];

		if ( txtbox->smooth_wheel_onoff == FALSE )
		{
			txtbox->smooth_wheel_onoff = TRUE;
			txtbox->smooth_wheel_mutex = TRUE;
		}

		[self NonnonTxtboxDrawScrollClamp];

		[self NonnonTxtboxRedraw];

	}

}




- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog( @"rightMouseUp" );

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( txtbox->find_icon_is_pressed )
		{
			if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->find_icon_rect ) )
			{
				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_F3 )
				{
					[self.delegate NonnonTxtbox_delegate_F3:self is_left:NO];
				}
			}

			[self NonnonTxtboxRedraw];

			txtbox->find_icon_is_pressed = FALSE;
		} else
		if ( txtbox->delete_circle_is_pressed )
		{
			txtbox->delete_circle_is_pressed = FALSE;

			if ( n_mac_window_is_hovered_offset_by_rect( self, txtbox->delete_circle_rect ) )
			{
				txtbox->delete_circle_fade_pressed_phase = 1;
			}
		}
	}

	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_MOUSEUP_RIGHT )
	{
		[delegate rightMouseUp:theEvent];
	}

}

- (void) rightMouseDown:(NSEvent*) theEvent
{
//NSLog( @"rightMouseDown : %ld", [theEvent clickCount] );

	txtbox->pt_cur = [NSEvent mouseLocation];

	if ( self.txtbox->mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{
		//
	} else {
		[self NonnonTxtboxClickEvent:theEvent detailed:YES];
	}

	if ( delegate_option & N_MAC_TXTBOX_DELEGATE_MOUSEDOWN_RIGHT )
	{
		[delegate rightMouseDown:theEvent];
	}

}




- (void) otherMouseUp:(NSEvent*) theEvent
{
//NSLog( @"otherMouseUp : %ld", (long) [theEvent buttonNumber] );

	if ( [theEvent buttonNumber] == 2 )
	{

		// [!] : middle button

		txtbox->grab_n_drag_onoff = FALSE;

		[[NSCursor arrowCursor] set];

	}

}

- (void) otherMouseDown:(NSEvent*) theEvent
{
//NSLog( @"otherMouseDown : %ld", (long) [theEvent buttonNumber] );

	// [!] : Grab N Drag

	if ( [theEvent buttonNumber] == 2 )
	{

		// [!] : middle button

		txtbox->grab_n_drag_onoff = TRUE;

		txtbox->pt_cur         = [NSEvent mouseLocation];
		txtbox->pt_grab_n_drag = [theEvent locationInWindow];

		[[NSCursor closedHandCursor] set];

	}

}

- (void) otherMouseDragged:(NSEvent*) theEvent
{
//NSLog( @"otherMouseDragged" );

	if ( [theEvent buttonNumber] == 2 )
	{

		// [!] : middle button

		txtbox->pt_prv = txtbox->pt_cur;
		txtbox->pt_cur = [NSEvent mouseLocation];

		CGFloat dy = txtbox->pt_cur.y - txtbox->pt_prv.y;
//NSLog( @"%f %f", dy, [theEvent deltaY] );

		txtbox->scr.unit_pos -= dy / txtbox->font_size.height;


		[self NonnonTxtboxRedraw];

	}

}



/*
-(void)mouseEntered:(NSEvent *)theEvent {
//NSLog(@"mouseEntered");
}

-(void)mouseExited:(NSEvent *)theEvent {
//NSLog(@"mouseExited");
}
*/



@end

