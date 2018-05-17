#!/bin/bash
set -v

rm -rf /usr/local/Modules
rm -rf /apps/userenv
cd /root/modules
make distclean
rm -f /etc/profile.d/modules.sh
mkdir -p /apps/userenv/modules/{applications,environment}
rm -f /home/user1/.modulerc
