/**\file basicfun.h
 * \brief Generic basic functions
 *
 *	Header file for special routines written to handle basic
 *	functions that do not have a direct C++ equivelent.
 */
#ifndef _BASICFUN_H_
#define _BASICFUN_H_

#include <stdlib.h>
#include <math.h>
#include <setjmp.h>
#include <time.h>

#include "bstring.h"

namespace basic
{
//
// Define some variable types
//
typedef int Int;		/**< \brief Whatever */
typedef short int Int16;	/**< \brief 16 bit integer */
typedef long int Int32;		/**< \brief 32 bit integer */

typedef double Dfloat;		/**< \brief double float type */
typedef double Gfloat;		/**< \brief G float type */
typedef double Hfloat;		/**< \brief H float type */
typedef float Sfloat;		/**< \brief single float type */

//
// Hard coded constants
//
static const char BEL = 7;	/**< \brief Bell */
static const char BS = 8;	/**< \brief Backspace */
static const char HT = 9;	/**< \brief Horizontal tab */
static const char LF = 10;	/**< \brief Line feed */
static const char VT = 11;	/**< \brief Vertical tab */
static const char FF = 12;	/**< \brief Form feed */
static const char CR = 13;	/**< \brief Carriage return */
static const char SO = 14;	/**< \brief Shift out */
static const char SI = 15;	/**< \brief Shift in */
static const char ESC = 27;	/**< \brief Escape */
static const char SP = 32;	/**< \brief Space */
static const char DEL = 127;	/**< \brief Delete */
#ifndef PI
static const double PI = 3.1415926535; /**< \brief Pi (Someone else already defined this) */
#endif

//
// Error routines
//
extern long ErrNum;		/**< \brief Error Number */
extern long ErrLine;		/**< \brief Line Number */
extern long Status;		/**< \brief Status flag */

//
// Horrible gosub/return code, but it does work (Icky icky icky)
//

/** 
 * \brief Initiaize gosub/return stack
 */
#define BStack(x) jmp_buf BGoStack[x]; int BGoCount = 0;
/**
 * \brief Execute a GOSUB
 */
#define BGosub(x) if (setjmp(BGoStack[BGoCount++]) == 0) { goto x; }
/**
 * \brief execute a RETURN
 */
#define BReturn longjmp(BGoStack[--BGoCount], 1);

//
// Now for some nasty on-error-goto code.
//
//	It's not completely compatible with the "On error goto"
//	code, but it's better than nothing.
//

/**
 * \brief Die on an eror
 */
#define OnErrorDie() cerr << "%Error " << ErrNum << " at " << ErrLine << endl; exit(EXIT_FAILURE);
/**
 * \brief ON ERROR GOTO 0
 */
#define OnErrorZero if (setjmp(ErrorStack) == 0) { OnErrorDie(); }
/**
 * \brief Create error return point
 */
#define OnErrorStack jmp_buf ErrorStack;
/**
 * \brief Handle ON EROR GOTO statement
 */
#define OnErrorGoto(x) if (setjmp(ErrorStack) != 0) { goto x; }
/**
 * \brief RESUME
 */
#define OnResumeLine(x) goto x;
/**
 * \brief Generate error
 */
#define OnErrorHit(x) ErrNum = x; longjmp(ErrorStack);

/**
 * \brief Rnd (random number between 0 and 1)
 *
 *	This is a real nasty hack, and may not produce as
 *	wide a range of random numbers as basic desires.
 *	
 * \bug fmax parameter is ignored because of strange
 *	usage in many basic programs which assume the value doesn't
 *	matter
 */
inline double floatrand(
	double fmax = 1		/**< Maximum value to return */
)
{
	return ((double) rand()) / ((double) RAND_MAX); // * fmax;
}

//
// Some simple math formulas
//

/**
 * \brief Integer Square root
 */
static inline int sqrt(
	int x		/**< Value to operate on */
	)
{ return (int)pow((double)x, 0.5); }
/**
 * \brief Long square root
 */
static inline long sqrt(
	long x		/**< Value to operate on */
	)
{ return (long)pow((double)x, 0.5); }
/**
 * \brief Integer sign
 */
static inline int sgn(
	int x		/**< Value to operate on */
	)
{ if (x > 0) return 1; else if (x < 0) return -1; else return 0; }
/**
 * \brief Long sign
 */
static inline int sgn(
	long x		/**< Value to operate on */
	) 
{ if (x > 0) return 1; else if (x < 0) return -1; else return 0; }
/**
 * \brief Float sign
 */
static inline int sgn(
	double x	/**< Value to operate on */
	) 
{ if (x > 0) return 1; else if (x < 0) return -1; else return 0; }
/**
 * \brief Time
 * \bug Assumes that time(0) is being called
 */
static inline long Time(
	int code	/**< Time code */
	) 
{ return time(NULL) % 86400; }
/**
 * \brief Swap bytes
 */
static inline int Swap(
	int x		/**< Value to operate on */
	)
{ return ((x & 0xff) << 8) | ((x >> 8) & 0xff); }

//
// Matrix functions
//

/**
 * \brief Integer Matrix Zero (1 dimension)
 */
inline void MatZer_1(
	int *x,		/**< Pointer to matrix */
	int s1, 	/**< Size 1 */
	int s2		/**< Size 2 */
	)
	{ int sz = s1 / s2; while (sz--) *x++ = 0; }
/**
 * \brief Float Matrix Zero (1 dimension)
 */
inline void MatZer_1(
	double *x,	/**< Pointer to matrix */
	int s1,		/**< Size 1 */
	int s2		/**< Size 2 */
	)
	{ int sz = s1 / s2; while (sz--) *x++ = 0.0; }
/**
 * \brief Integer Matrix Zero (2 dimension)
 */
inline void MatZer_2(
	int *x,		/**< Pointer to matrix */
	int s1,		/**< Size 1 */
	int s2,		/**< Size 2 */
	int s3		/**< Size 3 */
	)
	{ int sz = s1 / s3; while (sz--) *x++ = 0; }
/**
 * \brief Float Matrix Zero (2 dimension)
 */
inline void MatZer_2(
	double *x,	/**< Pointer to matrix */
	int s1,		/**< Size 1 */
	int s2,		/**< Size 2 */
	int s3		/**< Size 2 */
	)
	{ int sz = s1 / s3; while (sz--) *x++ = 0.0; }
/**
 * \brief Integer Matrix Zero (n dimension)
 */
inline void MatZer_n(
	int *x,		/**< Pointer to matrix */
	int s1,		/**< Size 1 */
	int s2		/**< Size 2 */
	)
	{ memset(x, s2, s1 / s2); }
/**
 * \brief Float Matrix Zero (n dimension)
 */
inline void MatZer_n(
	double *x,	/**< Pointer to matrix */
	int s1,		/**< Size 1 */
	int s2		/**< Size 2 */
	)
	{ memset(x, s2, s1 / s2); }

/**
 * \brief Error trapping stuff
 */
class BasicError
{
public:
	int ErrorNo;		/**< \brief Basc error number */
public:
	/**
	 * \brief Empty constructor
	 */
	BasicError() { ErrorNo = 0; }
	/**
	 * \brief Constructor
	 */
	BasicError(int x) { ErrorNo = x; ErrNum = x; }
};

std::string sys(const std::string& Source);
std::string Qdate(int x);
std::string Qtime(int x);
}

#endif
