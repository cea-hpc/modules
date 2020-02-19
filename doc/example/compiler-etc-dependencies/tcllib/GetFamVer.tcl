# =========================================================
# Author: Tom Payerle <payerle@umd.edu>

#--------------------------------------------------------------------
# GetPackageFamilyVersion
#
# Given the fully qualified spec for a modulefile, returns a list
# with the package family name and version.
#
# E.g., for foo/1.1/gcc/8.2.0/openmpi/3.1 would return {foo 1.1}
# But also handles stuff like foo/gcc/8.2.0/openmpi/3.1/1.1 (returning
# again {foo 1.1})
proc GetPackageFamilyVersion { tag } {
   # Return empty list if given empty tag
   if { $tag eq {} } { return {} }

   # Split tag into components
   set components [ split $tag / ]
   set compLen [ llength $components ]
   if { $compLen < 1 } { return {} }

   # Family should always be first
   set family [ lindex $components 0 ]
   if { $compLen < 2 } { return $family }

   # First guess, version immediately follows family
   set version [ lindex $components 1 ]
   # Check if it matches known non-version stuff following family
   # Compilers, MPIs, other things branch on
   set nonVers { gnu gcc pgi intel openmpi ompi intelmpi impi mvapich
      mvapich2 avx avx2 sse4.1 sse3 python perl hdf hdf4 hdf5 }
   set tmp [ lsearch $nonVers $version ]
   if { $tmp > -1 } {
      # The component immediately following family was NOT a version
      # Use last component as version
      set version [ lindex $components end ]
   }
   return  "$family $version"
}
