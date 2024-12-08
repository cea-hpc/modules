#!/bin/bash
set -v

cat <<EOF >~user1/.modulerc
#%Module
module-alias top bar/2.1
EOF
