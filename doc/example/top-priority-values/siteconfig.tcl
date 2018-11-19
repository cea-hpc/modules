#
# siteconfig.tcl - Site specific configuration introducing a '--top' argument
#   for the setenv, append-path and prepend-path modulefile commands. '--top'
#   argument enables a value set with this flag on to hold top priority and
#   thus cannot be altered unless by another top priority value. For
#   'append-path' command, '--top' ensure value will stay at last position in
#   path-like variable. Same goes for 'prepend-path' where '--top' ensure
#   first position in path-like variable.
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

# override 'setenv' procedure to add a '--top' optional argument
rename ::setenv ::__setenv
proc setenv {args} {
   set topPriority 0
   set errArgMsg "wrong # args: should be \"setenv ?--top? var val\""
   switch -- [llength $args] {
      {3} {
         if {[lindex $args 0] eq "--top"} {
            set topPriority 1
         } else {
            error $errArgMsg
         }
         set var [lindex $args 1]
         set val [lindex $args 2]
      }
      {2} {
         set var [lindex $args 0]
         set val [lindex $args 1]
      }
      default {
         error $errArgMsg
      }
   }

   if {$topPriority} {
      # define an helper variable to know a top-priority value has been set
      if {[currentMode] ne "display"} {
         __setenv MODULES_PRIORITY_$var $val
      }
      __setenv $var $val
   # set non-priority value only if no top priority value already set
   } elseif {![info exists ::env(MODULES_PRIORITY_$var)]} {
      __setenv $var $val
   }
}

# override 'setenv-un' procedure to interpret the '--top' optional argument
# when setenv is evaluated on an unload mode
rename ::setenv-un ::__setenv-un
proc setenv-un {args} {
   set topPriority 0
   set errArgMsg "wrong # args: should be \"setenv-un ?--top? var val\""
   switch -- [llength $args] {
      {3} {
         if {[lindex $args 0] eq "--top"} {
            set topPriority 1
         } else {
            error $errArgMsg
         }
         set var [lindex $args 1]
         set val [lindex $args 2]
      }
      {2} {
         set var [lindex $args 0]
         set val [lindex $args 1]
      }
      default {
         error $errArgMsg
      }
   }

   if {$topPriority} {
      # define an helper variable to know a top-priority value has been set
      if {[currentMode] ne "display"} {
         __setenv-un MODULES_PRIORITY_$var $val
      }
      __setenv-un $var $val
   # set non-priority value only if no top priority value already set
   } elseif {![info exists ::env(MODULES_PRIORITY_$var)]} {
      __setenv-un $var $val
   }
}

# override 'add-path' procedure to add a '--top' optional argument, which
# will benefit to the 'append-path' and 'prepend-path' modulefile commands
rename ::add-path ::__add-path
proc add-path {pos args} {
   set keep_top_priority 0
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [eval parsePathCommandArgs "add-path" $arglist] separator\
      allow_dup idx_val var path_list

   # top priority asked
   if {[llength $arglist] != [llength $args]} {
      # record this top priority value in an helper variable
      __setenv MODULES_PRIORITY_${pos}_$var $path_list
   } elseif {[info exists ::env(MODULES_PRIORITY_${pos}_$var)]} {
      set keep_top_priority 1
   }

   # ensure top-priority value keeps first or last position by unloading it
   # priority new value addition, then restoring it
   if {$keep_top_priority} {
      eval __unload-path $var $::env(MODULES_PRIORITY_${pos}_$var)
   }

   eval __add-path $pos $arglist

   if {$keep_top_priority} {
      eval __add-path $pos $var $::env(MODULES_PRIORITY_${pos}_$var)
   }
}

rename ::unload-path ::__unload-path
proc unload-path {args} {
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [eval parsePathCommandArgs "unload-path" $arglist] separator\
      allow_dup idx_val var path_list

   if {[llength $arglist] != [llength $args]} {
      # wipe priority helper variable when unloading top priority value
      switch -- [lindex [info level -1] 0] {
         {append-path}  { set pos "append" }
         {prepend-path} { set pos "prepend" }
      }
      if {[info exists pos]} {
         __setenv MODULES_PRIORITY_${pos}_$var $path_list
      }
   }

   eval __unload-path $arglist
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
