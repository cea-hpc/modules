#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load "$GCCGNU/9.1.0"
case $TMP_STRATEGY in
   flavours|homebrewed)
	do_cmd module load simd/avx
	do_cmd module load bar/5.4
	;;
   modulerc)
	do_cmd module load bar/avx
	;;
   modulepath)
	do_cmd module load bar/5.4/avx
	;;
esac
do_cmd module list
do_cmd bar
case $TMP_STRATEGY in
   flavours|homebrewed)
	do_cmd module switch "$AUTOFLAG" simd simd/avx2
	;;
   modulerc)
	do_cmd module switch "$AUTOFLAG" bar bar/avx2
	;;
   modulepath)
	do_cmd module switch "$AUTOFLAG" bar bar/5.4/avx2
	;;
esac
do_cmd module list
do_cmd bar
