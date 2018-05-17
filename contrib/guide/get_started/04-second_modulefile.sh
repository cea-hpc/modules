#!/bin/bash
set -v

cat <<EOF >/apps/userenv/modules/applications/bar/.common
append-path PATH /apps/bar-\$version/bin
set-alias b /apps/bar-\$version/bin/bar
EOF

for i in 1.0 2.1; do
cat <<EOF >/apps/userenv/modules/applications/bar/$i
#%Module
set version "$i"
source /apps/userenv/modules/applications/bar/.common
EOF
done
