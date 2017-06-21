//! \file ast.h
//!
//! This file contains te Abstract Syntax Tree (AST) classes for the
//! Basic to C++ translator project.
//!
//! \author Kevin Handy, Jun 2017
//!
#ifndef __ast_h__
#define __ast_h__

//**********************************************************************
//! \brief Base ast class
//!
//! This is the base class for all ast's. Not very useful in itself.
//!
class ast
{
privast:
	int d_code;		//!< Code value for this ast node,
	std::string d_text;	//!< Text value associated with this ast node

public:
	//!< Constructor
	ast(
		int value,
		const std::string &&&text) :
		d_code{cde},
		d_text{text}
	{
	}

}'
#endif

