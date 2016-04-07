/** \file bstring.h
 * \brief String class for Basic to C++ translator
 *
 *	This file impliments a string class for my Basic
 *	translater using VMS style descriptors and string
 *	functions. This allows the programs to interface
 *	with the VMS routines as easily as under the
 *	real VAX/BASIC compiler.
 */
#ifndef _bstring_h_
#define _bstring_h_

//
// Include files
//
#include <cstdio>
#include <cctype>
#include <cassert>
#include <string>
#include <iostream>

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#ifndef MAX_INPUT
/**
 * \brief Maximum number of characters allowed for read statements
 * \note Sometimes it's defined by the enviornment,
 * sometimes it isn't
 */
#define MAX_INPUT 128
#endif

//******************************************************************
// This version of the strings use the STL string class for its base
//
namespace basic
{
/**
 * \brief LEFT$()
 *
 * Return the left part of a string.
 *
 * \return Left part of the string
 */
inline std::string left(
	const std::string& Source,	/**< Original string */
	long Length			/**< Characters to take */
	)
	{ return std::string(Source, 0, Length); }

/**
 * \brief RIGHT$()
 *
 * Return the right part of a string
 *
 * If you use substr past the end of the string, it crashes.
 *
 * \return Right part of string
 */
inline std::string right(
	const std::string& Source,	/**< Original string */
	long Length			/**< Characters to skip over */
)
{
	if (Length <= 0)
	{
		Length = 1;
	}
	if (Length > 0 && Length <= Source.size())
	{
		return std::string(Source, Length - 1);
	}
	else
	{
		return "";
	}
}

/**
 * \brief SEG$()
 * \return substring
 */
inline std::string Qseg(
	const std::string& Source,	/**< Original string */
	long Start,			/**< Start position */
	long End			/**< End position */
	)
{
	if (Start <= 0)
	{
		Start = 1;
	}
	if (Start < Source.size())
	{
		return std::string(Source, Start - 1, End - Start + 1);
	}
	else
	{
		return "";
	}
}

//! MID$()
inline std::string mid(const std::string& Source, long Start, long Length)
{
	if (Start <= 0)
	{
		Start = 1;
	}
	if (Start < Source.size())
	{
		return std::string(Source, Start - 1, Length);
	}
	else
	{
		return "";
	}
}
//! TRM$()
inline std::string trm(const std::string& Source)
	{ std::string work = Source;
	work.erase(work.find_last_not_of(" \t") + 1);
	return work; }

//
// Various string type functions
//
int instr(unsigned long Start, char* Base, char* Search);
std::string &edit(const std::string& Source, const int Code);
std::string sys(const std::string& Source);
long cvtai(std::string x);
std::string cvtia(int x);
double cvtaf(std::string x);
std::string cvtfa(double x);
int unlink(std::string const &);
int atoi(const std::string&);

//! LENGTH()
inline long strlen(const std::string& Source) { return Source.length(); }
//! LENGTH()
inline long length(const std::string& Source) { return Source.length(); }
//! ASCII()
inline int Ascii(const char x) { return x; }
//! ASCII()
inline int Ascii(const char* x) { return *x; }
//! ASCII()
inline int Ascii(const std::string& Source) { return Source[0]; }

//! STRING$()
inline std::string Qstring(long Count, int Character)
	{ return std::string(Count, (char)Character); }
//! CHAR$()
inline std::string Char(int Character) { return std::string(1, (const char)Character); }
//! SPACE$()
inline std::string Qspace(long Count) { return std::string(Count, ' '); }

std::string Qnum(long Number);
//! NUM$()
inline std::string Qnum(int Number) { return Qnum((long)Number); }
std::string Qnum(double Number);
std::string Qnum1(long Number);
//! NUM1$()
inline std::string Qnum1(int Number) { return Qnum1((long)Number); }
std::string Qnum1(double Number);

//! STR$()
inline std::string str(long Number) { return Qnum1(Number); }
//! STR$()
inline std::string str(double Number) { return Qnum1(Number); }

//! INSTR()
inline int instr(unsigned long Start, const std::string& BaseString,
	const std::string& SearchString)
	{ return BaseString.find(SearchString, Start - 1) + 1; }
//! POS()
inline int pos(char* Base, char* Search, unsigned long Start)
	{ return instr(Start - 1, Base, Search) + 1; }
//! POS()
inline int pos(const std::string& BaseString,const std::string& SearchString,
	unsigned long Start)
	{ return BaseString.find(SearchString, Start - 1) + 1; }
//! VAL()
inline double atof(const std::string& Source)
	{ return strtod(Source.c_str(), 0); }

std::string Qrad(const unsigned int Number);
std::string& ert(const long Number);

void BChange1(const std::string& source, int *dest);
void BChange1(const char* source, int *dest);
void BChange1(const std::string& source, long *dest);
void BChange1(const char* source, long *dest);
void BChange1(const std::string& source, double *dest);
void BChange1(const char* source, double *dest);
void BChange2(const int *source, std::string& dest);
void BChange2(const long *source, std::string& dest);
void BChange2(const double *source, std::string& dest);

std::string Inkey(long channel, long wait = 0);
}

/** \brief basic++ lset operation
 *
 * Copies as much od the source string into the dest string
 * and pads the dest with spaces.
 * The dest string will retain its otiginal size.
 */
static inline void Lset
(
	std::string &dest,		/**< String to modify */
	const std::string &source	/**< String to copy into dest */
)
{
	for (int loop = 0; loop < dest.size(); loop++)
	{
		if (loop < source.size())
		{
			dest[loop] = source[loop];
		}
		else
		{
			dest[loop] = ' ';
		}
	}
}

/** \brief basic++ rset operation
 *
 * Copies as much od the source string into the dest string
 * and pads the front of the dest with spaces.
 * The dest string will retain its otiginal size.
 */
static inline void Rset
(
	std::string &dest,		/**< String to modify */
	const std::string &source	/**< String to copy into dest */
)
{
	int start = dest.size() - source.size();
	if (start < 0)
	{
		start = 0;
	}
	for (int loop = 0; loop < dest.size(); loop++)
	{
		if (loop >= start)
		{
			dest[loop] = source[loop - start];
		}
		else
		{
			dest[loop] = ' ';
		}
	}
}
#endif

