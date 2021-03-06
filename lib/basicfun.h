/**\file basicfun.h
 * \brief Generic basic functions
 *
 *	Header file for special routines written to handle basic
 *	functions that do not have a direct C++ equivelent.
 */
#ifndef _BASICFUN_H_
#define _BASICFUN_H_

#include <limits>
#include <stdlib.h>
#include <math.h>
//#include <setjmp.h>
#include <time.h>
#include <vector>

#include "bstring.h"
#include "pusing.h"

#ifndef PI
static const double PI = 3.1415926535; /**< \brief Pi (Someone else already defined this) */
#endif

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

//
// Error routines
//
extern long Status;		/* Status flag */

//
// Array of vector code stolen from
// https://mklimenko.github.io/english/2019/08/17/multidimensional-vector-allocation/
//
namespace detail
{
	template<typename T, std::size_t sz>
		struct vector_type
	{
		using type = std::vector<typename vector_type<T, sz - 1>::type>;
	};
	template<typename T>
	struct vector_type<T, 1>
	{
		using type = T;
	};

	template<typename T, std::size_t sz>
	using vector_type_t = typename vector_type<T, sz>::type;
}

template <typename T>
	struct VectorGenerator
{
	static auto Generate(std::size_t last_arg)
	{ 
		return std::vector<T>(last_arg);
	}

	template <typename ...Args>
	static auto Generate(std::size_t first_arg, Args... args)
	{
		using vector = std::vector<typename detail::vector_type_t<T, 1 + sizeof...(args)>>;

		return vector(first_arg, VectorGenerator<T>::Generate(args...));
	}
};

/**
 * \brief Error trapping stuff
 */
class BasicError
{
public:
	int err;		/**< \brief Basc error number */
	int erl;		/**< Line number for error */
	std::string ern;	/**< Function name of error */

public:
	/**
	 * \brief Empty constructor
	 */
	BasicError()
	{
		err = 0;
		erl = 0;
	}
	/**
	 * \brief Constructor
	 */
	BasicError(int newerr, int newerl = 0, std::string newern = "")
	{
		err = newerr;
		erl = newerl;
		ern = newern;
	}
	/**
	 * Copy constructor
	 */
	BasicError(const BasicError &be)
	{
		err = be.err;
		erl = be.erl;
		ern = be.ern;
	}

};
extern basic::BasicError Be;

#define __C(a, b) a ## b
#define __U(p, q) __C(p, q)
/** 
 * \brief Initiaize gosub/return stack
 */
#define BStack(x) void *BGoStack[x]; int BGoCount = 0;
/**
 * \brief Execute a GOSUB
 *
 * Do to the generation of 'return labels' in this version,
 * you must place each BGosub on seperate lines.
 * i.e. You cannot use 'BGosub(line1); BGosub(line2);'
 *
 * Be careful not to exceed the stack size created in the BStack reference.
 * there isn't any testing for overflow.
 */
#define BGosub(x) { BGoStack[BGoCount++] = && __U(Brp, __LINE__); goto x; __U(Brp, __LINE__):; }
/**
 * \brief execute a RETURN
 *
 * Be careful not to return more than you gosub;ed.
 * There is no test for stack underflow.
 */
#define BReturn goto *BGoStack[--BGoCount];

//
// Now for some nasty on-error-goto code.
//
//	It's not completely compatible with the "On error goto"
//	code, but it's better than nothing.
//

/**
 * \brief Die on an eror
 */
static inline void OnErrorDie()
{
	std::cerr << "%Error " << Be.err << " at " << Be.erl << std::endl;
	exit(EXIT_FAILURE);
}
/**
 * \brief ON ERROR GOTO 0
 */
#define OnErrorZero ErrorStack = 0;
/**
 * \brief Create error return point
 */
#define OnErrorStack void *ErrorStack;
/**
 * \brief Handle ON EROR GOTO statement
 */
#define OnErrorGoto(x) { ErrorStack = &&x; }
/**
 * \brief RESUME
 */
#define OnResumeLine(x) goto x;
/**
 * \brief Generate error
 */
#define OnErrorHit(x,y) if (ErrorStack) \
	{ Be.err = x; Be.erl = y; goto *ErrorStack; } else \
	throw basic::BasicError(x, y); ;
/**
 * \brief return error line
 */
static inline int erl()
{
	return Be.erl;
}
/**
 * \brief return error number
 */
static inline int err()
{
	return Be.err;
}

/**
 * \brief Rnd (random number between 0 and 1)
 *
 *	This is a real nasty hack, and may not produce as
 *	wide a range of random numbers as basic desires.
 *	This version has a small chance of returning 1.
 *	
 * \bug fmax parameter is ignored because of strange
 *	usage in many basic programs which assume the value doesn't
 *	matter
 */
inline double floatrand(
	double fmax = 1.0	/**< Maximum value to return  (ignored)*/
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

std::string sys(const std::string& Source);
std::string Qdate(int x);
std::string Qtime(int x);
}

#endif
