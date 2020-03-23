cmake_minimum_required(VERSION 3.12)

project(btran_all VERSION 1.0
	DESCRIPTION "VAX Basic to C++ translatoe"
	LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(lib)
