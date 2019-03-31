#
# siteconfig.tcl - Site specific configuration changing reportInfo to inhibit
#   to report info messages (like the summaries about the additional load and
#   unload triggered during a module load or unload action) depending on a
#   global variable value.
#
# Author: Wenzler
# Compatibility: Modules v4.2
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

set g_inhibit_inforeport 1 ;# Non-critical info reporting disabled if == 1

# override 'reportInfo' procedure to inhibit messages if g_inhibit_inforeport
# is set to 1
proc reportInfo {message {title INFO}} {
   if {!$::g_inhibit_inforeport} {
      # use reportError for conveniance but there is no error here
      reportError $message 0 $title 0
   }
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
