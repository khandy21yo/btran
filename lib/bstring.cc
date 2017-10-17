/** \file bstring.cc
 * \brief String class for Basic to C++ translator
 *
 *	This file impliments a string class for my Basic
 *	translater using VMS style descriptors and string
 *	functions. This allows the programs to interface
 *	with the VMS routines as easily as under the
 *	real VAX/BASIC compiler.
 *
 *	This is really nothing more than a wrapper around
 *	the STR$xxx functions, and the DSC$DESCRIPTOR_S
 *	stuff.
 */

//
// Include files
//
#include <cstdio>
#include <cctype>
#include <cassert>
#include <string>
#include <iostream>
#include <cstring>

#include "basicfun.h"
#include "bstring.h"
#include "basicchannel.h"

//
// Global values
//
long basic::ErrNum = 0;		/**< Error number (ern) */
long basic::ErrLine = 0;	/**< Error Line (erl) */
long basic::Status = 0;		/**< Status (status) */


//******************************************************************

/**
 * \brief NUM$(integer)
 *
 * Convert an intger value into a string.
 *
 * \return String containing converted value.
 */
std::string basic::Qnum(
	long Number	/**< Number to convert */
)
{
	return std::to_string(Number) + " ";
}

/**
 * \brief NUM$(float)
 *
 *	Convert an float value into a string.
 *
 * \return String containing converted value.
 */
std::string basic::Qnum(
	double Number	/**< Value to convert */
)
{
	return std::to_string(Number);
}

/**
 * \brief NUM1$(integer)
 *
 * Convert an intger value into a string.
 *
 * \return String containing converted value.
 * \note NUM$ and NUM1$ are the same for inteer values.
 */
std::string basic::Qnum1(
	long Number	/**< Number to convert */
)
{
	return std::to_string(Number);
}

/**
 * \brief NUM1$(float)
 * Convert a float value into a string.
 * NUM1$ differs from NUM$ in that it will not use scientific
 * notation in the converted string.
 *
 * \return String containing converted value.
*/
std::string basic::Qnum1(
	double Number	/**< Number to convert */
)
{
	return std::to_string(Number);
}

/**
 * \brief RAD$()
 *
 *	Convert a number to its radix50 (50octal = 40decimal)
 *	string equivelent.
 *
 *	\return String containing 3 character result.
 */
std::string basic::Qrad(
	const unsigned int Number	/**< Number to convert */
)
{
	const char RadChar[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ$.?0123456789";
	char Result[4];

	Result[0] = RadChar[(Number / (40 * 40)) % 40];	// 1st Character
	Result[1] = RadChar[(Number / 40) % 40];	// 2nd Character
	Result[2] = RadChar[Number % 40];		// 3rd character
	Result[3] = '\0';

	return std::string(Result);
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an integer array.
 */
void basic::BChange1(
	const std::string& source,	/**< Source string */
	int *dest			/**< Destination array */
)
{
	int lt = source.length();
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = int(source[loop]);
	}
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an integer array.
 */
void basic::BChange1(
	const char* source,	/**< Source string */
	int *dest		/**< Destination array */
)
{
	int lt = strlen(source);
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = int(source[loop]);
	}
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an long array.
 */
void basic::BChange1(
	const std::string& source,	/**< Source string */
	long *dest			/**< Destination array */
)
{
	int lt = source.length();
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = long(source[loop]);
	}
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an long array.
 */
void basic::BChange1(
	const char* source,	/**< Source string */
	long *dest		/**< Result array */
)
{
	int lt = strlen(source);
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = long(source[loop]);
	}
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an double array.
 */
void basic::BChange1(
	const std::string& source,	/**< Source string */
	double *dest			/**< Result array */
)
{
	int lt = source.length();
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = double(source[loop]);
	}
}

/**
 * \brief CHANGE x$ TO x%
 *
 *	Change a string into an double array.
 */
void basic::BChange1(
	const char* source,	/**< Source string */
	double *dest		/**< Result array */
)
{
	int lt = strlen(source);
	dest[0] = lt;
	for (int loop = 0; loop < lt; loop++)
	{
		dest[loop + 1] = double(source[loop]);
	}
}


/**
 * \brief CHANGE x% TO x$
 *
 *	Change an integer array into a string.
 */
void basic::BChange2(
	const int *source,		/**< Source array */
	std::string& dest		/**< Result string */
)
{
	dest = "";
	for (int loop = 1; loop <= source[0]; loop++)
	{
		dest += char(source[loop]);
	}
}

/**
 * \brief CHANGE x% TO x$
 *
 *	Change an integer array into a string.
 */
void basic::BChange2(
	const long *source,		/**< Source array */
	std::string& dest		/**< Result string */
)
{
	dest = "";
	for (int loop = 1; loop <= source[0]; loop++)
	{
		dest += char(source[loop]);
	}
}


/**
 * \brief CHANGE x% TO x$
 *
 *	Change an integer array into a string.
 */
void basic::BChange2(
	const double *source,		/**< Source array */
	std::string& dest		/**< Result string */
)
{
	dest = "";
	for (int loop = 1; loop <= source[0]; loop++)
	{
		dest += char(source[loop]);
	}
}


/**
 * \brief Inkey
 *
 *	Input a single character from the user
 */
std::string basic::Inkey(
	long channel,		/**< IO Channel to use */
	long timeout		/**< Timeout value */
)
{
//	std::cerr << "Inkey not yet implemented!" << std::endln;
	exit(1);
}

/**
 * \brief Convert ascii to integer
 *
 *	Really needs some error trapping.
 * \return integer value of string
 * \bug should use STRTOD instead of ATOI.
 */
int basic::atoi(
	const std::string &str	/**< String to convert */
)
{
	return atoi(str.c_str());
}
