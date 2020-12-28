10	EXTEND

1000 !	Program to calculate azimuth and elevation of the moon.  May-74
!
!	Translated from FORTRAN to BASIC *Mitchell (WB4BWK)
!
!	This program is based upon Lance Collister's work (WA1JXN/WA3GPL)
!	who acknowledged the vital help of Thomas Ake of the Warner-Swasey
!	Observatory.
!
!	WB4BWK < K3IXD < W6PO < WA1JXN/WA3GPL.
!
!	 4-Jun-80	last edit	*M

1020	PRINT
\	PRINT
\	PRINT "Moon  V7.0"
\	PRINT
!\	OPEN "KB:" FOR OUTPUT AS FILE 1% 
\	OPEN "/dev/tty" FOR OUTPUT AS FILE 1% 

1040	DIM MONTH$(13%), MONTH.DAYS%(12%)

1060 DEF FNARCTAN(Y,X)
\	A0=ATN(Y/X)
\	IF X<0.
		THEN A0=A0+PI
		ELSE IF Y<0.
			THEN A0=A0+TWO.PI
1080	FNARCTAN=A0
1100 FNEND

1120	ON ERROR GOTO 1960

1140	TWO.PI=2.*PI
\	RADIANS.2.DEG=180./PI
\	DEG.2.RADIANS=PI/180.
\	EL.MAX=100.
\	F0  = 0.658  * DEG.2.RADIANS
\	F1  = 6.289  * DEG.2.RADIANS
\	F2  = 1.274  * DEG.2.RADIANS
\	F3  = 0.186  * DEG.2.RADIANS
\	F4  = 0.214  * DEG.2.RADIANS
\	F5  = 0.114  * DEG.2.RADIANS
\	F6  = 0.059  * DEG.2.RADIANS
\	F7  = 0.057  * DEG.2.RADIANS
\	F8  = 0.6593 * DEG.2.RADIANS
\	F9  = 6.2303 * DEG.2.RADIANS
\	F10 = 1.2720 * DEG.2.RADIANS
\	F11 = 5.144  * DEG.2.RADIANS
\	F12 = 0.146  * DEG.2.RADIANS
\	O1$=SPACE$(5%)
\	READ CALL$, LATITUDE, LAT.MIN, LONGTITUDE, LONG.MIN, &
		    TIME.INC%, BEGINNING.HOUR%, ENDING.HOUR%
\	READ MONTH$(I%) FOR I%=1% TO 12%
\	READ MONTH.DAYS%(I%) FOR I%=1% TO 12%

1160	INPUT "Use preset values"; P$
\	P$=CVT$$(P$,34%)
\	GOTO 1220 IF ASCII(P$)=ASCII("Y")

1180	PRINT
\	INPUT "Do you want preset moon rise and moon set only"; P$
\	P$=CVT$$(P$,34%)
\	IF ASCII(P$)=ASCII("Y")
		THEN	TIME.INC%,EL.MAX=1.
		\	GOTO 1220

1200	PRINT
\	PRINT "Your call";
\	INPUT LINE CALL$
\	CALL$=CVT$$(CALL$,36%)
\	PRINT
\	PRINT "Your latitude and longitude (degrees, minutes)";
\	INPUT LATITUDE, LAT.MIN, LONGTITUDE, LONG.MIN
\	PRINT
\	INPUT "Time increments"; TIME.INC%
\	PRINT
\	PRINT "Beginning hour and ending hour each day";
\	INPUT BEGINNING.HOUR%, ENDING.HOUR%
\	PRINT
\	INPUT "Limit elevation"; P$
\	P$=CVT$$(P$,34%)
\	GOTO 1220 IF ASCII(P$)<>ASCII("Y")
\	PRINT
\	INPUT "What maximum elevation"; EL.MAX

1220	PRINT
\	PRINT "Beginning month, day, year (21-Jan-80 = JAN,21,80)";
\	INPUT MONTH$, DAY%, YEAR% 
\	MONTH$=CVT$$(MONTH$,34%)
\	MONTH%=0%
\	MONTH%=MONTH%+1% UNTIL CVT$$(MONTH$(MONTH%),32%)=MONTH$ OR MONTH%>12%
\	GOTO 1220 IF MONTH%>12%
\	YEAR%=YEAR%+1900% IF YEAR%<100%
\	PRINT
\	INPUT "Calculations for how many days"; NR.DAYS%
\	PRINT
\	PRINT #1%
\	PRINT #1%
\	PRINT #1%, "Earth-Moon-Earth data for "; CALL$
\	PRINT #1%
\	PRINT #1%,"Location:"; TAB(12%);
\	FORMAT.2$="### degrees ###.#### minutes /        /"
\	PRINT #1%, USING FORMAT.2$, LATITUDE,   LAT.MIN,  "latitude"
\	PRINT #1%, TAB(12%);
\	PRINT #1%, USING FORMAT.2$, LONGTITUDE, LONG.MIN, "longtitude"
\	LATITUDE  =(LATITUDE  + LAT.MIN/60.)*DEG.2.RADIANS
\	LONGTITUDE=(LONGTITUDE+LONG.MIN/60.)*DEG.2.RADIANS

1240	PRINT #1%
\	PRINT #1%
\	M.TIME%=BEGINNINGHOUR%
\	PRINT #1%, "Position of the moon on ";
\	PRINT #1%, NUM1$(DAY%); " "; MONTH$(MONTH%); " "; NUM1$(YEAR%)
\	PRINT #1%
\	PRINT #1%, " GMT          Az          El          GHA           Dec"
\	FORMAT.1$= "/   /      ###.#        ##.#        ###.#         ###.#"
\	PRINT #1%
\	I1=2.
1260	IF MONTH%>=3% THEN 1320
1280	IF INT((YEAR%-1853.)/4.)<11.
		THEN C1=0.
		ELSE C1=-1.
1300	J1=365.*(YEAR%-1853%)+DAY%+30.*(MONTH%+9%)+INT((MONTH%+10%)/2.)
\	J2=INT((YEAR%-1853%)/4.)+1.+C1
\	GOTO 1380

1320	IF INT((YEAR%-1852%)/4.)<11.
		THEN C1=0
		ELSE C1=-1
1340	C2=1.
\	C2=0. UNLESS MONTH%=9% OR MONTH%=11%
1360	J1=365.*(YEAR%-1852%)+DAY%+30.*(MONTH%-3%)+INT((MONTH%-2%)/2.)
\	J2=INT((YEAR%-1852%)/4.)+C1+C2
!	Julian data - 2397547.5 for 0 hr GMT

1380	J=J1+J2
\	T1=J-17472.5
1400	D9% = INT(M.TIME%/100%)*60% + (M.TIME%-INT(M.TIME%/100%)*100%)
\	D6% = INT(ENDING.HOUR%/100%)*60% &
		+ (ENDING.HOUR%-INT(ENDING.HOUR%/100%)*100%)
\	D7%=D9%-D6%
\	D8%=D7%-TIME.INC%
\	GOTO 1420 IF D7%<=0%
\	GOTO 1760 IF D8%>=0%
\	M.TIME%=ENDING.HOUR%
!
!	Calculations of lat and long of the moon:

1420	T=INT(M.TIME%/100.)/24. &
		+ (M.TIME%-INT(M.TIME%/100%)*100%)/1440.
\	T5=T1+T

1440	K0 = .751213  + .036601102  * T5
\	K1 = (K0-FIX(K0)) * TWO.PI
\	K0 = .822513  + .0362916457 * T5
\	K2 = (K0-FIX(K0)) * TWO.PI
\	K0 = .995766  + .00273777852* T5
\	K3 = (K0-FIX(K0)) * TWO.PI
\	K0 = .974271  + .0338631922 * T5
\	K4 = (K0-FIX(K0)) * TWO.PI
\	K0 = .0312525 + .0367481957 * T5
\	K5 = (K0-FIX(K0)) * TWO.PI

1460	K2K2=2.*K2
\	K4K4=2.*K4

1480	L8 = K1 + F0 * SIN(K4K4) + F1 * SIN(K2) - F2 * SIN(K2 - K4K4) &
		- F3 * SIN(K3) + F4 * SIN(K2K2) - F5 * SIN(2. * K5) &
		- F6 * SIN(K2K2 - K4K4) - F7 * SIN(K2 + K3 - K4K4)

1500	K6 = K5 + F8 * SIN(2 * K4) + F9 * SIN(K2) - F10 * SIN(K2 - K4K4)

1520	L7 = F11 * SIN(K6) - F12 * SIN(K5 - K4K4)
!
!	Calculations of right ascention (A=R1) and declination (D1)

1540	D1=COS(L7)*SIN(L8)*.397821+SIN(L7)*.917463
\	D1=ATN(D1/(SQR(1-D1*D1)))
\	A2=COS(L7)*COS(L8)/COS(D1)
\	A1=(COS(L7)*SIN(L8)*.917463-SIN(L7)*.397821)/COS(D1)
\	A=FNARCTAN(A1,A2)

1560	R1=A
\	L1=.065709822*T1
\	L=T*24*1.002738+6.646055+(L1-INT(L1/24)*24)
\	L=(L-INT(L/24)*24)
!
!	Calculations of greenwich hour angle, G, from sideral time.

1580	G=(L/24)*TWO.PI-R1
1600	IF G>=TWO.PI
		THEN G=G-TWO.PI
		ELSE G=G+TWO.PI IF G<0.
!
!	Calculations of local hour angle, H, from GHA.

1620	H=LONGTITUDE-G
!
!	Calculations of elevation, E, of object sin of el

1640	E3=COS(LATITUDE)*COS(H)*COS(D1)+SIN(D1)*SIN(LATITUDE)
\	E2=SQR(1-E3*E3)
\	E=ATN(E3/E2)
\	GOTO 1740 IF E<0.
\	GOTO 1740 IF E>(EL.MAX*DEG.2.RADIANS)
!
!	Calculation of azimuth, A, of object

1660	A2=SIN(D1)/(COS(LATITUDE)*COS(E))
\	A2=A2-(SIN(LATITUDE)/COS(LATITUDE))*(SIN(E)/COS(E))
\	A1=SIN(LATITUDE)*SIN(D1)+COS(LATITUDE)*COS(D1)*COS(H)
\	A1=(SIN(H)*COS(D1))/(SQR(1-A1*A1))
\	A=FNARCTAN(A1,A2)

1680	PRINT #1% IF T-I1>TIME.INC%*2./1440.

1700	A = A * RADIANS.2.DEG
\	E = E * RADIANS.2.DEG
\	G = G * RADIANS.2.DEG
\	D1=D1 * RADIANS.2.DEG

1720	H8%=M.TIME%
\	O1%=H8%/1000%
\	H8%=H8%-O1%*1000%
\	O2%=H8%/100%
\	H8%=H8%-O2%*100%
\	O3%=H8%/10%
\	O4%=H8%-O3%*10%
\	LSET O1$=CHR$(O1%+48%)+CHR$(O2%+48%)+":"+CHR$(O3%+48%)+CHR$(O4%+48%)
\	PRINT #1%, USING FORMAT.1$, O1$, A, E, G, D1
\	I1=T

1740	M.TIME%=M.TIME%+TIME.INC%
\	Z%=(M.TIME%-INT(M.TIME%/100%)*100%)-60%
\	M.TIME%=INT(M.TIME%/100%)*100%+100%+Z% IF Z%>=0%
\	GOTO 1400

1760	DAY%=DAY%+1%
\	DAY.COUNT%=DAY.COUNT%+1%
\	IF DAY.COUNT%>=NR.DAYS% THEN 2020
1780	IF MONTH%=2% AND DAY%=29% &
		AND YEAR%/4%=YEAR%/4. &
		AND (YEAR%/100%<>YEAR%/100. OR YEAR%/400%=YEAR%/400.) THEN 1240
1800	IF DAY%>MONTH.DAYS%(MONTH%)
		THEN	MONTH%=MONTH%+1%
		\	DAY%=1%
1820	IF MONTH%>12%
		THEN	MONTH%=1%
		\	YEAR%=YEAR%+1%
1840	GOTO 1240

1860	DATA "WB4BWK", 34, 19.7833, 82, 22.6333, 30, 0, 2400

1880	DATA Jan, Feb, Mar, Apr, May, Jun 
1900	DATA Jul, Aug, Sep, Oct, Nov, Dec
1920	DATA    31,    28,    31,    30,    31,    30
1940	DATA    31,    31,    30,    31,    30,    31

1960	IF ERR=48% OR ERR=61%
		THEN 1980
		ELSE ON ERROR GOTO 0
1980	IF ERL=1480%
		THEN RESUME 1500
		ELSE IF ERL=1500%
			THEN RESUME 1520
2000	ON ERROR GOTO 0

2020	CLOSE 1%
32767	END
