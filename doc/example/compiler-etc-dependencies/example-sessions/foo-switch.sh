#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load pgi/18.4
do_cmd module load openmpi/3.1
do_cmd module load foo/1.1
do_cmd module list
do_cmd foo
do_cmd module switch "$AUTOFLAG" pgi intel/2018
do_cmd module list
do_cmd foo
do_cmd mpirun
do_cmd module purge
do_cmd module load intel/2019
do_cmd module load foo
do_cmd module list
do_cmd foo
do_cmd module load "$AUTOFLAG" openmpi
do_cmd module list
do_cmd foo
