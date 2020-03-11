cmake_minimum_required(VERSION 3.12)

project(libbtran VERSION 1.0
	DESCRIPTION "Library for btran"
	LANGUAGES CXX)

add_library(btran STATIC 
	pusing.cc bstring.cc bedit.cc
	basicfun.cc ert.cc
	basicfun.h bstring.h datalist.h pusing.h
	virtual.h basicchannel.cc
	)

target_include_directories(btran
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

add_library(btranvms STATIC 
	smg.cc lib.cc smgdef.h
	)

target_include_directories(btranvms
          INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
          )

