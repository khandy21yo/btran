/**
 * \file vartype.h
 * \version 01.02.00
 * \brief Header file for variable type table
 *
 *	This is the header file for the variable table
 *
 */

//
// Variable.h - Some stuff just in case if isn't defined anywhere else
//
#ifndef _VARTYPE_H_
#define _VARTYPE_H_

/**
 * \brief Variable Type Definitions
 *
 *	These are numbered according to the 'weight' of the
 *	variable type. (With NONE being the lowest of the low)
 */
enum VARTYPE
{
	VARTYPE_NONE =		0,	/**< \brief Undefined */
	VARTYPE_VOID =		1,	/**< \brief Void (no) return */
	VARTYPE_STRUCT =	2,	/**< \brief Structure */
	VARTYPE_RFA =		3,	/**< \brief RFA */
	VARTYPE_LABEL =		4,	/**< \brief Label */
	VARTYPE_FIXSTR =	5,	/**< \brief Fixed length string */
	VARTYPE_DYNSTR =	6,	/**< \brief Dynamic String */
	VARTYPE_BYTE =		7,	/**< \brief Byte (char) */
	VARTYPE_WORD =		8,	/**< \brief 16 bit integer */
	VARTYPE_INTEGER =	9,	/**< \brief 16 bit integer */
	VARTYPE_LONG =		10,	/**< \brief 32 bit integer */
	VARTYPE_SINGLE =	11,	/**< \brief Single precision float */
	VARTYPE_REAL =		12,	/**< \brief float type */
	VARTYPE_DOUBLE =	13,	/**< \brief double type */
	VARTYPE_HFLOAT =	14,	/**< \brief HFloat floating point */
	VARTYPE_GFLOAT =	15,	/**< \brief GFloat floating point */
	VARTYPE_DECIMAL =	16	/**< \brief Label */
};

/**
 * \brief Variable Class Definitions
 */
enum VARCLASS
{
	VARCLASS_NONE =		0,	/**< \brief Regular variable */
	VARCLASS_ARRAY =	1,	/**< \brief An Array */
	VARCLASS_FUNC =		2,	/**< \brief Function */
	VARCLASS_CONST =	3,	/**< \brief Constant */
	VARCLASS_LABEL =	4,	/**< \brief Label */
	VARCLASS_MAP =		5	/**< \brief Map variable */
};

/**
 * Not used
 */
enum VARMECH
{
	VARMECH_NONE =		0,	/**< \brief Whatever goes */
	VARMECH_REF =		1,	/**< \brief By Reference */
	VARMECH_DESC =		2,	/**< \brief By Descriptor */
	VARMECH_VALUE =		3,	/**< \brief By Value */
	VARMECH_XREF =		4	/**< \brief Use C++ '&' reference mechanism (??) */
};

#endif
