#!/bin/bash
set -v

mkdir /apps/userenv/modules/applications/foo

cat <<EOF >/apps/userenv/modules/applications/foo/.common
prepend-path PATH /apps/foo-\$version/bin
EOF

for i in 0.9 1.6; do
cat <<EOF >/apps/userenv/modules/applications/foo/$i
#%Module
set version "$i"
source /apps/userenv/modules/applications/foo/.common
EOF
done
