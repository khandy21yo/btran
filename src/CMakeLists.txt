cmake_minimum_required(VERSION 3.12)

project(btran VERSION 1.0
	DESCRIPTION "VAX Basic to C++ translatoe"
	LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyParser parse.yy ${CMAKE_CURRENT_BINARY_DIR}/parse.cc COMPILE_FLAGS "-d -v")
FLEX_TARGET(MyScanner lex.ll  ${CMAKE_CURRENT_BINARY_DIR}/lex.cc COMPILE_FLAGS "-i -s -v")
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

 add_executable(btran-bin
	main.cc nodes1.cc nodes2.cc nodes3.cc
	program.cc variable.cc varlist.cc yywrap.c basic.h config.h
	nodes.h variable.h varlist.h
	${BISON_MyParser_OUTPUTS}
	${FLEX_MyScanner_OUTPUTS}
 )
set_target_properties(btran-bin
	PROPERTIES OUTPUT_NAME btran)

 add_executable(b1filter b1filter.cc)

 install(TARGETS btran-bin DESTINATION bin)

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /dist
    /.deps
    /.*build.*
    /autom4te.cache
    /\\\\.DS_Store
)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "KTH")
include(CPack)

