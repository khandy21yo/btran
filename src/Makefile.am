bin_PROGRAMS = btran b1filter unmbasic ungwbasic

## The translator itself
btran_SOURCES = parse.yy main.cc nodes1.cc nodes2.cc nodes3.cc \
program.cc variable.cc varlist.cc yywrap.c lex.ll basic.h config.h \
nodes.h osfcn.h parse.h variable.h varlist.h

## Filter for really old programs
b1filter_SOURCES = b1filter.cc

## mbasic de-tokenizer
unmbasic_SOURCES = unmbasic.cc

## mbasic de-tokenizer
ungwbasic_SOURCES = ungwbasic.cc

## Flags necessary to make YACC do the right thing
AM_YFLAGS = -d -v

## Flags necessary to make LEX do the right thing
AM_LFLAGS = -i -s -v

## This is needed because of the stupid rules automake has for YACC
parse.h: parse.cc
