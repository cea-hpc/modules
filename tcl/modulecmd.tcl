#!/bin/sh
# \
type tclsh 1>/dev/null 2>&1 && exec tclsh "$0" "$@"
# \
[ -x /usr/local/bin/tclsh ] && exec /usr/local/bin/tclsh "$0" "$@"
# \
[ -x /usr/bin/tclsh ] && exec /usr/bin/tclsh "$0" "$@"
# \
[ -x /bin/tclsh ] && exec /bin/tclsh "$0" "$@"
# \
echo "FATAL: module: Could not find tclsh in \$PATH or in standard directories" >&2; exit 1
	
########################################################################
# This is a pure TCL implementation of the module command
# to initialize the module environment, either
# - one of the scripts from the init directory should be sourced, or just
# - eval `/some-path/tclsh modulecmd.tcl MYSHELL autoinit`
# in both cases the path to tclsh is remembered and used furtheron
########################################################################
#
# Some Global Variables.....
#
regsub {\$[^:]+:\s*(\S+)\s*\$} {$Revision: 1.147 $} {\1}\
	 MODULES_CURRENT_VERSION
set g_debug 0 ;# Set to 1 to enable debugging
set error_count 0 ;# Start with 0 errors
set g_autoInit 0
set g_force 1 ;# Path element reference counting if == 0
set CSH_LIMIT 4000 ;# Workaround for commandline limits in csh
set flag_default_dir 1 ;# Report default directories
set flag_default_mf 1 ;# Report default modulefiles and version alias

# Used to tell if a machine is running Windows or not
proc isWin {} {
    global tcl_platform

    if { $tcl_platform(platform) == "windows" } {
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
if {[catch {exec stty size} stty_size] == 0 && $stty_size != ""} {
    set DEF_COLUMNS [lindex $stty_size 1]
}

# Change this to your support email address...
set contact "root@localhost"

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

#
# Info, Warnings and Error message handling.
#
proc reportWarning {message {nonewline ""}} {
    if {$nonewline != ""} {
	puts -nonewline stderr "$message"
    } else {
	puts stderr "$message"
    }
}

proc reportInternalBug {message} {
    global contact

    puts stderr "Module ERROR: $message\nPlease contact: $contact"
}

proc report {message {nonewline ""}} {
    if {$nonewline != ""} {
	puts -nonewline stderr "$message"
    } else {
	puts stderr "$message"
    }
}

########################################################################
# Use a slave TCL interpreter to execute modulefiles
#

proc unset-env {var} {
    global env g_debug

    if {[info exists env($var)]} {
        if {$g_debug} {
    	    report "DEBUG unset-env:  $var"
        }
	unset env($var)
    }
}

proc execute-modulefile {modfile {help ""}} {
    global g_debug
    global ModulesCurrentModulefile
    set ModulesCurrentModulefile $modfile

    if {$g_debug} {
	report "DEBUG execute-modulefile:  Starting $modfile"
    }
    set slave __[currentModuleName]
    if {![interp exists $slave]} {
	interp create $slave
	interp alias $slave setenv {} setenv
	interp alias $slave unsetenv {} unsetenv
	interp alias $slave getenv {} getenv
	interp alias $slave system {} system
	interp alias $slave append-path {} append-path
	interp alias $slave prepend-path {} prepend-path
	interp alias $slave remove-path {} remove-path
	interp alias $slave prereq {} prereq
	interp alias $slave conflict {} conflict
	interp alias $slave is-loaded {} is-loaded
	interp alias $slave module {} module
	interp alias $slave module-info {} module-info
	interp alias $slave module-whatis {} module-whatis
	interp alias $slave set-alias {} set-alias
	interp alias $slave unset-alias {} unset-alias
	interp alias $slave uname {} uname
	interp alias $slave x-resource {} x-resource
	interp alias $slave module-version {} module-version
	interp alias $slave module-alias {} module-alias
	interp alias $slave reportInternalBug {} reportInternalBug
	interp alias $slave reportWarning {} reportWarning
	interp alias $slave report {} report
	interp alias $slave isWin {} isWin

	interp eval $slave {global ModulesCurrentModulefile g_debug}
	interp eval $slave [list "set" "ModulesCurrentModulefile" $modfile]
	interp eval $slave [list "set" "g_debug" $g_debug]
	interp eval $slave [list "set" "help" $help]

    }
    set errorVal [interp eval $slave {
	if {$g_debug} {
	    report "Sourcing $ModulesCurrentModulefile"
        }
	set sourceFailed [catch {source $ModulesCurrentModulefile} errorMsg]
	if {$help != ""} {
	    if {[info procs "ModulesHelp"] == "ModulesHelp"} {
		ModulesHelp
	    } else {
		reportWarning "Unable to find ModulesHelp in\
		  $ModulesCurrentModulefile."
	    }
	    set sourceFailed 0
	}
	if {$sourceFailed} {
	    if {$errorMsg == "" && $errorInfo == ""} {
		unset errorMsg
		return 1
	    }\
	    elseif [regexp "^WARNING" $errorMsg] {
		reportWarning $errorMsg
		return 1
	    } else {
		global errorInfo
		reportInternalBug "ERROR occurred in file\
		  $ModulesCurrentModulefile:$errorInfo"
		exit 1
	    }
	} else {
	    unset errorMsg
	    return 0
	}
    }]
    interp delete $slave
    if {$g_debug} {
	report "DEBUG Exiting $modfile"
    }
    return $errorVal
}

# Smaller subset than main module load... This function runs modulerc and\
  .version files
proc execute-modulerc {modfile} {
    global g_rcfilesSourced
    global g_debug g_moduleDefault
    global ModulesCurrentModulefile


    if {$g_debug} {
       report "DEBUG execute-modulerc: $modfile"
    }

    set ModulesCurrentModulefile $modfile

    if {![checkValidModule $modfile]} {
	reportInternalBug "+(0):ERROR:0: Magic cookie '#%Module' missing in\
	  '$modfile'"
	return ""
    }

    set modparent [file dirname $modfile]

    if {![info exists g_rcfilesSourced($modfile)]} {
	if {$g_debug} {
	    report "DEBUG execute-modulerc: sourcing rc $modfile"
	}
	set slave __.modulerc
	if {![interp exists $slave]} {
	    interp create $slave
	    interp alias $slave uname {} uname
	    interp alias $slave system {} system
	    interp alias $slave module-version {} module-version
	    interp alias $slave module-alias {} module-alias
	    interp alias $slave module {} module
	    interp alias $slave reportInternalBug {} reportInternalBug

	    interp eval $slave {global ModulesCurrentModulefile g_debug}
	    interp eval $slave [list "global" "ModulesVersion"]
	    interp eval $slave [list "set" "ModulesCurrentModulefile" $modfile]
	    interp eval $slave [list "set" "g_debug" $g_debug]
	    interp eval $slave {set ModulesVersion {}}
	}
	set ModulesVersion [interp eval $slave {
	    if [catch {source $ModulesCurrentModulefile} errorMsg] {
		global errorInfo
		reportInternalBug "occurred in file\
		  $ModulesCurrentModulefile:$errorInfo"
		exit 1
	    }\
	    elseif [info exists ModulesVersion] {
		return $ModulesVersion
	    } else {
		return {}
	    }
	}]
	interp delete $slave

	if {[file tail $modfile] == ".version"} {
	    # only set g_moduleDefault if .version file,
	    # otherwise any modulerc settings ala "module-version /xxx default"
	    #  would get overwritten
	    set g_moduleDefault($modparent) $ModulesVersion
	}

	if {$g_debug} {
	    report "DEBUG execute-version: Setting g_moduleDefault($modparent)\
	      $ModulesVersion"
	}

	# Keep track of rc files we already sourced so we don't run them again
	set g_rcfilesSourced($modfile) $ModulesVersion
    }
    return $g_rcfilesSourced($modfile)
}


########################################################################
# commands run from inside a module file
#
set ModulesCurrentModulefile {}

proc module-info {what {more {}}} {
    global g_shellType g_shell g_debug tcl_platform
    global g_moduleAlias g_symbolHash g_versionHash

    set mode [currentMode]

    if {$g_debug} {
        report "DEBUG module-info: $what $more  mode=$mode"
    }

    switch -- $what {
    "mode" {
	    if {$more != ""} {
		if {$mode == $more} {
		    return 1
		} else {
		    return 0
		}
	    } else {
		return $mode
	    }
	}
    "name" -
    "specified" {
	    return [currentModuleName]
	}
    "shell" {
	    return $g_shell
	}
    "flags" {
	    return 0
	}
    "shelltype" {
	    return $g_shellType
	}
    "user" {
    	        return $tcl_platform(user)
        }
    "alias" {
	    if {[info exists g_moduleAlias($more)]} {
	        return $g_moduleAlias($more)
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
    "symbols" {
	    if {[regexp {^\/} $more]} {
		set tmp [currentModuleName]
		set tmp [file dirname $tmp]
		set more "${tmp}$more"
	    }
	    if {[info exists g_symbolHash($more)]} {
		return $g_symbolHash($more)
	    } else {
		return {}
	    }
	}
    "version" {
	    if {[regexp {^\/} $more]} {
		set tmp [currentModuleName]
		set tmp [file dirname $tmp]
		set more "${tmp}$more"
	    }
	    if {[info exists g_versionHash($more)]} {
		return $g_versionHash($more)
	    } else {
		return {}
	    }
	}
    default {
	    error "module-info $what not supported"
	    return {}
	}
    }
}

proc module-whatis {message} {
    global g_whatis g_debug

    set mode [currentMode]

    if {$g_debug} {
        report "DEBUG module-whatis: $message  mode=$mode"
    }

    if {$mode == "display"} {
	report "module-whatis\t$message"
    }\
    elseif {$mode == "whatis"} {
	set g_whatis $message
    }
    return {}
}

# Specifies a default or alias version for a module that points to an 
# existing module version Note that the C version stores aliases and 
# defaults by the short module name (not the full path) so aliases and 
# defaults from one directory will apply to modules of the same name found 
# in other directories.
proc module-version {args} {
    global g_moduleVersion g_versionHash
    global g_moduleDefault
    global g_debug
    global ModulesCurrentModulefile

    if {$g_debug} {
	report "DEBUG module-version: executing module-version $args"
    }
    set module_name [lindex $args 0]

    # Check for shorthand notation of just a version "/version".  Base is 
    # implied by current dir prepend the current directory to module_name
    if {[regexp {^\/} $module_name]} {
	set base [file dirname $ModulesCurrentModulefile]
	set module_name "${base}$module_name"
    }

    foreach version [lrange $args 1 end] {

	set base [file dirname $module_name]
	set aliasversion [file tail $module_name]

	if {$base != ""} {
	    if {[string match $version "default"]} {
		# If we see more than one default for the same module, just\
		  keep the first
		if {![info exists g_moduleDefault($base)]} {
		    set g_moduleDefault($base) $aliasversion
		    if {$g_debug} {
			report "DEBUG module-version: default $base\
			  =$aliasversion"
		    }
		}
	    } else {
		set aliasversion "$base/$version"
		if {$g_debug} {
		    report "DEBUG module-version: alias $aliasversion =\
		      $module_name"
		}
		set g_moduleVersion($aliasversion) $module_name

		if {[info exists g_versionHash($module_name)]} {
		    # don't add duplicates
		    if {[lsearch -exact $g_versionHash($module_name)\
		      $aliasversion] < 0} {
			set tmplist $g_versionHash($module_name)
			set tmplist [linsert $tmplist end $aliasversion]
			set g_versionHash($module_name) $tmplist
		    }
		} else {
		    set g_versionHash($module_name) $aliasversion
		}
	    }


	    if {$g_debug} {
		report "DEBUG module-version: $aliasversion  = $module_name"
	    }
	} else {
	    error "module-version: module argument for default must not be\
	      fully version qualified"
	}
    }
    if {[string match [currentMode] "display"]} {
	report "module-version\t$args"
    }
    return {}
}


proc module-alias {args} {
    global g_moduleAlias
    global ModulesCurrentModulefile
    global g_debug

    set alias [lindex $args 0]
    set module_file [lindex $args 1]

    if {$g_debug} {
	report "DEBUG module-alias: $alias  = $module_file"
    }

    set g_moduleAlias($alias) $module_file

    if {[info exists g_aliasHash($module_file)]} {
	set tmplist $g_aliasHash($module_file)
	set tmplist [linsert $tmplist end $alias]
	set g_aliasHash($module_file) $tmplist
    } else {
	set g_aliasHash($module_file) $alias
    }

    if {[string match [currentMode] "display"]} {
	report "module-alias\t$args"
    }


    return {}
}


proc module {command args} {
    set mode [currentMode]
    global g_debug

    # Resolve any module aliases
    if {$g_debug} {
	report "DEBUG module: Resolving $args"
    }
    set args [resolveModuleVersionOrAlias $args]
    if {$g_debug} {
	report "DEBUG module: Resolved to $args"
    }

    switch -- $command {
    add - lo -
    load {
	    if {$mode == "load"} {
		eval cmdModuleLoad $args
	    }\
	    elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "display"} {
		report "module load\t$args"
	    }
	}
    rm - unlo -
    unload {
	    if {$mode == "load"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "display"} {
		report "module unload\t$args"
	    }
	}
    reload {
	    cmdModuleReload
	}
    use {
	    eval cmdModuleUse $args
	}
    unuse {
	    eval cmdModuleUnuse $args
	}
    source {
	    eval cmdModuleSource $args
	}
    switch -
    swap {
	    eval cmdModuleSwitch $args
	}
    display - dis -
    show {
	    eval cmdModuleDisplay $args
	}
    avail - av {
	    if {$args != ""} {
		foreach arg $args {
		    cmdModuleAvail $arg
		}
	    } else {
		cmdModuleAvail
 	        # Not sure if this should be a part of cmdModuleAvail or not
	        cmdModuleAliases
	    }
	}
    aliases - al {
        cmdModuleAliases
    }
    path {
	    eval cmdModulePath $args
	}
    paths {
	    eval cmdModulePaths $args
	}
    list {
	    cmdModuleList
	}
    whatis {
	    if {$args != ""} {
		foreach arg $args {
		    cmdModuleWhatIs $arg
		}
	    } else {
		cmdModuleWhatIs
	    }
	}
    apropos - search -
    keyword {
	    eval cmdModuleApropos $args
	}
    purge {
	    eval cmdModulePurge
	}
    initadd {
	    eval cmdModuleInit add $args
	}
    initprepend {
	    eval cmdModuleInit prepend $args
	}
    initrm {
	    eval cmdModuleInit rm $args
	}
    initlist {
	    eval cmdModuleInit list $args
	}
    initclear {
	    eval cmdModuleInit clear $args
	}
    default {
	    error "module $command not understood"
	}
    }
    return {}
}

proc setenv {var val} {
    global g_stateEnvVars env g_debug
    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG setenv: ($var,$val) mode = $mode"
    }

    if {$mode == "load"} {
	set env($var) $val
	set g_stateEnvVars($var) "new"
    }\
    elseif {$mode == "unload"} {
	# Don't unset-env here ... it breaks modulefiles
	# that use env(var) is later in the modulefile
	#unset-env $var
	set g_stateEnvVars($var) "del"
    }\
    elseif {$mode == "display"} {
	# Let display set the variable for later use in the display
	# but don't commit it to the env
	set env($var) $val
	set g_stateEnvVars($var) "nop"
	report "setenv\t\t$var\t$val"
    }
    return {}
}

proc getenv {var} {
     global g_debug
     set mode [currentMode]

     if {$g_debug} {
         report "DEBUG getenv: ($var) mode = $mode"
     }

     if {$mode == "load" || $mode == "unload"} {
	 if {[info exists env($var)]} {
             return $::env($var)
         } else {
             return "_UNDEFINED_"
         }
     }\
     elseif {$mode == "display"} {
         return "\$$var"
     }
     return {}
}

proc unsetenv {var {val {}}} {
    global g_stateEnvVars env g_debug
    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG unsetenv: ($var,$val) mode = $mode"
    }

    if {$mode == "load"} {
	if {[info exists env($var)]} {
	    unset-env $var
	}
	set g_stateEnvVars($var) "del"
    }\
    elseif {$mode == "unload"} {
	if {$val != ""} {
	    set env($var) $val
	    set g_stateEnvVars($var) "new"
	}
    }\
    elseif {$mode == "display"} {
	report "unsetenv\t\t$var"
    }
    return {}
}

########################################################################
# path fiddling

proc getReferenceCountArray {var separator} {
    global env g_force g_def_separator g_debug

    if {$g_debug} {
       report "DEBUG getReferenceCountArray: ($var, $separator)"
    }

    set sharevar "${var}_modshare"
    set modshareok 1
    if {[info exists env($sharevar)]} {
	if {[info exists env($var)]} {
	    set modsharelist [split $env($sharevar) $g_def_separator]
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
			set countarr($path) 999999999
		    }
		}

		if {! $g_force} {
		    if {[array size fixers]} {
			reportWarning "WARNING: \$$var does not agree with\
			  \$${var}_modshare counter. The following\
			  directories' usage counters were adjusted to match.\
			  Note that this may mean that module unloading may\
			  not work correctly."
			foreach dir [array names fixers] {
			    reportWarning " $dir" -nonewline
			}
			reportWarning ""
		    }
		}

	    } else {
		#		sharevar was corrupted, odd number of elements.
		set modshareok 0
	    }
	} else {
	    if {$g_debug} {	    
		reportWarning "WARNING: module: $sharevar exists (\
	          $env($sharevar) ), but $var doesn't. Environment is corrupted."
            }
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
    global g_stateEnvVars env g_force g_def_separator g_debug

    array set countarr [getReferenceCountArray $var $separator]

    if {$g_debug} {
	report "DEBUG unload-path: ($var, $path, $separator)"
    }

    # Don't worry about dealing with this variable if it is already scheduled\
      for deletion
    if {[info exists g_stateEnvVars($var)] && $g_stateEnvVars($var) == "del"} {
	return {}
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
		    if {$elem != $dir} {
			lappend newpath $elem
		    }
		}
		if {$newpath == ""} {
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
	set env($sharevar) [join [array get countarr] $g_def_separator]
	set g_stateEnvVars($sharevar) "new"
    } else {
	unset-env $sharevar
	set g_stateEnvVars($sharevar) "del"
    }
    return {}
}

proc add-path {var path pos separator} {
    global env g_stateEnvVars g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG add-path: ($var, $path, $separator)"
    }

    set sharevar "${var}_modshare"
    array set countarr [getReferenceCountArray $var $separator]

    if {$pos == "prepend"} {
	set pathelems [reverseList [split $path $separator]]
    } else {
	set pathelems [split $path $separator]
    }
    foreach dir $pathelems {
	if {[info exists countarr($dir)]} {
	    #	    already see $dir in $var"
	    incr countarr($dir)
	} else {
	    if {[info exists env($var)]} {
		if {$pos == "prepend"} {
		    set env($var) "$dir$separator$env($var)"
		}\
		elseif {$pos == "append"} {
		    set env($var) "$env($var)$separator$dir"
		} else {
		    error "add-path doesn't support $pos"
		}
	    } else {
		set env($var) "$dir"
	    }
	    set countarr($dir) 1
	}
        if {$g_debug} {
    	   report "DEBUG add-path: env($var) = $env($var)"
        }
    }


    set env($sharevar) [join [array get countarr] $g_def_separator]
    set g_stateEnvVars($var) "new"
    set g_stateEnvVars($sharevar) "new"
    return {}
}

proc prepend-path {var path args} {
    global g_def_separator g_debug

    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG prepend-path: ($var, $path, $args) mode=$mode"
    }

    if {[string match $var "-delim"]} {
        set separator $path
        set var [lindex $args 0]
        set path [lindex $args 1]
    } else {
        set separator $g_def_separator
    }

    if {$mode == "load"} {
	add-path $var $path "prepend" $separator
    }\
    elseif {$mode == "unload"} {
	unload-path $var $path $separator
    }\
    elseif {$mode == "display"} {
	report "prepend-path\t$var\t$path"
    }
    return {}
}


proc append-path {var path args} {
    global g_def_separator g_debug

    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG append-path: ($var, $path, $args) mode=$mode"
    }

    if {[string match $var "-delim"]} {
        set separator $path
        set var [lindex $args 0]
        set path [lindex $args 1]
    } else {
        set separator $g_def_separator
    }

    if {$mode == "load"} {
	add-path $var $path "append" $separator
    }\
    elseif {$mode == "unload"} {
	unload-path $var $path $separator
    }\
    elseif {$mode == "display"} {
	report "append-path\t$var\t$path"
    }
    return {}
}

proc remove-path {var path args} {
    global g_def_separator g_debug

    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG remove-path: ($var, $path, $args) mode=$mode"
    }

    if {[string match $var "-delim"]} {
        set separator $path
        set var [lindex $args 0]
        set path [lindex $args 1]
    } else {
        set separator $g_def_separator
    }

    if {$mode == "load"} {
	unload-path $var $path $separator
    }\
    elseif {$mode == "display"} {
	report "remove-path\t$var\t$path"
    }
    return {}
}

proc set-alias {alias what} {
    global g_Aliases g_stateAliases g_debug
    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG set-alias: ($alias, $what) mode=$mode"
    }
    if {$mode == "load"} {
	set g_Aliases($alias) $what
	set g_stateAliases($alias) "new"
    }\
    elseif {$mode == "unload"} {
	set g_Aliases($alias) {}
	set g_stateAliases($alias) "del"
    }\
    elseif {$mode == "display"} {
	report "set-alias\t$alias\t$what"
    }
    return {}
}


proc unset-alias {alias} {
    global g_Aliases g_stateAliases g_debug
    set mode [currentMode]

    if {$g_debug} {
	report "DEBUG unset-alias: ($alias) mode=$mode"
    }
    if {$mode == "load"} {
	set g_Aliases($alias) {}
	set g_stateAliases($alias) "del"
    }\
    elseif {$mode == "display"} {
	report "unset-alias\t$alias"
    }
    return {}
}

proc is-loaded {modulelist} {
    global env g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG is-loaded: $modulelist"
    }

    if {[llength $modulelist] > 0} {
	if {[info exists env(LOADEDMODULES)]} {
	    foreach arg $modulelist {
		set arg "$arg/"
		foreach mod [split $env(LOADEDMODULES) $g_def_separator] {
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
    global ModulesCurrentModulefile g_debug
    set mode [currentMode]
    set currentModule [currentModuleName]

    if {$g_debug} {
	report "DEBUG conflict: ($args) mode = $mode"
    }

    if {$mode == "load"} {
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
    elseif {$mode == "display"} {
	report "conflict\t$args"
    }
    return {}
}

proc prereq {args} {
    global g_debug
    set mode [currentMode]
    set currentModule [currentModuleName]

    if {$g_debug} {
	report "DEBUG prereq: ($args) mode = $mode"
    }

    if {$mode == "load"} {
	if {![is-loaded $args]} {
	    set errMsg "WARNING: $currentModule cannot be loaded due to\
	      missing prereq."
	    set errMsg "$errMsg\nHINT: the following modules must be loaded\
	      first: $args"
	    error $errMsg
	}
    }\
    elseif {$mode == "display"} {
	report "prereq\t\t$args"
    }
    return {}
}

proc x-resource {resource {value {}}} {
    global g_newXResources g_delXResources g_debug
    set mode [currentMode]

    if {$g_debug} {
       report "DEBUG x-resource: ($resource, $value)"
    }

    if {$mode == "load"} {
	set g_newXResources($resource) $value
    }\
    elseif {$mode =="unload"} {
	set g_delXResources($resource) 1
    }\
    elseif {$mode == "display"} {
	report "x-resource\t$resource\t$value"
    }
    return {}
}

proc uname {what} {
    global unameCache tcl_platform g_debug
    set result {}

    if {$g_debug} {
       report "DEBUG uname: called: $what"
    }

    if {! [info exists unameCache($what)]} {
	switch -- $what {
	    sysname {
    	        set result $tcl_platform(os)
	    }
	    machine {
	        set result $tcl_platform(machine)
	    }
	    nodename -
	    node {
	        set result [info hostname]
	    }
	    release {
		# on ubuntu get the CODENAME of the Distribution
		if { [file isfile /etc/lsb-release]} {
                        set fd [open "/etc/lsb-release" "r"]
                        set a [read $fd]
                        regexp -nocase {DISTRIB_CODENAME=(\S+)(.*)} $a matched res end
                        set result $res
                } else {
			set result $tcl_platform(osVersion)
		}
	    }
	    domain {
		set result [exec /bin/domainname]
	    }
	    version {
		set result [exec /bin/uname -v]
	    }
	    default {
		error "uname $what not supported"
	    }
	}
	set unameCache($what) $result
    }

    return $unameCache($what)
}

########################################################################
# internal module procedures

set g_modeStack {}

proc currentMode {} {
    global g_modeStack

    set mode [lindex $g_modeStack end]
    return $mode
}

proc pushMode {mode} {
    global g_modeStack

    lappend g_modeStack $mode
}

proc popMode {} {
    global g_modeStack

    set len [llength $g_modeStack]
    set len [expr {$len - 2}]
    set g_modeStack [lrange $g_modeStack 0 $len]
}


set g_moduleNameStack {}

proc currentModuleName {} {
    global g_moduleNameStack

    set moduleName [lindex $g_moduleNameStack end]
    return $moduleName
}

proc pushModuleName {moduleName} {
    global g_moduleNameStack

    lappend g_moduleNameStack $moduleName
}

proc popModuleName {} {
    global g_moduleNameStack

    set len [llength $g_moduleNameStack]
    set len [expr {$len - 2}]
    set g_moduleNameStack [lrange $g_moduleNameStack 0 $len]
}


# Return the full pathname and modulename to the module.  
# Resolve aliases and default versions if the module name is something like
# "name/version" or just "name" (find default version).
proc getPathToModule {mod {separator {}}} {
    global env g_loadedModulesGeneric
    global g_moduleAlias g_moduleVersion
    global g_debug g_def_separator
    global ModulesCurrentModulefile flag_default_mf flag_default_dir

    set retlist ""

    if {$mod == ""} {
	return ""
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }

    if {$g_debug} {
	report "DEBUG getPathToModule: Finding $mod"
    }

    # Check for aliases
# This is already done at the root level so why do it again?
#    set newmod [resolveModuleVersionOrAlias $mod]
#    if {$newmod != $mod} {
#	# Alias before ModulesVersion
#	return [getPathToModule $newmod]
#    }

    # Check for $mod specified as a full pathname
    if {[string match {/*} $mod]} {
	if {[file exists $mod]} {
	    if {[file readable $mod]} {
		if {[file isfile $mod]} {
		    # note that a raw filename as an argument returns the full\
		      path as the module name
		    if {[checkValidModule $mod]} {
			return [list $mod $mod]
		    } else {
			report "+(0):ERROR:0: Unable to locate a modulefile\
			  for '$mod'"
			return ""
		    }
		}
	    }
	}
    }\
    elseif {[info exists env(MODULEPATH)]} {
	# Now search for $mod in MODULEPATH
	foreach dir [split $env(MODULEPATH) $separator] {
	    set path "$dir/$mod"

	    # modparent is the the modulename minus the module version.  
	    set modparent [file dirname $mod]
	    set modversion [file tail $mod]
	    # If $mod was specified without a version (no "/") then mod is\
	      really modparent
	    if {$modparent == "."} {
		set modparent $mod
	    }
	    set modparentpath "$dir/$modparent"


	    # Search the modparent directory for .modulerc files in case we\
	      need to translate an alias
	    if {[file isdirectory $modparentpath]} {
		# Execute any modulerc for this module
		if {[file exists "$modparentpath/.modulerc"]} {
		    if {$g_debug} {
			report "DEBUG getPathToModule: Found\
			  $modparentpath/.modulerc"
		    }
		    execute-modulerc $modparentpath/.modulerc
		}
		# Check for an alias
		set newmod [resolveModuleVersionOrAlias $mod]
		if {$newmod != $mod} {
		    # Alias before ModulesVersion
		    return [getPathToModule $newmod]
		}
	    }

	    # Now check if the mod specified is a file or a directory
	    if {[file readable $path]} {
		# If a directory, return the default if a .version file is
		# present or return the last file within the dir
		if {[file isdirectory $path]} {
		    set ModulesVersion ""
		    # Not an alias or version alias - check for a .version\
		      file or find the default file
		    if {[info exists g_loadedModulesGeneric($mod)]} {
			set ModulesVersion $g_loadedModulesGeneric($mod)
		    }\
		    elseif {[file exists "$path/.version"] && ![file readable\
		      "$path/.modulerc"]} {
			# .version files aren't read if .modulerc present
			if {$g_debug} {
			    report "DEBUG getPathToModule: Found $path/.version"
			}
			set ModulesVersion [execute-modulerc "$path/.version"]
		    }


		    # Try for the last file in directory if no luck so far
		    if {$ModulesVersion == ""} {
			set modlist [listModules $path "" 0 "-dictionary" 0 0]
			set ModulesVersion [lindex $modlist end]
			if {$g_debug} {
			    report "DEBUG getPathToModule: Found\
			      $ModulesVersion in $path"
			}
		    }


		    if {$ModulesVersion != ""} {
			# The path to the module file
			set verspath "$path/$ModulesVersion"
			# The modulename (name + version)
			set versmod "$mod/$ModulesVersion"
			set retlist [list $verspath $versmod]
		    }
		} else {
		    # If mod was a file in this path, try and return that file
		    set retlist [list $path $mod]
		}

		# We may have a winner, check validity of result
		if {[llength $retlist] == 2} {
		    # Check to see if we've found only a directory.  If so,\
		      keep looking
		    if {[file isdirectory [lindex $retlist 0]]} {
			set retlist [getPathToModule [lindex $retlist 1]]
		    }

		    if {! [checkValidModule [lindex $retlist 0]]} {
			set path [lindex $retlist 0]
		    } else {
			return $retlist
		    }
		}
	    }
	    # File wasn't readable, go to next path
	}
	# End of of foreach loop
	report "+(0):ERROR:0: Unable to locate a modulefile for '$mod'"
	return ""
    } else {
	error "\$MODULEPATH not defined"
	return ""
    }
}

proc runModulerc {} {
    # Runs the global RC files if they exist
    global env g_debug

    if {$g_debug} {
	report "DEBUG runModulerc: running..."
	report "DEBUG runModulerc: env MODULESHOME = $env(MODULESHOME)"
	report "DEBUG runModulerc: env HOME = $env(HOME)"
    }
    if {[info exists env(MODULERCFILE)]} {
	if {[file readable $env(MODULERCFILE)]} {
	    if {$g_debug} {
		report "DEBUG runModulerc: Executing $env(MODULERCFILE)"
	    }
	    cmdModuleSource $env(MODULERCFILE)
	}
    }
    if {[info exists env(MODULESHOME)]} {
	if {[file readable "$env(MODULESHOME)/etc/rc"]} {
	    if {$g_debug} {
		report "DEBUG runModulerc: Executing $env(MODULESHOME)/etc/rc"
	    }
	    cmdModuleSource "$env(MODULESHOME)/etc/rc"
	}
    }
    if {[info exists env(HOME)]} {
	if {[file readable "$env(HOME)/.modulerc"]} {
	    if {$g_debug} {
		report "DEBUG runModulerc: Executing $env(HOME)/.modulerc"
	    }
	    cmdModuleSource "$env(HOME)/.modulerc"
	}
    }
}

proc saveSettings {} {
    foreach var {env g_Aliases g_stateEnvVars g_stateAliases g_newXResource\
      g_delXResource} {
	eval "global g_SAVE_$var $var"
	eval "array set g_SAVE_$var \[array get $var\]"
    }
}

proc restoreSettings {} {
    foreach var {env g_Aliases g_stateEnvVars g_stateAliases g_newXResource\
      g_delXResource} {
	eval "global g_SAVE_$var $var"
	eval "array set $var \[array get g_SAVE_$var\]"
    }
}

proc renderSettings {} {
    global env g_Aliases g_shellType g_shell
    global g_stateEnvVars g_stateAliases
    global g_newXResources g_delXResources
    global g_pathList g_systemList error_count
    global g_autoInit CSH_LIMIT g_debug

    if {$g_debug} {
       report "DEBUG renderSettings: called."
    }

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

            # automatically detect which tclsh should be used for future module commands
            set tclshbin [info nameofexecutable]

	    # add cwd if not absolute script path
	    if {! [regexp {^/} $argv0]} {
		set pwd [exec pwd]
		set argv0 "$pwd/$argv0"
	    }

	    set env(MODULESHOME) [file dirname $argv0]
	    set g_stateEnvVars(MODULESHOME) "new"

	    switch -- $g_shellType {
	    csh {
		    puts stdout "if ( \$?histchars ) then"
		    puts stdout "  set _histchars = \$histchars"
		    puts stdout "  if (\$?prompt) then"
		    puts stdout "    alias module 'unset histchars;set\
		      _prompt=\"\$prompt\";eval `'$tclshbin' '$argv0' '$g_shell' \\!*`;set\
		      histchars = \$_histchars; set prompt=\"\$_prompt\";unset\
		      _prompt'"
		    puts stdout "  else"
		    puts stdout "    alias module 'unset histchars;eval `'$tclshbin' '$argv0'\
		      '$g_shell' \\!*`;set histchars = \$_histchars'"
		    puts stdout "  endif"
		    puts stdout "else"
		    puts stdout "  if (\$?prompt) then"
		    puts stdout "    alias module 'set _prompt=\"\$prompt\";set\
		      prompt=\"\";eval `'$tclshbin' '$argv0' '$g_shell' \\!*`;set\
		      prompt=\"\$_prompt\";unset _prompt'"
		    puts stdout "  else"
		    puts stdout "    alias module 'eval `'$tclshbin' '$argv0' '$g_shell' \\!*`'"
		    puts stdout "  endif"
		    puts stdout "endif"
		}
	    sh {
		    puts stdout "module () { eval `'$tclshbin' '$argv0' '$g_shell' \$*`; } ;"
		}
	    cmd {
	            puts stdout "start /b \%MODULESHOME\%/init/module.cmd %*"
	        }
	    perl {
		    puts stdout "sub module {"
		    puts stdout "  eval `$tclshbin \$ENV{\'MODULESHOME\'}/modulecmd.tcl perl @_`;"
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
		    puts stdout "        exec subprocess.Popen(\['$tclshbin', '$argv0', 'python', command\] \
                       list(arguments), stdout=subprcess.PIPE).communicate()\[0\]"
		}
	    lisp {
		    error "ERROR: XXX lisp mode autoinit not yet implemented"
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
	    if {$g_stateEnvVars($var) == "new"} {
		switch -- $g_shellType {
		csh {
			set val [multiEscaped $env($var)]
			# csh barfs on long env vars
			if {$g_shell == "csh" && [string length $val] >\
			  $CSH_LIMIT} {
			    if {$var == "PATH"} {
				reportWarning "WARNING: module: PATH exceeds\
				  $CSH_LIMIT characters, truncating and\
				  appending /usr/bin:/bin ..."
				set val [string range $val 0 [expr {$CSH_LIMIT\
				  - 1}]]:/usr/bin:/bin
			    } else {
				reportWarning "WARNING: module: $var exceeds\
				  $CSH_LIMIT characters, truncating..."
				set val [string range $val 0 [expr {$CSH_LIMIT\
				  - 1}]]
			    }
			}
			puts stdout "setenv $var $val;"
		    }
		sh {
			puts stdout "$var=[multiEscaped $env($var)]; export $var;"
		    }
		perl {
			set val [doubleQuoteEscaped $env($var)]
			set val [atSymbolEscaped $env($var)]
			puts stdout "\$ENV{\'$var\'} = \'$val\';"
		    }
		python {
			set val [singleQuoteEscaped $env($var)]
			puts stdout "os.environ\['$var'\] = '$val'"
		    }
		lisp {
			set val [doubleQuoteEscaped $env($var)]
			puts stdout "(setenv \"$var\" \"$val\")"
		    }
	        cmd {
	                set val $env($var)
	                puts stdout "set $var=$val"
	            }
		}
	    } elseif {$g_stateEnvVars($var) == "del"} {
		switch -- $g_shellType {
		csh {
			puts stdout "unsetenv $var;"
		    }
		sh {
			puts stdout "unset $var;"
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
           if {$g_stateAliases($var) == "new"} {
              switch -- $g_shellType {
                 csh {
                    # set val [multiEscaped $g_Aliases($var)]
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
              }
           } elseif {$g_stateAliases($var) == "del"} {
              switch -- $g_shellType {
                 csh {
                    puts stdout "unalias $var;"
                 }
                 sh {
                    puts stdout "unalias $var;"
                 }
              }
	   }
        }

	# new x resources
	if {[array size g_newXResources] > 0} {
	    set xrdb [findExecutable "xrdb"]
	    foreach var [array names g_newXResources] {
		set val $g_newXResources($var)
		if {$val == ""} {
		    switch -regexp -- $g_shellType {
		    {^(csh|sh)$} {
			    if {[file exists $var]} {
				puts stdout "$xrdb -merge $var;"
			    } else {
				puts stdout "$xrdb -merge <<EOF"
				puts stdout "$var"
				puts stdout "EOF;"
			    }
			}
		    perl {
			    if {[file isfile $var]} {
				puts stdout "system(\"$xrdb -merge $var\");"
			    } else {
				puts stdout "open(XRDB,\"|$xrdb -merge\");"
				set var [doubleQuoteEscaped $var]
				puts stdout "print XRDB \"$var\\n\";"
				puts stdout "close XRDB;"
			    }
			}
		    python {
			    if {[file isfile $var]} {
				puts stdout "os.popen('$xrdb -merge $var');"
			    } else {
				set var [singleQuoteEscaped $var]
				puts stdout "os.popen('$xrdb -merge').write('$var')"
			    }
			}
		    lisp {
			    if {[file exists $var]} {
				puts stdout "(shell-command-to-string \"$xrdb\
				  -merge $var\")"
			    } else {
				puts stdout "(shell-command-to-string \"echo $var\
				  | $xrdb -merge\")"
			    }
			}
		    }
		} else {
		    switch -regexp -- $g_shellType {
		    {^(csh|sh)$} {
			    puts stdout "$xrdb -merge <<EOF"
			    puts stdout "$var: $val"
			    puts stdout "EOF;"
			}
		    perl {
			    puts stdout "open(XRDB,\"|$xrdb -merge\");"
			    set var [doubleQuoteEscaped $var]
			    set val [doubleQuoteEscaped $val]
			    puts stdout "print XRDB \"$var: $val\\n\";"
			    puts stdout "close XRDB;"
			}
		    python {
			    set var [singleQuoteEscaped $var]
			    set val [singleQuoteEscaped $val]
			    puts stdout "os.popen('$xrdb\
			      -merge').write('$var: $val')"
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
	    foreach var [array names g_delXResources] {
		if {$val == ""} {
		    # do nothing
		} else {
		    puts stdout "xrdb -remove <<EOF"
		    puts stdout "$var:"
		    puts stdout "EOF;"
		}
	    }
	}

	if {[info exists g_systemList]} {
	    foreach var $g_systemList {
		puts stdout "$var;"
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
	    reportWarning "ERROR: $error_count error(s) detected."
	    switch -- $g_shellType {
	    csh {
		    puts stdout "/bin/false;"
		}
	    sh {
		    puts stdout "/bin/false;"
		}
	    cmd {
	            # nothing needed, reserve for future cygwin, MKS, etc
	        }
	    perl {
		    puts stdout "die \"modulefile.tcl: $error_count error(s)\
		      detected!\\n\""
		}
	    python {
		    puts stdout "raise RuntimeError, 'modulefile.tcl: $error_count error(s) detected!'"
		}
	    lisp {
		    puts stdout "(error \"modulefile.tcl: $error_count error(s)\
		      detected!\")"
		}
	    }
	    set nop 0
	} else {
	    switch -- $g_shellType {
	    perl {
		    puts stdout "1;"
		}
	    }
	}


	if {$nop} {
	    #	    nothing written!
	    switch -- $g_shellType {
	    csh {
		    puts "/bin/true;"
		}
	    sh {
		    puts "/bin/true;"
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
	} else {
	}
}

proc cacheCurrentModules {{separator {}}} {
    global g_loadedModules g_loadedModulesGeneric env g_def_separator g_debug


    if {$g_debug} {
	report "DEBUG cacheCurrentModules: ($separator)"
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }

    # mark specific as well as generic modules as loaded
    if {[info exists env(LOADEDMODULES)]} {
	foreach mod [split $env(LOADEDMODULES) $separator] {
	    set g_loadedModules($mod) 1
	    set g_loadedModulesGeneric([file dirname $mod]) [file tail $mod]
	}
    }
}

# This proc resolves module aliases or version aliases to the real module name\
  and version
proc resolveModuleVersionOrAlias {names} {
    global g_moduleVersion g_moduleDefault g_moduleAlias g_debug

    if {$g_debug} {
	report "DEBUG resolveModuleVersionOrAlias: Resolving $names"
    }
    set ret_list {}

    foreach name $names {
	# Chop off (default) if it exists
	set x [expr {[string length $name] - 9}]
	if {($x > 0) &&([string range $name $x end] == "\(default\)")} {
	    set name [string range $name 0 [expr {$x -1}]]
	    if {$g_debug} {
		report "DEBUG resolveModuleVersionOrAlias: trimming name =\
		  \"$name\""
	    }
	}
	if {[info exists g_moduleAlias($name)]} {
	    # if the alias is another alias, we need to resolve it
	    if {$g_debug} {
		report "DEBUG resolveModuleVersionOrAlias: $name is an alias"
	    }
	    set ret_list [linsert $ret_list end\
	      [resolveModuleVersionOrAlias $g_moduleAlias($name)]]
	}\
	elseif {[info exists g_moduleVersion($name)]} {
	    # if the pseudo version is an alias, we need to resolve it
	    if {$g_debug} {
		report "DEBUG resolveModuleVersionOrAlias: $name is a version\
		  alias"
	    }
	    set ret_list [linsert $ret_list end\
	      [resolveModuleVersionOrAlias $g_moduleVersion($name)]]
	}\
	elseif {[info exists g_moduleDefault($name)]} {
	    # if the default is an alias, we need to resolve it
	    if {$g_debug} {
		report "DEBUG resolveModuleVersionOrAlias: found a default for\
		  $name"
	    }
	    set ret_list [linsert $ret_list end [resolveModuleVersionOrAlias\
	      "$name/$g_moduleDefault($name)"]]
	} else {
	    if {$g_debug} {
		report "DEBUG resolveModuleVersionOrAlias: $name is nothing\
		  special"
	    }
	    set ret_list [linsert $ret_list end $name]
	}
    }
    if {$g_debug} {
	report "DEBUG resolveModuleVersionOrAlias: Resolved to $ret_list"
    }
    return $ret_list
}

proc spaceEscaped {text} {
    regsub -all " " $text "\\ " regsub_tmpstrg
    return $regsub_tmpstrg
}

proc multiEscaped {text} {
    regsub -all {([ \\\t\{\}|<>!;#^$&*"'`()])} $text {\\\1} regsub_tmpstrg
    return $regsub_tmpstrg
}

proc doubleQuoteEscaped {text} {
    regsub -all "\"" $text "\\\"" regsub_tmpstrg
    return $regsub_tmpstrg
}

proc atSymbolEscaped {text} {
    regsub -all "@" $text "\\@" regsub_tmpstrg
    return $regsub_tmpstrg
}

proc singleQuoteEscaped {text} {
    regsub -all "\'" $text "\\\'" regsub_tmpstrg
    return $regsub_tmpstrg
}

proc findExecutable {cmd} {
    foreach dir {/usr/X11R6/bin /usr/openwin/bin /usr/bin/X11} {
	if {[file executable "$dir/$cmd"]} {
	    return "$dir/$cmd"
	}
    }
    return $cmd
}

proc reverseList {list} {
    set newlist {}

    foreach item $list {
	set newlist [linsert $newlist 0 $item]
    }
    return $newlist
}

proc replaceFromList {list1 item {item2 {}}} {
     set xi [lsearch -exact $list1 $item]

     while {$xi >= 0} {
	 if {[string length $item2] == 0} {
             set list1 [lreplace $list1 $xi $xi]
         } else {
             set list1 [lreplace $list1 $xi $xi $item2]
         }
         set xi [lsearch -exact $list1 $item]
     }

     return $list1
}

proc checkValidModule {modfile} {
    global g_debug

    if {$g_debug} {
	report "DEBUG checkValidModule: $modfile"
    }

    # Check for valid module
    if {![catch {open $modfile r} fileId]} {
	gets $fileId first_line
	close $fileId
	if {[string first "\#%Module" $first_line] == 0} {
	    return 1
	}
    }
    return 0
}

# If given module maps to default or other version aliases, a list of 
# those aliases is returned.  This takes the full path to a module as
# an argument.
proc getVersAliasList {modulename} {
    global g_versionHash g_moduleDefault g_debug

    if {$g_debug} {
	report "DEBUG getVersAliasList: $modulename"
    }

    set modparent [file dirname $modulename]

    set tag_list {}
    if {[info exists g_versionHash($modulename)]} {
	# remove module basenames to get just version names
	foreach version $g_versionHash($modulename) {
	    set alias_tag [file tail $version]
	    set tag_list [linsert $tag_list end $alias_tag]
	}
    }
    if {[info exists g_moduleDefault($modparent)]} {
	set tmp_name "$modparent/$g_moduleDefault($modparent)"
	if {$tmp_name == $modulename} {
	    set tag_list [linsert $tag_list end "default"]
	}
    }
    return $tag_list
}

# Finds all module versions for mod in the module path dir
proc listModules {dir mod {full_path 1} {sort_order {-dictionary}}\
		  {flag_default_mf {1}} {flag_default_dir {1}}} {
    global ignoreDir
    global ModulesCurrentModulefile
    global g_debug
    global tcl_platform
    global g_versionHash

    # On Cygwin, glob may change the $dir path if there are symlinks involved
    # So it is safest to reglob the $dir.
    # example:
    # [glob /home/stuff] -> "//homeserver/users0/stuff"

    set dir [glob $dir]
    set full_list [glob -nocomplain "$dir/$mod"]

    # remove trailing / needed on some platforms
    regsub {\/$} $full_list {} full_list
	
    set clean_list {}
    set ModulesVersion {}
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

	set sstart [expr {[string length $dir] +1}]
	set modulename [string range $element $sstart end]


	if {[file isdirectory $element] && [file readable $element]} {
	    set ModulesVersion ""

	    if {$g_debug} {
		report "DEBUG listModules: found $element"
	    }

	    if {![info exists ignoreDir($tail)]} {
		# include .modulerc or if not present .version file
		if {[file readable $element/.modulerc]} {
		    lappend full_list $element/.modulerc
		}\
		elseif {[file readable $element/.version]} {
		    lappend full_list $element/.version
		}
		# Add each element in the current directory to the list
		foreach f [glob -nocomplain "$element/*"] {
		    lappend full_list $f
		}

		# if element is directory AND default or a version alias, add\
		  it to the list
		set tag_list [getVersAliasList $element]

		set tag {}
		if {[llength $tag_list]} {
		    append tag "(" [join $tag_list ":"] ")"

		    if {$full_path} {
			set mystr ${element}
		    } else {
			set mystr ${modulename}
		    }
		    if {[file isdirectory ${element}]} {
			if {$flag_default_dir} {
			    set mystr "$mystr$tag"
			}
		    }\
		    elseif {$flag_default_mf} {
			set mystr "$mystr$tag"
		    }
		    lappend clean_list $mystr

		}

	    }
	} else {
	    if {$g_debug} {
		report "DEBUG listModules: checking $element ($modulename)\
		  dir=$flag_default_dir mf=$flag_default_mf"
	    }
	    switch -glob -- $tail {
	    {.modulerc} {
		    if {$flag_default_dir || $flag_default_mf} {
			# set is needed for execute-modulerc
			set ModulesCurrentModulefile $element
			execute-modulerc $element
		    }
		}
	    {.version} {
		    if {$flag_default_dir || $flag_default_mf} {
			# set is needed for execute-modulerc
			set ModulesCurrentModulefile $element
			execute-modulerc "$element"

			if {$g_debug} {
			    report "DEBUG listModules: checking default\
			      $element"
			}
		    }
		}
	    {.*} -
	    {*~} -
	    {*,v} -
	    {\#*\#} { }
	    default {
		    if {[checkValidModule $element]} {

			set tag_list [getVersAliasList $element]

			set tag {}
			if {[llength $tag_list]} {
			    append tag "(" [join $tag_list ":"] ")"
			}
			if {$full_path} {
			    set mystr ${element}
			} else {
			    set mystr ${modulename}
			}
			if {[file isdirectory ${element}]} {
			    if {$flag_default_dir} {
				set mystr "$mystr$tag"
			    }
			}\
			elseif {$flag_default_mf} {
			    set mystr "$mystr$tag"
			}
			lappend clean_list $mystr
		    }
		}
	    }
	}
    }
    if {$sort_order != {}} {
	set clean_list [lsort $sort_order $clean_list]
    }
    if {$g_debug} {
	report "DEBUG listModules: Returning $clean_list"
    }

    return $clean_list
}


proc showModulePath {{separator {}}} {
    global env g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG showModulePath: $separator"
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }
    if {[info exists env(MODULEPATH)]} {
	report "Search path for module files (in search order):"
	foreach path [split $env(MODULEPATH) $separator] {
	    report "  $path"

	}
    } else {
	reportWarning "WARNING: no directories on module search path"
    }

}

########################################################################
# command line commands

proc cmdModuleList {{separator {}}} {
    global env DEF_COLUMNS show_oneperline show_modtimes g_debug
    global g_def_separator

    if {$separator == "" } {
        set separator $g_def_separator
    }

    if {[info exists env(LOADEDMODULES)]} {
        set loaded $env(LOADEDMODULES)
    } else {
        set loaded ""
    }

    if { [string length $loaded] == 0} { 
        report "No Modulefiles Currently Loaded."
    } else {
        set list {}
        report "Currently Loaded Modulefiles:"
        set max 0

        foreach mod [split $loaded $separator] {
            set len [string length $mod]

            if {$len > 0} {
	
	        if {$show_oneperline} {
	    	    report $mod
	        }\
	        elseif {$show_modtimes} {
		    set filetime [clock format [file mtime [lindex\
		      [getPathToModule $mod] 0]] -format "%Y/%m/%b %H:%M:%S"]
		    report [format "%-50s%10s" $mod $filetime]
	        } else {
		    if {$len > $max} {
		        set max $len
		    }
		    # skip zero length module names
		    # call getPathToModule to find and execute .version and\
		      .modulerc files for this module
		    getPathToModule $mod
		    set tag_list [getVersAliasList $mod]

		    if {[llength $tag_list]} {
			append mod "(" [join $tag_list $separator] ")"
			# expand string length to include version alises
			set len [string length $mod]
			if {$len > $max} {
			    set max $len
			}
		    }

		    lappend list $mod
                }
	    }
	}
	if {$show_oneperline ==0 && $show_modtimes == 0} {
	    # save room for numbers and spacing: 2 digits + ) + space + space
	    set cols [expr {int($DEF_COLUMNS/($max + 5))}]
	    # safety check to prevent divide by zero error below
	    if {$cols <= 0} {
		set cols 1
	    }

	    set item_cnt [llength $list]
	    set rows [expr {int($item_cnt / $cols)}]
	    set lastrow_item_cnt [expr {int($item_cnt % $cols)}]
	    if {$lastrow_item_cnt > 0} {
		incr rows
	    }
	    if {$g_debug} {
		report "list = $list"
		report "rows/cols = $rows/$cols,   max = $max"
		report "item_cnt = $item_cnt,  lastrow_item_cnt =\
		  $lastrow_item_cnt"
	    }
	    for {set row 0} {$row < $rows} {incr row} {
		for {set col 0} {$col < $cols} {incr col} {
		    set index [expr {$col * $rows + $row}]
		    set mod [lindex $list $index]

		    if {$mod != ""} {
			set n [expr {$index +1}]
			set mod [format "%2d) %-${max}s " $n $mod]
			report $mod -nonewline
		    }
		}
		report ""
	    }
	}
    }
}

proc cmdModuleDisplay {mod} {
    global env tcl_version ModulesCurrentModulefile

    set modfile [getPathToModule $mod]
    if {$modfile != ""} {
	pushModuleName [lindex $modfile 1]
	set modfile [lindex $modfile 0]
	report\
	  "-------------------------------------------------------------------"
	report "$modfile:\n"
	pushMode "display"
	execute-modulefile $modfile
	popMode
	popModuleName
	report\
	  "-------------------------------------------------------------------"
    }
}

proc cmdModulePaths {mod {separator {}}} {
    global env g_pathList flag_default_mf flag_default_dir g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModulePaths: ($mod, $separator)"
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }

    if {[catch {
	foreach dir [split $env(MODULEPATH) $separator] {
	    if {[file isdirectory $dir]} {
		foreach mod2 [listModules $dir $mod 0 "" $flag_default_mf\
		  $flag_default_dir] {
		    lappend g_pathList $mod2
		}
	    }
	}
    } errMsg]} {
	reportWarning "ERROR: module paths $mod failed. $errMsg"
    }
}

proc cmdModulePath {mod} {
    global env g_pathList ModulesCurrentModulefile g_debug

    if {$g_debug} {
	report "DEBUG cmdModulePath: ($mod)"
    }
    set modfile [getPathToModule $mod]
    if {$modfile != ""} {
	set modfile [lindex $modfile 0]
	set ModulesCurrentModulefile $modfile

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
    global env tcl_version ModulesCurrentModulefile
    global g_whatis g_def_separator g_debug


    if {$g_debug} {
	report "DEBUG cmdModuleSearch: ($mod, $search)"
    }
    if {$mod == ""} {
	set mod "*"
    }
    foreach dir [split $env(MODULEPATH) $g_def_separator] {
	if {[file isdirectory $dir]} {
	    report "----------- $dir ------------- "
	    set modlist [listModules $dir $mod 0 "" 0 0]
	    foreach mod2 $modlist {
		set g_whatis ""
		set modfile [getPathToModule $mod2]
		if {$modfile != ""} {
		    pushMode "whatis"
		    pushModuleName [lindex $modfile 1]
		    set modfile [lindex $modfile 0]
		    execute-modulefile $modfile
		    popMode
		    popModuleName
		    if {$search =="" || [regexp -nocase $search $g_whatis]} {
			report [format "%20s: %s" $mod2 $g_whatis]
		    }
		}
	    }
	}
    }
}

proc cmdModuleSwitch {old {new {}}} {
    global env g_debug g_loadedModulesGeneric g_loadedModules

    if {$new == ""} {
	set new $old
    } elseif {[info exists g_loadedModules($new)]} {
	set tmp $new
	set new $old
	set old $tmp
    }

    if {![info exists g_loadedModules($old)] &&
	 [info exists g_loadedModulesGeneric($old)]} {
	set old "$old/$g_loadedModulesGeneric($old)"
    }

    if {$g_debug} {
	report "DEBUG cmdModuleSwitch: new=\"$new\" old=\"$old\""
    }

    cmdModuleUnload $old
    cmdModuleLoad $new
}

proc cmdModuleSource {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleSource: $args"
    }
    foreach file $args {
	if {[file exists $file]} {
	    pushMode "load"
	    pushModuleName $file
	    execute-modulefile $file
	    popModuleName
	    popMode
	} else {
	    error "File $file does not exist"
	}
    }
}

proc cmdModuleLoad {args} {
    global env g_loadedModules g_loadedModulesGeneric g_force
    global ModulesCurrentModulefile
    global g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleLoad: loading $args"
    }

    foreach mod $args {
	set modfile [getPathToModule $mod]
	if {$modfile != ""} {
	    set currentModule [lindex $modfile 1]
	    set modfile [lindex $modfile 0]
	    set ModulesCurrentModulefile $modfile

	    if {$g_force || ! [info exists g_loadedModules($currentModule)]} {
		pushMode "load"
		pushModuleName $currentModule

		saveSettings
		if {[execute-modulefile $modfile]} {
		    restoreSettings
		} else {
		    append-path LOADEDMODULES $currentModule
		    append-path _LMFILES_ $modfile
		    set g_loadedModules($currentModule) 1
		    set genericModName [file dirname $mod]

		    if {![info exists\
		      g_loadedModulesGeneric($genericModName)]} {
			set g_loadedModulesGeneric($genericModName) [file tail\
			  $currentModule]
		    }
		}

		popMode
		popModuleName
	    }
	}
    }
}

proc cmdModuleUnload {args} {
    global tcl_version g_loadedModules g_loadedModulesGeneric
    global ModulesCurrentModulefile g_debug g_def_separator

    if {$g_debug} {
	report "DEBUG cmdModuleUnload: unloading $args"
    }

    foreach mod $args {
	if {[catch {
	    set modfile [getPathToModule $mod]
	    if {$modfile != ""} {
		set currentModule [lindex $modfile 1]
		set modfile [lindex $modfile 0]
		set ModulesCurrentModulefile $modfile

		if {[info exists g_loadedModules($currentModule)]} {
		    pushMode "unload"
		    pushModuleName $currentModule
		    saveSettings
		    if {[execute-modulefile $modfile]} {
			restoreSettings
		    } else {
			unload-path LOADEDMODULES $currentModule $g_def_separator
			unload-path _LMFILES_ $modfile $g_def_separator
			unset g_loadedModules($currentModule)
			if {[info exists g_loadedModulesGeneric([file dirname\
			  $currentModule])]} {
			    unset g_loadedModulesGeneric([file dirname\
			      $currentModule])
			}
		    }

		    popMode
		    popModuleName
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
	    reportWarning "ERROR: module: module unload $mod failed.\n$errMsg"
	}
    }
}


proc cmdModulePurge {{separator {}}} {
    global env g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModulePurge: $separator"
    }
    if {$separator == "" } {
        set separator $g_def_separator
    }

    if {[info exists env(LOADEDMODULES)]} {
	set list [split $env(LOADEDMODULES) $separator]
	eval cmdModuleUnload [reverseList $list]
    }
}


proc cmdModuleReload {{separator {}}} {
    global env g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleReload: $separator"
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }

    if {[info exists env(LOADEDMODULES)]} {
	set list [split $env(LOADEDMODULES) $separator]
	set rlist [reverseList $list]
	foreach mod $rlist {
	    cmdModuleUnload $mod
	}
	foreach mod $list {
	    cmdModuleLoad $mod
	}
    }
}

proc cmdModuleAliases {} {

    global DEF_COLUMNS g_moduleAlias g_moduleVersion g_debug

    set label "Aliases"
    set len  [string length $label]
    set lrep [expr {($DEF_COLUMNS - $len - 2)/2}]
    set rrep [expr {$DEF_COLUMNS - $len - 2 - $lrep}]

    report "[string repeat {-} $lrep] $label [string repeat {-} $rrep]"

    foreach name [lsort -dictionary [array names g_moduleAlias]] {
       report "$name -> $g_moduleAlias($name)"
    }

    set label "Versions"
    set len  [string length $label]
    set lrep [expr {($DEF_COLUMNS - $len - 2)/2}]
    set rrep [expr {$DEF_COLUMNS - $len - 2 - $lrep}]

    report "[string repeat {-} $lrep] $label [string repeat {-} $rrep]"

    foreach name [lsort -dictionary [array names g_moduleVersion]] {
        report "$name -> $g_moduleVersion($name)"
    }
}

proc system {mycmd args} {
    global g_systemList g_debug

    if {$g_debug} {
	report "DEBUG system: $mycmd $args"
    }
    set mode [currentMode]
    set mycmd [join [concat $mycmd $args] " "]

    if {$mode == "load"} {
	lappend g_systemList $mycmd
    }\
    elseif {$mode == "unload"} {
	# No operation here unable to undo a syscall.
    }\
    elseif {$mode == "display"} {
	report "system\t\t$mycmd"
    }
    return {}
}

proc cmdModuleAvail {{mod {*}}} {
    global env ignoreDir DEF_COLUMNS flag_default_mf flag_default_dir
    global show_oneperline show_modtimes g_def_separator

    if {$show_modtimes} {
	report "- Package -----------------------------.- Versions -.- Last\
	  mod. ------"
    }

    foreach dir [split $env(MODULEPATH) $g_def_separator] {
	if {[file isdirectory "$dir"] && [file readable $dir]} {
	    set len  [string length $dir]
	    set lrep [expr {($DEF_COLUMNS - $len - 2)/2}]
	    set rrep [expr {$DEF_COLUMNS - $len - 2 - $lrep}]
	    report "[string repeat {-} $lrep] $dir [string repeat {-} $rrep]"
	    set list [listModules "$dir" "$mod" 0 "" $flag_default_mf\
	      $flag_default_dir]
            # sort names (sometimes? returned in the order as they were
            # created on disk :-)
            set list [lsort $list]
	    if {$show_modtimes} {
		foreach i $list {
                    # don't change $i with the regsub - we need it 
                    # to figure out the file time.
                    regsub {\(default\)} $i "   (default)" i2 
		    set filetime [clock format [file mtime [lindex [getPathToModule $i] 0]] -format "%Y/%m/%b %H:%M:%S" ]
		    report [format "%-53s%10s" $i2 $filetime]
		}
	    }\
	    elseif {$show_oneperline} {
		foreach i $list {
                    regsub {\(default\)} $i "   (default)" i2 
		    report "$i2"
		}
	    } else {
		set max 0
		foreach mod2 $list {
		    if {[string length $mod2] > $max} {
			set max [string length $mod2]
		    }
		}
		incr max 1
		set cols [expr {int($DEF_COLUMNS / $max)}]
		# safety check to prevent divide by zero error below
		if {$cols <= 0} {
		    set cols 1
		}

		# There is no '{}' at the begining of this 'list' as there is\
		  in cmd 
		#               ModuleList - ?
		set item_cnt [expr {[llength $list] - 0}]
		set rows [expr {int($item_cnt / $cols)}]
		set lastrow_item_cnt [expr {int($item_cnt % $cols)}]
		if {$lastrow_item_cnt > 0} {
		    incr rows
		}
		for {set row 0} {$row < $rows} {incr row} {
		    for {set col 0} {$col < $cols} {incr col} {
			set index [expr {$col * $rows + $row}]
			set mod2 [lindex $list $index]
			if {$mod2 != ""} {
			    set mod2 [format "%-${max}s" $mod2]
			    report $mod2 -nonewline
			}
		    }
		    report ""
		}
	    }
	}
    }
}

proc cmdModuleUse {args} {
	global env g_debug g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleUse: $args"
    }

    if {$args == ""} {
	showModulePath
    } else {
	set stuff_path "prepend"
	foreach path $args {
	    if {$path == ""} {
		# Skip "holes"
	    }\
	    elseif {($path == "--append") ||($path == "-a") ||($path ==\
	      "-append")} {
		set stuff_path "append"
	    }\
	    elseif {($path == "--prepend") ||($path == "-p") ||($path ==\
	      "-prepend")} {
		set stuff_path "prepend"
	    }\
	    elseif {[file isdirectory $path]} {
		if {$g_debug} {
			report "DEBUG cmdModuleUse: calling add-path \
				MODULEPATH $path $stuff_path $g_def_separator"
		}

		pushMode "load"
		catch {
			add-path MODULEPATH $path $stuff_path $g_def_separator
		}
		popMode
	    } else {
		report "+(0):WARN:0: Directory '$path' not found"
	    }
	}
    }
}

proc cmdModuleUnuse {args} {
    global g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleUnuse: $args"
    }
    if {$args == ""} {
	showModulePath
    } else {
	global env
	foreach path $args {
	    regsub -all {\/} $path {\/} escpath
	    set regexp [subst {(^|\:)${escpath}(\:|$)}]
	    if {[info exists env(MODULEPATH)] && [regexp $regexp\
	      $env(MODULEPATH)]} {

		set oldMODULEPATH $env(MODULEPATH)

		if {$g_debug} {
			report "calling unload-path MODULEPATH $path $g_def_separator"
		}

		pushMode "unload"
		catch {
		    unload-path MODULEPATH $path $g_def_separator
		}
		popMode
		if {[info exists env(MODULEPATH)] && $oldMODULEPATH ==\
		  $env(MODULEPATH)} {
		    reportWarning "WARNING: Did not unuse $path"
		}
	    }
	}
    }
}

proc cmdModuleDebug {{separator {}}} {
    global env g_def_separator g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleDebug: $separator"
    }

    if {$separator == "" } {
        set separator $g_def_separator
    }

    foreach var [array names env] {
	array set countarr [getReferenceCountArray $var $separator]

	foreach path [array names countarr] {
	    report "$var\t$path\t$countarr($path)"
	}
	unset countarr
    }
    foreach dir [split $env(PATH) $separator] {
	foreach file [glob -nocomplain -- "$dir/*"] {
	    if {[file executable $file]} {
		set exec [file tail $file]
		lappend execcount($exec) $file
	    }
	}
    }
    foreach file [lsort -dictionary [array names execcount]] {
	if {[llength $execcount($file)] > 1} {
	    report "$file:\t$execcount($file)"
	}
    }
}

proc cmdModuleAutoinit {} {
    global g_autoInit g_debug

    if {$g_debug} {
	report "DEBUG cmdModuleAutoinit:"
    }
    set g_autoInit 1
}

proc cmdModuleInit {args} {

    global g_shell env g_debug
    set moduleinit_cmd [lindex $args 0]
    set notdone 1
    set notclear 1

    if {$g_debug} {
	report "DEBUG cmdModuleInit: $args"
    }

    # Define startup files for each shell
    set files(csh) [list ".modules" ".cshrc" ".cshrc_variables" ".login"]
    set files(tcsh) [list ".modules" ".tcshrc" ".cshrc" ".cshrc_variables"\
      ".login"]
    set files(sh) [list ".modules" ".bash_profile" ".bash_login" ".profile"\
      ".bashrc"]
    set files(bash) $files(sh)
    set files(ksh) $files(sh)
    set files(zsh) [list ".modules" ".zshrc" ".zshenv" ".zlogin"]

    array set nargs {
	list    0
	add     1
	load    1
	prepend 1
	rm      1
	unload  1
	switch  2
	clear   0
    }

    # Process startup files for this shell
    set current_files $files($g_shell)
    foreach filename $current_files {
	if {$notdone && $notclear} {
	    set filepath $env(HOME)
	    append filepath "/" $filename
	    # create a new file to put the changes in
	    set newfilepath "$filepath-NEW"

	    if {$g_debug} {
		report "DEBUG Looking at: $filepath"
	    }
	    if {[file readable $filepath] && [file isfile $filepath]} {
		set fid [open $filepath r]

		set temp [expr {[llength $args] -1}]
		if {$temp != $nargs($moduleinit_cmd)} {
		    error "'module init$moduleinit_cmd' requires exactly\
		      $nargs($moduleinit_cmd) arg(s)."
		    #	       cmdModuleHelp
		    exit -1
		}

		# Only open the new file if we are not doing "initlist"
		if {[string compare $moduleinit_cmd "list"] != 0} {
		    set newfid [open $newfilepath w]
		}

		while {[gets $fid curline] >= 0} {
		    # Find module load/add command in startup file 
		    set comments {}
		    if {$notdone && [regexp {^([ \t]*module[ \t]+(load|add)[\
		      \t]+)(.*)} $curline match cmd subcmd modules]} {
			regexp {([ \t]*\#.+)} $modules match comments
			regsub {\#.+} $modules {} modules
			# remove existing references to the named module from\
			  the list
			# Change the module command line to reflect the given\
			  command
			switch $moduleinit_cmd {
			list {
				report "$g_shell initialization file $filepath\
				  loads modules: $modules"
			    }
			add {
				set newmodule [lindex $args 1]
				set modules [replaceFromList $modules $newmodule]
				append modules " $newmodule"
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			prepend {
				set newmodule [lindex $args 1]
				set modules [replaceFromList $modules $newmodule]
				set modules "$newmodule $modules"
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			rm {
				set oldmodule [lindex $args 1]
				set modules [replaceFromList $modules $oldmodule]
				if {[llength $modules] == 0} {
				    set modules ""
				}
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			switch {
				set oldmodule [lindex $args 1]
				set newmodule [lindex $args 2]
				set modules [replaceFromList $modules\
				  $oldmodule $newmodule]
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			clear {
				set modules ""
				puts $newfid "$cmd$modules$comments"
				set notclear 0
			    }
			default {
				report "Command init$moduleinit_cmd not\
				  recognized"
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
		    if {[catch {file copy -force $filepath $filepath-OLD}] !=\
		      0} {
			report "Failed to back up original $filepath...exiting"
			exit -1
		    }
		    if {[catch {file copy -force $newfilepath $filepath}] !=\
		      0} {
			report "Failed to write $filepath...exiting"
			exit -1
		    }
		}
	    }
	}
    }
}

proc cmdModuleHelp {args} {
    global done MODULES_CURRENT_VERSION

    set done 0
    foreach arg $args {
	if {$arg != ""} {
	    set modfile [getPathToModule $arg]

	    if {$modfile != ""} {
		pushModuleName [lindex $modfile 1]
		set modfile [lindex $modfile 0]
		report\
		  "-------------------------------------------------------------------"
		report "Module Specific Help for $modfile:\n"
		set mode "Help"
		execute-modulefile $modfile 1
		popMode
		popModuleName
		report\
		  "-------------------------------------------------------------------"
	    }
	    set done 1
	}
    }
    if {$done == 0} {
	report "Modules Release Tcl $MODULES_CURRENT_VERSION " 1
        report {	Copyright GNU GPL v2 1991}
	report {Usage: module [ command ]}

	report {Commands:}
	report {	list                     [switches] modulefile\
	  [modulefile ...]}
	report {	display  |  show                    modulefile\
	  [modulefile ...]}
        report {	add  |  load                        modulefile\
	  [modulefile ...]}
	report {	purge  |  rm  |  unload             modulefile\
	  [modulefile ...]}
	report {	reload                              modulefile\
	  [modulefile ...]}
        report {	switch  |  swap                    \
	  [oldmodulefile] newmodulefile}
	report {	avail                    [switches] [modulefile\
	  [modulefile ...]]}
        report {	aliases}
	report {	whatis                              [modulefile\
	  [modulefile ...]]}
	report {	help                                [modulefile\
	  [modulefile ...]]}
	report {	path                                modulefile}
	report {	paths                               modulefile}
	report {	initlist                            modulefile}
        report {	initadd                             modulefile}
        report {	initrm                              modulefile}
	report {	initclear                           modulefile}
	report {	initprepend                         modulefile}
	report {	use                                 dir [dir ...]}
	report {	unuse                               dir [dir ...]}
	report {	source                              scriptfile}
	report {	apropos  |  keyword  | search       string}
	report {Switches:}
	report {	-t		terse format avail and list}
	report {	-l		long format avail and list}
    }
}


########################################################################
# main program

# needed on a gentoo system. Shouldn't hurt since it is
# supposed to be the default behavior
fconfigure stderr -translation auto

if {$g_debug} {
	report "CALLING $argv0 $argv"
}

# Parse options
set opt [lindex $argv 1]
switch -regexp -- $opt {
    {^(-deb|--deb)} {

        if {!$g_debug} {
		report "CALLING $argv0 $argv"
	}

	set g_debug 1
	report "DEBUG debug enabled"

	set argv [replaceFromList $argv $opt]
    }
    {^(--help|-h)} {
        cmdModuleHelp
        exit 0
    }
    {^(-V|--ver)} {
	report "Modules Release Tcl $MODULES_CURRENT_VERSION"
	exit 0
    }
    {^--} {
	report "+(0):ERROR:0: Unrecognized option '$opt'"
	exit -1
    }
}

set g_shell [lindex $argv 0]
set command [lindex $argv 1]
set argv [lreplace $argv 0 1]

switch -regexp -- $g_shell {
    ^(sh|bash|ksh|zsh)$ {
	set g_shellType sh
    }
    ^(cmd)$ {
        set g_shellType cmd
    }
    ^(csh|tcsh)$ {
	set g_shellType csh
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
	error " +(0):ERROR:0: Unknown shell type \'($g_shell)\'"
    }
}

cacheCurrentModules

# Find and execute any .modulerc file found in the module directories defined\
  in env(MODULESPATH)
runModulerc
# Resolve any aliased module names - safe to run nonmodule arguments
if {$g_debug} {
    report "DEBUG Resolving $argv"
}

if {[lsearch $argv "-t"] >= 0} {
    set show_oneperline 1
    set argv [replaceFromList $argv "-t"]
}
if {[lsearch $argv "-l"] >= 0} {
    set show_modtimes 1
    set argv [replaceFromList $argv "-l"]
}
set argv [resolveModuleVersionOrAlias $argv]
if {$g_debug} {
    report "DEBUG Resolved $argv"
}

if {[catch {
    switch -regexp -- $command {
    {^av} {
	    if {$argv != ""} {
		foreach arg $argv {
		    cmdModuleAvail $arg
		}
	    } else {
		cmdModuleAvail
   	        cmdModuleAliases
	    }
	}
    {^al} {
            cmdModuleAliases
        }
    {^li} {
	    cmdModuleList
	}
    {^(di|show)} {
	    foreach arg $argv {
		cmdModuleDisplay $arg
	    }
	}
    {^(add|lo)} {
	    eval cmdModuleLoad $argv
	    renderSettings
	}
    {^source} {
	    eval cmdModuleSource $argv
	    renderSettings
	}
    {^paths} {
	    # HMS: We probably don't need the eval
	    eval cmdModulePaths $argv
	    renderSettings
	}
    {^path} {
	    # HMS: We probably don't need the eval
	    eval cmdModulePath $argv
	    renderSettings
	}
    {^pu} {
	    cmdModulePurge
	    renderSettings
	}
    {^sw} {
	    eval cmdModuleSwitch $argv
	    renderSettings
	}
    {^(rm|unlo)} {
	    eval cmdModuleUnload $argv
	    renderSettings
	}
    {^use$} {
	    eval cmdModuleUse $argv
	    renderSettings
	}
    {^unuse$} {
	    eval cmdModuleUnuse $argv
	    renderSettings
	}
    {^wh} {
	    if {$argv != ""} {
		foreach arg $argv {
		    cmdModuleWhatIs $arg
		}
	    } else {
		cmdModuleWhatIs
	    }
	}
    {^(apropos|search|keyword)$} {
	    eval cmdModuleApropos $argv
	}
    {^debug$} {
	    eval cmdModuleDebug
	}
    {^rel} {
	    cmdModuleReload
	    renderSettings
	}
    {^init(add|lo)$} {
	    eval cmdModuleInit add $argv
	}
    {^initprepend$} {
	    eval cmdModuleInit prepend $argv
	}
    {^initswitch$} {
	    eval cmdModuleInit switch $argv
	}
    {^init(rm|unlo)$} {
	    eval cmdModuleInit rm $argv
	}
    {^initlist$} {
	    eval cmdModuleInit list $argv
	}
    {^initclear$} {
	    eval cmdModuleInit clear $argv
	}
    {^autoinit$} {
	    cmdModuleAutoinit
	    renderSettings
	}
    {^($|help)} {
	    cmdModuleHelp $argv
	}
    . {
	    reportWarning "ERROR: command '$command' not recognized"
	    cmdModuleHelp $argv
	}
    }
} errMsg ]} {
    reportWarning "ERROR: $errMsg"
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
