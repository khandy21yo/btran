cmake_minimum_required(VERSION 3.12)

project(libbtran VERSION 1.0
	DESCRIPTION "Library for btran"
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(btran STATIC 
	pusing.cc bstring.cc bedit.cc
	basicfun.cc ert.cc
	basicfun.h bstring.h datalist.h pusing.h
	virtual.h basicchannel.cc
	)

target_include_directories(btran
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

install(TARGETS btran DESTINATION lib)
install(FILES basicfun.h basicchannel.h bstring.h datalist.h pusing.h virtual.h
	DESTINATION include)

add_library(btranvms STATIC 
	smg.cc lib.cc smgdef.h
	)

target_include_directories(btranvms
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

install(TARGETS btranvms DESTINATION lib)
install(FILES smgdef.h libroutines.h smgroutines.h
	DESTINATION include)
