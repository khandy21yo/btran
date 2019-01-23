/** \file basicchannel.cc
 * \brief Source for Basic+ I/O

	Class to handle Basic channels.

	This class is used in a program (usually as an array of
	channels) to handle the BASIC I/O statements.

	It tries to map as much of the work through C++ streams
	as possible, so that converting to pure C++ will be
	easier.

	\bug Errors need to be improved. Decent numbers need to be allocated.
	Really nee to get a basic error number header file.
	Just throowing the most common errors, without really isolating
		the exact error that should be returned.
 */

//
// Include files
//
#include "basicchannel.h"

std::fstream BasicChannel[basic::MaxChannel + 1];

