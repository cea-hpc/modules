########################################################################
# This is a pure TCL implementation of the module command
#
# Some Global Variables.....
set g_debug 0 ;# Set to 1 to enable debugging
set error_count 0 ;# Start with 0 errors
set g_autoInit 0
set g_force 1 ;# Path element reference counting if == 0
set CSH_LIMIT 1000 ;# Workaround for commandline limits in csh
set DEF_COLUMNS 80 ;# Default size of columns for formating
set MODULES_CURRENT_VERSION 3.1.6
set flag_default_dir 1 ;# Report default directories
set flag_default_mf 1 ;# Report default modulefiles and version alias
set g_user "advanced" ;# username were running as....
set g_trace 0 ;# not implemented yet
set g_tracepat "-.*" ;# not implemented yet
set g_def_seperator ":" ;# Default path seperator

# Change this to your support email address...
set contact "root@localhost"

# Set some directories to ignore when looking for modules.
set ignoreDir(CVS) 1
set ignoreDir(RCS) 1
set ignoreDir(SCCS) 1

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
    global env

    if {[info exists env($var)]} {
	unset env($var)
    }
}

proc execute-modulefile {modfile {help ""}} {
    global env g_stateEnvVars g_debug
    global ModulesCurrentModulefile
    set ModulesCurrentModulefile $modfile

    if {$g_debug} {
	report "DEBUG Starting $modfile"
    }
    set slave __[currentModuleName]
    if {![interp exists $slave]} {
	interp create $slave
	interp alias $slave setenv {} setenv
	interp alias $slave unsetenv {} unsetenv
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

	interp eval $slave [list global ModulesCurrentModulefile g_debug]
	interp eval $slave [list set ModulesCurrentModulefile $modfile]
	interp eval $slave [list set g_debug $g_debug]
	interp eval $slave [list set help $help]

    }
    set errorVal [interp eval $slave {
	if [catch {source $ModulesCurrentModulefile} errorMsg] {
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
	    if {$help != ""} {
		if {[info procs "ModulesHelp"] == "ModulesHelp"} {
		    ModulesHelp
		} else {
		    reportWarning "Unable to find ModulesHelp in\
		      $ModulesCurrentModulefile."
		}
	    } else {
		if {[module-info mode "display"] && [info procs\
		  "ModulesDisplay"] == "ModulesDisplay"} {ModulesDisplay}
	    }
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
    global env g_stateEnvVars g_rcfilesSourced
    global g_debug g_moduleDefault
    global ModulesCurrentModulefile

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

	    interp eval $slave [list global ModulesCurrentModulefile g_debug]
	    interp eval $slave [list set ModulesCurrentModulefile $modfile]
	    interp eval $slave [list global ModulesVersion]
	    interp eval $slave [list set ModulesVersion {}]
	    interp eval $slave [list set g_debug $g_debug]
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

	set g_moduleDefault($modparent) $ModulesVersion

	if {$g_debug} {
	    report "DEBUG execute-version: Setting g_moduleDefault($modparent)\
	      $ModulesVersion"
	}

	# Keep track of rc files that we already source so we don't run them\
	  again
	set g_rcfilesSourced($modfile) 1

	return $ModulesVersion
    }
}


########################################################################
# commands run from inside a module file
#
set ModulesCurrentModulefile {}

proc module-info {what {more {}}} {
    global g_shellType g_shell g_user g_trace g_tracepat
    global g_moduleAlias g_symbolHash g_versionHash

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
    "user" {
	    return $g_user
	}
    "trace" {
	    return $g_trace
	}
    "tracepat" {
	    return $g_tracepat
	}
    "shelltype" {
	    return $g_shellType
	}
    "alias" {
	    return $g_moduleAlias($more)
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
    set mode [currentMode]

    global g_whatis

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
	set base [file tail [file dirname $ModulesCurrentModulefile]]
	set module_name "${base}$module_name"
    }

    foreach version [lrange $args 1 end] {

	set base [file tail [file dirname $module_name]]
	set aliasversion [file tail $module_name]

	if {$base != ""} {
	    if {[string match $version "default"]} {
		# If we see more than one default for the same module, just\
		  keep the first
		if {![info exists g_moduleDefault($module_name)]} {
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
    global g_moduleAlias g_symbolHash
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
    add -
    load {
	    if {$mode == "load"} {
		eval cmdModuleLoad $args
	    }\
	    elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "display"} {
		report "module load $args"
	    }
	}
    rm -
    unload {
	    if {$mode == "load"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    }\
	    elseif {$mode == "display"} {
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
    switch -
    swap {
	    eval cmdModuleSwitch $args
	}
    display -
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
    apropos -
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
	report "setenv\t$var\t$val"
    }
    return {}
}

proc unsetenv {var {val {}}} {
    global g_stateEnvVars env
    set mode [currentMode]

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
	report "unsetenv\t$var"
    }
    return {}
}

########################################################################
# path fiddling

proc getReferenceCountArray {var seperator} {
    global env g_force

    set sharevar "${var}_modshare"
    set modshareok 1
    if {[info exists env($sharevar)]} {
	if {[info exists env($var)]} {
	    set modsharelist [split $env($sharevar) $seperator]
	    if {[expr {[llength $modsharelist] % 2}] == 0} {
		array set countarr $modsharelist

		# sanity check the modshare list
		array set fixers {}
		array set usagearr {}
		foreach dir [split $env($var) $seperator] {
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
	    #	    reportWarning "WARNING: module: $sharevar exists (\
	      $env($sharevar) ), but $var doesn't. Environment is corrupted."
	    set modshareok 0
	}
    } else {
	set modshareok 0
    }

    if {$modshareok == 0 && [info exists env($var)]} {
	array set countarr {}
	foreach dir [split $env($var) $seperator] {
	    set countarr($dir) 1
	}
    }
    return [array get countarr]
}


proc unload-path {var path {seperator {}}} {
    global g_stateEnvVars env g_force g_def_seperator

    if {$seperator == ""} {
	set seperator $g_def_seperator
    }
    array set countarr [getReferenceCountArray $var $seperator]

    # Don't worry about dealing with this variable if it is already scheduled\
      for deletion
    if {[info exists g_stateEnvVars($var)] && $g_stateEnvVars($var) == "del"} {
	return {}
    }

    set doit 0

    foreach dir [split $path $seperator] {
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
		set dirs [split $env($var) $seperator]
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
		    set env($var) [join $newpath $seperator]
		    set g_stateEnvVars($var) "new"
		}
	    }
	}
    }

    set sharevar "${var}_modshare"
    if {[array size countarr] > 0} {
	set env($sharevar) [join [array get countarr] $seperator]
	set g_stateEnvVars($sharevar) "new"
    } else {
	unset-env $sharevar
	set g_stateEnvVars($sharevar) "del"
    }
    return {}
}

proc add-path {var path pos {seperator {}}} {
    global env g_stateEnvVars g_def_seperator

    if {$seperator == ""} {
	set seperator $g_def_seperator
    }
    set sharevar "${var}_modshare"
    array set countarr [getReferenceCountArray $var $seperator]

    if {$pos == "prepend"} {
	set pathelems [reverseList [split $path $seperator]]
    } else {
	set pathelems [split $path $seperator]
    }
    foreach dir $pathelems {
	if {[info exists countarr($dir)]} {
	    #	    already see $dir in $var"
	    incr countarr($dir)
	} else {
	    if {[info exists env($var)]} {
		if {$pos == "prepend"} {
		    set env($var) "$dir:$env($var)"
		}\
		elseif {$pos == "append"} {
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


    set env($sharevar) [join [array get countarr] $seperator]
    set g_stateEnvVars($var) "new"
    set g_stateEnvVars($sharevar) "new"
    return {}
}

proc prepend-path {var path {seperator {}}} {
    global g_def_seperator

    set mode [currentMode]

    if {$seperator == ""} {
	set seperator $g_def_seperator
    }

    if {$mode == "load"} {
	add-path $var $path "prepend" $seperator
    }\
    elseif {$mode == "unload"} {
	unload-path $var $path
    }\
    elseif {$mode == "display"} {
	report "prepend-path\t$var\t$path\t$seperator"
    }
    return {}
}


proc append-path {var path {seperator {}}} {
    global g_def_seperator

    set mode [currentMode]

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {$mode == "load"} {
	add-path $var $path "append" $seperator
    }\
    elseif {$mode == "unload"} {
	unload-path $var $path
    }\
    elseif {$mode == "display"} {
	report "append-path\t$var\t$path\t$seperator"
    }
    return {}
}

proc remove-path {var path {seperator {}}} {
    global g_def_seperator

    set mode [currentMode]

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {$mode == "load"} {
	unload-path $var $path $seperator
    }\
    elseif {$mode == "display"} {
	report "remove-path\t$var\t$path\t$seperator"
    }
    return {}
}

proc set-alias {alias what} {
    global g_Aliases g_stateAliases
    set mode [currentMode]

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
    global g_Aliases g_stateAliases
    set mode [currentMode]

    if {$mode == "load"} {
	set g_Aliases($alias) {}
	set g_stateAliases($alias) "del"
    }\
    elseif {$mode == "display"} {
	report "unset-alias\t$alias\t"
    }
    return {}
}

proc is-loaded {modulelist} {
    global env

    if {[llength $modulelist] > 0} {
	if {[info exists env(LOADEDMODULES)]} {
	    foreach arg $modulelist {
		set arg "$arg/"
		set arg_found 0
		foreach mod [split $env(LOADEDMODULES) ":"] {
		    set mod "$mod/"
		    if {[string first $arg $mod] == 0} {
			set arg_found 1
		    }
		}
		if {$arg_found == 0} {
		    return 0
		}
	    }
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
    set mode [currentMode]
    set currentModule [currentModuleName]

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
	report "prereq\t$args"
    }
    return {}
}

proc x-resource {resource {value {}}} {
    global g_newXResources g_delXResources
    set mode [currentMode]

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
    global unameCache tcl_platform
    set result {}

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
		set result $tcl_platform(osVersion)
	    }
	default {
		error "uname $what not supported"
	    }
	domain {
		set result [exec /bin/domainname]
	    }
	version {
		set result [exec /bin/uname -v]
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
# Resolve aliases and default versions the module name is something like 
# "name/version" or just name (find default version)
proc getPathToModule {mod {seperator {}}} {
    global env g_loadedModulesGeneric
    global g_moduleAlias g_moduleVersion
    global g_debug g_def_seperator
    global ModulesCurrentModulefile flag_default_mf flag_default_dir

    set retlist ""

    if {$mod == ""} {
	return ""
    }

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {$g_debug} {
	report "DEBUG getPathToModule: Finding $mod"
    }

    # Check for aliases

    set newmod [resolveModuleVersionOrAlias $mod]
    if {$newmod != $mod} {
	# Alias before ModulesVersion
	return [getPathToModule $newmod]
    }

    # Check for $mod specified as a full pathname
    if {[string match {/*} $mod]} {
	if {[file exists $mod]} {
	    if {[file readable $mod]} {
		if {[file isfile $mod]} {
		    # note that a raw filename as an argment returns the full\
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
	foreach dir [split $env(MODULEPATH) $seperator] {
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
		# If a directory, return the defailt if a .version file is\
		  present or return the last file
		# in the dir
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
			set ModulesVersion [lindex [listModules $path "" 0\
			  $flag_default_mf $flag_default_dir] end]
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

    if {[info exists env(MODULERCFILE)]} {
	if {[file readable $env(MODULERCFILE)]} {
	    if {$g_debug} {
		report "DEBUG runModulerc: Executing $env(MODULERCFILE)"
	    }
	    execute-modulerc $env(MODULERCFILE)
	}
    }
    if {[info exists env(MODULESHOME)]} {
	if {[file readable "$env(MODULESHOME)/etc/rc"]} {
	    if {$g_debug} {
		report "DEBUG runModulerc: Executing $env(MODULESHOME)/etc/rc"
	    }
	    execute-modulerc "$env(MODULESHOME)/etc/rc"
	}
    }
    if {[info exists env(HOME)]} {
	if {[file readable "$env(HOME)/.modulerc"]} {
	    execute-modulerc "$env(HOME)/.modulerc"
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
    global g_autoInit CSH_LIMIT

    set iattempt 0
    set f ""
    set tmpfile ""
    while {$iattempt < 100 && $f == ""} {
	set tmpfile [format "/tmp/modulescript_%d_%d" [pid] $iattempt]
	catch {set f [open $tmpfile "w"]}
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
	lisp {
		puts $f "(delete-file \"$tmpfile\")"
	    }
	}

	switch -- $g_shellType {
	python {
		puts $f "import os"

	    }
	}

	if {$g_autoInit} {
	    global argv0

	    # add cwd if not absolute script path
	    if {! [regexp {^/} $argv0]} {
		global tcl_platform
		if {$tcl_platform(platform) == "windows"} {
		    set pwd [exec pwd]
		} else {
		    set pwd [pwd]
		}
		set argv0 "$pwd/$argv0"
	    }

	    set env(MODULESHOME) [file dirname $argv0]
	    set g_stateEnvVars(MODULESHOME) "new"

	    set modulerc_init_file $env(MODULESHOME)/init/modulerc


	    switch -- $g_shellType {
	    csh {
		    puts $f "if ( \$?histchars ) then"
		    puts $f "  set _histchars = \$histchars"
		    puts $f "  if (\$?prompt) then"
		    puts $f "  alias module 'unset histchars;set\
		      _prompt=\"\$prompt\";eval `'$argv0' '$g_shell' \\!*`;set\
		      histchars = \$_histchars; set prompt=\"\$_prompt\";unset\
		      _prompt'"
		    puts $f "  else"
		    puts $f "    alias module 'unset histchars;eval `'$argv0'\
		      '$g_shell' \\!*`;set histchars = \$_histchars'"
		    puts $f "  endif"
		    puts $f "else"
		    puts $f "  if (\$?prompt) then"
		    puts $f "    alias module 'set _prompt=\"\$prompt\";set\
		      prompt=\"\";eval `'$argv0' '$g_shell' \\!*`;set\
		      prompt=\"\$_prompt\";unset _prompt'"
		    puts $f "  else"
		    puts $f "    alias module 'eval `'$argv0' '$g_shell' \\!*`'"
		    puts $f "  endif"
		    puts $f "endif"

		    if {[file exists $modulerc_init_file]} {
			puts $f "source $modulerc_init_file"
		    } else {
			reportWarning "WARNING: $modulerc_init_file does not\
			  exist"
		    }
		}
	    sh {
		    puts $f "module () { eval `'$argv0' '$g_shell' \$*`; }"
		    if {[file exists $modulerc_init_file]} {
			puts $f ". $modulerc_init_file"
		    } else {
			reportWarning "WARNING: $modulerc_init_file does not\
			  exist"
		    }
		}
	    perl {
		    puts $f "sub module {"
		    puts $f "  eval `\$ENV{MODULESHOME}/modulecmd.tcl perl @_`;"
		    puts $f "  if(\$@) {"
		    puts $f "    use Carp;"
		    puts $f "    confess \"module-error: \$@\n\";"
		    puts $f "  }"
		    puts $f "  return 1;"
		    puts $f "}"
		}
	    python {
		    puts $f "def module(command, *arguments):"
		    puts $f "        commands = os.popen('$argv0 python %s %s'\
		      % (command, string.join(arguments))).read()"
		    puts $f "        exec commands"
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
			set val [doubleQuoteEscaped $env($var)]
			# csh barfs on long env vars
			if {$g_shell == "csh" && [string length $val] >\
			  $CSH_LIMIT} {
			    if {$var == "PATH"} {
				reportWarning "WARNING: module: PATH exceeds\
				  $CSH_LIMIT characters, truncating to 900 and\
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
		lisp {
			set val [doubleQuoteEscaped $env($var)]
			puts $f "(setenv \"$var\" \"$val\")"
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
			    puts $f "alias $var=\'$val\'"
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
		lisp {
			puts $f "(setenv \"$var\" nil)"
		    }
		}
	    }
	}

	# obsolete aliases
	if {$g_shell != "sh"} {
	    foreach var [array names g_stateAliases] {
		if {$g_stateAliases($var) == "del"} {
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
	    set xrdb [findExecutable "xrdb"]
	    foreach var [array names g_newXResources] {
		set val $g_newXResources($var)
		if {$val == ""} {
		    switch -regexp -- $g_shellType {
		    {^(csh|sh)$} {
			    if {[file exists $var]} {
				puts $f "$xrdb -merge $var"
			    } else {
				puts $f "$xrdb -merge <<EOF"
				puts $f "$var"
				puts $f "EOF"
			    }
			}
		    perl {
			    if {[file isfile $var]} {
				puts $f "system(\"$xrdb -merge $var\");"
			    } else {
				puts $f "open(XRDB,\"|$xrdb -merge\");"
				set var [doubleQuoteEscaped $var]
				puts $f "print XRDB \"$var\\n\";"
				puts $f "close XRDB;"
			    }
			}
		    python {
			    if {[file isfile $var]} {
				puts $f "os.popen('$xrdb -merge $var');"
			    } else {
				set var [singleQuoteEscaped $var]
				puts $f "os.popen('$xrdb -merge').write('$var')"
			    }
			}
		    lisp {
			    if {[file exists $var]} {
				puts $f "(shell-command-to-string \"$xrdb\
				  -merge $var\")"
			    } else {
				puts $f "(shell-command-to-string \"echo $var\
				  | $xrdb -merge\")"
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
			    puts $f "os.popen('$xrdb\
			      -merge').write('$var: $val')"
			}
		    lisp {
			    puts $f "(shell-command-to-string \"echo $var:\
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
		    puts $f "xrdb -remove <<EOF"
		    puts $f "$var:"
		    puts $f "EOF"
		}
	    }
	}

	if {[info exists g_systemList]} {
	    foreach var $g_systemList {
		puts $f "$var"
	    }
	}

	# module path{s,} output
	if {[info exists g_pathList]} {
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
		lisp {
			puts $f "(message \"$var\")"
		    }
		}
	    }
	}

	###
	set nop 0
	if {$error_count == 0 && ! [tell $f]} {
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
		    puts $f "die \"modulefile.tcl: $error_count error(s)\
		      detected!\\n\""
		}
	    python {
		    # I am not a python programmer...
		}
	    lisp {
		    puts $f "(error \"modulefile.tcl: $error_count error(s)\
		      detected!\")"
		}
	    }
	    set nop 0
	} else {
	    switch -- $g_shellType {
	    perl {
		    puts $f "1;"
		}
	    }
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
	    lisp {
		    puts "t"
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
	    lisp {
		    # the module defun() expects just a pathname here
		    puts "$tmpfile"
		}
	    }
	}
    }
}

proc cacheCurrentModules {{seperator {}}} {
    global g_loadedModules g_loadedModulesGeneric env g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    # mark specific as well as generic modules as loaded
    if {[info exists env(LOADEDMODULES)]} {
	foreach mod [split $env(LOADEDMODULES) $seperator] {
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

proc removeFromList {list1 item} {
    eval set list2 \[list $list1 \]
    set list1 {}
    foreach x $list2 {
	if {$x != $item && $x != ""} {
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

proc checkValidModule {modfile} {
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
    global g_versionHash g_moduleDefault

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
proc listModules {dir mod {full_path 1} {how {-dictionary}} {flag_default_mf\
  {1}} {flag_default_dir {1}}} {
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
    set clean_list {}
    set ModulesVersion {}
    for {set i 0} {$i < [llength $full_list]} {incr i 1} {
	set element [lindex $full_list $i]
	set tag_list {}

	# Cygwin TCL likes to append ".lnk" to the end of symbolic links.
	# This is not necessary and pollutes the module names, so let's
	# trim it off.
	if {$tcl_platform(platform) == "windows"} {
	    regsub {\.lnk$} $element {} element
	}

	set tail [file tail $element]
	set direlem [file dirname $element]

	set sstart [expr {[string length $dir] +1}]
	set modulename [string range $element $sstart end]


	if {[file isdirectory $element]} {
	    set ModulesVersion ""
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
		report "DEGUG listModules: checking $element ($modulename)\
		  dir=$flag_default_dir mf=$flag_default_mf"
	    }
	    switch -glob -- $tail {
	    {.modulerc} {
		    if {$flag_default_dir || $flag_default_mf} {
			execute-modulerc $element
		    }
		}
	    {.version} {
		    if {$flag_default_dir || $flag_default_mf} {
			# set ModulesCurrentModulefile so it is available to\
			  execute-modulerc
			set ModulesCurrentModulefile $element
			execute-modulerc "$element"

			if {$g_debug} {
			    report "DEGUG listModules: checking default\
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
    if {$how != {}} {
	set clean_list [lsort -dictionary $clean_list]
    }
    if {$g_debug} {
	report "DEGUG listModules: Returning $clean_list"
    }

    return $clean_list
}


proc showModulePath {{seperator $g_def_seperator}} {
    global env g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }
    if {[info exists env(MODULEPATH)]} {
	report "Search path for module files (in search order):"
	foreach path [split $env(MODULEPATH) $seperator] {
	    report "  $path"

	}
    } else {
	reportWarning "WARNING: no directories on module search path"
    }

}

########################################################################
# command line commands

proc cmdModuleList {{seperator {}}} {
    global env DEF_COLUMNS show_oneperline show_modtimes g_debug
    global g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    set list {}
    report "Currently Loaded Modulefiles:"
    if {[info exists env(LOADEDMODULES)]} {
	set max 0
	foreach mod [split $env(LOADEDMODULES) $seperator] {
	    if {$show_oneperline} {
		report $mod
	    }\
	    elseif {$show_modtimes} {
		set filetime [clock format [file mtime [lindex\
		  [getPathToModule $mod] 0]]]
		report [format "%-50s%10s" $mod $filetime]
	    } else {
		set len [string length $mod]
		if {$len > $max} {
		    set max $len
		}
		if {$len > 0} {
		    # skip zero length module names
		    # call getPathToModule to find and execute .version and\
		      .modulerc files for this module
		    getPathToModule $mod
		    set tag_list [getVersAliasList $mod]

		    if {[llength $tag_list]} {
			append mod "(" [join $tag_list $seperator] ")"
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
	    set col_width [expr {$max +5}]
	    if {[info exists env(COLUMNS)]} {
		set cols [expr {int($env(COLUMNS)/$col_width)}]
	    } else {
		set cols [expr {int($DEF_COLUMNS/$col_width)}]
	    }
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

proc cmdModulePaths {mod {seperator {}}} {
    global env g_pathList flag_default_mf flag_default_dir g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {[catch {
	foreach dir [split $env(MODULEPATH) $seperator] {
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
    global env g_pathList ModulesCurrentModulefile

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
    global g_whatis

    if {$mod == ""} {
	set mod "*"
    }
    foreach dir [split $env(MODULEPATH) ":"] {
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
    }

    if {[info exists g_loadedModules($old)]} {
    } else {
	if {[info exists g_loadedModulesGeneric($old)]} {
	    set old "$old/$g_loadedModulesGeneric($old)"
	}
    }

    if {$g_debug} {
	report "new=\"$new\" old=\"$old\""
    }

    cmdModuleUnload $old
    cmdModuleLoad $new
}

proc cmdModuleSource {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force

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
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force
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
    global env tcl_version g_loadedModules g_loadedModulesGeneric
    global ModulesCurrentModulefile g_debug

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
			unload-path LOADEDMODULES $currentModule
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
		unload-path LOADEDMODULES $mod
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


proc cmdModulePurge {{seperator {}}} {
    global env g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {[info exists env(LOADEDMODULES)]} {
	set list [split $env(LOADEDMODULES) $seperator]
	eval cmdModuleUnload $list
    }
}


proc cmdModuleReload {{seperator {}}} {
    global env g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    if {[info exists env(LOADEDMODULES)]} {
	set list [split $env(LOADEDMODULES) $seperator]
	set rlist [reverseList $list]
	foreach mod $rlist {
	    cmdModuleUnload $mod
	}
	foreach mod $list {
	    cmdModuleLoad $mod
	}
    }
}

proc system {mycmd args} {
    global g_systemList

    set mode [currentMode]
    set mycmd [join [concat $mycmd $args] " "]

    if {$mode == "load"} {
	lappend g_systemList $mycmd
    }\
    elseif {$mode == "unload"} {
	# No operation here unable to undo a syscall.
    }\
    elseif {$mode == "display"} {
	report "system\t$mycmd"
    }
    return {}
}

proc cmdModuleAvail {{mod {*}}} {
    global env ignoreDir DEF_COLUMNS flag_default_mf flag_default_dir
    global show_oneperline show_modtimes

    if {$show_modtimes} {
	report "- Package -----------------------------+- Versions -+- Last\
	  mod. ------"
    }

    foreach dir [split $env(MODULEPATH) ":"] {
	if {[file isdirectory "$dir"]} {
	    report "------------ $dir ------------ "
	    set list [listModules "$dir" "$mod" 0 "" $flag_default_mf\
	      $flag_default_dir]
	    if {$show_modtimes} {
		foreach i $list {
		    set filetime [clock format [file mtime [lindex\
		      [getPathToModule "$i"] 0]]]
		    report [format "%-50s%10s" $i $filetime]
		}
	    }\
	    elseif {$show_oneperline} {
		foreach i $list {
		    report "$i"
		}
	    } else {
		set max 0
		foreach mod2 $list {
		    if {[string length $mod2] > $max} {
			set max [string length $mod2]
		    }
		}
		incr max 1
		if {[info exists env(COLUMNS)]} {
		    set cols [expr {int($env(COLUMNS)/($max))}]
		} else {
		    set cols [expr {int($DEF_COLUMNS/($max))}]
		}
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

    if {$args == ""} {
	showModulePath
    } else {
	set stuff_path "prepend-path"
	foreach path $args {
	    if {$path == ""} {
		# Skip "holes"
	    }\
	    elseif {($path == "--append") ||($path == "-a") ||($path ==\
	      "-append")} {
		set stuff_path "append-path"
	    }\
	    elseif {($path == "--prepend") ||($path == "-p") ||($path ==\
	      "-prepend")} {
		set stuff_path "prepend-path"
	    }\
	    elseif {[file isdirectory $path]} {
		pushMode "load"
		eval $stuff_path MODULEPATH $path
		popMode
	    } else {
		report "+(0):WARN:0: Directory '$path' not found"
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
	    regsub -all {\/} $path {\/} escpath
	    set regexp [subst {(^|\:)${escpath}(\:|$)}]
	    if {[info exists env(MODULEPATH)] && [regexp $regexp\
	      $env(MODULEPATH)]} {

		set oldMODULEPATH $env(MODULEPATH)
		pushMode "unload"
		catch {
		    unload-path MODULEPATH $path
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


proc cmdModuleDebug {{seperator {}}} {
    global env g_def_seperator

    if {$seperator == "" } {
        set seperator $g_def_seperator
    }

    foreach var [array names env] {
	set sharevar "${var}_modshare"
	array set countarr [getReferenceCountArray $var $seperator]

	foreach path [array names countarr] {
	    report "$var\t$path\t$countarr($path)"
	}
	unset countarr
    }
    foreach dir [split $env(PATH) $seperator] {
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
    global g_autoInit
    set g_autoInit 1
}

proc cmdModuleInit {args} {

    global g_shell env g_debug
    set moduleinit_cmd [lindex $args 0]
    set notdone 1
    set notclear 1

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

		if {[expr {[llength $args] -1}] != $nargs($moduleinit_cmd)} {
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
				set modules [removeFromList $modules $newmodule]
				append modules " $newmodule"
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			prepend {
				set newmodule [lindex $args 1]
				set modules [removeFromList $modules $newmodule]
				set modules "$newmodule $modules"
				puts $newfid "$cmd$modules$comments"
				set notdone 0
			    }
			rm {
				set oldmodule [lindex $args 1]
				set modules [removeFromList $modules $oldmodule]
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
	report "Modules Release Tcl $MODULES_CURRENT_VERSION (Copyright GNU\
	  GPL v2 1991):"
	report {Usage: module [ switches ] [ command ]}

	report {Switches:}
	report {	-t		terse format avail and list}
	report {	-l		long format avail and list}
	report {Commands:}
	report {	list         |  add|load            modulefile\
	  [modulefile ...]}
	report {	purge        |  rm|unload           modulefile\
	  [modulefile ...]}
	report {	reload       |  switch|swap       \
	  [oldmodulefile] newmodulefile}
	report {	             |  display|show        modulefile\
	  [modulefile ...]}
	report {	             |  avail              [modulefile\
	  [modulefile ...]]}
	report {	             |  whatis             [modulefile\
	  [modulefile ...]]}
	report {	             |  help               [modulefile\
	  [modulefile ...]]}
	report {	             |  path                modulefile}
	report {	             |  paths               modulefile}
	report {	             |  use                 dir [dir ...]}
	report {	             |  unuse               dir [dir ...]}
	report {	             |  source              scriptfile}
	report {	             |  apropos|keyword     string}
	report {	             |}
	report {	initlist     |  initadd             modulefile}
	report {	initclear    |  initprepend         modulefile}
	report {		     |  initrm              modulefile}
    }
}


########################################################################
# main program

# needed on a gentoo system. Shouldn't hurt since it is
# supposed to be the default behavior
fconfigure stderr -translation auto

# Parse options
set opt [lindex $argv 1]
switch -regexp -- $opt {
{^-deb} {
	set g_debug 1
	report "DEBUG debug enabled"

	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--verb} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--for} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--ter} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--lon} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--cre} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--us} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--ica} {
	# BOZO nothing to do with --verbose yet
	# only remove options here.  Some module commands have options also,\
	  which should
	# be preserved
	set argv [removeFromList $argv $opt]
    }
{^--ver} {
	report "$MODULES_CURRENT_VERSION"
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
    set argv [removeFromList $argv "-t"]
}
if {[lsearch $argv "-l"] >= 0} {
    set show_modtimes 1
    set argv [removeFromList $argv "-l"]
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
    {^autoinit$} {
	    cmdModuleAutoinit
	    renderSettings
	}
    help {
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
