#!/bin/bash
set -v

mkdir /usr/local/Modules/etc

cat <<EOF >/usr/local/Modules/etc/siteconfig.tcl
rename ::cmdModuleLoad ::__cmdModuleLoad
proc cmdModuleLoad {args} {
    report "loading \$args"
    return [eval __cmdModuleLoad \$args]
}
EOF
