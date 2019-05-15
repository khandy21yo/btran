/**
 * \file nodes2.cc
 * \brief Routines to work with nodes (variable scanning).
 *
 *	This file contains various routines for manipulating the tree
 *	nodes used when parsing a basic program.
 *
 *	This part includes the variable scanning routines.
 */

//
// System Include Files
//
#include <iostream>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cctype>

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
// Module Variables
//
extern int GosubFlag;
extern int OnErrorFlag;
extern int WhenErrorFlag;

/**
 * \brief Search block for variable names
 *
 *	Scans a block of code for variable names.
 *	This must be done before scanning for the loops.
 *	Call this funstion to process a block of statements.
 *
 */
void Node::VariableScan(
	int InDefineFlag	/**< Are we in a define statement */
)
{
	Node *ThisNode;

	assert(this != 0);

	//
	// Scan this statement
	//
	VariableScanOne(InDefineFlag);

	//
	// Scan Next Statement(s)
	//
	//	I'm doing the scan here instead of having each node
	//	call scan on it's own Dowm because the recursion becomes
	//	a real memory killer on large programs.
	//
	ThisNode = Block[0];

	while (ThisNode != 0)
	{
		ThisNode->VariableScanOne(InDefineFlag);
		ThisNode = ThisNode->Block[0];
	}
}

/**
 * \brief Search statement for variable names.
 *
 *	Scans a block of code for variable names.
 *	This must be done before scanning for the loops.
 *	Call this function to process a single statement.
 */
void Node::VariableScanOne(
	int InDefineFlag	/**< Are we in a define statement */
)
{
	VariableStruct *ThisVar;
	VARTYPE ThisType = VARTYPE_NONE;

	assert(this != 0);

	//
	// Look at current node type
	//
	switch (Type)
	{
	case BAS_N_EXTERNALCONSTANT:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_NONE, true);
		break;

	case BAS_N_EXTERNALSUB:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_FUNC, true);
		break;

	case BAS_S_CALL:
		Tree[0]->ScanVarList(VARTYPE_VOID, VARCLASS_FUNC, false);
		break;

	case BAS_S_COMMON:
		ScanMap();
		break;

	case BAS_S_DATA:
		break;

	case BAS_S_DECLARE:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_NONE, true);
		break;

	case BAS_V_DECLARECONSTANT:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_NONE, true);
		break;

	case BAS_V_DECLAREFUN:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_FUNC, true);
		break;

	case BAS_S_DIM:
		assert(Tree[0] != 0);
		if (Tree[1] != 0)
		{
			Tree[1]->VariableScanOne(1);
		}
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_NONE, true);
		break;

	case BAS_S_EXTERNAL:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_NONE, true);
		break;

	case BAS_S_MAP:
		ScanMap();
		break;

	case BAS_V_FUNCTION:
		//
		// Built in function
		//
		ThisVar = Variables->Lookup(TextValue, this);
		if (ThisVar == 0)
		{
			//
			// Create variable
			//
			VariableStruct NewVar(TextValue,
				VARTYPE_VOID, VARCLASS_FUNC, InDefineFlag);

			//
			// Add to variable table
			//
			Variables->Append(NewVar, true);	// Forced global
		}
		else
		{
			ThisVar->Class = VARCLASS_FUNC;
		}
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScan(InDefineFlag);
		}
		if (Tree[3] != 0)
		{
			Tree[3]->VariableScan(1);
		}
		break;

	case BAS_N_EXTERNALFUNCTION:
		assert(Tree[0] != 0);
		Tree[0]->ScanVarList(VARTYPE_REAL, VARCLASS_FUNC, true);
		break;

	case BAS_S_DEF:
	case BAS_S_DEFSTAR:
		break;

	case BAS_S_FUNCTION:
	case BAS_S_SUB:

		//
		// Function Definition
		//   0 = Return type
		//   1 - Function name
		//   2 -
		//   3 - Calling Parameters
		//
		// must have a name
		//
		assert(Tree[1] != 0);

		//
		// Functions return type
		//
		if (Tree[0] != 0)
		{
			ThisType = Tree[0]->ScanType();
		}
		else
		{
			ThisType = VARTYPE_VOID;
		}

		//
		// This functions name
		//
		ThisVar = Variables->Lookup(Tree[1]->TextValue, this);
		if (ThisVar == 0)
		{
			//
			// Create variable
			//
			VariableStruct NewVar(Tree[1]->TextValue,
				ThisType, VARCLASS_FUNC, InDefineFlag);

			//
			// Add to variable table
			//
			Variables->Append(NewVar, true);	// Forced global
		}
		else
		{
			ThisVar->Class =VARCLASS_FUNC;
			ThisVar->Type = ThisType;
		}

		//
		// Scan calling parameters
		//
		if (Tree[3] != 0)
		{
			Tree[3]->VariableScan(1);
		}

		break;

	case BAS_S_MAINFUNCTION:
		//
		// Function Definition
		//
		assert(Tree[2] != 0);

		if (Tree[0] != 0)
		{
			ThisType = Tree[0]->ScanType();
		}
		else
		{
			ThisType = VARTYPE_VOID;
		}

		ThisVar = Variables->Lookup(Tree[2]->TextValue, this);
		if (ThisVar == 0)
		{
			//
			// Create variable
			//
			VariableStruct NewVar(Tree[2]->TextValue,
				ThisType, VARCLASS_FUNC, InDefineFlag);

			//
			// Add to variable table
			//
			Variables->Append(NewVar, 1);		// Forced global
		}
		else
		{
			ThisVar->Class = VARCLASS_FUNC;
			ThisVar->Type = ThisType;
		}

		//
		// Scan calling parameters
		//
		if (Tree[2]->Tree[0] != 0)
		{
			Tree[2]->Tree[0]->VariableScan(1);
		}

		break;

	case BAS_V_LABEL:
		if (*(TextValue.end() - 1) == ':')
		{
#ifdef STLREMOVE
			TextValue.remove(TextValue.end() - 1);
#else
			TextValue.erase(TextValue.end() - 1);
#endif
		}
		ThisVar = Variables->Lookup(TextValue, 0);
		if (ThisVar == 0)
		{
			VariableStruct NewVar(TextValue,
				VARTYPE_LABEL, VARCLASS_LABEL, true);
			Variables->Append(NewVar);
		}
		break;

	case BAS_V_USELABEL:
		ThisVar = Variables->Lookup(TextValue, 0);
		if (ThisVar == 0)
		{
			VariableStruct NewVar(TextValue,
				VARTYPE_LABEL, VARCLASS_LABEL, true);
			NewVar.EverUsed++;
			Variables->Append(NewVar);
		}
		else
		{
			ThisVar->EverUsed++;
		}
		break;

	case BAS_V_NAME:
		Variables->Define(this, false, InDefineFlag);

		//
		// Look at possible array reference
		//
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScanOne(InDefineFlag);
		}

		if (Tree[2] != 0)
		{
			Tree[2]->VariableScanOne(InDefineFlag);
		}

		break;

	case BAS_S_RECORD:
		assert(Tree[0] != 0);
//
// FIXME:
// We don't scan the structure names because they are not real
// (ie. expposed to the outside world).
// We should probibly create some kind of definition list for structure
// definitions.
//
//		Tree[0]->VariableScanOne(1);
		break;

	case BAS_N_STRUCTNAME:

		//
		// Force structure variable to be usable under C++
		//
		LowerCase(TextValue);

		//
		// Look at possible array reference
		//
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScanOne(InDefineFlag);
		}

		if (Tree[2] != 0)
		{
			Tree[2]->VariableScanOne(InDefineFlag);
		}

		break;

	case BAS_X_STRREF:

		assert (Tree[0] != 0);
		assert (Tree[1] != 0);

		Tree[0]->VariableScanOne(InDefineFlag);
		Tree[1]->VariableScanOne(InDefineFlag);

		break;

	case BAS_S_GOSUB:
		GosubFlag = 1;
		assert(Tree[0] != 0);
		Tree[0]->VariableScanOne(InDefineFlag);
		break;

	case BAS_N_ONERROR:
		OnErrorFlag = 1;
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScanOne(InDefineFlag);
		}
		break;

	case BAS_N_WHENERRORIN:
		WhenErrorFlag = 1;
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScanOne(InDefineFlag);
		}
		break;

	case BAS_N_ONGOSUB:
		GosubFlag = 1;
		assert(Tree[0] != 0);
		assert(Tree[1] != 0);
		Tree[0]->VariableScanOne(InDefineFlag);
		Tree[1]->VariableScanOne(InDefineFlag);
		break;

	case BAS_S_RETURN:
		GosubFlag = 1;
		break;

	case BAS_S_RESUME:
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScanOne(InDefineFlag);
		}
		OnErrorFlag = 1;
		WhenErrorFlag = 1;
		break;

	case '+':
		//
		// Process lower levels first
		//
		Tree[0]->VariableScanOne(InDefineFlag);
		Tree[1]->VariableScanOne(InDefineFlag);

		//
		// Let's try and merge text strings together
		//
		if (Tree[0]->IsReallyString() && Tree[1]->IsReallyString())
		{
			//
			// Create merged string (Using the colon
			// concatination options)
			//
			std::string Merger = Tree[0]->TextValue + Tree[1]->TextValue;

			//
			// Change the root node to the concatinated string
			// type.
			//
			Type = BAS_V_TEXTSTRING;
			delete Tree[0];
			Tree[0] = 0;
			delete Tree[1];
			Tree[1] = 0;
			TextValue = Merger;
		}
		break;

	case BAS_S_CHANGE:
		//
		// This is a strange statement, and we need to determine
		// which one is a string and which one is a numeric
		// array.
		//
		{
			assert(Tree[1] != 0);
			//
			// Assume 2nd one is a string
			//
			VARTYPE Type1 = VARTYPE_DYNSTR;
			if (Tree[1]->Type != BAS_V_NAME)
			{
				//
				// If it isn't a variable name, it better be
				// a string. (You can't add arrays)
				//
				Type1 = VARTYPE_DYNSTR;
			}
			else
			{
				//
				// Does it already have a defined type?
				//
				ThisVar = Variables->Lookup(Tree[1]->TextValue, 0);
				if ((ThisVar != 0) &&
					(ThisVar->Type == VARTYPE_DYNSTR))
				{
					Type1 = ThisVar->Type;
				}
				else
				{
					Type1 = GuessVarType(Tree[0]->TextValue);
				}
			}
			//
			// Ok, now force one of them to be an array
			//
			if (Type1 == VARTYPE_DYNSTR)
			{
				// change a% to b$
				Type = BAS_S_CHANGE2;
				Tree[0]->Link(new Node(BAS_N_NULL));
			}
			else
			{
				// change a$ to b%
				Type = BAS_S_CHANGE1;
				Tree[1]->Link(new Node(BAS_N_NULL));
			}
		}
		Tree[0]->VariableScan(InDefineFlag);
		Tree[1]->VariableScan(InDefineFlag);
		break;

	case BAS_S_MAT:
		assert(Tree[0] != 0);
		assert(Tree[1] != 0);

		//
		// Force array type to l-value
		//
		Tree[0]->Link(new Node(BAS_N_NULL));
		Tree[0]->VariableScan(InDefineFlag);

		switch(Tree[1]->Type)
		{
		case BAS_S_CON:		// MAT xxx = ZER
		case BAS_S_IDN:
		case BAS_S_ZER:
			break;

		case BAS_S_TRN:		// MAT xxx = TRN(yyy)
		case BAS_S_INV:
			assert(Tree[1]->Tree[0] != 0);

			//
			// rvalue MUST be array
			//
			Tree[1]->Tree[0]->Link(new Node(BAS_N_NULL));
			Tree[1]->VariableScan(InDefineFlag);
			break;

		case '+':			// MAT xxx = yyy + zzz
		case '-':
		case '*':
			assert(Tree[1]->Tree[0] != 0);
			assert(Tree[1]->Tree[1] != 0);

			//
			// Two rvalues MUST be arrays
			//
			Tree[1]->Tree[0]->Link(new Node(BAS_N_NULL));
			Tree[1]->Tree[1]->Link(new Node(BAS_N_NULL));
			Tree[1]->VariableScan(InDefineFlag);
			break;

		case '(':			// MAT xxx = (yyy) * zzz
			assert(Tree[1]->Tree[0] != 0);
			assert(Tree[1]->Tree[1] != 0);

			//
			// right rvalue values MUST be arrays
			//
			Tree[1]->Tree[1]->Link(new Node(BAS_N_NULL));
			Tree[1]->Tree[0]->VariableScan(InDefineFlag);
			Tree[1]->Tree[1]->VariableScan(InDefineFlag);
			break;

		default:			// Oops, forgot to code something

			assert(Tree[1] == 0);	// Should never reach here
			break;
		}

		break;

	case BAS_V_INTEGER:
	case BAS_V_FLOAT:
	case BAS_V_INT:
		// Ignore various numeric constants
		break;

	default:
		if (Tree[0] != 0)
		{
			Tree[0]->VariableScan(InDefineFlag);
		}

		if (Tree[1] != 0)
		{
			Tree[1]->VariableScan(InDefineFlag);
		}

		if (Tree[2] != 0)
		{
			Tree[2]->VariableScan(InDefineFlag);
		}

		if (Tree[3] != 0)
		{
			Tree[3]->VariableScan(InDefineFlag);
		}

		if (Tree[4] != 0)
		{
			Tree[4]->VariableScan(InDefineFlag);
		}

		if (Block[1] != 0)
		{
			Block[1]->VariableScan(InDefineFlag);
		}

		if (Block[2] != 0)
		{
			Block[2]->VariableScan(InDefineFlag);
		}
		break;
	}
}

/**
 * \brief Scans a block of code for the variable type.
 *
 *	Scans a (possible) list for the variable names of the
 *	current type. This is used for definitions and calls.
 *
 */
void Node::ScanVarList(
	VARTYPE ThisType,	/**< This type */
	VARCLASS ThisClass,	/**< This class */
	int Status		/**< This status */
)
{
	VariableStruct* ThisVar;
	assert(this != 0);

	switch(Type)
	{
	case BAS_N_LIST:
	case BAS_N_ASSIGNLIST:

		if (Tree[0] != 0)
		{
			Tree[0]->ScanVarList(ThisType, ThisClass, Status);
		}
		if (Tree[1] != 0)
		{
			Tree[1]->ScanVarList(ThisType, ThisClass, Status);
		}
		break;

	case '=':
	case BAS_N_ASSIGN:

		Tree[0]->ScanVarList(ThisType, ThisClass, Status);
		break;

	case BAS_V_DEFINEVAR:

		assert(Tree[1] != NULL);
		ThisType = Tree[1]->ScanType();

		if (Tree[0] != 0)
		{
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[2]);
			if (ThisVar == 0)
			{
				VARCLASS ThisClass = VARCLASS_NONE;
				if (Tree[2] != 0)
				{
					ThisClass = VARCLASS_ARRAY;
				}

				//
				// Create variable
				//
				VariableStruct NewVar(Tree[0]->TextValue,
					ThisType, ThisClass, Status);

				//
				// Add to variable table
				//
				Variables->Append(NewVar, false);
			}
			else
			{
				ThisVar->Class =VARCLASS_NONE;
				ThisVar->Type = ThisType;
			}
		}

		if (Tree[2] != NULL)
		{
			Tree[2]->ScanVarList(ThisType, ThisClass, Status);
		}

		if (Block[0] != NULL)
		{
			Block[0]->ScanVarList(ThisType, ThisClass, Status);
		}

		break;

	case BAS_V_DEFINEFUN:

		if (Tree[1] != 0)
		{
			ThisType = Tree[1]->ScanType();
		}
		else
		{
			std::cerr << "Scanning: Type = NULL in BAS_V_DEFINEFUN" << std::endl;
			ThisType = VARTYPE_NONE;
		}
		if (Tree[0] != 0)
		{
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[2]);
			if (ThisVar == 0)
			{
				//
				// Create variable
				//
				VariableStruct NewVar(Tree[0]->TextValue,
					ThisType, VARCLASS_FUNC, Status);

				//
				// Add to variable table
				//
				Variables->Append(NewVar, true);
			}
			else
			{
				ThisVar->Class = VARCLASS_FUNC;
				ThisVar->Type = ThisType;
			}
		}

		if (Tree[2] != NULL)
		{
			Tree[2]->ScanVarList(ThisType, VARCLASS_NONE, true);
		}

		if (Block[0] != NULL)
		{
			Block[0]->ScanVarList(ThisType, ThisClass, Status);
		}

		break;

	case BAS_V_INTEGER:
	case BAS_V_FLOAT:
	case BAS_V_INT:
		// Ignore various numeric constants
		break;

	case BAS_V_FUNCTION:
		ThisVar = Variables->Lookup(TextValue, Tree[0]);
		if (ThisVar == 0)
		{
			VARTYPE FinalType = ThisType;
			if (FinalType == VARTYPE_NONE)
			{
				FinalType = GuessVarType(TextValue);
			}
			VariableStruct NewVar(TextValue,
				FinalType, ThisClass, Status);
			Variables->Append(NewVar);
		}
		if (Tree[0] != NULL)
		{
			Tree[0]->VariableScanOne(false);
		}
		break;

	case BAS_S_BY:
		if (Tree[0] != NULL)
		{
			Tree[0]->ScanVarList(ThisType, ThisClass, false);
		}
		break;

	case BAS_V_NAME:
		ThisVar = Variables->Lookup(TextValue, Tree[0]);
		if (ThisVar == 0)
		{
			VARTYPE FinalType = ThisType;
			if (FinalType == VARTYPE_NONE)
			{
				FinalType = GuessVarType(TextValue);
			}
			VariableStruct NewVar(TextValue,
				FinalType, ThisClass, Status);
			Variables->Append(NewVar);
		}
		break;
	}
}


/**
 * \brief Search block for type definion
 *
 *	Scans a block of code for the variable type.
 */
VARTYPE Node::ScanType(void)
{
	//
	// Don't even try it is we don't know what's happening
	//
	assert(this != 0);

	//
	// Try to figure out the variable type
	//
	switch (Type)
	{
	case BAS_S_REAL:
		return(RealType);

	case BAS_S_DOUBLE:
		return(VARTYPE_DOUBLE);

	case BAS_S_BYTE:
		return(VARTYPE_BYTE);

	case BAS_S_GFLOAT:
		return(VARTYPE_DOUBLE);

	case BAS_S_HFLOAT:
		return(VARTYPE_DOUBLE);

	case BAS_S_INTEGER:
		return(VARTYPE_INTEGER);

	case BAS_S_LONG:
		return(VARTYPE_LONG);

	case BAS_S_RFA:
		return(VARTYPE_RFA);

	case BAS_S_STRING:
		return(VARTYPE_DYNSTR);

	case BAS_S_WORD:
		return(VARTYPE_WORD);

	case BAS_S_DECIMAL:
		return(VARTYPE_DECIMAL);
	}

	return(VARTYPE_NONE);
}

/**
 * \brief Tries to determine if a node is a number
 *
 * Attempts to determine if a no9de is some type of number.
 *
 * \return
 * 0 - No
 * 1 - Yes
 * 2 - Maybe
 */
int Node::IsNumber(void)
{
	VariableStruct *ThisVar;

	//
	// Try to figure out the variable type
	//
	switch (Type)
	{
	case BAS_V_FUNCTION:
	case BAS_V_NAME:
		//
		// Look for variable in table
		//
		ThisVar = Variables->Lookup(TextValue, Tree[0]);

		//
		// Print this variable
		//
		if (ThisVar != 0)
		{
			switch (ThisVar->Type)
			{
			case VARTYPE_BYTE:
			case VARTYPE_WORD:
			case VARTYPE_INTEGER:
			case VARTYPE_LONG:
			case VARTYPE_SINGLE:
			case VARTYPE_REAL:
			case VARTYPE_DOUBLE:
			case VARTYPE_HFLOAT:
			case VARTYPE_GFLOAT:
			case VARTYPE_DECIMAL:
				return 1;

			}
		}
		return 0;

	//
	// Sometomes used for strings.
	//
	case '+':
		return Tree[0]->IsNumber();

	//
	// Always Numeric
	//
	case '-':
	case '*':
	case '/':
	case '>':
	case '<':
	case BAS_S_MOD:
	case '^':
	case BAS_X_LAND:
	case BAS_S_EQV:
	case BAS_S_IMP:
	case BAS_X_GE:
	case BAS_X_LE:
	case BAS_P_NAME:
	case BAS_X_EQ:
	case BAS_X_NEQ:
	case BAS_S_NOT:
	case BAS_X_LNOT:
	case BAS_X_LOR:
	case BAS_S_OR:
	case BAS_N_UMINUS:
	case BAS_N_UPLUS:
	case BAS_S_XOR:
		return 1;

	// Easily a number
	//
	case BAS_V_INT:
	case BAS_V_INTEGER:
	case BAS_V_FLOAT:
		return(1);

	//
	// I don't know
	//
	default:
		return(0);
	}
}

/**
 * \brief Tries to determine if a node is a string
 *
 * Attempts to determine if a no9de is some type of string.
 *
 * \return
 * 0 - No
 * 1 - Yes
 * 2 - Maybe
 */
int Node::IsString(void)
{
	VariableStruct *ThisVar;

	//
	// Try to figure out the variable type
	//
	switch (Type)
	{
	case BAS_V_FUNCTION:
	case BAS_V_NAME:
		//
		// Look for variable in table
		//
		ThisVar = Variables->Lookup(TextValue, Tree[0]);

		//
		// Print this variable
		//
		if (ThisVar != 0)
		{
			switch (ThisVar->Type)
			{
			case BAS_S_STRING:
			case VARTYPE_DYNSTR:
				return 1;

			}
		}
		return 0;

	//
	// Sometomes used for strings.
	//
	case '+':
		return Tree[0]->IsString();

	//
	// I don't know
	//
	default:
		return(0);
	}
}

/**
 * \brief SmoothTypes
 *
 *	Attempt to spread the data types through all items on
 *	a given definition instead of having them only on the
 *	first item
 */
void Node::SmoothTypes(
	Node* DefaultType	/**< Node that defines the default types */
)
{
	bool actualtype = false;

	assert(this != 0);

	if (Tree[1] == 0)
	{
		//
		// Fake up a defintion for this segment since we dont have
		// on entered directly on this line.
		//
		if (DefaultType != 0)
		{
			VARTYPE rawtype = GuessVarType(Tree[0]->TextValue);
			if (rawtype == VARTYPE_REAL)
			{
				Tree[1] = new Node(*DefaultType);
			}
			else
			{
				Tree[1] = new Node(UnguessVarType(rawtype));
			}
		}
		else
		{
			if (Tree[0] != 0)
			{
				Tree[1] = new Node(UnguessVarType(
					GuessVarType(Tree[0]->TextValue)));
			}
			else
			{
				Tree[1] = new Node(UnguessVarType(VARTYPE_NONE));
			}
		}
	}
	else
	{
		//
		// We need to keep track of weither we have been given
		// an actual definition on this segment, or if we have
		// faked one up.
		//
		actualtype = true;
	}

	if (Block[0] != 0)
	{
		//
		// If we have an actual type here, use it, else use
		// what came from a previous segment
		//
		if (actualtype)
		{
			Block[0]->SmoothTypes(Tree[1]);
		}
		else
		{
			Block[0]->SmoothTypes(DefaultType);
		}
	}
}

/** 
 * \brief Scans a map or common statement.
 *
 *	This function will mangle a map or common statement.
 */
void Node::ScanMap(void)
{
	assert(this != 0);
	assert(Tree[0] != 0);
 
	VariableStruct ThisVar(Tree[0]->TextValue,
		VARTYPE_STRUCT, VARCLASS_NONE, true);

	//
	// We need to name the map so we can use the nasme as a prefix.
	// This will likely cause problems with ocerlapping MAP/COMMON
	// statements, but it's better than nothing.
	//
	std::string tempname = genname(ThisVar.BasicName);
	if (Variables->LookupReserved(tempname) == false)
	{
		//tempName = tempname;
	}
	else
	{
		tempname += "_V" + std::to_string(Variables->vcount++);
	}
	ThisVar.CName = tempname;

	Variables->Append(ThisVar);
	Variables->Fixup();
 
	if (Tree[1] != 0)
	{
		Tree[1]->ScanOneMap(&ThisVar,
			Tree[1]->Type == BAS_V_DEFINEVAR &&
			Tree[1]->Block[0] == 0);
	}
}

/** 
 * \brief Scans a map or common statement.
 *
 *	This function will mangle a map or common statement.
 */ 
 void Node::ScanOneMap(
	VariableStruct* MapVar,	/**< Main map variable */
	int skipprefix
)
{
	assert(this != 0);
 
	switch (Type)
	{
	case BAS_N_LIST:
 
		if (Tree[0] != 0)
		{
			Tree[0]->ScanOneMap(MapVar, skipprefix);
		}
		if (Tree[1] != 0)
		{
			Tree[1]->ScanOneMap(MapVar, skipprefix);
		}
		break;                                                                   
	case BAS_V_NAME:
 
		{
			VariableStruct NewVar(TextValue,
				VARTYPE_NONE, VARCLASS_MAP, true);
			if (!skipprefix)
			{
				NewVar.Prefix = MapVar->CName;
			}
			Variables->Append(NewVar);
		}
		break;
 
	case BAS_N_VARTYPE:
	case BAS_S_BYTE:
	case BAS_S_DOUBLE:
	case BAS_S_GFLOAT:
	case BAS_S_HFLOAT:
	case BAS_S_LONG:
	case BAS_S_WORD:
	case BAS_S_INTEGER:
	case BAS_S_REAL:
	case BAS_S_RFA:
	case BAS_S_STRING:
	case BAS_S_DECIMAL:
 
		assert(Tree[1] != 0);
 
		{
			VariableStruct NewVar(Tree[1]->TextValue,
				VARTYPE_NONE, VARCLASS_MAP, true);
			if (!skipprefix)
			{
				NewVar.Prefix = MapVar->CName;
			}
			Variables->Append(NewVar);
		}
		break;
 
// FIXME: This is just to have something here, but it doesn't do
// the right thing yet
	case BAS_V_DEFINEVAR:
 
		assert(Tree[0] != 0);                                                
		{
			VariableStruct NewVar(Tree[0]->TextValue,
				VARTYPE_NONE, VARCLASS_MAP, true);
			if (!skipprefix)
			{
				NewVar.Prefix = MapVar->CName;
			}
			Variables->Append(NewVar);
		}
		break;
 
	default:
		std::cerr << "Undefined type " << Type << "@" << lineno << std::endl;
		break;
	}
 
	if (Block[0] != 0)
	{
		Block[0]->ScanOneMap(MapVar, skipprefix);
	}
}
