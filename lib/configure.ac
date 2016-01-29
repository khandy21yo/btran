dnl Process this file with autoconf to produce a configure script.
AC_INIT(pusing.cc)

# For automake.
VERSION=1.0.10
PACKAGE=libbtran

dnl Initialize automake stuff
AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_LIBTOOL

dnl Checks for libraries.

dnl Checks for header files.
AM_CONFIG_HEADER(config.h)
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_OBJEXT

dnl Checks for library functions.
AC_CHECK_FUNCS(strstr)

AC_OUTPUT(Makefile)
