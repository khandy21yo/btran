


#include <string>
#include <panel.h>

#include <smgdef.h>
#include <smgmsg.h>

#include "smg$routines.h"

long smg$change_pbd_characteristics(
	long *pasteboard_id,
	const long *desired_width,
	long *width,
	const long *desired_height,
	long *height,
	long *desired_background_color,
	long *background_color
)
{
	if (width != 0)
	{
		*width  = COLS;
	}
	if (height != 0)
	{
		*height = LINES;
	}
	return SMG$_NORMAL;
}

long smg$create_pasteboard(
	long *pasteboard_id,
	const std::string *output_device,
	long *number_of_pasteboard_rows,
	long *number_of_pasteboard_columns,
	const long *flags,
	long *type_of_terminal,
	std::string *device_name)
{
	initscr();	// Initialize curses
	cbreak();	// single character input mode
	noecho();	// don't echo characters as they are typed

	*pasteboard_id = 1234;	// Just to make it non-zero
	if (number_of_pasteboard_rows != 0)
	{
		*number_of_pasteboard_rows = COLS ;
	}
	if (number_of_pasteboard_columns != 0)
	{
		*number_of_pasteboard_columns = LINES;
	}

	return SMG$_NORMAL;
}

long smg$create_virtual_display(
	const long *number_of_rows,
	const long *number_of_columns,
	smg_display_struct **display_id,
	const long *display_attributes,
	const long *video_attributes,
	const long *character_set)
{
	smg_display_struct *dwin = new smg_display_struct;

	dwin->width = *number_of_columns;
	dwin->height = *number_of_rows;
	dwin->border = 0;
	dwin->hpos = 0;
	dwin->vpos = 0;

	if (display_attributes != 0 && *display_attributes & SMG$M_BORDER)
	{
		dwin->border = 1;
	}
	else
	{
		dwin->border = 0;
	}
	//
	// Initilally created at (0,0)
	//
	dwin->win = newwin(dwin->height + dwin->border * 2,
		dwin->width + dwin->border * 2, 0, 0);
	dwin->pan = new_panel(dwin->win);

	*display_id = dwin;

	return SMG$_NORMAL;
}

long smg$paste_virtual_display(
	struct smg_display_struct **display_id,
	const long *pasteboard_id,
	const long *pasteboard_row,
	const long *pasteboard_column,
	const long top_display_id)
{
	(*display_id)->hpos = *pasteboard_column - 1;
	(*display_id)->vpos = *pasteboard_row - 1;
	mvwin((*display_id)->win, (*display_id)->vpos, (*display_id)->hpos);

	return SMG$_NORMAL;
}

