//! file parser.ycc
//!
//! \brief parser for the Basic to C++ translator.
//!
//! This file contains the parser for the Basic to C++ translator.
//! It is used to convert Basic code to AST form (Abstract Syntax Tree).
//!
//! It is compiled using bisonc++
//!  apt-get install bisonc++
//!
//! \author Kevin Handy, June 2017
//!
%filenames parser
%scanner scanner.h


%

// Entire program file
program :
	lines
;

// Line in the program file.
// May contain multiple statements
line :
	line '/' statement
|	statement
|	'\n'
;

// Individual statement
statement:
	statementnomod
|	statementmod
 ;

// Statement that cannot take a modifier
statementnomod:
;

// Statement that can take a modifier
statementmod:
	statementmodone statementmodifiers
;

statementmodone:
;

statement modifiers:
;

