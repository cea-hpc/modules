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
echo FATAL: module: Could not find tclsh on \$PATH or in standard directories; exit 1


########################################################################
#
# This is a pure TCL implementation of the module command


########################################################################
# commands run from inside a module file
#

set ignoreDir(CVS) 1
set ignoreDIr(RCS) 1
set ignoreDir(SCCS) 1
set g_reloadMode  0
set error_count  0
set g_force 0

proc module-info {what {more {}}} {
    upvar g_mode mode

    switch $what {
	"name" {
	    upvar currentModule currentModule
	    return $currentModule
	}
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
	default {
	    error "module-info $what not supported"
	}
    }
}

proc module-whatis {message} {
    upvar g_mode  mode
    upvar g_whatis whatis

    if {$mode == "display"} {
	puts stderr "module-whatis\t$message"
    } elseif {$mode == "whatis"} {
	set whatis $message
    }
}

proc module {command args} {
    upvar g_mode mode
    global g_reloadMode

    if { $g_reloadMode == 1} {
	return
    }

    switch $command {
	add -
	load {
	    if {$mode == "load"} {
#puts stderr "DEBUG: module load: cmdModuleLoad <$args>"
		eval cmdModuleLoad $args
	    } elseif {$mode == "unload"} {
#puts stderr "DEBUG: module load: cmdModuleUnload <$args>"
		eval cmdModuleUnload $args
	    } elseif {$mode == "display"} {
		puts stderr "module load $args"
	    }
	}
	rm -
	unload {
	    if {$mode == "load"} {
		eval cmdModuleUnload $args
	    } elseif {$mode == "unload"} {
		eval cmdModuleUnload $args
	    } elseif {$mode == "display"} {
		puts stderr "module unload $args"
	    }
	}
	use {
#puts stderr "DEBUG: module: cmdModuleUse <$args>"
	    eval cmdModuleUse $args
	}
	unuse {
	    eval cmdModuleUnuse $args
	}
	default {
	    error "module $command not understood"
	}
    }
}

proc setenv {var val} {
    global g_stateEnvVars env
    upvar g_mode mode

    if {$mode == "load"} {
	set env($var) $val
	set g_stateEnvVars($var) "new"
    } elseif {$mode == "unload"} {
	if [info exists env($var)] {
	    unset env($var)
	}
	set g_stateEnvVars($var) "del"
    } elseif {$mode == "display"} {
	puts stderr "setenv\t$var\t$val"
    }
}

proc unsetenv {var} {
    global g_stateEnvVars env
    upvar g_mode mode

    if {$mode == "load"} {
	if [info exists env($var)] {
	    unset env($var)
	}
	set g_stateEnvVars($var) "del"
    } elseif {$mode == "display"} {
	puts stderr "unsetenv\t$var"
    }
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
		    puts stderr "WARNING: \$$var does not agree with \$${var}_modshare counter. The following directories' usage counters were adjusted to match. Note that this may mean that module unloading may not work correctly."
		    foreach dir [array names fixers] {
			puts -nonewline stderr " $dir"
		    }
		    puts stderr ""
		  }
		}

	    } else {
#		puts stderr "WARNING: module: $sharevar was corrupted, odd number of elements."
		set modshareok 0
	    }
	} else {
#	    puts stderr "WARNING: module: $sharevar exists ( $env($sharevar) ), but $var doesn't. Environment is corrupted. Contact lakata@mips.com for help."
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
    global g_stateEnvVars env

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

	if {$doit} {
	    if [info exists env($var)] {
		set dirs [split $env($var) ":"]
		set newpath ""
		foreach elem $dirs {
		    if {$elem != $dir} {
			lappend newpath $elem
		    }
		}
		if {$newpath == ""} {
		    if [info exists env($var)] {
			unset env($var)
		    }
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
	if [info exists env($sharevar)] {
	    unset env($sharevar)
	}
	set g_stateEnvVars($sharevar) "del"
    }
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
	    #	    puts stderr "INFO: already see $dir in $var"
	    incr countarr($dir)
	} else {
	    if [info exists env($var)] {
		if {$pos == "prepend"} {
		    set env($var) "$dir:$env($var)"
		} elseif {$pos == "append"} {
		    set env($var) "$env($var):$dir"
		} else {
		    error
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
}

proc prepend-path {var path} {
    upvar g_mode mode

    if {$mode == "load"} {
	add-path $var $path "prepend"
    } elseif {$mode == "unload"} {
	unload-path $var $path
    } elseif {$mode == "display"} {
	puts stderr "prepend-path\t$var\t$path"
    }
}


proc append-path {var path} {
    upvar g_mode mode

    if {$mode == "load"} {
	add-path $var $path "append"
    } elseif {$mode == "unload"} {
	unload-path $var $path
    } elseif {$mode == "display"} {
	puts stderr "append-path\t$var\t$path"
    }
}

proc set-alias {alias what} {
    global g_Aliases g_stateAliases
    upvar g_mode mode

    if {$mode == "load"} {
	set g_Aliases($alias) $what
	set g_stateAliases($alias) "new"
    } elseif {$mode == "unload"} {
	set g_Aliases($alias) $what
	set g_stateAliases($alias) "del"
    } elseif {$mode == "display"} {
	puts stderr "alias\t$alias\t$what"
    }
}


proc conflict {args} {
    global g_loadedModules g_loadedModulesGeneric g_reloadMode
    upvar g_mode mode
    upvar currentModule currentModule

    if { $g_reloadMode == 1} {
	return
    }

    if {$mode == "load"} {
	foreach conflict $args {
	    set modpath [split $conflict "/"]
	    if {[llength $modpath] == 1} {
		if { [info exists g_loadedModulesGeneric($conflict) ] } {
		    set x $conflict/$g_loadedModulesGeneric($conflict)
		    if { $x != $currentModule } {
			error "Conflict with loaded module $x \nHINT: Do 'module unload $x' and then try again."
		    }
		}
	    } else {
		if { [info exists g_loadedModules($conflict)] &&
		     $conflict != $currentModule } {
		    error "Conflict ($currentModule) with loaded module: $conflict"
		}
	    }
	}
    } elseif {$mode == "display"} {
	puts stderr "conflict\t$args"
    }
}

proc x-resource {resource {value {}}} {
    global g_newXResources g_delXResources
    upvar g_mode mode

    if {$mode == "load"} {
	set g_newXResources($resource) $value
    } elseif {$mode =="unload"} {
	set g_delXResources($resource) 1
    } elseif {$mode == "display"} {
	puts stderr "x-resource\t$resource\t$value"
    }
}

proc uname {what} {
    global unameCache
    if { ! [info exists unameCache($what)] } {
	switch $what {
	    sysname {
		catch { exec /bin/uname -s } result
	    }
	    machine {
		catch { exec /bin/uname -p } result
	    }
	    nodename -
	    node {
		catch { exec /bin/uname -n } result
	    }
	    release {
		catch { exec /bin/uname -r } result
	    }
	    default {
		error "uname setting $what not implemented"
	    }
	}
	set unameCache($what) $result
    }

    return $unameCache($what)
}

########################################################################
# internal module procedures


proc getPathToModule {mod} {
    global env g_loadedModulesGeneric

    if [info exists env(MODULEPATH)] {
	foreach dir [split $env(MODULEPATH) ":"] {
	    set path "$dir/$mod"
	    if [file exists $path] {
		if [file readable $path] {
		    if [file isdirectory $path] {
			if [info exists g_loadedModulesGeneric($mod)] {
			    set ModulesVersion $g_loadedModulesGeneric($mod)
			} elseif [file exists "$path/.version"] {
			    source "$path/.version"
			} else {
			    set ModulesVersion [file tail [lindex [lsHack $path] 0]]
			}
			set path "$path/$ModulesVersion"
			set mod "$mod/$ModulesVersion"
		    }

		    if [file isfile $path] {
			return [list $path $mod]
		    }
		} else {
		    error "$path not readable"
		}
	    }
	}
	error "Module $mod not found on \$MODULEPATH.\nHINT: Use 'module use ...' to add to search path."
    } else {
	error "\$MODULEPATH not defined"
    }
}

proc renderSettings {} {
    global env g_Aliases g_shellType g_shell
    global g_stateEnvVars g_stateAliases
    global g_newXResources g_delXResources
    global error_count

    set iattempt 0
    set f ""
    while {$iattempt < 100 && $f == ""} {
	set tmpfile [format "/tmp/modulescript_%d_%d" [pid] $iattempt]
	set f [open  $tmpfile "w"]
	incr iattempt
    }

    if {$f == ""} {
	error "Could not open a temporary file in /tmp/modulescript_* !"
    } else {

# required to work on cygwin, shouldn't hurt real linux
	fconfigure $f -translation lf

# preliminaries
	switch $g_shellType {
	    python {
		puts $f "import os";
	    }
	}

# new environment variables
	foreach var [array names g_stateEnvVars] {
	    if {$g_stateEnvVars($var) == "new"} {
		switch $g_shellType {
		    csh {
			set val [doubleQuoteEscaped $env($var)]
# csh barfs on long env vars
			if {$g_shell == "csh" && [string length $val] > 1000} {
			    if { $var == "PATH"} {
				puts stderr "WARNING: module: PATH exceeds 1000 characters, truncating to 900 and appending /usr/bin:/bin ..."
				set val [string range $val 0 900]:/usr/bin:/bin
			    } else {
				puts stderr "WARNING: module: $var exceeds 1000 characters, truncating..."
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
	foreach var [array names g_stateAliases] {
	    if {$g_stateAliases($var) == "new"} {
		switch $g_shellType {
		    csh {
			set val [multiEscaped $g_Aliases($var)]
			puts $f "alias $var $val"
		    }
		    sh {
			set val $g_Aliases($var)
			puts $f "function $var () {\n$val\n}"
		    }
		}
	    }
	}

# obsolete (deleted) env vars
	foreach var [array names g_stateEnvVars] {
	    if {$g_stateEnvVars($var) == "del"} {
		switch $g_shellType {
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
# this is not correct, but I'm not a python programmer
			puts $f "os.environ\['$var'\] = ''"
		    }
		}
	    }
	}
# obsolete aliases
	foreach var [array names g_stateAliases] {
	    if {$g_stateAliases($var) == "def"} {
		switch $g_shellType {
		    csh {
			puts $f "unalias $var"
		    }
		    sh {
			if {$g_shell == "zsh"} {
			    puts $f "unfunction $var"
			} else {
			    puts $f "unset $var"
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
		    switch -regexp $g_shellType {
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
		    switch -regexp $g_shellType {
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

	set nop 0
	if {  $error_count == 0  && ! [tell $f] } {
	    set nop 1
	}

	switch $g_shellType {
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

	if {$error_count > 0} {
	    puts stderr "ERROR: $error_count error(s) detected."
	    switch $g_shellType {
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
#	    puts stderr "WARNING: nothing written!"
	    file delete $tmpfile
	    switch $g_shellType {
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
	    switch $g_shellType {
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

# this is a hack to work around the fact that globs don't work
# well on Cygwin with symlinks

proc lsHack {dir} {
    global tcl_platform ignoreDir

    if {$tcl_platform(os) == "Windows NT"} {
	set fileParts [lrange [split $dir {/}] 0 1]
	if {[llength $fileParts] <= 1} {
	    lappend fileParts *
	}
	set pattern [join $fileParts {/}]
	set globlist {}
	catch {
#	    puts stderr  "globbing $pattern"
	    set globlist [exec ls -d $pattern]
	}
    } else {
	set globlist [glob -nocomplain "$dir/*"]
    }
    foreach file $globlist {
	set pieces [split $file "/"]
	set ok 1
	foreach piece $pieces {
#puts stderr "DEBUG: lsHack: Checking <$piece>"
	    if [info exists ignoreDir($piece)] {
#puts stderr "DEBUG: lsHack: <$piece> is an ignoreDir"
		set ok 0
	    }
# cygwin tcl doesn't seem to filter out hidden (dot) files
	    if [regexp {^\.} $piece] {
		set ok 0
	    }
	}
	if {$ok == 1} {
	    lappend cleanlist $file
	}
    }
    # Make sure cleanlist isn't empty
    lappend cleanlist
    return $cleanlist
}


proc showModulePath {} {
    global env
    if [info exists env(MODULEPATH)] {
	puts stderr "Search path for module files (in search order):"
	foreach path [split $env(MODULEPATH) ":"] {
	    puts stderr "  $path";
	}
    } else {
	puts stderr "WARNING: no directories on module search path"
    }

}

########################################################################
# command line commands

proc cmdModuleList {} {
    global env

    if [info exists env(LOADEDMODULES)] {
	set list [split $env(LOADEDMODULES) ":"]
	set max 0
	foreach mod $list {
	    if {[string length $mod] > $max} {
		set max [string length $mod]
	    }
	}
# save room for numbers and spacing: 2 digits + ) + space + space
	incr max 1
	set cols [expr int(79/($max+4))]
	set lines [expr int(([llength $list] -1)/ $cols) +1]
	for {set i 0} { $i < $lines} {incr i} {
	    for {set col 0} {$col < $cols } { incr col} {
		set index [expr $col * $lines + $i]
		set mod [lindex $list $index]
		if {$mod != ""} {
		    set mod [format "%2d) %-${max}s" $index $mod]
		    puts -nonewline stderr $mod
		}
	    }
	    puts stderr ""
	}
    }
}

proc cmdModuleDisplay {mod} {
    global env tcl_version

    set g_mode "display"
    catch {
	set modfile [getPathToModule $mod]
	set currentModule [lindex $modfile 1]
	set modfile [lindex [getPathToModule $mod] 0]
	puts stderr "-------------------------------------------------------------------"
	puts stderr "$modfile:\n"
	source $modfile
	puts stderr "-------------------------------------------------------------------"
	set junk ""
    } errMsg
    if {$errMsg != ""} {
	puts stderr "ERROR: module display $mod failed. $errMsg"
    }
}

proc cmdModuleWhatIs {{mod {}}} {
    cmdModuleSearch $mod {}
}

proc cmdModuleApropos {{search {}}} {
    cmdModuleSearch {} $search
}

proc cmdModuleSearch {{mod {}} {search {}} } {
    global env tcl_version

    if {$mod == ""} {
	set mod  "*"
    }
    foreach dir [split $env(MODULEPATH) ":"] {
	if [file isdirectory $dir] {
	    puts stderr "---- $dir ---- "
	    cd $dir
	    array set availHash {}
	    if [file isfile $mod] {
		set availHash($mod) 1
	    }
	    foreach file [lsHack $mod] {
		set availHash($file) 1
	    }
	    foreach mod2 [lsort -dictionary [array names availHash]] {
		set g_whatis ""
		catch {
		    set modfile [getPathToModule $mod2]
		    set currentModule [lindex $modfile 1]
		    set modfile       [lindex $modfile 0]
		    set g_mode "whatis"
		    source $modfile
		    set junk ""
		} errmsg
		if {$search =="" || [regexp -nocase $search $g_whatis]} {
		    puts stderr [format "%20s: %s" $mod2 $g_whatis]
		}
	    }
	    unset availHash
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
	    set g_mode "load"
	    source $file
	} else {
	    error "File $file does not exist"
	}
    }
}

proc cmdModuleLoad {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric g_force

#puts stderr "DEBUG: cmdModuleLoad: args <$args>"

    foreach mod $args {
#puts stderr "DEBUG: cmdModuleLoad: mods <$mod>"
	catch {
	    set modfile [getPathToModule $mod]
#puts stderr "DEBUG: cmdModuleLoad: modfile <$modfile>"
	    set currentModule [lindex $modfile 1]
	    set modfile       [lindex $modfile 0]
#puts stderr "DEBUG: cmdModuleLoad: currentModule <$currentModule>"
#puts stderr "DEBUG: cmdModuleLoad: modfile <$modfile>"

	    if { $g_force || ! [ info exists g_loadedModules($currentModule)]} {
		set g_mode "load"

		append-path LOADEDMODULES $currentModule
		set g_loadedModules($currentModule) 1
		set genericModName [file dirname $mod]

		if { ![ info exists g_loadedModulesGeneric($genericModName) ] } {
		    set g_loadedModulesGeneric($genericModName) [file tail $currentModule]
		}
		source $modfile
	    }
	    set junk ""
	} errMsg

        if {$errMsg != ""} {
	    puts stderr "ERROR: module: module load $mod failed. $errMsg"
	}
    }
}

proc cmdModuleUnload {args} {
    global env tcl_version g_loadedModules g_loadedModulesGeneric

    foreach mod $args {
	catch {
	    catch {
		set modfile [getPathToModule $mod]
		set currentModule [lindex $modfile 1]
		set modfile       [lindex $modfile 0]
		set junk ""
	    } moderr
	    if { $moderr == ""} {
		if [ info exists g_loadedModules($currentModule)] {
		    set g_mode "unload"
		    source $modfile
		    unload-path LOADEDMODULES $currentModule
		    unset g_loadedModules($currentModule)
		    if [info exists g_loadedModulesGeneric([file dirname $currentModule])] {
			unset g_loadedModulesGeneric([file dirname $currentModule])
		    }
		}
	    } else {
		puts stderr "WARNING: module: Could not find module $mod for unload, $moderr\n"
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
	    puts stderr "ERROR: module: module unload $mod failed. $errMsg"
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


proc cmdModuleAvail { {mod {}}} {
    global env

    if {$mod == ""} {
	set mod  "*"
    }
    foreach dir [split $env(MODULEPATH) ":"] {
	if [file isdirectory $dir] {
	    puts stderr "---- $dir ---- "
	    cd $dir
	    array set availHash {}
	    if [file isfile $mod] {
		set availHash($mod) 1
	    }
#puts stderr "globing $dir/$mod/*"
	    foreach file [lsHack $mod] {

#puts stderr "glob returned $file"
		set availHash($file) 1
	    }
	    set list [lsort -dictionary [array names availHash]]

	    set max 0
	    foreach mod2 $list {
		if {[string length $mod2] > $max} {
		    set max [string length $mod2]
		}
	    }
	    incr max 1
	    set cols [expr int(79/($max))]
	    set lines [expr int(([llength $list] -1)/ $cols) +1]
	    for {set i 0} { $i < $lines} {incr i} {
		for {set col 0} {$col < $cols } { incr col} {
		    set index [expr $col * $lines + $i]
		    set mod2 [lindex $list $index]
		    if {$mod2 != ""} {
			set mod2 [format "%-${max}s" $mod2]
			puts -nonewline stderr $mod2
		    }
		}
		puts stderr ""
	    }

	    unset availHash
	}
    }
}

proc cmdModuleUse {args} {

    set g_mode "load"
#puts stderr "DEBUG: cmdModuleUse: args <$args>"
    if {$args == ""} {
	showModulePath
    } else {
	set stuff_path "prepend-path"
	foreach path [split $args " "] {
#puts stderr "DEBUG: cmdModuleUse: <$path>"
	    # -a -append --append (and -p -prepend --prepend) would be nice...
	    if { $path == "" } {
		# Skip "holes"
	    } elseif { $path == "--append" } {
		set stuff_path "append-path"
	    } elseif {$path == "--prepend"} {
		set stuff_path "prepend-path"
	    } elseif [file isdirectory $path] {
#puts stderr "DEBUG: cmdModuleUse: $stuff_path MODULEPATH $path"
		eval $stuff_path MODULEPATH $path
	    } else {
		puts stderr "WARNING: Directory \"$path\" does not exist, not added to module search path."
	    }
	}
    }
}


proc cmdModuleUnuse {args} {

    set g_mode "unload"
    if {$args == ""} {
	showModulePath
    } else {
	global env
	foreach path $args {
	    if [info exists env(MODULEPATH)] {
		set oldMODULEPATH $env(MODULEPATH)
		unload-path MODULEPATH $path
		if { [info exists env(MODULEPATH) ] &&
		     $oldMODULEPATH == $env(MODULEPATH)} {
		    puts stderr "WARNING: Did not unuse $path"
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
	    puts stderr "$var\t$path\t$countarr($path)"
	}
	unset countarr
    }
    foreach dir  [split $env(PATH) ":"] {
	foreach file [lsHack $dir] {
	    if [file exists $file] {
		set exec [file tail $file]
		if [info exists execcount($exec)] {
		    lappend execcount($exec) $file
#		    puts stderr "conflict1: $execcount($exec)"
#		    puts stderr [llength $execcount($exec)]
		} else {
		    lappend execcount($exec) $file
#		    set execcount($exec) $file
		}
	    }
	}
    }
    foreach file [lsort -dictionary [array names execcount]] {
	if {[llength $execcount($file)] > 1} {
	    puts stderr "$file:\t$execcount($file)"
	}
    }
}

########################################################################
# main program

global g_shellType

set g_shell [lindex $argv 0]
set command [lindex $argv 1]
set argv [lreplace $argv 0 1]

switch -regexp $g_shell {
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

catch {
    switch -regexp $command {
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
#puts stderr "DEBUG: main: Calling cmdModuleLoad, argv <$argv>"
	    eval cmdModuleLoad $argv
	    renderSettings
	}
	{^source} {
	    eval cmdModuleSource $argv
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
#puts stderr "DEBUG: main: Calling cmdModuleUse, argv <$argv>"
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
	default {
	    puts stderr {
		ModulesTcl 0.98:
		Available Commands and Usage:
		 add|load              modulefile [modulefile ...]
		 rm|unload             modulefile [modulefile ...]
                 reload
		 source                scriptfile
		 switch|swap           modulefile1 modulefile2
		 display|show          modulefile [modulefile ...]
		 avail                 [modulefile [modulefile ...]]
		 use                   dir [dir ...]
		 unuse                 dir [dir ...]
		 purge
		 list
		 whatis                [modulefile [modulefile ...]]
		 apropos|keyword       string
	    }
	}

    }
} errMsg

if {$errMsg != ""} {
    puts stderr "ERROR: $errMsg"
}

# ;;; Local Variables: ***
# ;;; mode:tcl ***
# ;;; End: ***
