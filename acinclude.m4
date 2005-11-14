dnl
dnl File:        acinclude.m4
dnl Revision:    1.10
dnl Last Mod:    2005/11/13
dnl Created:     1994/06/19
dnl Author:      Leif Hedstrom<hedstrom@inf.ethz.ch>
dnl		 Martin S. Utesch <wstd@hl.siemens.de>
dnl 		 Peter W. Osel <pwo@guug.de>
dnl Modified:	 R.K.Owen <rk@owen.sj.ca.us>
dnl
dnl Search a set of full-path file names, and select the first one
dnl that we can locate. Works a little bit like AC_PATH_PROG, but
dnl search paths can be defined with $3.
dnl

AC_DEFUN([EM_PATH_PROG_SEARCH],
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
dnl AC_SET_STATIC(VARIABLE [, VALUE])
dnl
dnl Define a substition variable ($1) to be a reasonable option to
dnl link the binary `static'. $2 is the option as provided with
dnl the command line switch, e.g. yes, no,  "-static" etc.
dnl
AC_DEFUN([EM_SET_STATIC], [
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
dnl --------------------------------------------------------------
