dnl
dnl File:        configure.in
dnl Revision:    $Id: configure.ac,v 1.4 2002/06/13 16:16:03 rkowen Exp $
dnl Created:     94/06/18
dnl Author:      Leif Hedstrom<hedstrom@inf.ethz.ch>
dnl		 Martin S. Utesch <wstd@hl.siemens.de>
dnl 		 Peter W. Osel <pwo@GUUG.DE>
dnl
dnl Process this file with autoconf to produce a configure script
dnl You need autoconf 2.52 or better!
dnl
dnl --------------------------------------------------------------
AC_INIT
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(modules,3.2)
AC_CONFIG_SRCDIR([version.c])
AM_CONFIG_HEADER(config.h)
AC_PREREQ(2.52)

dnl --------------------------------------------------------------
dnl
dnl Some special substitution we need:
dnl
dnl STATIC		Link the binary 'static'
dnl WITH_LOGGING	Whether syslogging is enabled
dnl MODULEPATH		The modulepath we want
dnl VERSIONPATH		The modulepath different versions of modules
dnl AUTOLOADPATH	directories where Tcl will search for libraries to load
dnl ETCPATH		Where to find the /etc/"login" files
dnl SKELPATH		Where to find the $HOME/dot file skeletons
dnl DATE		Module package date
dnl VERSION		Module package version
dnl THISDIR		Full path to this source directory
dnl
AC_SUBST(STATIC)dnl
AC_SUBST(WITH_LOGGING)dnl
AC_SUBST(MODULEPATH)dnl
AC_SUBST(VERSIONPATH)dnl
AC_SUBST(AUTOLOADPATH)dnl
AC_SUBST(ETCPATH)dnl
AC_SUBST(SKELPATH)dnl
AC_SUBST(DOT_EXT)dnl
AC_SUBST(VERSION)dnl
AC_SUBST(DATE)dnl
AC_SUBST(THISDIR)dnl
dnl --------------------------------------------------------------
dnl Need to know if versioning is asked for before setting certain paths
dnl
dnl --enable-versioning		Use modules versioning 	[DEF]
dnl --disable-versioning	Without  - " -
dnl --------------------------------------------------------------
AC_ARG_ENABLE(versioning,
	[  --enable-versioning         use modules versioning          [yes]],
	if test "$enableval" = "yes"; then
	    WANTS_VERSIONING=1
	    AC_DEFINE(WANTS_VERSIONING,1)
	else
	    WANTS_VERSIONING=0
	    AC_DEFINE(WANTS_VERSIONING,0)
	fi, WANTS_VERSIONING=1
	    AC_DEFINE(WANTS_VERSIONING,1))
dnl
dnl --------------------------------------------------------------
dnl Get Modules version & date
dnl --------------------------------------------------------------
VERSION='(unknown)'
VERSION=`sed -n -e 's/^.*version_string.*"\(.*\)";$/\1/p' ${srcdir}/version.c`
AC_MSG_RESULT([VERSION = $VERSION])
DATE='(DDMmmYYYY)'
DATE=`sed -n -e 's/^.*date_string.*"\(.*\)";$/\1/p' ${srcdir}/version.c`
AC_MSG_RESULT([DATE = $DATE])
echo "configuring for Modules $VERSION		$DATE"
THISDIR=`pwd`
dnl --------------------------------------------------------------
dnl
dnl --enable-use-default        Use PREFIX - VERSION in init scripts [DEF]
dnl --disable-use-default       Use PREFIX in init scripts
dnl
AC_MSG_RESULT([THISDIR = $THISDIR])
dnl --------------------------------------------------------------
DEFAULTPATH=default
if test "$prefix" != "NONE" ; then
  if test "$WANTS_VERSIONING" -eq 1 ; then
    DEFAULTPATH=$prefix/Modules/default
    NODEFAULTPATH=$prefix/Modules/$VERSION
    BASEPREFIX=$prefix
    prefix=$BASEPREFIX/Modules/$VERSION
    export prefix
  else
    DEFAULTPATH=$prefix/Modules
    NODEFAULTPATH=$prefix/Modules
    BASEPREFIX=$prefix
    prefix=$BASEPREFIX/Modules
    export prefix
  fi
  PREFIX=$prefix
  MODULES_INIT_DIR=$prefix/init
else
  if test "$WANTS_VERSIONING" -eq 1 ; then
    DEFAULTPATH=$ac_default_prefix/Modules/default
    NODEFAULTPATH=$ac_default_prefix/Modules/$VERSION
    BASEPREFIX=$ac_default_prefix
    ac_default_prefix=$BASEPREFIX/Modules/$VERSION
    export ax_default_prefix
  else
    DEFAULTPATH=$ac_default_prefix/Modules
    NODEFAULTPATH=$ac_default_prefix/Modules
    BASEPREFIX=$ac_default_prefix
    ac_default_prefix=$BASEPREFIX/Modules
    export ac_default_prefix
  fi
  MODULES_INIT_DIR=$ac_default_prefix/init
  PREFIX=$ac_default_prefix
fi
AC_DEFINE_UNQUOTED(MODULES_INIT_DIR, "$MODULES_INIT_DIR")
AC_ARG_ENABLE(use-default,
	[  --enable-use-default        use DEFAULTPATH in init scripts [yes]],
	if test "$enableval" = "no" -o $WANTS_VERSIONING -eq 0; then
		DEFAULTPATH=$NODEFAULTPATH
		AC_SUBST(DEFAULTPATH)
	fi)
AC_MSG_RESULT([DEFAULTPATH = $DEFAULTPATH])
AC_MSG_RESULT([BASEPREFIX = $BASEPREFIX])
AC_MSG_RESULT([PREFIX = $PREFIX])
AC_SUBST(DEFAULTPATH)
AC_SUBST(BASEPREFIX)
AC_DEFINE_UNQUOTED(BASEPREFIX, "$BASEPREFIX")
AC_DEFINE_UNQUOTED(PREFIX, "$PREFIX")
export DEFAULTPATH
export NODEFAULTPATH
export BASEPREFIX
export MODULES_INIT_DIR
dnl --------------------------------------------------------------
dnl   --with-module-path
dnl   --with-version-path
dnl   --with-autoload-path
dnl   --with-etc-path
dnl   --with-skel-path
dnl
AC_ARG_WITH(module-path,
	[  --with-module-path=<path>   use module path=<path>        [$ac_default_prefix/Modules/modulefiles]],
	MODULEPATH="$withval"
	AC_MSG_RESULT([MODULEPATH = $with_module_path])
	AC_DEFINE_UNQUOTED(MODULEPATH, "$withval")
	,
dnl 	AC_MSG_WARN([if using '--with-module-path': need to specify a path!])
	withval="$BASEPREFIX/Modules/modulefiles"
	AC_MSG_WARN([will use MODULEPATH=$withval : rerun configure using --with-module-path to override default])
	MODULEPATH="$withval"
	AC_DEFINE_UNQUOTED(MODULEPATH, "$withval"))
dnl
dnl
AC_ARG_WITH(version-path,
	[  --with-version-path=<path>  use version path=<path>       [$ac_default_prefix/Modules/versions]],
	VERSIONPATH="$withval"
	AC_MSG_RESULT([VERSIONPATH = $with_version_path])
	AC_DEFINE_UNQUOTED(VERSIONPATH, "$withval")
	,
dnl AC_MSG_WARN([if using '--with-version-path': need to specify a path!])
	withval="$BASEPREFIX/Modules/versions"
	AC_MSG_WARN([will use VERSIONPATH=$withval : rerun configure using --with-version-path to override default])
	VERSIONPATH="$withval"
	AC_DEFINE_UNQUOTED(VERSIONPATH, "$withval"))
AC_ARG_WITH(autoload-path,
	[  --with-autoload-path=<path> use autoload path=<path>      [$ac_default_prefix/lib/tcl]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
		if test "$prefix" != "NONE" ; then
			withval="$prefix/lib/tcl"
		else
			withval="$ac_default_prefix/lib/tcl"
		fi
	fi
	AUTOLOADPATH="$withval"
	AC_DEFINE_UNQUOTED(AUTOLOADPATH, "$withval"))
AC_ARG_WITH(etc-path,
	[  --with-etc-path=<path>      use etc path=<path>           [/etc]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
	AC_MSG_WARN([if using '--with-etc-path': need to specify a path! Using /etc])
		withval="/etc"
	fi
	ETCPATH="$withval"
	,
	ETCPATH="/etc")
AC_ARG_WITH(skel-path,
	[  --with-skel-path=<path>     use skel path=<path>          [/etc/skel]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
	AC_MSG_WARN([if using '--with-skel-path': need to specify a path! Using /etc/skel])
		withval="/etc/skel"
	fi
	SKELPATH="$withval"
	,
	SKELPATH="/etc/skel")
AC_ARG_WITH(dot-ext,
	[  --with-dot-ext[=<.ext>]     append <.ext> to shell init   [.ext]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval=".ext"
		AC_DEFINE_UNQUOTED(DOT_EXT, "$withval")
	else
		AC_DEFINE_UNQUOTED(DOT_EXT, "")
	fi)
AC_ARG_WITH(tmp-dir,
	[  --with-tmp-dir=<dir>]     use <dir> as a temporary dir    [/tmp]],
        if test ! -d $withval ; then
           AC_MSG_ERROR([tmp dir $withval does not exist])
        fi
	AC_DEFINE_UNQUOTED(TMP_DIR, "$withval")
	AC_MSG_RESULT([TMP_DIR = $withval]),
        if test -d "/tmp" ; then
           AC_DEFINE_UNQUOTED(TMP_DIR, "/tmp")
        elif test -d "/var/tmp" ; then
           AC_DEFINE_UNQUOTED(TMP_DIR, "/var/tmp")
        fi
	)
dnl
dnl Check for cc
AC_PROG_CC
dnl --------------------------------------------------------------
dnl Some standard autoconfig stuff
dnl Checks for programs
dnl --------------------------------------------------------------
AC_PROG_CC
AC_PROG_CPP
AC_PROG_GCC_TRADITIONAL
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_AIX
AC_ISC_POSIX
AC_PATH_PROG([UNAME],[uname],[""])
AC_PATH_X
AC_PATH_XTRA
if test "$no_x" != "yes"; then
	AC_DEFINE(HAS_X11LIBS)
	AC_CHECK_LIB(X11, XMaxRequestSize)
	AC_CHECK_LIB(Xmu, XmuGetHostname)
fi

dnl --------------------------------------------------------------
dnl Checks for header files
dnl --------------------------------------------------------------

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS(string.h memory.h stdlib.h unistd.h termio.h fcntl.h ctype.h \
	stdarg.h varargs.h syslog.h)
AC_CHECK_HEADERS(errno.h sys/ioctl.h sys/termios.h sys/mode.h sys/stat.h      \
	sys/param.h)
dnl --------------------------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics.
dnl --------------------------------------------------------------
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM
dnl --------------------------------------------------------------
dnl Checks for libraries
dnl --------------------------------------------------------------
dnl Solaris 2.x: This requires -lnsl and -lsocket. If AC_PATH_XTRA
dnl              did not search for these, search them explicitely.
dnl              MUST be done BEFORE AC_PATH_XTRA!
dnl --------------------------------------------------------------
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(nsl, t_accept)

dnl --------------------------------------------------------------
dnl Checks for library functions.
dnl --------------------------------------------------------------
AC_CHECK_FUNCS([strdup strtok uname gethostname getdomainname \
	mktemp tmpnam tempnam \
	syslog dup2])
EM_PATH_PROG_SEARCH(_CPPSTDIN, cpp, /lib /usr/lib /usr/lang \
	/usr/local/lang /usr/ccs/lib, [${CC} -E])
AC_DEFINE_UNQUOTED(CPPSTDIN, "$_CPPSTDIN")
AC_PATH_PROG([_PHOST],[hostname],[""])
test "$_PHOST" != ""  && AC_DEFINE_UNQUOTED(PHOSTNAME, "$_PHOST")
AC_PATH_PROG(RUNTEST, runtest, "./not_installed")
test "$RUNTEST" = "" && echo "Install DEJAGNU and rerun configure if you \
want to run the testsuite"

dnl
dnl Here we define some --with & --enable things:
dnl
dnl --with-force-path=<path>	With force path=<path>
dnl --without-force-path	Without  - " -		[DEF]
dnl
dnl --with-split-size[=<size>]	With split size=<size>
dnl --without-split-size	Without  - " -		[DEF]
dnl
dnl --with-static[=<option>]	With static linking
dnl --without-static		Without  - " -		[DEF]
dnl
dnl --enable-cache		With directory cache	[DEF]
dnl --disable-cache		Without  - " -
dnl
dnl --enable-shell-funcs	With sh functions	[DEF]
dnl --disable-shell-funcs	Without  - " -
dnl
dnl --enable-shell-eval		With shell alias eval
dnl --disable-shell-eval	Without  - " -		[DEF]
dnl
dnl --enable-free		With free() calls
dnl --disable-free		Without  - " -		[DEF]
dnl
dnl --with-debug[=<option>]	Set up debugging level
dnl --without-debug		Without  - " -		[DEF]
dnl
dnl --enable-version-magic      Set .version magic requirement [DEF]
dnl --disable-version-magic     .version need not have magic cookie
dnl
AC_ARG_WITH(force-path,
	[  --with-force-path=<path>    use force path=<path>         [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="/usr/bin"
		AC_DEFINE_UNQUOTED(FORCE_PATH, "$withval")
	fi)
AC_ARG_WITH(split-size,
	[  --with-split-size[=<size>]  use split size=<size>         [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval=900
		AC_DEFINE_UNQUOTED(LMSPLIT_SIZE, ${withval})
	fi)
AC_ARG_WITH(static,
	[  --with-static[=<option>]    use static linking            [no]],
	EM_SET_STATIC(STATIC, $withval))
AC_ARG_ENABLE(cache,
	[  --enable-cache              use directory cache             [yes]],
	if test "$enableval" = "yes"; then
	    AC_DEFINE(CACHE_AVAIL)
	fi,
	AC_DEFINE(CACHE_AVAIL))
AC_ARG_ENABLE(shell-funcs,
	[  --enable-shell-funcs        use sh functions                [yes]],
	if test "$enableval" = "yes"; then
	    AC_DEFINE(HAS_BOURNE_FUNCS)
	fi, AC_DEFINE(HAS_BOURNE_FUNCS))
AC_ARG_ENABLE(shell-eval,
	[  --enable-shell-eval         use shell alias eval            [no]],
	if test "$enableval" = "yes"; then
	    AC_DEFINE(EVAL_ALIAS)
	fi)
AC_ARG_ENABLE(free,
	[  --enable-free               use free() calls                [no]],
	if test "$enableval" = "yes"; then
	    AC_DEFINE(USE_FREE)
	fi)
AC_ARG_WITH(cache-umask,
	[  --with-cache-umask=<umask>  use cache files umask=<umask> [000]],
	if test "$withval" != "no"; then
	    test "$withval" = "yes" && withval=0
	    AC_DEFINE_UNQUOTED(CACHE_UMASK, ${withval})
	fi)
AC_ARG_WITH(debug,
	[  --with-debug=<value>        set debugging level <value>   [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="1"
		AC_DEFINE_UNQUOTED(WITH_DEBUG_INFO, ${withval})
	fi)
AC_ARG_ENABLE(version-magic,
	[  --enable-version-magic      set .version magic requirement  [yes]],
	if test "$enableval" != "no"; then
		AC_DEFINE(VERSION_MAGIC,1)
	else
		AC_DEFINE(VERSION_MAGIC,0)
	fi, AC_DEFINE(VERSION_MAGIC,1))
dnl --------------------------------------------------------------
dnl  Logging
dnl  The following option allow switching logging on and off
dnl  and provide a possibility to set up the default facilities
dnl --------------------------------------------------------------
WITH_LOGGING=0
AC_ARG_ENABLE(logging,
	[  --enable-logging            turn logging on                 [no]],
	if test "$enableval" = "yes"; then
		AC_DEFINE(WITH_LOGGING)
		WITH_LOGGING=1
	fi)
AC_ARG_WITH(log-dir,
	[  --with-log-dir=<value>      select an alternate log dir.  [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="/tmp"
		AC_DEFINE_UNQUOTED(WITH_LOG_DIR, "$withval")
	fi)
AC_ARG_WITH(log-facility-verbose,
	[  --with-log-facility-verbose[=<facility>]  Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="stderr"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_VERBOSE, "$withval")
	fi)
AC_ARG_WITH(log-facility-info,
	[  --with-log-facility-info[=<facility>]     Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.notice"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_INFO, "$withval")
	fi)
AC_ARG_WITH(log-facility-debug,
	[  --with-log-facility-debug[=<facility>]    Log facility    [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.notice"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_DEBUG, "$withval")
	fi)
AC_ARG_WITH(log-facility-trace,
	[  --with-log-facility-trace[=<facility>]    Log facility    [no]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.notice"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_TRACE, "$withval")
	fi)
AC_ARG_WITH(log-facility-warn,
	[  --with-log-facility-warn[=<facility>]     Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.warning"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_WARN, "$withval")
	fi)
AC_ARG_WITH(log-facility-problem,
	[  --with-log-facility-problem[=<facility>]  Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.error"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_PROB, "$withval")
	fi)
AC_ARG_WITH(log-facility-error,
	[  --with-log-facility-error[=<facility>]    Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.error"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_ERROR, "$withval")
	fi)
AC_ARG_WITH(log-facility-fatal,
	[  --with-log-facility-fatal[=<facility>]    Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.error"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_FATAL, "$withval")
	fi)
AC_ARG_WITH(log-facility-panic,
	[  --with-log-facility-panic[=<facility>]    Log facility    [stderr]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval="local7.error"
		AC_DEFINE_UNQUOTED(WITH_LOG_FACILITY_PANIC, "$withval")
	fi)
dnl --------------------------------------------------------------
dnl  Tracing
dnl  The following option allow switching tracing on and off on a
dnl  per module command basis
dnl --------------------------------------------------------------
AC_ARG_WITH(trace-load,
	[  --with-trace-load=<value>   specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_LOAD, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_LOAD, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-unload,
	[  --with-trace-unload=<value> specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_UNLOAD, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_UNLOAD, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-switch,
	[  --with-trace-switch=<value> specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_SWITCH, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_SWITCH, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-disp,
	[  --with-trace-disp=<value>   specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_DISP, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_DISP, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-list,
	[  --with-trace-list=<value>   specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_LIST, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_LIST, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-avail,
	[  --with-trace-avail=<value>  specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_AVAIL, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_AVAIL, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-help,
	[  --with-trace-help=<value>   specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_HELP, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_HELP, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-init,
	[  --with-trace-init=<value>   specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_INIT, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_INIT, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-use,
	[  --with-trace-use=<value>    specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_USE, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_USE, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-unuse,
	[  --with-trace-unuse=<value>  specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_UNUSE, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_UNUSE, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-update,
	[  --with-trace-update=<value> specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_UPDATE, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_UPDATE, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-purge,
	[  --with-trace-purge=<value>  specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_PURGE, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_PURGE, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-clear,
	[  --with-trace-clear=<value>  specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_CLEAR, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_CLEAR, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-whatis,
	[  --with-trace-whatis=<value> specify a pattern for trace   [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_WHATIS, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_WHATIS, "$withval")
		fi
	fi)
AC_ARG_WITH(trace-apropos,
	[  --with-trace-apropos=<value> specify a pattern for trace  [no]],
	if test "$withval" != "no"; then
		if test "$withval" = "yes"; then
			AC_DEFINE_UNQUOTED(WITH_TRACE_APROPOS, _all_on)
		else
			AC_DEFINE_UNQUOTED(WITH_TRACE_APROPOS, "$withval")
		fi
	fi)
dnl --------------------------------------------------------------
dnl Find libraries, functions and special programs
dnl	The dynamic loader library should only be probed for, if
dnl	static linking is not switched on!
dnl --------------------------------------------------------------
AC_CHECK_LIB(m, log)
if test "${STATIC-foobar}" = "foobar"; then
    AC_CHECK_LIB(dl, dlopen)
fi

dnl
dnl--------------------------------------------------------------------
dnl Load the tclConfig.sh file
dnl--------------------------------------------------------------------

SC_PATH_TCLCONFIG
SC_LOAD_TCLCONFIG
SC_PUBLIC_TCL_HEADERS

dnl--------------------------------------------------------------------
dnl Load the tclxConfig.sh file
dnl search for tclx after tcl, not before to benefit from tcl_includes and
dnl--------------------------------------------------------------------

EM_PATH_TCLXCONFIG
EM_LOAD_TCLXCONFIG
if test x"$no_tclx" != x"true"; then
	AC_DEFINE(HAS_TCLXLIBS)
fi

dnl
dnl --------------------------------------------------------------
dnl
dnl   --with-module-path
dnl   --with-version-path
dnl   --with-autoload-path
dnl   --with-etc-path
dnl   --with-skel-path
dnl
AC_ARG_WITH(module-path,
	[  --with-module-path=<path>   use module path=<path>        [/usr/local/Modules/modulefiles]],
	MODULEPATH="$withval"
	AC_MSG_RESULT([MODULEPATH = $with_module_path])
	AC_DEFINE_UNQUOTED(MODULEPATH, "$withval")
	,
	AC_MSG_WARN([if using '--with-module-path': need to specify a path!])
	if test "$withval" = "no" || test "$withval" = "yes"; then
		withval="/usr/local/Modules/modulefiles"
	fi
	AC_MSG_WARN([will use MODULEPATH=$withval])
	MODULEPATH="$withval"
	AC_DEFINE_UNQUOTED(MODULEPATH, "$withval"))
AC_ARG_WITH(version-path,
	[  --with-version-path=<path>  use module path=<path>        [/usr/local/Modules/versions]],
	VERSIONPATH="$withval"
	AC_MSG_RESULT([VERSIONPATH = $with_version_path])
	AC_DEFINE_UNQUOTED(VERSIONPATH, "$withval")
	,
	AC_MSG_WARN([if using '--with-version-path': need to specify a path!])
	if test "$withval" = "no" || test "$withval" = "yes"; then
		withval="$prefix/Modules/versions"
	fi
	AC_MSG_WARN([will use VERSIONPATH=$withval])
	VERSIONPATH="$withval"
	AC_DEFINE_UNQUOTED(VERSIONPATH, "$withval"))
AC_ARG_WITH(autoload-path,
	[  --with-autoload-path=<path> use autoload path=<path>      [$ac_default_prefix/lib/tcl]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
		if test "$prefix" != "NONE" ; then
			withval="$prefix/lib/tcl"
		else
			withval="$ac_default_prefix/lib/tcl"
		fi
	fi
	AUTOLOADPATH="$withval"
	AC_DEFINE_UNQUOTED(AUTOLOADPATH, "$withval"))
AC_ARG_WITH(etc-path,
	[  --with-etc-path=<path>      use etc path=<path>           [/etc]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
	AC_MSG_WARN([if using '--with-etc-path': need to specify a path! Using /etc])
		withval="/etc"
	fi
	ETCPATH="$withval"
	,
	ETCPATH="/etc")
AC_ARG_WITH(skel-path,
	[  --with-skel-path=<path>     use skel path=<path>          [/etc/skel]],
	if test "$withval" = "no" || test "$withval" = "yes"; then
	AC_MSG_WARN([if using '--with-skel-path': need to specify a path! Using /etc/skel])
		withval="/etc/skel"
	fi
	SKELPATH="$withval"
	,
	SKELPATH="/etc/skel")
AC_ARG_WITH(dot-ext,
	[  --with-dot-ext[=<.ext>]     append <.ext> to shell init   [.ext]],
	if test "$withval" != "no"; then
		test "$withval" = "yes" && withval=".ext"
		AC_DEFINE_UNQUOTED(DOT_EXT, "$withval")
	else
		AC_DEFINE_UNQUOTED(DOT_EXT, "")
	fi)
dnl --------------------------------------------------------------
dnl Now put it all out!
dnl --------------------------------------------------------------
AC_CONFIG_FILES([Makefile modules.lsm .spec doc/Makefile init/Makefile
	init/bash init/csh init/ksh init/perl init/python init/sh
	init/zsh init/.modulespath
	doc/module.1 doc/modulefile.4
	etc/Makefile etc/add.modules etc/mkroot
	etc/skel/.cshrc etc/skel/.login 
	etc/skel/.kshenv etc/skel/.profile 
	etc/global/csh.cshrc etc/global/csh.login etc/global/csh.modules
	etc/global/bashrc etc/global/profile etc/global/profile.modules
	ext/Makefile ext/add.ext
	ext/common/.cshrc ext/common/.login 
	ext/common/.kshenv ext/common/.profile 
	modulefiles/Makefile modulefiles/modules
	modulefiles/module-cvs modulefiles/module-info modulefiles/null
	modulefiles/use.own modulefiles/dot modulefiles/version
	])
AC_CONFIG_COMMANDS([default],[[date > stamp-h]],[[]])
AC_OUTPUT
dnl
