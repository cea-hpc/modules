#!/bin/bash
set -v

for i in user1 project1 project2; do
cat <<EOF >/apps/userenv/modules/environment/datadir/$i
#%Module
source /apps/userenv/modules/environment/datadir/.common
EOF

chmod 750 /apps/userenv/modules/environment/datadir/$i
chgrp $i /apps/userenv/modules/environment/datadir/$i

done
