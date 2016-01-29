dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([yywrap.c])

# For automake.
VERSION=1.0.10
PACKAGE=btran

dnl Initialize automake stuff
AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)

dnl Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AM_PROG_LEX
AC_PROG_CXX

dnl Checks for libraries.

dnl Checks for header files.
AM_CONFIG_HEADER(config.h)
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS(malloc.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_STRUCT_TM
AC_TYPE_SIZE_T
AC_EXEEXT
AC_OBJEXT

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_STRFTIME

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
