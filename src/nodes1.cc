/**\file nodes1.cc
 * \brief Routines to work with nodes (generic routines).
 *
 *	This file contains various routines for manipulating the tree
 *	nodes used when parsing a basic program.
 *
 *	This file contains the generic functions.
 *
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
int GosubFlag = 0;		/**< \brief Gosub flag */
int OnErrorFlag = 0;		/**< \brief On Eooor flag. */
int WhenErrorFlag = 0;		/**< \brief When Eooor flag. */

int Node::Level = 0;		/**< \brief Indentation level */

/**
 * \brief Create a new node just like an existing node
 *
 *	Creates a new node which is a copy of an existing node,
 *	including all linked nodes.
 */
Node::Node(
	const Node &OldNode	/**< Node to copy */
)
{
	int loop;		// Generic loop variable

	//
	// Copy over all the easy bits
	//
	Type = OldNode.Type;
	TextValue = OldNode.TextValue;
	Level = OldNode.Level;
	FromInclude = OldNode.FromInclude;
	lineno = OldNode.lineno;

	//
	// Copy over tree entries
	//
	for (loop = 0; loop < 5; loop++)
	{
		if (OldNode.Tree[loop] == 0)
		{
			Tree[loop] = 0;
		}
		else
		{
			//
			// Recursive call here
			//
			Tree[loop] = new Node(*OldNode.Tree[loop]);
		}
	}

	//
	// Copy over block entries
	//
	//	Should we really copy over the block entries?
	//	This may proove to be rather a lot of stuff.
	//
	for (loop = 0; loop < 3; loop++)
	{
		if (OldNode.Block[loop] == 0)
		{
			Block[loop] = 0;
		}
		else
		{
			//
			// Recursive call here
			//
			Block[loop] = new Node(*OldNode.Block[loop]);
		}
	}
}

/**
 * \brief Link child nodes into parent node.
 *
 *	This function will link several child nodes into a parent node.
 *
 * \return
 *	Returns a pointer to the parent node (passed as the first
 *	parameter)
 */
Node *Node::Link
(
	Node *Node0,	/**< Link to slot 0 */
	Node *Node1,	/**< Link to slot 1 */
	Node *Node2,	/**< Link to slot 2 */
	Node *Node3,	/**< Link to slot 3 */
	Node *Node4	/**< Link to slot 4 */
)
{
	//
	// Trap stupid bugs
	//
	assert(this != 0);

	//
	// Attach them together
	//
	if (Node0 != 0)
	{
		assert(Tree[0] == 0);
		assert(Node0 != this);
		Tree[0] = Node0;
	}
	if (Node1 != 0)
	{
		assert(Tree[1] == 0);
		assert(Node1 != this);
		Tree[1] = Node1;
	}
	if (Node2 != 0)
	{
		assert(Tree[2] == 0);
		assert(Node2 != this);
		Tree[2] = Node2;
	}
	if (Node3 != 0)
	{
		assert(Tree[3] == 0);
		assert(Node3 != this);
		Tree[3] = Node3;
	}
	if (Node4 != 0)
	{
		assert(Tree[4] == 0);
		assert(Node4 != this);
		Tree[4] = Node4;
	}

	//
	// Return main node back
	//
	return(this);
}

/**
 * \brief Link two nodes together at same level.
 *
 *	This function will link two parent nodes together at the same
 *	tree level. If one or the other node doesn't exist, it will
 *	make whichever node exists the main node.
 *
 *	\return the top node.
 *
 */
Node *Node::DownLink
(
	Node *Node0,	/**< Node to link in */
	int Ptr		/**< Slot to link it into */
)
{
	//
	// Don't link if nothing to link to
	//
	if (this == 0)
	{
		return(Node0);
	}

	//
	// Skip out if link node is 0
	//
	if (Node0 == 0)
	{
		return this;
	}

	//
	// Test for stupid bugs
	//
	assert(Node0 != this);

	//
	// If we aren't at the top of the stack, scan to the end
	//
	if (Block[Ptr] == 0)
	{
		Block[Ptr] = Node0;
		return this;
	}

	Node* LookDown = Block[Ptr];
	while (LookDown->Block[0] != 0)
	{
		LookDown = LookDown->Block[0];
	}

	//
	// Attach them together
	//
	LookDown->Block[0] = Node0;

	//
	// Return main node back
	//
	return(this);
}

/**
 * \brief Prints the node tree.
 *
 *	This function will print out a simple picture of the node
 *	and its relationship to it's children.
 *
 * \bug Only displays to stdout. Ugly display.
 *
 */

void Node::PrintTree(
	const std::string &Level,	/**< Current indent level */
	int Flag,			/**< Output Flags */
	char Node			/**< Character to link to indent level */
)
{
	//
	// Not really a node
	//
	assert(this != 0);

	//
	// Local variables
	//
	std::string DownLevel;

	//
	// Make sure we have an active node
	//
	if ((Block[0] != 0) || (Block[1] != 0) ||
		(Block[2] != 0) || Flag)
	{
		DownLevel = " | ";
	}
	else
	{
		DownLevel = "   ";
	}

	std::string NextLevel = Level + DownLevel;

	//
	// Print bars for this level
	//
	std::cout << Level << " " << Node << "-" << Type;

	if (TextValue.length() != 0)
	{
		std::cout << " Text: " << TextValue;
	}

	//
	// Output variable type
	//
	std::cout << " <" << DumpVarType(GetNodeVarType()) << ">";

	std::cout << std::endl;

	//
	// Print Parameter Levels
	//
	for (int loop = 0; loop < 5; loop++)
	{
		if (Tree[loop] != 0)
		{
			int fflag = 0;
			for (int inloop = loop + 1; inloop < 5; inloop++)
			{
				if (Tree[inloop] != 0)
				{
					fflag = 1;
				}
			}
			Tree[loop]->PrintTree(NextLevel, fflag, loop + '0');
		}
	}

	//
	// Link down to next parent if possible
	//
	if (Block[1] != 0)
	{
		Block[1]->PrintTree(NextLevel, 0, 'T');
	}
	if (Block[2] != 0)
	{
		Block[2]->PrintTree(NextLevel, 0, 'E');
	}
	if (Block[0] != 0)
	{
		Block[0]->PrintTree(Level, 0, 'D');
	}
}

/**
 * \brief Determine the varible type for a given expression.
 *
 *	Determines the variable type for a given node, and returns
 *	the specified type.
 *
 *	Expects to be called with an expression node,
 *	not a statement node.
 *
 * \return data type for nodes expression.
 *
 * \bug Doesn't yet handle all the variable types necessary.
 */
VARTYPE Node::GetNodeVarType(void)
{
	VariableStruct *ThisVar;

	//
	// Handle this node type
	//
	switch (Type)
	{
	//
	// Takes the type of the rvalue
	//
	case '#':
	case '(':
	case BAS_N_ASSIGN:
	case BAS_N_ASSIGNLIST:
	case BAS_S_BY:
	case BAS_N_FORASSIGN:
	case BAS_N_UMINUS:
	case BAS_N_UPLUS:
		if (Tree[0] != 0)
		{
			return Tree[0]->GetNodeVarType();
		}
		break;

	//
	// Takes the 'higher' of the two types
	//
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
		assert(Tree[0] != 0);
		assert(Tree[1] != 0);
		return ChooseVarType(Tree[0]->GetNodeVarType(),
			Tree[1]->GetNodeVarType());

	case BAS_N_LIST:
		if ((Tree[0] != 0) && (Tree[1] != 0))
		{
			return ChooseVarType(Tree[0]->GetNodeVarType(),
				Tree[1]->GetNodeVarType());
		}
		break;
	//
	// Always an integer
	//
	case '=':
	case '>':
	case '<':
	case BAS_X_LAND:
	case BAS_S_AND:
	case BAS_S_EQV:
	case BAS_S_IMP:
	case BAS_S_ERL:
	case BAS_X_GE:
	case BAS_V_INT:
	case BAS_V_INTEGER:
	case BAS_X_LE:
	case BAS_X_NEQ:
	case BAS_S_NOT:
	case BAS_X_LNOT:
	case BAS_X_LOR:
	case BAS_S_OR:
	case BAS_X_UNARYGT:
	case BAS_X_UNARYGE:
	case BAS_X_UNARYLE:
	case BAS_X_UNARYLT:
	case BAS_X_UNARYNEQ:
	case BAS_S_XOR:
	case BAS_S_INTEGER:
		return VARTYPE_INTEGER;

	//
	// Always a floating point
	//
	case BAS_V_FLOAT:
		return VARTYPE_REAL;

	//
	// Always a string
	//
	case BAS_V_TEXTSTRING:
		return VARTYPE_DYNSTR;

	//
	// Function references
	//
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
			return ThisVar->Type;
		}
		break;

	case BAS_S_BYTE:
		return VARTYPE_BYTE;

	case BAS_S_DOUBLE:
		return VARTYPE_DOUBLE;

	case BAS_S_GFLOAT:
		return VARTYPE_GFLOAT;

	case BAS_S_HFLOAT:
		return VARTYPE_GFLOAT;

	case BAS_S_LONG:
		return VARTYPE_LONG;

	case BAS_S_WORD:
		return VARTYPE_WORD;

	case BAS_S_REAL:
		return VARTYPE_REAL;

	case BAS_S_STRING:
		return VARTYPE_DYNSTR;

	case BAS_S_DECIMAL:
		return VARTYPE_DECIMAL;

	//
	// Gonna havta handle these at some time, but for now ...
	//
	case BAS_V_PREDEF:
	case BAS_X_STRREF:
	case BAS_N_STRUCTNAME:
	case BAS_N_TYPEFUN:
	case BAS_N_VARTYPE:
	case BAS_S_RFA:

//**		return Type;

		break;
	}

	//
	// I don't know!
	//
	return VARTYPE_NONE;
}

/**
 * \brief Determines if a text string is really a simple string.
 *
 *	Sometines a TEXTSTRING is actually a number (ie "15"C), and
 *	this function is used when a determination is necessary
 *	about the "really a string" quality is needed.
 */
int Node::IsReallyString(void)
{
	if (Type == BAS_V_TEXTSTRING)
	{
		if (*(TextValue.end() - 1) == '"')
		{
			return 1;
		}
	}
	return 0;
}

/**
 * \brief Is the node a Logical operation (Returns true/false indication)
 *
 *	This function is used to determine if a nodes is a logical
 *	operation (returning true/faslse), so that higher code can
 *	optimize itself to also be logical.
 *
 * \return 1 if logical, 0 otherwise
 */
int Node::IsLogical(void)
{
	switch(Type)
	{
	case '<':
	case '>':
	case BAS_X_NEQ:
	case BAS_X_GE:
	case BAS_X_LE:
	case '=':
	case BAS_X_LNOT:
	case BAS_X_LAND:
	case BAS_X_LOR:
		//
		// logical operations
		//
		return 1;

	case '(':
		//
		// Look beneath the skin
		//
		return Tree[0]->IsLogical();

	default:
		//
		// By default, things are not logical
		//
		return 0;
	}
}

/**
 * \brief Is the node a simple integer value
 *
 *	This function is used to determine if the node is a simple
 *	integer value (i.e. one which we can do a 'atoi' on and get
 *	correct results).
 *
 *	Used mostly for the array size definition routines when trying
 *	to increase the dimension sizes by one.
 */
int Node::IsSimpleInteger(void)
{
	//
	// Base type must be integer
	//
	if ((Type == BAS_V_INTEGER) || (Type == BAS_V_INT))
	{
		//
		// Has to start with a digit
		//
		if (isdigit(TextValue[0]))
		{
			//
			// has to end with a digit or a '%'
			//
			char LastChar = *(TextValue.end() - 1);
			if (isdigit(LastChar) || (LastChar == '%'))
			{
				return 1;
			}
		}
	}

	return 0;
}


/**
 * \brief Count the number of parameters for a function/array
 *
 *	This function is used to determine the number of parameters
 *	that a function/array uses.
 */
int Node::CountParam(void)
{
	int Counter;

	switch(Type)
	{
	case BAS_N_LIST:
		if (Tree[0] != 0)
		{
			Counter = Tree[0]->CountParam();
		}
		else
		{
			Counter = 1;		// Empty parameter
		}

		if (Tree[1] != 0)
		{
			Counter += Tree[1]->CountParam();
		}
		else
		{
			Counter++;		// Empty parameter
		}
		break;

	default:
		//
		// By default, things are not logical
		//
		Counter = 1;
	}
	return Counter;
}

/**
 * \brief Link two nodes together at same level.
 *
 *	This function will link two parent nodes together at the same
 *	tree level. If one or the other node doesn't exist, it will
 *	make whichever node exists the main node.
 *
 * This versions differs slightly from the class function of the same name.
 * It is necessary because the stupid optimizers don't think it is possible
 * to call a class function with a null class.
 *
 *	\return the top node.
 *
 */
Node *DownLink
(
 	Node *Node1,	/**< Main node to link */
	Node *Node0,	/**< Node to link in */
	int Ptr		/**< Slot to link it into */
)
{
	//
	// If root node is non-existant, go to 2ndnode
	//
	if (Node1 == 0)
	{
		return Node0;
	}

	//
	// Link them together
	//
	if (Node1->Block[Ptr] == 0)
	{
		Node1->Block[Ptr] = Node0;
	}
	else
	{
		DownLink(Node1->Block[Ptr], Node0, 0);
	}

	return Node1;
}

