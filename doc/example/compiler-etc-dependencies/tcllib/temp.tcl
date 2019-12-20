## =======  Utilities related to which compiler is loaded, etc =========

#--------------------------------------------------------------------
# GetDefaultCompiler:
#
# Returns the default compiler, gcc/8.2.0
proc GetDefaultCompiler { } {
   return "gcc/8.2.0"
}

#--------------------------------------------------------------------
# RequireCompiler:
#
# Does a module load of specified compiler $mycomp.
# Includes special handling if $mycomp is the default compiler
proc RequireCompiler { mycomp } {
   # If your module tree is set up so that there is no module for the
   # default compiler (because e.g. it is available w/out loading a module
   # anyway), you can uncomment the following block which will cause
   # RequireCompiler to do nothing if mycomp is the default compiler
   #set defComp [GetDefaultCompiler]
   #if { $mycomp eq $defComp } {
      #return
   #}
   module load $mycomp
}

#--------------------------------------------------------------------
# GetKnownCompilerFamilies:
#
# Returns a list of recognized compiler family names
#E.g. gcc, intel, pgi
proc GetKnownCompilerFamilies { } {
   set cfamilies {gcc intel pgi}
   return $cfamilies
}

#--------------------------------------------------------------------
# GetLoadedCompiler:
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

