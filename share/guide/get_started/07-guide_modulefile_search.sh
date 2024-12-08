#!/bin/bash
set -v

cat <<EOF >/apps/userenv/modules/applications/bar/.modulerc
#%Module
module-version /1.0 default
module-version bar/2.1 2
EOF
