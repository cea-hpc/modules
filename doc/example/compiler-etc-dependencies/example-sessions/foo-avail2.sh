#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
case $TMP_STRATEGY in
   modulerc)
	do_cmd module avail foo/gcc
	;;
   *)
	do_cmd module avail foo
	;;
esac
