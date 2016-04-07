

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
		*number_of_pasteboard_rows =COLS ;
	}
	if (number_of_pasteboard_columns != 0)
	{
		*number_of_pasteboard_columns=LINES;
	}

	return 1;
}
