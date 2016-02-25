/**
 * \file basic.h
 * \version 01.02.00
 * \brief Basic Translater Main Header File
 *
 *	This is the main header file for the basic translater.
 */

#ifndef _BASIC_H_
#define _BASIC_H_

//
// Optional stuff in compile
//
#define USE_LONGOPT	/**< \brief Use getopt_long instead of regular getopt */

//
// Include files
//
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <list>

//
// These functions are refered to by the local include files
//
void UpperCase(std::string&);
void LowerCase(std::string&);

//
// Forward declarations
//
class VariableList;
class Node;

//
// Format for transfering info between flex and yacc
//
#define YYSTYPE Node *		/**< \brief yacc node type */

//
// Required by Lex and Yacc
//
extern FILE *yyin;		/**< \brief Input channel for lexer */
extern int xline;		/**< \brief Line counter */
extern int yydebug;		/**< \brief Debug flag */
extern std::list<std::string> include;	/**< \brief List of paths to search for include files */

//
// External stuff in main.c
//
extern VariableList *Variables;	/**< \brief Variable table */
extern std::map<std::string, int> node_type;	//!< \brief ?? Node types ??

extern int CompileFlag;		/**< \brief Compile flags */

extern int KeepAllLines;	/**< \brief Retain all source lines */
extern int NeedIostreamH;	/**< \brief Do we need to include iostream.h into C++ */
extern int NeedStdlibH;		/**< \brief Do we need to include stdlib.h into C++ */
extern int NeedStringH;		/**< \brief Do we need to include string.h intp C++ */
extern int NeedMathH;		/**< \brief Do we need to include math.h into C++ */
extern int NeedErrorH;		/**< \brief Do we need to include error.h into C++ */
extern int NeedRFA;		/**< \brief Do we need to include rfa.h into C++ */
extern int NeedBasicFun;	/**< \brief Do we need to include basicfun.h into C++ */
extern int NeedTimeH;		/**< \brief Do we need to include time.h into C++ */
extern int NeedDataList;	/**< \brief Do we need to include datalist.h into C++ */
extern int NeedPuse;		/**< \brief Do we need to include puse.h into C++ */
extern int NeedChannel;		/**< \brief Do we need to include basicchannel into C++ */
extern int NeedUnistd;		/**< \brief Do we need to include unistd.h into C++ */
extern int NeedVirtual;		/**< \brief Do we need to include virtual.h into C++ */

extern int IntegerType;		/**< \brief What is the real INTEGER type (word, long) */
extern VARTYPE RealType;	/**< \brief What is the real REAL type (float, double) */
extern VARTYPE DefaultType;	/**< \brief What is the type when not specified as part of variable name (%,$) */

extern int DebugDump;		/**< \brief Dump out debugging information */
extern int DebugDumpOne;	/**< \brief Dump out additional debugging information */
extern int VariableDump;	/**< \brief Dump variable tables out */
extern int PositionDump;	/**< \brief Dump out names of stages as they start */

extern std::ostream* OutFile;	/**< \brief Output C++ channel */

void DoProgram(Node *Program);	/* Main interface for parse/lex stages */

//
// Parsing stuff
//
extern int include_stack_pointer;	/**< \brief Current nesting level for include fies */
extern Node* CommentList;		/**< \brief Stash of comments from inside of a statement being processed */
extern char* include_name[];		/**< \brief Stack of include files */

int yyparse();			// Hook into bison parser
int yyerror(char *s);		// Hook into bison error routines
int yylex( void );		// Hook into flex lexer

#ifdef __MSDOS__
extern "C"
{
	void* alloca(int);	// alloca (needed for bison/flex)
}
#endif

#endif
