btran

This is a VAX Basic (and Basic+2) to C++ Translator.

Is is NOT a compiler. It is meant to be used as a tool to convert Basic Code
to C++, which will thereafter be maintained as C++ code.

Thiis project starteid many years ago to aid me in converting code from VMS and
RSTS/E into something that would run under Linux. Several programs from the
"101 Basic Computer Games" translate with few problems.

There are still several areas where code is not readily translated, due to the
differing underlying designes of Basic and C. Instead of creating wrappers
around this code, it is left as an exercise to the user to fix these.
Doing otherwise creates "not C++" code.

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
	- No jumping over initializers.
- DEF functions.
	- Don't have access to local variables.
- DATA sstatements
	- Bare Reserved words in basic cause parsing problems. Putting quotes
	  around the text works to fix this, and is compatable with basic.
- %xxx preprocessor)
	- Code exists but only thinly tested. %IF can cause problems if blocks
	  end inside of it.
	- %var is not allowed in VaxBasic, but btran accepts it.

