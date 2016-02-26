all: amazin kmaze snoopy revrse

amazin: amazin.bas
	../src/b1filter amazin.bas  | ../src/btran -i > amazin.cc
	c++ -I ../lib/ amazin.cc -o amazin

kmaze: kmaze.bas
	../src/btran < kmaze.bas > kmaze.cc
	c++ -I ../lib/ kmaze.cc -o kmaze

revrse: revrse.bas
	../src/btran -i < revrse.bas > revrse.cc
	c++ -I ../lib/ revrse.cc -o revrse

snoopy: snoopy.bas
	../src/btran snoopy.bas > snoopy.cc
	c++ -I ../lib/ snoopy.cc -o snoopy
