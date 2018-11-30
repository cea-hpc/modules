#!/bin/bash
set -v

mkdir -p /apps/userenv/modules/applications/bar

cat <<EOF >/apps/userenv/modules/applications/bar/1.0
#%Module
append-path PATH /apps/bar-1.0/bin
set-alias b /apps/bar-1.0/bin/bar
EOF
