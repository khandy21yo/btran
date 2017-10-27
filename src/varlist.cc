/**
 * \file varlist.cc
 * \brief Routines to handle variables.
 *
 * This file contains routines to handle the variable definitions
 * necessary for basic.
 */

/*
 * System Include Files
 */
#include <iostream>
#include <cstdio>
#include <string>
#include <cassert>
#include <cctype>
#include <cstring>

//
// Project Include Files
//
#include "vartype.h"
#include "basic.h"
#include "varlist.h"
#include "parse.hh"


/** Create base name given original name
 *
 * Create a base variable  name to work with based on the original
 * name used in the code to be converted.
 */
std::string genname(
	const std::string &basename	/**< Name to be converted */
)
{
	std::string tempname = basename;

	if (tempname[tempname.length()-1] == '(')
	{
		tempname = tempname.substr(0, tempname.length() - 1);
	}

	if ((tempname[tempname.length()-1] == '%') ||
		(tempname[tempname.length()-1] == '$'))
	{
		tempname = tempname.substr(0, tempname.length() - 1);
	}

	for (int j = 0; j < tempname.length(); j++)
	{
		tempname[j] = tolower(tempname[j]);
		if (tempname[j] == '.')
		{
			tempname[j] = '_';
		}
	}

	return tempname;
}

/**
 * \brief Constructor
 *
 * This routine creates a variable list.
 * 
 * \bug This function should not be made inline, because when that is
 * done it takes forever to compile.
 */
VariableList::VariableList()
{
	ListOfVariables newlist;

	insert(begin(), newlist);
	vcount = 1;

	//
	// Load in hard coded function list
	//
	InitFunctions();
}


/**
 * \brief Add a new block level to the table
 * 
 * Used when entering a new block (function/sub start, etc.)
 */
void VariableList::NewLevel(void)
{
	ListOfVariables newlist;

	insert(end(), newlist);
}

/**
 * \brief Drop a block level to the table
 * 
 * Used when leaving a program block.
 */
void VariableList::KillLevel(void)
{
//	erase(&back());
	erase(end() - 1);
}

/**
 * \brief Look for a variable in the table using it's basic name.
 * 
 * This routine will scan the variable table for the occurance of a
 * specified variable.
 */
VariableStruct *VariableList::Lookup
(
	std::string &VarName,		/**< Basic name to look up */
	const Node *ArrayRef,		/**< Array/function flag */
	int ScanTopOnly			/**< Scan Only the Topmost definition */
)
{
	ListOfVariables::iterator VarTable;
	std::string keyname = VarName;
	UpperCase(keyname);

	if ((ArrayRef != 0) || (keyname.substr(0,2) == "FN"))
	{
		keyname += "(";
	}

	//
	// Scan the list
	//
	if (ScanTopOnly != 0)
	{
		reverse_iterator loop = rbegin();
		VarTable = (*loop).find(keyname);
		if (VarTable != (*loop).end())
		{
			return &((*VarTable).second);
		}
	}
	else
	{
		for (reverse_iterator loop = rbegin();
			loop != rend(); loop++)
		{
			VarTable = (*loop).find(keyname);
			if (VarTable != (*loop).end())
			{
				return &((*VarTable).second);
			}
		}
	}

	//
	// Not found
	//
	return 0;
}

/**
 * \brief Look for a variable in the table using the C++ name.
 * 
 * This routine will scan the variable table for the occurance of a
 * specified variable.
 */
VariableStruct *VariableList::LookupCpp
(
	const std::string &VarName	/**< Variable name to look up */
)
{
	VariableStruct *VarTable;

	for (reverse_iterator loop = rbegin();
		loop != rend(); loop++)
	{
		VarTable = (*loop).LookupCpp(VarName);

		if (VarTable != 0)
		{
			break;
		}
	}

	return VarTable;
}

/**
 * \brief Look for a variable in the table using the C++ name.
 *
 * This routine will scan the variable table for the occurance of a
 * specified variable.
 */
VariableStruct *ListOfVariables::LookupCpp
(
	const std::string &VarName	/**< Name of variable to look up */
)
{
	//
	// Loop through all variables
	//
	for (iterator i = begin(); i != end(); i++)
	{
		//
		// Is this it?
		//
		if (((*i).second.CName.length() != 0) &&
			((*i).second.CName.compare(VarName) == 0))
		{
			return &((*i).second);
		}
	}

	//
	// Didn't find it
	//
	return 0;
}

/**
 * \brief Reserved word list
 *
 * This list represent names that are not available for one reason or
 * another to a C++ program for use as a variable name. It keeps
 * generated names from overlapping too badly with existing functions
 * and reserved words.
 *
 * \note Do not include constants that have been appended to the variable
 * list in the startup routines. That would be redundant, and a
 * duplication of effort.
 */
static const char* ReservedList[] =
{
	"argc",
	"argv",
	"atof",
	"atoi",
	"auto",
	"break",
	"case",
	"char",
	"cin",
	"class",
	"const",
	"continue",
	"cout",
	"default",
	"delete",
	"do",
	"double",
	"else",
	"extern",
	"far",
	"fclose",
	"float",
	"fopen",
	"for",
	"fprintf",
	"free",
	"goto",
	"if",
	"inline",
	"int",
	"long",
	"malloc",
	"namespace",
	"near",
	"new",
	"operator",
	"printf",
	"public",
	"private",
	"register",
	"return",
	"scanf",
	"short",
	"sprintf",
	"static",
	"strcmp",
	"struct",
	"switch",
	"union",
	"using",
	"virtual",
	"void",
	"volatile",
	"template",
	"throw",
	"typedef",
	"unsigned",
	"while",
	0
};

/**
 * \brief Look for a variable in the list of reserved names and the 
 * already used names using the C++ name.
 *
 * This routine will scan the variable table for the occurance of a
 * specified variable.
*/
int VariableList::LookupReserved
(
	const char *VarName	/**< Variable name to scan for */
)
{
	//
	// Look through reserved word list
	//
	for (int i = 0; ReservedList[i] != 0; i++)
	{
		if (strcmp(ReservedList[i] , VarName) == 0)
		{
			return true;
		}
	}

	//
	// Look through list of already used names
	//
	VariableStruct *VarTable = LookupCpp(VarName);

	if (VarTable != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * \brief Look for a variable in the list of reserved names and the 
 * already used names using the C++ name.
 *
 * This routine will scan the variable table for the occurance of a
 * specified variable.
 */
int VariableList::LookupReserved
(
	const std::string &VarName	/**< Variable name to look up */
)
{
	//
	// Look through reserved word list
	//
	for (int i = 0; ReservedList[i] != 0; i++)
	{
		if (VarName.compare(ReservedList[i]) == 0)
		{
			return true;
		}
	}

	//
	// Look through list of already used names
	//
	VariableStruct *VarTable = LookupCpp(VarName);

	if (VarTable != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * \brief Print out variable table.
 *
 * Prints out the variables stored in the variable table.
 *
 * \bug Ugly output.
 */
void VariableList::Dump(void)
{
	int Level = 0;

	for (iterator loop = begin();
		loop != end(); loop++)
	{
		//
		// Dump out global variables
		//
		std::cout << std::endl << "// Level " << Level++ << ":" << std::endl;
		(*loop).Dump();
	}

	std::cerr << std::endl;
}

/**
 * \brief Print out variable table.
 *
 * Prints out the variables stored in the variable table.
 *
 * \bug Ugly output.
 */
void ListOfVariables::Dump(void)
{
	//
	// Loop through all variables
	//
	for (iterator i = begin(); i != end(); i++)
	{
		//
		// Dump it out
		//
		std::cout << "//    Key: " << (*i).first << " ";
		(*i).second.Dump();
	}
}


/**
 * \brief Define the current node as a variable.
 *
 * This function tries to define the current node as a variable.
 * It is used to define those variables that are not defined
 * in some other manner by the user.
 */
void VariableList::Define(
	Node *ThisNode,		/**< Pointer to node */
	int GlobalFlag, 	/**< Global/local definition */
	int ThisOutput		/**< Output flags */
)
{
	//
	// Local Variables
	//
	VARTYPE Type;
	VariableStruct *ThisVar;
	VARCLASS ThisClass = VARCLASS_NONE;
	int ParCount = 0;
	int PreDefined = false;
	std::string TextName = ThisNode->GetText();

	UpperCase(TextName);

	//
	// If the name starts fn..., then it must be a function
	// (it's the law)
	//
	if (TextName.substr(0,2) == "FN")
	{
		ThisNode->Type = BAS_V_FUNCTION;
		PreDefined = true;
	}

	//
	// Is it already defined as a variable?
	//
	if ((ThisVar = Lookup(TextName, ThisNode->GetTree(0), ThisOutput)) != 0)
	{
		//
		// Force the node to be a function if that is what it is.
		//
		if (ThisVar->Class == VARCLASS_FUNC)
		{
			ThisNode->Type = BAS_V_FUNCTION;
		}
		return;
	}

	//
	// Check the last letter of the variable to determine what type
	// it ought to be.
	//
	Type = GuessVarType(TextName);

	//
	// Is this an array reference?
	//
	if (ThisNode->GetTree(0) != 0)
	{
		//
		// Assume if there are numerous parameters, that this
		// is an undocumented function call)
		//
		if (ThisNode->GetTree(0)->Type == BAS_N_LIST)
		{
			ThisNode->Type = BAS_V_FUNCTION;
		}
		else
		{
			ThisClass = VARCLASS_ARRAY;
		}
		ParCount = ThisNode->GetTree(0)->CountParam();
	}

	if ((ThisNode->Type == BAS_V_FUNCTION) ||
		(ThisNode->Type == BAS_S_MAINFUNCTION) ||
		(ThisNode->Type == BAS_S_FUNCTION))
	{
		ThisClass = VARCLASS_FUNC;
	}

	//
	// Add the variable to the list
	//
	VariableStruct NewVariable(
		TextName, Type, ThisClass,
		ThisOutput | PreDefined, ParCount);
	Append(NewVariable, GlobalFlag);
}


/**
 * \brief Add a variable to the table.
 *
 * This routine will add a variable to the end of the variable table.
 *
 * \bug Assumes variable is not already in the table.
 */
void VariableList::Append(
	const VariableStruct& Variable,	/**< Variable to add */
	int GlobalFlag			/**< Force to global level */
)
{
	std::string keyname(Variable.BasicName);

	if ((Variable.Class == VARCLASS_ARRAY) ||
		(Variable.Class == VARCLASS_FUNC))
	{
		keyname += "(";
	}

	//
	// Decide which table to stick it on
	//
	if (GlobalFlag)
	{
		front()[keyname] = Variable;
	}
	else
	{
		if (back().find(keyname) != back().end())
		{
			std::cerr << "Putting in duplicate variable " << keyname << std::endl;
		}
		back()[keyname] = Variable;
	}
}


/**
 * \brief Fix up variable names
 *
 * This function changes all of the variable names from
 * formats that BASIC likes, into those that C likes.
 *
 * Algorithm:
 *
 * 1. Convert those variables that can be to simply lower case
 *    versions of the upper case basic names.
 *
 * 2. For variables with trailing characters "%", try leaving
 *    the "%" off, and make sure there are no duplicates.
 *
 * 3. Any variables left over, convert to lower case, changing
 *    unallowed characters to underscores. Make sure that they
 *    don't duplicate any other name.
 *
 * 4. Additional to step 3, add a "Vn" to the variable to make
 *    unique.
 */
void VariableList::Fixup(void)
{
	for (reverse_iterator loop = rbegin();
		loop != rend(); loop++)
	{
		//
		// Do the various stages of fixup for global functions
		//
		Fixup1(&(*loop));
	}
}

/**
 * \brief Fix up variable names
 *
 *	Convert those variables that can be to simply lower case
 *	versions of the upper case basic names.
 *
 * \bug
 *	I would like to move this into the ListOfVariables class,
 *	but it needs to be able to search through the entire list
 *	of all the levels, so it is stuck here.
 */
void VariableList::Fixup1(
	ListOfVariables* Variable	/**< Table of all variables */
)
{
	//
	// Local Variables
	//
	std::string tempname;
	char number[6];

	//
	// Loop through all variables
	//
	for (ListOfVariables::iterator i = Variable->begin();
		i != Variable->end(); i++)
	{
		VariableStruct* VarList = &((*i).second);

		//
		// Don't re-convert, don't allow illegal basic characters.
		//
		if (VarList->CName.length() == 0)
		{
			if ((VarList->Type == VARTYPE_LABEL) &&
				(isdigit(VarList->BasicName[0])))
			{
				//
				// Lables are never reserved, so always
				// give them a specific value
				//
				VarList->CName = "L_" + genname(VarList->BasicName);
			}
			else
			{
				tempname = genname(VarList->BasicName);

				//
				// Don't assign if used by another variable
				//
				if (LookupReserved(tempname) == false)
				{
					VarList->CName = tempname;
				}
				else
				{
					tempname += "_V";
					sprintf(number, "%d", vcount++);
					tempname += number;

					VarList->CName = tempname;
				}
			}
		}
	}
}

/**
 * \brief Output any variables that have not yet been dumped.
 *
 *	This function will output any variables that have not been dumped
 *	by any previous routines.
 */
void ListOfVariables::OutputDef(
	std::ostream& os, 	/**< Output chnnel */
	int Level		/**< indent level */
)
{
	//
	// Loop through all variables
	//
	for (iterator i = begin(); i != end(); i++)
	{
		//
		// Output this variable
		//
		if ((*i).second.Output != true)
		{ 
			os << std::string(Level, '\t') <<
				(*i).second.OutputDef() << ";" << std::endl;
		}
	}
}

/**
 * \brief Initialize function names.
 *
 *	This function initializes several built in basic functions
 *	that have compatible C functions (same parameters).
 *
 * \note some of these are rewritten in nodes3.cc
 */
void VariableList::InitFunctions(void)
{
	InitOneFunction("ABS",		"fabs",			VARTYPE_DOUBLE);
	InitOneFunction("ABS%",		"abs",			VARTYPE_DOUBLE);
	InitOneFunction("ASC",		"basic::Ascii",		VARTYPE_LONG);
	InitOneFunction("ASCII",	"basic::Ascii",		VARTYPE_LONG);
	InitOneFunction("ATN",		"atan",			VARTYPE_DOUBLE);
	InitOneFunction("BUFSIZ",	"basic::Bufsiz",	VARTYPE_LONG);
	InitOneFunction("CCPOS",	"basic::Ccpos",		VARTYPE_LONG);
	InitOneFunction("CHR$",		"basic::Char",		VARTYPE_DYNSTR);
	InitOneFunction("COMP%",	"basic::Comp",		VARTYPE_LONG);
	InitOneFunction("COS",		"cos",			VARTYPE_DOUBLE);
	InitOneFunction("CTRLC",	"basic::ctrlc()",	VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("CVT$%",	"basic::cvtai",		VARTYPE_LONG);
	InitOneFunction("CVT%$",	"basic::cvtia",		VARTYPE_DYNSTR);
	InitOneFunction("CVT$F",	"basic::cvtaf",		VARTYPE_DOUBLE);
	InitOneFunction("CVTF$",	"basic::cvtfa",		VARTYPE_DYNSTR);
	InitOneFunction("CVT$$",	"basic::edit",		VARTYPE_DYNSTR);
	InitOneFunction("DATE$",	"basic::Qdate",		VARTYPE_DYNSTR);
	InitOneFunction("DET",		"basic::det()",		VARTYPE_DOUBLE, VARCLASS_NONE);
	InitOneFunction("DIF$",		"basic::Dif",		VARTYPE_DYNSTR);
	InitOneFunction("ECHO",		"basic::Echo",		VARTYPE_LONG);
	InitOneFunction("EDIT$",	"basic::edit",		VARTYPE_DYNSTR);
	InitOneFunction("ERR",		"Be.err",		VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("ERL",		"Be.erl",		VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("ERN$",		"Be.ern",		VARTYPE_DYNSTR, VARCLASS_NONE);
	InitOneFunction("ERT$",		"basic::ert",		VARTYPE_DYNSTR);
	InitOneFunction("EXP",		"basic::exp",		VARTYPE_DOUBLE);
	InitOneFunction("FIX",		"trunc",		VARTYPE_DOUBLE);
	InitOneFunction("FORMAT$",	"basic::Format",	VARTYPE_DYNSTR);
	InitOneFunction("FSP$",		"basic::Fsp",		VARTYPE_DYNSTR);
	InitOneFunction("FSS$",		"basic::Fss",		VARTYPE_DYNSTR);
	InitOneFunction("GETRFA",	"basic::Getrfa",	VARTYPE_RFA);
	InitOneFunction("INKEY$",	"basic::Inkey",		VARTYPE_DYNSTR);
	InitOneFunction("INSTR",	"basic::instr",		VARTYPE_LONG);
	InitOneFunction("INT",		"floor",		VARTYPE_DOUBLE);
	InitOneFunction("LEFT",		"basic::left",		VARTYPE_DYNSTR);
	InitOneFunction("LEFT$",	"basic::left",		VARTYPE_DYNSTR);
	InitOneFunction("LEN",		"basic::strlen",	VARTYPE_LONG);
	InitOneFunction("LOC",		"&",			VARTYPE_LONG);
	InitOneFunction("LOG",		"log",			VARTYPE_DOUBLE);
	InitOneFunction("LOG10",	"log10",		VARTYPE_DOUBLE);
	InitOneFunction("MAG",		"basic::Mag",		VARTYPE_DOUBLE);
	InitOneFunction("MAGTAPE",	"basic::Magtape",	VARTYPE_LONG);
	InitOneFunction("MAR",		"basic::Mar",		VARTYPE_LONG);
	InitOneFunction("MID",		"basic::mid",		VARTYPE_DYNSTR);
	InitOneFunction("MID$",		"basic::mid",		VARTYPE_DYNSTR);
	InitOneFunction("NOECHO",	"basic::NoEcho",	VARTYPE_LONG);
	InitOneFunction("NUM",		"basic::num()",		VARTYPE_DOUBLE, VARCLASS_NONE);
	InitOneFunction("NUM2",		"basic::num2()",	VARTYPE_DOUBLE, VARCLASS_NONE);
	InitOneFunction("NUM$",		"basic::Qnum",		VARTYPE_DYNSTR);
	InitOneFunction("NUM1$",	"std::to_string",	VARTYPE_DYNSTR);
	InitOneFunction("ONECHR",	"basic::OneChr",	VARTYPE_LONG);
	InitOneFunction("PLACE$",	"basic::Place",		VARTYPE_DYNSTR);
	InitOneFunction("POS",		"basic::Ccpos",		VARTYPE_LONG);
	InitOneFunction("PROD$",	"basic::Prod",		VARTYPE_DYNSTR);
	InitOneFunction("QUO$",		"basic::Quo",		VARTYPE_DYNSTR);
	InitOneFunction("RAD$",		"basic::Qrad",		VARTYPE_DYNSTR);
	InitOneFunction("RCTLC",	"basic::rctlc()",	VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("RCTLO",	"basic::rctlo()",	VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("RECOUNT",	"basic::recount()",	VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("RIGHT",	"basic::right",		VARTYPE_DYNSTR);
	InitOneFunction("RIGHT$",	"basic::right",		VARTYPE_DYNSTR);
	InitOneFunction("RND",		"basic::floatrand",	VARTYPE_DOUBLE);
	InitOneFunction("RND",		"basic::floatrand()",	VARTYPE_DOUBLE, VARCLASS_NONE);
	InitOneFunction("SEG$",		"basic::Qseg",		VARTYPE_DYNSTR);
	InitOneFunction("SGN",		"basic::sgn",		VARTYPE_LONG);
	InitOneFunction("SIN",		"sin",			VARTYPE_DOUBLE);
	InitOneFunction("SPACE$",	"basic::Qspace",	VARTYPE_DYNSTR);
	InitOneFunction("SQRT",		"sqrt",			VARTYPE_DOUBLE);
	InitOneFunction("SQR",		"sqrt",			VARTYPE_DOUBLE);
	InitOneFunction("STATUS",	"basic::status()",	VARTYPE_LONG, VARCLASS_NONE);
	InitOneFunction("STR$",		"basic::Qnum",		VARTYPE_DYNSTR);
	InitOneFunction("STRING$",	"basic::Qstring",	VARTYPE_DYNSTR);
	InitOneFunction("STR$FIND_FIRST_IN_SET",
					"str$find_first_in_set",VARTYPE_DYNSTR);
	InitOneFunction("SUM$",		"basic::Sum",		VARTYPE_DYNSTR);
	InitOneFunction("SWAP%",	"basic::Swap",		VARTYPE_LONG);
	InitOneFunction("SYS",		"basic::sys",		VARTYPE_DYNSTR);
	InitOneFunction("TAB",		"Tab",			VARTYPE_DOUBLE);
	InitOneFunction("TAN",		"tan",			VARTYPE_DOUBLE);
	InitOneFunction("TIME",		"basic::Time",		VARTYPE_DOUBLE);
	InitOneFunction("TIME$",	"basic::Qtime",		VARTYPE_DYNSTR);
	InitOneFunction("TRM$",		"boost::trim_right_copy",	VARTYPE_DYNSTR);
	InitOneFunction("VAL",		"std::stod",		VARTYPE_DOUBLE);
	InitOneFunction("VAL%",		"std::stol",		VARTYPE_LONG);
	InitOneFunction("XLATE",	"basic::Xlate",		VARTYPE_DYNSTR);
	InitOneFunction("LIB$SIGNAL",	"exit",			VARTYPE_LONG);
}

/**
 * \brief Initialize function names.
 *
 * This function fills in remaining blanks for a function.
 *
 * \bug This function is defined to be used by InitFunctions only.
 */
void VariableList::InitOneFunction(
	const std::string &BasName,		/**< Basic name */
	const char *CeeName,			/**< C++ name */
	VARTYPE Type,			/**< Variable type */
	VARCLASS Class			/**< Variable class */
)
{
	VariableStruct ThisVar(BasName, Type, Class, 1);
	ThisVar.SetCName(CeeName);
	Append(ThisVar, true);
}

