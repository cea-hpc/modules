#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load openmpi/3.1
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		case $TMP_STRATEGY in
		   modulerc|modulepath)
			do_cmd mpirun
			;;
		esac
		;;
	   4.*)
		do_cmd mpirun
		;;
	esac
fi
do_cmd module purge
do_cmd module load "$GCCGNU/8.2.0"
do_cmd module load openmpi
err=$?
do_cmd module list
if [ $err -eq 0 ]; then
	case $TMP_MODVERSION in
	   3.*)
		case $TMP_STRATEGY in
		   modulerc|modulepath)
			do_cmd mpirun
			;;
		esac
		;;
	   4.*)
		do_cmd mpirun
		;;
	esac
fi
