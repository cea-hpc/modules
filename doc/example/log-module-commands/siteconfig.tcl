#
# siteconfig.tcl - Site specific configuration script tracking the modulefile
#   evaluations to generate a log entry each time a module is loaded or
#   unloaded with an information to know if module is auto-loaded or not.
#
# Author: Xavier Delaruelle <xavier.delaruelle@cea.fr>
# Compatibility: Modules v4.2+
#
# Installation: put this file in the 'etc' directory of your Modules
#   installation. Refer to the "Modulecmd startup" section in the
#   module(1) man page to get this location.

proc execLogger {msg} {
   # ensure logger command is executed with system libs
   if {[info exists ::env(LD_LIBRARY_PATH)]} {
      set ORIG_LD_LIBRARY_PATH $::env(LD_LIBRARY_PATH)
      unset ::env(LD_LIBRARY_PATH)
   }
   exec logger -t module $msg
   # restore current user lib setup
   if {[info exists ORIG_LD_LIBRARY_PATH]} {
      set ::env(LD_LIBRARY_PATH) $ORIG_LD_LIBRARY_PATH
   }
}

proc logModfileInterp {cmdstring code result op} {
   # parse context
   lassign $cmdstring cmdname modfile modname
   set mode [currentState mode]
   if {[info level] > 1} {
      set caller [lindex [info level -1] 0]
   } else {
      set caller {}
   }

   # only log load and unload modulefile evaluation
   if {$mode in {load unload}} {

      # add info on load mode to know if module is auto-loaded or not
      if {$mode eq {load} && $caller eq {cmdModuleLoad}} {
         upvar 1 uasked uasked
         set extra ", \"auto\": [expr {$uasked ? {false} : {true}}]"
      } else {
         set extra {}
      }

      # produced log entry (formatted as a JSON record)
      execLogger "{ \"user\": \"[get-env USER]\", \"mode\": \"$mode\",\
         \"module\": \"$modname\"${extra} }"
   }
}

# run log procedure after each modulefile interpretation call
trace add execution execute-modulefile leave logModfileInterp

proc logModuleCmd {cmdstring code result op} {
   # parse context
   set args [lassign $cmdstring cmdname]
   if {[info level] > 1} {
      set caller [lindex [info level -1] 0]
   } else {
      set caller {}
   }

   # skip duplicate log entry when ml command calls module
   if {$cmdname ne {module} || $caller ne {ml}} {

      # produced log entry (formatted as a JSON record)
      execLogger "{ \"user\": \"[get-env USER]\", \"cmd\": \"$cmdname\",\
         \"args\": \"$args\" }"
   }
}

# run log procedure after each module or ml command
trace add execution module leave logModuleCmd
trace add execution ml leave logModuleCmd

# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
