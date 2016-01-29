/**\file variable.h
 * \brief Header file for variable definitions
 *
 *	This is the header file for the variable definitions
 *
 * \bug Lousy. Needs complete re-write
 */

/*
History:

	01.01.02 - 10/22/1996 - Kevin Handy
		Added 'SINGLE', "GFLOAT', HFLOAT' types.

	01.03.01 - 04/19/1997 - Kevin Handy
		Start working on parameters, pass mechanism
		Make use of more C++'isms.

	01.04.01 - 07/06/1997 - Kevin Handy
		Renumber VARTYPE's so they fall in weight order.
		Allows easier determination of conversions.

	01.04.02 - 11/28/1997 - Kevin Handy
		Modified to use STL classes instead of my own
		(slow) lists.

	01.05.01 - 12/15/1997 - Kevin Handy
		Changed 'Node' member 'TextValue' from 'char*' to 'string'

	01.05.02 - 02/15/1998 - Kevin Handy
		Changed VariableList to use a <vector> instead of
		the two variables "Global" and "Local", in preparation
		of handling common variables in "DEF*" statements.

		Moved three functions from VariableList to
		ListOfVariables, where they really belonged,
		and lost the 'Tree' on the end of their names.

	01.05.03 - 02/15/1998 - Kevin Handy
		Made VariableList inherit BaseVariableList instead
		haveing a variable based on that type. Get to lose a
		lot of references to 'Variables' that way, as well as
		a lot of junk trying to make iterators and reverse
		iterators happy.

	01.05.04 - 09/04/1998 - Kevin Handy
		Split out the headers for varlist.h from this code

	12/31/2001 - Kevin Handy
		Added ParList to variable to track parameter definitions

*/

//
// Variable.h - Some stuff just in case if isn't defined anywhere else
//
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

//
// Include files
//
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cassert>

/**
 * \brief Structure defining individual variables
 *
 * This class contains individual definitions for variables used
 * in the program.
 */
class VariableStruct
{
public:
	std::string BasicName;		/**< \brief Name used in BASIC */
	VARTYPE Type;			/**< \brief Type (integer, real, ...) */
	VARCLASS Class;			/**< \brief Variable Class (function, array, ...) */
	std::string CName;		/**< \brief Name to be used in C */
	int Output;			/**< \brief Definition Already output to source? */
	int EverUsed;			/**< \brief Is it ever used (Label) */
	int ParCount;			/**< \brief Parameter count */
	int GlobalFlag;			/**< \brief Global/Local flag */
	Node* Original;			/**< \brief Original definition of node */
	std::vector<Node*> ParList;	/**< \brief Original Parameter definitions */

public:
	/**
	 * \brief Constructor
	 *
	 * Used to initailize most values
	 */
	VariableStruct(
		const std::string VarName = "",		/**< Basic variable name */
		VARTYPE newType = VARTYPE_NONE,		/**< Type of variable */
		VARCLASS newClass = VARCLASS_NONE,	/**< Variable class */
		int newOutput = false,			/**< Has variable definition been output yet? */
		int newParCount = 0			/**< Parameter count */
	)
	{
		Type = newType;
		Class = newClass;
		Output = newOutput;
		ParCount = newParCount;
		EverUsed = 0;
		BasicName = VarName;
		UpperCase(BasicName);
		GlobalFlag = 0;
	  	assert(Type != 13 || isdigit(VarName.c_str()[0]) == 0);
		Original = 0;
	}
	/**
	 * \brief Copy constructor
	 */
	VariableStruct(
		const VariableStruct &OldStruct		/**< Original version */
	)
	{
		Type = OldStruct.Type;
		Class = OldStruct.Class;
		Output = OldStruct.Output;
		ParCount = OldStruct.ParCount;
		EverUsed = OldStruct.EverUsed;
		BasicName = OldStruct.BasicName;
		CName = OldStruct.CName;
		GlobalFlag = OldStruct.GlobalFlag;
		ParList = OldStruct.ParList;
		Original = OldStruct.Original;
	}

	// Dump (to terminal)
	void Dump() const;
	// Write out definition in human readable form
	std::string OutputDef();
	/**
	 * \brief Get the C version of the variable name
	 */
	std::string GetName()
	{
		if (CName.length() != 0 )
			return CName;
		else
			return BasicName;
	}
	/**
	 * \brief Change the C version of a variable name
	 */
	void SetCName(
		const char* NewName		/**< New C name */
	) 
	{ CName = NewName; }
	/**
	 * \brief Get type of variable
	 */
	VARTYPE GetType() 
	{
		return Type;
	}
	/**
	 * \brief Change the type of a variable
	 */
	void SetType(
		VARTYPE tType			/**< New type */
	) 
	{
		Type = tType;
	}
	/**
	 * \brief Get the class of a variable
	 */
	VARCLASS GetClass() 
	{
		return Class;
	}
	/**
	 * \brief Change the class of a variable
	 */
	void SetClass(
		const VARCLASS NewClass		/**< New class */
	) 
	{
		Class = NewClass;
	}
	/**
	 * \brief Increment the usage count
	 */
	void IncrUsed() 
	{ EverUsed++; }
	/**
	 * \brief Is this variable ever used for anything
	 */
	int GetEverUsed() 
	{ return EverUsed; }

	/**
	 * \brief Assignment operator
	 *
	 * Copies the most important data over, but does not copy trees or blocks
	 */
	VariableStruct& operator = (
		const VariableStruct &OldStruct		/**< Version to copy from */
	)
	{ Type = OldStruct.Type; Class = OldStruct.Class;
	  Output = OldStruct.Output; ParCount = OldStruct.ParCount;
	  EverUsed = OldStruct.EverUsed; BasicName = OldStruct.BasicName;
	  CName = OldStruct.CName; return *this; }

};


//
// Functions related to variables
//
const char* DumpVarType(VARTYPE VarType);
std::string OutputVarType(VARTYPE VarType);
VARTYPE GuessVarType(const std::string Name);
int UnguessVarType(VARTYPE vartype);

/**
 * \brief Which type to raise expression to
 *
 * When a calculation is done, and the two variables have differing types,
 * determine what the final result should be.
 */
static inline VARTYPE ChooseVarType(
	VARTYPE a,	/**< First variable type */
	VARTYPE b	/**< Second variable type */
)
{
	return (a > b) ? a : b; 
}

#endif
