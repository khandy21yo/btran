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

//
// Global Variables
//
extern int Recount;

static const int BASIC_FORINPUT = 1;	/**< \brief Channel is input only */
static const int BASIC_FOROUTPUT = 2;	/**< \brief Channel is output only */
static const int MODIFY = 1;		/**< \brief Modify flag */

class BChannel;

typedef BChannel& (*__bmanip)(BChannel&);

/**
 * \brief IO Channel class
 *
 * Defines a single IO channel.
 * 
 * \note The main reason for this class instead of using a simple std::ostream
 * is the need to track the cursor position so that TAB() and CCPOS()
 * functions can work, 
 * and to be able to have both input and output from the same channel.
 */
class BChannel
{
private:
	std::string filename;		/**< \brief  Name of file open on channel */
	std::ostream* outstream;	/**< \brief  ostream attached to file */
	std::istream* instream;		/**< \brief istream attached to file */
	int ioflag;			/**< \brief Input/Output flag */

public:
	int cpos;			/**< \brief Cursor position */

public:
	//
	// Constructors
	//
	BChannel();
	~BChannel();
	/**
	 * \brief Get the output io channel associated with the channel
	 *
	 * Grabs the std::ostream.
	 *
	 * \return the std::ostream associated with the channel
	 */
	std::ostream& GetOstream(void)
	{ return *outstream; }
	/**
	 * \brief Get the input io channel associated with the channel
	 *
	 * Grabs the std::istream.
	 *
	 * \return the std::istream associated with the channel
	 */
	std::istream& GetIstream(void) 
	{ return *instream; }


	//
	// Open
	//
	int Open();
	int Open(const std::string& Name);
	int Open(const char* Name);
	int ForInput();
	int ForOutput();

	int OpenZero();

	//
	// RMS Access
	//
	int Close();
	int Delete();
	int Find();
	int Get();
	int Put();
	int Reset();
	int Unlock();

	//
	// Inputline
	//
	int getline(char* Output, int MaxChar);
	std::string Tab(int x);

	//
	// Special
	//
	char* BufferLoc();
	void SetRecordSize();
	void SetFileSize();
	void SetExtendSize();
	void SetWindowSize();
	void SetBlockSize();
	void SetBucketSize();
	void SetMode();
	void SetBuffer();
	void SetDefaultName();
	void SetKey();
	void SetTemporary();
	void SetContiguous();
	void SetRewind();
	void SetSpan();
	void SetOrginization();
	void SetAccess();
	void SetAllow(const int);
	void SetConnect();
	void SetMap();
	void SetRecordType();
	void GoofyOpenStuff();
	void SetRecord(int);
	int Ccpos();

	//
	// Output functions (Horrible Hack, needs lots of work)
	//
	friend BChannel& operator<< (BChannel& os, const std::string& bs);
	friend BChannel& operator<< (BChannel& os, const char* bs);
	friend BChannel& operator<< (BChannel& os, double bs);
	friend BChannel& operator<< (BChannel& os, Int32 bs);
	friend BChannel& operator<< (BChannel& os, Int bs);
	friend BChannel& operator<< (BChannel& os, char bs);

	/**
	 * \brief Make io manipulators work line in std::ostreams
	 *
	 * This assumes that __omanip is defined already in iostream.h
	 *
	 * \return The original channel, so the operation can be chained.
	 */
	BChannel& operator<<(
		__bmanip func		/**< I dunno, but that what std::ostream wants */
	)
	{ return (*func)(*this); }


	//
	// Input functions (Horrible Hack, needs lots of work)
	//
	friend BChannel& operator>> (BChannel& is, std::string& bs);
	friend BChannel& operator>> (BChannel& is, double& bs);
	friend BChannel& operator>> (BChannel& is, Int32& bs);
	friend BChannel& operator>> (BChannel& is, Int& bs);
	friend BChannel& operator>> (BChannel& is, char& bs);

};

//
// Manipulators
//
extern BChannel& endl(BChannel& outs);

}
/**
 * \brief Define the array of channels
 *
 * Creates an array of IO channels 
 * (number depends on if you are emulating a RSTS/E ot a VAX
 */
extern basic::BChannel BasicChannel[basic::MaxChannel + 1];
namespace basic
{
/**
 * \brief Cursor position
 *
 * Calculates the current cursor position
 * (as best we can tell).
 *
 * \return The estimated cursor position.
 * \note There are many ways for this to be wrong.
 */
inline int Ccpos(int x) { return BasicChannel[x].Ccpos(); }
}
#endif
