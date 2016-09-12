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

//
// Include files
//
#include ,iostream>
#include <stdlib.h>
#include <string.h>
#include "basicfun.h"
#include "basicchannel.h"

namespace basic
{
//
// Global Variables
//
int Recount;		/**< \brief Last count of read characters */

/**
 * \brief Constructor
 *
 * Default constructor.
 */
BChannel::BChannel(void)
{
	outstream = NULL;
	instream = NULL;
	ioflag = 0;
	cpos = 0;
}

/**
 * brief Destructor
 *
 * Nothing intresting happens.
 */
BChannel::~BChannel(void)
{
}

/**
 * \brief Open without a file name
 *
 * Not yet functional
 *
 * \return Status value
 */
int BChannel::Open(void)
{
	std::cerr << "OPEN without name not yet functional" << std::endl;
	BasicError error(-1);
	throw(error);
	return 0;
}

/**
 * \brief Open when given a Basic String
 *
 * Opens a file when given a std::string filename
 *
 * \return Status value
 */
int BChannel::Open(
	const std::string& Name		/**< File name to open */
)
{
	return Open(Name.c_str());
}

/**
 * \brief Open when given a char string
 *
 * Opens a file when given a char* filename
 *
 * \return Status value
 * - 0 = success
 * - nonzero = error
 * \throw error Errors
 */
int BChannel::Open(
	const char* Name	//!< File name to open
)
{
	//
	// Handle the very common case of 'KB:' specially
	//
	if (strcasecmp(Name, "KB:") == 0)
	{
		OpenZero();
		return 0;
	}

	switch(ioflag)
	{
	case 0:	// Both input and output
		{
			std::ifstream *inxstream =
				new std::ifstream(Name, std::ios::in | std::ios::out);
			if (inxstream->bad())
			{
				BasicError error(5);	// Can't find file
				throw(error);
			}
			instream = inxstream;
			outstream = new std::ostream(inxstream->rdbuf());
		}
		break;

	case BASIC_FORINPUT:
		instream = new std::ifstream(Name);
		if (instream->bad())
		{
			BasicError error(5);	// Can't find file
			throw(error);
		}
		break;

	case BASIC_FOROUTPUT:
		outstream = new std::ofstream(Name);
		if (outstream->bad())
		{
			BasicError error(10);	// Protection violation
			throw(error);
		}
		break;
	default:
		{
			BasicError error(10);	// Protection violation
			throw(error);
		}
	}
	cpos = 0;
	return 0;
}

/**
 * \brief Set 'FOR INPUT' flag
 *
 * Sets up the "FOR INPUT" flag.
 * Forces the channel to close.
 *
 * \return 0 (future status code)
 */
int BChannel::ForInput(void)
{
	Close();
	ioflag = BASIC_FORINPUT;
	return 0;
}

/**
 * \brief Set 'FOR OUTPUT' flag
 *
 * Sets the "FOR OUTPUT" flag.
 * Forces the channel to close.
 *
 * \return 0 (future status code)
 */
int BChannel::ForOutput(void)
{
	Close();
	ioflag = BASIC_FOROUTPUT;
	return 0;
}

/**
 * \brief Set up the special channel 0
 *
 * Channel 0 is a special channel attached automatically
 * to the keyboard/screen.
 *
 * \return 0 (future status code)
 */
int BChannel::OpenZero(void)
{
	outstream = &std::cout;
	instream = &std::cin;
	cpos = 0;
	return 0;
}


/**
 * \brief Close all channels
 *
 * Without any parameters,
 * all channels get closed.
 *
 * \return 0 (future status code)
 */
int BChannel::Close(void)
{
	//
	// Close any open ostream's
	//
	if ((outstream != NULL) && (outstream != &std::cout))
	{
		delete outstream;
	}
	//
	// and any open istreams
	//
	if ((instream != NULL) && ((void*)instream != (void*)outstream) &&
		(instream != &std::cin))
	{
		delete instream;
	}
	outstream = NULL;
	instream = NULL;
	ioflag = 0;

	return 0;
}

/**
 * \brief RMS Delete record
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Delete(void)
{
	std::cerr << "DELETE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}

/**
 * \brief RMS Find record
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Find(void)
{
	std::cerr << "FIND not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}

/**
 * \brief RMS Get record
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Get()
{
	std::cerr << "GET not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}

/**
 * \brief RMS Put record
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Put()
{
	std::cerr << "PUT not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}

/**
 * \brief RMS Reset channel
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Reset()
{
	std::cerr << "RESET not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}

/**
 * \brief Unlock channel
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
int BChannel::Unlock()
{
	std::cerr << "UNLOCK not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return 0;
}


/**
 * \brief Inputline
 *
 * Inputs a line of text.
 *
 * \return 0 (Future status code)
 * \throw BasicError when failure occurs
 */
int BChannel::getline(
	char* Output,	//!< Buffer to fill
	int MaxChar	//!< Maximum number of characters to read
)
{
	cpos = 0;
	instream->getline(Output, MaxChar);
	if (instream->eof())
	{
		throw(BasicError(11));	// End of file on device
	}
	return 0;
}


/**
 * \brief RMS BUFFERLOC function
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
char* BChannel::BufferLoc()
{
	std::cerr << "BUFFERLOC not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
	return NULL;
}

/**
 * \brief RMS RECORDSIZE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetRecordSize()
{
	std::cerr << "SETRECORDSIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS FILESIZE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetFileSize()
{
	std::cerr << "SETFILESIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS EXTENDSIZE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetExtendSize()
{
	std::cerr << "SETEXTENDSIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS WINDOWSIZE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetWindowSize()
{
	std::cerr << "SETWINDOWSIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS Blocksize option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetBlockSize()
{
	std::cerr << "SETBLOCKSIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS BUCKETSIZE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetBucketSize()
{
	std::cerr << "SETBUCKETSIZE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief MODE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetMode()
{
	std::cerr << "SETMODE not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief SETBUFFER option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetBuffer()
{
	std::cerr << "SETBUFFER not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief DEFAULTNAME option
 *
 * Not yet functional
 *
 * \return 0 (Future status code
 * \throw BasicError Always fails
 */
void BChannel::SetDefaultName()
{
	std::cerr << "SETDEFAULTNAME not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS KEY option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetKey()
{
	std::cerr << "SETKEY not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS TEMPORARY option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetTemporary()
{
	std::cerr << "SETTEMPORARY not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief CONTIGUOUS option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetContiguous()
{
	std::cerr << "SETCONTIGUOUS not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS REWIND option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetRewind()
{
	std::cerr << "SETREWIND not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS SPAN option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetSpan()
{
	std::cerr << "SETSPAN not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS ORGANIZATION option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetOrginization()
{
	std::cerr << "SETORGANIZED not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief ACCESS option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetAccess()
{
	std::cerr << "SETACCESS not yet functional" << std::endl;
//	BasicError error(18);		// Illegal SYS usage
//	throw(error);
}

/**
 * \brief RMS ALLOW option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetAllow(const int flag)
{
	std::cerr << "SETALLOW not yet functional" << std::endl;
//	BasicError error(18);		// Illegal SYS usage
//	throw(error);
}

/**
 * \brief CONNECT option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetConnect()
{
	std::cerr << "SETCONNECT not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS MAP option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetMap()
{
	std::cerr << "SETMAP not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS RECORDTYPE option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetRecordType()
{
	std::cerr << "SETRECORD not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief Other unprocessed stuff
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::GoofyOpenStuff()
{
	std::cerr << "SETGOOFYOPENSTUFF not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief RMS RECORD option
 *
 * Not yet functional
 *
 * \return 0 (Future status code)
 * \throw BasicError Always fails
 */
void BChannel::SetRecord(int)
{
	std::cerr << "SETRECORD not yet functional" << std::endl;
	BasicError error(18);		// Illegal SYS usage
	throw(error);
}

/**
 * \brief CCPOS() function
 *
 * Attempts to return the horizontal cursor position.
 *
 * \return 0 (Future status code)
 */
int BChannel::Ccpos()
{
	return cpos;
}


/**
 * \brief Write end of line
 *
 * Writes out a "newline" to the channel.
 * Similiar to std::endl in C++.
 *
 * \return Channel used
 * \throw BasicError on error
 */
BChannel& endl(
	BChannel& outs	/**< Channel to write to */
)
{
	if (&outs == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	outs.GetOstream() << std::endl;
	outs.cpos = 0;
	return outs;
}

/**
 * \brief Write out string
 *
 * Output a std::string to the channel.
 * Attempts to track cursor position.
 *
 * \return Channel used
 * \throw BasicError on error
 * \bug Doesn't track tabs correctly.
 */
BChannel& operator<< (
	BChannel& os,		/**< Channel to write to */
	const std::string& bs		/**< String to write */
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	*(os.outstream) << bs;
	os.cpos += bs.length();
	return os;
}

/**
 * \brief Write out a character string
 *
 * Output a std::string to the channel.
 * Attempts to track cursor position.
 *
 * \return Channel used
 * \throw BasicError on error
 * \bug Doesn't track tabs correctly.
 */
BChannel& operator<< (
	BChannel& os,		/**< Channel to write to */
	const char* bs			/**< Character to write */
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	*(os.outstream) << bs;
	os.cpos += strlen(bs);
	return os;
}

/**
 * \brief Write out a floating point value
 *
 * Writes out a floating point number in a generic format.
 * Includes leading (for non-negitive values) and trailing spaces.
 *
 * \return Channel used
 * \throw BasicError on error
 */
BChannel& operator<< (
	BChannel& os,		/**!< Channel to write to */
	Dfloat bs		/**< Float value to write */
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	if (bs >= 0.0)
	{
		*(os.outstream) << ' ';
		os.cpos++;
	}
	*(os.outstream) << bs << ' ';
	// FIXME: Need to correctly calculate new position
	os.cpos += 2;
	return os;
}

/**
 * \brief Write out a long integer value
 *
 * Writes out an integer value using standard format.
 * Leading and trailing spaces are included.
 *
 * \return Channel used
 * \throw BasicError on error
 */
BChannel& operator<< (
	BChannel& os,		/**< Channel to write to */
	Int32 bs			/**< Value to write */
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	if (bs >= 0)
	{
		*(os.outstream) << ' ';
		os.cpos++;
	}
	*(os.outstream) << bs << ' ';
	os.cpos += 3 + int(log10((double)abs(bs)));
	return os;
}

/**
 * \brief Write out normal integer
 *
 * Writes out an integer value using standard format.
 * Leading and trailing spaces are included.
 *
 * \return Channel used
 * \throw BasicError on error
 */
BChannel& operator<< (
	BChannel& os,		//!< Channel to write to
	Int bs			//!< Integer to write
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	if (bs >= 0)
	{
		*(os.outstream) << ' ';
		os.cpos++;
	}
	*(os.outstream) << bs << ' ';
	os.cpos += 3 + int(log10((double)abs(bs)));
	return os;
}

/**
 * \brief Write out short integer
 *
 * Writes out an integer value using standard format.
 * Leading and trailing spaces are included.
 *
 * \return Channel used
 * \throw BasicError on error
 */
BChannel& operator<< (
	BChannel& os,		/**< Channel to write to */
	char bs				/**< Character to write */
)
{
	if (&os == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	*(os.outstream) << bs;

	//
	// Calculate current cursor position
	//
	switch(bs)
	{
	case '\n':
		os.cpos = 0;
		break;
	case '\t':
		os.cpos += (8 - os.cpos % 8);
		break;
	default:
		os.cpos++;
		break;
	}
	return os;
}

/**
 * \brief TAB() function
 *
 * Function used to generate the correct number of spaces
 * necessary to tab over to a specific column.
 * Resuires accurate ccpos() values.
 *
 * \return String with correct number of spaces to position
 */
std::string BChannel::Tab(
	int x		/**< Position to tab to */
)
{
        if ((x - cpos) > 0)
        {
                return std::string((unsigned int)(x - cpos), ' ');
        }
        else
        {
                return std::string("");
        }
}
 
/**
 * \brief Grab a string (up to a comma, tab, or newline)
 *
 * Normal "INPUT X$" functon.
 *
 * \return Channel used
 */
BChannel& operator>> (
	BChannel& is,		/**< Input channel */
	std::string& bs			/**< String to save in */
)
{
	if (&is == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	if (is.instream->eof())
	{
		BasicError error(11);	// End of file on device
		throw(error);
	}
	std::string inputstr;
	is.cpos = 0;
	while(1)
	{
		char inchar;
		is >> inchar;
		switch(inchar)
		{
		case '\n':
		case ',':
		case '\t':
			bs = inputstr;
			return is;

		case '\r':
			break;

		default:
			inputstr += inchar;
			break;

		}
	}
}

/**
 * \brief Input a long integer value
 *
 * Normal "INPUT X%" function.
 *
 * \return Channel used
 * \bug Bad entry is ignored.
 */
BChannel& operator>> (
	BChannel& is,		/**< Channel to read from */
	Int32& bs			/**< Long value to read */
)
{
	std::string instring;
	is >> instring;
	bs = atol(instring.c_str());		// Need to check for errors here
	return is;
}

/**
 * \brief Input a short integer value
 *
 * Normal "INPUT X%" function.
 *
 * \return Channel used
 * \bug Bad entry is ignored.
 */
BChannel& operator>> (
	BChannel& is,		/**< Channel to read from */
	Int& bs				/**< Integer to read into */
)
{
	std::string instring;
	is >> instring;
	bs = atoi(instring.c_str());		// Need to check for errors here
	return is;
}

/**
 * \brief Input a float value
 *
 * Normal "INPUT X" function.
 *
 * \return Channel used
 * \bug Bad entry is ignored.
 */
BChannel& operator>> (
	BChannel& is,	/**< Channel to read from */
	double& bs		//!< Double to store value in */
)
{
	std::string instring;
	is >> instring;
	bs = strtod(instring.c_str(), 0);	// Need to check for errors here
	return is;
}

/**
 * \brief Get a single character from the input stream
 *
 *	This routine is unlikely to be used in user code, but is used
 *	as a basis for all of the other input routines for BChannel
 *
 * \return Channel used
 */
BChannel& operator>> (
	BChannel& is,		/**< Channel to read from */
	char& bs			/**< Character to read into */
)
{
	if (&is == NULL)
	{
		BasicError error(9);	// I/O channel not open
		throw(error);
	}
	if (is.instream->eof())
	{
		BasicError error(11);	// End of file on device
		throw(error);
	}
	(is.instream)->get(bs);		// Must use get() to be able to see \n
	is.cpos = 0;
	return is;
}

}
