//!< \file lib.cc
//!
//! \brief vms compatability library
//!


#include <ctype.h>
#include <glob.h>
#include <libgen.h>
#include <vector>
#include <unistd.h>

#include "lib$routines.h"

/** \brief Delete a file
 *
 * This version only looks at filespec. All other parameters are ignored.
 */
long lib$delete_file(
	const std::string &filespec,
	const std::string &default_filespec,
	const std::string &related_filespec,
	long user_success_procedure,
	long user_error_procedure,
	long user_confirm_procedure,
	long user_specified_arguement,
	std::string *resultant_name,
	long file_scan_contest,
	long flags)
{
	unlink(filespec.c_str());
	return 1;
}

static std::vector<std::string> files;	/**< list of files scanned */

/** \brief look up file names
 *
 * Linux version of a VMS function for conversion purposes.
 * Only enough emulated to make it work in specific contexts.
 */
long lib$find_file(
	const std::string &filespec,
	std::string &result,
	long &context,
	const std::string &deflt,
	const std::string related,
	long flags)
{
	glob_t pglob;
	char *name_buffer;

	if (context == 0)
	{
		/*
		 * Look up files until we run out of files, or run out of
		 * array elements.
		 */
		glob(filespec.c_str(),
			GLOB_MARK | GLOB_TILDE,
			0 /* errfunc */,
			&pglob);

		for (int loop = 0; loop < pglob.gl_pathc; loop++)
		{
			name_buffer = pglob.gl_pathv[loop];
			files.push_back(name_buffer);
		}

		/*
		 * Finish up directory call
		 */
		globfree(&pglob);

		context = 1;
	}

	if (context < files.size())
	{
		result = files[context++];
		return 1;
	}
	else
	{
		result = "";
		return 0;
	}
}

/** \brief free up context for lib_find_file
 *
 * Frees up resources allocated by lib_find_file
 */
long lib_find_file_end(
	long &context)
{
	if (context)
	{
		files.clear();
		context = 0;
	}
}

