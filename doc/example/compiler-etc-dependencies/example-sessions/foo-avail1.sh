#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
case $TMP_STRATEGY in
   modulepath)
	do_cmd module load intel/2019
	;;
esac
do_cmd module avail foo/2.4
