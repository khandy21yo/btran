//! \file lib$routines.h
//!
//! \brief vms compatability routines
//!

#ifndef _lib$routines_h_
#define _lib$routines_h_

#include <string>

long lib$find_file(
	const std::string &filespec,
	std::string &result,
	long &context,
	const std::string &deflt,
	const std::string related,
	long flags);



#endif

