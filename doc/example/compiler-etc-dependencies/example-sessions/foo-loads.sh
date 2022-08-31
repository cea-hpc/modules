#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load pgi/19.4
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
do_cmd module unload foo
do_cmd module load openmpi/3.1
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo

do_cmd module unload foo
do_cmd module unload openmpi
do_cmd module switch "$AUTOFLAG" pgi intel/2019
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
do_cmd module unload foo
case $TMP_STRATEGY in
   flavours|homebrewed|modulepath)
	do_cmd module load intelmpi
	do_cmd module load foo/2.4
	;;
   modulerc)
	do_cmd module load foo/2.4/intel/2019/nompi
	;;
esac
do_cmd module list
do_cmd foo
do_cmd module unload foo
do_cmd module switch "$AUTOFLAG" intelmpi mvapich/2.3.1
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
do_cmd module unload foo
do_cmd module switch "$AUTOFLAG" mvapich openmpi/4.0
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo

do_cmd module unload foo
do_cmd module unload openmpi
do_cmd module switch "$AUTOFLAG" intel/2019 "$GCCGNU/9.1.0"
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
do_cmd module unload foo
do_cmd module load mvapich/2.3.1
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
do_cmd module unload foo
do_cmd module switch "$AUTOFLAG" mvapich openmpi/4.0
do_cmd module load foo/2.4
do_cmd module list
do_cmd foo
