/** \file pusing.cc
 * \brief Print using formatting
 *
 * \bug
 *	Each '.Output()' call must be on a seperate line, because
 *	C++ processes the arguments to 'cout <<' from right to left,
 *	which is backwards from this set of functions need.
 *
 *	This is preliminary stuff, so your mileage may vary.
 *
 *	Currently doesn't handle lower case format characters.
 */

#include <cstdio>
#include <string>
#include <cmath>
#include <cstring>
#include <cstdlib>

#include "bstring.h"
#include "pusing.h"

//
// Local function prototypes
//

//
// Constants for string format
//
static const int SIDE_LEFT = 0;		/**< \brief LSet the string "'LLLL" */
static const int SIDE_RIGHT = 1;	/**< \brief RSet the string "'RRRR" */
static const int SIDE_CENTER = 2;	/**< \brief Center the string "'CCCC" */
static const int SIDE_EXPAND = 3;	/**< \brief Use whatever space is needed "'E" */

static const char FILL_CHAR = ' ';	/**< Normal fill character */

static const int FORMAT_STRING = 1;	/**< Format is a string */
static const int FORMAT_NUMBER = 2;	/**< Format is numeric */
static const int FORMAT_END = 3;	/**< End of format string */

/**
 * \brief Constructor
 *
 * Used when the format string is not yet known.
 */
basic::PUsing::PUsing()
{
	BaseFormat = NULL;
	BaseLength = 0;
	BasePtr = 0;
	Outdata = NULL;
	OutdataPtr = NULL;
}

/**
 * \brief Constructor
 *
 * Used when there is an initial format string
 */
basic::PUsing::PUsing(
	const char* Format	/**< Print using format string */
)
{
	BaseFormat = Format;
	BaseLength = strlen(Format);
	BasePtr = 0;
	Outdata = NULL;
	OutdataPtr = NULL;
}

static char *fstr = 0;	/**< Temp format string holder */
/**
 * \brief Constructor
 *
 * Used when there is an initial format string
 */
basic::PUsing::PUsing(
	const std::string &Format	/**< Print using format string */
)
{
	if (fstr)
	{
		free(fstr);
		fstr = 0;
	}
	fstr = strdup(Format.c_str());
	BaseFormat = fstr;
	BaseLength = strlen(fstr);
	BasePtr = 0;
	Outdata = NULL;
	OutdataPtr = NULL;
}

/**
 * \brief Output string value
 *
 * Output function to write out a std::string value
 *
 * \return Formatted string
 */
std::string basic::PUsing::Output(
	const std::string& Value	/**< String value to output */
)
{
	return Output(Value.data(), Value.length());
}

/**
 * \brief Output character string value
 *
 * Output function to write out a char* value
 *
 * \return Formatted string
 */
std::string basic::PUsing::Output(
	const char* Valu,	/**< String to write out */
	int ValuLength		/**< Length of string */
)
{
	int FWidth;
	int FSide;

	//
	// Create a buffer
	//
	Outdata = new char[ValuLength + BaseLength + 1];
	OutdataPtr = Outdata;

	//
	// Skip over intro stuff
	//
	int Type = SkipFront();

	//
	// Pull off numeric format information
	//
	if (Type == FORMAT_STRING)
	{
		ScanStringFormat(FWidth, FSide);
		FormatString(Valu, ValuLength, FWidth, FSide);
	}

	//
	// Generate BString for output
	//
	std::string* Result = new std::string(Outdata, (long)(OutdataPtr - Outdata));
	delete[] Outdata;

	return *Result;
}

/**
 * \brief Finish up print using
 *
 * Flushes out remaining portions of the format string
 *
 * \return Formatted string
 */
std::string basic::PUsing::Finish()
{
	//
	// Create a work area
	//
	Outdata = new char[BaseLength];
	OutdataPtr = Outdata;

	//
	// Pull in non-format characters
	//
	SkipFront();

	//
	// Generate output string
	//
	std::string* Result = new std::string(Outdata, (long)(OutdataPtr - Outdata));
	delete[] Outdata;

	return *Result;
}

/**
 * \brief Skip over the text on the front of the format string.
 *
 * Skips over leading information in format string.
 * Determines the next type that will be formatted next.
 *
 * \return
 *	- FORMAT_STRING - A String format starts next<BR>
 *	- FORMAT_NUMBER - A Numerical format starts next<BR>
 *	- FORMAT_END - The End of format string
 */
int basic::PUsing::SkipFront()
{
	//
	// Scan until we get to the end of the string
	//
	while(BasePtr < BaseLength)
	{
		switch (BaseFormat[BasePtr])
		{
		//
		// Start of a "'xxx" string format.
		//
		case '\'':
			return FORMAT_STRING;

		//
		// Start of a "!" string format
		//
		case '!':
			return FORMAT_STRING;

		//
		// Start of a "/   /" string format
		//
		case '/':
			return FORMAT_STRING;

		//
		// Next character is quoted
		//
		case '_':
			BasePtr++;
			if (BasePtr == BaseLength)
			{
				*OutdataPtr++ = '_';
				return FORMAT_END;
			}
			*OutdataPtr++ = BaseFormat[BasePtr++];
			break;

		//
		// Dollar sign (May be the start of a number)
		//
		case '$':
			//
			// Last character
			//
			if (BasePtr + 1 == BaseLength)
			{
				*OutdataPtr++ = '$';
				return FORMAT_END;
			}

			//
			// Next character is a dollar sign
			//
			if (BaseFormat[BasePtr + 1] == '$')
			{
				return FORMAT_NUMBER;
			}

			//
			// Keep it
			//
			*OutdataPtr++ = BaseFormat[BasePtr++];
			break;

		case '#':
			//
			// A Pound sign
			//
			return FORMAT_NUMBER;

		case '*':
			//
			// Last character
			//
			if (BasePtr + 1 == BaseLength)
			{
				*OutdataPtr++ = '*';
				return FORMAT_END;
			}

			//
			// Next character is a dollar sign
			//
			if (BaseFormat[BasePtr + 1] == '*')
			{
				return FORMAT_NUMBER;
			}

			//
			// Keep it
			//
			*OutdataPtr++ = BaseFormat[BasePtr++];
			break;


		case '<':
			//
			// Look for any of '<0>', '<%>'
			//
			if ((strncmp(BaseFormat+BasePtr, "<0>", 3) == 0) ||
				(strncmp(BaseFormat+BasePtr, "<%>", 3) == 0))
			{
				return FORMAT_NUMBER;
			}

			//
			// Nope, just another character
			//
			*OutdataPtr++ = BaseFormat[BasePtr++];
			break;

		default:
			//
			// Normal character, output it
			//
			*OutdataPtr++ = BaseFormat[BasePtr++];
			break;
		}
	}

	//
	// Reached end of string w/o finging format character
	//
	return FORMAT_END;
}

/**
 * \brief Scan the format string for formatting definition
 *
 * Scans the format string for the next format specification
 */
void basic::PUsing::ScanStringFormat(
	int &FWidth,	/**< Returned width */
	int &FSide	/**< Returned justification */
)
{
	char FormChar;		// Marker character

	//
	// Give out some initial values.
	// Make sure we'll output at least one character.
	//
	FWidth = 1;
	FSide = SIDE_LEFT;

	//
	// Determine which type of string format we've got
	//
	switch(BaseFormat[BasePtr])
	{
	//
	// Single character format
	//
	case '!':
//		FSide = SIDE_LEFT;
		BasePtr++;
		break;

	//
	// Old string format
	//
	case '/':
//		FSide = SIDE_LEFT;
		BasePtr++;

		//
		// Scan for end marker
		//
		while ((BaseFormat[BasePtr] == ' ') &&
			(BasePtr < BaseLength))
		{
			BasePtr++;
			FWidth++;
		}

		//
		// Eat up closing marker
		//
		if ((BaseFormat[BasePtr] =='/') && (BasePtr < BaseLength))
		{
			BasePtr++;
			FWidth++;
		}
		break;

	//
	// New String Format
	//
	case '\'':
//		FSide = SIDE_LEFT;
		BasePtr++;
		FormChar = BaseFormat[BasePtr];

		switch(FormChar)
		{
		case 'L':
//			FSide = SIDE_LEFT;

			while ((BaseFormat[BasePtr] == FormChar) &&
				(BasePtr < BaseLength))
			{
				BasePtr++;
				FWidth++;
			}
			break;


		case 'R':
			FSide = SIDE_RIGHT;

			while ((BaseFormat[BasePtr] == FormChar) &&
				(BasePtr < BaseLength))
			{
				BasePtr++;
				FWidth++;
			}
			break;

		case 'C':
			FSide = SIDE_CENTER;

			while ((BaseFormat[BasePtr] == FormChar) &&
				(BasePtr < BaseLength))
			{
				BasePtr++;
				FWidth++;
			}
			break;

		case 'E':
//			FSide = SIDE_LEFT;
			FSide = SIDE_EXPAND;
			BasePtr++;
			break;
		}
		break;
	}

	return;
}

/**
 * \brief Format a string according to a set of flags
 *
 * Does the actual formatting of a string.
 */
void basic::PUsing::FormatString(
	const char* ParmPtr,	/**< String to format */
	int ParmLength,		/**< Length of string */
	int Length,		/**< Output length */
	int Side		/**< Justification */
)
{
	int FillSize;		// Number of characters used for fill

	//
	// Format according to available size/Side
	//
	switch(Side)
	{
	case SIDE_RIGHT:
		//
		// Pad the right with spaces
		//
		if (Length > ParmLength)
		{
			FillSize = Length - ParmLength;
			while(FillSize != 0)
			{
				*OutdataPtr++ = FILL_CHAR;
				FillSize--;
			}
		}

		//
		// Add on the current string
		//
		if (ParmLength > Length)
		{
			FillSize = Length;	// Only want first x chars
		}
		else
		{
			FillSize = ParmLength;	// Whole string will fit
		}
		memcpy(OutdataPtr, ParmPtr, FillSize);
		OutdataPtr += FillSize;
		break;

	case SIDE_CENTER:
		//
		// Pad the right with spaces
		//
		if (Length > ParmLength)
		{
			FillSize = (Length - ParmLength) / 2;
			while(FillSize != 0)
			{
				*OutdataPtr++ = FILL_CHAR;
				FillSize--;
			}
		}

		//
		// Add on the current string
		//
		if (ParmLength > Length)
		{
			FillSize = Length;	// Only want first x chars
		}
		else
		{
			FillSize = ParmLength;	// Whole string will fit
		}
		memcpy(OutdataPtr, ParmPtr, FillSize);
		OutdataPtr += FillSize;

		//
		// Pad the right with spaces
		//
		if (Length > ParmLength)
		{
			FillSize = (Length - ParmLength + 1) / 2;
			while(FillSize != 0)
			{
				*OutdataPtr++ = FILL_CHAR;
				FillSize--;
			}
		}

		break;

	case SIDE_EXPAND:
		//
		// Add on the current string
		//
		memcpy(OutdataPtr, ParmPtr, ParmLength);
		OutdataPtr += ParmLength;
		break;

	default:
		//
		// Add on the current string
		//
		if (ParmLength > Length)
		{
			FillSize = Length;	// Only want first x chars
		}
		else
		{
			FillSize = ParmLength;	// Whole string will fit
		}
		memcpy(OutdataPtr, ParmPtr, FillSize);
		OutdataPtr += FillSize;

		//
		// Pad the right with spaces
		//
		FillSize = Length - ParmLength;
		while(FillSize > 0)
		{
			*OutdataPtr++ = FILL_CHAR;
			FillSize--;
		}

		break;

	}

}

/**
 * \brief Format a number according to given specification
 *
 *	- Leading stars '**'			FORMAT_STAR<BR>
 *	- Leading $ '$$'			FORMAT_DOLLAR<BR>
 *	- Minus on back specified '-##', '##-'	FORMAT_MINUS<BR>
 *	- Exp format '^^^^'			FORMAT_EXP<BR>
 *	- Leading zero's '<0>'			FORMAT_ZERO<BR>
 *	- Blank zero number '<%>'		FORMAT_BLANK<BR>
 *	- CR/DR 				FORMAT_DRCR<BR>
 *	- Comma's				FORMAT_COMMA<BR>
 *
 * \return Formatted string.
 */
std::string basic::PUsing::Output(
	double Param		/**< Number to format */
)
{
	int Digits = 0;		// Number of digit positions
	int Decimals = 0;	// Number of decimal positions
	int Point = 0;		// Decimal point seen?
	int Comma = 0;		// Output comma's?
	int Stars = 0;		// Leading stars?
	int Minus = 0;		// Minus (0 = none specified,
				//  2 = trailing, 3 = <CD>
	int Dollar = 0;		// Floating '$' sign.
	int ZeroFill = 0;	// Fill with leading 0's
	int Percent = 0;	// Pergentage format
	int Exponent = 0;	// Scientific notation

	int KeepLooping = 1;	// Loop till quit

	//
	// Create a buffer
	//
	Outdata = new char[BaseLength + 10];
	OutdataPtr = Outdata;

	//
	// Skip over intro stuff
	//
	int Type = SkipFront();

	//
	// Pull off numeric format information
	//
	if (Type == FORMAT_NUMBER)
	{
		//
		// Look for leading items
		//
		KeepLooping = 1;
		while (KeepLooping == 1)
		{
			//
			// Leading stars
			//
			if ((Stars == 0) &&
				(strncmp(BaseFormat + BasePtr, "**", 2) == 0))
			{
				Digits += 2;
				BasePtr += 2;
				Stars = 1;
				continue;
			}

			//
			// Floating '$' sign
			//
			if ((Dollar == 0) &&
				(strncmp(BaseFormat + BasePtr, "$$", 2) == 0))
			{
				Digits += 1;
				BasePtr += 2;
				Dollar = 1;
				continue;
			}

			//
			// Fill with 0's
			//
			if ((ZeroFill == 0) &&
				(strncmp(BaseFormat + BasePtr, "<0>", 3) == 0))
			{
				Digits += 1;
				BasePtr += 3;
				ZeroFill = 1;
				continue;
			}

			//
			// Percentage
			//
			if ((Percent == 0) &&
				(strncmp(BaseFormat + BasePtr, "<%>", 3) == 0))
			{
				Digits += 1;
				BasePtr += 3;
				Percent = 1;
				continue;
			}

			KeepLooping = 0;
		}

		//
		// Scan for central items
		//
		KeepLooping = 1;
		while (KeepLooping == 1)
		{
			//
			// Decimal position
			//
			if (BaseFormat[BasePtr] == '#')
			{
				if (Point == 0)
				{
					Digits++;
				}
				else
				{
					Decimals++;
				}
				BasePtr++;
				continue;
			}

			//
			// Comma
			//
			if (BaseFormat[BasePtr] == ',')
			{
				if (Point == 0)
				{
					Digits++;
				}
				else
				{
					Decimals++;
				}
				BasePtr++;
				Comma = 1;
				continue;
			}

			//
			// Decimal point
			//
			if ((Point == 0) && (BaseFormat[BasePtr] == '.'))
			{
				Point = 1;
				BasePtr++;
				continue;
			}
			KeepLooping = 0;
		}

		//
		// Trailing items
		//
		KeepLooping = 1;
		while (KeepLooping == 1)
		{
			//
			// <CD>
			//
			if ((Minus == 0) &&
				(strncmp(BaseFormat + BasePtr, "<CD>", 4) == 0))
			{
				BasePtr += 4;
				Minus = 3;
				continue;
			}

			if ((Exponent == 0) &&
				(strncmp(BaseFormat + BasePtr, "^^^^", 4) == 0))
			{
				BasePtr += 4;
				Exponent = 4;
				continue;
			}

			//
			// Trailing sign
			//
			if ((Minus == 0) && (BaseFormat[BasePtr] == '-'))
			{
				Minus = 2;
				BasePtr++;
				continue;
			}
			KeepLooping = 0;
		}

	}

//cout << "Digits " << Digits << " Decimals " << Decimals << " Point " << Point <<
//	" Comma " << Comma << " Stars " << Stars << " Minus " << Minus <<
//	" Dollar " << Dollar << " ZeroFill " << ZeroFill <<
//	" Percent " << Percent << " Exponent " << Exponent << std::endl;

	//***********************************************************
	// Now that we've scanned the format, we need to generate
	// the output
	//***********************************************************

	if ((Percent != 0) && (Param == 0.0))
	{
		long PDigits;		// Digit portion of number

		//
		// Calculate size of output
		//
		PDigits = Dollar + Digits + Point + Decimals + Exponent;
		switch(Minus)
		{
		case 2:
			PDigits++;
			break;
		case 3:
			PDigits += 2;
			break;
		}

		//
		// Output appropiate number of spaces
		//
		while(PDigits)
		{
			*OutdataPtr++ = ' ';
			PDigits--;
		}
	}
	else
	{
		int PSign;		// Sign of result
		long PDigits;		// Digit portion of number
		long PExp = 0;		// Exponent
		double PAbs;		// Absolute value of number
		char Work[64];		// Working buffer
		int WorkPtr = 0;	// Pointer into working buffer

		if (Param < 0)
		{
			PSign = 1;
			PAbs = -Param;
		}
		else
		{
			PSign = 0;
			PAbs = Param;
		}

		//
		// If we are using scientific notation, normalize the
		// number
		//
		if (Exponent)
		{
			PExp = (int)floor(log10(PAbs));
			PAbs = PAbs / pow(10.0, PExp * 1.0);
		}

		//
		// handle the decimal portion first
		//
		//	Generates it in a work string in reverse order, which
		//	will be reversed back before appending to the output.
		//
		//	NOTE: The conversion to a long may not be the best thing
		//	to do, but this is just version 1 anyway.
		//
		//	Also, I don't check for overflow of the work buffer
		//
		WorkPtr = 0;
		PDigits = (long)floor(PAbs);
		if (PDigits == 0)
		{
			Work[WorkPtr++] = '0';
		}
		else
		while (PDigits)
		{
			//
			// Place a comma every 4th character
			//
			if ((Comma != 0) && ((WorkPtr + 1) % 4) == 0)
			{
				Work[WorkPtr++] = ',';
			}

			//
			// Place one digit (This is why I'm using an integer)
			//
			Work[WorkPtr++] = (PDigits % 10) + '0';

			//
			// Drop the digit just added
			//
			PDigits /= 10;
		}

		//
		// Handle any dollar sign
		//
		if (Dollar != 0)
		{
			Work[WorkPtr++] = '$';
		}

		//
		// Minus sign?
		//
		if ((Minus == 0) && (PSign != 0))
		{
			Work[WorkPtr++] = '-';
		}

		//
		// Fill to final length
		//
		char FillChar;
		if (Stars != 0)
		{
			FillChar = '*';
		}
		else
		{
			if (ZeroFill != 0)
			{
				FillChar = '0';
			}
			else
			{
				FillChar = ' ';
			}
		}
		while (WorkPtr < Digits + Dollar)
		{
			Work[WorkPtr++] = FillChar;
		}

		//
		// Write out this portion to the output
		//
		while (WorkPtr)
		{
			*OutdataPtr++ = Work[--WorkPtr];
		}

		//
		// Decimal point?
		//
		if (Point)
		{
			*OutdataPtr++ = '.';
		}

		//
		// Decimal digits
		//
		//	We drop off the digit part, then add in a rounding
		//	amout to help hide any (.99999) type problems.
		//
		PAbs = PAbs - floor(PAbs) + pow(10.0, -(Decimals + 1));
		WorkPtr = 1;
		while (WorkPtr <= Decimals)
		{
			//
			// Place a comma every 4th character
			//
			if ((Comma != 0) && (WorkPtr % 4) == 0)
			{
				*OutdataPtr++ = ',';
				WorkPtr++;
			}

			//
			// Place one decimal
			//
			PAbs *= 10;
			PDigits = (long)floor(PAbs);
			PAbs -= PDigits;

			*OutdataPtr++ = (PDigits % 10) + '0';
			WorkPtr++;
		}

		//
		// Trailing sign
		//
		switch(Minus)
		{
		case 2:
			if (PSign)
			{
				*OutdataPtr++ = '-';
			}
			else
			{
				*OutdataPtr++ = ' ';
			}
			break;

		case 3:
			if (PSign)
			{
				*OutdataPtr++ = 'C';
				*OutdataPtr++ = 'R';
			}
			else
			{
				*OutdataPtr++ = 'D';
				*OutdataPtr++ = 'R';
			}
			break;
		}

		//
		// If we are using scientific notation, output it now
		//
		if (Exponent)
		{
			sprintf(Work, "E%-3ld", PExp);
			strcpy(OutdataPtr, Work);
			OutdataPtr += strlen(Work);
		}
	}

	//
	// Send out the result
	//
	*OutdataPtr = '\0';
	std::string *Result = new std::string(Outdata);
	return *Result;
}

/**
 * \brief Format integer for output
 *
 * Formats a long according to the format string.
 *
 * \bug This is a real quick and dirty hack to get thiss working
 * at a minimum level. Should be written better
 *
 * \return Formatted string.
 */
std::string basic::PUsing::Output(
	int Param	/**< Integer to be formatted */
)
{
	return Output((double)Param);
}

/**
 * \brief Format long for output
 *
 * Formats a long according to the format string.
 *
 * \bug This is a real quick and dirty hack to get thiss working
 * at a minimum level. Should be written better
 *
 * \return Formatted string.
 */
std::string basic::PUsing::Output(
	long Param	/**< Long to be formatted */
)
{
	return Output((double)Param);
}

