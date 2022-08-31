#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
case $TMP_STRATEGY in
   modulepath)
	do_cmd module load foo
	do_cmd module load "$GCCGNU/8.2.0"
	;;
esac
do_cmd module load foo
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		#modules 3 does not set exit code correctly
		case $TMP_STRATEGY in
		   modulepath|modulerc)
			do_cmd foo
			;;
		esac
		;;
	   4.*)
		do_cmd foo
		;;
	esac
fi

do_cmd module purge
case $TMP_STRATEGY in
   modulepath)
	do_cmd module load "$GCCGNU/8.2.0"
	do_cmd module load openmpi
	do_cmd module load foo
	do_cmd module list
	do_cmd foo
	do_cmd module purge
	;;
esac
do_cmd module load foo/1.1
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		#modules 3 does not set exit code correctly
		case $TMP_STRATEGY in
		   modulerc|modulepath)
			do_cmd foo
			;;
		esac
		;;
	   4.*)
		do_cmd foo
		;;
	esac
fi

do_cmd module purge
do_cmd module load pgi/18.4
do_cmd module load foo
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		#modules 3 does not set exit code correctly
		case $TMP_STRATEGY in
		   modulerc)
			do_cmd foo
			;;
		esac
		;;
	   4.*)
		do_cmd foo
		;;
	esac
fi
