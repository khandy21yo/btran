10	!
	! KMAZE.BAS
	!
	!	A simple maze generating program
	!
	! Description:
	!
	!	This is a simple maze generating program to show how	
	!	to generate two dimensional mazes in BASIC.
	!
	!	This version randomally looks for a cell to connect
	!	to the maze, and if that cell is not close to a cell
	!	that is already part of the maze, it moves to the next
	!	cell.
	!
	!	It has a habit of always making the first row in
	!	the maze open all the way across, and having many
	!	very short paths. The companion program KMAZE1.BAS
	!	solves these problems by using the added cell as the
	!	next cell to look from.
	!
	! Algorithm:
	!
	!	1. Mark all walls around the cells as being 'up'.
	!	2. Pick one cell at random above the top row,
	!	   and the cell below it as the destination.
	!	3. Mark the wall between the two selected cells
	!	   as 'down'.
	!	4. If all cells are used, then were finished.
	!	5. Pick a cell at random, which has been connected
	!	   to at least one cell, and has an open cell in one
	!	   of the four adjacent cells.
	!	6. Goto 3
	!
	! Enviornment:
	!
	!	VAX/BASIC on VAX/VMS
	!
	! Copyright:
	!
	!	I place this code in the public domain, with the
	!	provision that my name remain attached to the code when it
	!	is used in any additional program.
	!
	!	This code is given for demonstration purposes only, and
	!	is simplified from what would be required for a commercial
	!	program.
	!
	!	No warranties, express or implied, is granted. This code
	!	is supplied for educational purposes only.  It is the
	!	users responsibility to make sure that this code works
	!	appropriately for any given task.
	!
	! History:
	!
	!	08/15/1997 - Kevin Handy
	!		Original version
	!

	!
	! WALLS%() Contains a reference to which walls have been taken
	! down from the cell. If the cell has a zero value, then the
	! cell isn't included in a path yet (no broken walls).
	!	1 - up		2 - down
	!	4 - left	8 - right
	!
	OPTION SIZE = INTEGER WORD
20	DIM WALLS%(100%, 100%)		! Array size limits maze size
					! Should really be defined

25 ! ON ERROR GOTO 200

50	INPUT "MAZE SIZE (X,Y)"; XMAX%, YMAX%

100	!
	! Initialize the maze to having all walls up
	!
	WALLS%(X%, Y%) = 0% FOR X% = 1% TO XMAX% FOR Y% = 1% TO YMAX%
	UNUSED% = XMAX% * YMAX%

110	!
	! Randomally pick a starting cell on the first row,
	! to get things started and tell it we're going up
	!
	X% = 1%
	Y% = INT(RND * YMAX%) + 1%

	AVAIL% = 1%
	AVAILPATH%(AVAIL%) = 1%
	PATH% = 1%

140	!
	! Mark the path taken
	!
	SELECT AVAILPATH%(PATH%)
	!
	! Going up
	!
	CASE 1
		WALLS%(X%, Y%) = WALLS%(X%, Y%) OR 1%
		WALLS%(X% - 1%, Y%) = WALLS%(X% - 1%, Y%) OR 2%

	!
	! Going down
	!
	CASE 2
		WALLS%(X%, Y%) = WALLS%(X%, Y%) OR 2%
		WALLS%(X% + 1%, Y%) = WALLS%(X% + 1%, Y%) OR 1%

	!
	! Going left
	!
	CASE 4
		WALLS%(X%, Y%) = WALLS%(X%, Y%) OR 4%
		WALLS%(X%, Y% - 1%) = WALLS%(X%, Y% - 1%) OR 8%

	!
	! Going right
	!
	CASE 8
		WALLS%(X%, Y%) = WALLS%(X%, Y%) OR 8%
		WALLS%(X%, Y% + 1%) = WALLS%(X%, Y% + 1%) OR 4%

	END SELECT

	UNUSED% = UNUSED% - 1%

150	!
	! Now, we need to look for another available path
	!
	GOTO 200 IF UNUSED% = 0%

	!
	! Randomally pick a starting cell to get things started
	!
	X% = INT(RND * XMAX%) + 1%
	Y% = INT(RND * YMAX%) + 1%

155	!
	! Move over one square
	!
	X% = X% + 1%
	IF X% > XMAX%
	THEN
		X% = 1%
		Y% = Y% + 1%
		IF Y% > YMAX%
		THEN
			Y% = 1%
		END IF
	END IF

	!
	! Move over again if this is not connected
	!
	GOTO 155 IF WALLS%(X%, Y%) = 0%

160	!
	! Having an (X,Y) location with at least one open path,
	! Create a list of available directions
	!
	AVAIL% = 0%
	IF (X% > 1%) AND WALLS%(X% - 1%, Y%) = 0%
	THEN
		AVAIL% = AVAIL% + 1%
		AVAILPATH%(AVAIL%) = 1%
	END IF

	IF (X% < XMAX%) AND WALLS%(X% + 1%, Y%) = 0%
	THEN
		AVAIL% = AVAIL% + 1%
		AVAILPATH%(AVAIL%) = 2%
	END IF

	IF (Y% > 1%) AND WALLS%(X%, Y% - 1%) = 0%
	THEN
		AVAIL% = AVAIL% + 1%
		AVAILPATH%(AVAIL%) = 4%
	END IF

	IF (Y% < YMAX%) AND WALLS%(X%, Y% + 1%) = 0%
	THEN
		AVAIL% = AVAIL% + 1%
		AVAILPATH%(AVAIL%) = 8%
	END IF

170	!
	! If we haven't got somewhere to go, look at the next square
	!
	GOTO 155 IF AVAIL% = 0%

	!
	! Now, choose one of the available directions for the
	! next site to connect to
	!
	PATH% = RND * AVAIL% + 1%

	GOTO 140

200	!
	! Now, print out the maze
	!
	FOR X% = 1% TO XMAX%

		!
		! Box Tops
		!
		FOR Y% = 1% TO YMAX%

			IF (WALLS%(X%, Y%) AND 1%) = 0%
			THEN
				PRINT ":--";
			ELSE
				PRINT ":  ";
			END IF

		NEXT Y%

		PRINT ":"

		!
		! Left sides
		!
		FOR Y% = 1% TO YMAX%

			IF (WALLS%(X%, Y%) AND 4%) = 0%
			THEN
				PRINT "I  ";
			ELSE
				PRINT "   ";
			END IF

		NEXT Y%

		PRINT "I"

	NEXT X%

	X% = RND * YMAX% + 1%
	!
	! Box Tops
	!
	FOR Y% = 1% TO YMAX%

	IF Y% <> X%
		THEN
			PRINT ":--";
		ELSE
			PRINT ":  ";
		END IF

	NEXT Y%

	PRINT ":"

	END
