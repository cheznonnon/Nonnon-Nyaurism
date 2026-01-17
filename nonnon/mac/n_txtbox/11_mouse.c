// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File



/*
CGFloat
n_mac_txtbox_focus_calculate( NSPoint local_point )
{
	return trunc( scroll ) + trunc( ( local_point.y - offset_y ) / font_size.height );
}
*/



@implementation NonnonTxtbox (NonnonTxtboxMouse)




- (void) NonnonTxtboxMouseClamp
{

//NSLog( @"NonnonTxtboxMouseClamp : %f %f", scroll, scroll_step );
//NSLog( @"NonnonTxtboxMouseClamp : %f %lld", scroll + scroll_page, n_txt_data->sy );
//NSLog( @"NonnonTxtboxMouseClamp : %f %lld", scroll + scroll_page + scroll_step, n_txt_data->sy );

	CGFloat line_sy = n_txt_data->sy;

	if ( scroll < 1.0 )
	{
		scroll = 0;
	} else
	if ( ( scroll + scroll_page + scroll_step ) > line_sy )
	{
//NSLog( @"overshoot" );
		scroll = line_sy - scroll_page;
	}

	return;
}




- (void) NonnonTxtboxClickEvent:(NSEvent*)theEvent detailed:(BOOL)onoff
{

	static NSInteger prev = 0;


	[self becomeFirstResponder];
	//if ( n_txtbox_focus != self ) { return; }


	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		if ( n_txt_data->readonly ) { return; }

		NSPoint local_point = n_mac_cursor_position_get( self );

		n_focus = [self NonnonTxtboxFocusCalculate:local_point];
		if ( n_focus >= n_txt_data->sy )
		{
			if ( n_listbox_no_selection_onoff )
			{
				[self NonnonTxtboxCaretReset];
				[self NonnonTxtboxRedraw];
				return;
			} else {
				n_focus = n_txt_data->sy - 1;
			}
		}
//NSLog( @"%f", n_focus );

		thumb_is_hovered = FALSE;

		if ( ( self.n_listbox_edit_onoff )&&( listbox_edit_index == n_focus ) )
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
				caret_fr = caret_to = [self NonnonTxtboxMouseCursorDetect];
			}

			[self NonnonTxtboxRedraw];
		} else {
			self.n_listbox_edit_onoff = FALSE;

			caret_fr = caret_to = [self NonnonTxtboxMouseCursorDetect];

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
	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
//NSLog( @"!" );

		if ( n_mac_window_is_hovered_offset_by_rect( self, find_icon_rect ) )
		{
			find_icon_is_pressed = TRUE;
		} else
		if ( n_mac_window_is_hovered_offset_by_rect( self, delete_circle_rect ) )
		{
			delete_circle_is_pressed = TRUE;
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
				caret_fr = caret_to = [self NonnonTxtboxMouseCursorDetect];
			}
		}

		thumb_is_hovered = FALSE;

		caret_blink_force_onoff = TRUE;

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
			caret_fr = caret_to = [self NonnonTxtboxMouseCursorDetect];
		}

		thumb_is_hovered = FALSE;

		caret_blink_force_onoff = TRUE;


		[self NonnonTxtboxRedraw];

	}
}

- (BOOL) acceptsFirstMouse:(NSEvent *)event
{
	return self.n_direct_click_onoff;
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

	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( n_mac_window_is_hovered_offset_by_rect( self, delete_circle_rect ) )
		{
			if ( delete_circle_is_hovered == FALSE )
			{
//NSLog( @"In" );
				delete_circle_is_hovered = TRUE;
				n_bmp_fade_always_on( &delete_circle_fade_hovered, n_bmp_black, n_bmp_white );
				[self NonnonTxtboxRedraw];
			}
		} else {
			if ( delete_circle_is_hovered != FALSE )
			{
//NSLog( @"Out" );
				delete_circle_is_hovered = FALSE;
				n_bmp_fade_always_on( &delete_circle_fade_hovered, n_bmp_white, n_bmp_black );
				[self NonnonTxtboxRedraw];
			}
		}

	}

}

- (void) mouseExited:(NSEvent *)theEvent
{
//NSLog( @"mouseExited" );

	// [Needed] : NSTrackingMouseEnteredAndExited

	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( delete_circle_is_hovered != FALSE )
		{
//NSLog( @"Out" );
			delete_circle_is_hovered = FALSE;
			n_bmp_fade_always_on( &delete_circle_fade_hovered, n_bmp_white, n_bmp_black );
			[self NonnonTxtboxRedraw];
		}
	}

}

- (void) mouseUp:(NSEvent*) theEvent
{
//NSLog( @"mouseUp" );


	[self NonnonTxtboxMouseClamp];


	BOOL is_dragged = drag_timer_queue;

	drag_timer_queue = FALSE;


	if ( thumb_is_captured )
	{
		thumb_is_captured = FALSE;

		scrollbar_fade_captured_onoff = TRUE;
		n_bmp_fade_always_on( &scrollbar_fade, n_bmp_black, n_bmp_white );
	}


	if ( thumb_is_hovered ) { return; }
	if ( shaft_is_hovered ) { return; }

	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		//

	} else {

		if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
		{
			if ( find_icon_is_pressed )
			{
				if ( n_mac_window_is_hovered_offset_by_rect( self, find_icon_rect ) )
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

				find_icon_is_pressed = FALSE;
			} else
			if ( delete_circle_is_pressed )
			{
				delete_circle_is_pressed = FALSE;

				if ( n_mac_window_is_hovered_offset_by_rect( self, delete_circle_rect ) )
				{
					delete_circle_fade_pressed_phase = 1;
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

	pt               = [NSEvent mouseLocation];
	thumb_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_thumb );
	shaft_is_hovered = n_mac_window_is_hovered_offset_by_rect( self, scroller_rect_shaft );
//if ( thumb_is_hovered ) { NSLog( @"!" ); }

	NSPoint pt_cur   = n_mac_cursor_position_get( self );
	thumb_offset     = scroller_rect_thumb.origin.y - pt_cur.y;
//NSLog( @"%f : %f %f", thumb_offset, scroller_rect_thumb.origin.y, pt_cur.y );

	if ( thumb_is_hovered )
	{

		if ( thumb_is_captured == FALSE )
		{
			thumb_is_captured = TRUE;

			scrollbar_fade_captured_onoff = TRUE;
			n_bmp_fade_always_on( &scrollbar_fade, n_bmp_black, n_bmp_white );
		}

	} else
	if ( shaft_is_hovered )
	{

		CGFloat sy               = [self frame].size.height;
		CGFloat csy              = sy - ( offset_y * 2 );
		CGFloat items_per_canvas = csy / font_size.height;

		if ( pt_cur.y < scroller_rect_thumb.origin.y )
		{
//NSLog( @"upper" );
			scroll -= items_per_canvas;
		} else {
//NSLog( @"lower" );
			scroll += items_per_canvas;
		}

//NSLog( @"%f / %f", scroll, scroll_step );
		scroll = trunc( scroll );

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

	CGPoint pt_cur = [NSEvent mouseLocation];

	CGFloat dy = pt.y - pt_cur.y;
//NSLog( @"%f %f", dy, [theEvent deltaY] );

	pt = pt_cur;

//scroll -= dy / font_size.height;

	if ( thumb_is_captured )
	{
//NSLog( @"%f", scroll );

		n_type_gfx scroll_csy = (n_type_gfx) self.frame.size.height - ( offset_y * 2 );


		CGFloat max_c = (CGFloat) n_txt_data->sy;
		CGFloat step  = scroll_csy / ( max_c * font_size.height );


		scroll += dy / ( font_size.height * step );

		[self NonnonTxtboxRedraw];

	} else
	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
	{

		if ( thumb_is_hovered ) { return; }
		if ( shaft_is_hovered ) { return; }

		[self NonnonTxtboxRedraw];

	} else {

		if ( thumb_is_hovered ) { return; }
		if ( shaft_is_hovered ) { return; }

		if ( find_icon_is_pressed ) { return; }


//NSLog( @"%ld", [theEvent clickCount] ); return;
		if ( [theEvent clickCount] != 1 ) { return; }


		caret_to = [self NonnonTxtboxMouseCursorDetect];
//n_caret_debug_cch( caret_fr, caret_to );

		if ( caret_fr.cch.x < caret_to.cch.x )
		{
			shift_selection_is_tail = TRUE;
		} else {
			shift_selection_is_tail = FALSE;
		}


		drag_timer_queue = TRUE;


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


	smooth_wheel_delta = [theEvent deltaY];
	if ( smooth_wheel_delta == 0 ) { return; }


	if ( [theEvent scrollingDeltaY] == 0.0 ) { smooth_wheel_is_mos = TRUE; }


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

		smooth_wheel_delta = [theEvent scrollingDeltaY];

		CGFloat step;

		step = fabs( smooth_wheel_delta );
		step = MIN( step, 2.0 );

		if ( smooth_wheel_delta < 0 )
		{
			scroll += step;
		} else {
			scroll -= step;
		}

		[self NonnonTxtboxRedraw];

	} else
	if ( smooth_wheel_is_mos )
	{

		CGFloat step;

		step = fabs( smooth_wheel_delta );
		step = MIN( step, 2.0 );

		if ( smooth_wheel_delta < 0 )
		{
			scroll += step;
		} else {
			scroll -= step;
		}

		[self NonnonTxtboxRedraw];

	} else {

		smooth_wheel_delta = [theEvent scrollingDeltaY];

		if ( smooth_wheel_onoff == FALSE )
		{
			smooth_wheel_onoff = TRUE;
			smooth_wheel_mutex = TRUE;
		}

	}

}




- (void) rightMouseUp:(NSEvent*) theEvent
{
//NSLog( @"rightMouseUp" );

	if ( self.n_mode == N_MAC_TXTBOX_MODE_FINDBOX )
	{
		if ( find_icon_is_pressed )
		{
			if ( n_mac_window_is_hovered_offset_by_rect( self, find_icon_rect ) )
			{
				if ( delegate_option & N_MAC_TXTBOX_DELEGATE_F3 )
				{
					[self.delegate NonnonTxtbox_delegate_F3:self is_left:NO];
				}
			}

			[self NonnonTxtboxRedraw];

			find_icon_is_pressed = FALSE;
		} else
		if ( delete_circle_is_pressed )
		{
			delete_circle_is_pressed = FALSE;

			if ( n_mac_window_is_hovered_offset_by_rect( self, delete_circle_rect ) )
			{
				delete_circle_fade_pressed_phase = 1;
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

	pt = [NSEvent mouseLocation];

	if ( self.n_mode == N_MAC_TXTBOX_MODE_LISTBOX )
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

		grab_n_drag_onoff = FALSE;

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

		grab_n_drag_onoff = TRUE;

		pt             = [NSEvent mouseLocation];
		pt_grag_n_drag = [theEvent locationInWindow];

		[[NSCursor closedHandCursor] set];

	}

}

- (void) otherMouseDragged:(NSEvent*) theEvent
{
//NSLog( @"otherMouseDragged" );

	if ( [theEvent buttonNumber] == 2 )
	{

		// [!] : middle button

		CGPoint pt_cur = [NSEvent mouseLocation];

		CGFloat dy = pt.y - pt_cur.y;
//NSLog( @"%f %f", dy, [theEvent deltaY] );

		pt = pt_cur;


		scroll -= dy / font_size.height;


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

