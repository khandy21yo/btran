/*
 * Parse.l - basic lexical analyzer
 *
 *	Vax/Basic to C++ lexical analyzer
 *
 * Description:
 *
 *	This procedure is used to break Vax/Basic source code
 *	into individual tokens, when are passed on to 'parse.y'.
 *
 */
%x incl
%x xremark

%{
#include <stdio.h>
#include <string>
#include <assert.h>
#include <ctype.h>
#include "vartype.h"
#include "basic.h"
#include "variable.h"
#include "varlist.h"
#include "nodes.h"
#include "parse.hh"

int xline = 1;
int include_stack_pointer = 0;
Node* CommentList;

static const int MAX_INCLUDE_DEPTH = 10;
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
static int include_line[MAX_INCLUDE_DEPTH];
char* include_name[MAX_INCLUDE_DEPTH] = {(char*)NULL};
static void StartInclude(char* filename);
static char *mangle_string(char *Text);
static void mangle_string_value(char *ThisPart, int Base, char Type);
static void my_fatal_error(const char* msg);

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define YY_FATAL_ERROR(msg) my_fatal_error(msg)

static inline int SetReturn(int x)
{
	yylval = new Node(x, yytext, include_stack_pointer, xline);
	return(x);
}

static inline int SetStringReturn(int x)
{
//std::std::cerr << "Test1: Mangle '" << yytext << "'" << std::endl;
	char* MangleValue = mangle_string(yytext);
	yylval = new Node(x, MangleValue, include_stack_pointer, xline);
	delete MangleValue;
	return(x);
}

static inline int SetNullReturn(int x)
{
	yylval = new Node(x, "", include_stack_pointer, xline);
	return(x);
}

static inline void SetComment(int x)
{
	yylval = new Node(x, yytext, include_stack_pointer, xline);

	if (CommentList == NULL)
	{
		CommentList = yylval;
	}
	else
	{
		CommentList->DownLink(yylval);
	}
}

static inline void CountLine()
{
	xline++;
}

%}


%%
	/*
	 * White-space characters
	 */
[\r\0]			;
[\n\f]			{ CountLine(); return('\n'); }
"&"[\n\f]		CountLine();
[ \t]+			;
[DBOX]?"\""([0-9A-Fa-f]*)"\""[BWLQFDGHPSTX]	return SetStringReturn(BAS_V_INTEGER);
"A\""([^\"]*)"\""[BWLQFDGHPSTX]	return SetStringReturn(BAS_V_INTEGER);
[DBOX]?"'"([0-9A-Fa-f]*)"'"[BWLQFDGHPSTX]	return SetStringReturn(BAS_V_INTEGER);
"A'"([^\']*)"'"[BWLQFDGHPSTX]	return SetStringReturn(BAS_V_INTEGER);
[DBOX]?"\""([0-9A-Fa-f]*)"\"C"	return SetStringReturn(BAS_V_TEXTSTRING);
"A\""([^\"]*)"\"C"	return SetStringReturn(BAS_V_TEXTSTRING);
[DBOX]?"'"([0-9A-Fa-f]*)"'C"		return SetStringReturn(BAS_V_TEXTSTRING);
"A'"([^\']*)"'C"		return SetStringReturn(BAS_V_TEXTSTRING);
"\""([^\"]*)"\""	return SetStringReturn(BAS_V_TEXTSTRING);
"'"([^']*)"'"		return SetStringReturn(BAS_V_TEXTSTRING);

"!"[^\n\f\!]*"!"	{ SetComment(BAS_S_REMARK); }
"!"[^\n\f\!]*/"&"[\n\f]	{ SetComment(BAS_S_REMARK); }
"!"[^\n\f\!]*/[\n\f]	{ SetComment(BAS_S_REMARK); }
"REM"			{ BEGIN(xremark); }

<xremark>[^\n\f]*/"&"[\n\f]	{ SetComment(BAS_S_REMARK); BEGIN(INITIAL); }
<xremark>[^\n\f]*/[\n\f]	{ SetComment(BAS_S_REMARK); BEGIN(INITIAL); }

"::"		return SetNullReturn(BAS_X_STRREF);

	/*
	 * Single character operations
	 */
"(" |
")" |
"*" |
"/" |
"\\" |
"+" |
"-" |
":" |
";" |
"," |
"#" |
"^" |
"=" |
"<" |
">"		return SetReturn(yytext[0]);

"=="		return SetReturn(BAS_X_EQ);

"["		return SetReturn('(');
"]"		return SetReturn(')');

"**"		return SetNullReturn('^');
"><" |
"<>"		return SetNullReturn(BAS_X_NEQ);
">=" |
"=>"		return SetNullReturn(BAS_X_GE);
"<=" |
"=<"		return SetNullReturn(BAS_X_LE);

	/*
	 * Statements (usually)
	 */
"END"[ \t]*"IF"	return SetNullReturn(BAS_N_ENDIF);
"RANDOM"	{ NeedTimeH = 1; NeedBasicFun = 1; return SetNullReturn(BAS_S_RANDOM); }
"RANDOMIZE"	{ NeedTimeH = 1; NeedBasicFun = 1; return SetNullReturn(BAS_S_RANDOM); }

	/*
	 * functions with multiple determinates
	 */
"CVT%$"		return SetReturn(BAS_V_NAME);
"CVT$%"		return SetReturn(BAS_V_NAME);
"CVT$$"		return SetReturn(BAS_V_NAME);

	/*
	 * Percent Sign Commands
	 */
"%INCLUDE"	{ BEGIN(incl); return SetReturn(BAS_P_INCLUDE); }
"%FROM"		return SetReturn(BAS_P_FROM);
"%CDD"		return SetReturn(BAS_P_CDD);
"%PAGE"		;
"%TITLE"	return SetReturn(BAS_P_TITLE);
"%SBTTL"	return SetReturn(BAS_P_SBTTL);
"%IDENT"	return SetReturn(BAS_P_IDENT);
"%LIBRARY"	return SetReturn(BAS_P_LIBRARY);
"%ABORT"	return SetReturn(BAS_P_ABORT);
"%PRINT"	return SetReturn(BAS_P_PRINT);
"%LET"		return SetReturn(BAS_P_LET);

"%CROSS" |
"%NOCROSS" |
"%LIST" |
"%NOLIST"	SetComment(BAS_S_REMARK);

	/*
	 * These lines are used to trap an include file name
	 */
<incl>[ \t]+		;
<incl>[\n\f]		{ BEGIN(INITIAL); CountLine(); return('\n'); }
<incl>"%FROM"		{ BEGIN(INITIAL); return SetReturn(BAS_P_FROM); }
<incl>"\""([^\"]*)"\""	{ char* MangleValue = mangle_string(yytext);
				yylval = new Node(BAS_V_TEXTSTRING,
				MangleValue, include_stack_pointer);
				StartInclude(yytext); BEGIN(INITIAL);
				delete MangleValue;
				return BAS_V_TEXTSTRING; }
<incl>"'"([^']*)"'"	{ char* MangleValue = mangle_string(yytext);
				yylval = new Node(BAS_V_TEXTSTRING,
				MangleValue, include_stack_pointer);
				StartInclude(yytext); BEGIN(INITIAL);
				delete MangleValue;
				return BAS_V_TEXTSTRING; }

	/*
	 * Numbers, variables, etc.
	 */
[A-Z][A-Z0-9._$]*":"/[^:]		return SetReturn(BAS_V_LABEL);
(([0-9]+"."[0-9]*)|("."[0-9]+))([ED]"-"?[0-9]+)?	return SetReturn(BAS_V_FLOAT);
[0-9]+[ED]"-"?[0-9]+	return SetReturn(BAS_V_FLOAT);
[0-9]+		return SetReturn(BAS_V_INTEGER);
[0-9]+[%]	return SetReturn(BAS_V_INT);
[A-Z][A-Z0-9._$]*[%]?		{ std::string test = yytext; LowerCase(test);
				  int nt = node_type[test];
				  if (nt != 0) return SetReturn(nt);
				  else return SetReturn(BAS_V_NAME); }
%[A-Z][A-Z0-9._$]*[%]?		{ return SetReturn(BAS_P_NAME); }

	/*
	 * End of file (try to back up one include level)
	 */
<<EOF>>		{ if (--include_stack_pointer < 0)
			{ include_stack_pointer = 0; yyterminate(); }
			else
			{ yy_switch_to_buffer(
			include_stack[include_stack_pointer]);
			xline = include_line[include_stack_pointer];
			if (PositionDump){ std::cerr << "Leaving Include " <<
			include_name[include_stack_pointer] << std::endl; }
			delete include_name[include_stack_pointer];
			}
		}

%%

static void StartInclude(char* FileName)
{
	char UseName[64];	// File name to actually use
	FILE* NewChannel;	// File Open Info

	//
	// Strip off quotes (We KNOW that we have them)
	//
	assert(FileName[0] != 0);
	assert(FileName[1] != 0);
	strcpy(UseName, FileName + 1);
	UseName[strlen(UseName) - 1] = '\0';

	//
	// Can we handle the include?
	//
	if (include_stack_pointer >= MAX_INCLUDE_DEPTH)
	{
		std::cerr << "Includes Nested Too Deep (" << UseName << ")" << std::endl;
		return;
	}

	//
	// Try to open up the file
	//
//std::cerr << "Try: " << UseName << std::endl;
	NewChannel = fopen(UseName, "r");
	if (NewChannel == 0)
	{
		//
		// Try to lose any logicals on front of file name,
		//  and convert it to lower case. Also strip off
		//  logicals and directories.
		//
		//	This assumes that file names are of the
		//	format 'host::disk:[directory]name.ext'
		//
		char* cindex = strchr(UseName, ':');
		if (cindex != 0)
		{
			char newname[512];
			int loop;
			while (*cindex)
			{
				switch(*cindex)
				{
				case ']':
				case ':':
					loop = 0;
					break;
				default:
					newname[loop++] = tolower(*cindex);
					break;
				}
				cindex++;
			}
			newname[loop++] = '\0';
//std::cerr << "Try: " << newname << std::endl;
			NewChannel = fopen(newname, "r");

			if (NewChannel == 0)
			{
				for (std::list<std::string>::iterator inclist = include.begin();
					inclist != include.end() && NewChannel == 0;
					inclist++)
				{
					char tryname[512];
					strcpy(tryname, (*inclist).c_str());
					strcat(tryname, "/");
					strcat(tryname, newname);

//std::cerr << "Try: " << tryname << std::endl;
					NewChannel = fopen(tryname, "r");
				}
			}
		}
	}

	if (NewChannel == 0)
	{
		std::cerr << "Unable to open (" << UseName << ")" << std::endl;
		return;
	}


	//
	// Switch over to new buffer
	//
	include_stack[include_stack_pointer] = YY_CURRENT_BUFFER;
	include_line[include_stack_pointer] = xline;
	include_name[include_stack_pointer] = new char[strlen(UseName)+1];
	strcpy(include_name[include_stack_pointer], UseName);
	xline = 1;
	include_stack_pointer++;

	yy_switch_to_buffer(yy_create_buffer(NewChannel, YY_BUF_SIZE));
	if (PositionDump) { std::cerr << "Including (" << UseName << ")" << std::endl;};
}

/*******************************************************************************

Local Function:

	mangle_string()

Title:

	Translates a quoted string from Basic format to C++ format.

Description:

	This function processes a quoted string, changing the
	format from the way Basic does it to the way C++ does
	it.

Bugs:


*******************************************************************************/

static char *mangle_string(char *Text)
{
	char *ThisPart;
	char *ThisPtr;
	char qmark;
	int Base = 10;

	assert(Text != NULL);

	//
	// Create area to work in
	// (Probibly more than we will need)
	//
	ThisPart = new char[max(14, strlen(Text) * 2)];
	ThisPtr = ThisPart;

	//
	// Handle any possible bases on front of string
	//
	switch (*Text)
	{
	case 'd':
	case 'D':
		Base = 10;
		Text++;
		break;
	case 'b':
	case 'B':
		Base = 2;
		Text++;
		break;
	case 'o':
	case 'O':
		Base = 8;
		Text++;
		break;
	case 'x':
	case 'X':
		Base = 16;
		Text++;
		break;
	case 'a':
	case 'A':
		Base = 256;
		Text++;
		break;
	}

	//
	// Determine quote character used (Better start with a quote
	// or were in deep ****)
	//
	qmark = '\0';

	//
	// Process all the characters in original string
	//
	while (*Text)
	{
		switch (*Text)
		{
		//
		// Slash must be doubled
		//
		case '\\':
			*ThisPtr++ = '\\';
			*ThisPtr++ = '\\';
			break;

		//
		// Change (audible bell) character to (\a)
		//
		case '\a':
			*ThisPtr++ = '\\';
			*ThisPtr++ = 'a';
			break;

		//
		// Change (backspace) character to (\b)
		//
		case '\b':
			*ThisPtr++ = '\\';
			*ThisPtr++ = 'b';
			break;

		//
		// Change (tab) character to (\t)
		//
		case '\t':
			*ThisPtr++ = '\\';
			*ThisPtr++ = 't';
			break;

		//
		// Change (vertical tab) character to (\v)
		//
		case '\v':
			*ThisPtr++ = '\\';
			*ThisPtr++ = 'v';
			break;

		//
		// Single quote mark
		//
		case '\'':
			switch(qmark)
			{
			case '\'':
				qmark = '\0';
				*ThisPtr++ = '"';
				break;
			case '\0':
				qmark = '\'';
				*ThisPtr++ = '"';
				break;
			default:
				*ThisPtr++ = '\'';
				break;
			}
			break;

		//
		// Double Quote Mark
		//
		case '"':
			switch(qmark)
			{
			case '"':
				qmark = '\0';
				*ThisPtr++ = '"';
				break;
			case '\0':
				qmark = '"';
				*ThisPtr++ = '"';
				break;
			default:
				*ThisPtr++ = '\\';
				*ThisPtr++ = '"';
				break;
			}
			break;

		case 'B':
		case 'b':
		case 'W':
		case 'w':
		case 'L':
		case 'l':
		case 'f':
		case 'F':
		case 'd':
		case 'D':
		case 'g':
		case 'G':
		case 'h':
		case 'H':
		case 'c':
		case 'C':
		case 'q':
		case 'Q':
		case 's':
		case 'S':
		case 't':
		case 'T':
		case 'x':
		case 'X':
			if (qmark == '\0')
			{
				*ThisPtr = '\0';
				mangle_string_value(ThisPart, Base, *Text);
				ThisPtr = ThisPart + strlen(ThisPart);
			}
			else
			{
				*ThisPtr++ = *Text;
			}
			break;

		default:
			*ThisPtr++ = *Text;
			break;
		}

		Text++;
	}

	//
	// Return result
	//
	*ThisPtr = '\0';
	return (ThisPart);
}

/*******************************************************************************

Local Function:

	mangle_string_value()

Title:

	Prints out a number in a given base.

Description:

	This function processes a quoted string

Bugs:


*******************************************************************************/

static void mangle_string_value(char *ThisPart, int Base, char Type)
{
	const char *bascode;
	int len;
	char worktext[64];
	int StartChar = 1;


//std::cerr << "Test: '" << ThisPart << "' base " << Base << ", type " << Type << std::endl;
	assert(ThisPart != NULL);

	//
	// Set up format for outputing of this number
	//
	switch(Base)
	{
	case 2:
		bascode = "0b%s";
		break;
	case 8:
		bascode = "0%s";
		break;
	case 16:
		bascode = "x%s";
		break;
	case 256:
		bascode = "'%s'";
		break;
	default: // 10
		bascode = "%s";
		break;
	}

	//
	// Lose final quote mark
	//
	len = strlen(ThisPart);
	if (len > 0)
	{
		ThisPart[len-1] = '\0';
	}

	/*
	 * Skip over leading zero's
	 */
	while(ThisPart[StartChar] == '0')
	{
		StartChar++;
	}
	if ((isdigit(ThisPart[StartChar]) == 0) && (StartChar != 1))
	{
		StartChar--;
	}

	switch(Type)
	{
	case 'B':
	case 'b':
	case 'W':
	case 'w':
		sprintf(worktext, bascode, ThisPart+StartChar);
		break;

	case 'L':
	case 'l':
		sprintf(worktext, bascode, ThisPart+StartChar);
		strcat(worktext, "L");
		break;

	case 'C':
	case 'c':
		if (Base == 10)
		{
			long value = atoi(ThisPart + 1);
			if (value == 0)
			{
				strcpy(worktext, "std::string('\\0')");
				break;
			}
		}
		worktext[0] = '"';
		worktext[1] = '\\';
		if (Base == 10)
		{
			long value = atoi(ThisPart + 1);
			sprintf(worktext + 2, "x%x", value);
		}
		else
		{
			sprintf(worktext + 2, bascode, ThisPart + 1);
		}
		strcat(worktext, "\"");
		break;

	default:
		strcpy(worktext, ThisPart + 1);
		break;

	}

	strcpy(ThisPart, worktext);
}

//
// Fatal error handler, which reports the failed line.
//
//	This may break in future versions of flex. If so,
//	you can just remove all references to it and let flex use the
//	default error handler.
//
static void my_fatal_error(const char* msg)
{
	std::cerr << msg << " at line " << xline;
	if (include_stack_pointer > 0)
	{
		std::cerr << " in " << include_name[include_stack_pointer - 1];
	}
	std::cerr << std::endl;
	exit(YY_EXIT_FAILURE);
}
