/**\file datalist.h
 * \brief Handle 'DATA' statements.
 */

//
// History
//
#ifndef _datalist_h_
#define _datalist_h_

namespace basic
{
/**
 * \brief class to handle "DATA" statements
 *
 * Class to handle "DATA" statements.
 */
class DataListClass
{
private:
	int count;		/**< \brief Current element */
	const char** Values;	/**< \brief Array containing values */

public:
	//! Empty constructor
	DataListClass() { count = 0; }
	//! Constructor with data list
	DataListClass(const char* List[])
		{ Values = List; count = 0; }

	//! Read off a double value
	void Read(double& result) { result = atof(Values[count++]); }
	//! Read off an integer value
	void Read(int& result) { result = atoi(Values[count++]); }
	//! Read off a long value
	void Read(long& result) { result = atol(Values[count++]); }
	//! Read off a string value
	void Read(std::string& result) { result = Values[count++]; }
	//! Reset to the beginning of the list
	void Reset() { count = 0; }
};
}
#endif

