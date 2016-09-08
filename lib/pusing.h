/**\file pusing.h
 * \brief Header file for print using class.
 */

//
// History
//
//	04/26/1997 - Kevin Handy
//		Initial version
//
//	03/18/2000 - Kevin Handy
//		Added SetFormat(std::string)
//

#ifndef _pusing_h_
#define _pusing_h_

#include "bstring.h"

namespace basic
{
/**
 * \brief Print using class
 *
 * Handles "PRINT USING" statements
 */
class PUsing
{
private:
	const char* BaseFormat;		//!< Pointer to original format string
	int BaseLength;			//!< Length of format string (Mat not be
					//!<   null terminated)
	int BasePtr;			//!< Current pointer into format
	char* Outdata;			//!< Pointer to output string being built
	char* OutdataPtr;		//!< Pointer to end of Output

public:
	PUsing();			// Constructor
	PUsing(const char* Format);	// Constructor with a base format
	PUsing(const std::string& Format);	// Constructor with a base format

	//! Set format string
	void SetFormat(const char* Format)
		{ BaseFormat = Format; BaseLength = strlen(Format);
		BasePtr = 0; }
	//! Set format string
	void SetFormat(const std::string& Format)
		{ BaseFormat = Format.c_str(); BaseLength = Format.length();
		BasePtr = 0; }

	//
	// Various output commands
	//
	std::string Output(const char* Value, int ValueLength);
	//! Output character string value
	inline std::string Output(const char* Value)
		{ return Output(Value, strlen(Value)); }
	std::string Output(const std::string& Value);
	std::string Output(const double Value);
	std::string Output(const int Value);
	std::string Output(const long Value);

	std::string Finish();		// Final characters in format string

private:
	int SkipFront();
	void ScanStringFormat(int &FWidth, int &FSide);
	void FormatString(const char* ParmPtr, int ParmLength,
		int Length, int Side);
};

//
// Variations of the "FORMAT$(value, format)" command
//
//! FORMAT$(string,string)
inline std::string Format(const std::string& Value, const std::string& Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(string,char)
inline std::string Format(const std::string& Value, const char* Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(char,char)
inline std::string Format(const char* Value, const char* Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(char,string)
inline std::string Format(const char* Value, const std::string& Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(double,string)
inline std::string Format(const double Value, const std::string& Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(double,char)
inline std::string Format(const double Value, const char* Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(int,string)
inline std::string Format(const int Value, const std::string& Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(int,char)
inline std::string Format(const int Value, const char* Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(int,string)
inline std::string Format(const long Value, const std::string& Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
//! FORMAT$(int,char)
inline std::string Format(const long Value, const char* Format)
	{ PUsing Puse(Format); std::string One = Puse.Output(Value);
	std::string Two = Puse.Finish(); return One + Two; }
}
#endif

