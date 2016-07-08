all: amazin kmaze snak snoopy revrse works

amazin: amazin.bas
	../src/b1filter amazin.bas  | ../src/btran -i > amazin.cc
	c++ -I ../lib/ amazin.cc -o amazin

kmaze: kmaze.bas
	../src/btran < kmaze.bas > kmaze.cc
	c++ -I ../lib/ kmaze.cc -o kmaze

snak: snak.bas
	../src/b1filter snak.bas  | ../src/btran > snak.cc
	c++ -I ../lib/ snak.cc -o snak

snoopy: snoopy.bas
	../src/btran snoopy.bas > snoopy.cc
	c++ -I ../lib/ snoopy.cc -o snoopy

revrse: revrse.bas
	../src/btran -i < revrse.bas > revrse.cc
	c++ -I ../lib/ revrse.cc -o revrse

works: works.bas
	../src/btran works.bas > works.cc
	c++ -I ../lib/ works.cc -o works
