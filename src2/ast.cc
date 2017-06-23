//! \file ast.cc
//!
//! \brief Code for the abstract syntax tree clases.
//!
//! \author Kevin Handy, June 2017

#include <iostream>
#include "ast.h"

//**********************************************************************
// class ast
//

//! \brief dump routine.
//!
//! Dumps out the contents of the class to standard output.
//! This is for debugging purposes.
//!
//! All of the dump() routines for inherited classes are written as self
//! contained routines (not calling each previous level).
//! This just makes debugging a little bit simppler.
//!
void ast::dump() const
{
	//
	// We output in a rather simple format
	//
	std::cout << "(ast " << d_code << " " << d_text << ") ";
}

//**********************************************************************
// class aststmt
//

//! \brief dump routine.
//!
//! Dumps out the contents of the class to standard output.
//! This is for debugging purposes.
//!
void aststmt::dump() const
{
	//
	// We output in a rather simple format
	//
	std::cout << "(aststmt " << get_code() << " " << get_text() << ")\n";
	if (d_next != 0)
	{
		d_next->dump();
	}
}

//**********************************************************************
// class astexpr
//

//! \brief dump routine.
//!
//! Dumps out the contents of the class to standard output.
//! This is for debugging purposes.
//!
void astexpr::dump() const
{
	//
	// We output in a rather simple format
	//
	std::cout << "(astexpr " << get_code() << " " << get_text() << ")\n";
}


//**********************************************************************
// class astexprlist
//

//! \brief dump routine.
//!
//! Dumps out the contents of the class to standard output.
//! This is for debugging purposes.
//!
void astexprlist::dump() const
{
	//
	// We output in a rather simple format
	//
	std::cout << "(astexprlist " << get_code() << " " << get_text() << " ";
       
	for (auto loop = d_list.begin();
		loop != d_list.end();
		loop++)
	{
		if (loop != d_list.begin())
		{
			std::cout << ", ";
		}

		if (*loop == 0)
		{
			std::cout << "<null>";
		}
		else
		{
			(*loop)->dump();
		}
	}

	std::cout << ")\n";
}

//**********************************************************************
// class astexprvar
//

//**********************************************************************
// class varlistelement
//

//**********************************************************************
// class varlist
//


