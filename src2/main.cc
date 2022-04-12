/**\file main.cc
 * \brief Basic Translater Main Function
 *
 * This is the main driver function for the basic translater.
 *
 * Algorithm:
 *
 *	This translater operates in steps on the source code:
 *	<< This sequence has been changed somewhat >>
 *
 *	1. Rough translation using LEX and YACC to create a tree out
 *	   of the entire program. This tree only approximates the actual
 *	   program sequence.
 *
 *	2. Scan through tree, removing out initilization code, and making
 *	   tables of line numbers and variables used in the program.
 *
 *	3. Write out the variable definition section of the output.
 *
 *	4. Modify the tree to actual execution format, converting
 *	   if-then-else, for-next, ... into a better node format.
 *
 *	5. Lose line numbers and labels that are not used.
 *
 *	6. Pull subroutines out into seperate units.
 *
 *	7. Optimize tree. See if we can replace statements with
 *		auto-increment/decrement, ...
 *
 *	8. Write out code section of program.
 *
 *\bug Requires the source code to be formatted into a more strict
 *	format than the actual basic compiler likes.
 */

//
// System Include Files
//
#include <iostream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <map>
#include <list>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <unistd.h>
extern "C"
{
#include <getopt.h>
}

//
// Project Include Files
//
#include "vartype.h"
#include "basic.h"
#include "variable.h"
#include "varlist.h"
#include "nodes.h"
#include "parse.hh"

//
// Global Variables
//
VariableList *Variables;
std::map<std::string, int> node_type;
std::list<std::string> include;

int CompileFlag;

int KeepAllLines = false;
int NeedIostreamH = 0;
int NeedStringH = 0;
int NeedMathH = 0;
int NeedErrorH = 0;
int NeedStdlibH = 0;
int NeedRFA = 0;
int NeedBasicFun = 0;
int NeedTimeH = 0;
int NeedDataList = 0;
int NeedPuse = 0;
int NeedChannel = 0;
int NeedUnistd = 0;
int NeedVirtual = 0;

int IntegerType = VARTYPE_LONG;
VARTYPE RealType = VARTYPE_DOUBLE;
VARTYPE DefaultType = VARTYPE_REAL;

int DebugDump = false;
int DebugDumpOne = false;
int VariableDump = false;
int PositionDump = false;


//
// Module Function Prototypes
//
static void WriteHeader(std::ostream& os, const char* Name);
static void initialize_node_type();

//
// Module Variables
//
std::ostream *OutFile;		// Only global because YACC code
				// can't pass this easily to DoProgram

//! Option list for getopt processing
static const char* OptionList = "t:T:vVcCpPo:O:hHliI:";

#ifdef USE_LONGOPT
//! Option list for getopt_long processing
static const struct option OptionListLong[] =
{
	{"trace", 1, 0, 't'},
	{"varlist", 0, 0, 'v'},
	{"compile", 0, 0, 'c'},
	{"position", 0, 0, 'p'},
	{"output", 1, 0, 'o'},
	{"help", 0, 0, 'h'},
	{"lines", 0, 0, 'l'},
	{"integer", 0, 0, 'i'},
	{"include", 1, 0, 'I'},
	{0, 0, 0, 0}
};
#endif

/**
 * \brief Main driver function.
 *
 * This is the overall controlling function of the translater.
 * It scans the command line parametes, initializes the enviornment,
 * and starts up the procesing for each file on the command line.
 */
int main(int argc, char *argv[])
{
	//
	// Local Variables
	//
	int DidOneFlag = 0;
	int OptionSelected;
	int OptionIndex;
	int TestValue;

	//
	// Initialize variables
	//
	OutFile = &std::cout;
	CompileFlag = 0;
	KeepAllLines = false;

	initialize_node_type();

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
		case 't':
		case 'T':
			//
			// Tracing
			//
			TestValue = optarg[0] - '0';
			if (TestValue & 1)
			{
				DebugDump = true;
			}
			if (TestValue & 2)
			{
				DebugDumpOne = true;
			}
			break;

		case 'v':
		case 'V':
			//
			// Variable dump(s)
			//
			VariableDump = true;
			break;

		case 'o':
		case 'O':
			//
			// Output File
			//
			if (OutFile != &std::cout)
			{
				delete OutFile;
				OutFile = (std::ostream*)NULL;
			}

			OutFile = new std::ofstream(optarg);
			if (OutFile == (std::ostream*)NULL)
			{
				std::cerr << "Unable to open " << optarg <<
					"for output" << std::endl;
				exit(1);
			}

			break;

		case 'h':
		case 'H':
			DidOneFlag = 1;
			std::cerr << "Usage: basic [-h] [-t<n>] [-v] [-c] [-p] [-l] "
				"[-I <path>] [-o <file>] <source>" << std::endl;
			std::cerr << "   or: basic [--help] [--trace <n>] [--varlist] " << std::endl <<
			"       [--include <path>] [--compile] [--position] [--lines] [--output <file>] <source>" << std::endl;

			break;

		case 'c':
		case 'C':
			CompileFlag = 1;
			break;

		case 'p':
		case 'P':
			PositionDump = true;
			break;

		case 'l':
		case 'L':
			KeepAllLines = true;
			break;

		case 'i':
			DefaultType = VARTYPE_LONG;
			break;

		case 'I':
			include.push_back(optarg);
			break;

		default:
			std::cerr << "%Error on command line '-h' for help" << std::endl;
			exit(EXIT_FAILURE);
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

		//
		// Reset line counter
		//
		xline = 1;
		include_stack_pointer = 0;

		//
		// Open up source file
		//
		if ((yyin = fopen(argv[optind], "r")) == 0)
		{
			std::cerr << "Unable to open input file " <<
				argv[optind] << std::endl;
			exit(1);
		}

		WriteHeader(*OutFile, argv[optind]);

		//
		// Step 1. Parse the program. YACC will call the
		//  rest of the program if it scans correctly.
		//
		if (PositionDump)
		{
			std::cerr << "Parsing." << std::endl;
		}

		Variables = new VariableList;

		//
		// Default variable sizes
		//
		IntegerType = VARTYPE_LONG;
		RealType = VARTYPE_DOUBLE;

		if (yyparse() != 0)
		{
			std::cerr << "%Failure during parse" << std::endl;
			delete Variables;
			fclose(yyin);
			exit(1);
		}

		//
		// Close the input file
		//
		fclose(yyin);
		delete Variables;

		optind++;
	}

	//
	// If we haven't seen any file names, then get input
	// from the users terminal
	//
	if (DidOneFlag == 0)
	{
		//
		// Reset line counter
		//
		xline = 1;
		include_stack_pointer = 0;

		//
		// Read from standard input if we haven't handled
		// any input files yet.
		//
		yyin = stdin;

		WriteHeader(*OutFile, "Standard Input");

		if (PositionDump)
		{
			std::cerr << "Parsing." << std::endl;
		}

		Variables = new VariableList;

		if (yyparse() != 0)
		{
			std::cerr << "%Failure during parse" << std::endl;
			delete Variables;
			exit(1);
		}

		//
		// Close the input file
		//
		delete Variables;
	}

	if (OutFile != &std::cout)
	{
		delete OutFile;
	}

	return 0;
}

/**
 * \brief initialize node type table
 *
 * Fills in the node_type map, which is used in the lexer.
 * This is a simple map used to map basic keywords to node types.
 * This table is used to simplify the lexer, and greatly reduces the size
 * of the program.
 *
 * \note This list should only contain words reserved in basic
 */
static void initialize_node_type(void)
{
	node_type["access"] = BAS_S_ACCESS;
	node_type["allow"] = BAS_S_ALLOW;
	node_type["alternate"] = BAS_S_ALTERNATE;
	node_type["and"] = BAS_S_AND;
	node_type["any"] = BAS_S_ANY;
	node_type["append"] = BAS_S_APPEND;
	node_type["as"] = BAS_S_AS;
	node_type["ascending"] = BAS_S_ASCENDING;
	node_type["back"] = BAS_S_BACK;
	node_type["bel"] = BAS_V_PREDEF;
	node_type["block"] = BAS_S_BLOCK;
	node_type["blocksize"] = BAS_S_BLOCKSIZE;
	node_type["bs"] = BAS_V_PREDEF;
	node_type["bucketsize"] = BAS_S_BUCKETSIZE;
	node_type["buffer"] = BAS_S_BUFFER;
	node_type["by"] = BAS_S_BY;
	node_type["byte"] = BAS_S_BYTE;
	node_type["call"] = BAS_S_CALL;
	node_type["cause"] = BAS_S_CAUSE;
	node_type["case"] = BAS_S_CASE;
	node_type["chain"] = BAS_S_CHAIN;
	node_type["change"] = BAS_S_CHANGE;
	node_type["changes"] = BAS_S_CHANGES;
	node_type["close"] = BAS_S_CLOSE;
	node_type["clustersize"] = BAS_S_CLUSTERSIZE;
	node_type["com"] = BAS_S_COMMON;
	node_type["common"] = BAS_S_COMMON;
	node_type["con"] = BAS_S_CON;
	node_type["connect"] = BAS_S_CONNECT;
	node_type["constant"] = BAS_S_CONSTANT;
	node_type["contiguous"] = BAS_S_CONTIGUOUS;
	node_type["continue"] = BAS_S_CONTINUE;
	node_type["count"] = BAS_S_COUNT;
	node_type["cr"] = BAS_V_PREDEF;
	node_type["data"] = BAS_S_DATA;
	node_type["decimal"] = BAS_S_DECIMAL;
	node_type["declare"] = BAS_S_DECLARE;
	node_type["def"] = BAS_S_DEF;
	node_type["defaultname"] = BAS_S_DEFAULTNAME;
	node_type["del"] = BAS_V_PREDEF;
	node_type["delete"] = BAS_S_DELETE;
	node_type["desc"] = BAS_S_DESC;
	node_type["descending"] = BAS_S_DESCENDING;
	node_type["dim"] = BAS_S_DIM;
	node_type["dimension"] = BAS_S_DIM;
	node_type["double"] = BAS_S_DOUBLE;
	node_type["duplicates"] = BAS_S_DUPLICATES;
	node_type["else"] = BAS_S_ELSE;
	node_type["end"] = BAS_S_END;
	node_type["eq"] = BAS_S_EQ;
	node_type["eqv"] = BAS_S_EQV;
	node_type["error"] = BAS_S_ERROR;
	node_type["esc"] = BAS_V_PREDEF;
	node_type["exit"] = BAS_S_EXIT;
	node_type["explicit"] = BAS_S_EXPLICIT;
	node_type["extend"] = BAS_S_EXTEND;
	node_type["extendsize"] = BAS_S_EXTENDSIZE;
	node_type["external"] = BAS_S_EXTERNAL;
	node_type["explicit"] = BAS_S_EXPLICIT;
	node_type["ff"] = BAS_V_PREDEF;
	node_type["field"] = BAS_S_FIELD;
	node_type["file"] = BAS_S_FILE;
	node_type["filesize"] = BAS_S_FILESIZE;
	node_type["find"] = BAS_S_FIND;
	node_type["fixed"] = BAS_S_FIXED;
	node_type["fnend"] = BAS_S_FNEND;
	node_type["fnexit"] = BAS_S_FNEXIT;
	node_type["for"] = BAS_S_FOR;
	node_type["fortran"] = BAS_S_FORTRAN;
	node_type["free"] = BAS_S_FREE;
	node_type["from"] = BAS_S_FROM;
	node_type["function"] = BAS_S_FUNCTION;
	node_type["functionend"] = BAS_S_FUNCTIONEND;
	node_type["functionexit"] = BAS_S_FUNCTIONEXIT;
	node_type["ge"] = BAS_S_GE;
	node_type["get"] = BAS_S_GET;
	node_type["gfloat"] = BAS_S_GFLOAT;
	node_type["sfloat"] = BAS_S_GFLOAT;
	node_type["tfloat"] = BAS_S_GFLOAT;
	node_type["xfloat"] = BAS_S_GFLOAT;
	node_type["dfloat"] = BAS_S_GFLOAT;
	node_type["go"] = BAS_S_GO;
	node_type["gosub"] = BAS_S_GOSUB;
	node_type["goto"] = BAS_S_GOTO;
	node_type["gt"] = BAS_S_GT;
	node_type["handler"] = BAS_S_HANDLER;
	node_type["hfloat"] = BAS_S_HFLOAT;
	node_type["ht"] = BAS_V_PREDEF;
	node_type["idn"] = BAS_S_IDN;
	node_type["if"] = BAS_S_IF;
	node_type["imp"] = BAS_S_IMP;
	node_type["in"] = BAS_S_IN;
	node_type["inv"] = BAS_S_INV;
	node_type["indexed"] = BAS_S_INDEXED;
	node_type["input"] = BAS_S_INPUT;
	node_type["integer"] = BAS_S_INTEGER;
	node_type["iterate"] = BAS_S_ITERATE;
	node_type["key"] = BAS_S_KEY;
	node_type["kill"] = BAS_S_KILL;
	node_type["let"] = BAS_S_LET;
	node_type["lf"] = BAS_V_PREDEF;
	node_type["line"] = BAS_S_LINE;
	node_type["linput"] = BAS_S_LINPUT;
	node_type["list"] = BAS_S_LIST;
	node_type["long"] = BAS_S_LONG;
	node_type["basic$quadword"] = BAS_S_LONG;
	node_type["quad"] = BAS_S_LONG;
	node_type["lset"] = BAS_S_LSET;
	node_type["map"] = BAS_S_MAP;
	node_type["margin"] = BAS_S_MARGIN;
	node_type["mat"] = BAS_S_MAT;
	node_type["mod"] = BAS_S_MOD;
	node_type["mode"] = BAS_S_MODE;
	node_type["modify"] = BAS_S_MODIFY;
	node_type["move"] = BAS_S_MOVE;
	node_type["name"] = BAS_S_NAME;
	node_type["next"] = BAS_S_NEXT;
	node_type["nochanges"] = BAS_S_NOCHANGES;
	node_type["no"] = BAS_S_NO;
	node_type["noduplicates"] = BAS_S_NODUPLICATES;
	node_type["none"] = BAS_S_NONE;
	node_type["nospan"] = BAS_S_NOSPAN;
	node_type["not"] = BAS_S_NOT;
	node_type["nx"] = BAS_S_GT;
	node_type["nxeq"] = BAS_S_GE;
	node_type["on"] = BAS_S_ON;
	node_type["onerror"] = BAS_N_ONERROR;
	node_type["open"] = BAS_S_OPEN;
	node_type["option"] = BAS_S_OPTION;
	node_type["or"] = BAS_S_OR;
	node_type["otherwise"] = BAS_S_OTHERWISE;
	node_type["output"] = BAS_S_OUTPUT;
	node_type["organization"] = BAS_S_ORGANIZATION;
	node_type["pi"] = BAS_V_PREDEF;
	node_type["primary"] = BAS_S_PRIMARY;
	node_type["print"] = BAS_S_PRINT;
	node_type["program"] = BAS_S_PROGRAM;
	node_type["prompt"] = BAS_S_PROMPT;
	node_type["put"] = BAS_S_PUT;
	node_type["read"] = BAS_S_READ;
	node_type["real"] = BAS_S_REAL;
	node_type["record"] = BAS_S_RECORD;
	node_type["recordtype"] = BAS_S_RECORDTYPE;
	node_type["recordsize"] = BAS_S_RECORDSIZE;
	node_type["ref"] = BAS_S_REF;
	node_type["regardless"] = BAS_S_REGARDLESS;
	node_type["relative"] = BAS_S_RELATIVE;
	node_type["reset"] = BAS_S_RESET;
	node_type["restore"] = BAS_S_RESTORE;
	node_type["resume"] = BAS_S_RESUME;
	node_type["return"] = BAS_S_RETURN;
	node_type["retry"] = BAS_S_RETRY;
	node_type["rfa"] = BAS_S_RFA;
	node_type["rset"] = BAS_S_RSET;
	node_type["scale"] = BAS_S_SCALE;
	node_type["scratch"] = BAS_S_SCRATCH;
	node_type["select"] = BAS_S_SELECT;
	node_type["sequential"] = BAS_S_SEQUENTIAL;
	node_type["set"] = BAS_S_SET;
	node_type["si"] = BAS_V_PREDEF;
	node_type["single"] = BAS_S_SINGLE;
	node_type["size"] = BAS_S_SIZE;
	node_type["sleep"] = BAS_S_SLEEP;
	node_type["so"] = BAS_V_PREDEF;
	node_type["sp"] = BAS_V_PREDEF;
	node_type["span"] = BAS_S_SPAN;
	node_type["step"] = BAS_S_STEP;
	node_type["stop"] = BAS_S_STOP;
	node_type["stream"] = BAS_S_STREAM;
	node_type["string"] = BAS_S_STRING;
	node_type["sub"] = BAS_S_SUB;
	node_type["subend"] = BAS_S_SUBEND;
	node_type["subexit"] = BAS_S_SUBEXIT;
	node_type["temporary"] = BAS_S_TEMPORARY;
	node_type["then"] = BAS_S_THEN;
	node_type["to"] = BAS_S_TO;
	node_type["trn"] = BAS_S_TRN;
	node_type["type"] = BAS_S_TYPE;
	node_type["undefined"] = BAS_S_UNDEFINED;
	node_type["unless"] = BAS_S_UNLESS;
	node_type["unlock"] = BAS_S_UNLOCK;
	node_type["until"] = BAS_S_UNTIL;
	node_type["update"] = BAS_S_UPDATE;
	node_type["use"] = BAS_S_USE;
	node_type["using"] = BAS_S_USING;
	node_type["value"] = BAS_S_VALUE;
	node_type["variable"] = BAS_S_VARIABLE;
	node_type["variant"] = BAS_S_VARIANT;
	node_type["virtual"] = BAS_S_VIRTUAL;
	node_type["vt"] = BAS_V_PREDEF;
	node_type["wait"] = BAS_S_WAIT;
	node_type["when"] = BAS_S_WHEN;
	node_type["while"] = BAS_S_WHILE;
	node_type["windowsize"] = BAS_S_WINDOWSIZE;
	node_type["word"] = BAS_S_WORD;
	node_type["write"] = BAS_S_WRITE;
	node_type["xor"] = BAS_S_XOR;
	node_type["zer"] = BAS_S_ZER;
}


/**
 * \brief Write Header To Output File
 * 
 * Writes out a nice little title at the start if an output
 * translation saying I'll take the blame for the lousy code
 * output.
 */
static void WriteHeader(
	std::ostream& os,		/**< Output stream */
	const char* Name		/**< File name */
)
{
#ifdef HAVE_STRFTIME
	char Text[64];
	time_t timer;
	struct tm *tblock;

	timer = time((time_t*)NULL);
	tblock = localtime(&timer);

	strftime(Text, sizeof(Text), "%A, %B %d, %Y at %X",
		tblock);
#endif

	os << "//" << std::endl;
	os << "// Source: " << Name << std::endl;
	os << "// Translated from Basic to C++ using btran" << std::endl;
#ifdef HAVE_STRFTIME
	os << "// on " << Text << std::endl;
#endif
	os << "//" << std::endl << std::endl;
}


/**
 * \brief Force string to be upper case.
 * 
 * Converts a mixed case STL string to upper case.
 * 
 * \note Since this has a loop, it will not inline very well with most compilers
 */
void UpperCase(
	std::string& TextValue		/**< string to be converted (in place) to upper case */
)
{
	//
	// Force string to upper case so it's easier to handle
	//
	for (std::string::iterator uploop = TextValue.begin();
		uploop < TextValue.end(); uploop++)
	{
		*uploop = toupper(*uploop);
	}
}

/**
 * \brief Force string to be lower case
 * 
 * Converts a mixed case STL string to lower case.
 * 
 * \note Since this has a loop, it will not inline very well with most compilers
 */
void LowerCase(
	std::string& TextValue		/**< string to be converted (in place) to lower case */
)
{
	//
	// Force string to lower case so it's easier to handle
	//
	for (std::string::iterator uploop = TextValue.begin();
		uploop < TextValue.end(); uploop++)
	{
		*uploop = tolower(*uploop);
	}
}
