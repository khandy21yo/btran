/**\file virtual.h
 * \brief Header for virtual array handling code.
 *
 *	This header defines code for handling virtual arrays for the
 *	BASIC to C++ translator.
 *
 *	When this works, the plan is to load all of the data into a
 *	vector on the first access, and write out the data when the
 *	array is destroyed. It will probably not work exactly like
 *	a "real" virtual array, but will hopefully be close enough
 *	to do a decent job.
 */

//
// Bugs
//
//	Currently doesn't actually interface with the data file. It
//	always acts as if the file is blank, but lets the idea be tested.
//
// History
//
//	04/10/2000 - Kevin Handy
//		Original version. Doesn't do anything besides act as a
//		placeholder for the functions that will need to be
//		developed.
//

#ifndef _basic_virtual_h
#define _basic_virtual_h

#include <vector>

template <class T>
class VirtualArray
{
private:
	int Channel;		// Remember what IO channel attached to
	int ArraySize;		// Number of elements in the array
	int ElementSize;	// Size of an individual element
	std::vector<T> Storage;	// Vector to hold the data

public:
	VirtualArray(int channel, int maxelement, int size = sizeof(T)) :
		Storage(maxelement + 1)
		{ Channel = channel; ArraySize = maxelement;
		ElementSize = size; }
	~VirtualArray() {}

	T &operator[] (const unsigned int index) { return Storage[index]; }
};

#endif
