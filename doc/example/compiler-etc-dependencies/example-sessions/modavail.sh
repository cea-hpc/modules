#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module avail
case $TMP_STRATEGY in
   modulerc)
	do_cmd module avail mvapich
	;;
   modulepath)
	do_cmd module load "$GCCGNU/9.1.0"
	do_cmd module avail
	do_cmd module load openmpi/4.0
	do_cmd module avail
	;;
esac

