btran

This is a VAX Basic (and Basic+2) to C++ Translator.

Is is NOT a compiler. It is meant to be used as a tool to convert Basic Code
to C++, which will thereafter be maintained as C++ code.

Thiis project starteid many years ago to aid me in converting code from VMS and
RSTS/E into something that would run under Linux. Several programs from the
"101 Basic Computer Games" translate with few problems.

Requires Flex and Bison programs.

It makes use of several BOOST library functions (libboost).
These functions are available to be installed in most Linux distributions, but
not usually installed by default.

There are still several areas where code is not readily translated, due to the
differing underlying designes of Basic and C. Instead of creating wrappers
around this code, it is left as an exercise to the user to fix these.
Doing otherwise creates "not C++" code.

  Variable names
	- Basic can use appended characters to define the variable type, such as
	  '%', '$', etc. Also subscripted variables can overlap non-subscripted
	  variables. To handle the multiple name collisions, a postfix is
	  appended as necessary, of the form "_V<x>", where the <x> is a unique
	  number.
	- Some words are keywords in one context, and cab be used as variable
	  names in another. For example, SET PROMPT is a command, but PROMPT 
	  can also be used as a variable in a RECORD. These will currently 
	  cause an error in btran. Rename these variables in the source to
	  get btran to parse them correctly.
  Value widths
	- Much code exists that assumes the number of bits in varouus value
	  types, such as in COMMON and MAP statements, SWAP%, etc.
	  Also the original code expects some strings to be fixes length
	  instead of the variable C++ std::string lengths. Handling this is
	  extremely complex, and is left to post-editing to deal with it.
- File I/O.
	- Basic uses an integer channel number, with a limited number of
	  channels available. C++ uses a data structure to refer to the open
	  files. This can cause several translation problems. You cannot add
	  or subtract channel numbers, for example.
	- FIELD statements a;so have no equivelent C++ translation.
	- RMS operations have no equivelent in C++.
	- MAP et. al. translation is buggy.
- Error handling
	- ON ERROR GOTO, RESUME, etc. are not easily translated. It is somewhat
	  lige a subroutine, but not closely enoug to translate well.
	- ERR C++ error references are not enumerated with the same values.
	- ERL line numbers are not kept current in C++.
	- C++ functions do not usually throw errors, they return a status.
- Strings. std::string is similiar, but there are still many difficulties.
	- "Foe example, A$=RIGHT(B$,LEN(B$))" translated to 
	  "a=b.substr(b.length*(-1))" which causes a segfault when "B$" is
	  empty. Editing this to be "a=b.rbegin()" works better, but it
	  returnes a "char" instead of a "std::string".
	- RIGHT off the end of a string is Ok in bsic, but will crash C++.
	- Zero length strings can cause odd problems. text.rbegin() for
	  example does not return a 0.
- GOSUB. Actually works, but has the same limitations in C++ as goto.
	- No jumping over initializers. No calling from inside user defined
	  functions.
- DEF and DEF* functions.
	- GOSUB insode DEF functions isn't likely to work.
	- DEF and DEF* are treated identically.
- DATA sstatements
	- Bare Reserved words in basic cause parsing problems. Putting quotes
	  around the text works to fix this, and is compatable with basic.
	  Data statements are not easily handled in Flex/Bison grammers.
- %xxx preprocessor)
	- Code exists but only thinly tested. %IF can cause problems if blocks
	  start or end inside of it.
	- %var is not allowed in normal VaxBasic statements, but btran accepts
	  it.
  TAB, POS, CCOS
	- Unix doesn't attempt to track the cursor position, to these functions
	  require a greate deal of complexity to implemet correctly. It requires
	  replacing the IO library with something else that tracks the current
	  cursoe position, and escape sequences would quickly overwhelm it.
	  Best to rewrite code so it doesn't need these.
  COMMON
	- COMMON is freqiently used to overlap memory so that a common data
	  area can be seen in different ways, i,e, as a STRING in one COMMON,
	  and an array of integers in another.  Under C++ this would be handled
	  as a 'union'. Due to the complesity of figuring out what is going on,
	  they are just being converted to 'class' for now, and you are left
	  to clean up the code.
  MAP
	- MAP is being handled exactly the same as COMMON.
	  MAP's are usually used for RMS I/O, which doesn't exist in any
	  standard way in C++. The code needs to be cleaned up after
	  processing, So it is being converted into a simple 'class' for now.


Several packages are required to use this code.
The debian command to install thse ackages include

	apt-get install gcc g++ automake autoconf libtool
	apt-get install libboost-dev bison flex
