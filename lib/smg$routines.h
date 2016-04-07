// smg$routines
//
// Part of a simple library using ncurses to implement SMG under
// Linux?unix enough to make a specific set of programs work.
//
// Definitions based on the SMG routines manual.
// This is not the VMS smg#routines header file.
// Many incompatabilities exist, many functions are missing, and
// those that exist may not operate like the original.
//
# if !defined _smg_routines_h_
#define _smg_routines_h_

#include <string>

long smg$create_pasteboard(
	long *pasteboard_id,
	const std::string *output_device = 0,
	long *number_of_pasteboard_rows = 0,
	long *number_of_pasteboard_columns = 0,
	const long *flags = 0,
	long *type_of_terminal = 0,
	std::string *device_name = 0);

#endif
