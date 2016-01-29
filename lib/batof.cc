//
// batof.cc
//
//	String class for Basic to C++ translator
//
// Description:
//
//	Implements the atof function for the 'bstring' class.
//
// History:
//
//	03/02/1998 - Kevin Handy
//		Split out from bstring.cc
//
// Include files
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "bstring.h"

#ifndef BASIC_USE_STL

/*! \brief atof function

Convert a number stored as an ASCII text string into a floating
point value.
*/
double std::string::atof() const
{
	unsigned short Length;
	char* Address;
	double result;

	//
	// Determine string info
	//
	Length = length();
	Address = data();

	//
	// make a copy we can do stuff to
	//
	if (Length == 0)
	{
		return 0.0;
	}
	char* Work = new char[Length + 1];
	assert(Work != NULL);

	memcpy(Work, Address, Length);
	Work[Length] = '\n';

	//
	// Now we can actually do an atof
	//
	result = ::atof(Work);

	//
	// Release allocated memory
	//
	delete Work;

	//
	// Return result
	//
	return result;
}
#else
#endif
