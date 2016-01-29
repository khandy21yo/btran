/**\file bedit.cc
 * \brief EDIT$(str,val)
 *
 * EDIT$(), CVT$$() function.
 */

//
// History:
//
//	08/27/1998 - Kevin Handy
//		Broke out of bstring.cc
//
//	03/16/2000 - Kevin Handy
//		Improve documentation.
//		Make flag (256) work for (1) and (16)
//
// Include files
//
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include "bstring.h"

/**
 * \brief edit$() function
 *
 *	Implementation of the basic EDIT$ or CVT$$ function, which will
 *	do transformations on the source string depending on the flags
 *	passed.
 *
 *	-1  Discard each characters parity bit
 *	-2  Discards all spaces and tabs
 *	-4  Discards all carriage returns, line feeds, form feeds
 *		deletes, escapes, and nulls.
 *	-8  Discards leading spaces and tabs
 *	-16  Convert multiple spaces and tabs to a single space
 *	-32  Convert lowercase letters to upper case
 *	-64  Convert left bracket '[' to left parenthesis '(' and right
 *		bracket ']' to right parenthesis ')'
 *	-128  Discards trailing spaces and tabs
 *	-256  Supress all editing for characters in quotation marks.
 *		If there is only one quotation mark, then suppress to the
 *		end of the text.
 *
 *	\note Also used by CVT$$ code.
 */
std::string &basic::edit(
	const std::string& Source,	/**< String to be converted */
	const int Code			/**< Flag for conversion */
)
{
	std::string* NewString = new std::string;
	unsigned int Length;
	const char* Address;
	int ResultPtr = 0;
	int LeadSpace = 0;
	int MultiSpace = 0;
	int QuoteMark = 0;

	Length = Source.length();
	Address = Source.data();
	char* Result = new char[Length + 1];

	while (Length--)
	{
		unsigned char OneCh = *Address++;

		//
		// 1 - Discard parity bit
		//
		if ((Code & 1) && (QuoteMark == 0))
		{
			OneCh &= 0x7f;
		}

		//
		// 2 - Discard all spaces and tabs
		//
		if ((Code & 2) && ((OneCh == ' ') || (OneCh == '\t')))
		{
			if (QuoteMark == 0)
			{
				continue;
			}
		}

		//
		// 4 - Discard all carriage returns, line feeds,
		// form feeds, deletes, escapes, and nulls
		//
		if ((Code & 4) && ((OneCh == '\r') || (OneCh == '\n') ||
			(OneCh == '\f') || (OneCh == 27) || (OneCh == 0)))
		{
			if (QuoteMark == 0)
			{
				continue;
			}
		}

		//
		// 8 - Discard Leading Spaces and Tabs
		//
		if ((Code & 8) && (LeadSpace == 0))
		{
			if ((OneCh == ' ') || (OneCh == '\t'))
			{
				continue;
			}
			LeadSpace = 1;
		}

		//
		// 16 - Convert Multiple Spaces and Tabs to one space
		//
		if ((Code & 16) && (QuoteMark == 0))
		{
			if ((OneCh == ' ') || (OneCh == '\t'))
			{
				OneCh = ' ';
				if (MultiSpace != 0)
				{
					continue;
				}
				MultiSpace = 1;
			}
			else
			{
				MultiSpace = 0;
			}
		}

		//
		// 32 - Convert lower case to upper case
		//
		if (Code & 32)
		{
			if (QuoteMark == 0)
			{
				OneCh = toupper(OneCh);
			}
		}

		//
		// 64 - Convert left bracket to left parentheses, and
		// right bracket to right parentheses
		//
		if ((Code & 64) && (QuoteMark == 0))
		{
			if (OneCh == '[')
			{
				OneCh = '(';
			}
			if (OneCh == ']')
			{
				OneCh = ')';
			}
		}

		//
		// 256 - Suppress all editing for characters within
		// quotation marks. If the string has only one quotation
		// mark, suppresses all editing for the characters
		// following the quotation mark.
		//
		if (Code & 256)
		{
			if (QuoteMark)
			{
				if (OneCh == QuoteMark)
				{
					QuoteMark = 0;
				}
			}
			else
			{
				if ((OneCh == '"') || (OneCh == '\''))
				{
					QuoteMark = OneCh;
				}
			}
		}

		//
		// If we get here, we want to keep the character
		//
		Result[ResultPtr++] = OneCh;
	}

	//
	// 128 - Discard Trailing Spaces and Tabs
	//
	if ((Code & 128) && (QuoteMark == 0))
	{
		while ((ResultPtr > 0) &&
			((Result[ResultPtr - 1] == ' ') ||
			(Result[ResultPtr - 1] == '\t')))
		{
			ResultPtr--;
		}
	}

	//
	// Terminate the string
	//
	Result[ResultPtr] = '\0';

	//
	// Return the final answer
	//
	*NewString = Result;
	return *NewString;
}

