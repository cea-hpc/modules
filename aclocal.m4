dnl
dnl File:        aclocal.m4
dnl Revision:    1.10
dnl Last Mod:    13:53:23, 10/01/96
dnl Created:     94/06/19
dnl Author:      Leif Hedstrom<hedstrom@inf.ethz.ch>
dnl		 Martin S. Utesch <wstd@hl.siemens.de>
dnl 		 Peter W. Osel <pwo@guug.de>
dnl Modified:	 R.K.Owen <rk@owen.sj.ca.us>
dnl
dnl Search a set of full-path file names, and select the first one
dnl that we can locate. Works a little bit like AC_PATH_PROG, but
dnl search paths can be defined with $3.
dnl
builtin(include,config/tcl.m4)

AC_DEFUN(EM_PATH_PROG_SEARCH,
[# Extract the first word of "$2", so it can be a program name with args.
set dummy $2; ac_word=[$]2
AC_MSG_CHECKING([for $ac_word])
AC_CACHE_VAL(ac_cv_path_$1,
[case "[$]$1" in
  /*)
  ac_cv_path_$1="[$]$1" # Let the user override the test with a path.
  ;;
  *)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in $3; do
    test -z "$ac_dir" && ac_dir=.
    if test -f $ac_dir/$ac_word; then
      ac_cv_path_$1="$ac_dir/$ac_word"
      break
    fi
  done
  IFS="$ac_save_ifs"
dnl If no 4th arg is given, leave the cache variable unset,
dnl so AC_PATH_PROG_SEARCH will keep looking.
  ifelse([$4], , , [  test -z "[$]ac_cv_path_$1" && ac_cv_path_$1="$4"
  ])dnl
  ;;
esac])dnl
$1="$ac_cv_path_$1"
if test -n "[$]$1"; then
  AC_MSG_RESULT([$]$1)
else
  AC_MSG_RESULT(no)
fi
AC_SUBST($1)dnl
])
dnl --------------------------------------------------------------
dnl EM_FIND_TCLX
dnl
dnl Find extended TCL (tclX) libraries and include file.  If they
dnl are in $tcl_includes and $tcl_libraries we don't set anything.
dnl Thus run this after EM_FIND_TCL, not before.
dnl
AC_DEFUN(EM_FIND_TCLX, [
AC_MSG_CHECKING([for tclX (extended TCL)])
AC_REQUIRE([SC_LOAD_TCLCONFIG])dnl
AC_ARG_WITH(tclx-include,
	[  --with-tclx-include         define TCLX include path],
	tclx_includes=$withval)
if test ! -n "$tclx_includes"; then
    for dir in ../tcl* ./tcl* ../tcl*/include ./tcl*/include ../include \
	    /usr/include /usr/include/tcl /usr/local/include \
	    /usr/gnu/include /opt/include /opt/local/include /opt/gnu/include \
	    /local/include /opt/local/gnu/include /usr/local/gnu/include;
    do
       if test -r "$dir/tclExtend.h"; then
	  tclx_includes=$dir
	  break
       fi
    done
fi

if test -z "$tclx_includes"; then
   AC_MSG_WARN(can't find tclX (extended TCL) include files)
fi

AC_ARG_WITH(tclx-libraries,
	[  --with-tclx-libraries       define TCLX include path],
	tclx_libraries=$withval)
if test ! -n "$tclx_libraries"; then
    for dir in `echo "$tclx_includes" | sed s/include/lib/` \
	    ../tcl* ./tcl* ../tcl*/lib ./tcl*/lib ../lib \
	    /usr/lib /usr/local/lib /usr/gnu/lib /usr/local/gnu/lib /local/lib \
	    /opt/lib /opt/local/lib /opt/gnu/lib /opt/local/gnu/lib;
    do
       if test -r "$dir/libtclx.a" ; then
	  tclx_libraries=$dir
	  break
       fi
    done
fi

test -z "$tclx_libraries" && AC_MSG_WARN(can't find extended TCL (tclX) library file)
msg_tclx_libs=$tclx_libraries
msg_tclx_incs=$tcl_includes
if test "$tclx_includes" = "$tcl_includes"; then
   tclx_includes=""
   msg_tclx_incs="same directory $tcl_includes as tcl"
fi
if test "$tclx_libraries" = "$tcl_libraries"; then
   msg_tclx_libs="same directory $tcl_libraries as tcl"
   tclx_libraries=""
fi
AC_CACHE_VAL(ac_cv_path_tclx,
[#cache values $tclx_includes and $tclx_libraries
ac_cv_path_tclx="ac_tclx_includes=$tclx_includes ac_tclx_libraries=$tclx_libraries"
])dnl
AC_MSG_RESULT([libraries $msg_tclx_libs; headers $msg_tclx_incs])
])dnl
dnl --------------------------------------------------------------
dnl AC_MAKE_INCLUDE(INCLUDE, INCLUDE_DIR)
dnl
dnl Avoid /usr in -I, and generate a -I<directory> if needed
dnl
AC_DEFUN(EM_MAKE_INCLUDE, [
if test "[$]$1" != "/usr/include" -a -n "[$]$1"; then
    $2=-I[$]$1
fi])dnl
dnl
dnl --------------------------------------------------------------
dnl AC_MAKE_LIBRARY(LIBRARY, LIBRARY_DIR)
dnl
dnl Avoid /usr in -L, and generate a -L<directory> if needed.
dnl IF we are on Solaris2, we will also add a -R<directory>.
dnl I tried to use the `case' structure here, but it wouldn't
dnl work well when called within another macro call...
dnl
AC_DEFUN(EM_MAKE_LIBRARY, [
if test "[$]$1" != "/usr/lib" -a -n "[$]$1"; then
    $2=-L[$]$1
    if test "$UNAME" != ""; then
        os_sys_rel=`$UNAME -rs`
dnl         echo "OS is '$os_sys_rel'"
        case "$os_sys_rel" in
        [SunOS*5*|HP-UX*)] 
            if test "$R_OPTION" = "" ; then
                case "$os_sys_rel" in
                [SunOS*)] R_OPTION="-R" ;;
                [HP-UX*)] R_OPTION="-Xlinker +b -Xlinker " ;; 
                esac
             else
                 R_OPTION="${R_OPTION}:"
             fi
             R_OPTION="${R_OPTION}[$]$1" 
             ;;
         [Linux*)] R_OPTION="${R_OPTION} -Xlinker -rpath -Xlinker [$]$1" ;;
         esac
        AC_MSG_RESULT(R_OPTION set to $R_OPTION for $os_sys_rel)
    fi
fi] )
dnl
dnl --------------------------------------------------------------
dnl AC_SET_STATIC(VARIABLE [, VALUE])
dnl
dnl Define a substition variable ($1) to be a reasonable option to
dnl link the binary `static'. $2 is the option as provided with
dnl the command line switch, e.g. yes, no,  "-static" etc.
dnl
AC_DEFUN(EM_SET_STATIC, [
AC_MSG_CHECKING([for static linking flag])
if test "$2" = "yes"; then
    if test "$CC" = "gcc"; then
        $1="-static"
	AC_MSG_RESULT([setting $1 to '-static'])
    elif test "$UNAME" != ""; then
        os_system=`$UNAME -s`
        if test "$os_system" = "IRIX"; then
            $1="-non_shared"
	    AC_MSG_RESULT([setting $1 to '-non_shared'])
        else
            $1="-Bstatic"
	    AC_MSG_RESULT([setting $1 to '-Bstatic'])
        fi
    else
        $1="-Bstatic"
	AC_MSG_RESULT([setting $1 to '-Bstatic'])
    fi
elif test "$2" != "no"; then
    AC_MSG_RESULT([setting $1 to '$2'])
    $1=$2
else
    test "$2" != "no" && $1=$2
    AC_MSG_RESULT(no)
fi])dnl
dnl
dnl --------------------------------------------------------------
dnl AC_PROG_TOUCH(VARIABLE) -- not used
dnl
dnl Checks, how 'touch' has to be called in order to change file
dnl modification dates
dnl VARIABLE is set to the correct invocation of 'touch'
dnl
AC_DEFUN(EM_PROG_TOUCH, [
AC_MSG_CHECKING([how to call 'touch'])
    cwd=`pwd`
    cd /tmp
    $1="echo Don't know how to touch"
    for cmd in "/usr/bin/touch" "/usr/bin/touch -t" "/usr/5bin/touch"
    do
       $cmd 0101120096 mist
       if test -f mist -a ! -f 0101120096; then
	  $1=$cmd
	  break
       fi
       /bin/rm -f mist 0101120096
    done
    if test -f mist; then
       /bin/rm -f mist
    fi
    cd $cwd
AC_MSG_RESULT([$cmd])
])dnl
