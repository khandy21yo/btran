/**\file varlist.h
 * \brief Header file for variable table
 *
 *	This is the header file for the variable table/
 *
 * \bug Lousy. Needs complete re-write
 */

//
// Variable.h - Some stuff just in case if isn't defined anywhere else
//
#ifndef _VARLIST_H_
#define _VARLIST_H_

//
// Include files
//
#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "variable.h"
#include "nodes.h"

std::string genname(const std::string &basename);

/**
 * \brief Contains a list of variables at one block level
 *
 * This class contains a list of all variables that are defined at one
 * block level.
 * This class is contained in VariableList.
 */
class ListOfVariables:public std::map < std::string, VariableStruct, std::less<std::string> >
{
public:
	// Look for C version of Basic name
	VariableStruct *LookupCpp(const std::string &VarName);
	// Dump out to console list of all variables
	void Dump(void);
	// Output C++ definition for a variable
	std::string OutputDef(std::ostream& os, int Level);
};

/**
 * \brief Class that holds a list of variable block levels
 *
 * This class contains a list of block levels.
 * Each time a new block level starts (at the beginning of a function
 * or subroutine), a new level is added to the list. on exit of that
 * level the block level is popped off the stack.
 * Searching for a variable starts with the most recenty added block
 * level and works back to the oldest.
 */
class VariableList : public std::vector<ListOfVariables>
{
public:
	int vcount;			/**< \brief Variable sequence counter */

public:
	// Constructor
	VariableList(void);

	// Add a new block level to variable list
	void NewLevel();
	// Drop top block level
	void KillLevel();
	// Load up (hardcoded) list of functions to variable list
	void InitFunctions(void);
	// Function used by InitFunctions to load each function
	void InitOneFunction(const std::string &BasName, const char *CeeName, VARTYPE Type,
		VARCLASS Class=VARCLASS_FUNC);

	// Add q new variable to the variable table
	void Append(const VariableStruct& Variable, int GlobalFlag = false);
	// Search for a variable by name/type
	VariableStruct* Lookup(std::string &VarName, const Node *ArrayRef,
		int ScanOneLevel = 0);
	// Search for a variable by C name
	VariableStruct* LookupCpp(const std::string &VarName);
	// Compare a possible variable name to a list of reserved C++ words
	int LookupReserved(const char *VarName);
	// Compare a possible variable name to a list of reserved C++ words
	int LookupReserved(const std::string &VarName);
	// Create a variable definition
	void Define(Node *ThisNode, int GlobalFlag = false,
		int ThisOutput = false);
	// Dump out debugging version of the variable table
	void Dump();
	// Create C++ names for all variables not yet processed
	void Fixup();
	/**
	 * \brief Output definition
	 */
	std::string OutputDef(
		std::ostream& os,		/**< Output Stream */
		int Level			/**< identation level */
	)
	{ return back().OutputDef(os, Level); }

private:
	// Used during the creation of C++ names
	void Fixup1(ListOfVariables* Variable);
};

#endif
