cmake_minimum_required(VERSION 3.12)

project(btran VERSION 1.0
	DESCRIPTION "VAX Basic to C++ translatoe"
	LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyParser parse.yy ${CMAKE_CURRENT_BINARY_DIR}/parse.cc)
FLEX_TARGET(MyScanner lex.ll  ${CMAKE_CURRENT_BINARY_DIR}/lex.cc)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
 include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

 add_executable(btran
	main.cc nodes1.cc nodes2.cc nodes3.cc
	program.cc variable.cc varlist.cc yywrap.c basic.h config.h
	nodes.h variable.h varlist.h
	${BISON_MyParser_OUTPUTS}
	${FLEX_MyScanner_OUTPUTS}
 )
 add_executable(b1filter b1filter.cc)

 install(TARGETS btran DESTINATION bin)

