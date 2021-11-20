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
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [parseSetenvCommandArgs load set {*}$arglist] bhv var val

   # top priority asked
   if {[llength $arglist] != [llength $args]} {
      # define an helper variable to know a top-priority value has been set
      if {[currentState mode] ne "display"} {
         __setenv MODULES_PRIORITY_$var $val
      }
      __setenv {*}$arglist
   # set non-priority value only if no top priority value already set
   } elseif {![info exists ::env(MODULES_PRIORITY_$var)]} {
      __setenv {*}$arglist
   }
}

# override 'setenv-un' procedure to interpret the '--top' optional argument
# when setenv is evaluated on an unload mode
rename ::setenv-un ::__setenv-un
proc setenv-un {args} {
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [parseSetenvCommandArgs unload unset {*}$arglist] bhv var val

   # top priority asked
   if {[llength $arglist] != [llength $args]} {
      # define an helper variable to know a top-priority value has been set
      if {[currentState mode] ne "display"} {
         __setenv-un MODULES_PRIORITY_$var $val
      }
      __setenv-un {*}$arglist
   # set non-priority value only if no top priority value already set
   } elseif {![info exists ::env(MODULES_PRIORITY_$var)]} {
      __setenv-un {*}$arglist
   }
}

# override 'setenv-wh' procedure to interpret the '--top' optional argument
# when setenv is evaluated on an whatis mode
rename ::setenv-wh ::__setenv-wh
proc setenv-wh {args} {
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   __setenv-wh {*}$arglist
}

# override 'add-path' procedure to add a '--top' optional argument, which
# will benefit to the 'append-path' and 'prepend-path' modulefile commands
rename ::add-path ::__add-path
proc add-path {cmd mode dflbhv args} {
   set keep_top_priority 0
   set pos [expr {$cmd eq {append-path} ? {append} : {prepend}}]
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [parsePathCommandArgs $cmd $mode $dflbhv {*}$arglist] separator\
      allow_dup idx_val ign_refcount bhv var path_list

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
      eval __unload-path $cmd load remove $var\
         $::env(MODULES_PRIORITY_${pos}_$var)
   }

   __add-path $cmd $mode $dflbhv {*}$arglist

   if {$keep_top_priority} {
      eval __add-path $cmd $mode $dflbhv $var\
         $::env(MODULES_PRIORITY_${pos}_$var)
   }
}

rename ::unload-path ::__unload-path
proc unload-path {cmd mode dflbhv args} {
   set arglist [lsearch -all -inline -not -exact $args "--top"]
   lassign [parsePathCommandArgs $cmd $mode $dflbhv {*}$arglist] separator\
      allow_dup idx_val ign_refcount bhv var path_list

   if {[llength $arglist] != [llength $args]} {
      # wipe priority helper variable when unloading top priority value
      switch -- $cmd {
         {append-path}  { set pos append }
         {prepend-path} { set pos prepend }
      }
      if {[info exists pos]} {
         __setenv MODULES_PRIORITY_${pos}_$var $path_list
      }
   }

   __unload-path $cmd $mode $dflbhv {*}$arglist
}

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
