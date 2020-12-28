//!\file
//!\brief Lexical analysis
//!
//! These classes are used to vreak source code into individual tokens.
//! The tokens returned can vary depending on the state of the parser.
//! For eample, DATA statements will lex tokens dirrerently than an
//! expression would, or an OPTION statement would.
//!
//! Tokens must be able to be eamined multiple values 

//
// Read an entire file
//
std::string getFileContent(const std::string& path)
{
	std::ifstream file(path);
	std::string content((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());

	return content;
}



// Lexical scanner
//
//
//


// Float: [-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?


class lex_pattern
{
private:
	regex pattern;		//!< Search pattern
	int basevalue;		//!< Default parse value
	std::string codestring;	//!< Default code string
	int flags;		//!< Special flags

public:
	lex_pattern(
		const std::string &ppattern,
		int pbasevalue,
		const std::string &pcodestring = "",
		int pflag = 0)
	{
		pattern = ppattern;
		basevalue = pbasevalue;
		codestring = pcodestring;
		flag = pflag;
	}

};

class lex_list : public std::vector<lex_pattern>
{
};

class lexer
{
private:
	std::InputIterator position;
punlic:
	lexer(std::InputIterator *start))
	{
		position = start;
	}
	lex_reg();
	lex_data();
};
