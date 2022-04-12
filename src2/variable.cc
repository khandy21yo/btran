/**
 * \file variable.cc
 * \brief Routines to handle variables.
 *
 *	This file contains routines to handle the variable definitions
 *	necessary for basic.
 */

/*
 * System Include Files
 */
#include <iostream>
#include <string>
#include <cctype>

//
// Project Include Files
//
#include "vartype.h"
#include "basic.h"
#include "variable.h"
#include "parse.hh"


/**
 * \brief Print out variable.
 * 
 * Prints out a variables information.
 * 
 * \bug Ugly output.
*/
void VariableStruct::Dump(void) const
{
	//
	// Print out this variable
	//
	std::cout << "Var: " << BasicName;

	if (Class == VARCLASS_ARRAY)
	{
		std::cout << "[" << ParCount << "]";
	}

	std::cout << ", Type " << Type << DumpVarType(Type) <<
		", Class " << Class;

	switch (Class)
	{
	case VARCLASS_NONE:
		std::cout << " Regular variable";
		break;

	case VARCLASS_ARRAY:
		std::cout << " An Array";
		break;

	case VARCLASS_FUNC:
		std::cout << " Function";
		break;

	case VARCLASS_CONST:
		std::cout << " Constant";
		break;

	case VARCLASS_MAP:
		std::cout << " Map";
		break;

	default:
		std::cout << " Undefined";
		break;
	}

	if (Prefix.length() != 0)
	{
		std::cout << ", Prefix " << Prefix;
	}

	if (CName.length() != 0)
	{
		std::cout << ", CName " << CName;

		if (Class == VARCLASS_ARRAY)
		{
			std::cout << "[]";
		}
	}

	std::cout << ", Usage " << EverUsed;
	std::cout << std::endl;
}


/**
 * \brief Dump out one variable node.
 *
 * This function will return one variables definition.
 * These are only used if the definition isn't output
 * as part of the source code.
 *
 * \return string for the variables type.
 */
std::string VariableStruct::OutputDef(void)
{
	//
	// Local Variables
	//
	std::string result;
	VARTYPE ActualType = Type;

	//
	// Output This Variable
	//
	switch (Class)
	{
	case VARCLASS_CONST:
		result = "const ";
		break;
	}

	result += OutputVarType(ActualType);
	result += " ";

	//
	// Pick a good name
	//
	if (Prefix.length() != 0)
	{
		result += Prefix + ".";
	}

	//
	// Pick a good name
	//
	if (CName.length() != 0)
	{
		result += CName;
	}
	else
	{
		result += BasicName;
	}

	switch(Type)
	{
	case VARTYPE_FIXSTR:
		result += "[]";
		break;
	}

	//
	// Add brackets if this is an array definition
	//
	switch (Class)
	{
	case VARCLASS_ARRAY:
		//
		// Default array dimension under Basic is 0..10
		// for most Basic's I've seen.
		//
		result += "[11]";
		for (int loop = 2; loop <= ParCount; loop++)
		{
			result += "[11]";
		}
		break;

	case VARCLASS_FUNC:
		result += "()";
		break;
	}

	result += OutputVarInit(Class, Type);

	//
	// Flag this one as having been output.
	//
	Output = true;
	return result;
}


/**
 * \brief Guess the type of a variable based on it's name
 *
 * Looks at a variables name and attempts to guess what type
 * of variable it is. This type of guess is only used when
 * an actual definition is not given.
 *
 * \bug Makes really simple guesses.
 */
VARTYPE GuessVarType(
	const std::string Name	/**< Name variable name */
)
{
	switch(*(Name.end() - 1))
	{
	case '$':
		return VARTYPE_DYNSTR;
	case '%':
		return VARTYPE_INTEGER;
	default:
		return DefaultType;
	}
}

/**
* \brief Given a VARTYPE, return back the node type
*/
int UnguessVarType(
	VARTYPE vartype		/**< Type of variable to 
				 * work backwards from. */
)
{
	switch(vartype)
	{
	case VARTYPE_RFA:
		return BAS_S_RFA;
	case VARTYPE_FIXSTR:
	case VARTYPE_DYNSTR:
		return BAS_S_STRING;
	case VARTYPE_BYTE:
		return BAS_S_BYTE;
	case VARTYPE_WORD:
		return BAS_S_WORD;
	case VARTYPE_INTEGER:
		return BAS_S_INTEGER;
	case VARTYPE_LONG:
		return BAS_S_LONG;
	case VARTYPE_SINGLE:
		return BAS_S_SINGLE;
	case VARTYPE_REAL:
		return BAS_S_REAL;
	case VARTYPE_DOUBLE:
		return BAS_S_DOUBLE;
	case VARTYPE_HFLOAT:
		return BAS_S_HFLOAT;
	case VARTYPE_GFLOAT:
		return BAS_S_GFLOAT;
	case VARTYPE_DECIMAL:
		return BAS_S_DECIMAL;
	default:
		return BAS_S_INTEGER;
	}
}

/**
 * \brief Print out variable type.
 *
 * Returns a variable type in human readable format
 *
 * \return String describing the variable
 * \bug Ugly output.
 */
const char* DumpVarType(
	VARTYPE VarType		/**< Type of variable to display */
)
{
	const char* result;

	switch (VarType)
	{
	case VARTYPE_DYNSTR:
		result = " Dynamic String";
		break;

	case VARTYPE_FIXSTR:
		result = " Fixed length string";
		break;

	case VARTYPE_WORD:
		result = " 16 bit integer";
		break;

	case VARTYPE_INTEGER:
		result = " integer";
		break;

	case VARTYPE_LONG:
		result = " 32 bit integer";
		break;

	case VARTYPE_REAL:
		result = " float";
		break;

	case VARTYPE_DOUBLE:
		result = " double";
		break;

	case VARTYPE_STRUCT:
		result = " Structure";
		break;

	case VARTYPE_VOID:
		result = " Void";
		break;

	case VARTYPE_BYTE:
		result = " Byte";
		break;

	case VARTYPE_RFA:
		result = " RFA";
		break;

	case VARTYPE_LABEL:
		result = " Label";
		break;

	case VARTYPE_DECIMAL:
		result = " Decimal";
		break;

	case VARTYPE_SINGLE:
		result = " Single";
		break;

	case VARTYPE_GFLOAT:
		result = " GFloat";
		break;

	case VARTYPE_HFLOAT:
		result = " HFloat";
		break;

	default:
		result = " Undefined Type";
		break;
	}
	return result;
}

/**
 * \brief Print out variable type.
 *
 * Returns a variable type in C++ format.
 *
 * \return String containing description.
 *
 * \bug Ugly output.
*/
std::string OutputVarType(
	VARTYPE VarType		/**< Variable type */
)
{
	VARTYPE ActualType = VarType;
	const char* result;

	switch(ActualType)
	{
	case VARTYPE_DYNSTR:
//	case BAS_S_STRING:
		result = "std::string";
		break;

	case VARTYPE_FIXSTR:
		result = "char";
		break;

	case VARTYPE_INTEGER:
//	case BAS_S_INTEGER:
		switch(IntegerType)
		{
		case VARTYPE_WORD:
			result = "int";
			break;

		case VARTYPE_LONG:
			result = "long";
			break;

		case VARTYPE_BYTE:
			result = "char";
			break;

		default:
			result = "??unknown int??";
			break;
		}
		break;

	case VARTYPE_WORD:
//	case BAS_S_WORD:
		result = "int";
		break;

	case VARTYPE_LONG:
//	case BAS_S_LONG:
		result = "long";
		break;

	case VARTYPE_REAL:
//	case BAS_S_REAL:
		switch(RealType)
		{
		case VARTYPE_DOUBLE:
			result = "double";
			break;

		case VARTYPE_GFLOAT:
			result = "Gfloat";
			break;

		case VARTYPE_HFLOAT:
			result = "Hfloat";
			break;

		default:
			result = "??unknown float??";
			break;
		}
		break;

	case VARTYPE_DOUBLE:
//	case BAS_S_DOUBLE:
		result = "double";
		break;

	case VARTYPE_VOID:
		result = "void";
		break;

	case VARTYPE_BYTE:
//	case BAS_S_BYTE:
		result = "char";
		break;

	case VARTYPE_RFA:
//	case BAS_S_RFA:
		result = "RFA";
		break;

	case VARTYPE_DECIMAL:
//	case BAS_S_DECIMAL:
		result = "Decimal";
		break;

	case VARTYPE_SINGLE:
//	case BAS_S_SINGLE:
		result = "Sfloat";
		break;

	case VARTYPE_GFLOAT:
//	case BAS_S_GFLOAT:
		result = "Gfloat";
		break;

	case VARTYPE_HFLOAT:
//	case BAS_S_HFLOAT:
		result = "Hfloat";
		break;

	case VARTYPE_LABEL:
		result = "label";
		break;

//	case BAS_N_STRUCTNAME:
//		std::cerr << "FIXME: Called OutputVarType with a structure name" << std::endl;
//		result = "struct";
//		break;

	default:
		result = "??unknown??";
		break;

	}
	return std::string(result);
}

/**
 * \brief Output an initializer for a variable definition, since c++ does
 * not initialize memory for variables automatically.
 *
 *	This function is used to output the variable initilization for
 *	nodes in a definition statement for types that are not initialized
 *	in some other method.
 */
std::string OutputVarInit(int Class, int Type)
{
	std::string result;

	switch (Class)
	{
	case VARCLASS_ARRAY:
		switch (Type)
		{
		case VARTYPE_INTEGER:
		case VARTYPE_LONG:
		case VARTYPE_BYTE:
		case VARTYPE_WORD:
			result = " = { 0 }";
			break;

		case VARTYPE_REAL:
		case VARTYPE_GFLOAT:
		case VARTYPE_HFLOAT:
		case VARTYPE_DOUBLE:
		case VARTYPE_DECIMAL:
		case VARTYPE_SINGLE:
			result = " = { 0.0 }";
			break;
		}
		break;


	default:
		switch(Type)
		{
		case VARTYPE_INTEGER:
		case VARTYPE_LONG:
		case VARTYPE_BYTE:
		case VARTYPE_WORD:
			result = " = 0";
			break;

		case VARTYPE_REAL:
		case VARTYPE_GFLOAT:
		case VARTYPE_HFLOAT:
		case VARTYPE_DOUBLE:
		case VARTYPE_DECIMAL:
		case VARTYPE_SINGLE:
			result = " = 0.0";
			break;
		}
		break;
	}

	return result;
}

