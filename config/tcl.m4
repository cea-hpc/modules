# This software is copyrighted by the Regents of the University of
# California, Sun Microsystems, Inc., Scriptics Corporation, ActiveState
# Corporation and other parties.  The following terms apply to all files
# associated with the software unless explicitly disclaimed in
# individual files.
# 
# The authors hereby grant permission to use, copy, modify, distribute,
# and license this software and its documentation for any purpose, provided
# that existing copyright notices are retained in all copies and that this
# notice is included verbatim in any distributions. No written agreement,
# license, or royalty fee is required for any of the authorized uses.
# Modifications to this software may be copyrighted by their authors
# and need not follow the licensing terms described here, provided that
# the new terms are clearly indicated on the first page of each file where
# they apply.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
# DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
# IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
# NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
# MODIFICATIONS.
# 
# GOVERNMENT USE: If you are acquiring this software on behalf of the
# U.S. government, the Government shall have only "Restricted Rights"
# in the software and related documentation as defined in the Federal 
# Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
# are acquiring the software on behalf of the Department of Defense, the
# software shall be classified as "Commercial Computer Software" and the
# Government shall have only "Restricted Rights" as defined in Clause
# 252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
# authors grant the U.S. Government and others acting in its behalf
# permission to use and distribute the software in accordance with the
# terms specified in this license. 
#------------------------------------------------------------------------
# License file provided with the Tcl8.4 source distribution
#
# parts have been removed or edited by
#	R.K.Owen	<rk@owen.sj.ca.us>
# for the environment modules project

#------------------------------------------------------------------------
# SC_PATH_TCLCONFIG --
#
#	Locate the tclConfig.sh file and perform a sanity check on
#	the Tcl compile flags
#
# Arguments:
#	none
#
# Results:
#
#	Adds the following arguments to configure:
#		--with-tcl=...
#
#	Defines the following vars:
#		TCL_BIN_DIR	Full path to the directory containing
#				the tclConfig.sh file
#------------------------------------------------------------------------

AC_DEFUN([SC_PATH_TCLCONFIG], [
    #
    # Ok, lets find the tcl configuration
    # First, look for one uninstalled.
    # the alternative search directory is invoked by --with-tcl
    #

    if test x"${no_tcl}" = x ; then #{
	# we reset no_tcl in case something fails here
	no_tcl=true
	AC_ARG_WITH(tcl,
	[AC_HELP_STRING([--with-tcl=<dir>],
	[directory containing tcl configuration (tclConfig.sh) [[searches]]])],
	[with_tclconfig=${withval}],
	[with_tclconfig=search]
	)
	AC_MSG_CHECKING([for Tcl configuration (tclConfig.sh)])
	if test x"$with_tclconfig" = x"no" ; then #{{
	    AC_MSG_RESULT([disabled])
	else #}{
	AC_CACHE_VAL(em_cv_tclconfig,[

	    # First check to see if --with-tcl was specified.
	    if test x"${with_tclconfig}" != xsearch ; then #{
		if test -f "${with_tclconfig}/tclConfig.sh" ; then #{{
		    em_cv_tclconfig=`(cd ${with_tclconfig}; pwd)`
		else #}{
		    AC_MSG_ERROR([${with_tclconfig} directory doesn't contain tclConfig.sh])
		fi #}}
	    fi #}

	    # then check for a private Tcl installation
	    if test x"${em_cv_tclconfig}" = x ; then #{
		for i in \
			../tcl \
			`ls -dr ../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../tcl[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../tcl[[8-9]].[[0-9]]* 2>/dev/null` \
			../../tcl \
			`ls -dr ../../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../../tcl[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../../tcl[[8-9]].[[0-9]]* 2>/dev/null` \
			../../../tcl \
			`ls -dr ../../../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../../../tcl[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../../../tcl[[8-9]].[[0-9]]* 2>/dev/null` \
		; do
		    if test -f "$i/unix/tclConfig.sh" ; then #{
			em_cv_tclconfig=`(cd $i/unix; pwd)`
			break
		    fi #}
		done
	    fi #}

	    # check in a few common install locations
	    if test x"${em_cv_tclconfig}" = x ; then #{
		for i in `ls -d ${libdir} 2>/dev/null` \
			`ls -d /usr/local/lib 2>/dev/null` \
			`ls -d /usr/local/lib/tcl[[8-9]].[[0-9]]* 2>/dev/null` \
			`ls -d /usr/contrib/lib 2>/dev/null` \
			`ls -d /usr/contrib/lib/tcl[[8-9]].[[0-9]]* 2>/dev/null` \
			`ls -d /usr/lib 2>/dev/null` \
			`ls -d /usr/lib/tcl[[8-9]].[[0-9]]* 2>/dev/null` \
			; do
		    if test -f "$i/tclConfig.sh" ; then #{
			em_cv_tclconfig=`(cd $i; pwd)`
			break
		    fi #}
		done
	    fi #}

	    # check in a few other private locations
	    if test x"${em_cv_tclconfig}" = x ; then #{
		for i in \
			${srcdir}/../tcl \
			`ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ${srcdir}/../tcl[[8-9]].[[0-9]]* 2>/dev/null` ; do
		    if test -f "$i/unix/tclConfig.sh" ; then #{
		    em_cv_tclconfig=`(cd $i/unix; pwd)`
		    break
		    fi #}
		done
	    fi #}
	])
	if test x"${em_cv_tclconfig}" = x ; then #{{
	    TCL_BIN_DIR="# no Tcl configs found"
	    AC_MSG_RESULT([not found])
	else #}{
	    no_tcl=
	    TCL_BIN_DIR=${em_cv_tclconfig}
	    AC_MSG_RESULT([found $TCL_BIN_DIR/tclConfig.sh])
	fi #}}
	fi #}}
    fi #}
])

#------------------------------------------------------------------------
# SC_LOAD_TCLCONFIG --
#
#	Load the tclConfig.sh file
#
# Arguments:
#	
#	Requires the following vars to be set:
#		TCL_BIN_DIR
#
# Results:
#
#	Subst the following vars:
#		TCL_BIN_DIR
#		TCL_SRC_DIR
#		TCL_LIB_FILE
#
#------------------------------------------------------------------------

AC_DEFUN([SC_LOAD_TCLCONFIG], [
  if test x"$no_tcl" != x"true" ; then #{
    AC_MSG_CHECKING([for existence of tclConfig.sh])

    if test -f "$TCL_BIN_DIR/tclConfig.sh" ; then #{{
        AC_MSG_RESULT([loading])
	. "$TCL_BIN_DIR"/tclConfig.sh
    else #}{
        AC_MSG_RESULT([file not found])
    fi #}}

    #
    # If the TCL_BIN_DIR is the build directory (not the install directory),
    # then set the common variable name to the value of the build variables.
    # For example, the variable TCL_LIB_SPEC will be set to the value
    # of TCL_BUILD_LIB_SPEC. An extension should make use of TCL_LIB_SPEC
    # instead of TCL_BUILD_LIB_SPEC since it will work with both an
    # installed and uninstalled version of Tcl.
    #

    if test -f "$TCL_BIN_DIR/Makefile" ; then #{
        TCL_LIB_SPEC=${TCL_BUILD_LIB_SPEC}
        TCL_STUB_LIB_SPEC=${TCL_BUILD_STUB_LIB_SPEC}
        TCL_STUB_LIB_PATH=${TCL_BUILD_STUB_LIB_PATH}
    fi #}

    #
    # eval is required to do the TCL_DBGX substitution
    #

    eval "TCL_LIB_FILE=\"${TCL_LIB_FILE}\""
    eval "TCL_LIB_FLAG=\"${TCL_LIB_FLAG}\""
    eval "TCL_LIB_SPEC=\"${TCL_LIB_SPEC}\""

    eval "TCL_STUB_LIB_FILE=\"${TCL_STUB_LIB_FILE}\""
    eval "TCL_STUB_LIB_FLAG=\"${TCL_STUB_LIB_FLAG}\""
    eval "TCL_STUB_LIB_SPEC=\"${TCL_STUB_LIB_SPEC}\""

#   AC_SUBST(TCL_VERSION)
#   AC_SUBST(TCL_BIN_DIR)
#   AC_SUBST(TCL_SRC_DIR)

#   AC_SUBST(TCL_LIB_FILE)
#   AC_SUBST(TCL_LIB_FLAG)
#   AC_SUBST(TCL_LIB_SPEC)

#   AC_SUBST(TCL_STUB_LIB_FILE)
#   AC_SUBST(TCL_STUB_LIB_FLAG)
#   AC_SUBST(TCL_STUB_LIB_SPEC)

#   AC_SUBST(TCL_INCLUDE_SPEC)
  fi #}
])

#------------------------------------------------------------------------
# EM_PATH_TCLXCONFIG --
#
#	Locate the tclxConfig.sh file and perform a sanity check on
#	the TclX compile flags
#
# Arguments:
#	none
#
# Results:
#
#	Adds the following arguments to configure:
#		--with-tclx=...
#
#	Defines the following vars:
#		TCLX_BIN_DIR	Full path to the directory containing
#				the tclxConfig.sh file
#------------------------------------------------------------------------

AC_DEFUN([EM_PATH_TCLXCONFIG], [
    #
    # Ok, lets find the TclX configuration
    # First, look for one uninstalled.
    # the alternative search directory is invoked by --with-tcl
    #

    if test x"${no_tclx}" = x ; then #{
	# we reset no_tclx in case something fails here
	no_tclx=true
	AC_ARG_WITH(tclx,
	AC_HELP_STRING([--with-tclx=<dir>],
	[directory containing TclX configuration (tclxConfig.sh) [[searches]]]),
	[with_tclxconfig=${withval}],
	[with_tclxconfig=search]
	)
	if test x"$with_tclxconfig" = x"no" ; then #{{
		AC_MSG_NOTICE([Skipping TclX configuration])
		no_tclx=true
	else #}{
		AC_MSG_CHECKING([for TclX configuration (tclxConfig.sh)])
		AC_CACHE_VAL(em_cv_tclxconfig,[

	    # First check to see if --with-tclx was specified.
	    if test x"${with_tclxconfig}" != xsearch ; then #{
		if test -f "${with_tclxconfig}/tclxConfig.sh" ; then #{{
		    em_cv_tclxconfig=`(cd ${with_tclxconfig}; pwd)`
		else #}{
		    AC_MSG_ERROR([${with_tclxconfig} directory does not contain tclxConfig.sh])
		fi #}}
	    fi #}

	    # then check for a private Tclx installation
	    if test x"${em_cv_tclxconfig}" = x ; then #{
		for i in \
			../tclx \
			`ls -dr ../tclx[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../tclx[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../tclx[[8-9]].[[0-9]]* 2>/dev/null` \
			../../tclx \
			`ls -dr ../../tclx[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../../tclx[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../../tclx[[8-9]].[[0-9]]* 2>/dev/null` \
			../../../tclx \
			`ls -dr ../../../tclx[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ../../../tclx[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ../../../tclx[[8-9]].[[0-9]]* 2>/dev/null` \
		; do
		    if test -f "$i/unix/tclxConfig.sh" ; then #{
			em_cv_tclxconfig=`(cd $i/unix; pwd)`
			break
		    fi #}
		done
	    fi #}

	    # check in a few common install locations
	    if test x"${em_cv_tclxconfig}" = x ; then #{
		for i in `ls -d ${libdir} 2>/dev/null` \
			`ls -d /usr/local/lib 2>/dev/null` \
			`ls -d /usr/local/lib/tclx[[8-9]].[[0-9]]* 2>/dev/null` \
			`ls -d /usr/contrib/lib 2>/dev/null` \
			`ls -d /usr/contrib/lib/tclx[[8-9]].[[0-9]]* 2>/dev/null` \
			`ls -d /usr/lib 2>/dev/null` \
			`ls -d /usr/lib/tclx[[8-9]].[[0-9]]* 2>/dev/null` \
			; do
		    if test -f "$i/tclxConfig.sh" ; then #{
			em_cv_tclxconfig=`(cd $i; pwd)`
			break
		    fi #}
		done
	    fi #}

	    # check in a few other private locations
	    if test x"${em_cv_tclxconfig}" = x ; then #{
		for i in \
			${srcdir}/../tclx \
			`ls -dr ${srcdir}/../tclx[[8-9]].[[0-9]].[[0-9]]* 2>/dev/null` \
			`ls -dr ${srcdir}/../tclx[[8-9]].[[0-9]] 2>/dev/null` \
			`ls -dr ${srcdir}/../tclx[[8-9]].[[0-9]]* 2>/dev/null` ; do
		    if test -f "$i/unix/tclxConfig.sh" ; then
		    em_cv_tclxconfig=`(cd $i/unix; pwd)`
		    break
		fi
		done
	    fi #}
		])

		if test x"${em_cv_tclxconfig}" = x ; then #{{
		    TCLX_BIN_DIR="# no TclX configs found"
		    AC_MSG_RESULT([not found])
		else #}{
		    no_tclx=
		    TCLX_BIN_DIR=${em_cv_tclxconfig}
		    AC_MSG_RESULT([found $TCLX_BIN_DIR/tclxConfig.sh])
		fi #}}
	fi #}}
    fi #}
])

#------------------------------------------------------------------------
# EM_LOAD_TCLXCONFIG --
#
#	Load the tclxConfig.sh file
#
# Arguments:
#	
#	Requires the following vars to be set:
#		TCLX_BIN_DIR
#
# Results:
#
#	Subst the following vars:
#		TCLX_BIN_DIR
#		TCLX_SRC_DIR
#		TCLX_LIB_FILE
#
#------------------------------------------------------------------------

AC_DEFUN([EM_LOAD_TCLXCONFIG], [
  if test x"$no_tclx" = x"true" ; then #{{
    AC_SUBST(TCLX_LIB_SPEC,"")
  else #}{
    AC_MSG_CHECKING([for existence of tclxConfig.sh])

    if test -f "$TCLX_BIN_DIR/tclxConfig.sh" ; then #{{
        AC_MSG_RESULT([loading])
	. $TCLX_BIN_DIR/tclxConfig.sh
    else #}{
        AC_MSG_RESULT([file not found])
    fi #}}

#   AC_SUBST(TCLX_FULL_VERSION)
    AC_SUBST(TCLX_VERSION)
#   AC_SUBST(TCLX_DEFS)
#   AC_SUBST(TCLX_LIBS)
#   AC_SUBST(TCLX_BUILD_LIB_SPEC)
    AC_SUBST(TCLX_LIB_SPEC)
#   AC_SUBST(TCLX_LD_SEARCH_FLAGS)
  fi #}}
])
#------------------------------------------------------------------------
