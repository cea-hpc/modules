#!/bin/bash
set -v

for i in user1 project1 project2; do
rm /apps/userenv/modules/environment/datadir/$i
done

cat <<EOF >/apps/userenv/modules/environment/datadir/.modulerc
#%Module
foreach grp [exec groups] {
    if {[string first proj \$grp] == 0 || \$grp eq $::env(USER)} {
        module-virtual /\$grp .common
    }
}
EOF
