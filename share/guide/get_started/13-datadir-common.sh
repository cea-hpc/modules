#!/bin/bash
set -v

mkdir -p /apps/userenv/modules/environment/datadir
cat <<EOF >/apps/userenv/modules/environment/datadir/.common
#%Module
setenv STOREDIR /store/[file tail [module-info name]]
EOF
