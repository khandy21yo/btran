1	!
	! This is a test program to show a few of the things
	! that are actually working in the btran translator.
	!
	! It is not meant as a useful program, it just gives
	! a sample of some of the features that seem to work
	! completely, and to show that some fairly complex
	! programs can be compiled using btran.
	!
	option size = integer long

100	PRINT "Test Program To Show " + &
		"Things That Really Work " + &
		"With My VaxBasic to C++ Converter"
	PRINT

	INPUT "Enter a Number"; A%
	PRINT "You Entered "; A%
	PRINT

120	!
	! For loop on statement
	!
	PRINT "Implied FOR loop"
	PRINT I%, FOR I% = 1% TO 100%
	PRINT
	PRINT

200	!
	! Let's calculate some primes (slowly)
	! I'm not using any fancy calculations, just a brute force
	! check to execerise the math sections.
	!
	PRINT "Prime Numbers"
	CRFLAG% = 0%
	! Loops don't work all the way to 32767% in 16bit mode,
	! because it increments to -32786 before the test
	FOR I% = 5% TO A% STEP 2%

		FOR J% = 3% TO I% / 2% STEP 2%

			IF ((I% / J%) * J%) = I%
			THEN
				GOTO 220
			END IF
		NEXT J%

		PRINT I%,

		CRFLAG% = CRFLAG% + 1%
		IF CRFLAG% = 10%
		THEN
			PRINT
			CRFLAG% = 0%
		END IF

220	NEXT I%

290	PRINT
	PRINT

300	!
	! Print a triangle (Play with some simple string functions)
	!
	PRINT "Triangle"
	FOR I% = 1% TO 10%

		FOR J% = 1% to 11% - I%
			PRINT " ";
		NEXT J%

		FOR J% = 1% TO I% * 2% - 1%
			PRINT "*";
		NEXT J%

		PRINT
	NEXT I%

	!
	! And again using different functions to test string addition
	! (Warning: Negitive for loops need to see the unary minus
	! for btran to do the right thing)
	!
	FOR I% = 10% TO 1% STEP -1%

		TEXT$ = ""
		TEXT$ = TEXT$ + " " FOR J% = 1% TO 11% - I%
		TEXT$ = TEXT$ + "*" FOR J% = 1% TO I% * 2% - 1%
		PRINT TEXT$

	NEXT I%

	!
	! And one more time for some string function tests
	! and an implied for loop
	!
	PRINT SPACE$(11% - I%); STRING$(I% * 2% - 1%, ASCII("*")) &
		FOR I% = 1% to 10%

	!
	! Let's also try it with data statements
	!
 	DATA "DATA STATEMENT"
	READ T$
	PRINT T$
	DATA 10,1, 9,3, 8,5, 7,7, 6,9, 5,11, 4,13, 3,15, &
		2,17, 1,19, -1, -1
	READ SP%, ST%
	WHILE (SP% <> -1%)
		PRINT SPACE$(SP%); STRING$(ST%, ASCII("*"))
		READ SP%, ST%
	NEXT

	PRINT

	!
	! Let's do a graph
	!
	PRINT "Sine Wave"
	FOR I = -PI TO PI STEP 0.2

		X% = SIN(I) * 25 + 27

		X$ = SPACE$(26%) + "|" + SPACE$(26%)
		X$ = LEFT(X$, X% - 1%) + "*" + RIGHT(X$, X% + 1%)

		X% = COS(I) * 25 + 27
		X$ = LEFT(X$, X% - 1%) + "+" + RIGHT(X$, X% + 1%)

		X1$ = TRM$(X$) + ""
		PRINT X1$; " "; NUM1$(LEN(X1$))

	NEXT I

1000	!
	! test gosub routines
	!
	X$ = "This is A"
	X1$ = "Test Of Gosub"
	GOSUB 1100

	X$ = "Another"
	GOSUB 1100

	X1$ = "Silly Test"
	GOSUB 1100

	GOTO 2000

1100	PRINT X$; " "; X1$
	RETURN

2000	!
	! Test simple DEF functions
	!
	C = 1
	DEF FNTEST1(A, B) = A + B + C

	PRINT
	PRINT "Testing DEF Functions"
	PRINT "FNTEST1(1,2) = "; FNTEST1(1.0,2.0)
	PRINT "FNTEST1(5,7) = "; FNTEST1(5.0,7.0)
	PRINT "FNTEST1(7.5,6.2) = "; FNTEST1(7.5,6.2)

4100	!
	! Test INSTR
	!
	A$ = "1234567890"
	PRINT "INSTR TEST: "; I%, INSTR(1%, A$, NUM1$(I%)) FOR I% = 0% TO 9%
32767	END
