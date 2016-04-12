

#include "smg$routines.h"
#include "smgdef.h"

#include <string>
#include <panel.h>

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

	return 1;
}

long smg$create_virtual_display(
	const long *number_of_rows,
	const long *number_of_columns,
	long *display_id,
	const long *display_attributes,
	const long *video_attributes,
	const long *character_set)
{
	WINDOW *win;			//!< Window
	PANEL *pan;			//!< Panel
	int width = *number_of_columns;
	int height = *number_of_rows;
	int border = 0;

	if (display_attributes != 0 && *display_attributes & SMG$M_BORDER)
	{
		border = 1;
	}
	else
	{
		border = 0;
	}
	//
	// Initilally created at (0,0)
	//
	win = newwin(height + border * 2, width + border * 2, 0, 0);
	pan = new_panel(win);

	*display_id = pan;

	return 1;
}
