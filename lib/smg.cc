


#include <string>
#include <panel.h>

#include <smgdef.h>
#include <smgmsg.h>

#include "smg$routines.h"

int smg$begin_display_update(
	struct ncurses_struct **display_id)
{
}

int smg$begin_pasteboard_update(
	long *pasteboard_id)
{
	return 0;
}

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
	const long number_of_rows,
	const long number_of_columns,
	ncurses_struct **display_id,
	long display_attributes,
	long video_attributes,
	long character_set)
{
	ncurses_struct *dwin = new ncurses_struct;

	dwin->width = number_of_columns;
	dwin->height = number_of_rows;
	dwin->border = 0;
	dwin->hpos = 0;
	dwin->vpos = 0;

	if (display_attributes != 0 && display_attributes & SMG$M_BORDER)
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

long smg$create_virtual_keyboard(
	long *keyboard_id,
	const std::string *input_device,
	const std::string *default_filespec,
	std::string *resultant_filespec,
	const long *recall_size)
{
	return SMG$_NORMAL;
}

long smg$delete_virtual_display(
	struct ncurses_struct **display_id)
{
	del_panel((*display_id)->pan);
	delwin((*display_id)->win);
	update_panels();
}

/** Draw line
 */
int smg$draw_line(
	struct ncurses_struct **display_id,
	int srow,
	int scolumn,
	int erow,
	int ecolumn)
{
	if (srow == erow)
	{
		mvwhline((*display_id)->win, srow, scolumn, 0, ecolumn - scolumn + 1);
	}
	else
	{
		mvwvline((*display_id)->win, srow, scolumn, 0, erow - srow + 1);
	}
	return 0;
}

int smg$bend_display_update(
	struct ncurses_struct **display_id)
{
	wrefresh((*display_id)->win);
}

int smg$end_pasteboard_update(
	long *pasteboard_id)
{
	return 0;
}

long smg$erase_display(
	struct ncurses_struct **display_id,
	long start_row,
	long end_row,
	long start_column,
	long end_column)
{
	/*
	 * If they gave us the easy way
	 * No border, clear full window
	 */
	if ((*display_id)->border == 0 &
		start_row == 0 && end_row == 0 &&
		start_column == 0 && end_column == 0)
	{
		/*
		 * Wipe everything
		 */
		werase((*display_id)->win);
	}
	else
	{
		/*
		 * Be selective
		 */
		for (long row = start_row; row <= end_row; row++)
		{
			for (long column = start_column; row <= end_column; column++)
			{
				mvwaddch((*display_id)->win,
					row - 1 + (*display_id)->border,
					column - 1 + (*display_id)->border, ' ');
			}
		}
	}
}

int smg$erase_line(
	struct ncurses_struct **display_id,
	int vpos,
	int hpos)
{
	wmove((*display_id)->win, vpos, hpos);
	wclrtoeol((*display_id)->win);
	return 0;
}

long smg$flush_buffer(
	long *pasteboard_id)
{
	update_panels();
	refresh();
}


int smg$get_display_attr(
	struct ncurses_struct **display_id,
	long *height,
	long *width)
{
	if (height != 0)
	{
		*height = (*display_id)->height;
	}
	if (width != 0)
	{
		*width = (*display_id)->width;
	}
	return 1;
}

int smg$label_border(
	struct ncurses_struct **display_id,
	const std::string &newlabel)
{
	(*display_id)->label = newlabel;
	return 1;
}

long smg$paste_virtual_display(
	struct ncurses_struct **display_id,
	const long *pasteboard_id,
	long pasteboard_row,
	long pasteboard_column,
	long top_display_id)
{
	(*display_id)->hpos = pasteboard_column - 1;
	(*display_id)->vpos = pasteboard_row - 1;
	mvwin((*display_id)->win, (*display_id)->vpos, (*display_id)->hpos);

	return SMG$_NORMAL;
}

long smg$pop_virtual_display(
	struct ncurses_struct **display_id,
	const long *pasteboard_id)
{
	del_panel((*display_id)->pan);
	delwin((*display_id)->win);
	update_panels();
}

/** Put characters on the screen
 *
 * Parameters set to match the SMG versions.
 */
int smg$put_chars(
	struct ncurses_struct **display_id,
	const std::string &str,		/**< String to disply */
	int x,				/**< X posdition */
	int y,				/**< Y position */
	int flag,			/**< Flag value? */
	int attr)			/**< Attributes */
{
	if (attr & SMG$M_BOLD)
	{
		wattron((*display_id)->win, A_STANDOUT);
	}
	if (attr & SMG$M_REVERSE)
	{
		wattron((*display_id)->win, A_REVERSE);
	}

	mvwaddstr((*display_id)->win, y + (*display_id)->border, x + (*display_id)->border, str.c_str());
	wattrset((*display_id)->win, 0);
	return 0;
}

long smg$set_cursor_abs(
	struct ncurses_struct **display_id,
	long x,
	long y)
{
	wmove((*display_id)->win, x, y);
}

long smg$set_cursor_mode(
	long *pasteboard_id,
	long flags)
{
	return SMG$_NORMAL;
}

