/*! \file basic_main.cc

	This function interfaces the C++ 'main' function to
	the BASIC 'basic_main' function, which is the starting
	point of all translated basic programs.

	This allows me to do various startup procedures that
	need to be done to set up the translated enviornment.
*/
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "bstring.h"
#include "basicchannel.h"

//
// External declaration of the basic_main function that gets generated.
//
int basic_main();

//
// Global variables that need to be defined in one place
//
basic::BChannel BasicChannel[basic::MaxChannel + 1];	//! BASIC I/O Channels

//
//! Here is the main startup code
//
int main(int argc, char* argv[])
{
	//
	// Initialize channel zero as the console
	//
	BasicChannel[0].OpenZero();

	try
	{
		basic_main();
	}
	catch (basic::BasicError error)
	{
		std::cerr << "BASIC Error: " << error.ErrorNo << " " <<
			basic::ert(error.ErrorNo) << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
