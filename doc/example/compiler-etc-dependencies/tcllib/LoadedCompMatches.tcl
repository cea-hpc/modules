# =========================================================
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# LoadedCompilerMatches:
#
# Takes the tag of the desired compiler $wanted (in form of FAMILY/VERSION)
# Checks if any compiler is loaded, and will throw error if one is
# loaded and it does not match $wanted.
# If compilers match, it will prereq the compiler if $requireIt set
# If no compiler is loaded, will load if optional boolean parameter
# $loadIt is set (default unset), and prereq if $requireIt is set, and
# then return.
# Option parameters:
# requireIt: boolean, default false. If set, require $wanted
# loadIt: boolean, default false.  If set, load $wanted if no compiler
#	already loaded
# modTag: string, defaults go [module-info specified].  Used in error messages
proc LoadedCompilerMatches { wanted {requireIt 0} { loadIt 0 } {modTag {} } } {
   # Default modTag
   if { $modTag eq {} } { set modTag [ module-info specified ] }
   # If no compiler given in $wanted, just return
   if { $wanted eq {} } { return }

   # Get loaded compiler (w/out defaults)
   set loaded [ GetLoadedCompiler 0  0 ]
   # If no compiler is loaded, then require it if asked, and return
   if { $loaded eq {} } {
      if { $loadIt } {
         RequireCompiler $wanted
         if { $requireIt } { prereq $wanted }
      }
      return
   }

   # Have a loaded compiler, split into family and version
   set tmpLoaded [ GetPackageFamilyVersion $loaded ]
   set loadedFam [ lindex $tmpLoaded 0 ]
   set loadedVer [ lindex $tmpLoaded 1 ]
   # Always use 'gcc' not 'gnu'
   if { $loadedFam eq {gnu} } { set loadedFam gcc }

   # Split wanted into family and version
   set tmpWanted [ split $wanted / ]
   set wantedLen [ llength $tmpWanted ]
   #Nothing to do if no components to wanted
   if { $wantedLen < 1 } { return }

   set wantedFam [ lindex $tmpWanted 0 ]
   # Always use 'gcc' not 'gnu'
   if { $wantedFam eq {gnu} } { set wantedFam gcc }

   # Ensure families match
   if { $loadedFam ne $wantedFam } {
      PrintLoadError "Compiler Mismatch
Package $modTag does not appear to be built for currently
loaded compiler $loaded."
   }

   # OK, families match
   # If no version specified in $wanted, we are basically done
   if { $wantedLen < 2 } {
      if { $requireIt } { prereq $wanted }
      return
   }

   # Ensure versions match
   set wantedVer [ lindex $tmpWanted 1 ]
   if { $loadedVer eq $wantedVer } {
      if { $requireIt } { prereq $wanted }
      return
   }
   # Versions don't match
   PrintLoadError "Compiler Mismatch
Package $modTag does not appear to be built for currently
loaded compiler $loaded."
}
