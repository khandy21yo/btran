btran

This is a VAX Basic (and Basic+2) to C++ Translator.

Is is NOT a compiler. It is meant to be used as a tool to help convert
Basic Code into C++, which will thereafter be maintained as C++ code.

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
Doing otherwise creates very ugly "not C++" code.

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
	  files. This causes several translation problems. You cannot add
	  or subtract channel numbers, for example.
	- FIELD statements a;so have no equivelent C++ translation.
	- RMS operations have no equivelent in C++.
	- Data errors, such as entering letters when numbers are wanted, will
	  cause odd behaviour. It can get the stream stuck. Right nowm it should
	  act as if '0' was entered. Not easy to fix without introducing massive
	  amounts of code.
- Error handling
	- ON ERROR GOTO, RESUME, etc. are not easily translated. It is somewhat
	  lige a subroutine, but not closely enoug to translate well.
	- ERR C++ error references are not enumerated with the same values.
	- ERL line numbers are not kept current in C++.
	- C++ functions do not usually throw errors, they return a status.
	- To handle errors like BASIC would require generating huge amounts
	  of validating code around any statement that coule possibly cause
	  an error, when most of it is probably going to be unnecessary in
	  most programs. 
	- 'catch/try' does not handle the same errors as 'ON ERROR' does, so
	  it would help very little.
  Strings
-	- std::string is similiar but still very different from BASIC strings.
	  BASIC strings are actually nultiple entities.
	  There are regular variable length strings, fixed length strings,
	  fixed position strings, etc. Strings in RECORDS/MAPS/COMMON should
	  be handled as 'char[]', and strings in FIELD statements should be
	  'char*'s pointing into file buffers, then using assorted 'str*'
	  functions to do the work on these type of strings.
	  All this stuff gets extremely complicated and would generate
	  extremely ugly C++ code, therefor
	- all strings are translated as regular 'std::string's, and the
	  comples stuff is left for the user to fix since it is likely
	  doing something that needs to be handled different;y in C++
	  anyway.. 
- GOSUB. Actually works, but has the same limitations in C++ as goto.
	- No jumping over initializers. No calling from inside user defined
	  functions.
- DEF and DEF* functions.
	- GOSUB insode DEF functions isn't likely to work.
	- DEF and DEF* are treated identically.
	- These functions are translated using C++ 'lambda's, instead of the
	  g++ extensions, but requires cxx-11 capabilities.
- DATA sstatements
	- Bare Reserved words in basic cause parsing problems. Putting quotes
	  around the text works to fix this, and is compatable with basic.
	  Data statements are not easily handled in Flex/Bison grammers.
- %xxx preprocessor)
	- Code exists but only thinly tested. %IF can cause problems if blocks
	  start or end inside of it (IF, FOR, etc.).
	- They are parsed as if they were regular BASIC code, but generate
	  C++ marco's instead of regular C++ code.
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
 File Names
	In BASIC, file names were case insensitive. Under most modern
	systems, theyy are case sensitive, and often transfered in lower
	case. P:Old programers picked whatever case they wanted, and were
	not always sconsistant. This may require fixing the case used in
	the source code.

Several packages are required to use this code.
The debian command to install thse ackages include

	apt-get install gcc g++ automake autoconf libtool
	apt-get install libboost-dev bison flex
