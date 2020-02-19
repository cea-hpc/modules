# =======  Utilities related to which compiler is loaded, etc =========
# Author: Tom Payerle <payerle@umd.edu>

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
# module loaded).  E.g., gcc/8.2.0
# If no compiler was previously loaded, then if the optional parameter
# $pathDefault is set, it will look for a compiler family and
# version in the last components to the path to the current modulefile or
# .modulerc, and if found, uses that.
# If still no path found, it will return the value of [GetDefaultCompiler]
# if $useDefault is set.
# Otherwise, returns empty string.
#
# Takes the following arguments:
#	pathDefault: boolean, default false.  If set, attempt to determine
#		the compiler from the full path to the modulefile if
#		no compiler was loaded.
#	useDefault: boolean, default false.  If set, return the value of
#		GetDefaultCompiler if no compiler is loaded or found from
#		path (if $pathDefault).
#	loadIt: boolean, default false.  If set and a compiler
#		was defaulted from path of GetDefaultCompiler, we will
#		module load that compiler.
#		Ignored unless either pathDefault or useDefault is set
#	requireIt: boolean, default false.  If set, we will prereq the
#		compiler before returning.
proc GetLoadedCompiler {{pathDefault 0} { useDefault 0}
   {loadIt 0 } { requireIt 0 } } {
   global ModulesCurrentModulefile
   set ctag {}
   set cfams [ GetKnownCompilerFamilies ]
   foreach cfam $cfams {
      if { [ is-loaded $cfam ] } {
         set ctag [ GetTagOfModuleLoaded $cfam  ]
         if { $requireIt } { prereq $ctag }
         return $ctag
      }
   }

   # No loaded compiler found, try to default from path to modulefile?
   if { $pathDefault} {
      set moduledir [file dirname $ModulesCurrentModulefile ]
      set cversion [file tail $moduledir]
      set tmppath [file dirname $moduledir]
      set cfamily [file tail $tmppath]

      if { [lsearch $cfams $cfamily] > -1 } {
         # We matched a known compiler family in our path
         set ctag "$cfamily/$cversion"
         if { $loadIt } { RequireCompiler $ctag }
         if { $requireIt } { prereq $ctag }
         return $ctag
      }
   }

   # Still no compiler, default to GetDefaultCompiler>
   if { $useDefault } {
      set ctag [ GetDefaultCompiler ]
      if { $loadIt } { RequireCompiler $ctag }
      if { $requireIt } { prereq $ctag }
      return $ctag
   }

   #Nothing found, and not defaulting
   return $ctag
}
