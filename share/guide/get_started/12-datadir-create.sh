#!/bin/bash
set -v

mkdir -p /store/{user1,project1,project2}
chmod 0770 /store/{user1,project1,project2}

for i in user1 project1 project2; do
chgrp $i /store/$i
done
