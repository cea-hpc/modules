#--------------------------------------------------------------------
# DefaultCompilerFromPath
#
# Returns the string for the compiler we are using (i.e. was previously
# module loaded), or empty string if nothing loaded (from is-loaded command)
# E.g., intel/2013.1.117
# Takes two optional arguments, 
#	useDefault: boolean, default false.  If set, return the value of
#		GetDefaultCompiler if no compiler is loaded
#	requireDefault: boolean, default false.  If set and useDefault is
#		set and the default compiler is returned because no 
#		compiler was loaded, we `module load` the default compiler.
#		Ignored unless useDefault is set

proc GetLoadedCompiler { { useDefault 0} { requireDefault 0 } } {
   set ctag {}
   foreach cfam [ GetKnownCompilerFamilies ] {
      if { [ is-loaded $cfam ] } {
         set ctag [ GetTagOfModuleLoaded $cfam  ]
         return $ctag
      }
   }
   # No loaded compiler found, should we return default compiler
   if { $useDefault } {
      #Yes
      set ctag [ GetDefaultCompiler ]
      #Should we module load the default compiler???
      if { $requireDefault } {
         RequireCompiler $ctag
      }
   }
   return $ctag
}

