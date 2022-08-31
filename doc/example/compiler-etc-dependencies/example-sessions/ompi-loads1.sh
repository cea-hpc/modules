#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load pgi/19.4
do_cmd module load openmpi/4.0
do_cmd module list
do_cmd mpirun

do_cmd module unload openmpi
do_cmd module switch "$AUTOFLAG" pgi intel/2019
do_cmd module load openmpi/4.0
do_cmd module list
do_cmd mpirun

do_cmd module unload openmpi
do_cmd module switch "$AUTOFLAG" intel "$GCCGNU/9.1.0"
do_cmd module load openmpi/4.0
do_cmd mpirun

do_cmd module unload openmpi
do_cmd module switch "$AUTOFLAG" "$GCCGNU" "$GCCGNU/8.2.0"
do_cmd module load openmpi/4.0
do_cmd module list

