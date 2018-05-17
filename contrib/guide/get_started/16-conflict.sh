#!/bin/bash
set -v

for i in bar foo; do
sed -i "1a conflict $i" /apps/userenv/modules/applications/$i/.common
done

