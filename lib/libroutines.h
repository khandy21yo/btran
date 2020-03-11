//! \file lib$routines.h
//!
//! \brief vms compatability routines
//!

#ifndef _lib$routines_h_
#define _lib$routines_h_

#include <string>

long lib$delete_file(
	const std::string &filespec,
	const std::string &default_filespec = "",
	const std::string &related_filespec = "",
	long user_success_procedure = 0,
	long user_error_procedure = 0,
	long user_confirm_procedure = 0,
	long user_specified_arguement = 0,
	std::string *resultant_name = 0,
	long file_scan_contest = 0,
	long flags = 0);
long lib$find_file(
	const std::string &filespec,
	std::string &result,
	long &context,
	const std::string &deflt = "",
	const std::string related = "",
	long flags = 0);



#endif

