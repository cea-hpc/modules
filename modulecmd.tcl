#!/bin/sh
# \
tclshbin=$(type -p tclsh) && exec $tclshbin "$0" "$@"
# \
[ -x /usr/local/bin/tclsh ] && exec /usr/local/bin/tclsh "$0" "$@"
# \
[ -x /usr/bin/tclsh ] && exec /usr/bin/tclsh "$0" "$@"
# \
[ -x /bin/tclsh ] && exec /bin/tclsh "$0" "$@"
# \
echo "FATAL: module: Could not find tclsh in \$PATH or in standard directories" >&2; exit 1

# MODULECMD.TCL, a pure TCL implementation of the module command
# Copyright (C) 2002-2004 Mark Lakata
# Copyright (C) 2004-2017 Kent Mein
# Copyright (C) 2016-2017 Xavier Delaruelle
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

##########################################################################

#
# Some Global Variables.....
#
set MODULES_CURRENT_VERSION 1.890
set MODULES_CURRENT_RELEASE_DATE "2017-06-21"
set g_debug 0 ;# Set to 1 to enable debugging
set error_count 0 ;# Start with 0 errors
set g_autoInit 0
set g_inhibit_interp 0 ;# Modulefile interpretation disabled if == 1
set g_inhibit_errreport 0 ;# Non-critical error reporting disabled if == 1
set g_inhibit_dispreport 0 ;# Display-mode reporting disabled if == 1
set g_force 0 ;# Path element reference counting if == 0
set CSH_LIMIT 4000 ;# Workaround for commandline limits in csh
set flag_default_dir 1 ;# Report default directories
set flag_default_mf 1 ;# Report default modulefiles and version alias

# Used to tell if a machine is running Windows or not
proc isWin {} {
   global tcl_platform

   if { $tcl_platform(platform) eq "windows" } {
      return 1
   } else {
      return 0
   }
}

#
# Set Default Path separator
#
if { [isWin] } {
   set g_def_separator "\;"
} else {
   set g_def_separator ":"
}

# Dynamic columns
set DEF_COLUMNS 80 ;# Default size of columns for formatting
if {[catch {exec stty size} stty_size] == 0 && $stty_size ne ""} {
   set DEF_COLUMNS [lindex $stty_size 1]
}

# Use MODULECONTACT variable to set your support email address
if {[info exists env(MODULECONTACT)]} {
   set contact $env(MODULECONTACT)
} else {
   # Or change this to your support email address...
   set contact "root@localhost"
}

# Set some directories to ignore when looking for modules.
set ignoreDir(CVS) 1
set ignoreDir(RCS) 1
set ignoreDir(SCCS) 1
set ignoreDir(.svn) 1
set ignoreDir(.git) 1

global g_shellType
global g_shell

set show_oneperline 0 ;# Gets set if you do module list/avail -t
set show_modtimes 0 ;# Gets set if you do module list/avail -l
set show_filter "" ;# Gets set if you do module avail -d or -L

proc raiseErrorCount {} {
   global error_count
   incr error_count
}

proc renderError {} {
   global g_shellType error_count

   reportDebug "Error: $error_count error(s) detected."

   if {[info exists g_shellType]} {
      switch -- $g_shellType {
         csh {
            puts stdout "/bin/false;"
         }
         sh {
            puts stdout "/bin/false;"
         }
         fish {
            puts stdout "/bin/false;"
         }
         tcl {
            puts stdout "exec /bin/false;"
         }
         cmd {
            # nothing needed, reserved for future cygwin, MKS, etc
         }
         perl {
            puts stdout "die \"modulefile.tcl: $error_count error(s)\
               detected!\\n\""
         }
         python {
            puts stdout "raise RuntimeError(\
               'modulefile.tcl: $error_count error(s) detected!')"
         }
         lisp {
            puts stdout "(error \"modulefile.tcl:\
               $error_count error(s) detected!\")"
         }
      }
   }
}

#
# Debug, Info, Warnings and Error message handling.
#
proc reportDebug {message {nonewline ""}} {
   global g_debug

   if {$g_debug} {
      report "DEBUG $message" "$nonewline"
   }
}

proc reportWarning {message {nonewline ""}} {
   global g_inhibit_errreport

   if {!$g_inhibit_errreport} {
      report "WARNING: $message" "$nonewline"
   }
}

proc reportError {message {nonewline ""}} {
   global g_inhibit_errreport

   # if report disabled, also disable error raise to get a coherent
   # behavior (if no message printed, no error code change)
   if {!$g_inhibit_errreport} {
      raiseErrorCount
      report "ERROR: $message" "$nonewline"
   }
}

proc reportErrorAndExit {message} {
   raiseErrorCount
   renderError
   error "$message"
}

proc reportInternalBug {message} {
   global contact g_inhibit_errreport

   # if report disabled, also disable error raise to get a coherent
   # behavior (if no message printed, no error code change)
   if {!$g_inhibit_errreport} {
      raiseErrorCount
      report "Module ERROR: $message\nPlease contact: $contact"
   }
}

proc report {message {nonewline ""}} {
   if {$nonewline ne ""} {
      puts -nonewline stderr "$message"
   } else {
      puts stderr "$message"
   }
}

# raise error to top level, but no message as information have already
# been printed by a previous report* call
proc exitOnError {} {
   renderError
   error ""
}

# disable error reporting (non-critical report only) unless debug enabled
proc inhibitErrorReport {} {
   global g_inhibit_errreport g_debug

   if {!$g_debug} {
      set g_inhibit_errreport 1
   }
}

proc reenableErrorReport {} {
   global g_inhibit_errreport

   set g_inhibit_errreport 0
}

########################################################################
# Use a slave TCL interpreter to execute modulefiles
#

proc unset-env {var} {
   global env

   if {[info exists env($var)]} {
      reportDebug "unset-env:  $var"
      unset env($var)
   }
}

proc execute-modulefile {modfile {exit_on_error 1} {must_have_cookie 1}} {
   global g_debug g_inhibit_interp g_inhibit_errreport g_inhibit_dispreport
   global ModulesCurrentModulefile
   global g_modfileUntrackVars g_modfileAliases

   set ModulesCurrentModulefile $modfile

   # skip modulefile if interpretation has been inhibited
   if {$g_inhibit_interp} {
      reportDebug "execute-modulefile: Skipping $modfile"
      return 1
   }

   reportDebug "execute-modulefile:  Starting $modfile"
   # create modulefile interpreter at first interpretation
   if {![interp exists __modfile]} {
      interp create __modfile

      # list variable that should not be tracked for saving
      array set g_modfileUntrackVars [list g_debug 1 g_inhibit_interp 1\
         g_inhibit_errreport 1 g_inhibit_dispreport 1\
         ModulesCurrentModulefile 1 must_have_cookie 1 modcontent 1 env 1]

      # list interpreter alias commands to define
      array set g_modfileAliases [list setenv setenv unsetenv unsetenv getenv\
         getenv system system chdir chdir append-path append-path\
         prepend-path prepend-path remove-path remove-path prereq prereq\
         conflict conflict is-loaded is-loaded module module module-info\
         module-info module-whatis module-whatis set-alias set-alias\
         unset-alias unset-alias uname uname x-resource x-resource exit\
         exitModfileCmd module-version module-version module-alias\
         module-alias module-trace module-trace module-verbosity\
         module-verbosity module-user module-user module-log module-log\
         reportInternalBug reportInternalBug reportWarning reportWarning\
         reportError reportError raiseErrorCount raiseErrorCount report\
         report isWin isWin readModuleContent readModuleContent]

      # dump initial interpreter state to restore it before each modulefile
      # interpreation
      dumpInterpState __modfile g_modfileVars g_modfileArrayVars\
         g_modfileUntrackVars g_modfileProcs
   }

   # reset interp state command before each interpretation
   resetInterpState __modfile g_modfileVars g_modfileArrayVars\
      g_modfileUntrackVars g_modfileProcs g_modfileAliases g_modfileCommands

   # reset modulefile-specific variable before each interpretation
   interp eval __modfile {global ModulesCurrentModulefile g_debug\
      g_inhibit_interp g_inhibit_errreport g_inhibit_dispreport}
   interp eval __modfile set ModulesCurrentModulefile $modfile
   interp eval __modfile set g_debug $g_debug
   interp eval __modfile set g_inhibit_interp $g_inhibit_interp
   interp eval __modfile set g_inhibit_errreport $g_inhibit_errreport
   interp eval __modfile set g_inhibit_dispreport $g_inhibit_dispreport
   interp eval __modfile set must_have_cookie $must_have_cookie

   set errorVal [interp eval __modfile {
      set modcontent [readModuleContent $ModulesCurrentModulefile 1\
         $must_have_cookie]
      if {$modcontent eq ""} {
         # exit after end of slave evaluation
         return 2
      }
      info script $ModulesCurrentModulefile
      # eval then call for specific proc depending mode under same catch
      set sourceFailed [catch {
         eval $modcontent
         switch -- [module-info mode] {
            {help} {
               if {[info procs "ModulesHelp"] eq "ModulesHelp"} {
                  ModulesHelp
               } else {
                  reportWarning "Unable to find ModulesHelp in\
                     $ModulesCurrentModulefile."
               }
            }
            {display} {
               if {[info procs "ModulesDisplay"] eq "ModulesDisplay"} {
                  ModulesDisplay
               }
            }
            {test} {
               if {[info procs "ModulesTest"] eq "ModulesTest"} {
                  if {[string is true -strict [ModulesTest]]} {
                     report "Test result: PASS"
                  } else {
                     report "Test result: FAIL"
                     raiseErrorCount
                  }
               } else {
                  reportWarning "Unable to find ModulesTest in\
                     $ModulesCurrentModulefile."
               }
            }
         }
      } errorMsg]
      if {$sourceFailed} {
         global errorInfo
         # no error in case of "continue" command
         # catch continue even if called outside of a loop
         if {$errorMsg eq "invoked \"continue\" outside of a loop"\
            || $sourceFailed == 4} {
            unset errorMsg
            return 0
         # catch break even if called outside of a loop
         } elseif {$errorMsg eq "invoked \"break\" outside of a loop"\
            || ($errorMsg eq "" && (![info exists errorInfo]\
            || $errorInfo eq ""))} {
            raiseErrorCount
            unset errorMsg
            return 1
         }\
         elseif [regexp "^WARNING" $errorMsg] {
            raiseErrorCount
            report $errorMsg
            return 1
         } else {
            reportInternalBug "Occurred in file\
               $ModulesCurrentModulefile:$errorInfo"
            # return a specific value to provoke an exit after end
            # of slave evaluation since here 'exit' procedure has
            # been superseeded for modulefile interpretation
            return 2
         }
      } else {
         unset errorMsg
         return 0
      }
   }]

   reportDebug "Exiting $modfile"

   # exits rather returns if a critical error has been raised
   # and if the exit_on_error behavior is set
   if {$errorVal == 2 && $exit_on_error} {
      exitOnError
   } else {
      return $errorVal
   }
}

# Smaller subset than main module load... This function runs modulerc and
# .version files
proc execute-modulerc {modfile {exit_on_error 1}} {
   global g_rcfilesSourced ModulesVersion
   global g_debug g_inhibit_errreport g_inhibit_dispreport
   global ModulesCurrentModulefile
   global g_modrcUntrackVars g_modrcAliases

   reportDebug "execute-modulerc: $modfile"

   set ModulesCurrentModulefile $modfile
   set ModulesVersion {}
   # does not report commands from rc file on display mode
   set g_inhibit_dispreport 1

   set modname [file dirname [currentModuleName]]

   if {![info exists g_rcfilesSourced($modfile)]} {
      reportDebug "execute-modulerc: sourcing rc $modfile"
      # create modulerc interpreter at first interpretation
      if {![interp exists __modrc]} {
         interp create __modrc

         # list variable that should not be tracked for saving
         array set g_modrcUntrackVars [list g_debug 1 g_inhibit_errreport 1\
            g_inhibit_dispreport 1 ModulesCurrentModulefile 1\
            ModulesVersion 1 modcontent 1 env 1]

         # list interpreter alias commands to define
         array set g_modrcAliases [list uname uname system system chdir\
            chdir module-version module-version module-alias module-alias\
            module module module-info module-info module-trace module-trace\
            module-verbosity module-verbosity module-user module-user\
            module-log module-log reportInternalBug reportInternalBug\
            setModulesVersion setModulesVersion readModuleContent\
            readModuleContent]

         # dump initial interpreter state to restore it before each modulerc
         # interpreation
         dumpInterpState __modrc g_modrcVars g_modrcArrayVars\
            g_modrcUntrackVars g_modrcProcs
      }

      # reset interp state command before each interpretation
      resetInterpState __modrc g_modrcVars g_modrcArrayVars\
         g_modrcUntrackVars g_modrcProcs g_modrcAliases g_modrcCommands

      interp eval __modrc {global ModulesCurrentModulefile g_debug\
         g_inhibit_errreport g_inhibit_dispreport ModulesVersion}
      interp eval __modrc set ModulesCurrentModulefile $modfile
      interp eval __modrc set g_debug $g_debug
      interp eval __modrc set g_inhibit_errreport $g_inhibit_errreport
      interp eval __modrc set g_inhibit_dispreport $g_inhibit_dispreport
      interp eval __modrc {set ModulesVersion {}}

      set errorVal [interp eval __modrc {
         set modcontent [readModuleContent $ModulesCurrentModulefile]
         if {$modcontent eq ""} {
            # simply skip rc file, no exit on error here
            return 1
         }
         info script $ModulesCurrentModulefile
         if [catch {eval $modcontent} errorMsg] {
            global errorInfo

            reportInternalBug "Occurred in file\
               $ModulesCurrentModulefile:$errorInfo"
            # return a specific value to provoke an exit after end
            # of slave evaluation
            return 2
         } else {
            # pass ModulesVersion value to master interp
            if {[info exists ModulesVersion]} {
               setModulesVersion $ModulesVersion
            }
            return 0
         }
      }]

      # default version set via ModulesVersion variable in .version file
      # override previously defined default version for modname
      if {[file tail $modfile] eq ".version" && $ModulesVersion ne ""} {
         reportDebug "execute-version: default $modname =\
            $modname/$ModulesVersion"
         setModuleResolution $modname $modname/$ModulesVersion "default" 1
      }

      # Keep track of rc files we already sourced so we don't run them again
      set g_rcfilesSourced($modfile) $ModulesVersion

      # exits rather returns if a critical error has been raised
      # and if the exit_on_error behavior is set
      if {$errorVal == 2 && $exit_on_error} {
         exitOnError
      }
   }

   # re-enable command report on display mode
   set g_inhibit_dispreport 0

   return $g_rcfilesSourced($modfile)
}

# Save list of the defined procedure and the global variables with their
# associated values set in slave interpreter passed as argument. Global
# structures are used to save these information and the name of these
# structures are provided as argument.
proc dumpInterpState {itrp dumpVarsVN dumpArrayVarsVN untrackVarsVN\
   dumpProcsVN} {
   upvar #0 $dumpVarsVN dumpVars
   upvar #0 $dumpArrayVarsVN dumpArrayVars
   upvar #0 $untrackVarsVN untrackVars
   upvar #0 $dumpProcsVN dumpProcs

   # save name and value for any other global variables
   foreach var [$itrp eval {info globals}] {
      if {![info exists untrackVars($var)]} {
         reportDebug "dumpInterpState: saving for $itrp var $var"
         if {[$itrp eval array exists ::$var]} {
            set dumpVars($var) [$itrp eval array get ::$var]
            set dumpArrayVars($var) 1
         } else {
            set dumpVars($var) [$itrp eval set ::$var]
         }
      }
   }

   # save name of every defined procedures
   foreach var [$itrp eval {info procs}] {
      set dumpProcs($var) 1
   }
   reportDebug "dumpInterpState: saving for $itrp proc list [array names\
      dumpProcs]"
}

# Restore initial setup of slave interpreter passed as argument based on
# global structure previously filled with initial list of defined procedure
# and values of global variable.
proc resetInterpState {itrp dumpVarsVN dumpArrayVarsVN untrackVarsVN\
   dumpProcsVN aliasesVN dumpCommandsVN} {
   upvar #0 $dumpVarsVN dumpVars
   upvar #0 $dumpArrayVarsVN dumpArrayVars
   upvar #0 $untrackVarsVN untrackVars
   upvar #0 $dumpProcsVN dumpProcs
   upvar #0 $aliasesVN aliases
   upvar #0 $dumpCommandsVN dumpCommands

   # look at list of defined procedures and delete those not part of the
   # initial state list. do not check if they have been altered as no vital
   # procedures lied there. note that if a Tcl command has been overridden
   # by a proc, it will be removed here and command will also disappear
   foreach var [$itrp eval {info procs}] {
      if {![info exists dumpProcs($var)]} {
         reportDebug "resetInterpState: removing on $itrp proc $var"
         $itrp eval [list rename $var {}]
      }
   }

   # set interpreter alias commands each time to guaranty them being
   # defined and not overridden by modulefile or modulerc content
   foreach alias [array names aliases] {
      interp alias $itrp $alias {} $aliases($alias)
   }

   # dump interpreter command list here on first time as aliases should be
   # set prior to be found on this list for correct match
   if {![info exists dumpCommands]} {
      set dumpCommands [$itrp eval {info commands}]
      reportDebug "resetInterpState: saving for $itrp command list\
         $dumpCommands"
   # if current interpreter command list does not match initial list it
   # means that at least one command has been altered so we need to recreate
   # interpreter to guaranty proper functioning
   } elseif {$dumpCommands ne [$itrp eval {info commands}]} {
      reportDebug "resetInterpState: missing command(s), recreating $itrp"
      interp delete $itrp
      interp create $itrp
      # set aliases again on fresh interpreter
      foreach alias [array names aliases] {
         interp alias $itrp $alias {} $aliases($alias)
      }
   }

   # check every global variables currently set and correct them to restore
   # initial interpreter state. work on variables at the very end to ensure
   # procedures and commands are correctly defined
   foreach var [$itrp eval {info globals}] {
      if {![info exists untrackVars($var)]} {
         if {![info exists dumpVars($var)]} {
            reportDebug "resetInterpState: removing on $itrp var $var"
            $itrp eval unset ::$var
         } elseif {![info exists dumpArrayVars($var)]} {
            if {$dumpVars($var) ne [$itrp eval set ::$var]} {
               reportDebug "resetInterpState: restoring on $itrp var $var"
               $itrp eval set ::$var $dumpVars($var)
            }
         } else {
            if {$dumpVars($var) ne [$itrp eval array get ::$var]} {
               reportDebug "resetInterpState: restoring on $itrp var $var"
               $itrp eval array set ::$var [list $dumpVars($var)]
            }
         }
      }
   }
}

########################################################################
# commands run from inside a module file
#
set ModulesCurrentModulefile {}

# Dummy procedures for commands available on C-version but not
# implemented here. These dummy procedures enables support for
# modulefiles using these commands while warning users these
# commands have no effect.
proc module-log {args} {
   reportWarning "'module-log' command not implemented"
}

proc module-verbosity {args} {
   reportWarning "'module-verbosity' command not implemented"
}

proc module-user {args} {
   reportWarning "'module-user' command not implemented"
}

proc module-trace {args} {
   reportWarning "'module-trace' command not implemented"
}

proc module-info {what {more {}}} {
   global g_shellType g_shell tcl_platform

   set mode [currentMode]

   reportDebug "module-info: $what $more  mode=$mode"

   switch -- $what {
      "mode" {
         if {$more ne ""} {
            set command [currentCommandName]
            if {$mode eq $more || ($more eq "remove" && $mode eq "unload")\
               || ($more eq "switch" && $command eq "switch")} {
               return 1
            } else {
               return 0
            }
         } else {
            return $mode
         }
      }
      "command" {
         set command [currentCommandName]
         if {$more eq ""} {
            return $command
         } elseif {$command eq $more} {
            return 1
         } else {
            return 0
         }
      }
      "name" {
         return [currentModuleName]
      }
      "specified" {
         return [currentSpecifiedName]
      }
      "shell" {
         if {$more ne ""} {
            if {$g_shell eq $more} {
               return 1
            } else {
               return 0
            }
         } else {
            return $g_shell
         }
      }
      "flags" {
         # C-version specific option, not relevant for Tcl-version but return
         # a zero integer value to avoid breaking modulefiles using it
         return 0
      }
      "shelltype" {
         if {$more ne ""} {
            if {$g_shellType eq $more} {
               return 1
            } else {
               return 0
            }
         } else {
            return $g_shellType
         }
      }
      "user" {
         # C-version specific option, not relevant for Tcl-version but return
         # an empty value or false to avoid breaking modulefiles using it
         if {$more ne ""} {
            return 0
         } else {
            return {}
         }
      }
      "alias" {
         set ret [resolveModuleVersionOrAlias $more]
         if {$ret ne $more} {
            return $ret
         } else {
            return {}
         }
      }
      "trace" {
         return {}
      }
      "tracepat" {
         return {}
      }
      "type" {
         return "Tcl"
      }
      "symbols" {
         lassign [getModuleNameVersion $more 1 1] mod
         return [join [getVersAliasList $mod] ":"]
      }
      "version" {
         lassign [getModuleNameVersion $more 1 1] mod
         return [resolveModuleVersionOrAlias $mod]
      }
      default {
         error "module-info $what not supported"
         return {}
      }
   }
}

proc module-whatis {args} {
   global g_whatis
   set mode [currentMode]
   set message [join $args " "]

   reportDebug "module-whatis: $message  mode=$mode"

   if {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "module-whatis\t$message"
   }\
   elseif {$mode eq "whatis"} {
      lappend g_whatis $message
   }
   return {}
}

# Determine with a name provided as argument the corresponding module name,
# version and name/version. Module name is guessed from current module name
# when shorthand version notation is used. Both name and version are guessed
# from current module if name provided is empty. If 'default_is_special'
# argument is enabled then a 'default' version name is considered as a
# symbol not a filename (useful for module-version proc for instance). If
# 'name_relative_tocur' is enabled then name argument may be interpreted as
# a name relative to the current modulefile directory (useful for
# module-version and module-alias for instance).
proc getModuleNameVersion {{name {}} {default_is_special 0}\
{name_relative_tocur 0}} {
   set curmod [currentModuleName]
   set curmodname [file dirname $curmod]
   set curmodversion [file tail $curmod]

   if {$name eq ""} {
      set name $curmodname
      set version $curmodversion
   # check for shorthand version notation like "/version" or "./version"
   # only if we are currently interpreting a modulefile or modulerc
   } elseif {$curmod ne "" && [regexp {^\.?\/(.*)$} $name match version]} {
      # if we cannot distinguish a module name, raise error when
      # shorthand version notation is used
      global ModulesCurrentModulefile
      if {$ModulesCurrentModulefile ne $curmod} {
         # name is the name of current module directory
         set name $curmodname
      } else {
         reportError "Invalid modulename '$name' found"
         return {}
      }
   } else {
      set version [file tail $name]
      if {$name eq $version} {
         set version ""
      } else {
         set name [file dirname $name]
         # clear version if default
         if {$version eq "default" && $default_is_special} {
            set version ""
         }
      }
      # name may correspond to last part of current module
      # if so name is replaced by current module name
      if {$name_relative_tocur && [file tail $curmodname] eq $name} {
         set name $curmodname
      }
   }

   if {$version eq ""} {
      set mod $name
   } else {
      set mod $name/$version
   }

   return [list $mod $name $version]
}

# Register alias or symbolic version deep resolution in a global array that
# can be used thereafter to get in one query the actual modulefile behind
# a virtual name. Also consolidate a global array that in the same manner
# list all the symbols held by modulefiles.
proc setModuleResolution {mod res {symver {}} {override_default 0}} {
   global g_moduleResolved g_resolvedHash
   global g_symbolHash

   # find end-point module and register path to get to it
   lappend res_path $res
   while {$mod ne $res && [info exists g_moduleResolved($res)]} {
      set res $g_moduleResolved($res)
      lappend res_path $res
   }

   # error if resolution end on initial module
   if {$mod eq $res} {
      reportError "Resolution loop on '$res' detected"
      return 0
   }

   # change default symbol owner if previously given and override permitted
   if {$symver eq "default" && [info exists g_moduleResolved($mod)]} {
      if {!$override_default} {
         reportDebug "setModuleResolution: symbol 'default' already set for\
            $mod"
         return 0
      }

      set prev $g_moduleResolved($mod)
      if {[info exists g_symbolHash($prev)]\
         && [set idx [lsearch -exact $g_symbolHash($prev) "default"]] != -1} {
         reportDebug "setModuleResolution: remove symbol 'default' from\
            '$prev'"
         set g_symbolHash($prev) [lreplace $g_symbolHash($prev) $idx $idx]
      }
   }

   # register end-point resolution
   reportDebug "setModuleResolution: $mod resolved to $res"
   set g_moduleResolved($mod) $res
   lappend g_resolvedHash($res) $mod

   # if other modules were pointing to this one, adapt resolution end-point
   if {[info exists g_resolvedHash($mod)]} {
      foreach relmod $g_resolvedHash($mod) {
         set g_moduleResolved($relmod) $res
         reportDebug "setModuleResolution: $relmod now resolved to $res"
         lappend g_resolvedHash($res) $relmod
      }
      unset g_resolvedHash($mod)
   }

   # propagate symbols to the resolution path
   if {$symver ne ""} {
      lappend sym_list $symver
      if {[info exists g_symbolHash($mod)]} {
         # dictionary-sort symbols and remove eventual duplicates
         set sym_list [lsort -dictionary -unique [concat $sym_list\
            $g_symbolHash($mod)]]
      }
      reportDebug "setModuleResolution: add symbols '$sym_list' to $res_path"
      foreach modres $res_path {
         if {[info exists g_symbolHash($modres)]} {
            set g_symbolHash($modres) [lsort -dictionary -unique [concat\
               $g_symbolHash($modres) $sym_list]]
         } else {
            set g_symbolHash($modres) $sym_list
         }
      }
   }

   return 1
}

# Specifies a default or alias version for a module that points to an 
# existing module version Note that aliases defaults are stored by the
# short module name (not the full path) so aliases and defaults from one
# directory will apply to modules of the same name found in other
# directories.
proc module-version {args} {
   global g_moduleVersion

   reportDebug "module-version: executing module-version $args"
   lassign [getModuleNameVersion [lindex $args 0] 1 1] mod modname modversion

   foreach version [lrange $args 1 end] {
      if {[string match $version "default"]} {
         # If we see more than one default for the same module, just
         # keep the first
         if {[setModuleResolution $modname $mod "default"]} {
            reportDebug "module-version: default $modname = $mod"
         }
      } else {
         set aliasversion "$modname/$version"
         reportDebug "module-version: alias $aliasversion = $mod"

         if {![info exists g_moduleVersion($aliasversion)]} {
            if {[setModuleResolution $aliasversion $mod $version]} {
               set g_moduleVersion($aliasversion) $mod
            }
         } else {
            reportWarning "Symbolic version '$aliasversion' already defined"
         }
      }
   }

   if {[currentMode] eq "display" && !$::g_inhibit_dispreport} {
      report "module-version\t$args"
   }
   return {}
}

proc module-alias {args} {
   global g_moduleAlias
   global g_sourceAlias ModulesCurrentModulefile

   lassign [getModuleNameVersion [lindex $args 0]] alias
   lassign [getModuleNameVersion [lindex $args 1] 0 1] mod

   reportDebug "module-alias: $alias = $mod"

   if {[setModuleResolution $alias $mod]} {
      set g_moduleAlias($alias) $mod
      set g_sourceAlias($alias) $ModulesCurrentModulefile
   }

   if {[currentMode] eq "display" && !$::g_inhibit_dispreport} {
      report "module-alias\t$args"
   }

   return {}
}

proc module {command args} {
   set mode [currentMode]

   # guess if called from top level
   set topcall [expr {[info level] == 1}]
   if {$topcall} {
      set msgprefix ""
   } else {
      set msgprefix "module: "
   }

   switch -regexp -- $command {
      {^(add|lo)} {
         # no error raised on empty argument list to cope with
         # initadd command that may expect this behavior
         if {[llength $args] > 0} {
            pushCommandName "load"
            if {$topcall || $mode eq "load"} {
               eval cmdModuleLoad $args
            }\
            elseif {$mode eq "unload"} {
               # on unload mode, unload mods in reverse order
               eval cmdModuleUnload [lreverse $args]
            }\
            elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
               report "module load\t$args"
            }
            popCommandName
            set needrender 1
         }
      }
      {^(rm|unlo)} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'unload' command"
         } else {
            pushCommandName "unload"
            if {$topcall || $mode eq "load"} {
               eval cmdModuleUnload $args
            }\
            elseif {$mode eq "unload"} {
               eval cmdModuleUnload $args
            }\
            elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
               report "module unload\t$args"
            }
            popCommandName
            set needrender 1
         }
      }
      {^(ref|rel)} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'reload' command"
         } else {
            pushCommandName "reload"
            cmdModuleReload
            popCommandName
            set needrender 1
         }
      }
      {^use$} {
         if {$topcall || $mode eq "load"} {
            eval cmdModuleUse $args
         } elseif {$mode eq "unload"} {
            eval cmdModuleUnuse $args
         } elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
            report "module use\t$args"
         }
         set needrender 1
      }
      {^unuse$} {
         if {$topcall || $mode eq "load" || $mode eq "unload"} {
            eval cmdModuleUnuse $args
         } elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
            report "module unuse\t$args"
         }
         set needrender 1
      }
      {^source$} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'source' command"
         } else {
            pushCommandName "source"
            eval cmdModuleSource $args
            popCommandName
            set needrender 1
         }
      }
      {^sw} {
         if {[llength $args] == 0 || [llength $args] > 2} {
            set errormsg "Unexpected number of args for 'switch' command"
         } else {
            pushCommandName "switch"
            eval cmdModuleSwitch $args
            popCommandName
            set needrender 1
         }
      }
      {^(di|show)} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'show' command"
         } else {
            pushCommandName "display"
            foreach arg $args {
               eval cmdModuleDisplay $arg
            }
            popCommandName
            set needrender 1
         }
      }
      {^av} {
         pushCommandName "avail"
         if {$args ne ""} {
            foreach arg $args {
               cmdModuleAvail $arg
            }
         } else {
            cmdModuleAvail
         }
         popCommandName
      }
      {^al} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'aliases' command"
         } else {
            pushCommandName "aliases"
            cmdModuleAliases
            popCommandName
         }
      }
      {^path$} {
         if {[llength $args] != 1} {
            set errormsg "Unexpected number of args for 'path' command"
         } else {
            eval cmdModulePath $args
            set needrender 1
         }
      }
      {^paths$} {
         if {[llength $args] != 1} {
            set errormsg "Unexpected number of args for 'paths' command"
         } else {
            eval cmdModulePaths $args
            set needrender 1
         }
      }
      {^li} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'list' command"
         } else {
            pushCommandName "list"
            cmdModuleList
            popCommandName
         }
      }
      {^wh} {
         pushCommandName "whatis"
         if {$args ne ""} {
            foreach arg $args {
               cmdModuleWhatIs $arg
            }
            set needrender 1
         } else {
            cmdModuleWhatIs
         }
         popCommandName
      }
      {^(apropos|search|keyword)$} {
         if {[llength $args] > 1} {
            set errormsg "Unexpected number of args for '$command' command"
         } else {
            pushCommandName "search"
            eval cmdModuleApropos $args
            popCommandName
         }
      }
      {^pu} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'purge' command"
         } else {
            pushCommandName "purge"
            eval cmdModulePurge
            popCommandName
            set needrender 1
         }
      }
      {^save$} {
         if {[llength $args] > 1} {
            set errormsg "Unexpected number of args for 'save' command"
         } else {
            eval cmdModuleSave $args
         }
      }
      {^restore$} {
         if {[llength $args] > 1} {
            set errormsg "Unexpected number of args for 'restore' command"
         } else {
            pushCommandName "restore"
            eval cmdModuleRestore $args
            popCommandName
            set needrender 1
         }
      }
      {^saverm$} {
         if {[llength $args] > 1} {
            set errormsg "Unexpected number of args for 'saverm' command"
         } else {
            eval cmdModuleSaverm $args
         }
      }
      {^saveshow$} {
         if {[llength $args] > 1} {
            set errormsg "Unexpected number of args for 'saveshow' command"
         } else {
            eval cmdModuleSaveshow $args
         }
      }
      {^savelist$} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'savelist' command"
         } else {
            cmdModuleSavelist
         }
      }
      {^init(a|lo)} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'initadd' command"
         } else {
            eval cmdModuleInit add $args
         }
      }
      {^initp} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'initprepend' command"
         } else {
            eval cmdModuleInit prepend $args
         }
      }
      {^initsw} {
         if {[llength $args] != 2} {
            set errormsg "Unexpected number of args for 'initswitch' command"
         } else {
            eval cmdModuleInit switch $args
         }
      }
      {^init(rm|unlo)$} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'initrm' command"
         } else {
            eval cmdModuleInit rm $args
         }
      }
      {^initl} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'initlist' command"
         } else {
            eval cmdModuleInit list $args
         }
      }
      {^initclear$} {
         if {[llength $args] != 0} {
            set errormsg "Unexpected number of args for 'initclear' command"
         } else {
            eval cmdModuleInit clear $args
         }
      }
      {^autoinit$} {
         if {$topcall} {
            if {[llength $args] != 0} {
               set errormsg "Unexpected number of args for 'autoinit' command"
            } else {
               cmdModuleAutoinit
               set needrender 1
            }
         } else {
            # autoinit cannot be called elsewhere than from top level
            set errormsg "${msgprefix}Command '$command' not supported"
         }
      }
      {^($|help)} {
         if {$topcall} {
            pushCommandName "help"
            eval cmdModuleHelp $args
            popCommandName
            if {[llength $args] != 0} {
               set needrender 1
            }
         } else {
            # help cannot be called elsewhere than from top level
            set errormsg "${msgprefix}Command '$command' not supported"
         }
      }
      {^test$} {
         if {[llength $args] == 0} {
            set errormsg "Unexpected number of args for 'test' command"
         } else {
            pushCommandName "test"
            foreach arg $args {
               eval cmdModuleTest $arg
            }
            popCommandName
            set needrender 1
         }
      }
      . {
         set errormsg "${msgprefix}Invalid command '$command'"
      }
   }

   # if an error need to be raised, proceed differently depending of
   # call level: if called from top level render errors then raise error
   # elsewhere call is made from a modulefile or modulerc and error
   # will be managed from execute-modulefile or execute-modulerc
   if {[info exists errormsg]} {
      if {$topcall} {
         reportErrorAndExit "$errormsg\nTry 'module --help'\
            for more information."
      } else {
         error "$errormsg"
      }
   # if called from top level render settings if any
   } elseif {$topcall && [info exists needrender]} {
      renderSettings
   }

   return {}
}

proc setenv {var val} {
   global g_stateEnvVars env

   set mode [currentMode]

   reportDebug "setenv: ($var,$val) mode = $mode"

   if {$mode eq "load"} {
      set env($var) $val
      set g_stateEnvVars($var) "new"
   }\
   elseif {$mode eq "unload"} {
      # Don't unset-env here ... it breaks modulefiles
      # that use env(var) is later in the modulefile
      #unset-env $var
      set g_stateEnvVars($var) "del"
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      # Let display set the variable for later use in the display
      # but don't commit it to the env
      set env($var) $val
      set g_stateEnvVars($var) "nop"
      report "setenv\t\t$var\t$val"
   }
   return {}
}

proc getenv {var} {
   set mode [currentMode]

   reportDebug "getenv: ($var) mode = $mode"

   if {$mode eq "load" || $mode eq "unload"} {
      if {[info exists ::env($var)]} {
         return $::env($var)
      } else {
         return "_UNDEFINED_"
      }
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      return "\$$var"
   }
   return {}
}

proc unsetenv {var {val {}}} {
   global g_stateEnvVars env

   set mode [currentMode]

   reportDebug "unsetenv: ($var,$val) mode = $mode"

   if {$mode eq "load"} {
      if {[info exists env($var)]} {
         unset-env $var
      }
      set g_stateEnvVars($var) "del"
   }\
   elseif {$mode eq "unload"} {
      if {$val ne ""} {
         set env($var) $val
         set g_stateEnvVars($var) "new"
      } else {
         set g_stateEnvVars($var) "del"
      }
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      if {$val ne ""} {
         report "unsetenv\t$var\t$val"
      } else {
         report "unsetenv\t$var"
      }
   }
   return {}
}

proc chdir {dir} {
   global g_changeDir
   set mode [currentMode]
   set currentModule [currentModuleName]

   reportDebug "chdir: ($dir) mode = $mode"

   if {$mode eq "load"} {
      if {[file exists $dir] && [file isdirectory $dir]} {
         set g_changeDir $dir
      } else {
         # report issue but does not treat it as an error to have the
         # same behavior as C-version
         reportWarning "Cannot chdir to '$dir' for '$currentModule'"
      }
   } elseif {$mode eq "unload"} {
      # No operation here unable to undo a syscall.
   } elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "chdir\t\t$dir"
   }

   return {}
}

# superseed exit command to handle it if called within a modulefile
# rather than exiting the whole process
proc exitModfileCmd {{code 0}} {
   global g_inhibit_interp
   set mode [currentMode]

   reportDebug "exit: ($code)"

   if {$mode eq "load"} {
      reportDebug "exit: Inhibit next modulefile interpretations"
      set g_inhibit_interp 1
   }

   # break to gently end interpretation of current modulefile
   return -code break
}

# enables slave interp to return ModulesVersion value to the master interp
proc setModulesVersion {val} {
   global ModulesVersion

   set ModulesVersion $val
}

########################################################################
# path fiddling
#
proc getReferenceCountArray {var separator} {
   global env g_force g_def_separator g_debug

   reportDebug "getReferenceCountArray: ($var, $separator)"

   set sharevar "${var}_modshare"
   set modshareok 1
   if {[info exists env($sharevar)]} {
      if {[info exists env($var)]} {
         set modsharelist [psplit $env($sharevar) $g_def_separator]
         set temp [expr {[llength $modsharelist] % 2}]

         if {$temp == 0} {
            array set countarr $modsharelist

            # sanity check the modshare list
            array set fixers {}
            array set usagearr {}

            foreach dir [split $env($var) $separator] {
               set usagearr($dir) 1
            }
            foreach path [array names countarr] {
               if {! [info exists usagearr($path)]} {
                  unset countarr($path)
                  set fixers($path) 1
               }
            }

            foreach path [array names usagearr] {
               if {! [info exists countarr($path)]} {
                  # if no ref count found for a path, assume it has a ref
                  # count of 1 to be able to unload it easily if needed
                  set countarr($path) 1
               }
            }

            if {! $g_force} {
               if {[array size fixers]} {
                  reportWarning "\$$var does not agree with\
                     \$${var}_modshare counter. The following\
                     directories' usage counters were adjusted to match.\
                     Note that this may mean that module unloading may\
                     not work correctly."
                  foreach dir [array names fixers] {
                     report " $dir" -nonewline
                  }
                  report ""
               }
            }
         } else {
            # sharevar was corrupted, odd number of elements.
            set modshareok 0
         }
      } else {
         reportWarning "$sharevar exists ( $env($sharevar) ), but $var\
            doesn't. Environment is corrupted."
         set modshareok 0
      }
   } else {
      set modshareok 0
   }

   if {$modshareok == 0 && [info exists env($var)]} {
      array set countarr {}
      foreach dir [split $env($var) $separator] {
         set countarr($dir) 1
      }
   }
   return [array get countarr]
}


proc unload-path {var path separator} {
   global g_stateEnvVars env g_force g_def_separator

   array set countarr [getReferenceCountArray $var $separator]

   reportDebug "unload-path: ($var, $path, $separator)"

   # Don't worry about dealing with this variable if it is already scheduled
   #  for deletion
   if {[info exists g_stateEnvVars($var)] && $g_stateEnvVars($var) eq "del"} {
      return {}
   }

   # enable removal of an empty path
   if {$path eq ""} {
      set path $separator
   # raise error on removal attempt of a path equals to separator
   } elseif {$path eq $separator} {
      error "unload-path cannot handle path equals to separator string"
   }

   foreach dir [split $path $separator] {
      set doit 0

      if {[info exists countarr($dir)]} {
         incr countarr($dir) -1
         if {$countarr($dir) <= 0} {
            set doit 1
            unset countarr($dir)
         }
      } else {
         set doit 1
      }

      if {$doit || $g_force} {
         if {[info exists env($var)]} {
            set dirs [split $env($var) $separator]
            set newpath ""
            foreach elem $dirs {
               if {$elem ne $dir} {
                  lappend newpath $elem
               }
            }
            if {$newpath eq ""} {
               unset-env $var
               set g_stateEnvVars($var) "del"
            } else {
               set env($var) [join $newpath $separator]
               set g_stateEnvVars($var) "new"
            }
         }
      }
   }

   set sharevar "${var}_modshare"
   if {[array size countarr] > 0} {
      set env($sharevar) [pjoin [array get countarr] $g_def_separator]
      set g_stateEnvVars($sharevar) "new"
   } else {
      unset-env $sharevar
      set g_stateEnvVars($sharevar) "del"
   }
   return {}
}

proc add-path {var path pos separator} {
   global env g_stateEnvVars g_def_separator

   reportDebug "add-path: ($var, $path, $separator)"

   set sharevar "${var}_modshare"
   array set countarr [getReferenceCountArray $var $separator]

   # enable add of an empty path
   if {$path eq ""} {
      set path $separator
   # raise error on removal attempt of a path equals to separator
   } elseif {$path eq $separator} {
      error "add-path cannot handle path equals to separator string"
   }

   if {$pos eq "prepend"} {
      set pathelems [lreverse [split $path $separator]]
   } else {
      set pathelems [split $path $separator]
   }
   foreach dir $pathelems {
      if {[info exists countarr($dir)]} {
         # already see $dir in $var"
         incr countarr($dir)
      } else {
         if {[info exists env($var)] && $env($var) ne ""} {
            if {$pos eq "prepend"} {
               set env($var) "$dir$separator$env($var)"
            }\
            elseif {$pos eq "append"} {
               set env($var) "$env($var)$separator$dir"
            } else {
               error "add-path doesn't support $pos"
            }
         } else {
            set env($var) "$dir"
         }
         set countarr($dir) 1
      }
      reportDebug "add-path: env($var) = $env($var)"
   }

   set env($sharevar) [pjoin [array get countarr] $g_def_separator]
   set g_stateEnvVars($var) "new"
   set g_stateEnvVars($sharevar) "new"
   return {}
}

proc prepend-path {var path args} {
   global g_def_separator

   set mode [currentMode]

   reportDebug "prepend-path: ($var, $path, $args) mode=$mode"

   if {($var eq "--delim") || ($var eq "-d") || ($var eq "-delim")} {
      set separator $path
      set var [lindex $args 0]
      set path [lindex $args 1]
   } else {
      set separator $g_def_separator
   }

   if {$mode eq "load"} {
      add-path $var $path "prepend" $separator
   }\
   elseif {$mode eq "unload"} {
      unload-path $var $path $separator
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "prepend-path\t$var\t$path"
   }

   return {}
}


proc append-path {var path args} {
   global g_def_separator

   set mode [currentMode]

   reportDebug "append-path: ($var, $path, $args) mode=$mode"

   if {($var eq "--delim") || ($var eq "-d") || ($var eq "-delim")} {
      set separator $path
      set var [lindex $args 0]
      set path [lindex $args 1]
   } else {
      set separator $g_def_separator
   }

   if {$mode eq "load"} {
      add-path $var $path "append" $separator
   }\
   elseif {$mode eq "unload"} {
      unload-path $var $path $separator
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "append-path\t$var\t$path"
   }

   return {}
}

proc remove-path {var path args} {
   global g_def_separator

   set mode [currentMode]

   reportDebug "remove-path: ($var, $path, $args) mode=$mode"

   if {($var eq "--delim") || ($var eq "-d") || ($var eq "-delim")} {
      set separator $path
      set var [lindex $args 0]
      set path [lindex $args 1]
   } else {
      set separator $g_def_separator
   }

   if {$mode eq "load"} {
      unload-path $var $path $separator
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "remove-path\t$var\t$path"
   }
   return {}
}

proc set-alias {alias what} {
   global g_Aliases g_stateAliases
   set mode [currentMode]

   reportDebug "set-alias: ($alias, $what) mode=$mode"
   if {$mode eq "load"} {
      set g_Aliases($alias) $what
      set g_stateAliases($alias) "new"
   }\
   elseif {$mode eq "unload"} {
      set g_Aliases($alias) {}
      set g_stateAliases($alias) "del"
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "set-alias\t$alias\t$what"
   }

   return {}
}

proc unset-alias {alias} {
   global g_Aliases g_stateAliases

   set mode [currentMode]

   reportDebug "unset-alias: ($alias) mode=$mode"
   if {$mode eq "load"} {
      set g_Aliases($alias) {}
      set g_stateAliases($alias) "del"
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "unset-alias\t$alias"
   }

   return {}
}

proc is-loaded {modulelist} {
   reportDebug "is-loaded: $modulelist"

   if {[llength $modulelist] > 0} {
      set loadedmodlist [getLoadedModuleList]
      if {[llength $loadedmodlist] > 0} {
         foreach arg $modulelist {
            set arg "$arg/"
            foreach mod $loadedmodlist {
               set mod "$mod/"
               if {[string first $arg $mod] == 0} {
                  return 1
               }
            }
         }
         return 0
      } else {
         return 0
      }
   }
   return 1
}

proc conflict {args} {
   set mode [currentMode]
   set currentModule [currentModuleName]

   reportDebug "conflict: ($args) mode = $mode"

   if {$mode eq "load"} {
      foreach mod $args {
         # If the current module is already loaded, we can proceed
         if {![is-loaded $currentModule]} {
            # otherwise if the conflict module is loaded, we cannot
            if {[is-loaded $mod]} {
               set errMsg "WARNING: $currentModule cannot be loaded due\
                  to a conflict."
               set errMsg "$errMsg\nHINT: Might try \"module unload\
                  $mod\" first."
               error $errMsg
            }
         }
      }
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "conflict\t$args"
   }

   return {}
}

proc prereq {args} {
   set mode [currentMode]
   set currentModule [currentModuleName]

   reportDebug "prereq: ($args) mode = $mode"

   if {$mode eq "load"} {
      if {![is-loaded $args]} {
         set errMsg "WARNING: $currentModule cannot be loaded due to\
             missing prereq."
         # adapt error message when multiple modules are specified
         if {[llength $args] > 1} {
            set errMsg "$errMsg\nHINT: at least one of the following\
               modules must be loaded first: $args"
         } else {
            set errMsg "$errMsg\nHINT: the following module must be\
               loaded first: $args"
         }
         error $errMsg
      }
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "prereq\t\t$args"
   }

   return {}
}

proc x-resource {resource {value {}}} {
   global g_newXResources g_delXResources env

   set mode [currentMode]

   reportDebug "x-resource: ($resource, $value)"

   # sometimes x-resource value may be provided within resource name
   # as the "x-resource {Ileaf.popup.saveUnder: True}" example provided
   # in manpage. so here is an attempt to extract real resource name and
   # value from resource argument
   if {[string length $value] == 0 && ![file exists $resource]} {
      # look first for a space character as delimiter, then for a colon
      set sepapos [string first " " $resource]
      if { $sepapos == -1 } {
         set sepapos [string first ":" $resource]
      }

      if { $sepapos > -1 } {
         set value [string range $resource [expr {$sepapos + 1}] end]
         set resource [string range $resource 0 [expr {$sepapos - 1}]]
         reportDebug "x-resource: corrected ($resource, $value)"
      } else {
         # if not a file and no value provided x-resource cannot be
         # recorded as it will produce an error when passed to xrdb
         reportWarning "x-resource $resource is not a valid string or file"
         return {}
      }
   }

   if {![info exists env(DISPLAY)] && ($mode eq "load"\
      || $mode eq "unload")} {
      error "WARNING: x-resource cannot edit X11 resource with no DISPLAY set"
   }

   # if a resource does hold an empty value in g_newXResources or
   # g_delXResources arrays, it means this is a resource file to parse
   if {$mode eq "load"} {
      set g_newXResources($resource) $value
   }\
   elseif {$mode eq "unload"} {
      set g_delXResources($resource) $value
   }\
   elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      report "x-resource\t$resource\t$value"
   }

   return {}
}

proc uname {what} {
   global unameCache tcl_platform
   set result {}

   reportDebug "uname: called: $what"

   if {! [info exists unameCache($what)]} {
      switch -- $what {
         sysname {
            set result $tcl_platform(os)
         }
         machine {
            set result $tcl_platform(machine)
         }
         nodename - node {
            set result [info hostname]
         }
         release {
            # on ubuntu get the CODENAME of the Distribution
            if { [file isfile /etc/lsb-release]} {
               set fd [open "/etc/lsb-release" "r"]
               set a [read $fd]
               regexp -nocase {DISTRIB_CODENAME=(\S+)(.*)}\
                  $a matched res end
               set result $res
            } else {
               set result $tcl_platform(osVersion)
            }
         }
         domain {
            set result [exec /bin/domainname]
         }
         version {
            if { [file isfile /bin/uname]} {
               set result [exec /bin/uname -v]
            } else {
               set result [exec /usr/bin/uname -v]
            }
         }
         default {
            error "uname $what not supported"
         }
      }
      set unameCache($what) $result
   }

   return $unameCache($what)
}

proc system {mycmd args} {
   reportDebug "system: $mycmd $args"

   set mode [currentMode]
   set status {}

   if {$mode eq "load" || $mode eq "unload"} {
      if {[catch {exec >&@stderr $mycmd $args}]} {
          # non-zero exit status, get it:
          set status [lindex $::errorCode 2]
      } else {
          # exit status was 0
          set status 0
      }
   } elseif {$mode eq "display" && !$::g_inhibit_dispreport} {
      if {[llength $args] == 0} {
         report "system\t\t$mycmd"
      } else {
         report "system\t\t$mycmd $args"
      }
   }

   return $status
}

########################################################################
# internal module procedures
#
set g_modeStack {}

proc currentMode {} {
   global g_modeStack

   return [lindex $g_modeStack end]
}

proc pushMode {mode} {
   global g_modeStack

   lappend g_modeStack $mode
}

proc popMode {} {
   global g_modeStack

   set g_modeStack [lrange $g_modeStack 0 end-1]
}

set g_moduleNameStack {}

proc currentModuleName {} {
   global g_moduleNameStack

   return [lindex $g_moduleNameStack end]
}

proc pushModuleName {moduleName} {
   global g_moduleNameStack

   lappend g_moduleNameStack $moduleName
}

proc popModuleName {} {
   global g_moduleNameStack

   set g_moduleNameStack [lrange $g_moduleNameStack 0 end-1]
}

set g_specifiedNameStack {}

proc currentSpecifiedName {} {
   global g_specifiedNameStack

   return [lindex $g_specifiedNameStack end]
}

proc pushSpecifiedName {specifiedName} {
   global g_specifiedNameStack

   lappend g_specifiedNameStack $specifiedName
}

proc popSpecifiedName {} {
   global g_specifiedNameStack

   set g_specifiedNameStack [lrange $g_specifiedNameStack 0 end-1]
}

set g_commandNameStack {}

proc currentCommandName {} {
   global g_commandNameStack

   return [lindex $g_commandNameStack end]
}

proc pushCommandName {commandName} {
   global g_commandNameStack

   lappend g_commandNameStack $commandName
}

proc popCommandName {} {
   global g_commandNameStack

   set g_commandNameStack [lrange $g_commandNameStack 0 end-1]
}


# return list of loaded modules by parsing LOADEDMODULES env variable
proc getLoadedModuleList {} {
   global env g_def_separator

   if {[info exists env(LOADEDMODULES)]} {
      return [split $env(LOADEDMODULES) $g_def_separator]
   } else {
      return {}
   }
}

# return list of loaded module files by parsing _LMFILES_ env variable
proc getLoadedModuleFileList {} {
   global env g_def_separator

   if {[info exists env(_LMFILES_)]} {
      return [split $env(_LMFILES_) $g_def_separator]
   } else {
      return {}
   }
}

# return list of module paths by parsing MODULEPATH env variable
# behavior param enables to exit in error when no MODULEPATH env variable
# is set. by default an empty list is returned if no MODULEPATH set
proc getModulePathList {{behavior "returnempty"}} {
   global env g_def_separator

   if {[info exists env(MODULEPATH)]} {
      return [split $env(MODULEPATH) $g_def_separator]
   } elseif {$behavior eq "exiterronundef"} {
      reportErrorAndExit "No module path defined"
   } else {
      return {}
   }
}

# test if two modules share the same root name
proc isSameModuleRoot {mod1 mod2} {
   set mod1split [split $mod1 "/"]
   set mod2split [split $mod2 "/"]

   return [expr {[lindex $mod1split 0] eq [lindex $mod2split 0]}]
}

# Return the full pathname and modulename to the module.  
# Resolve aliases and default versions if the module name is something like
# "name/version" or just "name" (find default version).
proc getPathToModule {mod {indir {}}} {
   global g_loadedModules g_loadedModulesGeneric
   global g_invalid_mod_info g_access_mod_info

   set retlist {}
   set g_found_mod_issue 0

   if {$mod eq ""} {
      return [list "" 0]
   }

   reportDebug "getPathToModule: finding '$mod' in '$indir'"

   # Check for $mod specified as a full pathname
   if {[string match {/*} $mod]} {
      # note that a raw filename as an argument returns the full
      # path as the module name
      if {[checkValidModule $mod]} {
         set retlist [list $mod $mod]
      } elseif {[info exists g_invalid_mod_info($mod)]} {
         reportInternalBug $g_invalid_mod_info($mod)
         set g_found_mod_issue 1
      } elseif {[info exists g_access_mod_info($mod)]} {
         reportError $g_access_mod_info($mod)
         set g_found_mod_issue 1
      }
   # try first to look at loaded modules
   } elseif {[info exists g_loadedModules($mod)]} {
      set retlist [list $g_loadedModules($mod) $mod]
   } elseif {[info exists g_loadedModulesGeneric($mod)]} {
      set elt "$mod/$g_loadedModulesGeneric($mod)"
      set retlist [list $g_loadedModules($elt) $elt]
   } else {
      if {$indir ne ""} {
         set dir_list $indir
      } else {
         set dir_list [getModulePathList "exiterronundef"]
      }

      # modparent is the the modulename minus the module version.
      lassign [getModuleNameVersion $mod] mod modparent modversion
      set modroot [lindex [split $mod "/"] 0]

      # Now search for $mod in module paths
      foreach dir $dir_list {
         # get list of modules related to the root of searched module to get
         # in one call a complete list of any module kind (file, alias, etc)
         # related to search to be able to then determine in this proc the
         # correct module to return without restarting new searches
         array unset mod_list
         array set mod_list [getModules $dir $modroot]
         # Check for an alias set by .modulerc found on parent path
         # or by a previously looked modulefile/modulerc and follow
         set newmod [resolveModuleVersionOrAlias $mod]
         # search on newmod if it is a module from same root as mod_list
         # already contains everything related to this root module
         if {[isSameModuleRoot $mod $newmod]} {
            set mod $newmod
         # elsewhere restart search on new modulename
         } else {
            return [getPathToModule $newmod]
         }

         # look if we can find mod in dir
         set path "$dir/$mod"
         if {[info exists mod_list($mod)]} {
            set prevpath ""
            # loop to resolve correct modulefile in case specified mod is a
            # directory that should be analyzed to get default mod in it
            while {$prevpath ne $path && !$g_found_mod_issue} {
               set prevpath $path
               # If a directory, check for default
               if {[lindex $mod_list($mod) 0] eq "directory"} {
                  set ModulesVersion ""

                  # Check for an alias that may have been set by modulerc
                  lassign [getModuleNameVersion\
                     [resolveModuleVersionOrAlias $mod]]\
                     newmod newparent newversion
                  # if alias resolve to a different modulename then
                  if {$newmod ne $mod} {
                     # follow search on newmod if module from same root
                     if {[isSameModuleRoot $mod $newmod]} {
                        set mod $newparent
                        set ModulesVersion $newversion
                        set path "$dir/$mod"
                     # elsewhere restart search on new modulename
                     } else {
                        return [getPathToModule $newmod]
                     }
                  }

                  # Try for the last element in directory if no luck so far
                  # and if any element in directory
                  if {$ModulesVersion eq "" && [set ModulesVersion [lindex\
                     [lsort -dictionary [lrange $mod_list($mod) 1 end]]\
                     end]] ne ""} {

                     # Check if last element is an alias that need resolution
                     lassign [getModuleNameVersion\
                        [resolveModuleVersionOrAlias $mod/$ModulesVersion]]\
                        newmod newparent newversion
                     # if alias resolve to a different modulename then
                     if {$newmod ne "$mod/$ModulesVersion"} {
                        # follow search on newmod if module from same root
                        if {[isSameModuleRoot $mod $newmod]} {
                           set mod $newparent
                           set ModulesVersion $newversion
                           set path "$dir/$mod"
                        # elsewhere restart search on new modulename
                        } else {
                           return [getPathToModule $newmod]
                        }
                     }
                  }

                  if {$ModulesVersion ne ""} {
                     # check found version is known before moving to it
                     if {[info exists mod_list($mod/$ModulesVersion)]} {
                        set modparent $mod
                        # The path to the module file
                        set path "$path/$ModulesVersion"
                        # The modulename (name + version)
                        set mod "$mod/$ModulesVersion"

                        # if ModulesVersion is a directory keep looking in it
                        # elsewhere we have found a module to return
                        if {[lindex $mod_list($mod) 0] ne "directory"} {
                           set retlist [list $path $mod]
                        }
                     # or an error message to report if file is invalid
                     } elseif {[info exists\
                        g_invalid_mod_info($path/$ModulesVersion)]} {
                        reportInternalBug\
                           $g_invalid_mod_info($path/$ModulesVersion)
                        set g_found_mod_issue 1
                     } elseif {[info exists\
                        g_access_mod_info($path/$ModulesVersion)]} {
                        reportError $g_access_mod_info($path/$ModulesVersion)
                        set g_found_mod_issue 1
                     }
                  }
               } else {
                  # If mod was a file in this path, try and return that file
                  set retlist [list $path $mod]
               }
            }
         # mod may be found but invalid, so report error and end search
         } elseif {[info exists g_invalid_mod_info($path)]} {
            reportInternalBug $g_invalid_mod_info($path)
            set g_found_mod_issue 1
         } elseif {[info exists g_access_mod_info($path)]} {
            reportError $g_access_mod_info($path)
            set g_found_mod_issue 1
         }
         # break loop if found something (valid or invalid module)
         # elsewhere go to next path
         if {[llength $retlist] == 2 || $g_found_mod_issue} {
            break
         }
      }
   }

   if {[llength $retlist] == 2} {
      reportDebug "getPathToModule: found '[lindex $retlist 0]' as\
         '[lindex $retlist 1]'"
   } else {
      set retlist [list "" $g_found_mod_issue]
      if {!$g_found_mod_issue && $indir eq ""} {
         reportError "Unable to locate a modulefile for '$mod'"
      }
   }
   return $retlist
}

# return the currently loaded module whose name is the closest to the
# name passed as argument. if no loaded module match at least one part
# of the passed name, an empty string is returned.
proc getLoadedWithClosestName {name} {
   set ret ""
   set retmax 0
   set namesplit [split $name "/"]

   # compare name to each currently loaded module name
   foreach mod [getLoadedModuleList] {
      set modsplit [split $mod "/"]
      # min expr function is not supported in Tcl8.4 and earlier
      if {[llength $namesplit] < [llength $modsplit]} {
         set imax [llength $namesplit]
      } else {
         set imax [llength $modsplit]
      }

      # compare each element of the name to find closest answer
      # in case of equality, last loaded module will be returned as it
      # overwrites previously found value
      for {set i 0} {$i < $imax} {incr i} {
         if {[lindex $modsplit $i] eq [lindex $namesplit $i]} {
            if {$i >= $retmax} {
               set retmax $i
               set ret $mod
            }
         }
      }
   }

   reportDebug "getLoadedWithClosestName: \"$ret\" closest to \"$name\""

   return $ret
}

proc runModulerc {} {
   # Runs the global RC files if they exist
   global env
   global g_moduleAlias g_rcAlias
   set rclist {}

   reportDebug "runModulerc: running..."

   if {[info exists env(MODULERCFILE)]} {
      # if MODULERCFILE is a dir, look at a modulerc file in it
      if {[file isdirectory $env(MODULERCFILE)]\
         && [file isfile "$env(MODULERCFILE)/modulerc"]} {
         lappend rclist "$env(MODULERCFILE)/modulerc"
      } elseif {[file isfile $env(MODULERCFILE)]} {
         lappend rclist $env(MODULERCFILE)
      }
   }
   if {[info exists env(MODULESHOME)]\
      && [file isfile "$env(MODULESHOME)/etc/rc"]} {
      lappend rclist "$env(MODULESHOME)/etc/rc"
   }
   if {[info exists env(HOME)] && [file isfile "$env(HOME)/.modulerc"]} {
      lappend rclist "$env(HOME)/.modulerc"
   }

   foreach rc $rclist {
      if {[file readable $rc]} {
         reportDebug "runModulerc: Executing $rc"
         cmdModuleSource "$rc"
      }
   }

   # identify alias set in these global RC files to be able to display
   # them in a specific category on avail output
   array set g_rcAlias [array get g_moduleAlias]
}

# manage settings to save as a stack to have a separate set of settings
# for each module loaded or unloaded in order to be able to restore the
# correct set in case of failure
proc pushSettings {} {
   foreach var {env g_Aliases g_stateEnvVars g_stateAliases g_newXResource\
      g_delXResource} {
      eval "global g_SAVE_$var $var"
      eval "lappend g_SAVE_$var \[array get $var\]"
   }
}

proc popSettings {} {
   foreach var {env g_Aliases g_stateEnvVars g_stateAliases g_newXResource\
      g_delXResource} {
      eval "global g_SAVE_$var"
      eval "set g_SAVE_$var \[lrange \$g_SAVE_$var 0 end-2\]"
   }
}

proc restoreSettings {} {
   foreach var {env g_Aliases g_stateEnvVars g_stateAliases g_newXResource\
      g_delXResource} {
      eval "global g_SAVE_$var $var"
      # clear current $var arrays
      if {[info exists $var]} {
         eval "unset $var; array set $var {}"
      }
      eval "array set $var \[lindex \$g_SAVE_$var end\]"
   }
}

proc renderSettings {} {
   global env g_Aliases g_shellType g_shell
   global g_stateEnvVars g_stateAliases
   global g_newXResources g_delXResources
   global g_pathList g_changeDir error_count
   global g_autoInit CSH_LIMIT cwd

   reportDebug "renderSettings: called."

   set iattempt 0

   # required to work on cygwin, shouldn't hurt real linux
   fconfigure stdout -translation lf

   # preliminaries

   switch -- $g_shellType {
      python {
         puts stdout "import os"
      }
   }

   if {$g_autoInit} {
      global argv0

      # automatically detect which tclsh should be used for 
      # future module commands
      set tclshbin [info nameofexecutable]

      # ensure script path is absolute
      set argv0 [getAbsolutePath $argv0]

      # guess MODULESHOME from modulecmd directory
      set modshome [file dirname $argv0]
      if { [file tail $modshome] eq "bin" \
         || [file tail $modshome] eq "libexec" } {
         # use upper dir if modulecmd is located in a bin or libexec dir
         set modshome [file dirname $modshome]
      }
      set env(MODULESHOME) $modshome
      set g_stateEnvVars(MODULESHOME) "new"

      switch -- $g_shellType {
         csh {
            puts stdout "    alias module 'eval \
               `'$tclshbin' '$argv0' '$g_shell' \\!*`';"
         }
         sh {
            puts stdout "module () { eval \
               `'$tclshbin' '$argv0' '$g_shell' \$*`; } ;"
         }
         fish {
            puts stdout "function module"
            puts stdout "    eval '$tclshbin' '$argv0' '$g_shell' \$argv | source -"
            puts stdout "end"
         }
         tcl {
            puts stdout "proc module {args}  {"
            puts stdout "    global env;"
            puts stdout "    set script {};"
            puts stdout "    if {\[catch { set script \[eval exec\
                \"$tclshbin\" \"$argv0\" \"$g_shell\" \$args] } msg]} {"
            puts stdout "        puts \$msg"
            puts stdout "    };"
            puts stdout "    uplevel \$script;"
            puts stdout "}"

         }
         cmd {
            puts stdout "start /b \%MODULESHOME\%/init/module.cmd %*"
         }
         perl {
            puts stdout "sub module {"
            puts stdout "  eval `$tclshbin $argv0 perl @_`;"
            puts stdout "  if(\$@) {"
            puts stdout "    use Carp;"
            puts stdout "    confess \"module-error: \$@\n\";"
            puts stdout "  }"
            puts stdout "  return 1;"
            puts stdout "}"
         }
         python {
            puts stdout "import subprocess"
            puts stdout "def module(command, *arguments):"
            puts stdout "        exec(subprocess.Popen(\['$tclshbin',\
               '$argv0', 'python', command\] +\
               list(arguments),\
               stdout=subprocess.PIPE).communicate()\[0\])"
         }
         lisp {
            reportErrorAndExit "lisp mode autoinit not yet implemented"
         }
      }

      if {[file exists "$env(MODULESHOME)/modulerc"]} {
         cmdModuleSource "$env(MODULESHOME)/modulerc"
      }
      if {[file exists "$env(MODULESHOME)/init/modulerc"]} {
         cmdModuleSource "$env(MODULESHOME)/init/modulerc"
      }
   }

   # new environment variables
   foreach var [array names g_stateEnvVars] {
      if {$g_stateEnvVars($var) eq "new"} {
         switch -- $g_shellType {
            csh {
               set val [charEscaped $env($var)]
               # csh barfs on long env vars
               if {$g_shell eq "csh" && [string length $val] >\
                  $CSH_LIMIT} {
                  if {$var eq "PATH"} {
                     reportWarning "PATH exceeds $CSH_LIMIT characters,\
                        truncating and appending /usr/bin:/bin ..."
                     set val [string range $val 0 [expr {$CSH_LIMIT\
                        - 1}]]:/usr/bin:/bin
                  } else {
                      reportWarning "$var exceeds $CSH_LIMIT characters,\
                         truncating..."
                      set val [string range $val 0 [expr {$CSH_LIMIT\
                         - 1}]]
                  }
               }
               puts stdout "setenv $var $val;"
            }
            sh {
               puts stdout "$var=[charEscaped $env($var)];\
                  export $var;"
            }
            fish {
               set val [charEscaped $env($var)]
               # fish shell has special treatment for PATH variable
               # so its value should be provided as a list separated
               # by spaces not by semi-colons
               if {$var eq "PATH"} {
                  regsub -all ":" $val " " val
               }
               puts stdout "set -xg $var $val;"
            }
            tcl {
               set val [charEscaped $env($var) \"]
               puts stdout "set env($var) \"$val\";"
            }
            perl {
               set val [charEscaped $env($var) \']
               puts stdout "\$ENV{\'$var\'} = \'$val\';"
            }
            python {
               set val [charEscaped $env($var) \']
               puts stdout "os.environ\['$var'\] = '$val'"
            }
            lisp {
               set val [charEscaped $env($var) \"]
               puts stdout "(setenv \"$var\" \"$val\")"
            }
            cmd {
               set val $env($var)
               puts stdout "set $var=$val"
            }
         }
      } elseif {$g_stateEnvVars($var) eq "del"} {
         switch -- $g_shellType {
            csh {
               puts stdout "unsetenv $var;"
            }
            sh {
               puts stdout "unset $var;"
            }
            fish {
               puts stdout "set -e $var;"
            }
            tcl {
               puts stdout "unset env($var);"
            }
            cmd {
               puts stdout "set $var="
            }
            perl {
               puts stdout "delete \$ENV{\'$var\'};"
            }
            python {
               puts stdout "os.environ\['$var'\] = ''"
               puts stdout "del os.environ\['$var'\]"
            }
            lisp {
               puts stdout "(setenv \"$var\" nil)"
            }
         }
      }
   }

   foreach var [array names g_stateAliases] {
      if {$g_stateAliases($var) eq "new"} {
         switch -- $g_shellType {
            csh {
               # set val [charEscaped $g_Aliases($var)]
               set val $g_Aliases($var)
               # Convert $n -> \!\!:n
               regsub -all {\$([0-9]+)} $val {\\!\\!:\1} val
               # Convert $* -> \!*
               regsub -all {\$\*} $val {\\!*} val
               puts stdout "alias $var '$val';"
            }
            sh {
               set val $g_Aliases($var)
               puts stdout "alias $var=\'$val\';"
            }
            fish {
               set val $g_Aliases($var)
               puts stdout "alias $var '$val';"
            }
            tcl {
               set val $g_Aliases($var)
               puts stdout "alias $var \"$val\";"
            }
         }
      } elseif {$g_stateAliases($var) eq "del"} {
         switch -- $g_shellType {
            csh {
               puts stdout "unalias $var;"
            }
            sh {
               puts stdout "unalias $var;"
            }
            fish {
               puts stdout "functions -e $var;"
            }
            tcl {
               puts stdout "unalias $var;"
            }
         }
      }
   }

   # new x resources
   if {[array size g_newXResources] > 0} {
      set xrdb [findExecutable "xrdb"]
      switch -- $g_shellType {
         python {
            puts stdout "import subprocess"
         }
      }
      foreach var [array names g_newXResources] {
         set val $g_newXResources($var)
         # empty val means that var is a file to parse
         if {$val eq ""} {
            switch -regexp -- $g_shellType {
               {^(csh|fish|sh)$} {
                  puts stdout "$xrdb -merge $var;"
               }
               tcl {
                  puts stdout "exec $xrdb -merge $var;"
               }
               perl {
                  puts stdout "system(\"$xrdb -merge $var\");"
               }
               python {
                  set var [charEscaped $var \']
                  puts stdout "subprocess.Popen(\['$xrdb',\
                     '-merge', '$var'\])"
               }
               lisp {
                  puts stdout "(shell-command-to-string \"$xrdb\
                     -merge $var\")"
               }
            }
         } else {
            switch -regexp -- $g_shellType {
               {^(csh|fish|sh)$} {
                  set var [charEscaped $var \"]
                  set val [charEscaped $val \"]
                  puts stdout "echo \"$var: $val\" | $xrdb -merge;"
               }
               tcl {
                  puts stdout "set XRDBPIPE \[open \"|$xrdb -merge\" r+\];"
                  set var [charEscaped $var \"]
                  set val [charEscaped $val \"]
                  puts stdout "puts \$XRDBPIPE \"$var: $val\";"
                  puts stdout "close \$XRDBPIPE;"
                  puts stdout "unset XRDBPIPE;"
               }
               perl {
                  puts stdout "open(XRDBPIPE, \"|$xrdb -merge\");"
                  set var [charEscaped $var \"]
                  set val [charEscaped $val \"]
                  puts stdout "print XRDBPIPE \"$var: $val\\n\";"
                  puts stdout "close XRDBPIPE;"
               }
               python {
                  set var [charEscaped $var \']
                  set val [charEscaped $val \']
                  puts stdout "subprocess.Popen(\['$xrdb', '-merge'\],\
                     stdin=subprocess.PIPE).communicate(input='$var: $val\\n')"
               }
               lisp {
                  puts stdout "(shell-command-to-string \"echo $var:\
                     $val | $xrdb -merge\")"
               }
            }
         }
      }
   }

   if {[array size g_delXResources] > 0} {
      set xrdb [findExecutable "xrdb"]
      set xres_to_del {}
      foreach var [array names g_delXResources] {
         # empty val means that var is a file to parse
         if {$g_delXResources($var) eq ""} {
            # xresource file has to be parsed to find what resources
            # are declared there and need to be unset
            foreach fline [split [exec $xrdb -n load $var] "\n"] {
               lappend xres_to_del [lindex [split $fline ":"] 0]
            }
         } else {
            lappend xres_to_del $var
         }
      }

      # xresource strings are unset by emptying their value since there
      # is no command of xrdb that can properly remove one property
      switch -regexp -- $g_shellType {
         {^(csh|fish|sh)$} {
            foreach var $xres_to_del {
               puts stdout "echo \"$var:\" | $xrdb -merge;"
            }
         }
         tcl {
            foreach var $xres_to_del {
               puts stdout "set XRDBPIPE \[open \"|$xrdb -merge\" r+\];"
               set var [charEscaped $var \"]
               puts stdout "puts \$XRDBPIPE \"$var:\";"
               puts stdout "close \$XRDBPIPE;"
               puts stdout "unset XRDBPIPE;"
            }
         }
         perl {
            foreach var $xres_to_del {
               puts stdout "open(XRDBPIPE, \"|$xrdb -merge\");"
               set var [charEscaped $var \"]
               puts stdout "print XRDBPIPE \"$var:\\n\";"
               puts stdout "close XRDBPIPE;"
            }
         }
         python {
            puts stdout "import subprocess"
            foreach var $xres_to_del {
               set var [charEscaped $var \']
               puts stdout "subprocess.Popen(\['$xrdb', '-merge'\],\
                  stdin=subprocess.PIPE).communicate(input='$var:\\n')"
            }
         }
         lisp {
            foreach var $xres_to_del {
               puts stdout "(shell-command-to-string \"echo $var: |\
                  $xrdb -merge\")"
            }
         }
      }
   }

   if {[info exists g_changeDir]} {
      switch -regexp -- $g_shellType {
         {^(csh|fish|sh)$} {
            puts stdout "cd '$g_changeDir';"
         }
         tcl {
            puts stdout "cd \"$g_changeDir\";"
         }
         perl {
            puts stdout "chdir '$g_changeDir';"
         }
         python {
            puts stdout "os.chdir('$g_changeDir')"
         }
         lisp {
            puts stdout "(shell-command-to-string \"cd '$g_changeDir'\")"
         }
      }
   }

   # module path{s,} output
   if {[info exists g_pathList]} {
      foreach var $g_pathList {
         switch -- $g_shellType {
            csh {
               puts stdout "echo '$var';"
            }
            sh {
               puts stdout "echo '$var';"
            }
            fish {
               puts stdout "echo '$var';"
            }
            tcl {
               puts stdout "puts \"$var\";"
            }
            cmd {
               puts stdout "echo '$var'"
            }
            perl {
               puts stdout "print '$var'.\"\\n\";"
            }
            python {
               puts stdout "print '$var'"
            }
            lisp {
               puts stdout "(message \"$var\")"
            }
         }
      }
   }

   set nop 0
   if {$error_count == 0 && ! [tell stdout]} {
      set nop 1
   }

   if {$error_count > 0} {
      renderError
      set nop 0
   } else {
      switch -- $g_shellType {
         perl {
            puts stdout "1;"
         }
      }
   }

   if {$nop} {
      #            nothing written!
      switch -- $g_shellType {
         csh {
            puts "/bin/true;"
         }
         sh {
            puts "/bin/true;"
         }
         fish {
            puts "/bin/true;"
         }
         tcl {
            puts "exec /bin/true;"
         }
         cmd {
            # nothing needed, reserve for future cygwin, MKS, etc
         }
         perl {
            puts "1;"
         }
         python {
            # this is not correct
            puts ""
         }
         lisp {
            puts "t"
         }
      }
   }
}

proc cacheCurrentModules {} {
   global g_loadedModules g_loadedModulesGeneric

   reportDebug "cacheCurrentModules"

   # mark specific as well as generic modules as loaded
   set i 0
   set filelist [getLoadedModuleFileList]
   foreach mod [getLoadedModuleList] {
      set g_loadedModules($mod) [lindex $filelist $i]
      set g_loadedModulesGeneric([file dirname $mod]) [file tail $mod]
      incr i
   }
}

# This proc resolves module aliases or version aliases to the real module name
# and version.
proc resolveModuleVersionOrAlias {name} {
   global g_moduleResolved

   if {[info exists g_moduleResolved($name)]} {
      set ret $g_moduleResolved($name)
   } else {
      set ret $name
   }

   reportDebug "resolveModuleVersionOrAlias: '$name' resolved to '$ret'"

   return $ret
}

proc charEscaped {str {charlist { \\\t\{\}|<>!;#^$&*"'`()}}} {
   return [regsub -all "\(\[$charlist\]\)" $str {\\\1}]
}

proc charUnescaped {str {charlist { \\\t\{\}|<>!;#^$&*"'`()}}} {
   return [regsub -all "\\\\\(\[$charlist\]\)" $str {\1}]
}

proc findExecutable {cmd} {
   foreach dir {/usr/X11R6/bin /usr/openwin/bin /usr/bin/X11} {
      if {[file executable "$dir/$cmd"]} {
         return "$dir/$cmd"
      }
   }

   return $cmd
}

proc getAbsolutePath {path} {
   global cwd
   # register pwd at first call
   if {![info exists cwd]} {
      set cwd [pwd]
   }

   set abslist {}
   # get a first version of the absolute path by joining the current working
   # directory to the given path. if given path is already absolute
   # 'file join' will not break it as $cwd will be ignored as soon a
   # beginning '/' character is found on $path. this first pass also clean
   # extra '/' character. then each element of the path is analyzed to clear
   # "." and ".." components.
   foreach elt [file split [file join $cwd $path]] {
      if {$elt eq ".."} {
         # skip ".." element if it comes after root element, remove last
         # element elsewhere
         if {[llength $abslist] > 1} {
            set abslist [lreplace $abslist end end]
         }
      # skip any "." element
      } elseif {$elt ne "."} {
         lappend abslist $elt
      }
   }

   # return cleaned absolute path
   return [eval file join $abslist]
}

# split string while ignore any separator character that is espaced
proc psplit {str sep} {
   set previdx -1
   set idx [string first $sep $str]
   while {$idx != -1} {
      # look ahead if found separator is escaped
      if {[string index $str [expr {$idx-1}]] ne "\\"} {
         # unescape any separator character when adding to list
         lappend res [charUnescaped [string range $str [expr {$previdx+1}]\
            [expr {$idx-1}]] $sep]
         set previdx $idx
      }
      set idx [string first $sep $str [expr {$idx+1}]]
   }

   lappend res [charUnescaped [string range $str [expr {$previdx+1}] end]\
      $sep]

   return $res
}

# join list while escape any character equal to separator
proc pjoin {lst sep} {
   set res ""

   foreach elt $lst {
      if {$res ne ""} {
         append res $sep
      }
      # escape any separator character when adding to string
      append res [charEscaped $elt $sep]
   }

   return $res
}

# Dictionary-style string comparison
# Use dictionary sort of lsort proc to compare two strings in the "string
# compare" fashion (returning -1, 0 or 1). Tcl dictionary-style comparison
# enables to compare software versions (ex: "1.10" is greater than "1.8")
proc stringDictionaryCompare {str1 str2} {
    if {$str1 eq $str2} {
        return 0
    # put both strings in a list, then lsort it and get last element
    } elseif {[lindex [lsort -dictionary [list $str1 $str2]] end] eq $str2} {
        return -1
    } else {
        return 1
    }
}

# provide a lreverse proc for Tcl8.4 and earlier
if {[info commands lreverse] eq ""} {
   proc lreverse l {
      set r {}
      set i [llength $l]
      while {[incr i -1] > 0} {
         lappend r [lindex $l $i]
      }
      lappend r [lindex $l 0]
   }
}

# provide a lassign proc for Tcl8.4 and earlier
if {[info commands lassign] eq ""} {
   proc lassign {values args} {
      uplevel 1 [list foreach $args [linsert $values end {}] break]
      lrange $values [llength $args] end
   }
}

proc replaceFromList {list1 item {item2 {}}} {
    while {[set xi [lsearch -exact $list1 $item]] >= 0} {
       if {[string length $item2] == 0} {
          set list1 [lreplace $list1 $xi $xi]
       } else {
          set list1 [lreplace $list1 $xi $xi $item2]
       }
    }

    return $list1
}

proc registerAccessIssue {modfile} {
   global g_access_mod_info errorCode

   # retrieve issue message
   if {[regexp {POSIX .* \{(.*)\}$} $errorCode match errMsg]} {
      set issue_info "[string totitle $errMsg] on"
   } else {
      set issue_info "Cannot access"
   }

   # register access issue
   set g_access_mod_info($modfile) "$issue_info '$modfile'"
}

proc checkValidModule {modfile} {
   reportDebug "checkValidModule: $modfile"

   # Check for valid module
   if {[catch {
      set fid [open $modfile r]
      set fheader [read $fid 8]
      close $fid
   }]} {
      registerAccessIssue $modfile
   } else {
      if {$fheader eq "\#%Module"} {
         return 1
      } else {
         global g_invalid_mod_info
         set g_invalid_mod_info($modfile) "Magic cookie '#%Module' missing in\
            '$modfile'"
      }
   }

   return 0
}

proc readModuleContent {modfile {report_read_issue 0} {must_have_cookie 1}} {
   reportDebug "readModuleContent: $modfile"

   # read file
   if {[catch {
      set fid [open $modfile r]
      set fdata [read $fid]
      close $fid
   } errMsg ]} {
      if {$report_read_issue} {
         reportInternalBug "Module $modfile cannot be read.\n$errMsg"
      }
      return {}
   }

   # check module validity if magic cookie is mandatory
   if {[string first "\#%Module" $fdata] == 0 || !$must_have_cookie} {
      return $fdata
   } else {
      reportInternalBug "Magic cookie '#%Module' missing in '$modfile'"
      return {}
   }
}

# If given module maps to default or other symbolic versions, a list of
# those versions is returned. This takes module/version as an argument.
proc getVersAliasList {mod} {
   global g_symbolHash

   if {[info exists g_symbolHash($mod)]} {
      set tag_list $g_symbolHash($mod)
   } else {
      set tag_list {}
   }

   reportDebug "getVersAliasList: '$mod' has tag list '$tag_list'"

   return $tag_list
}

# finds all module-related files matching mod in the module path dir
proc findModules {dir {mod {}} {fetch_mtime 0}} {
   global ignoreDir

   reportDebug "findModules: finding '$mod' in $dir\
      (fetch_mtime=$fetch_mtime)"

   # use catch protection to handle non-readable and non-existent dir
   if {[catch {
      # On Cygwin, glob may change the $dir path if there are symlinks
      # involved. So it is safest to reglob the $dir.
      # example:
      # [glob /home/stuff] -> "//homeserver/users0/stuff"
      set dir [glob $dir]
      set full_list [glob -nocomplain "$dir/$mod"]
   }]} {
      return {}
   }

   # remove trailing / needed on some platforms
   regsub {\/$} $full_list {} full_list

   set sstart [expr {[string length $dir] +1}]
   array set mod_list {}
   for {set i 0} {$i < [llength $full_list]} {incr i 1} {
      set element [lindex $full_list $i]
      set tag_list {}

      # Cygwin TCL likes to append ".lnk" to the end of symbolic links.
      # This is not necessary and pollutes the module names, so let's
      # trim it off.
      if { [isWin] } {
         regsub {\.lnk$} $element {} element
      }

      set tail [file tail $element]
      set direlem [file dirname $element]
      set modulename [string range $element $sstart end]
      set add_ref_to_parent 0
      if {[file isdirectory $element]} {
         if {![info exists ignoreDir($tail)]} {
            # try then catch any issue rather than test before trying
            # workaround 'glob -nocomplain' which does not return permission
            # error on Tcl 8.4, so we need to avoid registering issue if
            # raised error is about a no match
            set treat_dir 1
            if {[catch {set elt_list [glob "$element/*"]} errMsg]} {
               if {$errMsg eq "no files matched glob pattern\
                  \"$element/*\""} {
                  set elt_list {}
               } else {
                  registerAccessIssue $element
                  set treat_dir 0
               }
            }
            if {$treat_dir} {
               set mod_list($modulename) [list "directory"]
               # Add each element in the current directory to the list
               if {[file readable $element/.modulerc]} {
                  lappend full_list $element/.modulerc
               }
               if {[file readable $element/.version]} {
                  lappend full_list $element/.version
               }
               if {[llength $elt_list] > 0} {
                  set full_list [concat $full_list $elt_list]
               }
               set add_ref_to_parent 1
            }
         }
      } else {
         switch -glob -- $tail {
            {.modulerc} {
               set mod_list($modulename) [list "modulerc"]
            }
            {.version} {
               set mod_list($modulename) [list "modulerc"]
            }
            {.*} - {*~} - {*,v} - {\#*\#} { }
            default {
               if {[checkValidModule $element]} {
                  if {$fetch_mtime} {
                     set mtime [file mtime $element]
                  } else {
                     set mtime {}
                  }
                  set mod_list($modulename) [list "modulefile" $mtime]
                  set add_ref_to_parent 1
               }
            }
         }
      }

      # add reference to parent structure
      if {$add_ref_to_parent} {
         set parentname [file dirname $modulename]
         if {[info exists mod_list($parentname)]} {
            lappend mod_list($parentname) $tail
         }
      }
   }

   reportDebug "findModules: found [array names mod_list]"

   return [array get mod_list]
}

proc getModules {dir {mod {}} {fetch_mtime 0} {search {}} {exit_on_error 1}} {
   global ModulesCurrentModulefile
   global g_sourceAlias g_moduleVersion

   reportDebug "getModules: get '$mod' in $dir\
      (fetch_mtime=$fetch_mtime, search=$search)"

   # if search for global or user rc alias only, no dir lookup is performed
   # and aliases are looked from g_rcAlias array rather than g_moduleAlias
   if {$search eq "rc_alias_only"} {
      global g_rcAlias
      array set g_moduleAlias [array get g_rcAlias]
      array set found_list {}
   } else {
      global g_moduleAlias

      # find modules by searching with first path element if mod is a deep
      # modulefile (elt1/etl2/vers) in order to catch all .modulerc and
      # .version files of module-related parent directories in case we need
      # to translate an alias or a version
      set parentlist [split $mod "/"]
      set findmod [lindex $parentlist 0]
      # if searched mod is an empty or flat element append wildcard character
      # to match anything starting with mod
      if {$search eq "wild" && [llength $parentlist] <= 1} {
         append findmod "*"
      }
      array set found_list [findModules $dir $findmod $fetch_mtime]
   }

   array set mod_list {}
   foreach elt [lsort [array names found_list]] {
      if {[lindex $found_list($elt) 0] eq "modulerc"} {
         # push name to be found by module-alias and version
         pushSpecifiedName $elt
         pushModuleName $elt
         # set is needed for execute-modulerc
         set ModulesCurrentModulefile $dir/$elt
         execute-modulerc $ModulesCurrentModulefile $exit_on_error
         popModuleName
         popSpecifiedName
      # add other entry kind to the result list
      } elseif {[string match $mod* $elt]} {
         set mod_list($elt) $found_list($elt)
      }
   }

   # add aliases found when parsing .version or .modulerc files in this
   # directory (skip aliases not registered from this directory) if they
   # match passed $mod (as for regular modulefiles)
   foreach alias [array names g_moduleAlias -glob $mod*] {
      if {$dir eq "" || [string first $dir $g_sourceAlias($alias)] == 0} {
         set mod_list($alias) [list "alias" $g_moduleAlias($alias)]

         # add reference to this alias version in parent structure
         set parentname [file dirname $alias]
         if {[info exists mod_list($parentname)]} {
            lappend mod_list($parentname) [file tail $alias]
         }
      }
   }

   # add the target of symbolic versions found when parsing .version or
   # .modulerc files if these symbols match passed $mod (as for regular
   # modulefiles). modulefile target of these version symbol should have
   # been found previously to be added
   foreach vers [array names g_moduleVersion -glob $mod*] {
      set versmod $g_moduleVersion($vers)
      if {[info exists found_list($versmod)]\
         && ![info exists mod_list($versmod)]} {
         set mod_list($versmod) $found_list($versmod)
      }
   }

   reportDebug "getModules: got [array names mod_list]"

   return [array get mod_list]
}

# Finds all module versions for mod in the module path dir
proc listModules {dir mod {show_flags {1}} {filter {}} {search "wild"}} {
   global ignoreDir ModulesCurrentModulefile
   global flag_default_mf flag_default_dir show_modtimes

   reportDebug "listModules: get '$mod' in $dir\
      (show_flags=$show_flags, filter=$filter, search=$search)"

   # report flags for directories and modulefiles depending on show_flags
   # procedure argument and global variables
   if {$show_flags && $flag_default_mf} {
      set show_flags_mf 1
   } else {
      set show_flags_mf 0
   }
   if {$show_flags && $flag_default_dir} {
      set show_flags_dir 1
   } else {
      set show_flags_dir 0
   }
   if {$show_flags && $show_modtimes} {
      set show_mtime 1
   } else {
      set show_mtime 0
   }

   if {$filter eq "onlydefaults"} {
       # init a control list to correctly set implicit
       # or defined module default version
       set clean_defdefault {}
   }

   # get module list
   # as we treat a full directory content do not exit on an error
   # raised from one modulerc file
   array set mod_list [getModules $dir $mod $show_mtime $search 0]

   # prepare results for display
   set clean_list {}
   set clean_defname {}
   foreach elt [array names mod_list] {
      set elt_type [lindex $mod_list($elt) 0]
      if {$filter ne ""} {
         set elt_name [file dirname $elt]
         if {$elt_name eq "."} {
            set elt_name $elt
         }
      }

      set tag_list [getVersAliasList $elt]
      set add_to_clean_list 1
      # add to list only if it is the default set
      # or if it is an implicit default when no default is set
      if {$filter eq "onlydefaults"} {
         # do not add element if a default has already
         # been added for this module
         if {[lsearch -exact $clean_defdefault $elt_name] == -1} {
            set clean_elt_idx [lsearch -exact $clean_defname $elt_name]
            # only one element has to be set for this module
            # so replace previously existing element
            if {$clean_elt_idx >= 0} {
               # only replace if new occurency is greater than
               # existing one or if new occurency is the default set
               # add a dir only if it holds default tag
               if {([stringDictionaryCompare $elt [lindex $clean_list\
                  $clean_elt_idx]] == 1 && $elt_type ne "directory") \
                  || [lsearch $tag_list "default"] >= 0} {
                  set clean_list [lreplace $clean_list \
                     $clean_elt_idx $clean_elt_idx]
                  set clean_defname [lreplace $clean_defname \
                     $clean_elt_idx $clean_elt_idx]
               } else {
                  set add_to_clean_list 0
               }
            # add dir only if it holds default tag
            } elseif {$elt_type eq "directory" \
               && [lsearch $tag_list "default"] == -1} {
               set add_to_clean_list 0
            }

            # if default is defined add to control list
            if {[lsearch $tag_list "default"] >= 0} {
               lappend clean_defdefault $elt_name
            }

            if {$add_to_clean_list} {
               lappend clean_defname $elt_name
            }
         } else {
            set add_to_clean_list 0
         }
      } elseif {$filter eq "onlylatest"} {
         # add latest version to list only (no directory in this case as we
         # compare version number between each other
         if {$elt_type eq "directory"} {
            set add_to_clean_list 0
         } else {
            set clean_elt_idx [lsearch -exact $clean_defname $elt_name]

            # only one element has to be set for this module
            # so replace previously existing element and only
            # if new occurency is greater than existing one
            if {$clean_elt_idx >= 0 && [stringDictionaryCompare $elt \
               [lindex $clean_list $clean_elt_idx]] == 1} {
               set clean_list [lreplace $clean_list \
                  $clean_elt_idx $clean_elt_idx]
               set clean_defname [lreplace $clean_defname \
                  $clean_elt_idx $clean_elt_idx]
            } elseif {$clean_elt_idx != -1} {
               set add_to_clean_list 0
            }
         }

         if {$add_to_clean_list} {
            lappend clean_defname $elt_name
         }
      # do not add a dir if it does not hold tags
      } elseif {$elt_type eq "directory" && [llength $tag_list] == 0} {
         set add_to_clean_list 0
      }

      if {$add_to_clean_list} {
         switch -- $elt_type {
            {directory} {
               if {$show_flags_dir} {
                  if {$show_mtime} {
                     lappend clean_list [format "%-40s%-20s" $elt\
                        [join $tag_list ":"]]
                  } else {
                     lappend clean_list [join [list $elt "("\
                        [join $tag_list ":"] ")"] {}]
                  }
               } else {
                  lappend clean_list $elt
               }
            }
            {modulefile} {
               if {$show_mtime} {
                  # add to display file modification time in addition
                  # to potential tags
                  lappend clean_list [format "%-40s%-20s%10s" $elt\
                     [join $tag_list ":"]\
                     [clock format [lindex $mod_list($elt) 1]\
                     -format "%Y/%m/%d %H:%M:%S"]]
               } elseif {$show_flags_mf && [llength $tag_list] > 0} {
                  lappend clean_list [join [list $elt "("\
                     [join $tag_list ":"] ")"] {}]
               } else {
                  lappend clean_list $elt
               }
            }
            {alias} {
               if {$show_mtime} {
                  lappend clean_list [format "%-40s%-20s"\
                     "$elt -> [lindex $mod_list($elt) 1]"\
                     [join $tag_list ":"]]
               } elseif {$show_flags_mf} {
                  lappend tag_list "@"
                  lappend clean_list [join [list $elt "("\
                     [join $tag_list ":"] ")"] {}]
               } else {
                  lappend clean_list $elt
               }
            }
         }
      }
   }

   # always dictionary-sort results
   set clean_list [lsort -dictionary $clean_list]
   reportDebug "listModules: Returning $clean_list"

   return $clean_list
}

proc showModulePath {} {
   reportDebug "showModulePath"

   set modpathlist [getModulePathList]
   if {[llength $modpathlist] > 0} {
      report "Search path for module files (in search order):"
      foreach path $modpathlist {
         report "  $path"
      }
   } else {
      reportWarning "No directories on module search path"
   }
}

proc displaySeparatorLine {{title {}}} {
   global DEF_COLUMNS

   if {$title eq ""} {
      report "[string repeat {-} 67]"
   } else {
      set len  [string length $title]
      # max expr function is not supported in Tcl8.4 and earlier
      if {[set lrep [expr {($DEF_COLUMNS - $len - 2)/2}]] < 1} {
         set lrep 1
      }
      if {[set rrep [expr {$DEF_COLUMNS - $len - 2 - $lrep}]] < 1} {
         set rrep 1
      }
      report "[string repeat {-} $lrep] $title [string repeat {-} $rrep]"
   }
}

# get a list of elements and print them in a column or in a
# one-per-line fashion
proc displayElementList {header one_per_line display_idx args} {
   global DEF_COLUMNS

   set elt_cnt [llength $args]
   reportDebug "displayElementList: header=$header, elt_cnt=$elt_cnt,\
      one_per_line=$one_per_line, display_idx=$display_idx"

   # display header if any provided
   if {$header ne "noheader"} {
      displaySeparatorLine $header
   }

   # end proc if no element are to print
   if {$elt_cnt == 0} {
      return
   }

   # display one element per line
   if {$one_per_line} {
      if {$display_idx} {
         set idx 1
         foreach elt $args {
            report [format "%2d) %s " $idx $elt]
            incr idx
         }
      } else {
         foreach elt $args {
            report $elt
         }
      }
   # elsewhere display elements in columns
   } else {
      if {$display_idx} {
         # save room for numbers and spacing: 2 digits + ) + space
         set elt_prefix_len 4
      } else {
         set elt_prefix_len 0
      }
      # save room for two spaces after element
      set elt_suffix_len 2

      # compute rows*cols grid size with optimized column number
      # the size of each column is computed to display as much column
      # as possible on each line
      set max_len 0
      foreach arg $args {
         lappend elt_len [set len [expr {[string length $arg] + $elt_suffix_len}]]
         if {$len > $max_len} {
            set max_len $len
         }
      }

      # find valid grid by starting with non-optimized solution where each
      # column length is equal to the length of the biggest element to display
      set cur_cols [expr {int($DEF_COLUMNS / $max_len)}]
      # when display is found too short to display even one column
      if {$cur_cols == 0} {
         set cols 1
         set rows $elt_cnt
         array set col_width [list 0 $max_len]
      } else {
         set cols 0
      }
      set last_round 0
      set restart_loop 0
      while {$cur_cols > $cols} {
         if {!$restart_loop} {
            if {$last_round} {
               incr cur_rows
            } else {
               set cur_rows [expr {int(ceil(double($elt_cnt) / $cur_cols))}]
            }
            for {set i 0} {$i < $cur_cols} {incr i} {
               set cur_col_width($i) 0
            }
            for {set i 0} {$i < $cur_rows} {incr i} {
               set row_width($i) 0
            }
            set istart 0
         } else {
            set istart [expr {$col * $cur_rows}]
            # only remove width of elements from current col
            for {set row 0} {$row < ($i % $cur_rows)} {incr row} {
               incr row_width($row) -[expr {$pre_col_width + $elt_prefix_len}]
            }
         }
         set restart_loop 0
         for {set i $istart} {$i < $elt_cnt} {incr i} {
            set col [expr {int($i / $cur_rows)}]
            set row [expr {$i % $cur_rows}]
            # restart loop if a column width change
            if {[lindex $elt_len $i] > $cur_col_width($col)} {
               set pre_col_width $cur_col_width($col)
               set cur_col_width($col) [lindex $elt_len $i]
               set restart_loop 1
               break
            }
            # end search of maximum number of columns if computed row width
            # is larger than terminal width
            if {[incr row_width($row) +[expr {$cur_col_width($col) \
               + $elt_prefix_len}]] > $DEF_COLUMNS} {
               # start last optimization pass by increasing row number until
               # reaching number used for previous column number, by doing so
               # this number of column may pass in terminal width, if not
               # fallback to previous number of column
               if {$last_round && $cur_rows == $rows} {
                  incr cur_cols -1
               } else {
                  set last_round 1
               }
               break
            }
         }
         # went through all elements without reaching terminal width limit so
         # this number of column solution is valid, try next with a greater
         # column number
         if {$i == $elt_cnt} {
            set cols $cur_cols
            set rows $cur_rows
            array set col_width [array get cur_col_width]
            # number of column is fixed if last optimization round has started
            # reach end also if there is only one row of results
            if {!$last_round && $rows > 1} {
               incr cur_cols
            }
         }

      }
      reportDebug "displayElementList: list=$args"
      reportDebug "displayElementList: rows/cols=$rows/$cols,\
         lastcol_item_cnt=[expr {int($elt_cnt % $rows)}]"

      for {set row 0} {$row < $rows} {incr row} {
         for {set col 0} {$col < $cols} {incr col} {
            set index [expr {$col * $rows + $row}]
            if {$index < $elt_cnt} {
               if {$display_idx} {
                  append displist [format "%2d) %-$col_width($col)s"\
                     [expr {$index +1}] [lindex $args $index]]
               } else {
                  append displist [format "%-$col_width($col)s"\
                     [lindex $args $index]]
               }
            }
         }
         append displist "\n"
      }
      report "$displist" -nonewline
   }
}

# build list of what to undo then do to move
# from an initial list to a target list
proc getMovementBetweenList {from to} {
   reportDebug "getMovementBetweenList: from($from) to($to)"

   set undo {}
   set do {}

   # determine what element to undo then do
   # to restore a target list from a current list
   # with preservation of the element order
   if {[llength $to] > [llength $from]} {
      set imax [llength $to]
   } else {
      set imax [llength $from]
   }
   set list_equal 1
   for {set i 0} {$i < $imax} {incr i} {
      set to_obj [lindex $to $i]
      set from_obj [lindex $from $i]

      if {$to_obj ne $from_obj} {
         set list_equal 0
      }
      if {$list_equal == 0} {
         if {$to_obj ne ""} {
            lappend do $to_obj
         }
         if {$from_obj ne ""} {
            lappend undo $from_obj
         }
      }
   }

   return [list $undo $do]
}

# build list of currently loaded modules where modulename
# is registered minus module version if loaded version is
# the default one. a helper list may be provided and looked
# at if no module path is set
proc getSimplifiedLoadedModuleList {{helper_raw_list {}}\
   {helper_list {}}} {
   reportDebug "getSimplifiedLoadedModuleList"

   set curr_mod_list {}
   set modpathlist [getModulePathList]
   foreach mod [getLoadedModuleList] {
      if {[string length $mod] > 0} {
         set modparent [file dirname $mod]
         if {$modparent eq "."} {
            lappend curr_mod_list $mod
         } elseif {[llength $modpathlist] > 0} {
            # fetch all module version available
            set modlist {}
            foreach dir $modpathlist {
               if {[file isdirectory $dir]} {
                  set modlist [listModules $dir $modparent 0 "onlydefaults"]
                  # quit loop if result found
                  if {[llength $modlist] > 0} {
                     break
                  }
               }
            }
            # check if loaded version is default
            if {[lsearch -exact $modlist $mod] >-1 } {
               lappend curr_mod_list $modparent
            } else {
               lappend curr_mod_list $mod
            }
         } else {
            # if no path set currently, cannot search for all
            # available version so use helper lists if provided
            set helper_idx [lsearch -exact $helper_raw_list $mod]
            if {$helper_idx == -1} {
               lappend curr_mod_list $mod
            } else {
               # if mod found in a previous LOADEDMODULES list use
               # simplified version of this module found in relative
               # helper list (previously computed simplified list)
               lappend curr_mod_list [lindex $helper_list $helper_idx]
            }
         }
      }
   }

   return $curr_mod_list
}

# get collection target currently set if any.
# a target is a domain on which a collection is only valid.
# when a target is set, only the collections made for that target
# will be available to list and restore, and saving will register
# the target footprint
proc getCollectionTarget {} {
   global env

   if {[info exists env(MODULES_COLLECTION_TARGET)]} {
      return $env(MODULES_COLLECTION_TARGET)
   } else {
      return ""
   }
}

# get filename corresponding to collection name provided as argument.
# name provided may already be a file name. a variable name may also be
# provided to get back collection description (with target info if any)
proc getCollectionFilename {coll {descvar}} {
   global env

   # initialize description with collection name
   # if description variable is set
   if {[info exists descvar]} {
      uplevel 1 set $descvar $coll
   }

   # is collection a filepath
   if {[string first "/" $coll] > -1} {
      # collection target has no influence when
      # collection is specified as a filepath
      set collfile "$coll"
   # elsewhere collection is a name
   } elseif {[info exists env(HOME)]} {
      set collfile "$env(HOME)/.module/$coll"
      # if a target is set, append the suffix corresponding
      # to this target to the collection file name
      set colltarget [getCollectionTarget]
      if {$colltarget ne ""} {
         append collfile ".$colltarget"
         # add knowledge of collection target on description
         if {[info exists descvar]} {
            uplevel 1 append $descvar \" (for target \\"$colltarget\\")\"
         }
      }
   } else {
      reportErrorAndExit "HOME not defined"
   }

   return $collfile
}

# generate collection content based on provided path and module lists
proc formatCollectionContent {path_list mod_list} {
   set content ""

   # start collection content with modulepaths
   foreach path $path_list {
      # 'module use' prepends paths by default so we clarify
      # path order here with --append flag
      append content "module use --append $path" "\n"
   }

   # then add modules
   foreach mod $mod_list {
      append content "module load $mod" "\n"
   }

   return $content
}

# read given collection file and return the path and module lists it defines
proc readCollectionContent {collfile} {
   # init lists (maybe coll does not set mod to load)
   set path_list {}
   set mod_list {}

   # read file
   if {[catch {
      set fid [open $collfile r]
      set fdata [split [read $fid] "\n"]
      close $fid
   } errMsg ]} {
      reportErrorAndExit "Collection $collfile cannot be read.\n$errMsg"
   }

   # analyze collection content
   foreach fline $fdata {
      if {[regexp {module use (.*)$} $fline match patharg] == 1} {
         # paths are appended by default
         set stuff_path "append"
         # manage with "split" multiple paths and path options
         # specified on single line, for instance:
         # module use --append path1 path2 path3
         foreach path [split $patharg] {
            # following path is asked to be appended
            if {($path eq "--append") || ($path eq "-a")\
               || ($path eq "-append")} {
               set stuff_path "append"
            # following path is asked to be prepended
            # collection generated with 'save' does not prepend
            } elseif {($path eq "--prepend") || ($path eq "-p")\
               || ($path eq "-prepend")} {
               set stuff_path "prepend"
            } else {
               # ensure given path is absolute to be able to correctly
               # compare with paths registered in MODULEPATH
               set path [getAbsolutePath $path]
               # add path to end of list
               if {$stuff_path eq "append"} {
                  lappend path_list $path
               # insert path to first position
               } else {
                  set path_list [linsert $path_list 0 $path]
               }
            }
         }
      } elseif {[regexp {module load (.*)$} $fline match modarg] == 1} {
         # manage multiple modules specified on a
         # single line with "split", for instance:
         # module load mod1 mod2 mod3
         set mod_list [concat $mod_list [split $modarg]]
      }
   }

   return [list $path_list $mod_list]
}


########################################################################
# command line commands
#
proc cmdModuleList {} {
   global show_oneperline show_modtimes
   global g_loadedModules

   set loadedmodlist [getLoadedModuleList]

   if {[llength $loadedmodlist] == 0} {
      report "No Modulefiles Currently Loaded."
   } else {
      set list {}
      if {$show_modtimes} {
         report "- Package -----------------------------.- Versions\
            --------.- Last mod. -------"
      }
      report "Currently Loaded Modulefiles:"
      set display_list {}
      if {$show_modtimes || $show_oneperline} {
         set display_idx 0
         set one_per_line 1
      } else {
         set display_idx 1
         set one_per_line 0
      }

      foreach mod $loadedmodlist {
         if {[string length $mod] > 0} {
            if {$show_oneperline} {
               lappend display_list $mod
            } else {
               # call getModules to find and execute rc files for this module
               set dir [string range $g_loadedModules($mod) 0\
                  end-[expr {[string length $mod] +1}]]
               array set mod_list [getModules $dir $mod $show_modtimes]
               set tag_list [getVersAliasList $mod]

               if {$show_modtimes} {
                  # add to display file modification time in addition
                  # to potential tags
                  lappend display_list [format "%-40s%-20s%10s" $mod\
                     [join $tag_list ":"]\
                     [clock format [lindex $mod_list($mod) 1]\
                     -format "%Y/%m/%d %H:%M:%S"]]
               } else {
                  if {[llength $tag_list]} {
                     append mod "(" [join $tag_list ":"] ")"
                  }
                  lappend display_list $mod
               }
            }
         }
      }

      eval displayElementList "noheader" $one_per_line $display_idx\
         $display_list
   }
}

proc cmdModuleDisplay {mod} {
   pushMode "display"
   lassign [getPathToModule $mod] modfile modname
   if {$modfile ne ""} {
      pushSpecifiedName $mod
      pushModuleName $modname
      displaySeparatorLine
      report "$modfile:\n"
      execute-modulefile $modfile
      popModuleName
      popSpecifiedName
      displaySeparatorLine
   }
   popMode
}

proc cmdModulePaths {mod} {
   global g_pathList

   reportDebug "cmdModulePaths: ($mod)"

   foreach dir [getModulePathList "exiterronundef"] {
      array unset mod_list
      array set mod_list [getModules $dir $mod]

      # build list of modulefile to print
      foreach elt [array names mod_list] {
         switch -- [lindex $mod_list($elt) 0] {
            {modulefile} {
               lappend g_pathList $dir/$elt
            }
            {alias} {
               # resolve alias target
               set aliastarget [lindex $mod_list($elt) 1]
               lassign [getPathToModule $aliastarget $dir]\
                  modfile modname
               # add module target as result instead of alias
               if {$modfile eq "$dir/$modname"\
                  && ![info exists mod_list($modname)]} {
                  lappend g_pathList $modfile
               }
            }
         }
      }
   }

   # sort results if any
   if {[info exists g_pathList]} {
      set g_pathList [lsort -dictionary $g_pathList]
   }
}

proc cmdModulePath {mod} {
   global g_pathList

   reportDebug "cmdModulePath: ($mod)"
   lassign [getPathToModule $mod] modfile modname
   if {$modfile ne ""} {
      set g_pathList $modfile
   }
}

proc cmdModuleWhatIs {{mod {}}} {
   cmdModuleSearch $mod {}
}

proc cmdModuleApropos {{search {}}} {
   cmdModuleSearch {} $search
}

proc cmdModuleSearch {{mod {}} {search {}}} {
   global g_whatis g_inhibit_errreport

   reportDebug "cmdModuleSearch: ($mod, $search)"

   # disable error reporting to avoid modulefile errors
   # to mix with valid search results
   inhibitErrorReport

   if {$mod eq ""} {
      set searchmod "wild"
   } else {
      set searchmod {}
   }
   set foundmod 0
   pushMode "whatis"
   foreach dir [getModulePathList "exiterronundef"] {
      array unset mod_list
      array set mod_list [getModules $dir $mod 0 $searchmod 0]
      array unset interp_list
      array set interp_list {}

      # build list of modulefile to interpret
      foreach elt [array names mod_list] {
         switch -- [lindex $mod_list($elt) 0] {
            {modulefile} {
               set interp_list($elt) $dir/$elt
            }
            {alias} {
               # resolve alias target
               set aliastarget [lindex $mod_list($elt) 1]
               lassign [getPathToModule $aliastarget $dir]\
                  modfile modname
               # add module target as result instead of alias
               if {$modfile eq "$dir/$modname"\
                  && ![info exists mod_list($modname)]} {
                  set interp_list($modname) $modfile
               }
            }
         }
      }

      if {[array size interp_list] > 0} {
         set foundmod 1
         set display_list {}
         # interpret every modulefiles obtained to get their whatis text
         foreach elt [lsort -dictionary [array names interp_list]] {
            set g_whatis {}
            pushSpecifiedName $elt
            pushModuleName $elt
            # as we treat a full directory content do not exit on an error
            # raised from one modulefile
            execute-modulefile $interp_list($elt) 0
            popModuleName
            popSpecifiedName

            # treat whatis as a multi-line text
            if {$search eq "" || [regexp -nocase $search $g_whatis]} {
               foreach line $g_whatis {
                  lappend display_list [format "%20s: %s" $elt $line]
               }
            }
         }

         if {[llength $display_list] > 0} {
            eval displayElementList $dir 1 0 $display_list
         }
      # if looking for a specific mod and no modulefile to interpret
      # call for a resolution of mod in current searched dir to get any
      # access error message
      } elseif {$mod ne ""} {
         reenableErrorReport
         lassign [getPathToModule $mod $dir] modfile modissue
         inhibitErrorReport
         # mod was found but issue occurs
         if {$modissue} {
            set foundmod 1
         }
      }
   }
   popMode

   reenableErrorReport

   # report error if a modulefile was searched but not found
   if {$mod ne "" && !$foundmod} {
      # if an alias was found for searched mod but no modulefile can be
      # associated to this alias, report locate error on this defined target
      if {[info exists aliastarget]} {
         set mod $aliastarget
      }
      reportError "Unable to locate a modulefile for '$mod'"
   }
}

proc cmdModuleSwitch {old {new {}}} {
   global g_loadedModulesGeneric g_loadedModules

   if {$new eq ""} {
      set new $old
      # find closest module to unload if any
      set old [getLoadedWithClosestName [resolveModuleVersionOrAlias $old]]
   }

   if {![info exists g_loadedModules($old)] &&
      [info exists g_loadedModulesGeneric($old)]} {
      set old "$old/$g_loadedModulesGeneric($old)"
   }

   reportDebug "cmdModuleSwitch: new=\"$new\" old=\"$old\""

   cmdModuleUnload $old
   cmdModuleLoad $new
}

proc cmdModuleSave {{coll {}}} {
   # default collection used if no name provided
   if {$coll eq ""} {
      set coll "default"
   }
   reportDebug "cmdModuleSave: $coll"

   # format collection content
   set save [formatCollectionContent [getModulePathList] \
      [getSimplifiedLoadedModuleList]]

   if { [string length $save] == 0} {
      reportErrorAndExit "Nothing to save in a collection"
   }

   # get coresponding filename and its directory
   set collfile [getCollectionFilename $coll colldesc]
   set colldir [file dirname $collfile]

   if {![file exists $colldir]} {
      reportDebug "cmdModuleSave: Creating $colldir"
      file mkdir $colldir
   } elseif {![file isdirectory $colldir]} {
      reportErrorAndExit "$colldir exists but is not a directory"
   }

   reportDebug "cmdModuleSave: Saving $collfile"

   if {[catch {
      set fid [open $collfile w]
      puts $fid $save
      close $fid
   } errMsg ]} {
      reportErrorAndExit "Collection $colldesc cannot be saved.\n$errMsg"
   }
}

proc cmdModuleRestore {{coll {}}} {
   # default collection used if no name provided
   if {$coll eq ""} {
      set coll "default"
   }
   reportDebug "cmdModuleRestore: $coll"

   # get coresponding filename
   set collfile [getCollectionFilename $coll colldesc]

   if {![file readable $collfile]} {
      reportErrorAndExit "Collection $colldesc does not exist or is not\
         readable"
   }

   # read collection
   lassign [readCollectionContent $collfile] coll_path_list coll_mod_list

   # collection should at least define a path
   if {[llength $coll_path_list] == 0} {
      reportErrorAndExit "$colldesc is not a valid collection"
   }

   # fetch what is currently loaded
   set curr_path_list [getModulePathList]
   # get current loaded module list in simplified and raw versions
   # these lists may be used later on, see below
   set curr_mod_list_raw [getLoadedModuleList]
   set curr_mod_list [getSimplifiedLoadedModuleList]

   # determine what module to unload to restore collection
   # from current situation with preservation of the load order
   lassign [getMovementBetweenList $curr_mod_list $coll_mod_list] \
      mod_to_unload mod_to_load

   # proceed as well for modulepath
   lassign [getMovementBetweenList $curr_path_list $coll_path_list] \
      path_to_unuse path_to_use

   # unload modules
   if {[llength $mod_to_unload] > 0} {
      eval cmdModuleUnload [lreverse $mod_to_unload]
   }
   # unuse paths
   if {[llength $path_to_unuse] > 0} {
      eval cmdModuleUnuse [lreverse $path_to_unuse]
   }

   # since unloading a module may unload other modules or
   # paths, what to load/use has to be determined after
   # the undo phase, so current situation is fetched again
   set curr_path_list [getModulePathList]

   # here we may be in a situation were no more path is left
   # in module path, so we cannot easily compute the simplified loaded
   # module list. so we provide two helper lists: simplified and raw
   # versions of the loaded module list computed before starting to
   # unload modules. these helper lists may help to learn the
   # simplified counterpart of a loaded module if it was already loaded
   # before starting to unload modules
   set curr_mod_list [getSimplifiedLoadedModuleList\
      $curr_mod_list_raw $curr_mod_list]

   # determine what module to load to restore collection
   # from current situation with preservation of the load order
   lassign [getMovementBetweenList $curr_mod_list $coll_mod_list] \
      mod_to_unload mod_to_load

   # proceed as well for modulepath
   lassign [getMovementBetweenList $curr_path_list $coll_path_list] \
      path_to_unuse path_to_use

   # use paths
   if {[llength $path_to_use] > 0} {
      # always append path here to guaranty the order
      # computed above in the movement lists
      eval cmdModuleUse --append $path_to_use
   }

   # load modules
   if {[llength $mod_to_load] > 0} {
      eval cmdModuleLoad $mod_to_load
   }
}

proc cmdModuleSaverm {{coll {}}} {
   # default collection used if no name provided
   if {$coll eq ""} {
      set coll "default"
   }
   reportDebug "cmdModuleSaverm: $coll"

   # avoid to remove any kind of file with this command
   if {[string first "/" $coll] > -1} {
      reportErrorAndExit "Command does not remove collection specified as\
         filepath"
   }

   # get coresponding filename
   set collfile [getCollectionFilename $coll colldesc]

   if {![file exists $collfile]} {
      reportErrorAndExit "Collection $colldesc does not exist"
   }

   # attempt to delete specified colletion
   if {[catch {
      file delete $collfile
   } errMsg ]} {
      reportErrorAndExit "Collection $colldesc cannot be removed.\n$errMsg"
   }
}

proc cmdModuleSaveshow {{coll {}}} {
   # default collection used if no name provided
   if {$coll eq ""} {
      set coll "default"
   }
   reportDebug "cmdModuleSaveshow: $coll"

   # get coresponding filename
   set collfile [getCollectionFilename $coll colldesc]

   if {![file readable $collfile]} {
      reportErrorAndExit "Collection $colldesc does not exist or is not\
         readable"
   }

   # read collection
   lassign [readCollectionContent $collfile] coll_path_list coll_mod_list

   # collection should at least define a path
   if {[llength $coll_path_list] == 0} {
      reportErrorAndExit "$colldesc is not a valid collection"
   }

   displaySeparatorLine
   report "$collfile:\n"
   report [formatCollectionContent $coll_path_list $coll_mod_list]
   displaySeparatorLine
}

proc cmdModuleSavelist {} {
   global env show_oneperline show_modtimes

   # if a target is set, only list collection matching this
   # target (means having target as suffix in their name)
   set colltarget [getCollectionTarget]
   if {$colltarget ne ""} {
      set suffix ".$colltarget"
      set targetdesc " (for target \"$colltarget\")"
   } else {
      set suffix ""
      set targetdesc ""
   }

   reportDebug "cmdModuleSavelist: list collections for target\
      \"$colltarget\""

   # list saved collections (matching target suffix)
   set coll_list [glob -nocomplain -- "$env(HOME)/.module/*$suffix"]

   if { [llength $coll_list] == 0} {
      report "No named collection$targetdesc."
   } else {
      set list {}
      if {$show_modtimes} {
         report "- Collection\
            ----------------------------------------------.- Last mod. ------"
      }
      report "Named collection list$targetdesc:"
      set display_list {}
      if {$show_modtimes || $show_oneperline} {
         set display_idx 0
         set one_per_line 1
      } else {
         set display_idx 1
         set one_per_line 0
      }

      foreach coll [lsort -dictionary $coll_list] {
         # remove target suffix from names to display
         regsub "$suffix$" [file tail $coll] {} mod

         if {[string length $mod] > 0} {
            if {$show_modtimes} {
               set filetime [clock format [file mtime $coll]\
                  -format "%Y/%m/%d %H:%M:%S"]
               lappend display_list [format "%-60s%10s" $mod $filetime]
            } else {
               lappend display_list $mod
            }
         }
      }

      eval displayElementList "noheader" $one_per_line $display_idx\
         $display_list
   }
}


proc cmdModuleSource {args} {
   reportDebug "cmdModuleSource: $args"
   foreach file $args {
      if {[file exists $file]} {
         pushMode "load"
         pushSpecifiedName $file
         pushModuleName $file
         # relax constraint of having a magic cookie at the start of the
         # modulefile to execute as sourced files may need more flexibility
         # as they may be managed outside of the modulefile environment like
         # the initialization modulerc file
         execute-modulefile $file 1 0
         popModuleName
         popSpecifiedName
         popMode
      } else {
         reportErrorAndExit "File $file does not exist"
      }
   }
}

proc cmdModuleLoad {args} {
   global g_loadedModules g_loadedModulesGeneric g_force
   global ModulesCurrentModulefile

   reportDebug "cmdModuleLoad: loading $args"

   pushMode "load"
   foreach mod $args {
      lassign [getPathToModule $mod] modfile modname
      if {$modfile ne ""} {
         set currentModule $modname
         set ModulesCurrentModulefile $modfile

         if {$g_force || ! [info exists g_loadedModules($currentModule)]} {
            pushSpecifiedName $mod
            pushModuleName $currentModule
            pushSettings

            if {[execute-modulefile $modfile]} {
               restoreSettings
            } else {
               append-path LOADEDMODULES $currentModule
               append-path _LMFILES_ $modfile

               set g_loadedModules($currentModule) $modfile
               set genericModName [file dirname $currentModule]

               reportDebug "cmdModuleLoad: genericModName = $genericModName"

               if {![info exists\
                  g_loadedModulesGeneric($genericModName)]} {
                     set g_loadedModulesGeneric($genericModName) [file tail\
                        $currentModule]
               }
            }

            popSettings
            popModuleName
            popSpecifiedName
         }
      }
   }
   popMode
}

proc cmdModuleUnload {args} {
   global g_loadedModules g_loadedModulesGeneric
   global ModulesCurrentModulefile g_def_separator

   reportDebug "cmdModuleUnload: unloading $args"

   pushMode "unload"
   foreach mod $args {
      if {[catch {
         lassign [getPathToModule $mod] modfile modname
         if {$modfile ne ""} {
            set currentModule $modname
            set ModulesCurrentModulefile $modfile

            if {[info exists g_loadedModules($currentModule)]} {
               pushSpecifiedName $mod
               pushModuleName $currentModule
               pushSettings

               if {[execute-modulefile $modfile]} {
                  restoreSettings
               } else {
                  unload-path LOADEDMODULES $currentModule\
                     $g_def_separator
                  unload-path _LMFILES_ $modfile $g_def_separator
                  unset g_loadedModules($currentModule)

                  if {[info exists g_loadedModulesGeneric([file dirname\
                     $currentModule])]} {
                     unset g_loadedModulesGeneric([file dirname\
                        $currentModule])
                  }
               }

               popSettings
               popModuleName
               popSpecifiedName
            }
         } else {
            if {[info exists g_loadedModulesGeneric($mod)]} {
               set mod "$mod/$g_loadedModulesGeneric($mod)"
            }
            unload-path LOADEDMODULES $mod $g_def_separator
            unload-path _LMFILES_ $modfile $g_def_separator

            if {[info exists g_loadedModules($mod)]} {
               unset g_loadedModules($mod)
            }
            if {[info exists g_loadedModulesGeneric([file dirname $mod])]} {
               unset g_loadedModulesGeneric([file dirname $mod])
            }
         }
      } errMsg ]} {
         reportError "module unload $mod failed.\n$errMsg"
      }
   }
   popMode
}

proc cmdModulePurge {} {
   reportDebug "cmdModulePurge"

   eval cmdModuleUnload [lreverse [getLoadedModuleList]]
}

proc cmdModuleReload {} {
   reportDebug "cmdModuleReload"

   set list [getLoadedModuleList]
   set rlist [lreverse $list]
   foreach mod $rlist {
      cmdModuleUnload $mod
   }
   foreach mod $list {
      cmdModuleLoad $mod
   }
}

proc cmdModuleAliases {} {
   global g_moduleAlias g_moduleVersion

   # disable error reporting to avoid modulefile errors
   # to mix with avail results
   inhibitErrorReport

   # parse paths to fill g_moduleAlias and g_moduleVersion if empty
   if {[array size g_moduleAlias] == 0 \
      && [array size g_moduleVersion] == 0 } {
      foreach dir [getModulePathList "exiterronundef"] {
         getModules $dir "" 0 "" 0
      }
   }

   reenableErrorReport

   set display_list {}
   foreach name [lsort -dictionary [array names g_moduleAlias]] {
      lappend display_list "$name -> $g_moduleAlias($name)"
   }
   if {[llength $display_list] > 0} {
      eval displayElementList "Aliases" 1 0 $display_list
   }

   set display_list {}
   foreach name [lsort -dictionary [array names g_moduleVersion]] {
      lappend display_list "$name -> $g_moduleVersion($name)"
   }
   if {[llength $display_list] > 0} {
      eval displayElementList "Versions" 1 0 $display_list
   }
}

proc cmdModuleAvail {{mod {*}}} {
   global show_oneperline show_modtimes show_filter

   if {$show_modtimes || $show_oneperline} {
      set one_per_line 1
   } else {
      set one_per_line 0
   }

   # disable error reporting to avoid modulefile errors
   # to mix with avail results
   inhibitErrorReport

   if {$show_modtimes} {
      report "- Package/Alias -----------------------.- Versions --------.-\
         Last mod. -------"
   }

   # look if aliases have been defined in the global or user-specific
   # modulerc and display them if any in a dedicated list
   set display_list [listModules "" "$mod" 1 $show_filter "rc_alias_only"]
   if {[llength $display_list] > 0} {
      eval displayElementList "{global/user modulerc}" $one_per_line 0\
         $display_list
   }

   foreach dir [getModulePathList "exiterronundef"] {
      set display_list [listModules "$dir" "$mod" 1 $show_filter]
      if {[llength $display_list] > 0} {
         eval displayElementList $dir $one_per_line 0 $display_list
      }
   }

   reenableErrorReport
}

proc cmdModuleUse {args} {
   global g_def_separator

   reportDebug "cmdModuleUse: $args"

   if {$args eq ""} {
      showModulePath
   } else {
      set stuff_path "prepend"
      foreach path $args {
         if {($path eq "--append") ||($path eq "-a") ||($path eq\
            "-append")} {
            set stuff_path "append"
         }\
         elseif {($path eq "--prepend") ||($path eq "-p") ||($path eq\
            "-prepend")} {
            set stuff_path "prepend"
         }\
         elseif {[file isdirectory $path]} {
            # tranform given path in an absolute path to avoid dependency to
            # the current work directory.
            set path [getAbsolutePath $path]

            reportDebug "cmdModuleUse: calling add-path \
               MODULEPATH $path $stuff_path $g_def_separator"

            pushMode "load"
            catch {
               add-path MODULEPATH $path $stuff_path $g_def_separator
            }

            popMode
         } else {
            reportError "Directory '$path' not found"
         }
      }
   }
}

proc cmdModuleUnuse {args} {
   global g_def_separator

   reportDebug "cmdModuleUnuse: $args"
   if {$args eq ""} {
      showModulePath
   } else {
      foreach path $args {
         # get current module path list
         if {![info exists modpathlist]} {
            set modpathlist [getModulePathList]
         }

         # transform given path in an absolute path which should have been
         # registered in the MODULEPATH env var. however for compatibility
         # with previous behavior where relative paths were registered in
         # MODULEPATH given path is first checked against current path list
         set abspath [getAbsolutePath $path]
         if {[lsearch -exact $modpathlist $path] >= 0} {
            set unusepath $path
         } elseif {[lsearch -exact $modpathlist $abspath] >= 0} {
            set unusepath $abspath
         } else {
            set unusepath ""
         }

         if {$unusepath ne ""} {
            reportDebug "calling unload-path MODULEPATH $unusepath\
               $g_def_separator"

            pushMode "unload"

            catch {
               unload-path MODULEPATH $unusepath $g_def_separator
            }
            popMode

            # refresh path list after unload
            set modpathlist [getModulePathList]
            if {[lsearch -exact $modpathlist $unusepath] >= 0} {
               reportWarning "Did not unuse $unusepath"
            }
         }
      }
   }
}

proc cmdModuleAutoinit {} {
   global g_autoInit

   reportDebug "cmdModuleAutoinit:"
   set g_autoInit 1
}

proc cmdModuleInit {args} {
   global g_shell env

   set moduleinit_cmd [lindex $args 0]
   set moduleinit_list [lrange $args 1 end]
   set notdone 1
   set notclear 1

   reportDebug "cmdModuleInit: $args"

   # Define startup files for each shell
   set files(csh) [list ".modules" ".cshrc" ".cshrc_variables" ".login"]
   set files(tcsh) [list ".modules" ".tcshrc" ".cshrc" ".cshrc_variables"\
      ".login"]
   set files(sh) [list ".modules" ".bash_profile" ".bash_login" ".profile"\
      ".bashrc"]
   set files(bash) $files(sh)
   set files(ksh) $files(sh)
   set files(fish) [list ".modules" ".config/fish/config.fish"]
   set files(zsh) [list ".modules" ".zshrc" ".zshenv" ".zlogin"]

   # Process startup files for this shell
   set current_files $files($g_shell)
   foreach filename $current_files {
      if {$notdone && $notclear} {
         set filepath $env(HOME)
         append filepath "/" $filename
         # create a new file to put the changes in
         set newfilepath "$filepath-NEW"

         reportDebug "Looking at: $filepath"
         if {[file readable $filepath] && [file isfile $filepath]} {
            set fid [open $filepath r]

            # Only open the new file if we are not doing "initlist"
            if {[string compare $moduleinit_cmd "list"] != 0} {
               set newfid [open $newfilepath w]
            }

            while {[gets $fid curline] >= 0} {
               # Find module load/add command in startup file 
               set comments {}
               if {$notdone && [regexp {^([ \t]*module[ \t]+(load|add)[\
                  \t]*)(.*)} $curline match cmd subcmd modules]} {
                  regexp {([ \t]*\#.+)} $modules match comments
                  regsub {\#.+} $modules {} modules

                  # remove existing references to the named module from
                  # the list Change the module command line to reflect the 
                  # given command
                  switch $moduleinit_cmd {
                     list {
                        if {![info exists notheader]} {
                           report "$g_shell initialization file\
                              \$HOME/$filename loads modules:"
                           set notheader 0
                        }
                        report "\t$modules"
                     }
                     add {
                        foreach newmodule $moduleinit_list {
                           set modules [replaceFromList $modules $newmodule]
                        }
                        puts $newfid "$cmd$modules $moduleinit_list$comments"
                        # delete new modules in potential next lines
                        set moduleinit_cmd "rm"
                     }
                     prepend {
                        foreach newmodule $moduleinit_list {
                           set modules [replaceFromList $modules $newmodule]
                        }
                        puts $newfid "$cmd$moduleinit_list $modules$comments"
                        # delete new modules in potential next lines
                        set moduleinit_cmd "rm"
                     }
                     rm {
                        set oldmodcount [llength $modules]
                        foreach oldmodule $moduleinit_list {
                           set modules [replaceFromList $modules $oldmodule]
                        }
                        set modcount [llength $modules]
                        if {$modcount > 0} {
                           puts $newfid "$cmd$modules$comments"
                        }
                        if {$oldmodcount > $modcount} {
                           set notdone 0
                        }
                     }
                     switch {
                        set oldmodule [lindex $moduleinit_list 0]
                        set newmodule [lindex $moduleinit_list 1]
                        set newmodules [replaceFromList $modules\
                           $oldmodule $newmodule]
                        puts $newfid "$cmd$newmodules$comments"
                        if {"$modules" ne "$newmodules"} {
                           set notdone 0
                        }
                     }
                     clear {
                     }
                  }
               } else {
                  # copy the line from the old file to the new
                  if {[info exists newfid]} {
                     puts $newfid $curline
                  }
               }
            }

            close $fid
            if {[info exists newfid]} {
               close $newfid
               if {[catch {file copy -force $newfilepath $filepath} errMsg]} {
                  reportErrorAndExit "Init file $filepath cannot be\
                     written.\n$errMsg"
               }
               if {[catch {file delete $newfilepath} errMsg]} {
                  reportErrorAndExit "Temp file $filepath cannot be\
                     deleted.\n$errMsg"
               }
            }
         }
      }
   }
}

proc cmdModuleTest {mod} {
   pushMode "test"
   lassign [getPathToModule $mod] modfile modname
   if {$modfile ne ""} {
      pushSpecifiedName $mod
      pushModuleName $modname
      displaySeparatorLine
      report "Module Specific Test for $modfile:\n"
      execute-modulefile $modfile
      popModuleName
      popSpecifiedName
      displaySeparatorLine
   }
   popMode
}

proc cmdModuleHelp {args} {
   global MODULES_CURRENT_VERSION MODULES_CURRENT_RELEASE_DATE

   set done 0
   pushMode "help"
   foreach arg $args {
      if {$arg ne ""} {
         lassign [getPathToModule $arg] modfile modname

         if {$modfile ne ""} {
            pushSpecifiedName $arg
            pushModuleName $modname
            displaySeparatorLine
            report "Module Specific Help for $modfile:\n"
            execute-modulefile $modfile
            popModuleName
            popSpecifiedName
            displaySeparatorLine
         }
         set done 1
      }
   }
   popMode
   if {$done == 0} {
      report "Modules Release Tcl $MODULES_CURRENT_VERSION " 1
      report "($MODULES_CURRENT_RELEASE_DATE)" 1
      report {   Copyright GNU GPL v2 1991}
      report {Usage: module [options] [command] [args ...]}

      report {}
      report {Loading / Unloading commands:}
      report {  add | load      modulefile [...]  Load modulefile(s)}
      report {  rm | unload     modulefile [...]  Remove modulefile(s)}
      report {  purge                             Unload all loaded\
         modulefiles}
      report {  reload | refresh                  Unload then load all\
         loaded modulefiles}
      report {  switch | swap   [mod1] mod2       Unload mod1 and load mod2}
      report {}
      report {Listing / Searching commands:}
      report {  list            [-t|-l]           List loaded modules}
      report {  avail   [-d|-L] [-t|-l] [mod ...] List all or matching\
         available modules}
      report {  aliases                           List all module aliases}
      report {  whatis          [modulefile ...]  Print whatis\
         information of modulefile(s)}
      report {  apropos | keyword | search  str   Search all name and\
         whatis containing str}
      report {}
      report {Collection of modules handling commands:}
      report {  save            [collection|file] Save current module\
         list to collection}
      report {  restore         [collection|file] Restore module list\
         from collection or file}
      report {  saverm          [collection]      Remove saved collection}
      report {  saveshow        [collection|file] Display information\
         about collection}
      report {  savelist        [-t|-l]           List all saved\
         collections}
      report {}
      report {Shell's initialization files handling commands:}
      report {  initlist                          List all modules\
         loaded from init file}
      report {  initadd         modulefile [...]  Add modulefile to\
         shell init file}
      report {  initrm          modulefile [...]  Remove modulefile\
         from shell init file}
      report {  initprepend     modulefile [...]  Add to beginning of\
         list in init file}
      report {  initswitch      mod1 mod2         Switch mod1 with mod2\
         from init file}
      report {  initclear                         Clear all modulefiles\
         from init file}
      report {}
      report {Other commands:}
      report {  help            [modulefile ...]  Print this or\
         modulefile(s) help info}
      report {  display | show  modulefile [...]  Display information\
         about modulefile(s)}
      report {  test            [modulefile ...]  Test modulefile(s)}
      report {  use     [-a|-p] dir [...]         Add dir(s) to\
         MODULEPATH variable}
      report {  unuse           dir [...]         Remove dir(s) from\
         MODULEPATH variable}
      report {  path            modulefile        Print modulefile path}
      report {  paths           modulefile        Print path of\
         matching available modules}
      report {  source          scriptfile [...]  Execute scriptfile(s)}
      report {}
      report {Switches:}
      report {  -t | --terse    Display output in terse format}
      report {  -l | --long     Display output in long format}
      report {  -d | --default  Only show default versions available}
      report {  -L | --latest   Only show latest versions available}
      report {  -a | --append   Append directory to MODULEPATH}
      report {  -p | --prepend  Prepend directory to MODULEPATH}
      report {}
      report {Options:}
      report {  -h | --help     This usage info}
      report {  -V | --version  Module version}
      report {  -D | --debug    Enable debug messages}
   }
}


########################################################################
# main program

# needed on a gentoo system. Shouldn't hurt since it is
# supposed to be the default behavior
fconfigure stderr -translation auto

reportDebug "CALLING $argv0 $argv"

if {[catch {
   # Parse shell
   set g_shell [lindex $argv 0]
   switch -regexp -- $g_shell {
      ^(sh|bash|ksh|zsh)$ {
          set g_shellType sh
      }
      ^(fish)$ {
          set g_shellType fish
      }
      ^(cmd)$ {
          set g_shellType cmd
      }
      ^(csh|tcsh)$ {
          set g_shellType csh
      }
      ^(tcl)$ {
         set g_shellType tcl
      }
      ^(perl)$ {
          set g_shellType perl
      }
      ^(python)$ {
          set g_shellType python
      }
      ^(lisp)$ {
          set g_shellType lisp
      }
      . {
          reportErrorAndExit "Unknown shell type \'($g_shell)\'"
      }
   }

   # Parse options
   set opt [lindex $argv 1]
   switch -regexp -- $opt {
      {^(-deb|--deb|-D)} {
         set g_debug 1
         reportDebug "CALLING $argv0 $argv"
         set argv [lreplace $argv 1 1]
      }
      {^(--help|-h)} {
          cmdModuleHelp
          exit 0
      }
      {^(-V|--ver)} {
          report "Modules Release Tcl $MODULES_CURRENT_VERSION\
            ($MODULES_CURRENT_RELEASE_DATE)"
          exit 0
      }
      {^-} {
          reportErrorAndExit "Invalid option '$opt'\nTry\
            'module --help' for more information."
      }
   }

   set command [lindex $argv 1]
   # default command is help if none supplied
   if {$command eq ""} {
      set command "help"
   }
   set argv [lreplace $argv 0 1]

   cacheCurrentModules

   # Find and execute any .modulerc file found in the module directories
   # defined in env(MODULESPATH)
   runModulerc

   # extract command switches from other args
   set otherargv {}
   foreach arg $argv {
      switch -- $arg {
         "-t" - "--terse" {
            set show_oneperline 1
         }
         "-l" - "--long" {
            set show_modtimes 1
         }
         "-d" - "--default" {
            set show_filter "onlydefaults"
         }
         "-L" - "--latest" {
            set show_filter "onlylatest"
         }
         default {
            lappend otherargv $arg
         }
      }
   }

   # eval needed to pass otherargv as list to module proc
   eval module $command $otherargv
} errMsg ]} {
   # no use of reportError here to get independent from any
   # previous error report inhibition
   if {$errMsg ne ""} {
      report "ERROR: $errMsg"
   }
   exit 1
}

exit 0

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
# vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
