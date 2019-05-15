/**\file basicfun.cc
 * \brief Basic functions
 */
#include <cstring>
#include <cstdlib>
#include "basicfun.h"
#include <time.h>

long Status;		/**< \brief Status flag */
basic::BasicError Be;	/**< Global error values */

/**
 * \brief SYS function
 *
 *	This is based on the RSTS/E definitions.
 *
 * \note Not really usefull on Unix because of the completely different
 *	operating system enviornment.
 */
std::string basic::sys(
	const std::string &command	/**< Command */
	)
{
	std::cerr << "SYS not functional" << std::endl;
	basic::BasicError error(-1);
	throw(error);

	//
	// First character
	//
	// 0 = Cancel Control/O on terminal
	// 1 = Enter tape mode on terminal
	// 2 = Enable echoing on terminal
	// 3 = Disable echoing on terminal
	// 4 = Enable ODT submode on terminal
	// 5 = Exit with no prompt message
	// 6 = SYS call to the file processor (FIP)
	// 7 = Get core common string
	// 8 = Put core common string
	// 9 = Exit and clear program
	// 10 = Reserved for special implementations
	// 11 = Cancel all type ahead
	// 12 = Return information on last open file
	// 13 = Reserved for special implementations
	// 14 = Execute CCL command

	//
	// FIP Calls (second character)
	//
	// -29 = Get monitor tables - Part III
	// -28 = Spooling (Obsolete, use PBS request)
	// -27 = Snap shot dump
	// -26 = File utility functions
	// -25 = Read/Write file attributes
	//     = Read pack attributes
	//     = Read/Write account attributes
	//     = Delete account attributes
	// -24 = Add/delete CCL command
	// -23 = Terminating file name string scan
	// -22 = Get special run priority
	// -21 = Drop/regain (temporary) privileges
	// -20 = Lock/unlock job in memory
	// -19 = Set number of logins
	// -18 = Add run-time system
	//     = Remove run-time system
	//     = Unload run-time system
	//     = Add resident library
	//     = Remove resident library
	//     = Unload resident library
	//     = Create dynamic region
	// -17 = Name run-time system
	// -16 = Shut down system
	// -15 = Accounting Dump
	// -14 = Change system date/time
	// -13 = Change priority/runburst/job size
	// -12 = Get monitor tables - Part II
	// -11 = Change file backup statictics
	// -10 = File name string scan
	// -9 = Hang up a dataset
	// -8 = Get open channel statistics
	// -7 = Enable Control/C trap
	// -6 = Poke memory
	// -5 = Broadcast to terminal
	// -4 = Force input to terminal
	// -3 = Get monitor tables - Part I
	// -2 = Disable logins
	// -1 = Enable logins
	// 0 = Create user account
	// 1 = Delete user account
	// 2 = Reserved
	// 3 = Disk pack status
	// 4 = Login
	//   = Verify password
	// 5 = Logout
	// 6 = Attach
	//   = Reattach
	//   = Swap console
	// 7 = Detach
	// 8 = Change Quota
	//   = Set password
	//   = Kill job
	//   = Disable terminal
	// 9 = Return error messages
	// 10 = Allocate/reallocate device
	//    = Assign user logical
	// 11 = Deallocate device or user logical
	// 12 = Deallocate all devices
	// 13 = Zero a device
	// 14 = Read/Read and reset accounting information
	// 15 = Directory lookup on index
	//    = Special magtape directory lookup
	// 16 = Set terminal characteristics - Part I and II
	// 17 = Disk directory lookup on filename
	//    = Disk wildcard directory lookup
	// 18 = Obsolete (use function code 22)
	// 19 = Enable/Disable disk caching
	// 20 = Convert date and time
	// 21 = Add new logical names
	//    = Remove logical names
	//    = Change disk logical name
	//    = List logical names
	// 22 = Message send/receive
	//    = Send local data message with priveleges
	//    = Send print/batch services request
	// 23 = Add system files
	// 24 = Create a job
	// 25 = Wildcard PPN lookup
	// 26 = Return job status
	// 27 = Reserved
	// 28 = Set/Clear current privileges
	//    = Read current privileges
	// 29 = Stall/Unstall system
	// 30 = Reserved
	// 31 = Third party privilege check
	// 32 = Check file access rights
	//    = Convert privilege name to mask
	//    = Convert privilege mask to name
	// 33 = Open next disk file
	// 34 = Set device characteristics
	//    = Set line printer characteristics
	//    = Set system defaults
	//    = Load monitor overlay code
	//
}

/**
 * \brief Qdate - Date function
 * \bug Ignores the passed parameter
*/
std::string basic::Qdate(
	int d		/**< Option */
)
{
	char workdate[14];		// dd-mmm-yy work area
	time_t now_t;			// Current date as time_t
	struct tm *now_tm;		// Current date as tm

	now_t = time(0);
	now_tm = localtime(&now_t);
	strftime(workdate, sizeof(workdate), "%d-%b-%y", now_tm);

	return std::string(workdate);
}

/**
 * \brief Qtime - Date function
 * \bug Ignores the passed parameter
*/
std::string basic::Qtime(
	int d		/**< Option */
)
{
	char workdate[14];		// dd-mmm-yy work area
	time_t now_t;			// Current date as time_t
	struct tm *now_tm;		// Current date as tm

	now_t = time(0);
	now_tm = localtime(&now_t);
	strftime(workdate, sizeof(workdate), "%I:%m %p", now_tm);

	return std::string(workdate);
}

