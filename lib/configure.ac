# Process this file with autoconf to produce a configure script.
AC_INIT(libmodules,3.2,librko@kooz.sj.ca.us)
AC_CONFIG_AUX_DIR(../config)
AM_INIT_AUTOMAKE(libmodules,3.2)
AC_CONFIG_SRCDIR([avec.h])
# AM_CONFIG_HEADER([../config.h])
AC_PREREQ(2.52)

# Checks for programs.
AC_PROG_CC
# CFLAGS=-g
AM_PROG_CC_C_O
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_SETVBUF_REVERSED
AC_CHECK_FUNCS([strcasecmp strstr])

dnl ---------------------------------------------------------------------------
dnl MEMDEBUG		Whether to use the memory debug routines
dnl
AC_SUBST(MEMDEBUG)dnl
AH_TEMPLATE([MEMDEBUG],[
MEMDEBUG:
Set this to enable the memory debug routines
])dnl
AC_ARG_ENABLE(memdebug,
	AC_HELP_STRING([--enable-memdebug],[enable memory debugging    [[no]]]),
	if test "$enableval" = "yes"; then
		AC_DEFINE(MEMDEBUG)
	fi
)dnl

dnl ---------------------------------------------------------------------------
AC_CONFIG_FILES(Makefile tests/Makefile)
AC_OUTPUT
