// CatPad
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// Partial File




typedef struct {

	n_txt   txt;
	CGFloat scroll;
	CGFloat focus;
	n_caret caret_fr;
	n_caret caret_to;

} n_undo;

#define n_undo_zero( p ) n_memory_zero( p, sizeof( n_undo ) )


#define N_TXTBOX_UNDO_RESTORE  ( 0 )
#define N_TXTBOX_UNDO_REGISTER ( 1 )

