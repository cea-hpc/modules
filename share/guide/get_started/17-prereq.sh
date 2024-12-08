#!/bin/bash
set -v

sed -i "1a prereq foo" /apps/userenv/modules/applications/bar/.common
