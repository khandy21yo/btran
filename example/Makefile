all: amazin kmaze snak snoopy revrse works unbac trek superstartrek

clean:
	-rm *.o
	-rm amazin.cc
	-rm amazin
	-rm kmaze.cc
	-rm kmaze
	-rm snak.cc
	-rm snak
	-rm snoopy.cc
	-rm snoopy
	--rm recrse.cc
	-rm revrse
	-rm works.cc
	-rm works
	-rm unbac
	-rm trek
	-rm superstartrek

#
# A Maze genertor
# Older basic+ style probram, uses colons as statement seperators
# From  101 basic computer gnes
#
amazin: amazin.bas
	../src/b1filter amazin.bas  | ../src/btran -i > amazin.cc
	c++ -I ../lib/ amazin.cc -o amazin

#
# A Maze generator
# Simple basic+2 program
#
kmaze: kmaze.bas
	../src/btran < kmaze.bas > kmaze.cc
	c++ -I ../lib/ kmaze.cc -o kmaze

#
# Star trek
#
trek: trek.bas
	../src/b1filter trek.bas  | ../src/btran > trek.cc
	c++ -g -I ../lib/ trek.cc -o trek ../lib/libbtran.a

#
#
# Super Star trek
#
trek: superstartrek.bas
	../src/b1filter superstartrek.bas  | ../src/btran -i > superstartrek.cc
	c++ -g -I ../lib/ superstartrek.cc -o superstartrek ../lib/libbtran.a

#
# A Maze generator
# Older basic+ style probram, uses colons as statement seperators
#
snak: snak.bas
	../src/b1filter snak.bas  | ../src/btran > snak.cc
	c++ -I ../lib/ snak.cc -o snak

#
# A Poster
# Older basic+ style probram, uses colons as statement seperators
# From  101 basic computer gnes
#
snoopy: snoopy.bas
	../src/btran snoopy.bas > snoopy.cc
	c++ -I ../lib/ snoopy.cc -o snoopy

#
# A Poster
# Simple Bbasic+2 style probram
# From  101 basic computer gnes
#
revrse: revrse.bas
	../src/btran -i < revrse.bas > revrse.cc
	c++ -I ../lib/ revrse.cc -o revrse

#
# Test code. Does nothing useful.
# This is some test code used during development of
# the translator.
#
works: works.bas
	../src/btran works.bas > works.cc
	c++ -std=c++11 -I ../lib/ works.cc -o works

#
# A convertedprogram that was heavily modified
#
unbac: unbac.cc
	g++ -o unbac -O0 -g -I ../lib  unbac.cc ../lib/libbtran.a

