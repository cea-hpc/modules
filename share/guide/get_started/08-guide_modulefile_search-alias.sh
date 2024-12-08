#!/bin/bash
set -v

cat <<EOF >>/apps/userenv/modules/applications/bar/.modulerc
module-alias bar/subdir/last foo/1.6
module-alias foo/latest foo/1.6
EOF
