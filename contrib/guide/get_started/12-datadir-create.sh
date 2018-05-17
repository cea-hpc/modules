#!/bin/bash
set -v

mkdir -m 770 -p /store/{user1,project1,project2}

for i in user1 project1 project2; do
chgrp $i /store/$i
done
