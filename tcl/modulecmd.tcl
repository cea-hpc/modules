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
#

########################################################################
# Use a slave TCL interpreter to execute modulefiles
#

proc unset-env {var} {
    global env

    if [info exists env($var)] {
        unset env($var)
    }
}

proc execute-modulefile-help {modfile} {
    global env g_stateEnvVars

    #puts stderr "Starting $modfile"
    set slave [currentModuleName]
    if { ![interp exists $slave] } {
	interp create $slave
	interp alias $slave setenv        {} setenv
	interp alias $slave unsetenv      {} unsetenv
	interp alias $slave append-path   {} append-path
	interp alias $slave prepend-path  {} prepend-path
	interp alias $slave remove-path   {} remove-path
	interp alias $slave prereq        {} prereq
	interp alias $slave conflict      {} conflict
	interp alias $slave is-loaded     {} is-loaded
	interp alias $slave module        {} module
	interp alias $slave module-info   {} module-info
	interp alias $slave module-whatis {} module-whatis
	interp alias $slave set-alias     {} set-alias
	interp alias $slave unset-alias   {} unset-alias
	interp alias $slave uname         {} uname
	interp alias $slave x-resource    {} x-resource
	interp alias $slave module-version         {} module-version
	interp alias $slave module-alias  {} module-alias
	
	interp eval $slave [list global ModulesCurrentModulefile]
	interp eval $slave [list set ModulesCurrentModulefile $modfile]
	
    }
    set errorVal [interp eval $slave {
	if [catch {source $ModulesCurrentModulefile} errorMsg] {
	    if {$errorMsg == "" && $errorInfo == ""} {
		unset errorMsg
		return 1
	    } elseif [regexp "^WARNING" $errorMsg] {
		puts stderr $errorMsg
		return 1
	    } else {
		global errorInfo
		set errorMsg "ERROR occured in file $ModulesCurrentModulefile."
		set errorMsg "$errorMsg\nContact your local modulefile maintainer."
		set errorMsg "$errorMsg\n----------errorInfo----------\n$errorInfo"
		puts stderr $errorMsg
		exit 1
	    }
	} else {
	    ModulesHelp
	    unset errorMsg
	    return 0
	}
    }]
    interp delete $slave
    #puts stderr "Exiting $modfile"
    return $errorVal
}

proc execute-modulefile {modfile} {
    global env g_stateEnvVars

#    puts stderr "Starting $modfile"
    set slave [currentModuleName]
    if { ![interp exists $slave] } {
	interp create $slave
	interp alias $slave setenv        {} setenv       
	interp alias $slave unsetenv      {} unsetenv     
	interp alias $slave append-path   {} append-path  
	interp alias $slave prepend-path  {} prepend-path 
	interp alias $slave remove-path   {} remove-path  
	interp alias $slave prereq        {} prereq       
	interp alias $slave conflict      {} conflict     
	interp alias $slave is-loaded     {} is-loaded    
	interp alias $slave module        {} module       
	interp alias $slave module-info   {} module-info  
	interp alias $slave module-whatis {} module-whatis
	interp alias $slave set-alias     {} set-alias    
	interp alias $slave unset-alias   {} unset-alias  
	interp alias $slave uname         {} uname        
	interp alias $slave x-resource    {} x-resource   
# BOZO Is this needed in regular modules?     	interp alias $slave module-version         {} module-version
# BOZO Is this needed in regular modules?	interp alias $slave module-alias  {} module-alias

	interp eval $slave [list global ModulesCurrentModulefile]
	interp eval $slave [list set ModulesCurrentModulefile $modfile]

    }
    set errorVal [interp eval $slave {
        if [catch {source $ModulesCurrentModulefile} errorMsg] {
            if {$errorMsg == "" && $errorInfo == ""} {
                unset errorMsg
                return 1
            } elseif [regexp "^WARNING" $errorMsg] {
                puts stderr $errorMsg
                return 1
            } else {
                global errorInfo
                set errorMsg "ERROR occured in file $ModulesCurrentModulefile."
                set errorMsg "$errorMsg\nContact your local modulefile maintainer."
                set errorMsg "$errorMsg\n----------errorInfo----------\n$errorInfo"
                puts stderr $errorMsg
                exit 1
            }
        } else {
            unset errorMsg
            return 0
        }
    }]
    interp delete $slave
    #puts stderr "Exiting $modfile"
    return $errorVal
}

proc execute-modulerc {modfile} {
# BOZO - ajb: I'm not sure what functions should be supported in a .modulerc file.  For now it's a small subset
    global env g_stateEnvVars g_rcfilesSourced
    global g_debug

    # If we have already loaded an RC file, don't load it again - BOZO may want this to add to execute-version
    if { ![info exists g_rcfilesSourced($modfile)]} {
	if { $g_debug } { puts stderr "DEBUG execute-modulerc: sourcing rc $modfile" }
	set slave .modulerc
	if { ![interp exists $slave] } {
	    interp create $slave
	    interp alias $slave uname         {} uname        
	    interp alias $slave module-version         {} module-version
	    interp alias $slave module-alias  {} module-alias
	    interp alias $slave module         {} module
	    interp  eval $slave [list global ModulesCurrentModulefile]
	    interp  eval $slave [list set ModulesCurrentModulefile $modfile]
	}
	set ModulesVersion [interp eval $slave {
	    if [catch {source $ModulesCurrentModulefile} errorMsg] {
		set errorMsg "ERROR occured in file $ModulesCurrentModulefile."
		global errorInfo
		set errorMsg "$errorMsg\nContact your local modulefile maintainer."
		set errorMsg "$errorMsg\n----------errorInfo----------\n$errorInfo"
		puts stderr $errorMsg
		exit 1
	    }
	}]
	interp delete $slave
	# Keep track of rc files that we already source so we don't run them again
	set g_rcfilesSourced($modfile) 1
    }
}


proc execute-version {modfile} {
    global env g_stateEnvVars

    set slave .version
    if { ![interp exists $slave] } {
	interp create $slave
	interp alias $slave uname         {} uname        
	interp alias $slave module-version         {} module-version
	interp alias $slave module-alias  {} module-alias
	interp  eval $slave [list global ModulesCurrentModulefile]
	interp  eval $slave [list set ModulesCurrentModulefile $modfile]
	interp  eval $slave [list global ModulesVersion]
	interp  eval $slave [list set ModulesVersion {}]
    }
    set ModulesVersion [interp eval $slave {
	if [catch {source $ModulesCurrentModulefile} errorMsg] {
	    set errorMsg "ERROR occured in file $ModulesCurrentModulefile."
            global errorInfo
            set errorMsg "$errorMsg\nContact your local modulefile maintainer."
            set errorMsg "$errorMsg\n----------errorInfo----------\n$errorInfo"
            puts stderr $errorMsg
            exit 1
        } elseif [info exists ModulesVersion] {
            return $ModulesVersion
        } else {
            return {}
        }
    }]
    interp delete $slave
    return $ModulesVersion
}


########################################################################
# commands run from inside a module file
#

global g_shellType
global g_shell

set ignoreDir(CVS) 1
set ignoreDir(RCS) 1
set ignoreDir(SCCS) 1
set g_reloadMode  0
set error_count  0
set g_force 1
set ModulesCurrentModulefile {}

proc module-info {what {more {}}} {
    global g_shellType g_shell

    set mode [currentMode]
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
	"name" {
	    return [currentModuleName]
	}
        "shell" {
            return $g_shell
        }
        "shelltype" {
            return $g_shellType
        }
	default {
	    error "module-info $what not supported"
            return {}
	}
    }
}

proc module-whatis {message} {
    set mode [currentMode]

    global g_whatis

    if {$mode == "display"} {
	report "module-whatis\t$message"
    } elseif {$mode == "whatis"} {
	set g_whatis $message
    }
    return {}
}

proc module-version {args} {
# Specifies a default or alias version for a module that points to an existing module version
    global g_moduleVersion
    global g_moduleDefault
    global g_debug

    if { $g_debug } { puts stderr "DEBUG module-version: executing module-version $args" }
    set module_file [ lindex $args 0]
    foreach version [lrange $args 1 end ] {
	# Default version
	if { [ string compare $version "default"] == 0} {
	    if { [ regexp {(\S+)\/(\S+)} $module_file match base defversion] } {
		# If we see more than one default for the same module, just keep the first
		if { ![info exists g_moduleDefault($base)] } {
		    set g_moduleDefault($base) $defversion
		    if { $g_debug } { puts stderr "DEBUG module-verions: default $base = $defversion" }
		}
	    } else {
		error "module-version: module argument for default must not be fully version qualified"
	    }
	} else {
	    # alias version
	    if { [ regexp {(\S+)\/\S+} $module_file match base ] } {
		set aliasversion "$base/$version"
		set g_moduleVersion($aliasversion) $module_file
		if { $g_debug } { puts stderr "DEBUG module-version: $aliasversion  = $module_file" }
	    } else {
		error "module-version: module argument needs a fully qualifed version"
	    }
	}
    }
    return {}
}


proc module-alias {args} {
    global g_moduleAlias

    set module_file [ lindex $args 0]
    set alias [ lindex $args 1 ]

    set g_moduleAlias($alias) $module_file

    return {}
}
			    

proc module {command args} {
    set mode [currentMode]
    global g_reloadMode g_debug

    if { $g_reloadMode == 1} {
        puts stderr "g_reloadMode is set"
	return
    }

    # Resolve any module aliases
    if { $g_debug } { puts stderr "DEBUG module: Resolving $args" }
    set args [resolveModuleVersionOrAlias $args]
    if { $g_debug } { puts stderr "DEBUG module: Resolved to $args" }

    switch -- $command {
	add -
	load {
	    if {$mode == "load"} {
		eval cmdModuleLoad $args
	    } elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    } elseif {$mode == "display"} {
		report "module load $args"
	    }
	}
	rm -
	unload {
	    if {$mode == "load"} {
		eval cmdModuleUnload $args
	    } elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    } elseif {$mode == "display"} {
		report "module unload $args"
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
	switch {
	    eval cmdModuleSwitch $args
	}
	swap {
	    eval cmdModuleSwitch $args
	}
	display {
	    eval cmdModuleDisplay $args
	}
	show {
	    eval cmdModuleDisplay $args
	}
	avail {
	    if {$args != ""} {
		foreach arg $args {
		    cmdModuleAvail $arg
		}
	    } else {
		cmdModuleAvail
	    }
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
	apropos {
	    eval cmdModuleApropos $args
	}
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
    global g_stateEnvVars env
    set mode [currentMode]

    if {$mode == "load"} {
	set env($var) $val
	set g_stateEnvVars($var) "new"
    } elseif {$mode == "unload"} {
        # Don't unset-env here ... it breaks modulefiles
        # that use env(var) is later in the modulefile
        #unset-env $var
	set g_stateEnvVars($var) "del"
    } elseif {$mode == "display"} {
	report "setenv\t$var\t$val"
    }
    return {}
}

proc unsetenv {var {val {}}} {
    global g_stateEnvVars env
    set mode [currentMode]

    if {$mode == "load"} {
	if [info exists env($var)] {
	    unset-env $var
	}
	set g_stateEnvVars($var) "del"
    } elseif {$mode == "unload"} {
        if {$val != ""} {
            set env($var) $val
            set g_stateEnvVars($var) "new"
        }
    } elseif {$mode == "display"} {
	report "unsetenv\t$var"
    }
    return {}
}

########################################################################
# path fiddling

proc getReferenceCountArray {var} {
    global env g_force

    set sharevar "${var}_modshare"
    set modshareok 1
    if [info exists env($sharevar)] {
	if [info exists env($var)] {
	    set modsharelist [split $env($sharevar) ":"]
	    if {[expr [llength $modsharelist] % 2] == 0 } {
		array set countarr $modsharelist

		# sanity check the modshare list
		array set fixers {}
		array set usagearr {}
		foreach dir [split $env($var) ":"] {
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
#			set fixers($path) 1
		    }
		}

		if {! $g_force} {
		  if [array size fixers] {
		      reportWarning "WARNING: \$$var does not agree with \$${var}_modshare counter. The following directories' usage counters were adjusted to match. Note that this may mean that module unloading may not work correctly."
		    foreach dir [array names fixers] {
			report " $dir" -nonewline
		    }
		      reportWarning ""
		  }
		}

	    } else {
#		sharevar was corrupted, odd number of elements.
		set modshareok 0
	    }
	} else {
#	    WARNING: module: $sharevar exists ( $env($sharevar) ), but $var doesn't. Environment is corrupted. Contact lakata@mips.com for help.
	    set modshareok 0
	}
    } else {
	set modshareok 0
    }

    if {$modshareok == 0 && [info exists env($var)]} {
	array set countarr {}
	foreach dir [split $env($var) ":"] {
	    set countarr($dir) 1
	}
    }
    return [array get countarr]
}


proc unload-path {var path} {
    global g_stateEnvVars env g_force

    array set countarr [getReferenceCountArray $var]

    set doit 0

    foreach dir [split $path ":"] {
	if [info exists countarr($dir)] {
	    incr countarr($dir) -1
	    if {$countarr($dir) <= 0} {
		set doit 1
		unset countarr($dir)
	    }
	} else {
	    set doit 1
	}

	if {$doit || $g_force } {
	    if [info exists env($var)] {
		set dirs [split $env($var) ":"]
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
		    set env($var) [join $newpath ":"]
		    set g_stateEnvVars($var) "new"
		}
	    }
	}
    }

    set sharevar "${var}_modshare"
    if {[array size countarr] > 0} {
	set env($sharevar) [join [array get countarr] ":"]
	set g_stateEnvVars($sharevar) "new"
    } else {
        unset-env $sharevar
	set g_stateEnvVars($sharevar) "del"
    }
    return {}
}

proc add-path {var path pos} {
    global env g_stateEnvVars

    set sharevar "${var}_modshare"
    array set countarr [getReferenceCountArray $var]


    if {$pos == "prepend"} {
	set pathelems [reverseList [split $path ":"]]
    } else {
	set pathelems [split $path ":"]
    }
    foreach dir $pathelems {
	if [info exists countarr($dir)] {
	    #	    already see $dir in $var"
	    incr countarr($dir)
	} else {
	    if [info exists env($var)] {
		if {$pos == "prepend"} {
		    set env($var) "$dir:$env($var)"
		} elseif {$pos == "append"} {
		    set env($var) "$env($var):$dir"
		} else {
		    error "add-path doesn't support $pos"
		}
	    } else {
		set env($var) "$dir"
	    }
	    set countarr($dir) 1
	}
    }

    set env($sharevar) [join [array get countarr] ":"]
    set g_stateEnvVars($var) "new"
    set g_stateEnvVars($sharevar) "new"
    return {}
}

proc prepend-path {var path} {
    set mode [currentMode]

    if {$mode == "load"} {
	add-path $var $path "prepend"
    } elseif {$mode == "unload"} {
	unload-path $var $path
    } elseif {$mode == "display"} {
	report "prepend-path\t$var\t$path"
    }
    return {}
}


proc append-path {var path} {
    set mode [currentMode]

    if {$mode == "load"} {
	add-path $var $path "append"
    } elseif {$mode == "unload"} {
	unload-path $var $path
    } elseif {$mode == "display"} {
	report "append-path\t$var\t$path"
    }
    return {}
}

proc remove-path {var path} {
    set mode [currentMode]

    if {$mode == "load"} {
        unload-path $var $path
    } elseif {$mode == "display"} {
        report "remove-path\t$var\t$path"
    }
    return {}
}

proc set-alias {alias what} {
    global g_Aliases g_stateAliases
    set mode [currentMode]

    if {$mode == "load"} {
	set g_Aliases($alias) $what
	set g_stateAliases($alias) "new"
    } elseif {$mode == "unload"} {
	set g_Aliases($alias) $what
	set g_stateAliases($alias) "del"
    } elseif {$mode == "display"} {
	report "set-alias\t$alias\t$what"
    }
    return {}
}


proc unset-alias {alias} {
    global g_Aliases g_stateAliases
    set mode [currentMode]

    if {$mode == "load"} {
	set g_Aliases($alias) {}
	set g_stateAliases($alias) "del"
    } elseif {$mode == "display"} {
	report "unset-alias\t$alias\t$what"
    }
    return {}
}

proc is-loaded {args} {
    global env

    if {[llength $args] > 0} {
        if [info exists env(LOADEDMODULES)] {
            foreach mod [split $env(LOADEDMODULES) ":"] {
                set mod "$mod/"
                foreach arg $args {
                    set arg "$arg/"
                    if {[string first $arg $mod] == 0} {
                        return 1
                    }
                }
            }
	}
    }
    return 0
}


proc conflict {args} {
    global g_reloadMode ModulesCurrentModulefile
    set mode [currentMode]
    set currentModule [currentModuleName]

    if { $g_reloadMode == 1} {
	return {}
    }

    if {$mode == "load"} {
        foreach mod $args {
            if [is-loaded $mod] {
                set modfile [getPathToModule $mod]
                set modfile [lindex $modfile 0]
                if [string compare $modfile $ModulesCurrentModulefile] {
                    set errMsg "WARNING: $currentModule cannot be loaded due to a conflict."
                    set errMsg "$errMsg\nHINT: Might try \"module unload $mod\" first."
                    error $errMsg
                }
            }
        }
    } elseif {$mode == "display"} {
	report "conflict\t$args"
    }
    return {}
}

proc prereq {args} {
    global g_reloadMode
    set mode [currentMode]
    set currentModule [currentModuleName]

    if { $g_reloadMode == 1} {
	return {}
    }

    if {$mode == "load"} {
        if {![is-loaded $args]} {
            set errMsg "WARNING: $currentModule cannot be loaded due to missing prereq."
            set errMsg "$errMsg\nHINT: Load one of the following modules first: $args"
            error $errMsg
        }
    } elseif {$mode == "display"} {
	report "prereq\t$args"
    }
    return {}
}

proc x-resource {resource {value {}}} {
    global g_newXResources g_delXResources
    set mode [currentMode]

    if {$mode == "load"} {
	set g_newXResources($resource) $value
    } elseif {$mode =="unload"} {
	set g_delXResources($resource) 1
    } elseif {$mode == "display"} {
	report "x-resource\t$resource\t$value"
    }
    return {}
}

proc uname {what} {
    global unameCache tcl_platform
    if { ! [info exists unameCache($what)] } {
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
                set result $tcl_platform(osVersion)
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
#set g_mode {}

proc currentMode {} {
#    upvar 2 g_mode mode
#    return $mode

    global g_modeStack
#    report "g_modeStack/cu = $g_modeStack"
    set mode [lindex $g_modeStack end]
#    report "mode = $mode"
    return $mode
}

proc pushMode {mode} {
#    upvar 1 g_mode mode2
#    set mode2 $mode

    global g_modeStack
    lappend g_modeStack $mode
#    report "g_modeStack/push = $g_modeStack"
}

proc popMode {} {

    global g_modeStack
    set len [llength $g_modeStack]
    set len [expr $len - 2]
    set g_modeStack [lrange $g_modeStack 0 $len]
#    report "g_modeStack/pop = $g_modeStack"
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
    set len [expr $len - 2]
    set g_moduleNameStack [lrange $g_moduleNameStack 0 $len]
}


proc getPathToModule {mod} {
    global env g_loadedModulesGeneric

    if {$mod == "null"} {
	return ""
    }

    if [string match {/*} $mod] {
        if [file exists $mod] {
            if [file readable $mod] {
                if [file isfile $mod] {
                    return [list $mod $mod]
                }
            }
        }
    } elseif [info exists env(MODULEPATH)] {
	foreach dir [split $env(MODULEPATH) ":"] {
	    set path "$dir/$mod"
	    while [file exists $path] {
		if [file readable $path] {

		    if [file isdirectory $path] {
			if [info exists g_loadedModulesGeneric($mod)] {
			    set ModulesVersion $g_loadedModulesGeneric($mod)
			} elseif [file exists "$path/.version"] {
			    set ModulesVersion [execute-version "$path/.version"]
	                    if { $ModulesVersion == "" || 
                                 ![file exists "$path/$ModulesVersion"]} {
				# Some may not want this warning message
				# comment it out if you do not want it.
           	                reportWarning "WARNING from getPathToModule: Execution of $path/.version did not set the ModulesVersion variable as expected."
                                # This is the fallback
                                set ModulesVersion [file dirname [lindex [listModules $path "" 0] end]]
				if {$ModulesVersion == ""} {break}
                            }
			} else {
			    set ModulesVersion [file dirname [lindex [listModules $path "" 0] end]]
			}
                        if { $ModulesVersion == "." } {
                           # No subdirectories left - take the tail file
                           set ModulesVersion [file tail [lindex [listModules $dir $mod] end]]
                        }
                        if { $ModulesVersion == "" } {
                            reportWarning "WARNING: Module $mod not found on \$MODULEPATH.\nHINT: Use 'module use ...' to add to search path."
                            return ""
                        }

			set path "$path/$ModulesVersion"
			set mod "$mod/$ModulesVersion"
		    }

		    if [file isfile $path] {
			return [list $path $mod]
		    }
		}
	    }
	}
        reportWarning "WARNING: Module $mod not found on \$MODULEPATH.\nHINT: Use 'module use ...' to add to search path."
	return ""
    } else {
	error "\$MODULEPATH not defined"
    }
}

proc runModulerc {} {
# Uses listModules to find and execute any .modulerc or .version file found in the modules directories
    global env g_debug

    catch {
        foreach dir [split $env(MODULEPATH) ":"] {
            if [file isdirectory $dir] {
		if { $g_debug } { puts stderr "DEBUG runModuleRC: running listModules $dir" }
                listModules $dir ""
            }
        }
    } errMsg
    if {$errMsg != ""} {
	reportWarning "ERROR: runModulerc failed. $errMsg"
    }
}

proc saveSettings {} {
    foreach var {env g_Aliases g_stateEnvVars g_stateAliases
	g_newXResource g_delXResource} {
	eval "global g_SAVE_$var $var"
	eval "array set g_SAVE_$var \[array get $var\]"
    }
}

proc restoreSettings {} {
    foreach var {env g_Aliases g_stateEnvVars g_stateAliases
	g_newXResource g_delXResource} {
	eval "global g_SAVE_$var $var"
	eval "array set $var \[array get g_SAVE_$var\]"
    }
}

proc renderSettings {} {
    global env g_Aliases g_shellType g_shell
    global g_stateEnvVars g_stateAliases
    global g_newXResources g_delXResources
    global g_pathList error_count

    set iattempt 0
    set f ""
    set tmpfile ""
    while {$iattempt < 100 && $f == ""} {
	set tmpfile [format "/tmp/modulescript_%d_%d" [pid] $iattempt]
	catch { set f [open  $tmpfile "w"] }
	incr iattempt
    }

    if {$f == ""} {
	error "Could not open a temporary file in $tmpfile !"
    } else {

# required to work on cygwin, shouldn't hurt real linux
	fconfigure $f -translation lf

# preliminaries

	# Do this first so if there is a problem while running the script
	# we won't leave a "turd".
	switch -- $g_shellType {
	    csh {
		puts $f "/bin/rm -f $tmpfile"
	    }
	    sh {
		puts $f "/bin/rm -f $tmpfile"
	    }
	    perl {
		puts $f "unlink(\"$tmpfile\");"
	    }
	    python {
		puts $f "os.unlink('$tmpfile')"
	    }
	}

	switch -- $g_shellType {
	    python {
		puts $f "import os";
	    }
	}

# new environment variables
	foreach var [array names g_stateEnvVars] {
	    if {$g_stateEnvVars($var) == "new"} {
		switch -- $g_shellType {
		    csh {
			set val [doubleQuoteEscaped $env($var)]
# csh barfs on long env vars
			if {$g_shell == "csh" && [string length $val] > 1000} {
			    if { $var == "PATH"} {
				reportWarning "WARNING: module: PATH exceeds 1000 characters, truncating to 900 and appending /usr/bin:/bin ..."
				set val [string range $val 0 900]:/usr/bin:/bin
			    } else {
				reportWarning "WARNING: module: $var exceeds 1000 characters, truncating..."
				set val [string range $val 0 999]
			    }
			}
			puts $f "setenv $var \"$val\""
		    }
		    sh {
			set val [doubleQuoteEscaped $env($var)]
			puts $f "$var=\"$val\"; export $var"
		    }
		    perl {
			set val [doubleQuoteEscaped $env($var)]
			set val [atSymbolEscaped $env($var)]
			puts $f "\$ENV{$var}=\"$val\";"
		    }
		    python {
			set val [singleQuoteEscaped $env($var)]
			puts $f "os.environ\['$var'\] = '$val'"
		    }
		}

	    }
	}

# new aliases
	if {$g_shell != "sh"} {
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
			    puts $f "alias $var '$val'"
			}
			sh {
			    set val $g_Aliases($var)
			    puts $f "function $var () { $val;}"
			}
		    }
		}
	    }
	}

# obsolete (deleted) env vars
	foreach var [array names g_stateEnvVars] {
	    if {$g_stateEnvVars($var) == "del"} {
		switch -- $g_shellType {
		    csh {
			puts $f "unsetenv $var"
		    }
		    sh {
			puts $f "unset $var"
		    }
		    perl {
			puts $f "delete \$ENV{$var};"
		    }
		    python {
			puts $f "os.environ\['$var'\] = ''"
			puts $f "del os.environ\['$var'\]"
		    }
		}
	    }
	}

# obsolete aliases
	if {$g_shell != "sh"} {
	    foreach var [array names g_stateAliases] {
		if {$g_stateAliases($var) == "def"} {
		    switch -- $g_shellType {
			csh {
			    puts $f "unalias $var"
			}
			sh {
                            puts $f "unset -f $var"
			}
		    }
		}
	    }
	}

# new x resources
	if {[array size g_newXResources] > 0} {
	    set xrdb [findExecutable xrdb]
	    foreach var [array names g_newXResources] {
		set val $g_newXResources($var)
		if { $val == "" } {
		    switch -regexp -- $g_shellType {
			{^(csh|sh)$} {
			    if [ file exists $var] {
				puts $f "$xrdb -merge $var"
			    } else {
				puts $f "$xrdb -merge <<EOF"
				puts $f "$var"
				puts $f "EOF"
			    }
			}
			perl {
			    if [ file isfile $var] {
				puts $f "system(\"$xrdb -merge $var\");"
			    } else {
				puts $f "open(XRDB,\"|$xrdb -merge\");"
				set var [doubleQuoteEscaped $var]
				puts $f "print XRDB \"$var\\n\";"
				puts $f "close XRDB;"
			    }
			}
			python {
			    if [ file isfile $var] {
				puts $f "os.popen('$xrdb -merge $var');"
			    } else {
				set var [singleQuoteEscaped $var]
				puts $f "os.popen('$xrdb -merge').write('$var')"
			    }
			}
		    }
		} else {
		    switch -regexp -- $g_shellType {
			{^(csh|sh)$} {
			    puts $f "$xrdb -merge <<EOF"
			    puts $f "$var: $val"
			    puts $f "EOF"
			}
			perl {
			    puts $f "open(XRDB,\"|$xrdb -merge\");"
			    set var [doubleQuoteEscaped $var]
			    set val [doubleQuoteEscaped $val]
			    puts $f "print XRDB \"$var: $val\\n\";"
			    puts $f "close XRDB;"
			}
			python {
			    set var [singleQuoteEscaped $var]
			    set val [singleQuoteEscaped $val]
			    puts $f "os.popen('$xrdb -merge').write('$var: $val')"
			}
		    }
		}
	    }
	}
	if {[array size g_delXResources] > 0} {
	    set xrdb [findExecutable xrdb]
	    foreach var [array names g_delXResources] {
		if {$val == ""} {
		    # do nothing
		} else {
		    puts $f "xrdb -remove <<EOF"
		    puts $f "$var:"
		    puts $f "EOF"
		}
	    }
	}

# module path{s,} output
	if [info exists g_pathList] {
	    foreach var $g_pathList {
		switch -- $g_shellType {
		    csh {
			puts $f "echo '$var'"
		    }
		    sh {
			puts $f "echo '$var'"
		    }
		    perl {
			puts $f "print '$var'.\"\\n\";"
		    }
		    python {
			# I'm not a python programmer
		    }
		}
	    }
	}

###
	set nop 0
	if {  $error_count == 0  && ! [tell $f] } {
	    set nop 1
	}

	if {$error_count > 0} {
	    reportWarning "ERROR: $error_count error(s) detected."
	    switch -- $g_shellType {
		csh {
		    puts $f "/bin/false"
		}
		sh {
		    puts $f "/bin/false"
		}
		perl {
		    puts $f "die \"modulefile.tcl: $error_count error(s) detected!\\n\""
		}
		python {
		    # I am not a python programmer...
		}
	    }
	    set nop 0
	}
	close $f

	fconfigure stdout -translation lf

	if {$nop} {
#	    nothing written!
	    file delete $tmpfile
	    switch -- $g_shellType {
		csh {
		    puts "/bin/true"
		}
		sh {
		    puts "/bin/true"
		}
		perl {
		    puts "1;"
		}
		python {
		    # this is not correct
		    puts ""
		}
	    }
	} else {
	    switch -- $g_shellType {
		csh {
		    puts "source $tmpfile"
		}
		sh {
		    puts ". $tmpfile"
		}
		perl {
		    puts "require \"$tmpfile\";"
		}
		python {
		    # this is not correct
		    puts "exec '$tmpfile'"
		}
	    }
	}
    }
}

proc cacheCurrentModules {} {
    global g_loadedModules g_loadedModulesGeneric env

# mark specific as well as generic modules as loaded
    if [info exists env(LOADEDMODULES)] {
	foreach mod [split $env(LOADEDMODULES) ":"] {
	    set g_loadedModules($mod) 1
	    set g_loadedModulesGeneric([file dirname $mod]) [file tail $mod]
	}
    }
}

proc resolveModuleVersionOrAlias {names} {
    # This proc resolves module aliases or version aliases to the real module name and version
    global g_moduleVersion
    global g_moduleDefault
    global g_moduleAlias
    global g_debug

    if { $g_debug } { puts stderr "DEBUG resolveModuleVersionOrAlias: Resolving $names" }
    set ret_list {}

    foreach name $names {
	if { [ info exists g_moduleAlias($name) ] } {
	    # if the alias is another alias, we need to resolve it
	    if { $g_debug } { puts stderr "DEBUG resolveModuleVersionOrAlias: $name is an alias"}
	    set ret_list [linsert $ret_list end [ resolveModuleVersionOrAlias $g_moduleAlias($name) ]]
	} elseif { [info exists g_moduleVersion($name) ] } {
	    # if the pseudo version is an alias, we need to resolve it
	    if { $g_debug } { puts stderr "DEBUG resolveModuleVersionOrAlias: $name is a version alias"}
	    set ret_list [linsert $ret_list end [resolveModuleVersionOrAlias $g_moduleVersion($name)]]
	} elseif { [ info exists g_moduleDefault($name) ] } {
	    # if the default is an alias, we need to resolve it
	    if { $g_debug } {  puts stderr "DEBUG resolveModuleVersionOrAlias: found a default for $name"}
	    set ret_list [linsert $ret_list end [resolveModuleVersionOrAlias "$name/$g_moduleDefault($name)"]]
	} else {
	    if { $g_debug } { puts stderr "DEBUG resolveModuleVersionOrAlias: $name is nothing special"}
	    set ret_list [linsert $ret_list end $name]
	}
    }
    if { $g_debug } { puts stderr "DEBUG resolveModuleVersionOrAlias: Resolved to $ret_list"}
    return $ret_list
}

proc spaceEscaped {text} {
    regsub -all " " $text "\\ " text
    return $text
}

proc multiEscaped {text} {
    regsub -all {["'; ]} $text {\\\0} text
#"
    return $text
}

proc doubleQuoteEscaped {text} {
    regsub -all "\"" $text "\\\"" text
    return $text
}

proc atSymbolEscaped {text} {
    regsub -all "@" $text "\\@" text
    return $text
}

proc singleQuoteEscaped {text} {
    regsub -all "\'" $text "\\\'" text
    return $text
}

proc findExecutable {cmd} {
    foreach dir {/usr/X11R6/bin /usr/openwin/bin /usr/bin/X11} {
	if [file executable "$dir/$cmd"] {
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

proc removeFromList {list1 item} {		    
    eval set list2 \[list $list1 \]
    set list1 {}
    foreach x $list2 {
	if {$x != $item && $x != "null"} {
	    lappend list1 $x
	}
    }
    return [join $list1 " "]
}

proc replaceFromList {list1 olditem newitem} {		    
    eval set list2 \[list $list1 \]
    set list1 {}
    foreach x $list2 {
	if {$x == $olditem} {
	    lappend list1 $newitem
	} else {
	    lappend list1 $x
	}
    }
    return [join $list1 " "]
}

proc listModules {dir mod {full_path 1} {how {-dictionary}}} {
    global ignoreDir

# On Cygwin, glob may change the $dir path if there are symlinks involved
# So it is safest to reglob the $dir.
# example:
# [glob /home/stuff] -> "//homeserver/users0/stuff"

    set flag_default_dir 1	;# Report default directories
    set flag_default_mf  1	;# Report default modulefiles
    set default_tag "(default)"	;# Text used to report defaults
    set dir [glob $dir]
    set full_list [glob -nocomplain "$dir/$mod"]
    set clean_list {}
    set ModulesVersion {}
    for {set i 0} {$i < [llength $full_list]} {incr i 1} {
        set element [lindex $full_list $i]
        set tail [file tail $element]
        set direlem [file dirname $element]

        if [file isdirectory $element] {
            if {![info exists ignoreDir($tail)]} {
                foreach f [glob -nocomplain "$element/.modulerc" "$element/.version" "$element/*"] {
                    lappend full_list $f
                }
            }
        } else {
            switch -glob -- $tail {
		{.modulerc} {
		    execute-modulerc $element
		}
                {.version} {
		    if { $flag_default_dir || $flag_default_mf } {
			set ModulesVersion [execute-version "$element"]
			if {$flag_default_dir && $ModulesVersion != "" && [file isdirectory $direlem/$ModulesVersion]} {
			    set sstart [expr [string length $dir] + 1]
			    # Directories below MODULEPATH that are defaulted in the path to the modulefile need to be reported.
			    lappend clean_list [string range $direlem/${ModulesVersion}$default_tag $sstart end]
			}
		    }
		}
                {.*} -
                {*~} -
                {*,v} -
                {#*#} {}
                default {
                    if {![catch {open $element r} fileId]} {
                        gets $fileId first_line
                        close $fileId
                        if {[string first {#%Module} $first_line] == 0} {
                            if {$full_path} {
				if {$flag_default_mf && $tail == $ModulesVersion} {
				    lappend clean_list "${element}$default_tag"
				} else {
				    lappend clean_list $element
				}
                            } else {
                                set sstart [expr [string length $dir] + 1]
				if {$flag_default_mf && $tail == $ModulesVersion} {
				    lappend clean_list [string range "${element}$default_tag" $sstart end]
				} else {
				    lappend clean_list [string range $element $sstart end]
				}
                            }
                        }
                    }
                }
            }
        }
    }
    if {$how != {}} {
        set clean_list [lsort -dictionary $clean_list]
    }
    return $clean_list
}


proc showModulePath {} {
    global env
    if [info exists env(MODULEPATH)] {
	report "Search path for module files (in search order):"
	foreach path [split $env(MODULEPATH) ":"] {
	    report "  $path";
	}
    } else {
	reportWarning "WARNING: no directories on module search path"
    }

}

#
# Info, Warnings and Error message handling.
#
proc reportWarning {message} {
    puts stderr $message
}

proc reportInternalBug {message} {
    puts stderr "ERROR: $message\nPlease contact your local friendly maintainer of the module command."
#
# future: send email to maintainer
#
}

proc report {message {nonewline ""}} {
    if {$nonewline != ""} {
	puts -nonewline stderr "$message"
    } else {
	puts stderr "$message"
    }
}

########################################################################
# command line commands

proc cmdModuleList {} {
    global env

    report "Currently Loaded Modulefiles:"
    if [info exists env(LOADEDMODULES)] {
	set max 0
	foreach mod [split  $env(LOADEDMODULES) ":"] {
            set len [string length $mod]
	    if {$len > $max} {
		set max $len
	    }
            if {$len > 0} {
                # skip zero length module names
                lappend list $mod
            }
	}
# save room for numbers and spacing: 2 digits + ) + space + space
        set col_width [expr $max +4]
        if [info exist env(COLUMNS)] {
           set cols [expr int($env(COLUMNS)/$col_width)]
        } else {
           set cols [expr int(80/$col_width)]
        }
        # safety check to prevent divide by zero error below
        if {$cols <= 0} {set cols 1}
 
        set item_cnt [llength $list]
        set rows [expr int($item_cnt / $cols)]
        set lastrow_item_cnt [expr int($item_cnt % $cols)]
        if {$lastrow_item_cnt > 0} {incr rows}
        #report "list = $list"
        #report "rows/cols = $rows/$cols,   max = $max"
        #report "item_cnt = $item_cnt,  lastrow_item_cnt = $lastrow_item_cnt"
        for {set row 0} { $row < $rows } {incr row} {
            for {set col 0} {$col < $cols} {incr col} {
                set index [expr $col * $rows + $row]
		set mod [lindex $list $index]
		if {$mod != ""} {
		    set n [expr $index +1]
		    set mod [format "%2d) %-${max}s" $n $mod]
		    report $mod -nonewline
		}
	    }
	    report ""
	}
    }
}

proc cmdModuleDisplay {mod} {
    global env tcl_version ModulesCurrentModulefile

    set modfile [getPathToModule $mod]
    if {$modfile != ""} {
	pushModuleName [lindex $modfile 1]
	set modfile	  [lindex $modfile 0]
	report "-------------------------------------------------------------------"
	report "$modfile:\n"
	pushMode "display"
        execute-modulefile $modfile
	popMode
	popModuleName
	report "-------------------------------------------------------------------"
    }
}

proc cmdModulePaths {mod} {
    global env g_pathList

    catch {
        foreach dir [split $env(MODULEPATH) ":"] {
            if [file isdirectory $dir] {
                foreach mod2 [listModules $dir $mod] {
                    lappend g_pathList $mod2
                }
            }
        }
	set junk ""
    } errMsg
    if {$errMsg != ""} {
	reportWarning "ERROR: module paths $mod failed. $errMsg"
    }
}

proc cmdModulePath {mod} {
    global env g_pathList ModulesCurrentModulefile

    set modfile [getPathToModule $mod]
    if {$modfile != ""} {
	set modfile	  [lindex $modfile 0]
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

proc cmdModuleSearch {{mod {}} {search {}} } {
    global env tcl_version ModulesCurrentModulefile
    global g_whatis

    if {$mod == ""} {
	set mod  "*"
    }
    foreach dir [split $env(MODULEPATH) ":"] {
	if [file isdirectory $dir] {
	    report "---- $dir ---- "
            set modlist [listModules $dir $mod 0]
	    foreach mod2 $modlist {
		set g_whatis ""
		set modfile [getPathToModule $mod2]
		if {$modfile != ""} {
		    pushMode "whatis"
		    pushModuleName [lindex $modfile 1]
		    set modfile    [lindex $modfile 0]
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
    if {$new == ""} {
	set new $old
	if {[file dirname $old] != "."} {
	    set old [file dirname $old]
	}
    }

    cmdModuleUnload $old
    cmdModuleLoad $new
}

proc cmdModuleSource {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force

    foreach file $args {
	if [file exists $file] {
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
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force
    global ModulesCurrentModulefile
    global g_debug

    if { $g_debug } { puts stderr "DEBUG cmdModuleLoad: loading $args" }
    
    foreach mod $args {
	set modfile [getPathToModule $mod]
        if {$modfile != ""} {
	    set currentModule [lindex $modfile 1]
	    set modfile       [lindex $modfile 0]
	    set ModulesCurrentModulefile $modfile
	    
	    if { $g_force || ! [ info exists g_loadedModules($currentModule)]} {
		pushMode "load"
		pushModuleName $currentModule
		
		saveSettings
                if [execute-modulefile $modfile] {
		    restoreSettings
		} else {
                    append-path LOADEDMODULES $currentModule
                    set g_loadedModules($currentModule) 1
                    set genericModName [file dirname $mod]
		
                    if { ![ info exists g_loadedModulesGeneric($genericModName) ] } {
                        set g_loadedModulesGeneric($genericModName) [file tail $currentModule]
                    }
                }

		popMode
		popModuleName
	    }
	}
    }
}

proc cmdModuleUnload {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric
    global ModulesCurrentModulefile

    foreach mod $args {
	catch {
	    set modfile [getPathToModule $mod]
	    if {$modfile != ""} {
		set currentModule [lindex $modfile 1]
		set modfile       [lindex $modfile 0]
                set ModulesCurrentModulefile $modfile
		
		if [ info exists g_loadedModules($currentModule)] {
		    pushMode "unload"
		    pushModuleName $currentModule
		    saveSettings
                    if [execute-modulefile $modfile] {
			restoreSettings
                    }
		    popMode
		    popModuleName
		    
		    unload-path LOADEDMODULES $currentModule
		    unset g_loadedModules($currentModule)
		    if [info exists g_loadedModulesGeneric([file dirname $currentModule])] {
			unset g_loadedModulesGeneric([file dirname $currentModule])
		    }
		}
	    } else {
		if [info exists g_loadedModulesGeneric($mod) ] {
		    set mod "$mod/$g_loadedModulesGeneric($mod)"
		}
		unload-path LOADEDMODULES $mod
		if [info exists g_loadedModules($mod) ] {
		    unset g_loadedModules($mod)
		}
		if [info exists g_loadedModulesGeneric([file dirname $mod])] {
		    unset g_loadedModulesGeneric([file dirname $mod])
		}
	    }
	    set junk ""
	} errMsg
	if {$errMsg != ""} {
	    reportWarning "ERROR: module: module unload $mod failed.\n$errMsg"
	}
    }
}


proc cmdModulePurge {} {
    global env

    if [info exists env(LOADEDMODULES)] {
	set list [split $env(LOADEDMODULES) ":"]
	eval cmdModuleUnload $list
    }
}


proc cmdModuleReload {} {
    global env g_reloadMode

    if [info exists env(LOADEDMODULES)] {
	set list [split $env(LOADEDMODULES) ":"]
	set rlist [reverseList $list]
	set g_reloadMode 1
	# we set reload mode for two reasons
	# 1) we DONT want to follow hierarchical modules
	# 2) we DONT want to check for conflicts
	# This is because all of these checks have been done
	# already, assuming the user used the normal module interface.
	eval cmdModuleUnload $rlist
	eval cmdModuleLoad $list
	set g_reloadMode 0
    }
}


proc cmdModuleAvail { {mod {*}}} {
    global env ignoreDir

    foreach dir [split $env(MODULEPATH) ":"] {
	if [file isdirectory $dir] {
	    report "---- $dir ---- "
            set list [listModules $dir $mod 0]
	    set max 0
	    foreach mod2 $list {
		if {[string length $mod2] > $max} {
		    set max [string length $mod2]
		}
	    }
	    incr max 1
            if [info exist env(COLUMNS)] {
                set cols [expr int($env(COLUMNS)/($max))]
            } else {
                set cols [expr int(80/($max))]
            }
            # safety check to prevent divide by zero error below
            if {$cols <= 0} {set cols 1}

           # There is no '{}' at the begining of this 'list' as there is in cmd 
	   #               ModuleList - ?
           set item_cnt [expr [llength $list] - 0]
           set rows [expr int($item_cnt / $cols)]
           set lastrow_item_cnt [expr int($item_cnt % $cols)]
           if {$lastrow_item_cnt > 0} {incr rows}
           #report "list = $list"
           #report "rows/cols = $rows/$cols,   max = $max"
           #report "item_cnt = $item_cnt,  lastrow_item_cnt = $lastrow_item_cnt"
           for {set row 0} { $row < $rows} {incr row} {
		for {set col 0} {$col < $cols } { incr col} {
		    set index [expr $col * $rows + $row]
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


proc cmdModuleUse {args} {

    if {$args == ""} {
	showModulePath
    } else {
	set stuff_path "prepend-path"
	foreach path [split $args " "] {
	    # -a -append --append (and -p -prepend --prepend) would be nice...
	    if { $path == "" } {
		# Skip "holes"
	    } elseif { $path == "--append" } {
		set stuff_path "append-path"
	    } elseif {$path == "--prepend"} {
		set stuff_path "prepend-path"
	    } elseif [file isdirectory $path] {
		pushMode "load"
		eval $stuff_path MODULEPATH $path
		popMode
	    } else {
		reportWarning "WARNING: Directory \"$path\" does not exist, not added to module search path."
	    }
	}
    }
}


proc cmdModuleUnuse {args} {

    if {$args == ""} {
	showModulePath
    } else {
	global env
	foreach path $args {
	    if [info exists env(MODULEPATH)] {
		set oldMODULEPATH $env(MODULEPATH)
		pushMode "unload"
		catch {
		    unload-path MODULEPATH $path
		    set junk ""
		}
		popMode
		if { [info exists env(MODULEPATH) ] &&
		     $oldMODULEPATH == $env(MODULEPATH)} {
		    reportWarning "WARNING: Did not unuse $path"
		}
	    }
	}
    }
}


proc cmdModuleDebug {} {
    global env

    foreach var [array names env] {
	set sharevar "${var}_modshare"
	array set countarr [getReferenceCountArray $var]

	foreach path [array names countarr] {
	    report "$var\t$path\t$countarr($path)"
	}
	unset countarr
    }
    foreach dir  [split $env(PATH) ":"] {
	foreach file [glob -nocomplain -- "$dir/*"] {
	    if [file executable $file] {
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


proc cmdModuleInit {args} {

    global g_shell env
    set moduleinit_cmd [lindex $args 0]

    # Define startup files for each shell
    set files(csh) [ list ".modules" ".cshrc" ".cshrc_variables" ".login" ]
    set files(tcsh) [ list ".modules" ".tcshrc" ".cshrc" ".cshrc_variables" ".login" ]
    set files(sh) [ list ".modules" ".bash_profile" ".bash_login" ".profile" ".bashrc"]
    set files(ksh) $files(sh)
    set files(zsh) [ list ".modules" ".zshrc" ".zshenv" ".zlogin" ]

    # Process startup files for this shell
    set current_files $files($g_shell)
    foreach filename $current_files {
	set filepath $env(HOME)
	append filepath "/" $filename
	# create a new file to put the changes in
	set newfilepath "$filepath-NEW"

	if { [ file readable $filepath ] } {
	    set fid [ open $filepath r ]

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
	    
	    # everything but list requires at least one argument
	    if { [expr [llength $args] -1 ] !=  $nargs($moduleinit_cmd) } {
		error "'module init$moduleinit_cmd' requires exactly $nargs($moduleinit_cmd) arg(s)."
#		cmdModuleHelp
		exit -1
	    }
	
	    # Only open the new file if we are not doing "initlist"
	    if {[ string compare $moduleinit_cmd "list"] != 0} {
		
		set newfid [ open $newfilepath w ]

	    } 
		

	    while { [gets $fid curline] >= 0 } { 
		# Find module load/add command in startup file 
		if { [regexp -line {^([ \t]*module[ \t]+)((?:load|add)\s+)([^\#]+\S)(\s+\#.*)?} $curline match cmd subcmd modules comments ] } {

# remove existing references to the named module from the list


		    # Change the module command line to reflect the given command
		    switch $moduleinit_cmd {
			list {
			    puts stderr "$g_shell initialization file $filepath loads modules: $modules"
			}
			add {
			    set newmodule [lindex $args 1]
			    set modules [removeFromList $modules $newmodule]
			    append modules " $newmodule"
			    puts $newfid "$cmd$subcmd$modules$comments"
			}
			prepend {
			    set newmodule [lindex $args 1]
			    set modules [removeFromList $modules $newmodule]
			    set modules "$newmodule $modules"
			    puts $newfid "$cmd$subcmd$modules$comments"
			}
			rm {
			    set oldmodule [lindex $args 1]
			    set modules [removeFromList $modules $oldmodule]
			    if { ![ regexp -line {\S} $modules] } {
				set modules "null"
			    }
			    puts $newfid "$cmd$subcmd$modules$comments"
			}
			switch {
			    set oldmodule [lindex $args 1]
			    set newmodule [lindex $args 2]
			    set modules [replaceFromList $modules $oldmodule $newmodule]
			    puts $newfid "$cmd$subcmd$modules$comments"
			}
			clear {
			    set modules "null"
			    puts $newfid "$cmd$subcmd$modules$comments"
			}
			default {
			    puts stderr "Command init$moduleinit_cmd not recognized"
			}

		    }  
		} else  {  
		    # copy the line from the old file to the new
		    if { [info exists newfid] } {
			puts $newfid $curline
		    }
		}
            }  
            close $fid
            if { [info exists newfid] } {
		close $newfid
		if { [catch "file copy -force $filepath $filepath-OLD"] != 0 } {
		    puts stderr "Failed to back up original $filepath...exiting"
		    exit -1
		}
		if { [catch "file copy -force $newfilepath $filepath"] != 0} {
		    puts stderr "Failed to write $filepath...exiting"
		    exit -1
		}
	    }
        }
    }
}


proc cmdModuleHelp {args} {
   global done

   set done 0
   foreach arg $args {
      if {$arg != ""} {
         set modfile [getPathToModule $arg]

          if {$modfile != ""} {
             pushModuleName [lindex $modfile 1]
             set modfile       [lindex $modfile 0]
             report "-------------------------------------------------------------------"
             report "Module Specific Help for $modfile:\n"
	     set mode "Help"
             execute-modulefile-help $modfile
             popMode
             popModuleName
             report "-------------------------------------------------------------------"
          }
	  set done 1
       }
    }
    if {$done == 0 } {
            report {
                ModulesTcl 0.101/$Revision: 1.37 $:
                Available Commands and Usage:

list         |  add|load            modulefile [modulefile ...]
purge        |  rm|unload           modulefile [modulefile ...]
reload       |  switch|swap        [oldmodulefile] newmodulefile
             |  display|show        modulefile [modulefile ...]
             |  avail              [modulefile [modulefile ...]]
             |  whatis             [modulefile [modulefile ...]]
             |  help               [modulefile [modulefile ...]]
             |  path                modulefile
             |  paths               modulefile
             |  use                 dir [dir ...]
             |  unuse               dir [dir ...]
             |  source              scriptfile
             |  apropos|keyword     string
             |
initlist     |  initadd             modulefile
initclear    |  initprepend         modulefile
	     |  initrm              modulefile
            }
    }
}


########################################################################
# main program

set g_shell [lindex $argv 0]
set command [lindex $argv 1]
set argv [lreplace $argv 0 1]
set g_debug 0

# Parse options
set optIndex [lsearch -regexp $argv {^-}]
while { $optIndex >= 0 } {
    if { [ regexp {^-debug$} [lindex $argv $optIndex] ] } {
	set g_debug 1
	puts stderr "DEBUG : debug enabled"
    }
    set argv [lreplace $argv $optIndex $optIndex]
    set optIndex [lsearch -regexp $argv {^-}]
}

switch -regexp -- $g_shell {
    ^(sh|bash|ksh|zsh)$ {
	set g_shellType sh
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
    . {
	error "bad shell $g_shell"
    }
}

cacheCurrentModules

# Resolve aliases and run .modulerc files only for commands that need it.  Finding modulerc files
# can be slow with lots of modules to search
if { [regexp $command {^(di|show|add|load|sw|rm|unload|wh|aprop|init(add|load|rm|unload))} ] } {
    # Find and execute any .modulerc file found in the module directories defined in env(MODULESPATH)
    runModulerc
    # Resolve any aliased module names - safe to run nonmodule arguments
    if { $g_debug } { puts stderr "DEBUG: Resolving $argv" }
    set argv [resolveModuleVersionOrAlias $argv]
    if { $g_debug } { puts stderr "DEBUG: Resolved $argv" }
}

catch {
    switch -regexp -- $command {
	{^av} {
	    if {$argv != ""} {
		foreach arg $argv {
		    cmdModuleAvail $arg
		}
	    } else {
		cmdModuleAvail
	    }
	}
	{^li} {
	    cmdModuleList
	}
	{^(di|show)} {
	    foreach arg $argv {
		cmdModuleDisplay $arg
	    }
	}
	{^(add|load)} {
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
	{^(rm|unload)} {
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
	{^apropos$} {
	    eval cmdModuleApropos $argv
	}
	{^debug$} {
	    eval cmdModuleDebug
	}
	{^rel} {
	    cmdModuleReload
	    renderSettings
	}
	{^init(add|load)$} {
	    eval cmdModuleInit add $argv
	}
	{^initprepend$} {
	    eval cmdModuleInit prepend $argv
	}
	{^initswitch$} {
	    eval cmdModuleInit switch $argv
	}
	{^init(rm|unload)$} {
	    eval cmdModuleInit rm $argv
	}
	{^initlist$} {
	    eval cmdModuleInit list $argv
	}
	{^initclear$} {
	    eval cmdModuleInit clear $argv
	}
	default {
	   cmdModuleHelp $argv
	}

    }
} errMsg

if {$errMsg != ""} {
    reportWarning "ERROR: $errMsg"
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
