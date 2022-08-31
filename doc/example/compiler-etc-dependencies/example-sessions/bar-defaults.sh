#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
case $TMP_STRATEGY in
   modulepath)
	do_cmd module load "$GCCGNU/9.1.0"
	;;
esac
do_cmd module load bar
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		case $TMP_STRATEGY in
		   modulerc|modulepath)
			do_cmd bar
			;;
		esac
		;;
	   4.*)
		do_cmd bar
		;;
	esac
fi

do_cmd module purge
case $TMP_STRATEGY in
   modulepath)
	do_cmd module load "$GCCGNU/8.2.0"
	;;
esac
do_cmd module load bar/4.7
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		case $TMP_STRATEGY in
		   modulerc|modulepath)
			do_cmd bar
			;;
		esac
		;;
	   4.*)
		do_cmd bar
		;;
	esac
fi
case $TMP_STRATEGY in
   modulerc)
	do_cmd module purge
	do_cmd module load bar/avx
	do_cmd module list
	do_cmd bar
	do_cmd module purge
	do_cmd module load gcc/9.1.0
	do_cmd module load bar/avx
	do_cmd module list
	do_cmd bar
	do_cmd module purge
	do_cmd module load bar/avx2
	do_cmd module list
	do_cmd bar
	do_cmd module purge
	do_cmd module load bar/sse4.1
	do_cmd module list
	do_cmd bar
	;;
esac
