//! \file ast.h
//!
//! This file contains te Abstract Syntax Tree (AST) classes for the
//! Basic to C++ translator project.
//!
//! \author Kevin Handy, Jun 2017
//!
#ifndef __ast_h__
#define __ast_h__

#include <string>
#include <vector>


//**********************************************************************
//! \brief Base ast class
//!
//! This is the base class for all ast's. Not very useful in itself.
//!
class ast
{
private:
	int d_code;		//!< Code value for this ast node,
	std::string d_text;	//!< Text value associated with this ast node

public:
	//!< Constructor
	ast(
		int code,	//!< AST node value. Specifies what type of
				//! code this node holds, such as a 'FOR LOOP',
				//! 'LABEL', ASSIGNMENT, etc.
		const std::string &text = "") :
				//!< Text value associated with this node.
				//! Value depends on the code.
		d_code{code},
		d_text{text}
	{
	}

	//!\brief accessor function for code
	inline int get_code() const
	{
		return d_code;
	}

	//!\brief accessor function for text
	const inline std::string &get_text() const
	{
		return d_text;
	}

	virtual void dump(void) const;
};

//**********************************************************************
//! \brief AST statement class
//!
//! Class used for simple statements.
//!
class aststmt : public ast
{
private:
	aststmt *d_next;	//!< Next statemet

public:
	//!< Constructor
	aststmt(
		int code,	//!< AST node value. Specifies what type of
				//! code this node holds, such as a 'FOR LOOP',
				//! 'LABEL', ASSIGNMENT, etc.
		const std::string &text = "",
				//!< Text value associated with this node.
				//! Value depends on the code.
		aststmt *nexts = 0) :
				//!< Pointer to next statement.
		ast {code, text}, d_next {nexts}
	{
	}

	virtual void dump(void) const;
};

//**********************************************************************
//! \brief AST expression class
//!
//! Class used for simple expressions.
//!
class astexpr : public ast
{
private:
	int basetype;		//!< Base type of this expression.
				//! i.e. string, integer, float, etc.
				//! This is usually assigned after te complete
				//! ast is created.

public:
	//!< Constructor
	astexpr(
		int code,	//!< AST node value. Specifies what type of
				//! code this node holds, such as a 'FOR LOOP',
				//! 'LABEL', ASSIGNMENT, etc.
		const std::string &text = "") :
				//!< Text value associated with this node.
				//! Value depends on the code.
				//!< Pointer to next statement.
		ast {code, text}, basetype {0}
	{
	}

	virtual void dump(void) const;
};

//**********************************************************************
//! \brief AST expression list lass
//!
//! Used to hold a list of several expressions, such as function parameters,
//! array references, etc.
//!
class astexprlist : public astexpr
{
private:
	int x;
	std::vector<astexpr *> d_list;
				//!< Vector holding array of expressions.
				//! Values that are empty in the basic are
				//! passed a 0 pointers.
public:
	astexprlist(
		int code,	//!< AST node value. Specifies what type of
				//! code this node holds, such as a 'FOR LOOP',
				//! 'LABEL', ASSIGNMENT, etc.
		const std::string &text = "") :
				//!< Text value associated with this node.
				//! Value depends on the code.
				//!< Pointer to next statement.
		astexpr {code, text}
	{
	}


	virtual void dump(void) const;
};

//**********************************************************************
//! \brief AST variable expression class
//!
//! Class used for variable references.
//!
class astexprvar : public astexpr
{
private:
	astexprlist *array;	//! Pointer to array elements.
				//!! 0 if not an array.
	astexprvar *reference;	//!< Pointer to record members (::b of a::b).
				//! 0 if not a reference.

public:
	//!< Constructor
	astexprvar(
		int code,	//!< AST node value. Specifies what type of
				//! code this node holds, such as a 'FOR LOOP',
				//! 'LABEL', ASSIGNMENT, etc.
		const std::string &text = "") :
				//!< Text value associated with this node.
				//! Value depends on the code.
				//!< Pointer to next statement.
		astexpr {code, text}
	{
	}

	virtual void dump(void) const;
};

//**********************************************************************
//! \brief Variable list element
//!
//! Contains one variable list element.
//!
class varlistelement
{
private:
	astexprvar d_vardef;	//!< Comtains the variable name and context
	int d_vartype;		//!< Type of variable
	bool d_static;		//!< DOes it need to be made static
	bool d_used;		//!< Has it beed refered to outside of a
				//   definition.
	bool d_defined;		//!< Does it have a definition statement.

public:
	virtual void dump(void) const;
};


//**********************************************************************
//! \brief Variable list
//!
//! List of variables
//!
class varlist : public std::vector<varlistelement>
{
private:
	varlist *d_previous;	//!< Link to previous variable list

public:
	virtual void dump(void) const;
};

//**********************************************************************
//! \brief AST procedure class
//!
//! Class used for a procedure block
//!
class astprocedure : public aststmt
{
private:
	varlist d_varlist;		//!< Variables local to block

public:
	virtual void dump(void) const;
};

#endif

