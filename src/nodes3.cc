/**
 * \file nodes3.cc
 * \brief Routines to work with nodes (code generation).
 *
 *	This file contains various routines for manipulating the tree
 *	nodes used when parsing a basic program.
 *
 *	This file contains the code generation routines.
 */

//
// System Include Files
//
#include <iostream>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cctype>
#include <cstring>

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

static int IOChPrinted;		/**< \brief Has channel number been output */
static Node* IOChannel;		/**< \brief IO Channel to use (NULL if default) */
static Node* IOUsing;		/**< \brief Print using format */
static int DataWidth;		/**< \brief Used to format DATA statements */

std::string erl = "0";		/**< Last numeric line number seen. */

/** \brief add/subtract from string with optimization
 *
 * Adds or subtracts from a value, and if possible
 * optimizing it to a simple value if possible, otherwise
 * appending a '+ n' or '- n' ss necessary.
 */
static std::string IncrString(const std::string &orig, long increment)
{
	const char *nptr = orig.c_str();
	char *endptr = 0;
	long value;
	char buffer[64];

	/*
	 * Try to convert original string
	 */
	value = strtol(nptr, &endptr, 10);

	/*
	 * Did it work?
	 */
	if (*endptr == 0)
	{
		sprintf(buffer, "%ld", value + increment);
		return buffer;
	}
	else
	{
		std::string Result = orig;
		if (increment >= 0)
		{
			sprintf(buffer, "%ld", increment);
			Result = Result + " + " + buffer;
		}
		else
		{
			sprintf(buffer, "%ld", -increment);
			Result = Result + " - " + buffer;
		}
		return Result;
	}
}


/**
 * \brief Main function to output translated code.
 *
 *	This function outputs the translated code.
 */
void Node::Output(
	std::ostream& os	/**< Stream to write C++ code to */
)
{
	//
	// Handle Include Files
	//
	if (NeedIostreamH != 0)
	{
		os << "#include <iostream>" << std::endl;
		os << "#include <limits>" << std::endl;
		os << "#include <array>" << std::endl;
		NeedIostreamH = 0;
	}

	os << "#include <cstdlib>" << std::endl;
	os << "#include <cstring>" << std::endl;
	NeedStdlibH = 0;

	os << "#include <unistd.h>" << std::endl;
	NeedUnistd = 0;

	if (NeedMathH != 0)
	{
		os << "#include <cmath>" << std::endl;
		NeedMathH = 0;
	}

	if (NeedErrorH != 0)
	{
		os << "#include <cerrno>" << std::endl;
		NeedErrorH = 0;
	}

	if (NeedTimeH != 0)
	{
		os << "#include <ctime>" << std::endl;
		NeedTimeH = 0;
	}

	os << "#include \"basicfun.h\"" << std::endl;
	os << "#include \"basicchannel.h\"" << std::endl;

	NeedChannel = 0;
	NeedBasicFun = 0;

	if (NeedDataList != 0)
	{
		os << "#include \"datalist.h\"" << std::endl;
		NeedDataList = 0;
	}

	if (NeedPuse != 0)
	{
		os << "#include \"pusing.h\"" << std::endl;
	}

	if (NeedVirtual != 0)
	{
		os << "#include \"virtual.h\"" << std::endl;
		NeedVirtual = 0;
	}

	os << std::endl;

	if (NeedRFA != 0)
	{
		os << "typedef char RmsRfa[6];" << std::endl;
		NeedRFA = 0;
	}

	if (NeedPuse != 0)
	{
		os << "static basic::PUsing PUse;\t// Needed for print using" << std::endl;
		NeedPuse = 0;
	}

	//
	// This code duplicates basicfun.h, but sometimes it is still needed
	// 
	os << Indent() << "extern basic::BasicError Be;" <<
		std::endl;

	os << std::endl;

	//
	// Output the variables
	//
	Variables->OutputDef(os, Level);

	//
	// Dump out prototypes
	//
	OutputPrototypes(os);

	//
	// Output code
	//
	Level = 0;
	OutputCode(os);
}


/**
 * \brief Output one block of code.
 *
 *	This function goes through the level 0 code (statements)
 *	and handle output of the commands at that level.
 */
void Node::OutputCode(
	std::ostream& os	/**< iostream to write the C++ code to */
)
{
	//
	// Local Variables
	//
	Node *Program = this;

	//
	// Starting at the top node, we scan down handling each
	// level 0 node type as necessary.
	//
	// Could do this by calling OutputCode recursively for the Down
	// level instead of splitting into OutputCode and OutputCodeOne,
	// but that would consume huge amounts of stack space.
	//
	while (Program != 0)
	{
		//
		// Handle Statement. Don't output code from included files.
		//
		if ((Program->FromInclude == 0) || (CompileFlag != 0))
		{
			Program->OutputCodeOne(os);
		}

		//
		// Get next node
		//
		Program = Program->Block[0];
	}
}


/**
 * \brief Output one block of code.
 *
 *	This function will output a block of code (if it exists)
 *	surrounded by parentheses, otherwise it will output a semicolon.
 */
void Node::OutputBlock(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	//
	// If this is a 0 bit of code, just output a semicolon
	//
	if (this == 0)
	{
		os << Indent() << ";" << std::endl;
	}
	else
	{
		//
		// Start the block
		//
		os << Indent() << "{" << std::endl;

		Level++;

		//
		// Local Variables
		//
		Node *Program = this;

		//
		// Starting at the top node, we scan down handling each
		// level 0 node type as necessary.
		//
		// Could do this by calling OutputCode recursively for the Down
		// level instead of splitting into OutputCode and OutputCodeOne,
		// but that would probibly consume too much stack space.
		//
		while (Program != 0)
		{
			//
			// Handle Statement
			//
			if ((Program->FromInclude == 0) || (CompileFlag != 0))
			{
				Program->OutputCodeOne(os);
			}

			//
			// Get next node
			//
			Program = Program->Block[0];
		}

		//
		// End the block
		//
		Level--;
		os << Indent() << "}" << std::endl;
	}
}


/**
 * \brief Return a node losing any parentheses around the whole group.
 *
 *	Removes the parentheses from around the nodes when they are
 *	not needed.
 *
 * \bug This should probably be taken care of during the parsing phase,
 *	which would simplify code generation a lot, but...
 */
std::string Node::NoParen(void)
{
	if (Type == '(')
	{
		return Tree[0]->NoParen();
	}
	else
	{
		return Expression();
	}
}

/**
 * \brief Return a node adding parentheses if necessary around the whole group.
 *
 *	Sometimes it is necessr to group parentheses around an expression
 *	when an operation is to be done to the entire group.
 *
 * \bug This should probably be taken care of during the parsing phase,
 *	which would simplify code generation a lot, but...
 */
std::string Node::Paren(void)
{
	switch(Type)
	{
	// Add other operations as needed
	case '+':
	case '-':
		return std::string("(") + Expression() + ")";

//	case  '(':
	default:
		return Expression();
	}
}

/**
 * \brief Return a node adding parentheses if necessary around the whole group,
 * and attempt to make sure resulting value is a std::string..
 *
 *	Sometimes it is necessr to group parentheses around an expression
 *	when an operation is to be done to the entire group.
 *
 * \bug This should probably be taken care of during the parsing phase,
 *	which would simplify code generation a lot, but...
 */
std::string Node::ParenString(void)
{
	if (TextValue[0] == '"' || TextValue[0] == '\'')
	{
		return std::string("std::string(") +
			Expression() + ")";
	}
	else
	{
		switch(Type)
		{
		// Add other operations as needed
		case '+':
		case '-':
			return std::string("(") + Expression() + ")";

	//	case  '(':
		default:
			return Expression();
		}
	}
}

/**
 * \brief Output function Prototypes.
 *
 *	This function scans through the basic blocks and dumps out
 *	function prototypes for all functions defined therin.
 */
void Node::OutputPrototypes(
	std::ostream& os	/**< iostream to write the C++ code to */
)
{
	Node* ThisNode = this;

	//
	// Output title for prototype section
	//
	os << std::endl <<
		"//" << std::endl <<
		"// Function Prototypes" << std::endl <<
		"//" << std::endl;

	//
	// Go through the level 0 functions
	//
	while(ThisNode != 0)
	{

		switch(ThisNode->Type)
		{
		case BAS_S_PROGRAM:
		case BAS_S_FUNCTION:
		case BAS_S_SUB:
		case BAS_S_HANDLER:

			//
			// Empty variable list
			//
			Variables->NewLevel();

			//
			// Handle any DEF*'s that are in there
			//
			if (ThisNode->Block[2] != 0)
			{
				ThisNode->Block[2]->OutputPrototypes(os);
			}

			//
			// Scan the call parameters
			//
			ThisNode->VariableScanOne(1);
			Variables->Fixup();

			//
			// Output function name
			//
			os << ThisNode->OutputNewDefinition() << ";" << std::endl;

			Variables->KillLevel();

			break;

		case BAS_S_MAINFUNCTION:
			//
			// Handle any DEF*'s that are in there
			//
			if (ThisNode->Block[2] != 0)
			{
				ThisNode->Block[2]->OutputPrototypes(os);
			}
			break;

		}

		ThisNode = ThisNode->Block[0];
	}

	os << std::endl;
}

/**
 * \brief Output one block of code.
 *
 *	This function goes through the level 0 code (statements)
 *	and handle output of the commands at that level.
 */
void Node::OutputCodeOne(
	std::ostream& os	/**< iostream to write the C++ code to */
)
{
	VariableStruct *ThisVar;
	Node* LookDown;
	int KeepGosubFlag;
	int KeepOnErrorFlag;
	int KeepWhenErrorFlag;
	VARTYPE ThisType = VARTYPE_NONE;

	//
	// Handle this node type
	//
	switch (Type)
	{
	case BAS_N_ASSIGN:
		os << Indent();
		if (Tree[0] != 0)
		{
			//
			// Function return values are stashed in the
			// 'Result' variable.
			//
			if (Tree[0]->Type == BAS_V_FUNCTION)
			{
				os << "Result = " <<
					Tree[1]->Expression() <<
					";" << std::endl;
			}
			else
			{
				//
				// Is this an increment command?
				//
				if (Tree[1]->Type == '+')
				{
					if (Tree[0]->Expression() == Tree[1]->Tree[0]->Expression())
					{
						if (Tree[1]->Tree[1]->Expression() == "1")
						{
							os << Tree[0]->Expression() << "++;" <<
								std::endl;
						}
						else
						{
							os << Tree[0]->Expression() << " += " <<
								Tree[1]->Tree[1]->Expression() << ";" <<
								std::endl;
						}
						break;
					}
				}

				//
				// Is this an decrement command?
				//
				if (Tree[1]->Type == '-')
				{
					if (Tree[0]->Expression() == Tree[1]->Tree[0]->Expression())
					{
						if (Tree[1]->Tree[1]->Expression() == "1")
						{
							os << Tree[0]->Expression() << "--;" <<
								std::endl;
							break;
						}
// This part doesn't workright because
//	x = = x - y + z
//	is bit equal to
//	x -= y + z
//	it would beed ti be converted to
//	x -= y - z
//	also,it is paesed into
//	x = (x - y) + z
//						else
//						{
//							os << Tree[0]->Expression() << " -= " <<
//								Tree[1]->Tree[1]->Expression() << ";" <<
//								std::endl;
//						}
					}
				}


				//
				// A simple assignment statement
				//
				os << Tree[0]->Expression() <<
					" = " << Tree[1]->Expression() <<
					";" << std::endl;
			}
		}
		else
		{
			os << "Result = " <<
				Tree[1]->Expression() << ";" << std::endl;
		}
		break;

	case BAS_S_CALL:
		os << Indent() << Tree[0]->Expression() << ";" << std::endl;
		break;

	case BAS_N_CAUSEERROR:
		os << Indent() <<
			"OnErrorHit(" <<
			Tree[0]->Expression() <<
			", " << erl << ");" << std::endl;
		break;

	case BAS_P_ABORT:
		os << "#abort " << Tree[0]->Expression() << std::endl;
		break;

	case BAS_P_CDD:
		os << "#dictionary " << Tree[0]->Expression() << std::endl;
		break;

	case BAS_P_PRINT:
		os << "#message " << Tree[0]->Expression() << std::endl;
		break;

	case BAS_P_LET:
		os << "#define " <<
			Tree[0]->Tree[0]->Expression() << " " <<
			Tree[0]->Tree[1]->Expression() << std::endl;
		break;

	case BAS_S_CHAIN:
		os << Indent() << "basic::BasicChain(" <<
			Tree[0]->NoParen() << ");" << std::endl;
		break;

	case BAS_S_CHANGE1:
		//
		// Look for variable in table
		//
		ThisVar = Variables->Lookup(Tree[1]->TextValue,
			Tree[1]->Tree[0]);
		os << Indent() << "basic::BChange1(" <<
			Tree[0]->Expression() << ", " << ThisVar->GetName() <<
			");" << std::endl;
		break;

	case BAS_S_CHANGE2:
		ThisVar = Variables->Lookup(Tree[0]->TextValue,
			Tree[0]->Tree[0]);
		os << Indent() << "basic::BChange2(" <<
			ThisVar->GetName() << ", " << Tree[1]->Expression() <<
			");" << std::endl;
		break;

	case BAS_S_CLOSE:
		Tree[0]->OutputClose(os);
		break;

	case BAS_S_COMMON:
		OutputMap(os);
		break;

	case BAS_S_DATA:
		os << Indent() << "//" << std::endl <<
			Indent() << "// Data Statement" << std::endl <<
			Indent() << "//" << std::endl <<
			Indent() << "static const char* DataValue[] = {" <<
			std::endl;
		Level++;
		OutputData(os);
		os << ", NULL};" << std::endl;
		Level--;
		os << Indent() <<
			"static basic::DataListClass DataList(DataValue);" <<
			std::endl;
		break;

	case BAS_S_DECLARE:
		Tree[0]->OutputDefinitionList(os, Tree[1], Type);
		break;

	case BAS_V_DECLARECONSTANT:
		Tree[0]->OutputDefinitionList(os, 0, 0, 1);
		break;

	case BAS_V_DECLAREFUN:
		Tree[0]->OutputDefinitionList(os, 0, 0, 2);
		break;

	case BAS_S_DELETE:
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			".Delete();" << std::endl;
		break;

	case BAS_S_SCRATCH:
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			".Scratch();" << std::endl;
		break;

	case BAS_S_DIM:
		if (Tree[1] == 0)
		{
			Tree[0]->OutputDefinitionList(os, 0, 0);
		}
		else
		{
			Tree[0]->OutputVirtualList(os, Tree[1], 0, 0);
		}
		break;

	case BAS_P_ELSE:
		os << "#else" << std::endl;
		break;

	case BAS_P_ENDIF:
		os << "#endif" << std::endl;
		break;

	case BAS_S_ELSE:
	case BAS_S_USE:
	case BAS_S_END:
	case BAS_S_FNEND:
	case BAS_S_OPTION:
	case BAS_N_NULL:
		//
		// These nodes are not useful for code generation at
		// this point
		//
		break;

	case BAS_S_EXTERNAL:
		Tree[0]->OutputDefinitionList(os, 0, Type, 4);
		break;

	case BAS_N_EXTERNALSUB:
		Tree[0]->OutputDefinitionList(os, 0, 0, 2);
		break;

	case BAS_N_EXTERNALCONSTANT:
		Tree[0]->OutputDefinitionList(os, Tree[1], Type, 3);
		break;

	case BAS_N_EXTERNALFUNCTION:
		Tree[0]->OutputDefinitionList(os, 0, 0, 2);
		break;

	case BAS_S_EXIT:
		os << Indent() <<  "return;" << std::endl;
		break;

	case BAS_N_EXITHANDLER:
		// Re-throw the exception
		os << Indent() << "throw Be;" << std::endl;
		break;

	case BAS_S_FIELD:
		os << Indent() << "// Field Statement" << std::endl <<
			Indent() << "{" << std::endl;
		Level++;
		os << Indent() << "char* FieldBase = BasicChannel[" <<
			Tree[0]->OutputForcedType(VARTYPE_INTEGER) <<
			"].BufferLoc();" << std::endl;
		Tree[1]->OutputField(os);
		Level--;
		os << Indent() << "}" << std::endl;
		break;

	case BAS_S_FIND:
		if (Tree[1] != 0)
		{
			Tree[1]->OutputGetPutOptions(Tree[0], os);
		}
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			"].Find()" << std::endl;
		break;

	case BAS_S_FOR:
		os << Indent() << "for (";

		//
		// from
		//
		os << Tree[0]->Expression() << "; ";

		//
		// to
		//
		//	We have to try to look at the step and guess if
		//	it is a negitive step value. If we can't tell,
		//	assume positive.
		//
		if ((Tree[2] != 0) &&
			(Tree[2]->Type == BAS_N_UMINUS))
		{
			os << Tree[0]->Tree[0]->Expression() << " >= " <<
				Tree[1]->Expression() << "; ";
		}
		else
		{
			os << Tree[0]->Tree[0]->Expression() << " <= " <<
				Tree[1]->Expression() << "; ";
		}

		//
		// step
		//
		if (Tree[2] != 0)
		{
			//
			// Loop going backwards?
			//
			if (Tree[2]->Type == BAS_N_UMINUS)
			{
				os << Tree[0]->Tree[0]->Expression() << " -= " <<
					Tree[2]->Tree[0]->Expression();
			}
			else
			{
				os << Tree[0]->Tree[0]->Expression() << " += " <<
					Tree[2]->Expression();
			}
		}
		else
		{
			os << Tree[0]->Tree[0]->Expression() << "++";
		}

		os << ")" << std::endl;

		Block[1]->OutputBlock(os);

		break;

	case BAS_N_FORUNTIL:

		//
		// from
		//
		os << Indent() << "for (" << Tree[0]->NoParen() << "; ";

		//
		// until
		//
		os << "!(" << Tree[1]->NoParen() << ")" << "; ";

		//
		// step
		//
		os << Tree[0]->Tree[0]->Expression();
		if (Tree[2] == 0)
		{
			os << "++)" << std::endl;
		}
		else
		{
			os << "+= " << Tree[2]->NoParen() << ")" << std::endl;
		}

		Block[1]->OutputBlock(os);

		break;

	case BAS_N_FORWHILE:

		//
		// from
		//
		os << Indent() << "for (" << Tree[0]->Expression() << "; ";

		//
		// until
		//
		os << Tree[0]->Tree[0]->Expression() << "==" <<
			Tree[1]->Expression() << "; ";

		//
		// step
		//
		os << Tree[0]->Tree[0]->Expression() << "++)" << std::endl;

		Block[1]->OutputBlock(os);

		break;

	case BAS_S_FUNCTION:
	case BAS_S_DEF:
	case BAS_S_DEFSTAR:

		//
		// Empty variable list
		//
		KeepGosubFlag = GosubFlag;
		GosubFlag = 0;
		KeepOnErrorFlag = OnErrorFlag;
		KeepWhenErrorFlag = WhenErrorFlag;
		OnErrorFlag = 0;
		WhenErrorFlag = 0;

		Variables->NewLevel();

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
				ThisType, VARCLASS_FUNC, 1);

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

		//
		// Scan local code
		//
		if (Block[1] != 0)
		{
			Block[1]->VariableScan(0);
		}
		Variables->Fixup();

		//
		// Scan def* code
		//
		if (Block[2] != 0)
		{
			//
			// Output the DEF* code
			//
			Block[2]->OutputCode(os);
		}

		//
		// Dump out variable names if user asked for it
		//
		if (VariableDump)
		{
			Variables->Dump();
		}

		//
		// Output function name
		//
		os << std::endl <<
			Indent() << OutputNewDefinition() << std::endl;

		//
		// Braces starting function
		//
		os << Indent() << "{" << std::endl;
		Level++;

		//
		// Variable to hold result code
		//
		//	NOTE: Need to generate actual data type here.
		//
		if (Tree[0] != 0)
		{
			os << Indent() << Tree[0]->OutputNodeVarType() <<
				" Result;" << std::endl;
		}
		else
		{
			os << Indent() <<
				OutputVarType(GuessVarType(Tree[1]->TextValue)) <<
				" Result;" << std::endl;
		}

		//
		// Output local variables
		//
		if (Block[1] != 0)
		{
			Variables->OutputDef(os, Level);
			os << std::endl;

			if (GosubFlag != 0)
			{
				os << Indent() << "BStack(20);" << std::endl;
				GosubFlag = 0;
			}

			if (OnErrorFlag != 0)
			{
				os << Indent() << "OnErrorStack;" << std::endl;
//				os << Indent() << "basic::BasicError Be;" <<
//					std::endl;
				OnErrorFlag = 0;
			}
			if (WhenErrorFlag != 0)
			{
				WhenErrorFlag = 0;
			}

			Block[1]->OutputCode(os);
		}

		//
		// Give back a result code
		//
		os << Indent() << "return Result;" << std::endl;

		Level--;
		if (Type == BAS_S_DEF || Type ==BAS_S_DEFSTAR)
		{
			os << Indent() << "};" << std::endl;
		}
		else
		{
			os << Indent() << "}" << std::endl;
		}

		Variables->KillLevel();
		GosubFlag = KeepGosubFlag;
		OnErrorFlag = KeepOnErrorFlag;
		WhenErrorFlag = KeepWhenErrorFlag;

		break;

	case BAS_S_MAINFUNCTION:

		//
		// Empty variable list
		//
		KeepGosubFlag = GosubFlag;
		GosubFlag = 0;
		KeepOnErrorFlag = OnErrorFlag;
		KeepWhenErrorFlag = WhenErrorFlag;
		OnErrorFlag = 0;
		WhenErrorFlag = 0;

		Variables->NewLevel();

		//
		// Scan the call parameters
		//
		VariableScanOne(1);
		if (Block[1] != 0)
		{
			Block[1]->VariableScan(0);
		}
		Variables->Fixup();

		//
		// Scan for any local def* functions
		//
		if (Block[2] != 0)
		{
			Block[2]->OutputCode(os);
		}

		//
		// Dump out variable names if user asked for it
		//
		if (VariableDump)
		{
			Variables->Dump();
		}

		//
		// Output function name
		//
		os << std::endl;
		os << Indent() << "int main(int argc, char **argv)" <<
			std::endl;

		//
		// Braces starting function
		//
		os << Indent() << "{" << std::endl;
		Level++;

		//
		// Output local variables
		//
		if (Block[1] != 0)
		{
			Variables->OutputDef(os, Level);
			os << std::endl;

			if (GosubFlag != 0)
			{
				os << Indent() << "BStack(20);" << std::endl;
				GosubFlag = 0;
			}

			if (OnErrorFlag != 0)
			{
				os << Indent() << "OnErrorStack;" << std::endl;
//				os << Indent() << "basic::BasicError Be;" <<
//					std::endl;
				OnErrorFlag = 0;
			}
			if (WhenErrorFlag != 0)
			{
				WhenErrorFlag = 0;
			}

			Block[1]->OutputCode(os);
		}

		//
		// Give back a result code
		//
		os << std::endl << Indent() << "return EXIT_SUCCESS;" << std::endl;
		Level--;
		os << Indent() << "}" << std::endl;

		//
		// Clean up
		//
		Variables->KillLevel();
		GosubFlag = KeepGosubFlag;
		OnErrorFlag = KeepOnErrorFlag;
		WhenErrorFlag = KeepWhenErrorFlag;

		break;

	case BAS_S_FUNCTIONEXIT:
		os << Indent() << "return Result;" << std::endl;
		break;

	case BAS_S_PROGRAMEXIT:
		if (Tree[0] != 0)
		{
			os << Indent() << "exit(" <<
				Tree[0]->Expression() << ");" << std::endl;
		}
		else
		{
			os << Indent() << "exit(EXIT_SUCCESS);" << std::endl;
		}
		break;

	case BAS_S_GET:
		if (Tree[1] != 0)
		{
			Tree[1]->OutputGetPutOptions(Tree[0], os);
		}
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			".Get();" << std::endl;

		break;

	case BAS_S_GOSUB:
		os << Indent() << "BGosub(" << Tree[0]->Expression() << ");" << std::endl;
		break;

	case BAS_S_GOTO:
		os << Indent() << "goto " << Tree[0]->Expression() << ";" << std::endl;
		break;

	case BAS_S_CONTINUE:
		if (Tree[0] != 0)
		{
			os << Indent() << "goto " << Tree[0]->Expression() << ";" << std::endl;
		}
		else
		{
			os << Indent() << "continue; // clear error, exit handler" << std::endl;
		}
		break;

	case BAS_S_IF:
		os << Indent() << "if (" << Tree[0]->NoParen() << ")" << std::endl;

		if (Block[1] != 0)
		{
			Block[1]->OutputBlock(os);
		}
		else
		{
			os << Indent() << "{" <<std::endl;
			os << Indent() << "{" <<std::endl;
		}

		if (Block[2] != 0)
		{
			os << Indent() << "else" << std::endl;

			Block[2]->OutputBlock(os);
		}
		break;

	case BAS_P_IF:
		os << "#if (" << Tree[0]->NoParen() << ")" << std::endl;
		break;

	case BAS_N_WHENERRORIN:
		os << Indent() << "try" << std::endl;

		Block[1]->OutputBlock(os);

		if (Block[2] != 0)
		{
			os << Indent() << "catch(basic::BasicError &Be)" << std::endl;

			Block[2]->OutputBlock(os);
		}
		break;

	case BAS_P_INCLUDE:
		//
		// Don't output '#include' if we are also outputting the
		// code that was included.
		//
		if (CompileFlag == 0)
		{
			os << "#include " << Tree[0]->Expression();
		}
		else
		{
			os << "// #include " << Tree[0]->Expression();
		}

		//
		// How should this really be handled?
		//
		if (Tree[1] != 0)
		{
			os << " %library " << Tree[1]->Expression();
		}
		os << std::endl;

		break;

	case BAS_S_INPUT:
		OutputInput(os, 1);
		break;

	case BAS_S_ITERATE:
		os << Indent();
		if (Tree[0] == 0)
		{
			os << "continue;" << std::endl;
		}
		else
		{
			os << "goto " << Tree[0]->Expression() <<
				";\t// ??? Iterate" << std::endl;
		}
		break;

	case BAS_S_KILL:
		os << Indent() << "unlink(" <<
			Tree[0]->NoParen() << ");" << std::endl;
		break;

	case BAS_V_LABEL:
		if (isdigit(TextValue[0]))
		{
			erl = TextValue;
		}
		ThisVar = Variables->Lookup(TextValue, 0);
		if (ThisVar != 0)
		{
			if ((ThisVar->EverUsed != 0) || (KeepAllLines == true))
			{
				os << ThisVar->GetName(1) << ":;" << std::endl;
			}
		}
		else
		{
			os << genname(TextValue) << ":;" << std::endl;
		}
		break;

	case BAS_S_LET:
		os << Indent() << Tree[0]->Expression() << ";" << std::endl;
		break;

	case BAS_S_LINPUT:
		OutputInput(os, 2);
		break;

	case BAS_S_LSET:
		os << Indent() << "Lset(" <<
			Tree[0]->Tree[0]->Expression() << ", " <<
			Tree[0]->Tree[1]->Expression() << ");" << std::endl;
		break;

	case BAS_S_MAP:
		OutputMap(os);
		break;

	case BAS_S_MARGIN:
		os << Indent() << "basic::margin(";
		if (Tree[0] != 0)
		{
			os << Tree[0]->Expression();
		}
		os << ", " << Tree[1]->Expression() << ");" << std::endl;
		break;

	case BAS_S_MAT:
		switch(Tree[1]->Type)
		{
		case BAS_S_CON:     // MAT xxx = CON
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatCon(" << ThisVar->GetName() << ");" << std::endl;
			break;

		case BAS_S_IDN:
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatIdn(" << ThisVar->GetName() << ");" << std::endl;
			break;

		case BAS_S_ZER:
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			switch (ThisVar->ParCount)
			{
			case 1:
				os << Indent() << "MatZer_1(" <<
					ThisVar->GetName() <<
					", sizeof(" << ThisVar->GetName() <<
					"), sizeof(" << ThisVar->GetName() <<
					"[0]));" << std::endl;
				break;
			case 2:
				os << Indent() << "MatZer_2(" <<
					ThisVar->GetName() <<
					", sizeof(" << ThisVar->GetName() <<
					"), sizeof(" << ThisVar->GetName() <<
					"[0]));" <<
					"), sizeof(" << ThisVar->GetName() <<
					"[0][0]));" << std::endl;
				break;
			default:
				os << Indent() << "MatZer_n(" <<
					ThisVar->GetName() <<
					", sizeof(" << ThisVar->GetName() <<
					"));" << std::endl;
				break;
			}
			break;

		case BAS_S_TRN:		// MAT xxx = TRN(yyy)
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatTrn(" << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->TextValue, Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

		case BAS_S_INV:		// MAT xx = INV(yyy)
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatInv(" << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->TextValue, Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

        case '+':			// MAT xxx = yyy + zzz
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatPlus(" << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[0]->TextValue,
				Tree[1]->Tree[0]->Tree[0]);
			os << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[1]->TextValue,
				Tree[1]->Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

        case '-':
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatMinus(" << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[0]->TextValue,
				Tree[1]->Tree[0]->Tree[0]);
			os << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[1]->TextValue,
				Tree[1]->Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

        case '*':
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatTimes(" << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[0]->TextValue,
				Tree[1]->Tree[0]->Tree[0]);
			os << ThisVar->GetName() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[1]->TextValue,
				Tree[1]->Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

        case '(':
			ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[0]->Tree[0]);
			os << Indent() << "MatScalar(" << ThisVar->GetName() << ", " <<
				Tree[1]->Tree[0]->Expression() << ", ";
			ThisVar = Variables->Lookup(Tree[1]->Tree[1]->TextValue,
				Tree[1]->Tree[1]->Tree[0]);
			os << ThisVar->GetName() << ");" << std::endl;
			break;

		}
		break;

	case BAS_N_ONERROR:
		os << Indent();
		if (Tree[0] != 0)
		{
			if (atoi(Tree[0]->TextValue.c_str()) == 0)
			{
				os << "OnErrorZero;" << std::endl;
			}
			else
			{
				os << "OnErrorGoto(" << Tree[0]->Expression() << ");" << std::endl;
			}
		}
		else
		{
			os << "OnErrorZero;" << std::endl;
		}

		break;

	case BAS_N_ONGOTO:
		os << Indent() << "switch(" <<
			Tree[0]->OutputForcedType(VARTYPE_INTEGER) <<
			")" << std::endl << Indent() << "{" << std::endl;
		Tree[1]->OutputOngo(os, "goto", 1);
		if (Tree[2] != 0)
		{
				os << Indent() << "default:" << std::endl <<
					Indent() << "\tgoto " << Tree[2]->OutputLabel() <<
					";" << std::endl;
		}
		else
		{
				os << Indent() << "default:" << std::endl <<
					Indent() <<
					"\tstd::cerr << \"ON-GOTO out of range\" << std::endl; abort();\t// Out of range" << std::endl;
		}
		os << Indent() << "}" << std::endl;
		break;

	case BAS_N_ONGOSUB:
		os << Indent() << "switch(" <<
			Tree[0]->OutputForcedType(VARTYPE_INTEGER) <<
			")" << std::endl << Indent() << "{" << std::endl;
		Tree[1]->OutputOngo(os, "BGosub", 1);
		if (Tree[2] != 0)
		{
				os << Indent() << "default:" << std::endl <<
					Indent() << "\tBGosub(" <<
					Tree[2]->OutputLabel() << ");" << std::endl;
		}
		else
		{
			os << Indent() << "default:" << std::endl <<
				Indent() <<
				"OnErrorHit(58, " <<
				erl << ");\t// Out of range" << std::endl;
		}
		os << Indent() << "}" << std::endl;
		break;

	case BAS_S_OPEN:
		os << Indent() <<
			GetIPChannel(Tree[2], 0) <<
			".open(";
		if (Tree[0] != 0)
		{
			os << Tree[0]->Expression();
		}
		//
		// For INPUT/OUTPUT
		//
		if (Tree[1] != 0)
		{
			if (Tree[1]->Type == BAS_S_INPUT)
			{
				os << ", std::ios_base::in";
			}
			else
			{
				os << ", std::ios_base::out";
			}

		}

		os << ");" << std::endl;

		//
		// Other flags set on open
		//
		if (Tree[3] != 0)
		{
			Tree[3]->OutputOpenStuff(os, Tree[2]);
		}

		//
		// Throw an error if file doesn't open
		//
		os << Indent() <<
			"if (!" <<
			GetIPChannel(Tree[2], 0) <<
			".is_open()) { OnErrorHit(5, " <<
			erl << "); }" <<
			std::endl;;
		break;

	case BAS_S_PRINT:
		OutputPrint(os);
		break;

	case BAS_S_PUT:
		if (Tree[1] != 0)
		{
			Tree[1]->OutputGetPutOptions(Tree[0], os);
		}
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			".Put();" << std::endl;
		break;

	case BAS_S_RANDOM:
		os << Indent() << "srand(time(0));" << std::endl;
		break;

	case BAS_S_RECORD:
		os << Indent() << "struct ";
		if (Tree[0] != 0)
		{
			os << genname(Tree[0]->Expression());
		}
		os << std::endl << Indent() << "{" << std::endl;

		if (Block[1] != 0)
		{
			Level++;
			Block[1]->OutputDefinitionList(os, 0, 0);
			Level--;
		}

		os << Indent() << "};" << std::endl;

		break;

	case BAS_S_RESTORE:
	case BAS_S_RESET:
		if (Tree[0] == 0)
		{
			os << Indent() << "DataList.Reset();" << std::endl;
		}
		else
		{
			if (Tree[1] != 0)
			{
				Tree[1]->OutputGetPutOptions(Tree[0], os);
			}
			os << Indent() << GetIPChannel(Tree[0], 0) <<
				"].seekg(0, std::ios::beg);" << std::endl;
		}
		break;

	case BAS_S_READ:
		os << Indent() << "DataList.Read(" << Tree[0]->Expression() <<
			");" << std::endl;
		break;

	case BAS_S_RSET:
		os << Indent() << "Rset(" <<
			Tree[0]->Tree[0]->Expression() << ", " <<
			Tree[0]->Tree[1]->Expression() << ");" << std::endl;
		break;

	case BAS_S_RESUME:
		os << Indent();
		if (Tree[0] != 0)
		{
			os << "OnErrorZero;" << std::endl <<
				Indent() << "goto " << Tree[0]->Expression() << ";" << std::endl;
		}
		else
		{
			os << "OnResume;" << std::endl;
		}
		break;

	case BAS_S_RETRY:
// FIXME: How to do a RETRY in C++
		os << Indent() << "retry;" << std::endl;
		break;

	case BAS_S_RETURN:
		os << Indent() << "BReturn;" << std::endl << std::endl;
		break;

	case BAS_S_SELECT:
		if (Block[1] != 0 && Block[1]->CheckCaseLabel())
		{
			os << Indent() <<
				"// ** Converted from a select statement **" <<
				std::endl;

			//
			// We create a new variable for the tests so that we
			// don't recalculate the expression multiple times,
			// which can cause problems for function calls with
			// side effects.
			//
			os << Indent() << "{" << std::endl;
			Level++;
			os << Indent() << "auto TempS = " <<
				Tree[0]->Expression() << ";" << std::endl;

			LookDown = Block[1];
			int ifelse = 0;

			while(LookDown)
			{
				switch(LookDown->Type)
				{
				case BAS_S_REMARK:
					os << Indent();
					LookDown->OutputRemark(os);
					break;

				case BAS_S_CASE:

					os << Indent();

					if (ifelse)
					{
						os << "else ";
					}
					ifelse = 1;

					os << "if (";
					LookDown->Tree[0]->OutputCaseIf(os, Tree[0]);
					os << ")" << std::endl;
					break;

				default:
					os << Indent() << "else" << std::endl;
					break;
				}

				if (LookDown->Block[1] != 0)
				{
					LookDown->Block[1]->OutputBlock(os);
				}
				else if (LookDown->Type != BAS_S_REMARK)
				{
					//
					// Empty case, Create a block anyway.
					//
					os << Indent() << "{" << std::endl;
					os << Indent() << "}" << std::endl;
				}
				LookDown = LookDown->Block[0];
			}
			Level--;
			os << Indent() << "}" << std::endl;
		}
		else
		{
			os << Indent() << "switch (" <<
				Tree[0]->OutputForcedType(VARTYPE_INTEGER) <<
				")" << std::endl << Indent() << "{" << std::endl;

			LookDown = Block[1];

			while(LookDown)
			{
				int isremark = 0;

				switch(LookDown->Type)
				{
				case BAS_S_CASE:
					if (LookDown->Tree[0] != 0)
					{
						LookDown->Tree[0]->OutputCaseLabel(os);
						os << std::endl;
					}
					else
					{
						Level++;
						os << Indent() << "case:\t// Empty case label" <<
							std::endl << std::endl;
					}
					break;


				case BAS_N_CASEELSE:
					os << Indent() << "default:" << std::endl;
					break;

				case BAS_S_REMARK:
					os << Indent();
					LookDown->OutputRemark(os);
					isremark = 1;
					break;

				}

				//
				// Output any info under case statement
				//
				if (isremark == 0)
				{
					Level++;
					if (LookDown->Block[1] != 0)
					{
						LookDown->Block[1]->OutputCode(os);
					}
					os << Indent() << "break;" <<
						std::endl << std::endl;
					Level--;
				}

				LookDown = LookDown->Block[0];
			}

			os << Indent() << "}" << std::endl;
		}

		break;

	case BAS_S_SET:
		if (Tree[0]->Type == BAS_S_NO)
		{
			os << Indent() << "// set no prompt" << std::endl;
		}
		else
		{
			os << Indent() << "// set prompt" << std::endl;
		}
		break;

	case BAS_S_STOP:
		os << Indent() << "abort();" << std::endl;
		break;

	case BAS_S_SUB:
	case BAS_S_PROGRAM:

		//
		// Empty variable list
		//
		KeepGosubFlag = GosubFlag;
		GosubFlag = 0;
		KeepOnErrorFlag = OnErrorFlag;
		KeepWhenErrorFlag = WhenErrorFlag;
		OnErrorFlag = 0;
		WhenErrorFlag = 0;

		Variables->NewLevel();

		//
		// Scan the call parameters
		//
		VariableScanOne(1);

		//
		// Scan local code
		//
		if (Block[1] != 0)
		{
			Block[1]->VariableScan(0);
		}
		Variables->Fixup();

		//
		// Scan def* code
		//
		if (Block[2] != 0)
		{
			//
			// Output the DEF* code
			//
			Block[2]->OutputCode(os);
		}

		//
		// Dump out variable names if user asked for it
		//
		if (VariableDump)
		{
			Variables->Dump();
		}

		//
		// Output function name
		//
		os << std::endl << OutputNewDefinition() << std::endl;

		//
		// Braces starting function
		//
		os << Indent() << "{" << std::endl;
		Level++;

		//
		// Output local variables
		//
		if (Block[1] != 0)
		{
			Variables->OutputDef(os, Level);
			os << std::endl;

			if (GosubFlag != 0)
			{
				os << Indent() << "BStack(20);" << std::endl;
				GosubFlag = 0;
			}

			if (OnErrorFlag != 0)
			{
				os << Indent() << "OnErrorStack;" << std::endl;
//				os << Indent() << "basic::BasicError Be;" <<
//					std::endl;
				OnErrorFlag = 0;
			}
			if (WhenErrorFlag != 0)
			{
				WhenErrorFlag = 0;
			}

			Block[1]->OutputCode(os);
		}

		Level--;
		os << Indent() << "}" << std::endl;

		Variables->KillLevel();
		GosubFlag = KeepGosubFlag;
		OnErrorFlag = KeepOnErrorFlag;
		WhenErrorFlag = KeepWhenErrorFlag;

		break;

	case BAS_S_HANDLER:

		//
		// Empty variable list
		//
		KeepGosubFlag = GosubFlag;
		GosubFlag = 0;
		KeepOnErrorFlag = OnErrorFlag;
		KeepWhenErrorFlag = WhenErrorFlag;
		OnErrorFlag = 0;
		WhenErrorFlag = 0;

		Variables->NewLevel();

		//
		// Scan the call parameters
		//
		VariableScanOne(1);

		//
		// Scan local code
		//
		if (Block[1] != 0)
		{
			Block[1]->VariableScan(0);
		}
		Variables->Fixup();

		//
		// Scan def* code
		//
		if (Block[2] != 0)
		{
			//
			// Output the DEF* code
			//
			Block[2]->OutputCode(os);
		}

		//
		// Dump out variable names if user asked for it
		//
		if (VariableDump)
		{
			Variables->Dump();
		}

		//
		// Output function name
		//
		os << std::endl << OutputNewDefinition() << std::endl;

		//
		// Braces starting function
		//
		os << Indent() << "{" << std::endl;
		Level++;

		//
		// Output local variables
		//
		if (Block[1] != 0)
		{
			Variables->OutputDef(os, Level);
			os << std::endl;

			if (GosubFlag != 0)
			{
				os << Indent() << "BStack(20);" << std::endl;
				GosubFlag = 0;
			}

			if (OnErrorFlag != 0)
			{
				os << Indent() << "OnErrorStack;" << std::endl;
//				os << Indent() << "basic::BasicError Be;" <<
//					std::endl;
				OnErrorFlag = 0;
			}
			if (WhenErrorFlag != 0)
			{
				WhenErrorFlag = 0;
			}

			Block[1]->OutputCode(os);
		}

		Level--;
		os << Indent() << "}" << std::endl;

		Variables->KillLevel();
		GosubFlag = KeepGosubFlag;
		OnErrorFlag = KeepOnErrorFlag;
		WhenErrorFlag = KeepWhenErrorFlag;

		break;

	case BAS_S_PROGRAMEND:
		if (Tree[0] != 0)
		{
			//
			// Only need this if exit value is given
			//
			os << Indent() <<  "exit(" <<
				Tree[0]->Expression() << ");" << std::endl;
		}
		break;

	case BAS_S_SUBEXIT:
		if (Tree[0] == 0)
		{
			os << Indent() <<  "return;" << std::endl;
		}
		else
		{
			os << Indent() <<  "return " <<
				Tree[0]->Expression() << ";" << std::endl;
		}
		break;

	case BAS_P_TITLE:
	case BAS_P_SBTTL:
	case BAS_P_IDENT:
	case BAS_S_REMARK:
		os << Indent();
		OutputRemark(os);
		break;

	case BAS_S_TO:
		//
		// TO used in a case statement
		//
		os << Tree[0]->Expression() << " ... " << Tree[1]->Expression();
		break;

	case BAS_S_UNLESS:
		os << Indent();
		LookDown = Tree[0];
		while(LookDown->Type == '(')
		{
			LookDown = LookDown->Tree[0];
		}

		//
		// Try to generate the unless as the opposite 'if' if
		// possible, otherwise just create an ungly not ('!').
		//
		switch(LookDown->Type)
		{
		case '=':
			os << "if (" << LookDown->Tree[0]->Expression() << " != " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case '>':
			os << "if (" << LookDown->Tree[0]->Expression() << " <= " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case '<':
			os << "if (" << LookDown->Tree[0]->Expression() << " >= " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case BAS_X_NEQ:
			os << "if (" << LookDown->Tree[0]->Expression() << " == " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case BAS_X_GE:
			os << "if (" << LookDown->Tree[0]->Expression() << " < " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case BAS_X_LE:
			os << "if (" << LookDown->Tree[0]->Expression() << " > " <<
				LookDown->Tree[1]->Expression() << ")" << std::endl;
			break;

		case BAS_X_LNOT:
			os << "if (" << LookDown->Tree[0]->Expression() << ")" << std::endl;
			break;

		default:
			os << "if (!(" << LookDown->Expression() << "))" << std::endl;
			break;
		}

		Block[1]->OutputBlock(os);

		break;

	case BAS_S_UNLOCK:
		os << Indent() <<
			GetIPChannel(Tree[0], 0) <<
			".Unlock();" << std::endl;
		break;

	case BAS_S_UNTIL:
		os << Indent() << "while (!(" << Tree[0]->NoParen() <<
			"))" << std::endl;

		Block[1]->OutputBlock(os);

		break;

	case BAS_S_UPDATE:
		os << Indent() << "RmsUpdate(" << Tree[0]->NoParen() <<
			");" << std::endl;
		break;

	case BAS_S_WAIT:
	case BAS_S_SLEEP:
		os << Indent() << "sleep(" << Tree[0]->NoParen() <<
			");" << std::endl;
		break;

	case BAS_S_WHILE:
		os << Indent() << "while (" << Tree[0]->NoParen() <<
			")" << std::endl;

		Block[1]->OutputBlock(os);
		break;

	default:
		os << "??%" << Type << " @" << lineno << ":" <<
			TextValue << "(";
		if (Tree[0] != 0)
		{
			os << Tree[0];
		}
		os << ",";
		if (Tree[1] != 0)
		{
			os << Tree[1];
		}
		os << ",";
		if (Tree[2] != 0)
		{
			os << Tree[2];
		}
		os << ",";
		if (Tree[3] != 0)
		{
			os << Tree[3];
		}
		os << ")" << std::endl;
		break;
	}
}


/**
 * \brief Generates string value of an exporeesion.
 *
 *	This function will convert an expression into a string, ready
 *	for output.
 *
 * \bug This used to be part of OutputCodeOne, but has been broken out to
 *	make understanding of the code easier, and to return a string
 *	value instead of outputing to a channel.
 */
std::string Node::Expression(void)
{
	std::string result;
	VariableStruct *ThisVar;

	//
	// Handle this node type
	//
	switch (Type)
	{
	case BAS_N_ASSIGNLIST:
		if (Tree[0]->Type == BAS_V_FUNCTION)
		{
			result = "Result = ";
		}
		else
		{
			result = Tree[0]->Expression() + " = ";
		}

		if (Tree[1]->Type == BAS_V_FUNCTION)
		{
			result += "Result";
		}
		else
		{
			result += Tree[1]->Expression();
		}
		break;

	case BAS_V_USELABEL:
		result = OutputLabel();
		break;

	case BAS_V_FUNCTION:
		//
		// Look for variable in table
		//
		ThisVar = Variables->Lookup(TextValue, Tree[0]);

		//
		// Print this variable
		//
		if (ThisVar != 0)
		{
			if (ThisVar->GetName() == "Tab")
			{
				/* \note Tab is not easy to handle on
				 * systems not designed to know where
				 * the printhead is currently sitting.
				 * Here we are just using spaces and
				 * assuming we are spacing from the left
				 * margin.
				 */
				result = std::string("std::string(") +
					Tree[0]->NoParen() +
					", ' ') /* Tab(" + 
					Tree[0]->NoParen() + ") */";
				break;
			}
			else
			{
				result = ThisVar->GetName();
			}
		}
		else
		{
			result = genname(TextValue);
		}

		if (result == "basic::strlen")
		{
			result = Tree[0]->Paren() + ".size()";
		}
		else if (result == "basic::Qspace")
		{
			result = std::string("std::string(") +
				Tree[0]->NoParen() + ", ' ')";
		}
		else if (result == "basic::Ascii")
		{
			result = Tree[0]->Paren() + "[0]";
		}
		else if (result == "basic::Char")
		{
			result = "std::string(1, (char)" + Tree[0]->Paren() + ")";
		}
		else if (result == "str$find_first_in_set" && Tree[0]->Type == BAS_N_LIST)
		{
			result = Tree[0]->Tree[0]->ParenString() +
				".find_first_of(" + 
				Tree[0]->Tree[1]->NoParen() + ") + 1";
		}
		else if (result == "basic::left" && Tree[0]->Type == BAS_N_LIST)
		{
			result = Tree[0]->Tree[0]->ParenString() +
				".substr(0, " + 
				Tree[0]->Tree[1]->NoParen() + ")";
		}

#if 0		// Doesn't work because substr bast end of string causes crash

		else if (result == "basic::Qseg" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->Type == BAS_N_LIST)
		{
			result = Tree[0]->Tree[0]->Paren() +
				".substr(" +
				Tree[0]->Tree[1]->Tree[0]->NoParen() +
				", " +
				Tree[0]->Tree[1]->Tree[1]->NoParen() +
				" - " +
				Tree[0]->Tree[1]->Tree[0]->Paren() +
				")";
;
		}
		else if (result == "basic::right" && Tree[0]->Type == BAS_N_LIST)
		{
			result = Tree[0]->Tree[0]->ParenString() + ".substr(" + 
				IncrString(Tree[0]->Tree[1]->NoParen(), -1) + ")";
		}
		else if (result == "basic::mid" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->Type == BAS_N_LIST)
		{
			result = Tree[0]->Tree[0]->ParenString() + ".substr(" + 
				IncrString(Tree[0]->Tree[1]->Tree[0]->NoParen(), -1) + ", " +
				Tree[0]->Tree[1]->Tree[1]->NoParen() + ")";
		}
#endif
		else if (result == "basic::instr" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->Type == BAS_N_LIST)
		{
			std::string temp = IncrString(Tree[0]->Tree[0]->NoParen(), -1);
			result = std::string("(") +
				Tree[0]->Tree[1]->Tree[0]->ParenString() + ".find(" + 
				Tree[0]->Tree[1]->Tree[1]->NoParen() + ", " +
				temp + ") + 1)";
		}
		//     EDIT$(str-exp, int-exp)
		//
		//     Values   Effect
		//
		//     1%       Trim parity bits
		//     2%       Discard all spaces and tabs
		//     4%       Discard characters: CR, LF, FF, ESC, RUBOUT, and NULL
		//     8%       Discard leading spaces and tabs
		//     16%      Reduce spaces and tabs to one space
		//     32%      Convert lowercase to uppercase
		//     64%      Convert [ to ( and ] to )
		//     128%     Discard trailing spaces and tabs
		//     256%     Do not alter characters inside quotes
		//
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "2")
		{
			result = std::string("boost::erase_all_copy(") +
				Tree[0]->Tree[0]->NoParen() + ", \" \")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "4")
		{
			result = std::string("boost::replace_regex_all_copy(") +
				Tree[0]->Tree[0]->NoParen() +
				", \"[\\n\\r\\f\\0x1a\\0xff]\", \"\")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "8")
		{
			result = std::string("boost::trim_left_copy(") +
				Tree[0]->Tree[0]->NoParen() + ")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "8 + 128")
		{
			result = std::string("boost::trim_copy(") +
				Tree[0]->Tree[0]->NoParen() + ")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "16")
		{
			result = std::string("boost::replace_regex_all_copy(") +
				Tree[0]->Tree[0]->NoParen() +
				", \"[ \\t]+\", \" \")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "32")
		{
			result = std::string("boost::to_upper_copy(") +
				Tree[0]->Tree[0]->NoParen() + ")";
		}
		else if (result == "basic::edit" &&
			Tree[0]->Type == BAS_N_LIST &&
			Tree[0]->Tree[1]->NoParen() == "128")
		{
			result = std::string("boost::trim_right_copy(") +
				Tree[0]->Tree[0]->NoParen() + ")";
		}
		else
		{
			if (Tree[0] != 0)
			{
				result += "(" + Tree[0]->NoParen() + ")";
			}
			else
			{
				result += "()";
			}
		}
		break;

	case BAS_N_FORASSIGN:
		result = Tree[0]->Expression() + " = " + Tree[1]->Expression();
		break;

	case BAS_X_STRREF:
		ThisVar = Variables->Lookup(TextValue, Tree[0]);
		if (ThisVar != 0 && ThisVar->Prefix != "")
		{
			result = ThisVar->Prefix + "." +
				Tree[0]->Expression() + "." +
				Tree[1]->Expression();
		}
		else
		{
			result = Tree[0]->Expression() + "." +
				Tree[1]->Expression();
		}
		break;

	case BAS_N_STRUCTNAME:
		result = genname(TextValue);
		if (Tree[0] != 0)
		{
			Node *array = Tree[0];
			while(array != 0)
			{
				result += "[" + array->OutputArrayParam(false) + "]";
				array = array->Block[0];
			}
		}
		break;

	case BAS_N_NULL:
		result = "NULL";
		break;

	//
	// I/O Channel
	//
	case '#':
		result = Tree[0]->OutputForcedType(VARTYPE_INTEGER);
		break;

	//
	// Equality statement
	//
	case '=':
		result = Tree[0]->OutputBstringText() + " == " +
			Tree[1]->Expression();
		break;

	case '+':
	case '-':
	case '*':
	case '/':
	case '>':
	case '<':
		result = Tree[0]->OutputBstringText() + " " + TextValue +
			" " + Tree[1]->Expression();
		break;

	case BAS_S_MOD:
		result = Tree[0]->Expression() + " % " + Tree[1]->Expression();
		break;

	case '(':
		//
		// Lose unecessary additional levels of pren's
		//
		result = std::string("(") + Tree[0]->NoParen() + ")";
		break;

	case '^':
		result = std::string("pow(") + Tree[0]->NoParen() + ", " +
			Tree[1]->NoParen() + ")";
		break;

	case BAS_X_LAND:
		result = Tree[0]->Expression() + " && " + Tree[1]->Expression();
		break;

	case BAS_S_AND:
		result = Tree[0]->Expression() + " & " + Tree[1]->Expression();
		break;

	case BAS_S_BY:
		switch(Tree[1]->Type)
		{
		case BAS_S_DESC:
			result = std::string("$descr(") + Tree[0]->Expression() + ")";
			break;

		case BAS_S_REF:
			result = std::string("&") + Tree[0]->Expression();
			break;

		default:
			result = Tree[0]->Expression();
			break;
		}
		break;

	case BAS_S_EQV:
		result = std::string("basic::BasicEqv( ") + Tree[0]->Expression() + ", " +
			Tree[1]->Expression() + ")";
		break;

	case BAS_S_IMP:
		result = std::string("basic::BasicImp( ") + Tree[0]->Expression() + ", " +
			Tree[1]->Expression() + ")";
		break;

	case BAS_X_GE:
		result = Tree[0]->Expression() + " >= " + Tree[1]->Expression();
		break;

	case BAS_V_INT:
	case BAS_V_INTEGER:
	case BAS_V_FLOAT:
		result = OutputInteger();
		break;

	case BAS_X_LE:
		result = Tree[0]->Expression() + " <= " + Tree[1]->Expression();
		break;

	case BAS_N_LIST:
		if (Tree[0] == 0)
		{
			result = std::string("0") + ", ";
		}
		else
		{
			result = Tree[0]->Expression() + ", ";
		}

		if (Tree[1] == 0)
		{
			result += "0";
		}
		else
		{
			result += Tree[1]->Expression();
		}
		break;

	case BAS_V_NAME:
		result = OutputVarName(Tree[0]);
		break;

	case BAS_P_NAME:
		result = OutputPName(Tree[0]);
		break;

	case BAS_X_EQ:
		result = Tree[0]->Expression() + " == " + Tree[1]->Expression();
		break;

	case BAS_X_NEQ:
		result = Tree[0]->Expression() + " != " + Tree[1]->Expression();
		break;

	case BAS_S_NOT:
		result = std::string("~") + Tree[0]->Expression();
		break;

	case BAS_X_LNOT:
		result = std::string("!") + Tree[0]->Expression();
		break;

	case BAS_X_LOR:
		result = Tree[0]->Expression() + " || " + Tree[1]->Expression();
		break;

	case BAS_S_OR:
		result = Tree[0]->Expression() + " | " + Tree[1]->Expression();
		break;

	case BAS_V_PREDEF:
		result = std::string("basic::") + genname(TextValue);
		if (result == "basic::bel")
		{
			result = "\"\\g\"";
		}
		else if (result == "basic::bs")
		{
			result = "\"\\h\"";
		}
		else if (result == "basic::cr")
		{
			result = "\"\\r\"";
		}
		else if (result == "basic::del")
		{
			result = "\"\\0xff\"";
		}
		else if (result == "basic::esc")
		{
			result = "\"\\e\"";
		}
		else if (result == "basic::ff")
		{
			result = "\"\\f\"";
		}
		else if (result == "basic::ht")
		{
			result = "\"\\i\"";
		}
		else if (result == "basic::lf")
		{
			result = "\"\\n\"";
		}
		else if (result == "basic::pi")
		{
			result = "PI";
		}
		else if (result == "basic::si")
		{
			result = "\"\\0x17\"";
		}
		else if (result == "basic::so")
		{
			result = "\"\\0x16\"";
		}
		else if (result == "basic::sp")
		{
			result = " ";
		}
		else if (result == "basic::vt")
		{
			result = "\"\\f\"";
		}
		break;

	case BAS_V_TEXTSTRING:
		result = TextValue;
		break;

	case BAS_N_TYPEFUN:
		result = std::string("(") + Tree[1]->OutputNodeVarType() +
			")(" + Tree[0]->Expression() + ")";
		break;

	case BAS_N_UMINUS:
		result =  std::string("-") + Tree[0]->Expression();
		break;

	case BAS_N_UPLUS:
		result = std::string("+") + Tree[0]->Expression();
		break;

	case BAS_X_UNARYGT:
		result = std::string("> ") + Tree[0]->Expression();
		break;

	case BAS_X_UNARYGE:
		result = std::string(">= ") + Tree[0]->Expression();
		break;

	case BAS_X_UNARYLE:
		result = std::string("<= ") + Tree[0]->Expression();
		break;

	case BAS_X_UNARYLT:
		result = std::string("< ") + Tree[0]->Expression();
		break;

	case BAS_X_UNARYNEQ:
		result = std::string("!= ") + Tree[0]->Expression();
		break;

	case BAS_S_XOR:
		result = Tree[0]->Expression() + " ^ " + Tree[1]->Expression();
		break;

	case BAS_S_STRING:
		result += OutputVarType(VARTYPE_DYNSTR);
		break;

	case BAS_S_INTEGER:
		result += OutputVarType(VARTYPE_INTEGER);
		break;

	case BAS_S_WORD:
		result += OutputVarType(VARTYPE_WORD);
		break;

	case BAS_S_LONG:
		result += OutputVarType(VARTYPE_LONG);
		break;

	case BAS_S_REAL:
		result += OutputVarType(VARTYPE_REAL);
		break;

	case BAS_S_DOUBLE:
		result += OutputVarType(VARTYPE_DOUBLE);
		break;

	case BAS_S_BYTE:
		result += OutputVarType(VARTYPE_BYTE);
		break;

	case BAS_S_RFA:
		result += OutputVarType(VARTYPE_RFA);
		break;

	case BAS_S_DECIMAL:
		result += OutputVarType(VARTYPE_DECIMAL);
		break;

	case BAS_S_SINGLE:
		result += OutputVarType(VARTYPE_SINGLE);
		break;

	case BAS_S_GFLOAT:
		result += OutputVarType(VARTYPE_GFLOAT);
		break;

	case BAS_S_HFLOAT:
		result += OutputVarType(VARTYPE_HFLOAT);
		break;

	default:
		{
			// FIXME: There must be a better way
			char buffer[32];
			sprintf(buffer, "%d@%d", Type, lineno);
			result = std::string("[Unknown Node Type ") + buffer + "]";
		}
		break;
	}

	return result;
}

/**
 * \brief Outputs Variable Name
 *
 *	Creates the full name for a variable
 */
std::string Node::OutputVarName(
	Node* array,		/**< Pointer to any array references */
	int InDefine		/**< Is this part of a definition,
       					bit 1 = Yes, bit2 = Don't add array dims*/
)
{
	int flags = 0;		// GetName flags

	if (InDefine & 1)
	{
		//
		// We don't want the prefix on the definition, since the
		// class name is already handled by the 'class' definition.
		//
		flags |= 1;
	}

	//
	// Local Variables
	//
	VariableStruct *ThisVar;
	std::string result;

	//
	// Look for variable in table
	//
	ThisVar = Variables->Lookup(TextValue, array);

	//
	// Make up a good name to print for this variable
	//
	if (ThisVar != 0)
	{
		result = ThisVar->GetName(flags);
	}
	else
	{
		result = genname(TextValue);
	}

	//
	// Handle array references (May need to be modified from x[a,b]
	// to x[a][b] manually.
	//
	if (array != 0 && (InDefine & 2) == 0)
	{
		if ((ThisVar != 0) && (ThisVar->Class == VARCLASS_FUNC))
		{
			result += "(" + array->NoParen() + ")";
		}
		else
		{
			while(array != 0)
			{
				result += "[" + array->OutputArrayParam(InDefine) + "]";
				array = array->Block[0];
			}
		}
	}

	return result;
}

/**
 * \brief Outputs preprocessor Variable Name
 *
 *	Creates the full name for a %variable defined by the
 *	preprocessor routines.
 *	These variables start with a '%' in BASIC, and are only used
 *	in the preprocessor statements, lile '%LET', '%IF', etc.
 */
std::string Node::OutputPName(
	Node* array,		/**< Pointer to any array references */
	int InDefine		/**< Is this part of a definition */
)
{
	std::string result;

	for (int loop = 0; loop < TextValue.size(); loop++)
	{
		switch (TextValue[loop])
		{
		case '%':	// Ignore
			break;

		case '.':	// Periods not allowed
			result += '_';
			break;

		default:
			result += toupper(TextValue[loop]);
			break;
		}
	}


	return result;
}

/**
 * \brief Outputs a print statement.
 *
 *	This function will mangle a print statement.
 */
void Node::OutputPrint(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	int ReturnFlag = 0;

	//
	// Indent as necessary
	//
	IOChPrinted = 0;	// IO Channel not yet output
	IOChannel = 0;		// No IO Channel defined yet
	IOUsing = 0;		// No Print Using Statement yet

	//
	// Output print using
	//
	if (Tree[0] != 0)
	{
		//
		// Handle print statement
		//
		ReturnFlag = Tree[0]->OutputPrintData(os, 0);

		//
		// Finish off print using statement
		//
		if (IOUsing != 0)
		{
			OutputIPChannel(os, 0);
			os << " << PUse.Finish()";
		}

		//
		// Force a return at end of line?
		//
		if (ReturnFlag != 0)
		{
			if ((IOChannel == 0) || (IOChPrinted != 0))
			{
				OutputIPChannel(os, 0);
				os << " << std::endl";
			}
			else
			{
				//
				// When using a BChannel, it doesn't handle this
				// case properly in C++, so fake it.
				//
				OutputIPChannel(os, 0);
				os << " << std::endl";
			}
		}
	}
	else
	{
		//
		// Empty print command
		//
		if ((IOChannel == 0) || (IOChPrinted != 0))
		{
			OutputIPChannel(os, 0);
			os << " << std::endl";
		}
		else
		{
			//
			// When using a BChannel, it doesn't handle this
			// case properly in C++, so fake it.
			//
			OutputIPChannel(os, 0);
			os << " << std::endl";
		}
	}

	//
	// Close off statement
	//
	if (IOChPrinted != 0)
	{
		os << ";" << std::endl;
	}
}

/**
 * \brief Output IO channel information
 */
void Node::OutputIPChannel(
	std::ostream& os,	/**< iostream to write C++ code to */
	int InputFlag		/**< INPUT/PRINT flag */
)
{
	//
	// Output channel only if we haven't already
	//
	if (IOChPrinted == 0)
	{
		os << Indent() << GetIPChannel(IOChannel, InputFlag);

		//
		// Flag that we have printed the channel
		//
		IOChPrinted = 1;
	}
}

/**
 * \brief Output IO channel information
 */
std::string GetIPChannel(
	Node *IOChannel,	/**< IOChannel. May be zero */
	int InputFlag		/**< INPUT/PRINT flag */
)
{
	//
	// Use default if no channel specified, else use
	// whatever is given
	//
	if ((IOChannel == 0) || (IOChannel->NoParen() == "0"))
	{
		if (InputFlag == 0)
		{
			return "std::cout";
		}
		else
		{
			return "std::cin";
		}
	}
	else
	{
		return "BasicChannel[" + IOChannel->Expression() + "]";
	}

}

/**
 * \brief Outputs a input statement.
 *
 *	This function will mangle a input statement.
 */
void Node::OutputInput(
	std::ostream& os,	/**< iostream to write C++ code to */
	int InputFlag		/**< INPUT/PRINT flag */
)
{
	//
	// Initialize flags
	//
	IOChPrinted = 0;		// Channel not yet printed
	IOChannel = 0;		// No channel specified
	IOUsing = 0;			// No print using specified

	//
	// Write out input statements
	//
	if (Tree[0] != 0)
	{
		Tree[0]->OutputInputData(os, InputFlag);
	}

	//
	// Finish off input statement
	//
	if (IOChPrinted != 0)
	{
		os << ";" << std::endl;
	}
	
	//
	// Try to handle bad entries with more grace.
	// Eat the bad entry instead of letting it block the next input.
	//
	if (InputFlag & 1)
	{
		os << Indent() <<
			"if (" << GetIPChannel(IOChannel, InputFlag) <<
			".fail()) { " << 
			GetIPChannel(IOChannel, InputFlag) << ".clear();" <<
			std::endl;
		os << Indent() <<
			GetIPChannel(IOChannel, InputFlag) << 
			".ignore(std::numeric_limits<std::streamsize>::max(), '\\n'); }" <<
			std::endl;
	}
}


/**
 * \brief Outputs data related to an input or print statement.
 *
 *	This function will mangle a input statement. Returns a
 *	flag depending on if the input ended in something that should
 *	cause a return to be appended.
 *
 * \return 0 - Blank item on right hand side.<BR>
 *	1 - Normal item on right hand side.<BR>
 *	2 - Special item was just output
 */
int Node::OutputPrintData(
	std::ostream& os,	/**< Channel to write C++ code to */
	int InputFlag		/**< Specifies if it is working on 
				 * an input statement, ie. specifies the 
				 * direction of the arrows. 
				 */
)
{
	int ReturnFlag;		// Does a return seem to be needed
				//   Gets set to one if the print/Input command
				//   ends with a ',' or ';'.
	int TestFlag = 0;	// Test for special item

	//
	// Is this a piece of a list?
	//
	switch (Type)
	{
	case BAS_N_LIST:
		//
		// Try the left side of the tree
		//
		if (Tree[0] != 0)
		{
			TestFlag = Tree[0]->OutputPrintData(os, InputFlag);
		}

		//
		// Do we need to append a tab between fields?
		// Only do so for print statements.
		//
		if ((TextValue[0] == ',') &&
			(TestFlag != 2) && (IOUsing == 0))
		{
			OutputIPChannel(os, InputFlag);
			os << " << '\\t'";
		}

		//
		// Handle right side of the tree
		//
		if (Tree[1] != 0)
		{
			ReturnFlag = Tree[1]->OutputPrintData(os, InputFlag);
		}
		else
		{
			ReturnFlag = 0;
		}

		break;

	case '#':
		IOChannel = this;
		ReturnFlag = 2;
		break;

	case BAS_S_USING:
		IOUsing = Tree[0];
		ReturnFlag = 2;

		os << Indent() << "PUse.SetFormat(" << IOUsing->Expression() <<
			");" << std::endl;

		break;

	case BAS_N_RECORD:
		OutputIPChannel(os, InputFlag);
		os << " << RECORD(" << Tree[0]->Expression() << ")";
		ReturnFlag = 2;
		break;

	case BAS_V_TEXTSTRING:
		OutputIPChannel(os, 0);

		//
		// Put a comma in print using statements, else
		// out out a '<<'.
		//
		if (IOUsing != 0)
		{
			os << " << PUse.Output(" << Expression() << ");" << std::endl;
			IOChPrinted = 0;
		}
		else
		{
			os << " << " << Expression();
		}
		ReturnFlag = 1;

		break;


	default:


		//
		// Print
		//
		if (CheckCpos())
		{
			//
			// If we have an item that needs the current
			// cursor position to be correct, we have to put
			// it at tyhe beginning of an output statement
			//
			os << ";" << std::endl;
			IOChPrinted = 0;
		}

		OutputIPChannel(os, InputFlag);

		//
		// Put a comma in print using statements, else
		// out out a '<<'.
		//
		if (IOUsing != 0)
		{
			os << " << PUse.Output(" << Expression() << ");" << std::endl;
			IOChPrinted = 0;
		}
		else
		{
			if (IsNumber() != 0)
			{
				//
				// Basic puts either a '-' or a ' ' in front
				// of numbers, and as ' ' after the number.
				//
				// \note we are assuming positive numbers only,
				// because dealing with negitive ones gets ug;y
				// really fast.
				//
				os << " << ' '" <<
					" << " << Expression() <<
					" << ' '";
			}
			else
			{
				os << " << " << Expression();
			}
		}
		ReturnFlag = 1;
		break;
	}

	//
	// Done
	//
	return ReturnFlag;
}

/**
 * \brief Check for functions that depend on cursor position
 *
 *	This function is used to check for functions that are sensitive
 *	to cursor position, since all of the parameters in a 'cout <<'
 *	are evaluated before any output is done.
 */
int Node::CheckCpos(void)
{
	//
	// Check this node
	//
	if (Type == BAS_V_FUNCTION)
	{
		VariableStruct *ThisVar;
		ThisVar = Variables->Lookup(TextValue, Tree[0]);

		//
		// Print this variable
		//
		if (ThisVar != 0)
		{
			std::string name = ThisVar->GetName();
			if ((name == "Tab") || (name == "basic::Ccpos"))
			{
				return 1;
			}
		}
	}

	//
	// Check any parameters
	//
	for (int loop = 0; loop <= 4; loop++)
	{
		if ((Tree[loop] != 0) && (Tree[loop]->CheckCpos()))
		{
			return 1;
		}
	}

	//
	// Most not be anything
	//
	return 0;
}

/**
 * \brief Outputs data related to an input or print statement.
 *
 *	This function will mangle a input statement. Returns a
 *	flag depending on if the input ended in something that should
 *	cause a return to be appended.
 *
 * /return 0 - Blank item on right hand side.<BR>
 *	1 - Normal item on right hand side.<BR>
 *	2 - Special item was just output
 */
int Node::OutputInputData(
	std::ostream& os,	/**< iostream to write C++ code to */
	int InputFlag		/**< Specifies if it is working on 
				 * an input statement, ie. specifies the 
				 * direction of the arrows. */
)
{
	int ReturnFlag;		// Does a return seem to be needed
				//   Gets set to one if the print/Input command
				//   ends with a ',' or ';'.
	int TestFlag = 0;	// Test for special item

	//
	// Is this a piece of a list?
	//
	switch (Type)
	{
	case BAS_N_LIST:
		//
		// Try the left side of the tree
		//
		if (Tree[0] != 0)
		{
			TestFlag = Tree[0]->OutputInputData(os, InputFlag);
		}

		//
		// Handle right side of the tree
		//
		if (Tree[1] != 0)
		{
			ReturnFlag = Tree[1]->OutputInputData(os, InputFlag);
		}
		else
		{
			ReturnFlag = 0;
		}

		break;

	case '#':
		IOChannel = this;
		ReturnFlag = 2;
		break;

	case BAS_S_USING:
		IOUsing = Tree[0];
		ReturnFlag = 2;

		os << Indent() << "PUse.SetFormat(" << IOUsing->Expression() <<
			");" << std::endl;

		break;

	case BAS_N_RECORD:
		OutputIPChannel(os, InputFlag);
		os << " >> RECORD(" << Tree[0]->Expression() << ")";
		ReturnFlag = 2;
		break;

	case BAS_V_TEXTSTRING:
		//
		// If this is a prompt used for input, close the current
		// input and set up an output prompt.
		//
		if (IOChPrinted != 0)
		{
			os << ";" << std::endl;
		}

		//
		// If this is an input statement, create the whole
		// statement on one line
		//
		os << Indent() << "std::cout << " <<
			Expression() << ";" << std::endl;
		ReturnFlag = 2;
		IOChPrinted = 0;
		break;


	default:

		//
		// Input: Do we need a prompt displayed?
		//
		if (IOChannel == 0)
		{
			if (IOChPrinted != 0)
			{
				os << ";" << std::endl;
				IOChPrinted = 0;
			}
			OutputIPChannel(os, 0);
			os << " << \"? \";" << std::endl;
			IOChPrinted = 0;
		}

		//
		// If we want to allow the user to enter a blank value
		// for a string, then we cannot use '>>' to ask for the
		// value.
		//
		if (IsString())
		{
			if (IOChPrinted != 0)
			{
				os << ";" << std::endl;
				IOChPrinted = 0;
			}

			os << Indent() <<
				"getline(" <<
				GetIPChannel(IOChannel, InputFlag) <<
				", " <<
				Expression() <<
				");" << std::endl;

			//
			//  getline drops terminater. INPUT LINE wants it back on.
			//
			if (InputFlag == 2)
			{
				os << Indent() <<
					Expression() <<
					" += \"\\n\";" << std::endl;
			}

			break;
		}

		//
		// Output seperator depending on what state we are in
		//
		switch(InputFlag)
		{
		//
		// Regular input
		//
		case 1:
			OutputIPChannel(os, InputFlag);
			os << " >> " << Expression();
			ReturnFlag = 1;
			break;

		//
		// LINPUT/INPUT LINE
		//
		case 2:
			os << Indent() <<
				"getline(" <<
				GetIPChannel(IOChannel, InputFlag) <<
				", " <<
				Expression() <<
				");" << std::endl;
			//
			//  getline drops terminater. INPUT LINE wants it back on.
			//
			os << Indent() <<
				Expression() << " += \"\\n\";" << std::endl;

			if (IOChannel != 0)
			{
				//
				// If we are on an IO channel, then
				// throw an error if we are at an eof
				//
				os << Indent() <<
					"if (" <<
					GetIPChannel(IOChannel, InputFlag) <<
					".eof()) { OnErrorHit(11, " <<
					erl << "); }" <<
					"\t// End of file on device" <<
					std::endl;
				os << Indent() <<
					"if (" <<
					GetIPChannel(IOChannel, InputFlag) <<
					".fail()) { OnErrorHit(12, " <<
					erl << "); }" <<
					"\t// Fatal system I/O failure" <<
					std::endl;
			}

			IOChPrinted = 0;
			ReturnFlag = 2;
		}

		break;
	}

	//
	// Done
	//
	return ReturnFlag;
}

/**
 * \brief Outputs a data statement.
 *
 *	This function will mangle a data statement.
 */
void Node::OutputData(
	std::ostream& os	/**< Output stream */
)
{
	DataWidth = 0;
	os << Indent();

	//
	// Handle NULL values specially
	//
	if (Tree[0] == 0)
	{
		os << "\"\"";
	}
	else
	{
		Tree[0]->OutputDataValue(os);
	}

	//
	// Handle additional data statements attached to this one
	//
	Node* Loop = Tree[1];
	while (Loop != 0)
	{
		//
		// Let's start on a new line for each new
		// data statement, so we keep more of the
		// original formatting.
		//
		os << "," << std::endl;
		DataWidth = 0;
		os << Indent();

		//
		// Handle NULL values specially
		//
		if (Loop->Tree[0] == 0)
		{
			os << "\"\"";
		}
		else
		{
			Loop->Tree[0]->OutputDataValue(os);
		}
		Loop = Loop->Tree[1];
	}
}


/**
 * \brief Outputs one data value in a data statement.
 *
 *	This function will mangle one data value for a data statement.
 *	It forces everything to be a string.
 */
void Node::OutputDataValue(
	std::ostream& os		/**< iostream to write C++ code to */
)
{
	if (Type == BAS_N_LIST)
	{
		//
		// Handle NULL values specially
		//
		if (Tree[0] == 0)
		{
			os << "\"\"";
			DataWidth += 2;
		}
		else
		{
			Tree[0]->OutputDataValue(os);
		}
		os << ",";
		DataWidth++;
		if (DataWidth > 65)
		{
			os << std::endl;
			DataWidth = 0;
			os << Indent();
		}
		//
		// Handle NULL values specially
		//
		if (Tree[1] == 0)
		{
			os << "\"\"";
			DataWidth += 2;
		}
		else
		{
			Tree[1]->OutputDataValue(os);
		}
	}
	else
	{
		switch(Type)
		{
		case BAS_V_TEXTSTRING:

			if (TextValue[0] == '"')
			{
				os << TextValue;
			}
			else
			{
				os << '"' << TextValue << '"';
			}
			DataWidth += TextValue.length();
			break;

		case BAS_V_NAME:
		case BAS_V_INT:
		case BAS_V_INTEGER:
		case BAS_V_FLOAT:

			os << '"' << TextValue << '"';
			DataWidth += TextValue.length() + 2;
			break;

		default:
			//
			// Force quotes around data value
			//
			os << '"' << Expression() << '"';
			DataWidth +=Expression().length() + 2;
			break;
		}
	}
}

/**
 * \brief Outputs a map or common statement.
 *
 *	This function will mangle a map or common statement.
 */
void Node::OutputMap(
	std::ostream& os		/**< iostream to write C++ code to */
)
{
	VariableStruct* ThisVar = Variables->Lookup(Tree[0]->TextValue, 0);

	os << std::endl << Indent() <<
		"// #pragma psect static_rw " << ThisVar->GetName(1) <<
		",gbl,ovr" << std::endl;

	//
	// We implement MAP and COMMON as classes because dealing with
	// their weirdness will be easier by creating methods in a base
	// class than putting stuff in the actual source. It should also
	// make putting everything in a common place simpler.
	//
	// Here we try to 'roll up' a MAP statement of the form
	//	MAP (my_map) my_regord my_var
	// into a shorder form than more comples MAPs. It means we can
	// rever to the values as 'my_var.value' instead of
	// 'my_map.my_var.value'.
	// Similiar code must exist in program.cc arount the prefix
	// generating code
	//
	if ((Tree[1]->Type == BAS_V_DEFINEVAR) && (Tree[1]->Block[0] == 0))
	{
		os <<
			Indent() << "class " << ThisVar->GetName(1) <<
				"_C : public " <<
				Tree[1]->Tree[1]->OutputNodeVarType() <<
				std::endl <<
			Indent() << "{" << std::endl;

		std::cout <<
			Indent() << "} " << Tree[1]->Tree[0]->Expression() <<
				";" << std::endl <<
			std::endl;
	}
	else
	{
		os <<
			Indent() << "class " << ThisVar->GetName(1) <<
				"_C" << std::endl <<
			Indent() << "{" << std::endl <<
			Indent() << "public:" << std::endl;

		Level++;

		if (Tree[1] != 0)
		{
			Tree[1]->OutputDefinitionList(os, 0, 0);
		}

		Level--;
		std::cout <<
			Indent() << "} " << ThisVar->GetName(1) << ";" << std::endl <<
			std::endl;
	}

//	os << "// #pragma psect end " << ThisVar->GetName() << std::endl;
}

/**
 * \brief Output the "case" selections, one at a time.
 */
void Node::OutputCaseLabel(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	//
	// Handle this if it is a list
	//
	if (Type == BAS_N_LIST)
	{
		Tree[0]->OutputCaseLabel(os);
		Tree[1]->OutputCaseLabel(os);
		return;
	}

	//
	// Output one case statement
	//
	os << Indent() << "case " << Expression() << ":" << std::endl;
}


/**
 * \brief Output the "case" selections, one at a time.
 */
void Node::OutputCaseIf(
	std::ostream& os,	/**< iostream to write C++ code to */
	Node *Parent		/**< parent node */
)
{
	//
	// Handle this if it is a list
	//
	switch (Type)
	{
	case BAS_N_LIST:

		os << "(";
		Tree[0]->OutputCaseIf(os, Parent);
		os << ") || (";
		Tree[1]->OutputCaseIf(os, Parent);
		os << ")";
		break;

	case BAS_X_UNARYLT:
		os << "TempS" << " < " <<
			Tree[0]->Expression();
		break;

	case BAS_X_UNARYLE:
		os << "TempS" << " <= " <<
			Tree[0]->Expression();
		break;

	case BAS_X_UNARYGT:
		os << "TempS" << " > " <<
			Tree[0]->Expression();
		break;

	case BAS_X_UNARYGE:
		os << "TempS" << " >= " <<
			Tree[0]->Expression();
		break;

	case BAS_X_UNARYNEQ:
		os << "TempS" << " != " <<
			Tree[0]->Expression();
		break;

	case BAS_S_TO:
		os << "TempS" << " >= " <<
			Tree[0]->Expression() << " && " <<
			"TempS" << " <= " <<
			Tree[1]->Expression();
		break;

	default:
		os << "TempS" << " == " <<
			Expression();
		break;
	}
}

/**
 * \breif Test the "case" selections, one at a time.
 */
int Node::CheckCaseLabel(void)
{
	//
	// Handle this if it is a list
	//
	switch (Type)
	{
	case BAS_S_REMARK:
		if (Block[0])
		{
			return Block[0]->CheckCaseLabel();
		}
		return 0;

	case BAS_S_CASE:

		if (Tree[0]->CheckCaseLabel())
		{
			return 1;
		}
		if (Block[0])
		{
			return Block[0]->CheckCaseLabel();
		}
		return 0;

	case BAS_N_CASEELSE:

		return 0;

	case BAS_N_LIST:

		if (Tree[0]->CheckCaseLabel())
		{
			return 1;
		}
		return Tree[1]->CheckCaseLabel();

	case BAS_X_UNARYLT:
	case BAS_X_UNARYLE:
	case BAS_X_UNARYGT:
	case BAS_X_UNARYGE:
	case BAS_X_UNARYNEQ:
		return 1;
	}

	int ThisType = GetNodeVarType();
	switch (ThisType)
	{
	case VARTYPE_INTEGER:
		return 0;
	default:
		return 1;
	}
}


/**
 * \brief Prints out a comment.
 *
 *	This function strips off all of the junk from a comment
 *	and prints it out.
 */
void Node::OutputRemark(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	std::string Remark = TextValue;

	//
	// Skip start of remark character
	//
	if (Remark[0] == '!')
	{
#ifdef STLREMOVE
		Remark.remove(0,1);
#else
		Remark.erase(0,1);
#endif
	}

	//
	// Skip extra white space
	//
	if (Remark[0] == ' ')
	{
#ifdef STLREMOVE
		Remark.remove(0,1);
#else
		Remark.erase(0,1);
#endif
	}

	//
	// Lose any junk at end of comment
	// (Yes, it does modify the original value)
	//
	while(Remark.length() > 0)
	{
		// Line continuation marker
		if (*(Remark.end() - 1) == '&')
		{
#ifdef STLREMOVE
			Remark.remove(Remark.end() - 1);
#else
			Remark.erase(Remark.end() - 1);
#endif
			continue;
		}

		// New Line
		if ((*(Remark.end() - 1) == '\n') ||
			(*(Remark.end() - 1) == '\r'))
		{
#ifdef STLREMOVE
			Remark.remove(Remark.end() - 1);
#else
			Remark.erase(Remark.end() - 1);
#endif
			continue;
		}

		// Space
		if (*(Remark.end() - 1) == ' ')
		{
#ifdef STLREMOVE
			Remark.remove(Remark.end() - 1);
#else
			Remark.erase(Remark.end() - 1);
#endif
			continue;
		}

		// Tab
		if (*(Remark.end() - 1) == '\t')
		{
#ifdef STLREMOVE
			Remark.remove(Remark.end() - 1);
#else
			Remark.erase(Remark.end() - 1);
#endif
			continue;
		}

		// Exclimation point
		if (*(Remark.end() - 1) == '!')
		{
#ifdef STLREMOVE
			Remark.remove(Remark.end() - 1);
#else
			Remark.erase(Remark.end() - 1);
#endif
			continue;
		}

		// Break out of loop
		break;
	}

	//
	// Dump out remark (and maybe add a space to the front)
	//
	if ((Remark.length() == 0) || (Remark[0] == '\t') || (Remark[0] == '*'))
	{
		os << "//" << Remark;
	}
	else
	{
		os << "// " << Remark;
	}

	//
	// There may be some extra stuff that needs printing
	//
	if (Tree[0] != 0)
	{
		os << ' ' << Tree[0]->Expression();
	}

	os << std::endl;

}

/**
 * \brief Prints out a computed goto/gosub statement
 *
 *	This translates an 'ON xxx GOTO yyy,zzz,...' into a select
 *	statement. The 'switch' portion is generated before calling this
 *	code, this only outputs the case labels/goto's.
 *
 *	\return returns nonzero if a default case was output, else it returns 0.
 */
int Node::OutputOngo(
	std::ostream& os,	/**< iostream to write C++ code to */
	const char* OnType,	/**< data type */
	int GoLevel		/**< goto level */
)
{
	int flag = 0;

	switch(Type)
	{
	case BAS_N_LIST:
		flag = Tree[0]->OutputOngo(os, OnType, GoLevel);
		flag += Tree[1]->OutputOngo(os, OnType, GoLevel + 1);
		break;

	case BAS_V_USELABEL:
		os << Indent() << "case " << GoLevel << ":" << std::endl;
		Level++;
		os << Indent() << OnType;
		if (strcmp(OnType, "goto") == 0)
		{
			os << ' ' << OutputLabel() << ";" << std::endl;
			// goto doesn't need to be followed by a break
		}
		else
		{
			os << '(' << OutputLabel() << ");" << std::endl;
			os << Indent() << "break;" << std::endl;
		}
		Level--;
		break;

	default:
		os << "Error in computed " << OnType <<
			" (" << Expression() << ") @ " << lineno << std::endl;
		break;
	}
	return flag;
}

/**
 * \brief Prints out a label
 *
 *	Output a label (if referenced anywhere)
 */
std::string Node::OutputLabel(void)
{
	VariableStruct *ThisVar = Variables->Lookup(TextValue, 0);
	if (ThisVar != 0)
	{
		return ThisVar->GetName();
	}
	else
	{
		return genname(TextValue);
	}
}

/**
 * \brief Prints out a series of definitions
 */
void Node::OutputDefinitionList(
	std::ostream& os,	/**< iostream to write C++ code to */
	Node* MainType,		/**< main data type */
	int ExtType,		/**< extended data type */
	int GlobalStat		/**< global status */
)
{
	switch(Type)
	{
	case BAS_S_DECLARE:
		Tree[0]->OutputDefinitionList(os,
			MainType, ExtType, GlobalStat);
		if (Block[0] != 0)
		{
			Block[0]->OutputDefinitionList(os,
				MainType, ExtType, GlobalStat);
		}
		break;

	case BAS_V_DEFINEVAR:
		os << Indent();

		switch(GlobalStat)
		{
		case 1:		// Constant definition
			os << "const ";
			break;

		case 3:		// External constant
			os << "extern const ";
			break;

		case 4:		// External
			os << "extern ";
			break;
		}

		//
		// We have an array
		//
		if (Tree[2] != 0)
		{
			os << Tree[2]->OutputArrayDef(this) << " " <<
				Tree[0]->OutputVarName(Tree[2], 1 + 2);
		}
		else
		{
			if (Tree[1] != 0)
			{
				os << Tree[1]->OutputNodeVarType() << " ";
			}
			if (Tree[3] != 0)
			{
				os << Tree[3]->OutputPassmech(0);
			}
			if (Tree[0] != 0)
			{
				os << Tree[0]->OutputVarName(Tree[2], 1);
			}
			if (Tree[4] != 0)
			{
				os << " = " << Tree[4]->Expression();
			}
			else
			{
				os << OutputVarInit(
					Tree[2] != 0,
					Tree[1]->GetNodeVarType());
			}
		}

		os << ";" << std::endl;

//
// For these definitions, a downlink points to the next variable 
// not to internal structures of this variable.
// So, is this needed by anything?
// 
// This is apparently used by DIM statements when multu=iplw variables
// are dimensioned.
//
		if (Block[0] != 0)
		{
			Block[0]->OutputDefinitionList(os, MainType, ExtType, GlobalStat);
		}
		break;

	case BAS_V_DEFINEFUN:
	case BAS_N_EXTERNALFUNCTION:
	case BAS_N_EXTERNALSUB:
		os << Indent();

		switch(GlobalStat)
		{
		case 2:		// Constant definition
			os << "extern ";
			break;
		}

		if (Tree[1] != 0)
		{
			os << Tree[1]->OutputNodeVarType() << " ";
		}
		if (Tree[3] != 0)
		{
			os << Tree[3]->OutputPassmech(1);
		}
		os << Tree[0]->OutputVarName(Tree[2], 1);
		if (Tree[4] != 0)
		{
			os << " = ";
			Tree[4]->OutputCodeOne(os);
		}
		os << ";" << std::endl;
		if (Block[0] != 0)
		{
			Block[0]->OutputDefinitionList(os, MainType, ExtType, GlobalStat);
		}
		break;

	case BAS_S_VARIANT:

		os << Indent() << "union" << std::endl <<
			Indent() << "{" << std::endl;
		Level++;
		Block[1]->OutputDefinitionList(os, MainType, ExtType, GlobalStat);
		Level--;
		os << Indent() << "};" << std::endl;

		break;

	case BAS_S_CASE:
		//
		// Variant case
		//
		for (Node* loop = this; loop != 0; loop = loop->Block[0])
		{
			os << Indent() << "struct" << std::endl <<
				Indent() << "{" << std::endl;
			Level++;
			loop->Block[1]->OutputDefinitionList(os, MainType, ExtType,
				GlobalStat);
			Level--;
			os << Indent() << "};" << std::endl;
		}

		break;


	case BAS_S_REMARK:
		os << Indent();
		OutputRemark(os);
		if (Block[0] != 0)
		{
			Block[0]->OutputDefinitionList(os, MainType, ExtType, GlobalStat);
		}
		break;

	default:
		os << Indent() << OutputDefinition(MainType, ExtType) << ";" << std::endl;
		break;
	}
}


/**
 * Output an array definition
 */
std::string Node::OutputArrayDef(Node *base)
{
	std::string working;	// Working definition being created

	working = "std::array<";

	if (Block[0] != 0)
	{
		//
		// If we have more dimensions to go, ...
		//
		working += Block[0]->OutputArrayDef(base);
	}
	else
	{
		//
		// No more dimensions, so output the base type
		//
		working += base->Tree[1]->OutputNodeVarType();
	}
//	working += ", " + Tree[2]->Expression() + ">";

	//
	// The array size in C++ includes the [0] element, while Basic
	// specifies the last element, so we have to add 1 to the size
	// requested.
	//
	if (IsSimpleInteger())
	{
		// FIXME: There really ought to be a better way
		char buffer[16];
		sprintf(buffer, ", %d>", atoi(TextValue.c_str()) + 1);
		working += buffer;
	}
	else
	{
		working += ", " + OutputForcedType(VARTYPE_INTEGER) + " + 1>";
	}


	return working;
}

/**
 * \brief Prints out a series of Virtual Array definitions
 */
void Node::OutputVirtualList(
	std::ostream& os,	/**< iostream to write C++ code */
	Node* Channel,		/**< IO channe to use */
	Node* MainType,		/**< Main stat type */
	int ExtType,		/**< Extra data type */
	int GlobalStat		/**< Global status */
)
{
	VariableStruct *ThisVar;

	switch(Type)
	{
	case BAS_N_LIST:

		//
		// Put multiple items on seperate lines
		//
		Tree[0]->OutputVirtualList(os, Channel, MainType, ExtType);
		Tree[1]->OutputVirtualList(os, Channel, MainType, ExtType);

		break;

	default:
		//
		// Look for variable in table
		//
		ThisVar = Variables->Lookup(Tree[0]->TextValue, Tree[2]);

		os << Indent() <<
			"VirtualArray<" <<
			Tree[1]->OutputNodeVarType() << "> ";

		if (ThisVar != 0)
		{
			os << ThisVar->GetName() << "(";
		}
		else
		{
			os << genname(Tree[0]->TextValue) << "(";
		}
		os << Channel->Expression() << ", " << Tree[2]->Expression();

		if (Tree[3] != 0)
		{
			os << ", " << Tree[3]->Expression();
		}
		os << ");" << std::endl;

		if (Block[0] != 0)
		{
			Block[0]->OutputVirtualList(os, Channel, MainType, ExtType);
		}
		break;

	}
}

/**
 * \brief Prints out a series of paramater definitions
 */
std::string Node::OutputDefinitionParams(
	Node* MainType,		/**< Main data type */
	int ExtType,		/**< Extra data type */
	int GlobalStat		/**< Global status */
)
{
	std::string result;

	switch(Type)
	{
	case BAS_V_DEFINEVAR:
		switch(GlobalStat)
		{
		case 1:		// Constant definition
			result = "const ";
			break;
		}

		if (Tree[1] != 0)
		{
			result += Tree[1]->OutputNodeVarType() + " ";
		}
		if (Tree[3] != 0)
		{
			result += Tree[3]->OutputPassmech(0);
		}
		if (Tree[0] != 0)
		{
			result += Tree[0]->OutputVarName(Tree[2], 1);
		}
		if (Tree[4] != 0)
		{
			result += " = " + Tree[4]->Expression();
		}

		if (Block[0] != 0)
		{
			result += ", " +
				Block[0]->OutputDefinitionParams(MainType, ExtType, GlobalStat);
		}
		break;

	case BAS_V_DEFINEFUN:
		if (Tree[1] != 0)
		{
			result += Tree[1]->OutputNodeVarType() + " ";
		}
		if (Tree[3] != 0)
		{
			result += Tree[3]->OutputPassmech(1);
		}
		result += Tree[0]->OutputVarName(Tree[2], 1);
		if (Tree[4] != 0)
		{
			result += " = " + Tree[4]->Expression();
		}
		if (Block[0] != 0)
		{
			result += ", " +
					Block[0]->OutputDefinitionParams(MainType, ExtType, GlobalStat);
		}
		break;

	default:
std::cerr << "Something isn't using BAS_V_DEFINE??? (default) @ " << lineno << std::endl;
		result += "const " + OutputDefinition(MainType, ExtType);
		break;
	}

	return result;
}


/**
 * \brief Handle writing out the parameters to a function/definition/var
 */
std::string Node::OutputPassmech(
	int FunctionFlag	/**> Function flag */
)
{
	std::string result;

	switch(Type)
	{
	case BAS_S_VALUE:
		break;

	case BAS_S_REF:
		result = "&";
		break;

	case BAS_S_DESC:
		result = "& /* Descriptor */ ";
		break;
	}

	return result;
}

/**
 * \brief Prints out a definition
 *
 *	This function will output a function/subroutine definition
 *	in a format that is acceptable to C++.
 *
 * \bug Doesn't yet know about passing mechanisms
 */
std::string Node::OutputNewDefinition(void)
{
	VariableStruct *ThisVar;
	int IsFunction = 0;
	std::string result;

	//
	// Handle class and type
	//
	switch(Type)
	{
	case BAS_N_EXTERNALFUNCTION:
		IsFunction = 1;
		result = std::string("extern ") + Tree[0]->OutputNodeVarType() + " ";
		break;

	case BAS_N_EXTERNALSUB:
		IsFunction = 1;
		result = "extern void ";
		break;

	case BAS_S_FUNCTION:
		IsFunction = 1;
		result = Tree[0]->OutputNodeVarType() + " ";
		break;

	case BAS_S_DEF:
	case BAS_S_DEFSTAR:
		IsFunction = 1;
		result += "auto ";
		break;

	case BAS_N_EXTERNALCONSTANT:
		result = "extern const " + Tree[0]->OutputNodeVarType() + " ";
		break;

	case BAS_S_SUB:
	case BAS_S_PROGRAM:
		IsFunction = 1;
		result = "void ";
		break;

	case BAS_S_HANDLER:
		IsFunction = 1;
		result = "void ";
		break;
	}

	//
	// Look for variable in table
	//
	ThisVar = Variables->Lookup(Tree[1]->TextValue, Tree[3]);

	//
	// Make up a good name to print for this variable/function
	//
	if (ThisVar != 0)
	{
		result += ThisVar->GetName();
	}
	else
	{
		result += "?" + Tree[1]->TextValue + "?";
	}

	//
	// Lambda function for DEF
	//
	switch(Type)
	{
	case BAS_S_DEF:
	case BAS_S_DEFSTAR:
		result += " = [&]";
		break;
	}

	if (Tree[3] != 0)
	{
		if (IsFunction == 0)
		{
			//
			// Handle references (Function or array)
			// (May need to be modified from x[a,b]
			// to x[a][b] manually.
			//
			if (Tree[3] != 0)
			{
				if (Tree[3]->IsSimpleInteger())
				{
					// FIXME: Isn't there a better way?
					char buffer[16];
					sprintf(buffer, "[%d]",
						(atoi(Tree[3]->TextValue.c_str()) + 1));
					result += buffer;
				}
				else
				{
					result += "[" +
						Tree[3]->OutputForcedType(VARTYPE_INTEGER) + " + 1]";
				}
			}
		}
		else
		{
			//
			// Handle references (Function)
			//
			result += "(";

			if (Tree[3] != 0)
			{
				result += Tree[3]->OutputDefinitionParams(0, BAS_N_LIST);
			}
			else
			{
				result += "...";
			}

			result += ")";
		}
	}
	else
	{
		if (IsFunction != 0)
		{
			result += "(void)";
		}
	}

	//
	// Output any constant definitions
	//
	if (Tree[4] != 0)
	{
		result += " = " + Tree[4]->Expression();
	}

	return result;
}

/**
 * \brief Prints out a definition
 */
std::string Node::OutputDefinition(
	Node* MainType,		/**< Main data type */
	int ExtType		/**< Extra data type */
)
{
	VariableStruct *ThisVar;
	std::string result;

	switch(Type)
	{
	case BAS_N_ASSIGN:

		result = Tree[0]->OutputDefinition(MainType, BAS_N_ASSIGN) +
			" = " + Tree[1]->Expression();
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
	case BAS_N_STRUCTNAME:

		//
		// This is a data type, so see if there are any variables
		// listed under this type, else dump out the data type
		// all be itself.
		//
		if (Tree[2] != 0)
		{
			result = Tree[2]->OutputDefinition(this, ExtType);
		}
		else
		{
			result = OutputNodeVarType() + " ";
		}
		if (Tree[3] != 0)
		{
			if (Tree[3]->IsSimpleInteger())
			{
				// FIXME: There has to be a better way
				char buffer[16];
				sprintf(buffer, "[%d]",
					(atoi(Tree[3]->TextValue.c_str()) + 1));
				result += buffer;
			}
			else
			{
				result += "[" +
					Tree[3]->OutputForcedType(VARTYPE_INTEGER) + " + 1]";
			}
		}
		break;

	default:
		//
		// Handle class
		//
		switch(ExtType)
		{
		case BAS_S_EXTERNAL:
			result = "extern ";
			break;

		case BAS_N_EXTERNALCONSTANT:
			result = "extern const ";
			break;

		}

		switch (Type)
		{
		case BAS_V_NAME:
			//
			// Look for variable in table
			//
			ThisVar = Variables->Lookup(TextValue, Tree[0]);

			//
			// Slap on sone kind of data type.
			//	1. Try the variable table
			//	2. Try for any kind of attached type
			//	3. Give up and call it a void.
			//
			if (ThisVar != 0)
			{
				result += OutputVarType(ThisVar->Type) + " ";
			}
			else
			{
				if (MainType != 0)
				{
					result += OutputVarType(MainType->GetNodeVarType()) + " ";
				}
				else
				{
					result += "void ";
				}
			}

			//
			// Make up a good name to print for this variable
			//
			if (ThisVar != 0)
			{
				result += ThisVar->GetName();
			}
			else
			{
				result += genname(TextValue);
			}

			//
			// Handle references (Function or array)
			// (May need to be modified from x[a,b]
			// to x[a][b] manually.
			//
			if (Tree[0] != 0)
			{
				result += "[" + Tree[0]->OutputArrayParam(1) + "]";
			}
			if (Tree[3] != 0)
			{
				if (Tree[3]->IsSimpleInteger())
				{
					// FIXME: There has to be a better way
					char buffer[16];
					sprintf(buffer, "[%d]",
						(atoi(Tree[3]->TextValue.c_str()) + 1));
					result += buffer;
				}
				else
				{
					result += "[" +
						Tree[3]->OutputForcedType(VARTYPE_INTEGER) + " + 1]";
				}
			}
			break;

		case BAS_V_FUNCTION:

			//
			// Look for variable in table
			//
			ThisVar = Variables->Lookup(TextValue, Tree[0]);

			//
			// Slap on sone kind of data type.
			//	1. Try the variable table
			//	2. Try for any kind of attached type
			//	3. Give up and call it a void.
			//
			if (ThisVar != 0)
			{
				result += OutputVarType(ThisVar->Type) + " ";
			}
			else
			{
				if (MainType != 0)
				{
					result += OutputVarType(MainType->GetNodeVarType()) + " ";
				}
				else
				{
					result += "void ";
				}
			}

			//
			// Make up a good name to print for this variable
			//
			if (ThisVar != 0)
			{
				result += ThisVar->GetName();
			}
			else
			{
				result += genname(TextValue);
			}

			//
			// Handle references (Function)
			//
			result += "(";

			if (Tree[0] != 0)
			{
				result += Tree[0]->OutputDefinitionParams(0, BAS_N_LIST);
			}
			else
			{
				if ((ExtType == BAS_S_EXTERNAL) ||
					(ExtType == BAS_N_EXTERNALSUB) ||
					(ExtType == BAS_N_EXTERNALFUNCTION))
				{
					result += "...";
				}
				else
				{
					result += "void";
				}
			}

			result += ")";

			break;

		default:
			{
				// FIXME: There has to be a better way
				char buffer[32];
				sprintf(buffer, "%d@%d", Type, lineno);
				result = std::string("(\?\?) ") + buffer + "(\?\?)" + TextValue;
			}
			break;
		}
		break;

	}

	return result;
}

/**
 * \brief Prints out an array reference list
 */
std::string Node::OutputArrayParam(
	int Increment	/**< Increment */
)
{
	std::string result;

	switch(Type)
	{
	case BAS_N_LIST:

		if (Tree[0] != 0)
		{
			result = Tree[0]->OutputArrayParam(Increment);
		}
		else
		{
			result = "(NULL)";
		}
		result += "][";
		if (Tree[1] != 0)
		{
			result += Tree[1]->OutputArrayParam(Increment);
		}
		else
		{
			result += "(NULL)";
		}
		break;

	default:

		if (Increment == 1)
		{
			if (IsSimpleInteger())
			{
				// FIXME: There really ought to be a better way
				char buffer[16];
				sprintf(buffer, "%d", atoi(TextValue.c_str()) + 1);
				result = buffer;
			}
			else
			{
				result = OutputForcedType(VARTYPE_INTEGER) + " + 1";
			}
		}
		else
		{
			result = OutputForcedType(VARTYPE_INTEGER);
		}
		break;

	}
	return result;
}


/**
 * \brief Output code for an 'OPEN' statement
 *
 *	This function generates the code necessary to handle
 *	a full 'RMS' open statement. It generates calls to a class
 *	for all the elements available in the statement.
 */
void Node::OutputOpenStuff(
	std::ostream& os,	/**< iosream to write C++ code to */
	Node* Channel		/**< Basic IO channel */
)
{
	switch(Type)
	{
	case BAS_S_RECORDSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetRecordSize(" << Tree[0]->Expression() << ");" << std::endl;
		break;

	case BAS_S_CLUSTERSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetClusterSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_FILESIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetFileSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_EXTENDSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetExtendSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_WINDOWSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetWindowSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_BLOCKSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetBlockSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_BUCKETSIZE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetBucketSize(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_MODE:

		os << Indent() <<
			GetIPChannel(Channel, 0)<<
			".SetMode(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_BUFFER:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetBuffer(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_DEFAULTNAME:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetDefaultName(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_PRIMARY:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetKey(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_ALTERNATE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetKey(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;

	case BAS_S_TEMPORARY:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetTemporary(True);" << std::endl;
		break;

	case BAS_S_CONTIGUOUS:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetContiguous(True);" << std::endl;
		break;

	case BAS_S_NOREWIND:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetRewind(False);" << std::endl;
		break;

	case BAS_S_NOSPAN:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetSpan(False);" << std::endl;
		break;

	case BAS_S_SPAN:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetSpan(True);" << std::endl;
		break;

	case BAS_S_ORGANIZATION:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetOrginization(" << Tree[0]->TextValue;
		if (Tree[0]->Tree[0] != 0)
		{
			os << ", " << Tree[0]->Tree[0]->TextValue;
		}
		os << ");" << std::endl;
		break;

	case BAS_S_ACCESS:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetAccess(" << Tree[0]->TextValue << ");" <<
			std::endl;
		break;

	case BAS_S_ALLOW:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetAllow(" << Tree[0]->TextValue << ");" <<
			std::endl;
		break;

	case BAS_S_CONNECT:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetConnect(BasicChannel[" << Tree[0]->Expression() << "]);" <<
			std::endl;
		break;

	case BAS_S_MAP:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetMap(" << Tree[0]->Expression() << ", sizeof(" <<
			Tree[0]->Expression() <<
			"));" << std::endl;
		break;

	case BAS_S_RECORDTYPE:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetRecordType(" << Tree[0]->TextValue <<
			");" << std::endl;
		break;

	case BAS_S_VIRTUAL:
		break;

	default:

		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".GoofyOpenStuff(" << Expression() << ");" <<
			std::endl;
		break;

	}

	//
	// Handle downlink
	//
	if (Block[0] != 0)
	{
		Block[0]->OutputOpenStuff(os, Channel);
	}
}

/**
 * \brief Output code for a 'CLOSE' statement
 *
 *	This function generates the code necessary to handle
 *	a full 'RMS' close statement.
 */
void Node::OutputClose(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	switch(Type)
	{
	case BAS_N_LIST:
		Tree[0]->OutputClose(os);
		Tree[1]->OutputClose(os);
		break;

	default:
		os << Indent() <<
			GetIPChannel(this, 0) <<
			".close();" << std::endl;
		break;
	}
}

/**
 * \brief Force first parameter to be a 'BString' instead of a string constant.
 *
 *	This function is used to force the first item in a string expression
 *	to be a BString, so that the compiler won't choke on exporessions
 *	like "ABC"+"DEF".
 */
std::string Node::OutputBstringText(void)
{
	std::string result;

	switch(Type)
	{
	case BAS_V_TEXTSTRING:
		result = "std::string(" + Expression() + ")";
		break;

	default:
		result = Expression();
		break;
	}

	return result;
}

/**
 * \brief Output an integer/number
 *
 *	Used to output integers, while stripping off unecessary stuff
 *	that confuses C++, like leading zeros and trailing percent
 *	signs..
 */
std::string Node::OutputInteger(void)
{
	int OFlag = 0;		// Have we output something yet?
	int Ptr = 0;		// What character are we looking at
	char ch;		// Actual character
	std::string result;		// Working string

	//
	// Output all digits in the string
	//
	while ((ch = TextValue[Ptr]), (Ptr < TextValue.length()))
	{
		Ptr++;
		if (ch == '0')
		{
			if (OFlag != 0)
			{
				result += ch;
			}
		}
		else
		{
			if (ch != '%')
			{
				if ((ch == '.') && (OFlag == 0))
				{
					result += '0';
				}
				result += ch;
				OFlag = 1;
			}
		}
	}

	//
	// If we didn't output anything, then output a zero
	// because it must have been all zeros.
	//
	if (OFlag == 0)
	{
		result += '0';
	}

	return result;
}


/**
 * \brief Output a field statement
 *
 *	Used to generate code for field statements
 */
void Node::OutputField(
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	switch(Type)
	{
	case BAS_N_LIST:

		Tree[0]->OutputField(os);
		Tree[1]->OutputField(os);

		break;

	case BAS_S_AS:
		os << Indent() << Tree[1]->Expression() <<
			".ReMap(FieldBase, " << Tree[0]->Expression() <<
			");" << std::endl <<
			Indent() << "FieldBase += " << Tree[0]->Expression() << ";" << std::endl;
		break;

	default:
		os << "// **** This shouldn't be possible ***** @ " << lineno << std::endl <<
			Indent() << genname(TextValue);
		break;
	}
}

/**
 * \brief Output code got get/put options
 *
 *	Outputs code for get/put/find for such options as 'record',
 *	'rfa', 'regardless' and other modifiers.
 */
void Node::OutputGetPutOptions(
	Node* Channel,		/**< BASIC channel */
	std::ostream& os	/**< iostream to write C++ code to */
)
{
	switch(Type)
	{
	case BAS_N_LIST:
		if (Tree[0] != 0)
		{
			Tree[0]->OutputGetPutOptions(Channel, os);
		}
		if (Tree[1] != 0)
		{
			Tree[1]->OutputGetPutOptions(Channel, os);
		}
		break;

	case BAS_V_RMSRECORD:
	case BAS_S_RECORD:
	case BAS_S_BLOCK:
		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetRecord(" <<
			Tree[0]->Expression() << ");" << std::endl;
		break;

	case BAS_S_REGARDLESS:
		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetRegardless();" << std::endl;
		break;

	case BAS_S_KEY:
		os << Indent() <<
			GetIPChannel(Channel, 0) <<
			".SetKey(" << Tree[0]->Expression() << ");" <<
			std::endl;
		if (Tree[1] != 0)
		{
			os << Indent() <<
				GetIPChannel(Channel, 0) <<
				".SetKeyMode(";
			switch(Tree[1]->Type)
			{
			case BAS_S_EQ:
				os << "Equal";
				break;
			case BAS_S_GE:
				os << "GreaterEqual";
				break;
			case BAS_S_GT:
				os << "Greater";
				break;

			}
			os << ");" << std::endl <<
				Indent() <<
				GetIPChannel(Channel, 0) <<
				".SetKeyValue(" << Tree[2]->Expression() << ");" << std::endl;
		}
		break;

	case BAS_S_RFA:
		os <<
			GetIPChannel(Channel, 0) <<
			".SetRfaValue(" <<
			Tree[0]->Expression() << ");" << std::endl;
		break;

	case BAS_S_COUNT:
		os <<
			GetIPChannel(Channel, 0) <<
			".SetCount(" << Tree[0]->Expression() << ");" <<
			std::endl;
		break;
	default:
		os <<
			GetIPChannel(Channel, 0) <<
			".Goofy(" << Expression() << ");" << std::endl;
	}
}

/**
 * \brief Calculate the varible type for a given node.
 *
 *	Determines the variable type for a given node, and returns
 *	the specified type.
 */
std::string Node::OutputForcedType(
	VARTYPE VarType		/**< Variable type */
)
{
	int ThisType = GetNodeVarType();	// Var type of this node
	int NeedForce = 0;			// Do we need to force the type
	std::string result;

	//
	// A NULL will match anything
	//
	if (Type == BAS_N_NULL)
	{
		result = NoParen();
		return result;
//		return 0;
	}

	//
	// Determine if we really need to force
	//
	switch(VarType)
	{
	case VARTYPE_INTEGER:
		//
		// Integer type allows for BYTE, INTEGER, LONG
		//
		if ((ThisType != VARTYPE_BYTE) &&
			(ThisType != VARTYPE_WORD) &&
			(ThisType != VARTYPE_INTEGER) &&
			(ThisType != VARTYPE_LONG))
		{
			NeedForce = 1;
		}
		break;

	default:
		if (ThisType != VarType)
		{
			NeedForce = 1;
		}
	}

	//
	// Now force out the proper code
	//
	if (NeedForce)
	{
		result = std::string("(") + OutputVarType(VarType) + ")(" +
			NoParen() + ")";
	}
	else
	{
		result = NoParen();
	}

	return result;
}

/**
 * \brief Output the variable type definition for a node
 *
 *	This function is used to output the variable type of a type node
 *	in a variable declaration statement.
 */
std::string Node::OutputNodeVarType(void)
{
	std::string result;

	switch(Type)
	{
	case BAS_N_STRUCTNAME:
	case BAS_N_VARTYPE:
// FIXME: This needs attention, at least lower case the bloody name.
		result = genname(TextValue);
		break;

	default:
		result = OutputVarType(GetNodeVarType());
		break;
	}

	return result;
}


