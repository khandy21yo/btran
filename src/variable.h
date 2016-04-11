/**\file variable.h
 * \brief Header file for variable definitions
 *
 *	This is the header file for the variable definitions
 *
 * \bug Lousy. Needs complete re-write
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
	 * \brief Get the class of a variable
	 */
	VARCLASS GetClass() 
	{
		return Class;
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
