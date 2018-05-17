#!/bin/bash
set -v

for i in bar foo; do
echo 'source /apps/userenv/modules/.common' \
  >>/apps/userenv/modules/applications/$i/.common
done

cat <<EOF >/apps/userenv/modules/.common
if {[module-info mode load] || [module-info mode unload]} {
    puts stderr "[module-info mode] module [module-info name]"
}
EOF
