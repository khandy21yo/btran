/**
 * \file program.cc
 * \brief Transform tree to more usable format
 *
 *	This procedure scans a tree created by 'parse.y' and transforms
 *	items that are not handled well in the initial pass.
 *
 *	Such items include:
 *	* 'if ... then ... else ... endif' blocks
 *	* 'for ... next' blocks
 *	* 'function ... end-function' blocks
 *	* 'data' statements merged together
 *	* etc...
 */

/*
History:

	04/03/1997 - Kevin Handy
		Split several functions out of main.cc.

	04/06/1997 - Kevin Handy
		Added code to move local functions out
		of main function, and to move data statements
		to the top of a function.

	06/04/1997 - Kevin Handy
		Modified to merge DATA statements together.

	06/16/1997 - Kevin Handy
		Use BAS_S_MAINFUNCTION for the main function.

	06/16/1997 - Kevin Handy
		Set version to 1.4

	07/09/1997 - Kevin Handy
		Handle bug with multiple data statements.

	01/28/1998 - Kevin Handy
		Added code to make BAS_S_DEFSTAR just like BAS_S_DEF

	02/20/1998 - Kevin Handy
		Moved 'DEF*' to start of function code rather than
		outside of it.

	06/24/1998 - Kevin Handy
		Changed to handle 'Block' instead of 'Down', 'Tree[2]',
		and 'Tree[2]'

	09/04/1998 - Kevin Handy
		Change a lot of 'NULL' to '0' to lose conversion
		warning messages.

	09/29/1998 - Kevin Handy
		Changes towards handline def* functions better

	11/14/1998 - Kevin Handy
		Move definitions before code. Should help eliminate the
		'crosses initialization of' errors

	03/06/1999 - Kevin Handy
		Added code for WHEN ERROR IN ... USE ... END WHEN

	07/08/1999 - Kevin Handy
		Lose include for stdarg.h, which is not needed in this
		module.

	04/12/2000 - Kevin Handy
		Put titles on the tree dumps, also output tree after
		function scan.

	04/12/2000 - Kevin Handy
		Modify function scan to look into loops and if statements
		for definitions that need to be moved up also.

	10/10/2000 - Kevin Handy
		Fix handling of NEXT in UNTIL statement

*******************************************************************************/

//
// System Include Files
//
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <unistd.h>

//
// Project Include Files
//
#include "vartype.h"
#include "basic.h"
#include "variable.h"
#include "varlist.h"
#include "nodes.h"
#include "parse.h"

//
// Module Function Prototypes
//
static Node* ScanForLoops(Node* Program);
static Node* MoveFunctions(Node* Program);
static Node* MoveFunctionsOne(Node* Program);
static Node* MoveFunctionsTwo(Node* Program);

static Node* LocalFunctions;	//!< Holds local functions
static Node* LocalData;		//!< Holds local DATA statements
static Node* LocalDefstar;	//!< Holds DEF functions
static Node* LocalVars;		//!< Holds local variables


/**
 * \brief Main part of translater
 *
 *	This function does most of the work after the parser has
 *	generated the tree.
 *
 * \bug Not yet complete.
 */
void DoProgram(
	Node *Program	/**< Root node of program */
)
{

	//
	// Pretend it all started with a FUNCTION INTGER MAIN.
	// This creates a 'main' function for the program.
	//
	Node* BeginProgram = new Node(BAS_S_MAINFUNCTION);
	Node* BeginType = new Node(BAS_S_INTEGER);
	Node* BeginName = new Node(BAS_V_FUNCTION, "BASIC_MAIN");

	BeginProgram->Link(BeginType, 0, BeginName);
	BeginProgram->DownLink(Program);

	//
	// Dump out according to flags
	//
	if (DebugDump && (Program != 0))
	{
		std::cout << std::endl << "*** Dump Before Reprocessing Tree ***" << std::endl;
		BeginProgram->PrintTree();
	}

	//
	// Now, mangle the code to correctly block out
	// the program
	//
	if (PositionDump)
	{
		std::cerr << "Scanning for loops" << std::endl;
	}
	BeginProgram = ScanForLoops(BeginProgram);

	if (DebugDumpOne)
	{
		std::cout << std::endl << "*** Dump After Loop Scan ***" << std::endl;
		BeginProgram->PrintTree();
	}

	//
	// Ok, now let's move functions/data statements to the
	// top of each module.
	//
	if (PositionDump)
	{
		std::cerr << "Move Local Functions" << std::endl;
	}
	BeginProgram = MoveFunctions(BeginProgram);

	//
	// Output the translated code
	//
	if (PositionDump)
	{
		std::cerr << "Outputing code" << std::endl;
	}

	if (DebugDumpOne)
	{
		std::cout << std::endl << "*** Dump After Function Scan ***" << std::endl;
		BeginProgram->PrintTree();
	}
	BeginProgram->Output(*OutFile);

	//
	// Free up all memory allocated
	//
	delete BeginProgram;
}

/**
 * \brief Fix up loops, if-then-else, functions.
 *
 *	This function scans through the program, fixing up for-next loops,
 *	if-then-else statements, while statements, program segments, etc.
 *	Since the parser doesn't make complete loops for anything except
 *	modified statements, we need to roll the statements together
 *	here.
 *
 * \bug Goes internally into the tree to restructure the code instead of
 *	using the interface code.
 */
static Node *ScanForLoops
(
	Node *Program		/**< root node of program */
)
{
	//
	// Local Variables
	//
	Node *ThisTree = Program;
	Node *NextTree;
	Node *IndentTree[128];
	int IndentType[128];
	int ThisLevel = 0;

	//
	// Start the whole mass going
	//
	assert(Program != 0);
	IndentTree[0] = 0;

	//
	// Scan through the program, looking for loops at level 0.
	//
	while (ThisTree != 0)
	{
		//
		// Remember what the next level will be
		//
		NextTree = ThisTree->GetDown();
		ThisTree->UnDownLink();

		//
		// Preview this node, and decide what to do with it.
		//
		switch(ThisTree->GetType())
		{
		case BAS_S_DATA:
			//
			// Force data statements to main statement level
			// (0 being function headers) whenever possible.
			//
			if (ThisLevel > 0)
			{
				if (IndentTree[1] == 0)
				{
					IndentTree[0]->Link(0, 0, 0, ThisTree);
					IndentTree[1] = ThisTree;
				}
				else
				{
					IndentTree[1]->DownLink(ThisTree);
				}
				goto reloop;
			}
			break;

		case BAS_S_THEN:
			//
			// We don't need to keep THEN's at all.
			//
			delete ThisTree;
			goto reloop;

		case BAS_S_ELSE:
			//
			// Look for matching IF
			//
			while ((ThisLevel > 0) &&
				((IndentType[ThisLevel-1] != BAS_S_IF) ||
				(IndentTree[ThisLevel-1]->GetDown(2) != 0)) &&
				((IndentType[ThisLevel-1] != BAS_S_UNLESS) ||
				(IndentTree[ThisLevel-1]->GetDown(2) != 0)))
			{
				ThisLevel--;
			}

			//
			// We better be in an if statement
			//
			if ((IndentType[ThisLevel-1] == BAS_S_IF) ||
				(IndentType[ThisLevel-1] == BAS_S_UNLESS))
			{
				//
				// Use the ELSE node as a hook to this
				// section.
				//
				assert(IndentTree[ThisLevel-1]->GetDown(2) == 0);
				IndentTree[ThisLevel-1]->DownLink(ThisTree, 2);
				IndentTree[ThisLevel] = ThisTree;
			}
			else
			{
				std::cerr << "Missing IF for ELSE" << std::endl;
			}

			goto reloop;

		case BAS_S_USE:
			//
			// Look for matching WHEN ERROR IN
			//
			while ((ThisLevel > 0) &&
				((IndentType[ThisLevel-1] != BAS_N_WHENERRORIN) ||
				(IndentTree[ThisLevel-1]->GetDown(2) != 0)))
			{
				ThisLevel--;
			}

			//
			// We better be in an if statement
			//
			if (IndentType[ThisLevel-1] == BAS_N_WHENERRORIN)
			{
				//
				// Use the USE node as a hook to this
				// section.
				//
				assert(IndentTree[ThisLevel-1]->GetDown(2) == 0);
				IndentTree[ThisLevel-1]->DownLink(ThisTree, 2);
				IndentTree[ThisLevel] = ThisTree;
			}
			else
			{
				std::cerr << "Missing WHEN ERROR IN for USE" << std::endl;
			}

			goto reloop;

		case BAS_S_NEXT:
			//
			// Next statement
			//
			// Look for previous "for" or "while". This may cause
			// IF's to terminate early, but if the program is
			// structured correctly we will only go back one
			// level.
			//
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_FOR) &&
				(IndentType[ThisLevel-1] != BAS_S_WHILE) &&
				(IndentType[ThisLevel-1] != BAS_S_UNTIL) &&
				(IndentType[ThisLevel-1] != BAS_N_FORUNTIL) &&
				(IndentType[ThisLevel-1] != BAS_N_FORWHILE))
			{
				ThisLevel--;
			}

			//
			// Now back up one level
			//
			if (ThisLevel > 0)
			{
				ThisLevel--;
			}

			//
			// We don't need to keep NEXT any longer
			//
			delete ThisTree;
			goto reloop;

		case BAS_N_ENDRECORD:
			//
			// END RECORD statement
			//
			// Look for previous "RECORD".
			//
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_RECORD))
			{
				ThisLevel--;
			}

			//
			// Now back up one level
			//
			if (ThisLevel > 0)
			{
				ThisLevel--;
			}

			//
			// We don't need to keep 'END RECORD' any longer
			//
			delete ThisTree;
			goto reloop;

		case BAS_N_ENDVARIANT:
			//
			// END VARIANT statement
			//
			// Look for previous "VARIANT".
			//
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_VARIANT))
			{
				ThisLevel--;
			}

			//
			// Now back up one level
			//
			if (ThisLevel > 0)
			{
				ThisLevel--;
			}

			//
			// We don't need to keep 'END VARIANT' any longer
			//
			delete ThisTree;
			goto reloop;

		case BAS_S_FUNCTIONEND:
		case BAS_S_SUBEND:
		case BAS_V_ENDHANDLER:

			//
			// End a function/subroutine
			//
			// Force back to lowest level
			//
			ThisLevel = 0;

			//
			// We don't need to keep 'END SUB' any longer
			//
			delete ThisTree;
			goto reloop;

		case BAS_S_FNEND:
			//
			// End an internal function
			//
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_DEF) &&
				(IndentType[ThisLevel-1] != BAS_S_DEFSTAR))
			{
				ThisLevel--;
			}

			//
			// Now back up one level
			//
			if (ThisLevel > 0)
			{
				ThisLevel--;
			}

			//
			// We don't need to keep FNEND any longer
			//
			delete ThisTree;
			goto reloop;

		case BAS_V_LABEL:
			//
			// A line number will terminate any open IF
			// statements. I'll assume they are all at the
			// top of the stack.
			//
			if (isdigit(ThisTree->GetText()[0]))
			{
				while ((ThisLevel > 0) &&
					((IndentType[ThisLevel-1] == BAS_S_IF) ||
					(IndentType[ThisLevel-1] == BAS_S_UNLESS)))
				{
					ThisLevel--;
				}
			}

			//
			// We need to keep line number.
			//
			break;

		case BAS_N_ENDIF:
			//
			// end-if
			//
			if ((ThisLevel > 0) &&
				((IndentType[ThisLevel-1] == BAS_S_IF) ||
				(IndentType[ThisLevel-1] == BAS_S_UNLESS)))
			{
				ThisLevel--;
			}

			//
			// Lose the 'END IF' statement.
			//
			delete ThisTree;
			goto reloop;

		case BAS_N_ENDWHEN:
			//
			// end-when
			//
			if ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] == BAS_N_WHENERRORIN))
			{
				ThisLevel--;
			}

			//
			// Lose the 'END WHEN' statement.
			//
			delete ThisTree;
			goto reloop;

		case BAS_S_CASE:
		case BAS_N_CASEELSE:
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_SELECT) &&
				(IndentType[ThisLevel-1] != BAS_S_VARIANT))
			{
				ThisLevel--;
			}

			break;

		case BAS_N_ENDSELECT:
			while ((ThisLevel > 0) &&
				(IndentType[ThisLevel-1] != BAS_S_SELECT))
			{
				ThisLevel--;
			}

			if (ThisLevel > 0)
			{
				ThisLevel--;
			}

			//
			// Lose the 'END SELECT'
			//
			delete ThisTree;
			goto reloop;

		case BAS_S_MAINFUNCTION:
		case BAS_S_FUNCTION:
		case BAS_S_SUB:
		case BAS_S_HANDLER:

			if (ThisTree->GetDown(1) == 0)
			{
				//
				// A Function or Subroutine
				//
				ThisLevel = 0;
			}
			break;
		}

		//
		// Connect this node into the current level
		//
		if (IndentTree[ThisLevel] == 0)
		{
			//
			// If we start with a empty level,
			// we need to hook this
			// node down to the previous level.
			//
			if (ThisLevel != 0)
			{
				assert(IndentTree[ThisLevel-1]->GetDown(1) == 0);
				IndentTree[ThisLevel-1]->DownLink(ThisTree, 1);
			}
		}
		else
		{
			//
			// Link Nodes to the previous node on same level
			//
			IndentTree[ThisLevel]->DownLink(ThisTree);
		}

		IndentTree[ThisLevel] = ThisTree;

		//
		// Look at this node, and decide what to do with it
		//
		switch(ThisTree->GetType())
		{
		case BAS_S_IF:
		case BAS_S_UNLESS:
		case BAS_S_FUNCTION:
		case BAS_S_MAINFUNCTION:
		case BAS_S_SUB:
		case BAS_S_FOR:
		case BAS_N_FORUNTIL:
		case BAS_N_FORWHILE:
		case BAS_S_UNTIL:
		case BAS_S_WHILE:
		case BAS_S_DEF:
		case BAS_S_DEFSTAR:
		case BAS_N_WHENERRORIN:
		case BAS_S_HANDLER:

			if (ThisTree->GetDown(1) == 0)
			{
				IndentType[ThisLevel] = ThisTree->GetType();
				ThisLevel++;
				IndentTree[ThisLevel] = 0;
			}
			break;

		case BAS_S_CASE:
		case BAS_N_CASEELSE:
		case BAS_S_SELECT:
		case BAS_S_RECORD:
		case BAS_S_VARIANT:

			assert(ThisTree->GetDown(1) == 0);

			IndentType[ThisLevel] = ThisTree->GetType();
			ThisLevel++;
			IndentTree[ThisLevel] = 0;
			break;
		}

		//
		// Point to next node
		//
reloop:
		ThisTree = NextTree;
	}

	//
	// Done. Hand back re-structured tree.
	//
	return(Program);
}


/**
 * \brief Move local functions
 *
 *	This function moves local functions and data statements
 *	to the beginning of the each major function.
 *	This simplifies the work needed to be done during code
 *	generation.
 *
 * \bug Probably ought to be part of ScanForLoops, but I tried
 *	to get it into there a couple of times and failed
 *	horribly.
 */
static Node* MoveFunctions(
	Node* Program		/**< Main node for the program */
)
{
	Node* FinalTree = 0;
	Node* ThisTree = Program;

	//
	// The main functions are all defined in the first layor of
	// the tree, so we want to block things out function by
	// function.
	//
	while (ThisTree != 0)
	{
		//
		// Keep track of the next node to look at
		//	
		Node* NextTree = ThisTree->GetDown();
		ThisTree->UnDownLink();

		//
		// We'll assume that all the (Level 0) nodes
		// are function definitions, and progrss them
		// accordingly
		//
		ThisTree = MoveFunctionsOne(ThisTree);
		if (FinalTree == 0)
		{
			FinalTree = ThisTree;
		}
		else
		{
			FinalTree->DownLink(ThisTree);
		}

		//
		// Go to next node
		//
		ThisTree = NextTree;
	}
	return FinalTree;
}

/**
 * \brief Helper function for MoveFunctions()
 */
static Node* MoveFunctionsOne(
	Node* Program		/**< Pointer to program section */
)
{
	LocalFunctions = 0;
	LocalData = 0;
	LocalDefstar = 0;
	LocalVars = 0;

	Node* LocalCode = MoveFunctionsTwo(Program->GetDown(1));

	//
	// Now lets hook the various nodes together, and
	// attach then to the programs code.
	//
	Program->UnDownLink(1);
	Node* FinalProgram = LocalData->DownLink(
		LocalFunctions->DownLink(
		Program->DownLink(LocalVars, 1)));
	Program->DownLink(LocalDefstar, 2);
	Program->DownLink(LocalCode, 1);

	//
	// Return back restructured code
	//
	return FinalProgram;
}

/**
 * \brief Handle one block of code
 */
static Node* MoveFunctionsTwo(
	Node* Program		/**< Pointer to program section */
)
{
	Node* ThisCode = Program;
	Node* LocalCode = 0;
	Node* InnerCode;

	while (ThisCode != 0)
	{
		//
		// Keep track of what the next statement to look at will be
		// and disconnect then statement from the next.
		//
		Node* NextCode = ThisCode->GetDown();
		ThisCode->UnDownLink();

		//
		// Determine which code sequence to attach this
		// node onto.
		//
		switch(ThisCode->GetType())
		{
		case BAS_S_DEF:
			//
			// Attach local functions to the function tree
			//
			if (LocalFunctions == 0)
			{
				LocalFunctions = ThisCode;
			}
			else
			{
				LocalFunctions->DownLink(ThisCode);
			}
			break;

		case BAS_S_DEFSTAR:
			//
			// Attach local functions to the function tree
			//
			if (LocalDefstar == 0)
			{
				LocalDefstar = ThisCode;
			}
			else
			{
				LocalDefstar->DownLink(ThisCode);
			}
			break;

		case BAS_S_DATA:
			//
			// Attach data statements to the data tree
			//
			if (LocalData == 0)
			{
				LocalData = ThisCode;
			}
			else
			{
				Node* TailNode = LocalData;
				while (TailNode->GetTree(1) != 0)
				{
					TailNode = TailNode->GetTree(1);
				}
				TailNode->Link(0, ThisCode);
			}
			break;

		case BAS_S_COMMON:
		case BAS_S_DECLARE:
		case BAS_S_DIM:
		case BAS_S_EXTERNAL:
		case BAS_S_MAP:
		case BAS_S_VARIANT:
			//
			// Attach local functions to the function tree
			//
			if (LocalVars == 0)
			{
				LocalVars = ThisCode;
			}
			else
			{
				LocalVars->DownLink(ThisCode);
			}
			break;

		case BAS_S_FOR:
		case BAS_S_WHILE:
		case BAS_N_FORUNTIL:
		case BAS_N_FORWHILE:
		case BAS_S_CASE:
		case BAS_S_SELECT:

			//
			// Process the inner block of a loop
			//
			if ((InnerCode = ThisCode->GetDown(1)) != 0)
			{
				ThisCode->UnDownLink(1);
				InnerCode = MoveFunctionsTwo(InnerCode);
				ThisCode->DownLink(InnerCode, 1);
			}

			//
			// Connect up statement
			//
			if (LocalCode == 0)
			{
				LocalCode = ThisCode;
			}
			else
			{
				LocalCode->DownLink(ThisCode);
			}
			break;

		case BAS_S_IF:
			//
			// Process the then block
			//
			if ((InnerCode = ThisCode->GetDown(1)) != 0)
			{
				ThisCode->UnDownLink(1);
				InnerCode = MoveFunctionsTwo(InnerCode);
				ThisCode->DownLink(InnerCode, 1);
			}

			//
			// Process the else block
			//
			if ((InnerCode = ThisCode->GetDown(2)) != 0)
			{
				ThisCode->UnDownLink(2);
				InnerCode = MoveFunctionsTwo(InnerCode);
				ThisCode->DownLink(InnerCode, 2);
			}

			//
			// Connect up statement
			//
			if (LocalCode == 0)
			{
				LocalCode = ThisCode;
			}
			else
			{
				LocalCode->DownLink(ThisCode);
			}
			break;

		default:
			//
			// By default, everything sticks to the main
			// code sequence
			//
			if (LocalCode == 0)
			{
				LocalCode = ThisCode;
			}
			else
			{
				LocalCode->DownLink(ThisCode);
			}
			break;
		}

		//
		// Go to next node down
		//
		ThisCode = NextCode;
	}

	return LocalCode;
}

