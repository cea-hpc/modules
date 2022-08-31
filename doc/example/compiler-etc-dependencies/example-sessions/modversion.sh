#!/bin/bash

source "$MOD_GIT_ROOTDIR"/doc/example/compiler-etc-dependencies/example-sessions/common_code.sh

do_cmd module --version
do_cmd echo "MODULEPATH=$MODULEPATH"
