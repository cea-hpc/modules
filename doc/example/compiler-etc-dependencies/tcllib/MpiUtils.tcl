# =======  Utilities related to which MPI library is loaded, etc =========
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# RequireMPI:
#
# Does a module load of specified MPI library $mympi
# Includes special handling if $mympi is nompi or intelmpi (or one of its aliases)
# If $mympi is nompi, nothing is loaded.
# If the optional parameter noLoadIntel is set (default false), and if
# $mympi is intelmpi (or intel or impi), then we do not load module if
# the loaded compiler is intel (as we assume that provides intel MPI as well)
proc RequireMPI { mympi {noLoadIntel 0} } {
   # We do not do anything if mympi is nompi
   if { $mympi eq {nompi} } { return }
   if { $noLoadIntel } {
      # Get the basename of requested MPI library
      set mympiSplit [ split $mympi / ]
      set mympiBase [ lindex $mympiSplit 0 ]
      # Check if we requested an intel MPI
      set intelList "intelmpi impi intel intelmpi-mt impi-mt intel-mt"
      if { [lsearch $intelList $mympiBase ] > -1 } {
         # We requested intelmpi in some form
         # Check if an intel compiler was loaded
         set curComp [ GetLoadedCompiler 1]
         if { $curComp ne {} } {
            set curCompSplit [ split $curComp / ]
            set curCompBase [ lindex $curCompSplit 0 ]
            if { $curCompBase eq {intel} } {
               # $noLoadIntel is set, requested MPI is intel MPI, and intel compiler loaded
               return
            }
         }
      }
   }
   module load $mympi
}

#--------------------------------------------------------------------
# GetKnownMpiFamilies:
#
# Returns a list of recognized MPI library family names
#E.g. gcc, intel, pgi
proc GetKnownMpiFamilies { } {
   set mfamilies {openmpi mpavich intelmpi}
   return $mfamilies
}

#--------------------------------------------------------------------
# GetLoadedMPI:
#
# Returns the string for the MPI library we are using (i.e. was previously
# module loaded), or empty string if nothing loaded (from is-loaded command)
# E.g., intel/2013.1.117
# Takes an optional argument,
#	useIntel: boolean, default false.  If set, returns 'intelmpi' if
#		no MPI library is loaded but intel compiler is loaded
#	forceIt: boolean, default false.  If set, prereq MPI lib before returning.
#	requireIt: boolean, default false.  If set, prereq the MPI library
proc GetLoadedMPI { { useIntel 0} {forceIt 0} {requireIt 0} } {
   set mtag {}
   foreach mfam [ GetKnownMpiFamilies ] {
      if { [ is-loaded $mfam ] } {
         set mtag [ GetTagOfModuleLoaded $mfam  ]
         if { $requireIt } { prereq $ctag }
         return $mtag
      }
   }
   # No loaded compiler found, should we check for Intel compiler and return intelmpi?
   if { $useIntel } {
      #Yes
      set ctag [ GetCompilerLoaded ]
      set cSplit [ split $ctag / ]
      set cBase [ lindex $cSplit 0 ]
      if { $cBase eq intel } { return intelmpi }
   }
   return $mtag
}
