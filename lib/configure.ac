#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([Envmodules], [1.0.1], [modules-interest@lists.sourceforge.net], , http://modules.sf.net)
AC_CONFIG_SRCDIR([envmodules.c])
AC_CONFIG_HEADERS([config.h])

# Setup TEA_ macros.
TEA_INIT([3.13])

# Load the tclConfig.sh file.
TEA_PATH_TCLCONFIG
TEA_LOAD_TCLCONFIG

# Checks for programs.

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h errno.h limits.h sys/types.h dirent.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SSIZE_T

# Checks for library functions.

# TEA-specific setup
TEA_SETUP_COMPILER
TEA_PUBLIC_TCL_HEADERS
TEA_CONFIG_CFLAGS
TEA_ENABLE_SYMBOLS
TEA_MAKE_LIB
TEA_ENABLE_SHARED

# Perform substitutions from the Tcl configuration
AC_SUBST(TCL_CC)
AC_SUBST(TCL_INCLUDE_SPEC)
AC_SUBST(TCL_SHLIB_CFLAGS)
AC_SUBST(TCL_SHLIB_LD)
AC_SUBST(TCL_SHLIB_SUFFIX)
AC_SUBST(TCL_STUB_LIB_SPEC)
AC_SUBST(SHLIB_SUFFIX)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
