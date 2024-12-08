#!/bin/bash
set -v

echo "setenv MODULES_COLLECTION_TARGET [uname nodename]" \
    >>/usr/local/Modules/init/modulerc
