# =========================================================
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# LoadedMpiMatches:
#
# Takes the tag of the desired MPI library $wanted (in form of FAMILY/VERSION)
# Checks if any MPI is loaded, and will throw error if one is
# loaded and it does not match $wanted.
# If MPIs match, it will prereq the MPI lib if $requireIt set
# If no MPI is loaded, will load if optional boolean parameter
# $loadIt is set (default unset), and prereq if $requireIt is set, and
# then return.
# Option parameters:
# requireIt: boolean, default false. If set, will prereq $wanted
# loadIt: boolean, default false. If set, load $wanted
#	if no MPI is already loaded
# noLoadIntel: boolean, passed to RequireMPI.  If set,
#	will not attempt to load intelmpi variants if compiler is intel
# forceNoMpi: boolean, default false.  If set and $wanted is 'nompi',
#  	insist that no MPI module is loaded
# modTag: string, defaults go [module-info specified].  Used in error messages
proc LoadedMpiMatches { wanted {requireIt 0} { loadIt 0 } { noLoadIntel 0 }
   { forceNoMpi 0 } {modTag {} } } {
   # Default modTag
   if { $modTag eq {} } { set modTag [ module-info specified ] }
   # If no MPI given in $wanted, just return
   if { $wanted eq {} } { return }

   # Get loaded MPI, no defaulting to intelmpi
   set loaded [ GetLoadedMPI 0 ]

   # If wanted is nompi, no need to do anything unless forceNoMpi set
   if { $wanted eq {nompi} } {
      if { $forceNoMpi } {
         # Complain if any MPI loaded
         if { $loaded ne {} } {
            PrintLoadError "MPI Mismatch
You have an MPI library loaded ($loaded), but package $modTag
really insists that no MPI library be loaded.

Please unload the MPI library and try again."
         }
      }
      return
   }

   # If no MPI is loaded, then load it if $loadIt (this is valid even
   # in edge cases of nompi or intelmpi), prereq it if $requireIt,
   # abd return
   if { $loaded eq {} } {
      if { $loadIt } {
         RequireMPI $wanted $noLoadIntel
         if { $requireIt } { prereq $wanted }
      }
      return
   }

   # Have a loaded MPI, split into family and version
   set tmpLoaded [ GetPackageFamilyVersion $loaded ]
   set loadedFam [ lindex $tmpLoaded 0 ]
   set loadedVer [ lindex $tmpLoaded 1 ]

   # Split wanted into family and version
   set tmpWanted [ split $wanted / ]
   set wantedLen [ llength $tmpWanted ]
   #Nothing to do if no components to wanted
   if { $wantedLen < 1 } { return }
   set wantedFam [ lindex $tmpWanted 0 ]

   # Ensure families match
   if { $loadedFam ne $wantedFam } {
      PrintLoadError "MPI Mismatch

Package $modTag does not appear to be built for currently
loaded MPI library $loaded."
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
   PrintLoadError "MPI Mismatch

Package $modTag does not appear to be built for currently
loaded MPI library $loaded."
}
