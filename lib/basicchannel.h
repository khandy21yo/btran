/**\file basicchannel.h
 * \brief Header file for Basic+ I/O
 *
 *
 *	Class to handle Basic channels.
 *
 *	This class is used in a program (usually as an array of
 *	channels) to handle the BASIC I/O statements.
 *
 *	It tries to map as much of the work through C++ streams
 *	as possible, so that converting to pure C++ will be
 *	easier.
 */
#ifndef _BASICCHANNEL_H_
#define _BASICCHANNEL_H_

//
// include files
//
#include <iostream>
#include <fstream>

#include "bstring.h"
#include "basicfun.h"

namespace basic
{
#ifdef RSTS_FILES
const static int MaxChannel = 13;	/** \brief Maximum number of IO channels
					 * 
					 * Channel 12 is maximum
					 */
#else
const static int MaxChannel = 100;	/** \brief Maximum number of IO Channels
					 *
					 * Channel 99 is maximum
					 */
#endif
}

/**
 * \brief Define the array of channels
 *
 * Creates an array of IO channels 
 * (number depends on if you are emulating a RSTS/E ot a VAX
 */
extern std::fstream BasicChannel[basic::MaxChannel + 1];

#endif
