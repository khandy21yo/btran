100 ! ON ERROR GOTO 310
101 DIM S%(10%,10%)
102 DIM D%(10%,10%)
300 PRINT "ORDERS:  ";
400 M$(1%)="WARP ENGINES":M$(2%)="S.R. SENSORS":M$(3%)="L.R. SENSORS":
	M$(4%)="PHASER CTRL":M$(5%)="PHOTON TUBES":M$(6%)="DAMAGE CTRL":
	T1$=CHR$(28%):T2$=CHR$(31%)
500 S$=".*KBE"
600 RANDOM:K1%=RND*20%+11%:S1%=RND*149%+151%:B1%=RND*9%+1%:D1%=INT((RND*
	1999%+2001%)/10%)*10%:D3%=40%:D2%=40%+D1%
700 FOR I%=1% TO S1%
800 A1%=RND*8+1%:A2%=RND*8+1:IF S%(A1%,A2%)>8% THEN 800 ELSE S%(A1%,A2%)
	=S%(A1%,A2%)+1%:NEXT I%:FOR I%=1% TO K1%:A1%=RND*8+1:A2%=RND*8+1
	:S%(A1%,A2%)=S%(A1%,A2%)+100%:NEXT I%
900 FOR I%=1% TO B1%:A1%=RND*8%+1%:A2%=RND*8%+1%:S%(A1%,A2%)=
	S%(A1%,A2%)+10%:NEXT I%:E1%=RND*8%+1%:E2%=RND*8%+1%:E7%=
	RND*8%+1%:E8%=RND*8%+1%:P=3000%:C1%=0%:T1%=10%
1000 PRINT "STARDATE -";D1%:PRINT:Q1%=3%:OPEN "morgul.bas" FOR INPUT AS FILE 1%:
	GOSUB 13600:PRINT "KLINGON INVASION";
	" FORCE OF";K1%;"BATTLE CRUISERS.":Q1%=2%:GOSUB 13600
1100 PRINT "(I.E. UNTIL STARDATE";D2%;").":Q1%=5%:GOSUB 13600:PRINT
	"QUADRANT";E1%"-"E2%,"SECTOR";E7%"-"E8%:PRINT:INPUT "DO YOU";
	" NEED FURTHER INSTRUCTIONS";A$:GOTO 1400 IF LEFT(A$,1%)="N"
1200 PRINT:Q1%=22%:GOSUB 13600:Q1%=23%:GOSUB 13600:
	Q1%=12%:GOSUB 13600:Q1%=19%:GOSUB 13600
1300 GOSUB 13600:Q1%=8%:GOSUB 13600
1400 CLOSE 1%:PRINT
1500 GOSUB 7700:GOSUB 9300:GOSUB 2100:GOTO 1600
1600 GOSUB 9300:PRINT:INPUT "COMMAND";C2%:PRINT:IF C2%<0% OR C2%>5% THEN
	INPUT "DO YOU WANT TO QUIT";R$:IF LEFT(R$,1%)="Y" THEN 13800
	ELSE PRINT:GOTO 1600
1700 ON C2%+1% GOTO 3900,2000,3400,4000,11900,1800
1800 IF R(6%)<0% THEN PRINT "DAMAGE CONTROL NOT AVAILABLE":GOTO 1600
1900 PRINT "DEVICE","STATE OF REPAIR":PRINT M$(I%),R(I%) FOR I%=1% TO 5%:
	GOTO 1600
2000 GOSUB 2100:GOTO 1600

2100 IF R(2%)<0% THEN PRINT "***S.R SENSORS ARE OUT***":RETURN
2200 GOSUB 7700:C2%=26%:GOSUB 6500:FOR I%=1% TO 8%:PRINT " ";MID(S$,
	D%(I%,J%)+1%,1%); FOR J%=1% TO 8%:ON I% GOSUB 2300,2400,2600,2700,
	2800,2900,3000,3100:NEXT I%:GOSUB 6500:RETURN
2300 PRINT , "STARDATE";D1%:RETURN
2400 PRINT ,"CONDITION ";:IF C1%>3% THEN PRINT "CLEAR" ELSE IF C1%=3%
	THEN PRINT "DOCKED" ELSE IF C1%=2% THEN PRINT "RED" ELSE IF
	C1%=1% THEN PRINT "YELLOW" ELSE PRINT "GREEN"
2500 RETURN
2600 PRINT ,"QUADRANT";E1%"-"E2%:RETURN
2700 PRINT ,"SECTOR";E7%"-"E8%:RETURN
2800 PRINT ,"ENERGY";P:RETURN
2900 PRINT ,"TORPEDOES";T1%:RETURN
3000 PRINT ,"KLINGONS";K1%:RETURN
3100 PRINT ,"DAYS LEFT";D2%-D1%:RETURN
3300 ON ERROR GOTO
3400 IF R(3%)<>0% THEN PRINT "***L.R. SENSORS ARE OUT***":GOTO 1600
3500 PRINT "LONG RANGE SENSOR SCAN QUADRANT";E1%;"-";E2%:C2%=19%:GOSUB
	6500
3600 FOR I%=E1%-1% TO E1%+1%:FOR J%=E2%-1% TO E2%+1%:IF I%<1% OR
	I%>8% THEN PRINT ": @@@ : @@@ : @@@ ";:J%=E2%+1%:
	GOTO 3800
3700 IF J%<1% OR J%>8% THEN PRINT ": @@@ "; ELSE PRINT USING ": ### ",
	S%(I%,J%);
3800 NEXT J%:PRINT ":":GOSUB 6500:NEXT I%:GOTO 1600
3900 INPUT "COURSE";C2;"    WARP FACTOR (0-8)";C3:GOTO 3900 IF C3<0 OR
	C3>8:GOTO 5000
4000 GOTO 4200 IF R(4%)=0%:PRINT "*** PHASER CONTROL IS DAMAGED ***":
	GOTO 1600
4200 PRINT "PHASERS LOCKED ON TARGET.  ";
4300 PRINT "ENERGY AVAILABLE =";P:INPUT "NUMBER OF UNITS TO FIRE";C2:
	PRINT:GOTO 4300 IF C2>P OR C2<0%:P=P-C2:GOTO 4900 IF S%(E1%,E2%)<
	100:P5=C2/INT(S%(E1%,E2%)/100):G=1:H=0
4400 FOR I%=1% TO S%(E1%,E2%)/100%
4500 H=H+1:GOTO 4600 IF H<9:H=1:G=G+1:GOTO 4900 IF G>8
4600 GOTO 4500 IF D%(G,H)>2%:Q1%=G-E7%:Q2%=H-E8%:GOTO 4500 IF D%(G,H)<2%:
	D4=SQR(Q1%^2+Q2%^2):P6=P5/D4:J(I%)=J(I%)-P6:PRINT "KLINGON AT";
	G;"-";H;"HIT (";P6;"UNITS )  LEFT =";J(I%)
4700 GOTO 4800 IF J(I%)>0:PRINT "   DESTROYED ***":J(I2%-1%)=J(I2%) FOR
	I2%=I%+1% TO S%(E1%,E2%)/100%:S%(E1%,E2%)=S%(E1%,E2%)-100%:K1%
	=K1%-1%:D%(G,H)=0%:I%=I%-1%
4800 NEXT I%
4900 GOTO 13200  IF K1%=0%:GOSUB 9300:GOSUB 11200:GOTO 1600
5000 GOTO 5100 IF R(1%)=0% OR C3<=.2:PRINT "WARP ENGINES ARE DAMAGED";
	" MAXIMUM WARP = .2":GOTO 3900
5100 P=P-16*C3+5:N1=FIX(8*C3):N2=-COS(C2*PI/180+.000001):N2=0 IF ABS(N2)
	<.01
5200 N3=SIN(C2*PI/180+.0000001):N3=0 IF ABS(N3)<.01:FOR A1%=1% TO N1:
	E3%=E7%:E4%=E8%
5300 P1=FIX(E3%+N2+.4):P2=FIX(E4%+N3+.4):GOTO 5800 IF P1<1 OR P1>8:GOTO
	6000 IF P2<1 OR P2>8:GOTO 6200 IF D%(P1,P2):D%(E3%,E4%)=0%
5400 D%(P1,P2)=4%:E7%=INT(P1):E8%=INT(P2)
5500 NEXT A1%
5600 D1%=D1%+1%:FOR I%=1% TO 6%:GOTO 5700 IF R(I%)=0%:R(I%)=R(I%)+1%:GOTO
	5700 IF R(I%)<0%:R(I%)=0%
5700 NEXT I%:GOTO 6300
5800 S2%=SGN(P1-1%):S3%=E1%+S2%:E7%=INT(P1)-8*S2%:L1%=0%:GOTO 5900 IF S3%<
	1% OR S3%>8%:E1%=E1%+S2%
5900 GOTO 6000 IF P2<1:GOTO 6100 IF P2<8%
6000 S2%=SGN(P2-1):S3%=E2%+S2%:E8%=INT(P2)-8*S2%:L2%=0%:GOTO 6100 IF S3%<
	1% OR S3%>8%:E2%=S3%
6100 GOSUB 7700:GOTO 5500
6200 PRINT "ENTERPRISE BLOCKED AT";INT(P1);"-";INT(P2):GOTO 5600
6300 GOSUB 9300:GOSUB 9800:GOSUB 11200:GOTO 13400 IF P<0 OR D1%>D2%:GOTO
	1600
6500 PRINT "-"; FOR I1%=1% TO C2%:PRINT:RETURN
7700 RETURN IF L1%=E1% AND L2%=E2%
7800 L1%=E1%:L2%=E2%:D%(I%,J%)=0% FOR J%=1% TO 8% FOR I%=1% TO 8%:
	D%(E7%,E8%)=4%:FOR I%=1% TO S%(E1%,E2%)-INT(S%(E1%,E2%)/10)*10
7900 E3%=RND*8%+1%:E4%=RND*8%+1%:GOTO 7900 IF D%(E3%,E4%):D%(E3%,E4%)=
	1%:NEXT I%:Z9%=S%(E1%,E2%):FOR I%=1% TO Z9%/10%-INT(Z9%/100%)*
	10%
8000 E3%=RND*8+1%:E4%=RND*8+1%:GOTO 8000 IF D%(E3%,E4%):D%(E3%,E4%)=
	3%:NEXT I%:FOR I%=1% TO S%(E1%,E2%)/100%:J(I%)=300%
8100 E3%=RND*8%+1%:E4%=RND*8%+1%:GOTO 8100 IF D%(E3%,E4%):
	D%(E3%,E4%)=2%:NEXT I%:RETURN
9300 FOR I%=E7%-1% TO E7%+1%:GOTO 9500 IF I%<1% OR I%>8%:FOR J%=E8%-1%
	TO E8%+1%:GOTO 9400 IF J%<1% OR J%>8%:GOTO 9700 IF D%(I%,J%)=3%
9400 NEXT J%
9500 NEXT I%:C1%=2%:RETURN IF S%(E1%,E2%)>99%:C1%=1%:RETURN IF P<500%
	OR D2%-D1%<6%:C1%=4%:RETURN IF S%(E1%,E2%)=0%:C1%=0%:RETURN
9600 RETURN IF P>500:C1%=1%:RETURN
9700 C1%=3%:P=3000:T1%=10%:R(I%)=0FOR I%=1 TO 6
9800 C2=RND:GOTO 9900 IF C2<.25:RETURN IF C2<.9:PRINT "*** SPACE";
	" STORM,  ";:GOTO 10000 IF C1%>=3%:GOTO 10100
9900 GOTO 10200 IF R(I%)<>0% FOR I%=1% TO 6%:RETURN
10000 PRINT "STARBASE SHIELDS PROTECT ENTERPRISE ***":RETURN
10100 C2%=RND*6%+1%:PRINT M$(C2%);" DAMAGED ***":R(C2%)=R(C2%)-RND*5:
	RETURN
10200 PRINT "*** TRUCE, "M$(I%)" STATE OF REPAIR IMPROVED":R(I%)=
	R(I%)+2*RND:RETURN IF R(I%)<0%:R(I%)=0%:RETURN
10400 !
10500 !
10600 !
10700 !
10800 !
10900 !
11000 !
11200 RETURN IF S%(E1%,E2%)<100%:GOSUB 10000 IF C1%=3%:G=1:H=0:FOR I%=1%
	TO S%(E1%,E2%)/100
11300 H=H+1:GOTO 11400 IF H<=8%:H=1:G=G+1
11400 GOTO 11300 IF D%(G,H)<>2%:Q1%=G-E7%:Q2%=H-E8%:D4=SQR(Q1%^2+Q2%^2):
	P5=INT(RND*(J(I%)-1%))+1%:J(I%)=J(I%)-P5:GOTO 11500 IF C1%=3%:
	P6=P5/D4:P=P-P6:PRINT "ENTERPRISE HIT (";P6;"UNITS )  KLINGON AT";
	" SECTOR";G;"-";H
11500 NEXT I%:IF P<=0 THEN 13400 ELSE RETURN
11900 GOTO 12000 IF R(5%)=0%:PRINT "*** PHOTON TUBES ARE OUT ***":GOTO 1600
12000 GOTO 12100 IF T1%>0:PRINT "TORPEDO SUPPLY EXHAUSTED!!":GOTO 1600
12100 INPUT "TORPEDO COURSE";C2:PRINT:T1%=T1%-1%
12200 N2=-COS(C2*PI/180+1E-13):N2=0 IF ABS(N2)<.01
12300 N3=SIN(C2*PI/180+1E-13):N3=0 IF ABS(N3)<.01
12400 P1=E7%:P2=E8%
12500 P1=P1+N2:P2=P2+N3:PRINT INT(P1*10)/10;"-";INT(P2*10)/10:GOTO 13100
	IF P1<=0 OR P1>=9 OR P2<=0 OR P2>=9:GOTO 12500 IF D%(P1+.5,P2+.5)
	=0:ON D%(P1+.5,P2+.5) GOTO 12600,12700,12800
12600 PRINT "STAR DESTROYED ***":S%(E1%,E2%)=S%(E1%,E2%)-1%:GOTO 12900
12700 PRINT "KLINGON DESTROYED ***":S%(E1%,E2%)=S%(E1%,E2%)-100%:K1%=K1%
	-1%:GOSUB 9300:GOTO 12900
12800 PRINT "STARBASE DESTROYED ***":S%(E1%,E2%)=S%(E1%,E2%)-10%
12900 D%(P1+.5,P2+.5)=0%
13000 GOTO 13200 IF K1%=0%:GOSUB 9300:GOSUB 11200:GOTO 1600
13100 PRINT "TORPEDO MISSED ***":GOTO 13000
13200 PRINT "IT IS STARDATE";D1%:PRINT "THE KLINGONS HAVE BEEN DESTROYED"
	:PRINT "THE FEDERATION IS SAFE.":PRINT T1$+"CONGRADULATIONS"+
	T2$+"!":GOTO 13800
13300 STOP
13400 PRINT "IT IS STARDATE";D1%:& "THE ENTERPRISE HAS BEEN "+
	"DESTROYED.":& "THE FEDERATION WILL BE CONQUERED.":PRINT
	"THERE ARE STILL"+T1$;K1%;T2$+"BATTLE CRUSERS.":&
	T1$+"YOU "+T2$+"ARE "+T1$+"DEAD"+T2$+"!"+T1$+"!"+T2$+
	"!"+T1$+"!"+T2$
13500 GOTO 13800
13600 FOR Q%=1% TO Q1%:INPUT LINE #1%,A$:PRINT A$;:NEXT Q%:RETURN
13800 INPUT "CAPTAIN KIRK, ANOTHER GAME";A$:GOTO 100 IF LEFT(A$,1%)="Y"
	:END
