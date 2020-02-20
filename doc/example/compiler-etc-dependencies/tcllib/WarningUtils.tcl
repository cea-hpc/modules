# =======  Utilities for printing warning/error messages,  etc =========
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# PrintIfLoading:
#
# Prints supplied text to stderr but only if in "load" mode
proc PrintIfLoading { args } {
   if [ module-info mode load ] {
      set tmp [ join $args ]
      puts stderr "$tmp"
   }
}

#--------------------------------------------------------------------
# PrintLoadInfo:
#
# Prints supplied text to stderr as informational message, but only
# if actually trying to load the module.
proc PrintLoadInfo { args } {
   set tmp [ join $args ]
   PrintIfLoading "
\[INFO\] $tmp
"
}

#--------------------------------------------------------------------
# PrintLoadWarning:
#
# Prints supplied text to stderr as warning message, but only
# if actually trying to load the module.
proc PrintLoadWarning { args } {
   set tmp [ join $args ]
   PrintIfLoading "
WARNING:
$tmp

"
}

#--------------------------------------------------------------------
# PrintLoadError:
#
# Like PrintLoadWarning, but as error message and does a "break"
proc PrintLoadError { args } {
   set tmp [ join $args ]
   PrintIfLoading "
**** ERROR *****:
$tmp

"
   if [ module-info mode load ] {
      break
   }
}
