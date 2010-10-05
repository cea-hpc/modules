# Created by
#	R.K.Owen	<rk@owen.sj.ca.us>
# for the environment modules project and is covered by the same license.

#------------------------------------------------------------------------
# EM_SET_TCL --
#
#	Allow an alternative to the tclConfig.sh ... adds several
#	configure options for fine tuning
#
# Arguments:
#	none
#
# Results:
#
#	Adds the following arguments to configure:
#		--with-tcl-ver=...
#		--with-tcl-lib=...
#		--with-tcl-inc=...
#
#	Defines the following vars:
#		TCL_VERSION		Tcl version (X.Y)
#		TCL_LIB_SPEC		Full path to the Tcl library directory
#		TCL_INCLUDE_SPEC	Full path to the Tcl include directory
#------------------------------------------------------------------------

AC_DEFUN([EM_SET_TCL], [
	AC_ARG_WITH(tcl-ver,
	AC_HELP_STRING([--with-tcl-ver=X.Y],
	[Tcl version to use [[search]]]), with_tclver=${withval},
	)
	AC_MSG_CHECKING([for Tcl version])
	AC_CACHE_VAL(em_cv_tclver,[
	    # First check to see if --with-tcl-ver was specified.
	    if test x"${with_tclver}" = x ; then #{{
		em_cv_tclver=`echo 'puts [[info tclversion]]' | tclsh 2>/dev/null`
	    else #}{
		em_cv_tclver=${with_tclver}
	    fi #}}

	    tclver=`echo $em_cv_tclver | sed -n -e '/[[0-9]]\.[[0-9]]/p'`
	    if test x"$tclver" = x ; then #{
		AC_MSG_ERROR([Must specify version (X.Y) if using this option])
	    fi #}
	    AC_MSG_RESULT([${em_cv_tclver}])
	])
	TCL_VERSION="${em_cv_tclver}"

	AC_ARG_WITH(tcl-lib,
	AC_HELP_STRING([--with-tcl-lib=<dir>],
	[directory containing tcl libraries (libtclX.Y) [[none]]]),
	[if test "$withval" = "no" || test "$withval" = "yes"; then #{{
	   AC_MSG_ERROR([must give a value if using --with-tcl-lib])
	else #}{
	   with_tcllib=${withval}
	fi #}}], [with_tcllib=not_given])
	if test x"$with_tcllib" != x"not_given" ; then #{
	AC_MSG_CHECKING([for Tcl lib directory])
	AC_CACHE_VAL(em_cv_tcllib,[

	    # First check to see if --with-tcl-lib was specified.
	    if test x"${with_tcllib}" = x ; then #{{
		    AC_MSG_ERROR([Must specify directory if using this option])
	    else #}{
		if test -d "${with_tcllib}" ; then #{{
		    em_cv_tcllib=`(cd ${with_tcllib}; pwd)`
		    AC_MSG_RESULT([${with_tcllib}])
		else #}{
		    AC_MSG_ERROR([${with_tcllib} directory not found])
		fi #}}
	    fi #}}
	])
	TCL_LIB_SPEC="-L${em_cv_tcllib} -ltcl${em_cv_tclver}"
	fi #}

	AC_ARG_WITH(tcl-inc,
	AC_HELP_STRING([--with-tcl-inc=<dir>],
	[directory containing tcl include files (tclDecls.h,...) [[none]]]),
	[if test "$withval" = "no" || test "$withval" = "yes"; then #{{
	   AC_MSG_ERROR([must give a value if using --with-tcl-inc])
	else #}{
	   with_tclinc=${withval}
	fi #}}], [with_tclinc=not_given])
	if test x"$with_tclinc" != x"not_given" ; then
	AC_MSG_CHECKING([for Tcl include directory])
	AC_CACHE_VAL(em_cv_tclinc,[

	    # First check to see if --with-tcl-inc was specified.
	    if test x"${with_tclinc}" = x ; then #{{
		    AC_MSG_ERROR([Must specify directory if using this option])
	    else #}{
		if test -d "${with_tclinc}" ; then #{{
		    em_cv_tclinc=`(cd ${with_tclinc}; pwd)`
		    AC_MSG_RESULT([${with_tclinc}])
		else #}{
		    AC_MSG_ERROR([${with_tclinc} directory not found])
		fi #}}
	    fi #}}
	])
	TCL_INCLUDE_SPEC="-I${em_cv_tclinc}"
	fi
])

#------------------------------------------------------------------------
# EM_CHECK_TCL --
#
#	Make sure we have all the necessary bits
#
# Arguments:
#	none
#
# Results:
#	Either stops the script or outputs the found bits
#		TCL_VERSION		Tcl version (X.Y)
#		TCL_LIB_SPEC		Full path to the Tcl library directory
#		TCL_INCLUDE_SPEC	Full path to the Tcl include directory
#------------------------------------------------------------------------
AC_DEFUN([EM_CHECK_TCL], [
	AC_MSG_CHECKING([TCL_VERSION])
	if test x"$TCL_VERSION" = x ; then #{{
		AC_MSG_ERROR(
		[TCL_VERSION not found, need to use --with-tcl-ver])
	else #}{
		AC_MSG_RESULT([$TCL_VERSION])
	fi #}}
	AC_SUBST(TCL_VERSION)

	AC_MSG_CHECKING([TCL_LIB_SPEC])
	if test x"$TCL_LIB_SPEC" = x ; then #{{
		AC_MSG_ERROR(
		[TCL_LIB_SPEC not found, need to use --with-tcl-lib])
	else #}{
		AC_MSG_RESULT([$TCL_LIB_SPEC])
	fi #}}
	AC_SUBST(TCL_LIB_SPEC)

	AC_MSG_CHECKING([TCL_INCLUDE_SPEC])
	if test x"$TCL_INCLUDE_SPEC" = x ; then #{{
		AC_MSG_ERROR(
		[TCL_INCLUDE_SPEC not found, need to use --with-tcl-inc])
	else #}{
		AC_MSG_RESULT([$TCL_INCLUDE_SPEC])
	fi #}}
	AC_SUBST(TCL_INCLUDE_SPEC)
])
#------------------------------------------------------------------------
# EM_SET_TCLX --
#
#	Allow an alternative to the tclxConfig.sh ... adds several
#	configure options for fine tuning
#
# Arguments:
#	none
#
# Results:
#
#	Adds the following arguments to configure:
#		--with-tclx-ver=...
#		--with-tclx-lib=...
#		--with-tclx-inc=...
#
#	Defines the following vars:
#		TCLX_VERSION		TclX version (X.Y)
#		TCLX_LIB_SPEC		Full path to the TclX library directory
#		TCLX_INCLUDE_SPEC	Full path to the TclX include directory
#------------------------------------------------------------------------

AC_DEFUN([EM_SET_TCLX], [
	AC_ARG_WITH(tclx-ver,
	AC_HELP_STRING([--with-tclx-ver=X.Y],
	[TclX version to use [[search]]]), with_tclxver=${withval},
	)
	AC_MSG_CHECKING([for TclX version])
	AC_CACHE_VAL(em_cv_tclxver,[
	    # First check to see if --with-tclx-ver was specified.
	    if test x"${with_tclxver}" = x ; then #{{
		em_cv_tclxver=`echo 'puts [[info tclversion]]' | tclsh 2>/dev/null`
	    else #}{
		em_cv_tclxver=${with_tclxver}
	    fi #}}

	    tclxver=`echo $em_cv_tclxver | sed -n -e '/[[0-9]]\.[[0-9]]/p'`
	    if test x"$tclxver" = x ; then #{
		AC_MSG_ERROR([Must specify version (X.Y) if using this option])
	    fi #}
	    AC_MSG_RESULT([using ${em_cv_tclxver}])
	])
	TCLX_VERSION="${em_cv_tclxver}"

	AC_ARG_WITH(tclx-lib,
	AC_HELP_STRING([--with-tclx-lib=<dir>],
	[directory containing tclx libraries (libtclxX.Y) [[none]]]),
	[if test "$withval" = "no" || test "$withval" = "yes"; then #{{
	   AC_MSG_ERROR([must give a value if using --with-tclx-lib])
	else #}{
	   with_tclxlib=${withval}
	fi #}}], [with_tclxlib=not_given])
	if test x"$with_tclxlib" != x"not_given" ; then #{
	AC_MSG_CHECKING([for Tclx lib directory])
	AC_CACHE_VAL(em_cv_tclxlib,[

	    # First check to see if --with-tclx-lib was specified.
	    if test x"${with_tclxlib}" = x ; then #{{
		    AC_MSG_ERROR([Must specify directory if using this option])
	    else #}{
		if test -d "${with_tclxlib}" ; then #{{
		    em_cv_tclxlib=`(cd ${with_tclxlib}; pwd)`
		    AC_MSG_RESULT([${with_tclxlib}])
		else #}{
		    AC_MSG_ERROR([${with_tclxlib} directory not found])
		fi #}}
	    fi #}}
	])
	TCLX_LIB_SPEC="-L${em_cv_tclxlib} -ltclx${em_cv_tclxver}"
	fi #}

	AC_ARG_WITH(tclx-inc,
	AC_HELP_STRING([--with-tclx-inc=<dir>],
	[directory containing tclx include files (tclExtend.h,...) [[none]]]),
	[if test "$withval" = "no" || test "$withval" = "yes"; then #{{
	   AC_MSG_ERROR([must give a value if using --with-tclx-inc])
	else #}{
	   with_tclxinc=${withval}
	fi #}}], [with_tclxinc=not_given])
	if test x"$with_tclxinc" != x"not_given" ; then #{
	AC_MSG_CHECKING([for TclX include directory])
	AC_CACHE_VAL(em_cv_tclxinc,[

	    # First check to see if --with-tclx-inc was specified.
	    if test x"${with_tclxinc}" = x ; then #{{
		    AC_MSG_ERROR([Must specify directory if using this option])
	    else #}{
		if test -d "${with_tclxinc}" ; then #{{
		    em_cv_tclxinc=`(cd ${with_tclxinc}; pwd)`
		    AC_MSG_RESULT([${with_tclxinc}])
		else #}{
		    AC_MSG_ERROR([${with_tclxinc} directory not found])
		fi #}}
	    fi #}}
	])
	TCLX_INCLUDE_SPEC="-I${em_cv_tclxinc}"
	fi #}
])

#------------------------------------------------------------------------
# EM_CHECK_TCLX --
#
#	Make sure we have all the necessary bits
#
# Arguments:
#	none
#
# Results:
#	Either stops the script or outputs the found bits
#		TCLX_VERSION		TclX version (X.Y)
#		TCLX_LIB_SPEC		Full path to the TclX library directory
#		TCLX_INCLUDE_SPEC	Full path to the TclX include directory
#------------------------------------------------------------------------
AC_DEFUN([EM_CHECK_TCLX], [
	AC_MSG_CHECKING([TCLX_VERSION])
	if test x"$TCLX_VERSION" = x ; then #{{
		AC_MSG_ERROR(
		[TCLX_VERSION not found, need to use --with-tclx-ver])
	else #}{
		AC_MSG_RESULT([$TCLX_VERSION])
	fi #}}
	AC_SUBST(TCLX_VERSION)

	AC_MSG_CHECKING([TCLX_LIB_SPEC])
	if test x"$TCLX_LIB_SPEC" = x ; then #{{
		no_tclx=true
		no_tclx_lib=true
		AC_MSG_RESULT(
		[TCLX_LIB_SPEC not found, need to use --with-tclx-lib])
	else #}{
		no_tclx=
		no_tclx_lib=
		AC_MSG_RESULT([$TCLX_LIB_SPEC])
	fi #}}
	AC_SUBST(TCLX_LIB_SPEC)

	AC_MSG_CHECKING([TCLX_INCLUDE_SPEC])
	if test x"$TCLX_INCLUDE_SPEC" = x ; then #{{
		no_tclx=true
		AC_MSG_RESULT(
		[TCLX_INCLUDE_SPEC not found, need to use --with-tclx-inc])
	else #}{
		test x"$no_tclx_lib" = x && no_tclx=
		AC_MSG_RESULT([$TCLX_INCLUDE_SPEC])
	fi #}}
	AC_SUBST(TCLX_INCLUDE_SPEC)
])
