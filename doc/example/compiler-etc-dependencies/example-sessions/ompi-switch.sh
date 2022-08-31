#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module purge
do_cmd module load pgi/19.4
do_cmd module load openmpi
do_cmd module list
do_cmd mpirun
do_cmd module switch "$AUTOFLAG" pgi intel/2019
do_cmd module list
do_cmd mpirun
case $TMP_STRATEGY in
   flavours)
	#Try switching to intel/2018, which does not support openmpi/4.0
	case $TMP_MODVERSION in
           3.*)
		#Only do it on 3.x, as switch fails on 4.x
		do_cmd module switch "$AUTOFLAG" intel intel/2018
		do_cmd module list
		do_cmd mpirun
		;;
	esac
	;;
   homebrewed|modulepath)
	#Try switching to intel/2018, which does not support openmpi/4.0
	case $TMP_MODVERSION in
           4.*)
		#Only do it on 4.x, as switch fails on 3.x
		do_cmd module switch "$AUTOFLAG" intel intel/2018
		do_cmd module list
		do_cmd mpirun
		;;
	esac
	;;
   modulerc)
        #We failed to reload openmpi on switch to intel/2018, no need to
	#continue
        ;;
esac

