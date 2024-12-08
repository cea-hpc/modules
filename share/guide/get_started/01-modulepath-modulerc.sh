#!/bin/bash
set -v

cat <<EOF >>/usr/local/Modules/init/modulerc
module use /apps/userenv/modules/environment
module use /apps/userenv/modules/applications
EOF
