/**
\file b1filter.cc

Filter old RSTS/E Basic+ program into VAX Basic format
This filter will read in old format Basic Plus programs
and convert them into a more modern format that is more
acceptable to VAX Basic (and my Basic to C++ translator)

\bug Data statements and remark statements may confuse this
simple program, so watch them.
\bug Its simple lexing routines can be easily confused, so
you may have to do some hand editing afterwards (yuck)
*/
//
// \author 03/13/2000 - Kevin Handy
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>

extern "C"
{
#include <getopt.h>
#include <ctype.h>
}

//
// Set up for the options
//
//! Short option list
static const char* OptionList = "o:O:hHe";
#ifdef USE_LONGOPT
//! Long option list
static const struct option OptionListLong[] =
{
	{"output", 1, 0, 'o'},
	{"help", 0, 0, 'h'},
	{"extended", 0, 0, 'e'},
	{0, 0, 0, 0}
};
#endif

//
// Local functions
//
static void ProcessFile(std::ostream* Outfile, std::istream* InFile,
	const char* name);
static void LoadFile(std::istream* InFile, std::vector<std::string> &source);
static int LoadLine(std::istream *Infile, std::string& oneline);
static void MungFile(std::vector<std::string> &source);
static void MungWhitespace(std::vector<std::string> &source);
static void MungWhitespaceLine(std::string &sloop);
static void MungContinuation(std::vector<std::string> &source);
static void Writefile(std::ostream *OutFile, std::vector<std::string> &source);
static int IsQuoted(const std::string& source, int pos);
static int isbpunct(char source);

//
// Global variables
//
int Extended = 0;	//!< Extend mode program?

////////////////////////////////////////////////////////////////////
//! Here is the main program
//
int main(int argc, char* argv[])
{
        int OptionSelected;		// getopt parameter
        int OptionIndex;		// getopt parameter
	int DidOneFlag = 0;		// Have we done any files?
	std::ostream *OutFile = &std::cout;	// Output file
	std::istream *InFile;		// Input file

        //
        // Handle input arguments
        //
#ifdef USE_LONGOPT
        while ((OptionSelected = getopt_long(argc, argv, OptionList,
                OptionListLong, &OptionIndex)) != -1)
#else
        while ((OptionSelected = getopt(argc, argv, OptionList)) != -1)
#endif
        {
                //
                // Process one arguement
                //
                switch (OptionSelected)
                {
		case 'o':
		case 'O':
			//
			// Output File
			//
			if (OutFile != &std::cout)
			{
				delete OutFile;
				OutFile = 0;
			}

			OutFile = new std::ofstream(optarg);
			if (OutFile->bad())
			{
				std::cerr << "Unable to open " << optarg <<
					"for output" << std::endl;
				delete OutFile;
				OutFile = 0;
				exit(1);
			}

			break;

		case 'h':
		case 'H':
			DidOneFlag = 1;
			std::cerr << "Usage: basic [-h] "
				"[-o <file>] <source>" << std::endl;

			break;

		case 'e':
			Extended = 1;
			break;

		}
	}

	//
	// Anything left over is files names to process
	//
	while (optind < argc)
	{
		//
		// At least one file name on command line
		//
		DidOneFlag = 1;

		InFile = new std::ifstream(argv[optind]);
		if (InFile->bad())
		{
			std::cerr << argv[0] << ": Bad Input File " <<
				argv[optind] << std::endl;
		}
		else
		{
			ProcessFile(OutFile, InFile, argv[optind]);
		}
		delete InFile;

		optind++;
	}

	//
	// If we haven't seen any file names, then get input
	// from the users terminal
	//
	if (DidOneFlag == 0)
	{
		ProcessFile(OutFile, &std::cin, "Standard Input");
	}

	//
	// Close OutFile if necessary
	//
	if (OutFile != &std::cout)
	{
		delete OutFile;
	}
}

////////////////////////////////////////////////////////////////////
//! Handle everything for one file
//
static void ProcessFile(
	std::ostream* OutFile,	//!< Output stream
	std::istream* InFile,	//!< Inout stream
	const char* name	//!< File name
)
{
//	std::cout << "Processing  " << name << std::endl;

	std::vector<std::string> source;

	LoadFile(InFile, source);
	MungFile(source);
	Writefile(OutFile, source);
}

////////////////////////////////////////////////////////////////////
//! Read in the file
//
static void LoadFile(std::istream* InFile,	//!< Input stream
	std::vector<std::string> &source	//!< Returned source
)
{
	std::string oneline;

	//
	// Just keep looping until we see the end of file marker
	//
	while (LoadLine(InFile, oneline) == 0)
	{
		source.push_back(oneline);
	}
}

////////////////////////////////////////////////////////////////////
/*! Read in a single line, stripping off any garbage

Assumes that EOF will occur on a blank line
*/
static int LoadLine(
	std::istream *InFile,	//!< Input stream
	std::string& oneline	//!< Return single source code line
)
{
	oneline = "";
	int inchar;
	int squote = 0;
	int dquote = 0;

	while ((inchar = InFile->get()) != EOF)
	{
		switch(inchar)
		{
		case '&':
			//
			// Shorthand for print maybe
			//
			if ((squote == 0) && (dquote == 0) && (Extended == 0))
			{
				oneline += "PRINT";
			}
			else
			{
				if ((Extended == 0) || (squote != 0) ||
					(dquote != 0))
				{
					oneline += char(inchar);
				}
			}
			break;

		case ':':
			//
			// Old statement separator
			//
			if ((squote == 0) && (dquote == 0) && (Extended == 0))
			{
				oneline += "\\";
			}
			else
			{
				oneline += char(inchar);
			}
			break;

		case '"':
			//
			// Maybe start of a double quote
			//
			if (dquote)
			{
				dquote = 0;
			}
			else
			{
				if (squote == 0)
				{
					dquote = 1;
				}
			}
			oneline += char(inchar);
			break;

		case '\'':
			//
			// Maybe start of a single quote
			//
			if (squote)
			{
				squote = 0;
			}
			else
			{
				if (dquote == 0)
				{
					squote = 1;
				}
			}
			oneline += char(inchar);
			break;

		case '\r':
		case '\0':
			//
			// Lose stupid extra characters
			//
			break;

		case '\n':
			//
			// Aha! We have the actual end of a line
			//
			return 0;
			break;

		default:
			//
			// Just stuff it on the end of the string
			//
			if (squote || dquote)
			{
				oneline += char(inchar);
			}
			else
			{
				oneline += toupper(inchar);
			}
			break;
		}
	}

	return 1;
}

////////////////////////////////////////////////////////////////////
//! Process the file into the correct format
//
static void MungFile(
	std::vector<std::string> &source	//!< Entire source code
)
{
	MungWhitespace(source);
	MungContinuation(source);
}

////////////////////////////////////////////////////////////////////
//! Reserved word list
static const char* reserved[] =
{
	"ASCII",
	"CLOSE",
	"CONTINUE",
	"DATA",
	"DEF",
	"DIM",
	"ELSE",
	"ERROR",
	"FILE",
	"FOR",
	"GOTO",
	"GO TO",
	"IF",
	"INPUT",
	"LET",
	"NEXT",
	"OPEN",
	"PRINT",
	"READ",
	"RECORDSIZE",
	"RECORD",
	"REM",
	"RESTORE",
	"THEN",
	"SIZE",
	"STOP",
	"STEP",
	"GO SUB",
	"GOSUB",
	"TO",
	"ON",
	"OR",
	"AS",
	0
};

////////////////////////////////////////////////////////////////////
/*! Add spaces where necessary to entire source file

This routine attempts to insert whitespace between common
basic commands.
*/
static void MungWhitespace(
	std::vector<std::string> &source	//!< Entire source code
)
{
	for (std::vector<std::string>::iterator sloop = source.begin();
		sloop != source.end(); sloop++)
	{
		MungWhitespaceLine(*sloop);
	}
}

////////////////////////////////////////////////////////////////////
/*! Add spaces where necessary to one source line

This routine attempts to insert whitespace between common
basic commands.
*/
static void MungWhitespaceLine(
	std::string &source		//!< Single source line
)
{
	for (int pos = 0; pos < source.length(); )
	{
		//
		// Are we looking at a reserved word at this position?
		//
		for (int rloop = 0; reserved[rloop] != 0; rloop++)
		{
			int rlen = strlen(reserved[rloop]);

			//
			// Do a really quick check on the first letter
			// followed by a full length match
			//
			if ((source[pos] == reserved[rloop][0]) &&
				(source.substr(pos, rlen) == reserved[rloop]))
			{
				//
				// Put spaces before string
				//
				if ((pos > 0) &&
					!isspace(source[pos - 1]) &&
					!isbpunct(source[pos - 1]))
				{
					source.insert(pos, " ");
					pos++;
				}

				//
				// Put spaces after string
				//
				if (!isspace(source[pos + rlen]) &&
					!isbpunct(source[pos + rlen]))
				{
					source.insert(pos + rlen, " ");
					pos++;
				}
				pos += rlen;
				goto endloop;
			}
		}

		//
		// Skip over single quote
		//
		if (source[pos] == '\'')
		{
			//
			// Search for closing quote
			//
			int fpos = source.find('\'', pos + 1);
			if (fpos != source.npos)
			{
				pos = fpos + 1;
				goto endloop;
			}

			//
			// Didn't find one? Add it in!
			//
			source += '\'';
			return;
		}

		//
		// Skip over double quote
		//
		if (source[pos] == '"')
		{
			//
			// Search for closing quote
			//
			int fpos = source.find('"', pos + 1);
			if (fpos != source.npos)
			{
				pos = fpos + 1;
				goto endloop;
			}

			//
			// Didn't find one? Add it in!
			//
			source += '"';
			return;
		}

		//
		// Point to next character
		//
		pos++;
endloop:;
	}
}

////////////////////////////////////////////////////////////////////
/*! Add spaces where necessary

This routine attempts to insert whitespace between common
basic commands.
*/
static void MungWhitespaceLineOld(
	std::string &sloop		//!< One source line
)
{
	for (int rloop = 0; reserved[rloop] != 0; rloop++)
	{
		int fpos;
		while ((fpos = sloop.find(reserved[rloop], fpos)) !=
			 sloop.npos)
		{
			if (IsQuoted(sloop, fpos))
			{
				//
				// Ignore in quoted string
				//
				fpos++;
			}
			else
			{
				//
				// Put spaces before string
				//
				if ((fpos > 0) &&
					!isspace(sloop[fpos - 1]) &&
					!isbpunct(sloop[fpos - 1]))
				{
					sloop.insert(fpos, " ");
					fpos++;
				}

				//
				// Put spaces after string
				//
				int rlen = strlen(reserved[rloop]);
				if ((fpos > 0) &&
					!isspace(sloop[fpos + rlen]) &&
					!isbpunct(sloop[fpos + rlen]))
				{
					sloop.insert(fpos  + rlen, " ");
				}
				fpos += rlen;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////
//! Add continuation markers where necessary
//
static void MungContinuation(
	std::vector<std::string> &source	//!< Entire source code
)
{
	std::vector<std::string>::iterator previous;
	int have_previous = 0;
	for (std::vector<std::string>::iterator loop = source.begin();
		loop != source.end(); loop++)
	{
		//
		// Check for a line number on the current line
		//
		if ((have_previous != 0) && (isdigit((*loop)[0]) == 0))
		{
			(*previous) += " &";
		}

		previous = loop;
		have_previous = 1;
	}
}

////////////////////////////////////////////////////////////////////
//! Output the final result
//
static void Writefile(
	std::ostream *OutFile,			//!< Output stream
	std::vector<std::string> &source	//!< Entire source code
)
{
	//
	// Write out all of the strings that we have sucked in
	//
	for (std::vector<std::string>::iterator loop = source.begin();
		loop != source.end(); loop++)
	{
		*OutFile << *loop << std::endl;
	}
}

////////////////////////////////////////////////////////////////////
//! Is this character inside of quotes?
//
static int IsQuoted(
	const std::string& source,	//!< One source line
	int pos				//!< Position to check
)
{
	int flag = 0;

	//
	// For the moment, assume it is not
	//
	for (int loop = 0; loop < source.length(); loop++)
	{
		int ch = source[loop];

		switch(ch)
		{
		//
		// Handle quote characters
		//
		case '"':
		case '\'':
			if (flag == 0)
			{
				flag = ch;
			}
			else
			{
				if (flag == ch)
				{
					flag = 0;
				}
			}
			break;
		}

		//
		// Have we reached the right position?
		//
		if (--pos <= 0)
		{
			return flag;
		}
	}

	//
	// Hit the end of the string before reaching the limiting position
	//
	return flag;
}

/*!  Punctuation in basic that does not need whitespace arount it.

This function exists (instead of using punct) because I need
to have the program add spaces around decimal points.
*/
static int isbpunct(
	char source	//!< Single character to check
)
{
	switch(source)
	{
	case '"':
	case '\'':
	case ';':
	case '+':
	case '-':
	case '/':
	case '*':
		return 1;
	default:
		return 0;
	}
}

