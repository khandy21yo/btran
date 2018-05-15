//! \file
//! \brief lexer for btran 2
//!
//! This file contains the lexer for btran 2.
//! This is a hand-written lexee.
//! Getting other lexers to handle DATA statements properly was extremely
//! difficult.
//!
//! \author Kevin Handy 2018
//!

#include <string>


//!
//! \brief pull one item off a data statement.
//!
std::string LexData(
	std::string &source,	//!< String being parsed
	int &offset)		//!< Current offset into source string
{
}

//!
//! \brief pull one (raw) token off the input stream.
//!
std::string LexToken(
	int &returntype;	//!< Type of token if known from format.
	std::string &source,	//!< String being parsed
	int &offset)		//!< Current offset into source string
{
	std::string result;

	char ch = source[offset];

	while (offset < source.size())	
	{
		//
		// Skip whitespace.
		//
		if (isblank(ch))
		{
			offset++;
			continue;
		}

		//
		// Line terminator
		//
		if (ch == '\n' || ch == '\f'))
		{
			offset++;
			return "\n";
		}

		//
		// Leading "alpha" characters.
		// Likely o token, but might be something special.
		//
		if (isalpha(ch) || ch == '%')
		{
			result = LexTokenAlpha(ch, returntype, source, offset);
			return result;
		}

		//
		// Raw number
		//
		if (isdigit(ch))
		{
abort(1);
		}

		//
		// Quoted string
		//
		if (ch == '"' || ch == '\'')
		{
abort(1);
		}

		//
		// Comment
		//
		if (ch == '!')
		{
abort(1);
		}

		//
		// Anything else. Assume single character token.
		//
		offset++;
		return ch;
	}

	//
	// Nothing to return
	//
	return "";
}
